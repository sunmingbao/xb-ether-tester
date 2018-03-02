/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <ucontext.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <stdlib.h>
#include "common.h"
#include "net.h"
#include "socket.h"
#include "config_file.h"
#include "misc_utils.h"
#include "debug.h"

uint64_t cpu_freq;
static struct timeval intvl = {0};
static int send_all;
static unsigned char src_mac[6], dst_mac[6];
static int  has_src_mac, has_dst_mac;
static int  no_wait, write_tap;
static int be_sending, need_stop;
static int  fd;

typedef struct
{
    uint64_t send_total, send_total_bytes;
    uint64_t send_succ, send_succ_bytes;
    uint64_t send_fail, send_fail_bytes;
} t_pkt_stat;

static t_pkt_stat gt_pkt_stat;
uint64_t send_times_cnt;
static struct timeval first_snd_tv, last_snd_tv;


static int   enable_max_speed;
static int  pkt_file_type = -1;
static char *config_file;
static int   burst_num;
static char  *if_name;
static int   cpu_idx = -1;
static int64_t   snd_interval = 1000000;
static int64_t   frequency = 1;




#define    HELP                (999)
#define    ENABLE_MAX_SPEED    (1000)
#define    CONFIG_FILE          (1001)
#define    BURST_NUM           (1002)
#define    INTERVAL            (1003)
#define    PPS                 (1004)
#define    INTERFACE           (1005)
#define    BIND_CPU            (1006)
#define    SEND_ALL            (1007)
#define    VERSION             (1008)
#define    SINGLE_BIN_PKT      (1009)
#define    CAP_PKT_FILE        (1010)
#define    SET_SRC_MAC         (1011)
#define    SET_DST_MAC         (1012)
#define    NO_WAIT             (1013)
#define    WRITE_TAP           (1014)
struct option my_options[] =
    {
        {"help",              no_argument,       NULL, HELP},
        {"version",           no_argument,       NULL, VERSION},
        {"config-file",       required_argument, NULL, CONFIG_FILE},
        {"bin-pkt-file",      required_argument, NULL, SINGLE_BIN_PKT},
        //{"cap-file",          required_argument, NULL, CAP_PKT_FILE},
        {"interface",         required_argument, NULL, INTERFACE},
        {"bind-cpu",          required_argument, NULL, BIND_CPU},
        {"enable-max-speed",  no_argument,       NULL, ENABLE_MAX_SPEED},
        {"burst-num",         required_argument, NULL, BURST_NUM},
        {"interval",          required_argument, NULL, INTERVAL},
        {"pps",               required_argument, NULL, PPS},
        {"set-src-mac",       required_argument, NULL, SET_SRC_MAC},
        {"set-dst-mac",       required_argument, NULL, SET_DST_MAC},
        {"send-all",          no_argument,       NULL, SEND_ALL},
        {"no-wait",           no_argument,       NULL, NO_WAIT},
        {"write-tap",         no_argument,       NULL, WRITE_TAP},
        {0},
    };


const char *opt_remark[][2] = {
    {"(-h or -H for short)","print help info"}, 
    {"(-v or -V for short)","show version"}, 
    {"(-f or -F for short)","specify config file"}, 
    {"","specify raw binary packet file contains one packet"}, 
    //{"","specify capture file contains packets"}, 
    {"(-i or -I for short)","interface to use for sending packets"}, 
    {"","bind sending thread to a free cpu. based from 0"},
    {"(-m or -M for short)", "send packet with max speed"}, 
    {"", "how many packet to send. If not specified, always sending until press CTRL+C"}, 
    {"", "how long to delay (in us) between two sending"},
    {"", "how many packets to send in one second"},
    {"", "set src mac of each packet"},
    {"", "set dst mac of each packet"},
    {"", "send all packets in config file, including packets not selected"},
    {"", "create tap interface whose name is given in -i option, and write packets to tap interface."},
    {"", "no wait before sending and finish sending"},
};


