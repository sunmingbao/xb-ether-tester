/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef  __CONFIG_FILE_H__
#define  __CONFIG_FILE_H__

#include <stdint.h>
#include "net.h"

typedef struct
{
    int speed_type;
    int speed_value;
    int snd_mode;
    int snd_times_cnt;
    char rsv[32];
} __attribute__((packed)) t_fc_cfg;



#define    MAX_CAP_FILTER_STR_LEN    256
typedef struct
{
    int need_save_capture;
    int pkt_cap_cfg_mode;
    int pkt_cap_pkt_type;
    int pkt_cap_sip, pkt_cap_dip, pkt_cap_protocol;
    int pkt_cap_sport, pkt_cap_dport;
    int filter_str_len;
    char rsv[32];
    char filter_str_usr[MAX_CAP_FILTER_STR_LEN];
} __attribute__((packed)) t_pkt_cap_cfg;

#define    PKT_CAP_CFG_FIX_LEN    ((unsigned long)(void *)(&(((t_pkt_cap_cfg *)NULL)->filter_str_usr)))

typedef struct
{
    char     valid;
    uint32_t flags;
    uint16_t offset;
    uint8_t  width;
    char     bits_from;
    char     bits_len;
    char     rsv[4];
    uint8_t base_value[8];
    uint8_t max_value[8];
    uint32_t step_size;
} __attribute__((packed)) t_rule;

#define    MAX_FIELD_RULE_NUM    (10)
typedef struct
{
    int  selected;
    char snd_cnt;
    char rsv[7];
    char name[64];
    uint32_t flags;
    unsigned char    rule_num;
    unsigned char    rule_idx[MAX_FIELD_RULE_NUM];
    t_rule  at_rules[MAX_FIELD_RULE_NUM];
    int len;
    union
    {
        t_ether_packet eth_packet;
        unsigned char data[MAX_PACKET_LEN];
    };

    //non save info
    uint32_t err_flags;

} __attribute__((packed)) t_stream;

/* t_stream.flags */
#define    CHECK_SUM_IP      0x1
#define    CHECK_SUM_ICMP    0x2
#define    CHECK_SUM_IGMP    0x4
#define    CHECK_SUM_UDP     0x8
#define    CHECK_SUM_TCP     0x10
#define    CHECK_SUM_ALL  \
    (CHECK_SUM_IP|CHECK_SUM_ICMP|CHECK_SUM_IGMP|CHECK_SUM_UDP|CHECK_SUM_TCP)

#define    IP_LEN    0x20
#define    UDP_LEN   0x40
#define    LEN_ALL  (IP_LEN|UDP_LEN)


#define    ERR_IP_CHECKSUM     (0x1<<30)
#define    ERR_UDP_CHECKSUM    (0x1<<29)
#define    ERR_TCP_CHECKSUM    (0x1<<28)
#define    ERR_PKT_LEN         (0x1<<27)
#define    ERR_ICMP_CHECKSUM    (0x1<<26)
#define    ERR_IGMP_CHECKSUM    (0x1<<25)

#define    STREAM_HDR_LEN    ((unsigned long)(void *)(&(((t_stream *)NULL)->data)))

#define    MAX_STREAM_NUM    100


extern t_fc_cfg  gt_fc_cfg;
extern t_pkt_cap_cfg  gt_pkt_cap_cfg;

extern t_stream    *g_apt_streams[MAX_STREAM_NUM];
extern int        nr_cur_stream;

extern const char *app_name;
extern const char version[4];
int load_config_file(char *file_path, unsigned char *src_mac, unsigned char *dst_mac);
int load_bin_packet_file(char *file_path, unsigned char *src_mac, unsigned char *dst_mac);
#endif

