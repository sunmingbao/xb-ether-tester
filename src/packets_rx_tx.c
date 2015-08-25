/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */
#include <pcap.h>
#include <sys/time.h>
#include "common.h"
#include "global_info.h"
#include "res.h"
#include "net.h"
#include "debug.h"

int need_stop=1;
int need_cap_stop=1;
int snd_stopped=1, rcv_stopped=1;
int snd_started, rcv_started;
uint64_t send_times_cnt;

t_pkt_stat gt_pkt_stat, gt_pkt_stat_pre;
struct timeval last_timer_tv;
struct timeval time_elapsed;
struct timeval last_stat_tv_snd, last_stat_tv_rcv, last_stat_tv_sndf;
    
int need_capture;


t_fc_cfg  gt_fc_cfg;

int snd_gap_s=0;
int snd_gap_us=1000;


t_pkt_cap_cfg  gt_pkt_cap_cfg;

char pkt_cap_filter_str[MAX_CAP_FILTER_STR_LEN];

unsigned char sample_pkt[SAMPLE_PKT_LEN]={
0x00, 0x23, 0xcd, 0x76, 0x63, 0x1a, 0x00 ,0x21 ,0x85 ,0xc5 ,0x2b ,0x8f ,0x08 ,0x00 ,0x45 ,0x00,
0x00, 0x3c, 0x79, 0x19, 0x00, 0x00, 0x40 ,0x01 ,0x7f ,0xf2 ,0xc0 ,0xa8 ,0x01 ,0x64 ,0xc0 ,0xa8,
0x01, 0x01, 0x08, 0x00, 0x44, 0x5c, 0x04 ,0x00 ,0x05 ,0x00 ,0x61 ,0x62 ,0x63 ,0x64 ,0x65 ,0x66,
0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d ,0x6e ,0x6f ,0x70 ,0x71 ,0x72 ,0x73 ,0x74 ,0x75 ,0x76,
0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66 ,0x67 ,0x68 ,0x69  
		
	};

pcap_if_t *alldevs;
pcap_if_t *cur_dev;
char *cur_dev_name;
void get_all_devs()
{
    char errbuf[PCAP_ERRBUF_SIZE];
    
    /* Retrieve the device list from the local machine */
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        WinPrintf(hwnd_frame, "Error in pcap_findalldevs_ex: %s。程序即将退出\n", errbuf);
        exit(1);
    }
}

void fc_and_pkt_cap_init()
{
    gt_pkt_cap_cfg.need_save_capture=1;
    gt_pkt_cap_cfg.pkt_cap_cfg_mode= PKT_CAP_CFG_MODE_NORMAL;
    gt_pkt_cap_cfg.filter_str_usr[0]=0;
    gt_pkt_cap_cfg.pkt_cap_pkt_type=PKT_CAP_PKT_TYPE_ALL;
    
    gt_pkt_cap_cfg.pkt_cap_sip=0;
    gt_pkt_cap_cfg.pkt_cap_dip=0;

    gt_pkt_cap_cfg.pkt_cap_protocol=-1;
    gt_pkt_cap_cfg.pkt_cap_sport=-1;
    gt_pkt_cap_cfg.pkt_cap_dport=-1;

    gt_fc_cfg.speed_type = SPEED_TYPE_HIGH;
    gt_fc_cfg.speed_value= 1000;
    gt_fc_cfg.snd_mode= SND_MODE_CONTINUE;
    gt_fc_cfg.snd_times_cnt = -1;

}

void rx_tx_init()
{
    get_all_devs();
}

#define IPTOSBUFFERS	12
char *iptos(u_long in)
{
	static char output[IPTOSBUFFERS][3*4+3+1];
	static short which;
	u_char *p;

	p = (u_char *)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}