void print_usage()
{
    int i, opt_num = ARRAY_SIZE(my_options);
    char *arg;
    printf("%s(v%s) \n"
        "for usage detail, visit http://blog.csdn.net/crazycoder8848/article/details/47209427\n"
        "Usage example:\n"
        "  ./%s --config-file=my_cfg.etc -i eth0  --pps=3\n",
                       app_name, version, app_name);




    printf("\n\noptions:\n");
    for (i=0; i<opt_num; i++)
    {
        arg = my_options[i].has_arg?"":"no argument";
        if (my_options[i].name==NULL) break;
        printf("--%s %s :  %s\n      %s\n\n"
            , my_options[i].name
            , opt_remark[i][0]
            , arg
            , opt_remark[i][1]);


    }
}

void report_working_paras()
{


    intvl.tv_sec = snd_interval/1000000;
    intvl.tv_usec = snd_interval%1000000;
    
    printf("\n[working paras]\n");
    
    if (enable_max_speed)
        printf("%sSPEED: MAX_SPEED\n", LINE_HDR);
    else
        printf("%sSPEED: interval=%lus %luus <==> (pps=%"PRIi64")\n"
        , LINE_HDR
        , intvl.tv_sec, intvl.tv_usec, frequency);


    if (burst_num>0)
        printf("%sMODE: burst %d times\n", LINE_HDR, burst_num);
    else
        printf("%sMODE: continue\n", LINE_HDR);

}

int check_args()
{


    if (config_file==NULL)
    {
        printf("you must provide a config file\n");
        goto FAIL_EXIT;
    }

    if (if_name==NULL)
    {
        printf("you must specify interface for sending packets\n");
        goto FAIL_EXIT;
    }
    
    if (enable_max_speed) goto SUCC_EXIT;
    if (snd_interval<=0)
    {
        printf("interval or pps invalid\n");
        goto FAIL_EXIT;
    }


SUCC_EXIT:


    report_working_paras();
    return 0;


FAIL_EXIT:
        print_usage();
        return -1;


}





int parse_and_check_args(int argc, char *argv[])
{
   int opt;
    while ((opt = getopt_long(argc, argv, "hHmMi:I:f:F:", my_options, NULL)) != -1)
    {
       switch (opt)
       {


           case 'f':
           case 'F':
           case CONFIG_FILE: 
               config_file = optarg;
               pkt_file_type = 0;
               break;
               
           case SINGLE_BIN_PKT: 
               config_file = optarg;
               pkt_file_type = 1;
               break;
#if 0
           case CAP_PKT_FILE: 
               config_file = optarg;
               pkt_file_type = 2;
               break;
#endif

           case 'i':
           case 'I':
           case INTERFACE:
               if_name = optarg;
               break;

           case BIND_CPU:
               cpu_idx = strtol(optarg, NULL, 0);
               break;
               
           case INTERVAL:
               snd_interval = strtol(optarg, NULL, 0);
               break;


           case BURST_NUM:
               burst_num = strtol(optarg, NULL, 0);
               break;


           case PPS:
               frequency  = strtol(optarg, NULL, 0);
               if (frequency>0)
                   snd_interval = 1000000/frequency;
               else
                   snd_interval = 0;
               break;

           case SET_SRC_MAC:
               mac_str2n(src_mac, optarg);
               has_src_mac = 1;
               break;

           case SET_DST_MAC:
               mac_str2n(dst_mac, optarg);
               has_dst_mac = 1;
               break;
               
           case SEND_ALL:
               send_all = 1;
               break;

           case NO_WAIT:
               no_wait = 1;
               break;

           case WRITE_TAP:
               write_tap = 1;
               break;
               
           case 'v':
           case 'V':
           case VERSION:
               printf("v%c.%c.%c\n", version[0], version[1], version[2]);
               exit(0);

           case 'h':
           case 'H':
           case HELP:
                print_usage();
                exit(0);

           case 'm':
           case 'M':
           case ENABLE_MAX_SPEED:
               enable_max_speed = 1;
               break;

           default: /* '?' */
               print_usage();
       }
    }


    return check_args();
}



static inline int time_a_smaller_than_b(struct timeval *a, struct timeval *b)
{
    if (a->tv_sec < b->tv_sec) return 1;
    if (a->tv_sec > b->tv_sec) return 0;


    return a->tv_usec < b->tv_usec;
}




struct timeval time_a_between_b2(struct timeval a, struct timeval b)
{
    struct timeval ret;
    if (b.tv_usec<a.tv_usec)
    {
        b.tv_usec+=1000000;
        b.tv_sec-=1;
    }
    ret.tv_sec = b.tv_sec-a.tv_sec;
    ret.tv_usec = b.tv_usec-a.tv_usec;
    return ret;
}


void report_snd_summary()
{
    struct timeval  tmp = time_a_between_b2(first_snd_tv, last_snd_tv);
    uint64_t pps = gt_pkt_stat.send_succ*1000000/(tmp.tv_sec*1000000 + tmp.tv_usec);
    uint64_t bps = gt_pkt_stat.send_succ_bytes*1000000/(tmp.tv_sec*1000000 + tmp.tv_usec);;
    
    printf("\n[packet send summary]\n"
        "%stime: %lu sec %lu us\n"
        "%ssend total : %"PRIu64" packets %"PRIu64" bytes\n"
        "%ssend succ  : %"PRIu64" packets %"PRIu64" bytes\n"
        "%ssucc performence : pps %"PRIu64"; bps %"PRIu64"\n\n"
        ,LINE_HDR, tmp.tv_sec, tmp.tv_usec
        ,LINE_HDR, gt_pkt_stat.send_total, gt_pkt_stat.send_total_bytes
        ,LINE_HDR, gt_pkt_stat.send_succ, gt_pkt_stat.send_succ_bytes
        ,LINE_HDR, pps, bps);


    if (gt_pkt_stat.send_fail)
        printf("[some packets sent fail]\n"
        "%sfail traffic : %"PRIu64" packets %"PRIu64" bytes\n"
            ,LINE_HDR, gt_pkt_stat.send_fail, gt_pkt_stat.send_fail_bytes);
    else
        printf("[no packets sent fail]\n");


}

void init_bits(void *field_addr, t_rule *pt_rule)
{
    unsigned whole_value1, whole_value2;
    unsigned shift_len=pt_rule->width*8 - pt_rule->bits_from - pt_rule->bits_len;

    if (1==pt_rule->width)
    {
        whole_value1 = *(unsigned char *)field_addr;
        whole_value2 = *(unsigned char *)(pt_rule->base_value);
    }
    else
    {
        whole_value1 = ntohs(*(unsigned short *)field_addr);
        whole_value2 = ntohs(*(unsigned short *)(pt_rule->base_value));
    }
    
        whole_value1 &= ~(((1<<pt_rule->bits_len)-1)<<shift_len);
        whole_value1 |= whole_value2;
        
    if (1==pt_rule->width)
        *(unsigned char *)field_addr = whole_value1;
    else
        *(unsigned short *)field_addr = htons(whole_value1);

}

void init_u8_n(void *data, t_rule *pt_rule)
{
    *(uint8_t *)data=pt_rule->base_value[0];
}

void init_u16_n(void *data, t_rule *pt_rule)
{
    *(uint16_t *)data=*(uint16_t *)(pt_rule->base_value);
}

void init_u32_n(void *data, t_rule *pt_rule)
{
    *(uint32_t *)data=*(uint32_t *)(pt_rule->base_value);
}

void init_rule_field(t_stream *pt_stream, t_rule *pt_rule)
{
    if (pt_rule->bits_len)
    {
        init_bits(pt_stream->data + pt_rule->offset, pt_rule);
        return;
    }
    
    switch (pt_rule->width)
    {
        case 1:
            init_u8_n(pt_stream->data + pt_rule->offset, pt_rule);
            return;

        case 2:
            init_u16_n(pt_stream->data + pt_rule->offset, pt_rule);
            return;

        case 4:
            init_u32_n(pt_stream->data + pt_rule->offset, pt_rule);
            return;

    }
}