void init_net_card_combbox(HWND hwnd_comb)
{
    pcap_if_t *d;
    char info[256];
    pcap_addr_t *a;
    int nic_cnt=0, last_nic_idx = 0;
    for(d= alldevs; d != NULL; d= d->next)
    {
       info[0]=0;
       sprintf(info, "%-15s %s"
                ,"no ipv4 addr"
                ,get_nic_FriendlyName(strchr(d->name, '{')));
                     
       for(a=d->addresses;a;a=a->next) 
       {
            if (a->addr->sa_family==AF_INET)
            {
              sprintf(info, "%-15s %s"
                ,ip_n2str(NULL, &(((struct sockaddr_in *)a->addr)->sin_addr.s_addr))
                ,get_nic_FriendlyName(strchr(d->name, '{')));
              break;
            }
        }

        SendMessage(hwnd_comb,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)info);
        if (strcmp(d->name, last_nic_name)==0) last_nic_idx = nic_cnt;
        nic_cnt++;
    }

    SendMessage(hwnd_comb, CB_SETCURSEL, (WPARAM)last_nic_idx, (LPARAM)0);
    select_if(SendMessage(hwnd_net_card_comb, (UINT) CB_GETCURSEL, 
                (WPARAM) 0, (LPARAM) 0));

}

int32_t        nr_cur_stream;
t_stream    *g_apt_streams[MAX_STREAM_NUM];
int        copy_idx=-1;

int32_t        nr_stream2snd;
t_stream    *g_apt_streams2snd;

void del_all_streams()
{
    int i;
    ListView_DeleteAllItems(hwnd_lv);
    for (i=0; i<nr_cur_stream; i++)
    {
        free(g_apt_streams[i]);
    }
    nr_cur_stream=0;
    
}