void update_u8_n(void *data, t_rule *pt_rule)
{
    (*(uint8_t *)data)+=pt_rule->step_size;
    if ((*(uint8_t *)data) > *(uint8_t *)(pt_rule->max_value))
        init_u8_n(data, pt_rule);
}

void update_u16_n(void *data, t_rule *pt_rule)
{
    uint16_t host_data=ntohs(*(uint16_t *)data);
    uint16_t max_data=ntohs(*(uint16_t *)(pt_rule->max_value));
    host_data+=pt_rule->step_size;
    *(uint16_t *)data=htons(host_data);
    if (host_data > max_data)
        init_u16_n(data, pt_rule);

}

void update_u32_n(void *data, t_rule *pt_rule)
{
    uint32_t host_data=ntohl(*(uint32_t *)data);
    uint32_t max_data=ntohl(*(uint32_t *)(pt_rule->max_value));
    host_data+=pt_rule->step_size;
    *(uint32_t *)data=htonl(host_data);
    if (host_data > max_data)
        init_u32_n(data, pt_rule);

}

void check_sum_proc(t_stream *pt_stream)
{
    t_ip_hdr *iph=eth_data(pt_stream->data);
    int type = eth_type(pt_stream->data);
    if (type!=ETH_P_IP) return;
    if ((pt_stream->flags & CHECK_SUM_IP))
    {
        ip_update_check(iph);
    }

    if ((iph->protocol == IPPROTO_ICMP) &&
        (pt_stream->flags & CHECK_SUM_ICMP))
    {
        icmp_igmp_update_check(iph);
        return;
    }

    if ((iph->protocol == IPPROTO_IGMP) &&
        (pt_stream->flags & CHECK_SUM_IGMP))
    {
        icmp_igmp_update_check(iph);
        return;
    }

    if ((iph->protocol == IPPROTO_UDP) &&
        (pt_stream->flags & CHECK_SUM_UDP))
    {
        udp_update_check(iph);
        return;
    }

    if ((iph->protocol == IPPROTO_TCP) &&
        (pt_stream->flags & CHECK_SUM_TCP))
    {
        tcp_update_check(iph);
        return;
    }

}

void update_bits(void *field_addr, t_rule *pt_rule)
{
    unsigned whole_value1, whole_value2, filed_value1, filed_value2;
    unsigned shift_len=pt_rule->width*8 - pt_rule->bits_from - pt_rule->bits_len;

    if (1==pt_rule->width)
    {
        whole_value1 = *(unsigned char *)field_addr;
        whole_value2 = *(unsigned char *)(pt_rule->max_value);
    }
    else
    {
        whole_value1 = ntohs(*(unsigned short *)field_addr);
        whole_value2 = ntohs(*(unsigned short *)(pt_rule->max_value));
    }

    filed_value1 = (whole_value1>>shift_len);
    filed_value1 &=((1<<pt_rule->bits_len)-1);

    filed_value1 += pt_rule->step_size;

    filed_value2 = (whole_value2>>shift_len);
    filed_value2 &=((1<<pt_rule->bits_len)-1);


    if (filed_value1>filed_value2)
    {
        init_bits(field_addr, pt_rule);
        return;
    }
    
    whole_value1 &= ~(((1<<pt_rule->bits_len)-1)<<shift_len);
    whole_value1 |= (filed_value1<<shift_len);

    if (1==pt_rule->width)
        *(unsigned char *)field_addr = whole_value1;
    else
        *(unsigned short *)field_addr = htons(whole_value1);

}

void update_rule_field(t_stream *pt_stream, t_rule *pt_rule)
{
    if (pt_rule->bits_len)
    {
        update_bits(pt_stream->data + pt_rule->offset, pt_rule);
        return;
    }

    switch (pt_rule->width)
    {
        case 1:
            update_u8_n(pt_stream->data + pt_rule->offset, pt_rule);
            return;

        case 2:
            update_u16_n(pt_stream->data + pt_rule->offset, pt_rule);
            return;

        case 4:
            update_u32_n(pt_stream->data + pt_rule->offset, pt_rule);
            return;

    }
}

void rule_fileds_init(t_stream *pt_stream)
{
    int i;
    t_rule *pt_rule;
    for (i=0; i<pt_stream->rule_num; i++)
    {
        pt_rule = &(pt_stream->at_rules[pt_stream->rule_idx[i]]);
        init_rule_field(pt_stream, pt_rule);
        check_sum_proc(pt_stream);
    }
}

void rule_fileds_update(t_stream *pt_stream)
{
    int i;
    t_rule *pt_rule;
    for (i=0; i<pt_stream->rule_num; i++)
    {
        pt_rule = &(pt_stream->at_rules[pt_stream->rule_idx[i]]);
        update_rule_field(pt_stream, pt_rule);

        check_sum_proc(pt_stream);

    }
}

void rule_fileds_init_all_pkt()
{
    int i;
    t_stream *pt_stream;
    for (i=0; i<nr_cur_stream; i++)
    {
        pt_stream = g_apt_streams[i];
        if (pt_stream->rule_num) rule_fileds_init(pt_stream);
    }
}


int send_pkt(void *arg)
{
    int i=0;
    struct timeval cur_tv, next_snd_tv;

    int ret;

    if (cpu_idx>=0)
    {
        ret = set_thread_cpu_affinity(pthread_self(), cpu_idx, cpu_idx);
        if (ret)
            ERR_DBG_PRINT("bind thread to cpu %d failed", cpu_idx);
    }

    //cpu_freq = get_cpu_freq();
    
    gettimeofday(&cur_tv, NULL);
    first_snd_tv = cur_tv;
    next_snd_tv = cur_tv;
    goto SND_PKT;

/* Send down the packet */
while (!need_stop)
{
   if (enable_max_speed)
    {
        //gettimeofday(&cur_tv, NULL);
         goto SND_PKT;
    }
        
    do
    {   
       gettimeofday(&cur_tv, NULL);
            //nano_sleep(0, 330000000);
    } while (!need_stop && time_a_smaller_than_b(&cur_tv, &next_snd_tv));


        if (need_stop) goto exit;

        last_snd_tv = cur_tv;
        
            SND_PKT:
            while (g_apt_streams[i]->selected==0 && !send_all)
            {
                i = (i+1)%nr_cur_stream;
                if (need_stop) goto exit;
            }

                gt_pkt_stat.send_total++;
                gt_pkt_stat.send_total_bytes+=g_apt_streams[i]->len;
                
           if (write(fd, g_apt_streams[i]->data, g_apt_streams[i]->len) != g_apt_streams[i]->len)
           {
                gt_pkt_stat.send_fail++;
                gt_pkt_stat.send_fail_bytes+=g_apt_streams[i]->len;
           }
            else
            {
                gt_pkt_stat.send_succ++;
                gt_pkt_stat.send_succ_bytes+=g_apt_streams[i]->len;
            }

            if (g_apt_streams[i]->rule_num) rule_fileds_update(g_apt_streams[i]);
            



        send_times_cnt++;
        i = (i+1)%nr_cur_stream;
        if (burst_num>0 && send_times_cnt>=burst_num)
        {
            goto exit;
        }


        if (enable_max_speed)  continue;


        next_snd_tv.tv_sec+=intvl.tv_sec;
        next_snd_tv.tv_usec+=intvl.tv_usec;
        next_snd_tv.tv_sec+=(next_snd_tv.tv_usec/1000000);
        next_snd_tv.tv_usec%=1000000;
    }
    
exit:
    if (enable_max_speed)
          gettimeofday(&last_snd_tv, NULL);
    report_snd_summary();
    be_sending = 0;
return 0;
}


void create_thread(pthread_t *thread, void *fn, void *arg)
{
    pthread_create(thread, NULL, fn, arg);
}


void register_sig_act(int sig, void *sig_proc_func)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = sig_proc_func;
    action.sa_flags = SA_SIGINFO;
    sigaction(sig, &action, NULL);
}