int set_filter(pcap_t *adhandle, char *packet_filter)
{
u_int netmask;
struct bpf_program fcode;

if (0==packet_filter[0]) return 0;

    if(cur_dev->addresses != NULL)
        /* Retrieve the mask of the first address of the interface */
        netmask=((struct sockaddr_in *)(cur_dev->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        /* If the interface is without addresses we suppose to be in a C class network */
        netmask=0xffffff; 

    //compile the filter
    if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
    {
        //err_msg_box("Unable to compile the packet filter: %s. Check the syntax.", packet_filter);
        /* Free the device list */
        //pcap_freealldevs(alldevs);
        return -1;
    }
    
    //set the filter
    if (pcap_setfilter(adhandle, &fcode)<0)
    {
        err_msg_box("Error setting the filter: %s", packet_filter);
        /* Free the device list */
        //pcap_freealldevs(alldevs);
        return -1;
    }
    
    return 0;
}

int is_filter_valid(char *packet_filter)
{
    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];
    int ret;

	if ((fp = pcap_open_live(cur_dev_name,	// name of the device
							 65536,			// portion of the packet to capture. It doesn't matter in this case 
							 1,				// promiscuous mode (nonzero means promiscuous)
							 10,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		err_msg_box(TEXT("Unable to open the adapter. %s is not supported by WinPcap"), cur_dev_name);
		return 2;
	}

    ret = (0==set_filter(fp, packet_filter));
    pcap_close(fp);	
    return ret;

}

uint64_t cap_save_cnt, cap_save_bytes_cnt;
int rcv_pkt(char *dev_name, int cnt)
{
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *header;
const u_char *pkt_data;
pcap_dumper_t *dumpfile=NULL;
    
	/* Open the adapter */
	if ((fp = pcap_open_live(dev_name,		// name of the device
							 65536,			// portion of the packet to capture. It doesn't matter in this case 
							 1,				// promiscuous mode (nonzero means promiscuous)
							 10,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		WinPrintf(hwnd_frame, TEXT("Unable to open the adapter. %s is not supported by WinPcap"), dev_name);
        PostMessage(hwnd_frame, WM_COMMAND, IDT_TOOLBAR_STOP, 0);
		goto exit;
	}

if (pkt_cap_filter_str[0]) set_filter(fp, pkt_cap_filter_str);

dumpfile = pcap_dump_open(fp, PKT_CAP_FILE_WHILE_SND);

	/* Send down the packet */
	while (!need_stop)
	{
	    if (!need_capture)
        {
            Sleep(100);
            continue;
        }
        if (1==pcap_next_ex( fp, &header, &pkt_data))
        {
            //if (pkt_data[5]!=g_at_streams[0].eth_packet.src[5]) continue;
            gt_pkt_stat.rcv_total++;
            gt_pkt_stat.rcv_total_bytes+=header->caplen;
            if (gt_pkt_cap_cfg.need_save_capture /* && saved_cnt<save_capture_num */) 
            {
                cap_save_cnt++;
                cap_save_bytes_cnt+=header->caplen;
                //fwrite(pkt_data, 1, header->len, file);
                pcap_dump((void *)dumpfile, header, pkt_data);
            }
        }

    	continue;
    }

exit:
    if (dumpfile) pcap_dump_close(dumpfile);
	if (fp) pcap_close(fp);	
	return 0;
}

int send_pkt(char *dev_name, int cnt)
{
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	int i=nr_stream2snd-1;
	struct timeval cur_tv, next_snd_tv={(time_t)0};
    
	/* Open the adapter */
	if ((fp = pcap_open_live(dev_name,		// name of the device
							 65536,			// portion of the packet to capture. It doesn't matter in this case 
							 1,				// promiscuous mode (nonzero means promiscuous)
							 1,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		WinPrintf(hwnd_frame, TEXT("Unable to open the adapter. %s is not supported by WinPcap"), dev_name);
        PostMessage(hwnd_frame, WM_COMMAND, IDT_TOOLBAR_STOP, 0);
		goto exit;
	}

    gettimeofday(&next_snd_tv, NULL);

	/* Send down the packet */
	while (!need_stop)
	{
	    if (gt_fc_cfg.speed_type==SPEED_TYPE_FASTEST)  goto SND_PKT;
        
	    do
        {   
    	    gettimeofday(&cur_tv, NULL);
        } while (!need_stop && time_a_smaller_than_b(&cur_tv, &next_snd_tv));

        if (need_stop) break;


        SND_PKT:
            i = (i+1)%nr_stream2snd;

            gt_pkt_stat.send_total++;
            gt_pkt_stat.send_total_bytes+=g_apt_streams2snd[i].len;
    	    if (pcap_sendpacket(fp,	g_apt_streams2snd[i].data,	g_apt_streams2snd[i].len) != 0)
    	    {
            	gt_pkt_stat.send_fail++;
                gt_pkt_stat.send_fail_bytes+=g_apt_streams2snd[i].len;
            }
            
            if (g_apt_streams2snd[i].rule_num) rule_fileds_update(&(g_apt_streams2snd[i]));

            send_times_cnt++;
            if (SND_MODE_BURST==gt_fc_cfg.snd_mode && send_times_cnt>=gt_fc_cfg.snd_times_cnt)
            {
                PostMessage(hwnd_frame, WM_COMMAND, IDT_TOOLBAR_STOP, 0);
                goto exit;
            }


        if (gt_fc_cfg.speed_type==SPEED_TYPE_FASTEST)  continue;

        next_snd_tv.tv_sec+=snd_gap_s;
        next_snd_tv.tv_usec+=snd_gap_us;
        next_snd_tv.tv_sec+=(next_snd_tv.tv_usec/1000000);
        next_snd_tv.tv_usec%=1000000;
    }
    
exit:
	pcap_close(fp);	
	return 0;
}

int cap_stopped=1;
DWORD WINAPI  rcv_pkt_2(LPVOID lpParameter)
{
    int ret = 0;
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *header;
const u_char *pkt_data;
pcap_dumper_t *dumpfile=NULL;
    
	/* Open the adapter */
	if ((fp = pcap_open_live(cur_dev_name,		// name of the device
							 65536,			// portion of the packet to capture. It doesn't matter in this case 
							 1,				// promiscuous mode (nonzero means promiscuous)
							 10,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		WinPrintf(hPktCapDlg, TEXT("Unable to open the adapter. %s is not supported by WinPcap"), cur_dev_name);
        PostMessage(hPktCapDlg, WM_COMMAND, ID_PKT_CAP_CAP, 0);
		goto exit;
	}

if (pkt_cap_filter_str[0]) set_filter(fp, pkt_cap_filter_str);

dumpfile = pcap_dump_open(fp, PKT_CAP_FILE_ONLY_CAP);

	/* Send down the packet */
	while (!need_cap_stop)
	{
        ret = pcap_next_ex( fp, &header, &pkt_data);
        if (1==ret)
        {
            update_pkt_cap_stats((void *)pkt_data);
            if (gt_pkt_cap_cfg.need_save_capture /* && saved_cnt<save_capture_num */) 
            {
                pcap_dump((void *)dumpfile, header, pkt_data);
            }
        }

    }

exit:
    if (dumpfile) pcap_dump_close(dumpfile);
	if (fp) pcap_close(fp);	
    cap_stopped=1;
	return 0;
}

int select_if(int idx)
{
    pcap_if_t *d;
    int i=0;
    
    /* Print the list */
    for(d= alldevs; d != NULL; d= d->next)
    {
        if (i==idx)
        {
            cur_dev = d;
            cur_dev_name = d->name;
            strcpy(last_nic_name, d->name);
            WritePrivateProfileString("last_nic", "name", last_nic_name, APP_PROFILE_FILE);
            return 0;
        }
        
        i++;
    }

    return 1;
}

void cp_stream2sndbuf()
{
    int i, cnt=0;
    t_stream *pt_stream;

    g_apt_streams2snd = malloc(nr_stream2snd*sizeof(t_stream));

    for (i=0; i<nr_cur_stream; i++)
    {
        pt_stream = g_apt_streams[i];
        if (pt_stream->selected)
        {
            g_apt_streams2snd[cnt] = *pt_stream;
            cnt++;
        }
    }
}

DWORD WINAPI  wpcap_snd_test(LPVOID lpParameter)
{
    snd_started=1;
    send_times_cnt=0;
    while (!rcv_started) Sleep(1);
    send_pkt(cur_dev_name, 3);
    free(g_apt_streams2snd);

    snd_stopped=1;

    return 0;
}

DWORD WINAPI  wpcap_rcv_test(LPVOID lpParameter)
{
    rcv_started=1;
    rcv_pkt(cur_dev_name, 3);

    rcv_stopped=1;

    return 0;
}

int stream2dump(char *file_name)
{
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr header;
const u_char *pkt_data;
int i=0;
pcap_dumper_t *dumpfile;

gettimeofday(&(header.ts), NULL);


    
	/* Open the adapter */
	if ((fp = pcap_open_live(cur_dev_name,		// name of the device
							 65536,			// portion of the packet to capture. It doesn't matter in this case 
							 1,				// promiscuous mode (nonzero means promiscuous)
							 10,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		WinPrintf(hwnd_frame, TEXT("Unable to open the adapter. %s is not supported by WinPcap"), cur_dev_name);
		return 2;
	}


dumpfile = pcap_dump_open(fp, file_name);

    for (i=0; i<nr_cur_stream; i++)
    {
        if (!(g_apt_streams[i]->selected)) continue;
        
        pkt_data=g_apt_streams[i]->data;
        header.caplen=g_apt_streams[i]->len;
        header.len=g_apt_streams[i]->len;
        pcap_dump((void *)dumpfile, &header, pkt_data);
    }

    pcap_dump_close(dumpfile);
	pcap_close(fp);	
	return 0;
}

int stream_2_bin(char *file_path)
{
    FILE *file=fopen(file_path, "wb");
    int i;
    

     t_stream *pt_stream = g_apt_streams[GetIndex(hwnd_lv)];

     fwrite(pt_stream->data, pt_stream->len, 1, file);

    fclose(file);
    return 0;
}

int stream_2_text(char *file_path)
{
    FILE *file=fopen(file_path, "wb");
    char file_data_d[512];
    unsigned char *cur_pos;
    int left;
    int i,j;
    

     t_stream *pt_stream = g_apt_streams[GetIndex(hwnd_lv)];
    {
        left = pt_stream->len;
        cur_pos = pt_stream->data;
        while (left>=16)
       {
          for (j=0;j<16;j++)
             sprintf(file_data_d + j*6, "0x%02x, ", (int)cur_pos[j]);
        fwrite(file_data_d, 16*6, 1, file);
        fwrite("\n", 1, 1, file);
        left-=16;
        cur_pos+=16;
       }
       if (left>0)
       {
          for (j=0;j<left;j++)
             sprintf(file_data_d + j*6, "0x%02x, ", (int)cur_pos[j]);
        fwrite(file_data_d, left*6, 1, file);

       }

    }

    fclose(file);
    return 0;
}

int save_stream(char *file_path)
{
    FILE *file=fopen(file_path, "wb");
    int i;
    
    fwrite(version, sizeof(version), 1, file);

    fwrite(&gt_fc_cfg, sizeof(gt_fc_cfg), 1, file);

    gt_pkt_cap_cfg.filter_str_len = strlen(gt_pkt_cap_cfg.filter_str_usr);
    fwrite(&gt_pkt_cap_cfg, PKT_CAP_CFG_FIX_LEN+gt_pkt_cap_cfg.filter_str_len, 1, file);
    
    fwrite(&nr_cur_stream, sizeof(nr_cur_stream), 1, file);
    for(i=0;i<nr_cur_stream;i++)
    {
        fwrite(g_apt_streams[i], STREAM_HDR_LEN+g_apt_streams[i]->len, 1, file);
    }

    fclose(file);
    doc_modified=0;
    return 0;
}

void update_fc_gap()
{
    if (SPEED_TYPE_HIGH==gt_fc_cfg.speed_type)
    {
        if (gt_fc_cfg.speed_value==1)
        {
            snd_gap_s = 0;
            snd_gap_us = 1000000;
        }
        else
        {
            snd_gap_s = 0;
            snd_gap_us = 1000000/gt_fc_cfg.speed_value;
        }
            
    
    }
    else
    {
        snd_gap_s = gt_fc_cfg.speed_value/1000000;
        snd_gap_us = gt_fc_cfg.speed_value%1000000;
    
    }

}

int load_stream(char *file_path)
{
    FILE *file=fopen(file_path, "rb");
    int i, ret = 0;
    char version_tmp[4];

    fread(version_tmp, sizeof(version), 1, file);
    fread(&gt_fc_cfg, sizeof(gt_fc_cfg), 1, file);

    if (version_tmp[0] != version[0])
    {
        err_msg_box(TEXT("配置文件版本不匹配"));
        ret = -1;
        goto EXIT;

    }

    del_all_streams();

    fread(&gt_pkt_cap_cfg, PKT_CAP_CFG_FIX_LEN, 1, file);
    fread(gt_pkt_cap_cfg.filter_str_usr, gt_pkt_cap_cfg.filter_str_len, 1, file);
    gt_pkt_cap_cfg.filter_str_usr[gt_pkt_cap_cfg.filter_str_len]=0;
    build_filter(pkt_cap_filter_str);
    fread(&nr_cur_stream, sizeof(nr_cur_stream), 1, file);
    nr_cur_stream=(nr_cur_stream>MAX_STREAM_NUM?MAX_STREAM_NUM:nr_cur_stream);

    for(i=0;i<nr_cur_stream;i++)
    {
        g_apt_streams[i] = alloc_stream();
        fread(g_apt_streams[i], 1, STREAM_HDR_LEN, file);
        fread(g_apt_streams[i]->data, 1, g_apt_streams[i]->len, file);
        g_apt_streams[i]->err_flags = build_err_flags((void *)(g_apt_streams[i]->data), g_apt_streams[i]->len);
    }


    doc_modified=0;

EXIT:
    fclose(file);
    return ret;
}