static void  my_sig_handler(int sig_no, siginfo_t *pt_siginfo, void *p_ucontext)
{
    need_stop = 1;
}


int load_packets()
{
    if (0==pkt_file_type)
        return load_config_file(config_file
        ,has_src_mac?src_mac:NULL
        ,has_dst_mac?dst_mac:NULL);

    if (1==pkt_file_type)
        return load_bin_packet_file(config_file
        ,has_src_mac?src_mac:NULL
        ,has_dst_mac?dst_mac:NULL);

    return 0;
}

int start_up_tap_if(const char *tap)
{
    char cmd_str[128];


    sprintf(cmd_str, "ifconfig %s up", tap);
    if (0!=system(cmd_str))
    {
        ERR_DBG_PRINT("exec cmd %s failed", cmd_str);
        return -1;
    }


    return 0;
}

int prepare_tap_if(char *tap_name)
{
    struct ifreq ifr;
    int fd, ret;
    unsigned int features;
    
    fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0) {
        ERR_DBG_PRINT("could not open %s", "/dev/net/tun");
        return -1;
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;


    if (ioctl(fd, TUNGETFEATURES, &features) == -1) {
        ERR_DBG_PRINT("warning: TUNGETFEATURES failed:");
        features = 0;
    }


//    if (features & IFF_ONE_QUEUE) {
//        ifr.ifr_flags |= IFF_ONE_QUEUE;
//    }




    strcpy(ifr.ifr_name, tap_name);
    
    ret = ioctl(fd, TUNSETIFF, (void *) &ifr);
    if (ret != 0) 
    {


        ERR_DBG_PRINT("could not configure tap interface %s", tap_name);
        close(fd);
        return -1;
    }
    
    strcpy(tap_name, ifr.ifr_name);
    start_up_tap_if(tap_name);


    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}

int has_sendable_pkt(void)
{
    int i;
    t_stream *pt_stream;

    if (send_all) goto HAS_SENDABLE_PKT;

    for (i=0; i<nr_cur_stream; i++)
    {
        pt_stream = g_apt_streams[i];
        if (pt_stream->selected)
            goto HAS_SENDABLE_PKT;
    }

    printf("\n\n\n"
		"**** Error: No sendable packet found. It can be solved by one of the following way. ****\n"
        "        use --send-all option.\n"
        "        select at least 1 packet in file %s under windows version of xb_ether_tester ^_^\n"
        ,config_file);
    return 0;

HAS_SENDABLE_PKT:
    return 1;
}

int main(int argc, char *argv[])
{
    int ret;
    pthread_t  the_thread;
    int dot_str_idx = 0;
    const char *dots[] = {"-", "\\", "|", "/"};


    if (parse_and_check_args(argc, argv))
    {
        return 0;
    }

    if (write_tap)
        fd  = prepare_tap_if(if_name);
    else
        fd  = create_l2_raw_socket(if_name);
    
    if (fd<0) return 0;
    
    ret=load_packets();
    if (ret<0) return 0;

	if (!has_sendable_pkt()) return 0;

    if (!no_wait)
    {
        printf("\n====press s begin sending packet====\n");
        while (getchar()!='s');
    }


    need_stop = 0;
    be_sending = 1;
    rule_fileds_init_all_pkt();

    create_thread(&the_thread, send_pkt, NULL);
    register_sig_act(SIGINT, my_sig_handler);


    printf("\n\n\n");
    while (be_sending)
    {
        printf("\r                  ");
        printf("\rsending packet  %s  [%"PRIu64" packets, %"PRIu64" bytes] "
            , dots[dot_str_idx]
            , gt_pkt_stat.send_succ, gt_pkt_stat.send_succ_bytes);
        fflush(stdout);
        nano_sleep(0, 330000000);
        dot_str_idx++;
        dot_str_idx = (dot_str_idx %4);
    }

    if (!no_wait)
    {
        printf("\n====press q to exit====\n");
        while (getchar()!='q');
    }


    return 0;
}

//./target/xb_ether_tester.exe  -i eth0 -f /home/sunmingbao/aaa.etc  -M

