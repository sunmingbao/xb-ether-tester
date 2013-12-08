/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */
#include <windows.h>
#include "common.h"
#include "global_info.h"
#include "res.h"

#define  STREAM_EDIT_DATA_CHANGE     (WM_USER + 1)
void stream_edit_data_change(HWND  hwnd, int offset)
{
    SendMessage(hwnd, STREAM_EDIT_DATA_CHANGE, offset, 0);
}

void u8_str2n(void *field_addr, char *info)
{
    if (strchr(info, 'x') || strchr(info, 'X'))
        *(unsigned char *)field_addr=strtol(info+0,NULL,16);
    else
        *(unsigned char *)field_addr=strtol(info+0,NULL,10);
}

void u8_n2str(char *info, void * field_addr, int is_hex)
{
if (is_hex)
            sprintf(info, "0x%02hhx"
            , *(unsigned char *)field_addr);
else
            sprintf(info, "%hhu"
            , *(unsigned char *)field_addr);
}


void u16_str2n(void *field_addr, char *info)
{
    if (strchr(info, 'x') || strchr(info, 'X'))
        *(unsigned short *)field_addr=htons(strtol(info+0,NULL,16));
    else        
        *(unsigned short *)field_addr=htons(strtol(info+0,NULL,10));
}

void u16_n2str(char *info, void * field_addr, int is_hex)
{
    if (is_hex)
            sprintf(info, "0x%04hx"
            , ntohs(*(unsigned short *)field_addr));
    else
            sprintf(info, "%hu"
            , ntohs(*(unsigned short *)field_addr));
}


void u32_str2n(void *field_addr, char *info)
{
    if (strchr(info, 'x') || strchr(info, 'X'))
        *(unsigned *)field_addr=htonl(strtol(info+0,NULL,16));
    else
        *(unsigned *)field_addr=htonl(strtol(info+0,NULL,10));
}

void u32_n2str(char *info, void * field_addr, int is_hex)
{
    if (is_hex)
        sprintf(info, "0x%08x"
            , ntohl(*(unsigned *)field_addr));
    else
       sprintf(info, "%u"
            , ntohl(*(unsigned *)field_addr));
}




void mac_str2n(unsigned char *mac, char *info_usr)
{
    char info[32];
    strcpy(info, info_usr);
            info[2]=0;
            mac[0]=strtol(info+0,NULL,16);
            info[5]=0;
            mac[1]=strtol(info+3,NULL,16);
            info[8]=0;
            mac[2]=strtol(info+6,NULL,16);
            info[11]=0;
            mac[3]=strtol(info+9,NULL,16);
            info[14]=0;
            mac[4]=strtol(info+12,NULL,16);
            info[17]=0;
            mac[5]=strtol(info+15,NULL,16);
}

void mac_n2str(char *info, void *field_addr)
{
unsigned char *mac = field_addr;
sprintf(info, "%02hhx %02hhx %02hhx %02hhx %02hhx %02hhx"
            , mac[0]
            , mac[1]
            , mac[2]
            , mac[3]
            , mac[4]
            , mac[5]);
}

void bits_str2n(void *field_addr, char *info, int bytes_len, int bits_from, int bits_len)
{
    unsigned whole_value, value, shift_len=bytes_len*8 - bits_from - bits_len;
    if (strchr(info, 'x') || strchr(info, 'X'))
        value=strtol(info+0,NULL,16);
    else        
        value=strtol(info+0,NULL,10);
    
    if (1==bytes_len)
        whole_value = *(unsigned char *)field_addr;
    else if (2==bytes_len)
        whole_value = ntohs(*(unsigned short *)field_addr);
    else if (4==bytes_len)
        whole_value = ntohl(*(unsigned int *)field_addr);

        whole_value &= ~(((1<<bits_len)-1)<<shift_len);
        whole_value |= (value<<shift_len);
        
    if (1==bytes_len)
        *(unsigned char *)field_addr = whole_value;
    else if (2==bytes_len)
        *(unsigned short *)field_addr = htons(whole_value);
    else if (4==bytes_len)
        *(unsigned int *)field_addr = htonl(whole_value);

}

void bits_n2str(char *info, void * field_addr, int bytes_len, int bits_from, int bits_len)
{
    unsigned whole_value, shift_len=bytes_len*8 - bits_from - bits_len;
    
    if (1==bytes_len)
        whole_value = *(unsigned char *)field_addr;
    else if (2==bytes_len)
        whole_value = ntohs(*(unsigned short *)field_addr);
    else if (4==bytes_len)
        whole_value = ntohl(*(unsigned int *)field_addr);

        whole_value=(whole_value>>shift_len);
        whole_value&=((1<<bits_len)-1);
            sprintf(info, "%u"
            , whole_value);
}

t_stream gt_edit_stream;

typedef void (* tvi_update_proto_hdr)(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream);


typedef void (*init_field_func)(void *, t_rule *);
typedef void (*update_field_func)(void *, t_rule *);

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

void init_stream(t_stream    *pt_streams)
{
    memset(pt_streams, 0, STREAM_HDR_LEN);
    pt_streams->snd_cnt=1;
    pt_streams->flags |= CHECK_SUM_ALL;
    pt_streams->flags |= LEN_ALL;
    sprintf(pt_streams->name, "stream_%d", nr_cur_stream+1);    
}

typedef struct
{
    uint32_t  flags;
    uint32_t  offset;
    uint8_t   width;
    init_field_func   init_field;
    update_field_func update_field;
}    t_field_rule_proc_info;

t_field_rule_proc_info gat_field_rule_proc_info[]=
    {
            {CHECK_SUM_IP, 15, 1,  init_u8_n, update_u8_n},
            {0, 16, 2, NULL, NULL},
            {CHECK_SUM_IP, 18, 2,  init_u16_n, update_u16_n},
            {0, 20, 1, NULL, NULL},
            {0, 21, 1, NULL, NULL},
            {CHECK_SUM_IP, 22, 1,  init_u8_n, update_u8_n},
            {CHECK_SUM_IP, 23, 1,  NULL, NULL},
            {CHECK_SUM_IP, 24, 2,  NULL, NULL},
            {CHECK_SUM_IP|CHECK_SUM_UDP|CHECK_SUM_TCP, 26, 4,  init_u32_n, update_u32_n},
            {CHECK_SUM_IP|CHECK_SUM_UDP|CHECK_SUM_TCP, 30, 4,  init_u32_n, update_u32_n},
    };


typedef void (*n2str_func)(char *, void *);
typedef void (*str2n_func)(void *, char *);

typedef struct
{
    char *name;
    int ori_offset;
    int len;
    uint32_t flags;
    char bits_from;
    char bits_len;
    int data_offset;
} t_tvi_data;

#define    FLAG_REBUILD_TV    0x1<<30
#define    SUPPORT_RULE       0x1<<29
#define    DISPLAY_HEX        0x1<<28
#define    IS_MAC             0x1<<27
#define    IS_IP              0x1<<26
#define    IS_IP6             0x1<<25

void field_n2str(char *info, void *field_addr, int len, int bits_from, int bits_len, uint32_t flags)
{
    if (bits_len)
    {
        bits_n2str(info, field_addr
        , len
        , bits_from
        , bits_len);
        
        return;        
    }
    
    if (flags&IS_MAC)
    {
        mac_n2str(info, field_addr);
        return;
    }
    
    if (flags&IS_IP)
    {
        ip_n2str(info, field_addr);
        return;
    }
    else if (flags&IS_IP6)
    {
        ip6_n2str(info, field_addr);
        return;
    }
    
    switch(len)
    {
        case 1:
        u8_n2str(info, field_addr , flags&DISPLAY_HEX);
        return;
        
        case 2:
        u16_n2str(info, field_addr , flags&DISPLAY_HEX);
        return;
        
        case 4:
        u32_n2str(info, field_addr , flags&DISPLAY_HEX);
        return;
    }

}

void field_str2n(char *info, void *field_addr, int len, int bits_from, int bits_len, uint32_t flags)
{
    if (bits_len)
    {
        bits_str2n(field_addr, info
        , len
        , bits_from
        , bits_len);
        
        return;        
    }
    
    if (flags&IS_MAC)
    {
        mac_str2n(field_addr, info);
        return;
    }
    
    if (flags&IS_IP)
    {
        ip_str2n(field_addr, info);
        return;
    }
    else if (flags&IS_IP6)
    {
        ip6_str2n(field_addr, info);
        return;
    }
    
    switch(len)
    {
        case 1:
        u8_str2n(field_addr, info);
        return;
        
        case 2:
        u16_str2n(field_addr, info);
        return;
        
        case 4:
        u32_str2n(field_addr, info);
        return;
    }

}

t_tvi_data gat_eth_hdr_tvis[]=
{
    {"dst mac", 0, 6, IS_MAC},
    {"src mac", 6, 6, IS_MAC},
    {"eth type", 12, 2, FLAG_REBUILD_TV|DISPLAY_HEX},
};

t_tvi_data gat_arp_base_tvis[]=
{
    {"hdwr type", 14, 2, DISPLAY_HEX},
    {"protocol type", 16, 2},

    {"hdwr size", 18, 1},
    {"protocol size", 19, 1, FLAG_REBUILD_TV},
    {"opcode", 20, 2, DISPLAY_HEX},

};


t_tvi_data gat_arp_tvis[]=
{
    {"hdwr type", 14, 2, DISPLAY_HEX},
    {"protocol type", 16, 2},

    {"hdwr size", 18, 1},
    {"protocol size", 19, 1, FLAG_REBUILD_TV},
    {"opcode", 20, 2, DISPLAY_HEX},

    {"sender mac", 22, 6, IS_MAC},
    {"sender ip", 28, 4, IS_IP},
    {"target mac", 32, 6, IS_MAC},
    {"target ip", 38, 4, IS_IP},
};

t_tvi_data gat_arp6_tvis[]=
{
    {"hdwr type", 14, 2, DISPLAY_HEX},
    {"protocol type", 16, 2},

    {"hdwr size", 18, 1},
    {"protocol size", 19, 1, FLAG_REBUILD_TV},
    {"opcode", 20, 2, DISPLAY_HEX},

    {"sender mac", 22, 6, IS_MAC},
    {"sender ip", 28, 16, IS_IP6},
    {"target mac", 32, 6, IS_MAC},
    {"target ip", 38, 16, IS_IP6},
};

t_tvi_data gat_ip_hdr_tvis[]=
{
 {"ver",    14, 1, 0, 0, 4},
 {"hdr_len", 14, 1, FLAG_REBUILD_TV, 4, 4},
 {"tos", 15, 1, SUPPORT_RULE},
 {"total len", 16, 2},
 {"id", 18, 2, SUPPORT_RULE},
 {"rsv", 20, 1, 0, 0, 1},
  {"df",  20, 1, 0, 1, 1},
  {"mf",  20, 1, FLAG_REBUILD_TV, 2, 1},
 {"frag offset", 20, 2, FLAG_REBUILD_TV, 3, 13},
 {"ttl", 22, 1, SUPPORT_RULE},
 {"protocol", 23, 1, FLAG_REBUILD_TV},
 {"check sum", 24, 2, DISPLAY_HEX},

 {"src ip", 26, 4, SUPPORT_RULE|IS_IP},
 {"dst ip", 30, 4, SUPPORT_RULE|IS_IP},
};

t_tvi_data gat_ipv6_hdr_tvis[]=
{
 {"ver",    14, 4, 0, 0, 4},
 {"traffic class", 14, 4, SUPPORT_RULE, 4, 8},
  {"flow label", 14, 4, SUPPORT_RULE, 12, 20},
 {"payload Len", 18, 2},
 {"next hdr", 20, 1, FLAG_REBUILD_TV},
 {"hop limit", 21, 1, SUPPORT_RULE},

 {"src ip", 22, 16, IS_IP6},
 {"dst ip", 38, 16, IS_IP6},
};

t_tvi_data gat_ipv6_frag_hdr_tvis[]=
{
 {"next hdr", 54, 1, FLAG_REBUILD_TV},
 {"Reserved",    55, 1},
 {"frag offset", 56, 2, FLAG_REBUILD_TV, 0, 13},
  {"Res", 56, 2, 0, 13, 2},
  {"M", 56, 2, FLAG_REBUILD_TV, 15, 1},
 {"id", 58, 4, SUPPORT_RULE},
};

t_tvi_data gat_icmp_hdr_tvis[]=
{
  {"type", 34, 1, FLAG_REBUILD_TV},
  {"code", 35, 1, FLAG_REBUILD_TV},
  {"checksum", 36, 2, DISPLAY_HEX},
};

t_tvi_data gat_icmp_echo_hdr_tvis[]=
{
  {"id", 38, 2, SUPPORT_RULE},
  {"sequence", 40, 2, SUPPORT_RULE},
};

t_tvi_data gat_igmp_hdr_tvis[]=
{
  {"type", 34, 1},
  {"code", 35, 1},
  {"checksum", 36, 2, DISPLAY_HEX},
  {"group", 38, 4, SUPPORT_RULE|IS_IP},
};

t_tvi_data gat_udp_hdr_tvis[]=
{
 {"source port", 34, 2, SUPPORT_RULE},
 {"dest   port", 36, 2, SUPPORT_RULE},
 {"length", 38, 2, SUPPORT_RULE},
 {"checksum", 40, 2, DISPLAY_HEX},
};

t_tvi_data gat_tcp_hdr_tvis[]=
{
 {"source port", 34, 2, SUPPORT_RULE},
 {"dest   port", 36, 2, SUPPORT_RULE},
 {"seq", 38, 4, SUPPORT_RULE},
 {"ack", 42, 4, SUPPORT_RULE},
 {"hdr_len", 46, 1, FLAG_REBUILD_TV, 0, 4},
 {"rsv", 46, 2, SUPPORT_RULE|DISPLAY_HEX, 4,6},
 {"URG", 47, 1, SUPPORT_RULE, 2,1},
 {"ACK", 47, 1, SUPPORT_RULE, 3,1},
 {"PSH", 47, 1, SUPPORT_RULE, 4,1},
 {"RST", 47, 1, SUPPORT_RULE, 5,1},
 {"SYN", 47, 1, SUPPORT_RULE, 6,1},
 {"FIN", 47, 1, SUPPORT_RULE, 7,1},

 {"win size", 48, 2, SUPPORT_RULE},
 {"checksum", 50, 2, DISPLAY_HEX},
 {"urgent_p", 52, 2, SUPPORT_RULE},
};

HTREEITEM Selected=NULL;
WNDPROC old_tv_proc;
LRESULT CALLBACK my_tv_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 	int id = GetWindowLong (hwnd, GWL_ID) ;
 	switch (message)
 	{
     	case WM_VSCROLL :
            SendMessage(GetParent(hwnd), WM_NOTIFY, ID_SED_TREE_VIEW, 0);
          		break ;
 	}

    return CallWindowProc (old_tv_proc, hwnd, message, wParam,lParam) ;
}

int need_rebuild_tv;
int stream_changed;

void hex_win_reinit(HWND  hwnd)
{
    SendMessage(hwnd, WM_CREATE, 0, 0);
    resize_window(hwnd);
    InvalidateRect(hwnd, NULL, TRUE) ;
}

void *get_tvi_lParam(HWND htv, HTREEITEM htvi)
{
    TVITEM tvi={0};

    tvi.hItem = htvi;
    tvi.mask=TVIF_PARAM;
    TreeView_GetItem(htv, &tvi);
    return (void *)(tvi.lParam);

}

void add_tvi(HWND hwnd_tree, HTREEITEM treeItem1, int adjust, t_tvi_data *pt_tvi_data)
{
    HTREEITEM treeItem2;
    pt_tvi_data->data_offset=pt_tvi_data->ori_offset + adjust;
    treeItem2 = insertItem(hwnd_tree, TEXT(""), treeItem1, TVI_LAST, -1, -1, pt_tvi_data);
    update_tvi_proto_field(hwnd_tree, treeItem2);

}

void build_tvis(HWND hwnd_tree, HTREEITEM treeItem1, int adjust, t_tvi_data tvis[], int num)
{
    int i;
    for (i=0;i<num;i++)
    add_tvi(hwnd_tree, treeItem1, adjust, &(tvis[i]));

}

void build_hdr_info(char *info, const char *str, int off, int len)
{

    sprintf(info, "%-12s[offset=%d;length=%d]", str, off, len);
}


void set_tvi_text(HWND htv, HTREEITEM htvi, TCHAR *text)
{
    TVITEM tvi={0};
    tvi.hItem = htvi;
    tvi.mask=TVIF_TEXT;
    tvi.pszText=text;
    TreeView_SetItem(htv,&tvi);

}

void get_tvi_text(HWND htv, HTREEITEM htvi, TCHAR *text, int buf_len)
{
    TVITEM tvi={0};
    tvi.hItem = htvi;
    tvi.mask=TVIF_TEXT;
    tvi.pszText=text;
    tvi.cchTextMax = buf_len;
    TreeView_GetItem(htv, &tvi);
}

int htvi_is_proto_hdr(HWND htv, HTREEITEM htvi)
{
    char info[128];

    get_tvi_text(htv, htvi, info, sizeof(info));

    if (NULL==strchr(info, '='))
    {
        return 0;
    }
    
    return 1;
}

int htvi_is_data(HWND htv, HTREEITEM htvi)
{
    char info[128];

    get_tvi_text(htv, htvi, info, sizeof(info));

    if (0!=memcmp(info, "data", 4))
    {
        return 0;
    }
    
    return 1;
}

int htvi_is_option(HWND htv, HTREEITEM htvi)
{
    char info[128];

    get_tvi_text(htv, htvi, info, sizeof(info));

    if (0!=memcmp(info, "options", 7))
    {
        return 0;
    }
    
    return 1;
}

void update_tvi_proto_field(HWND htv, HTREEITEM htvi)
{
    TVITEM tvi={0};
    t_tvi_data *pt_tvi_data;
    char info[128], info_2[128];
    pt_tvi_data = get_tvi_lParam(htv, htvi);

    field_n2str(info_2
        , gt_edit_stream.data+pt_tvi_data->data_offset
        ,pt_tvi_data->len
        ,pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        ,pt_tvi_data->flags);
  
    sprintf(info, "%-13s: %s", pt_tvi_data->name, info_2);
    set_tvi_text(htv, htvi, info);

}

void update_tvi_proto_hdr(HWND htv, HTREEITEM htvi)
{

    tvi_update_proto_hdr pt_tvi_data;
    pt_tvi_data = get_tvi_lParam(htv, htvi);

    if (NULL != pt_tvi_data)
        pt_tvi_data(htv, htvi, &gt_edit_stream);
    else  
        set_tvi_text(htv, htvi, "haha");

}

void update_tvi_options(HWND htv, HTREEITEM htvi)
{

    tvi_update_proto_hdr pt_tvi_data;
    pt_tvi_data = get_tvi_lParam(htv, htvi);

    if (NULL != pt_tvi_data)
        pt_tvi_data(htv, htvi, &gt_edit_stream);
    else  
        set_tvi_text(htv, htvi, "haha");

}

void update_tvi_text(HWND htv, HTREEITEM htvi)
{

    if (NULL==htvi) return 0;
    
    if (htvi_edit_able(htv, htvi))
        update_tvi_proto_field(htv, htvi);
    else if (htvi_is_proto_hdr(htv, htvi))
        update_tvi_proto_hdr(htv, htvi);
    else if (htvi_is_data(htv, htvi))
        tvi_update_data(htv, htvi, &gt_edit_stream);
    else if (htvi_is_option(htv, htvi))
        update_tvi_options(htv, htvi);

}


void tvi_update_eth_hdr(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
    build_hdr_info(info, TEXT("ethernet"), 0, 14);
    set_tvi_text(htv, htvi, info);
}

void tvi_update_arp_hdr(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];

    t_arp_hdr *pt_arp = (void *)(pt_edit_stream->eth_packet.payload);
    build_hdr_info(info, TEXT("arp"), 14, arp_pkt_len(pt_arp));
    set_tvi_text(htv, htvi, info);
}

void tvi_update_ip_hdr(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
  
    t_ip_hdr *iph=(void *)(pt_edit_stream->eth_packet.payload);
    
        if (ip_pkt_is_frag(&(pt_edit_stream->eth_packet)))
        {
            if (ip_frag_offset(&(pt_edit_stream->eth_packet)))
            {
                build_hdr_info(info, TEXT("ip(frag-x)"), 14, ip_hdr_len(iph));
                set_tvi_text(htv, htvi, info);
                return;
            }
            
            build_hdr_info(info, TEXT("ip(frag-1)"), 14, ip_hdr_len(iph));
            set_tvi_text(htv, htvi, info);
            return;
        }

        build_hdr_info(info, TEXT("ip"), 14, ip_hdr_len(iph));
        set_tvi_text(htv, htvi, info);

}

void tvi_update_ip_upper_hdr(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
  
    t_ip_hdr *iph=(void *)(pt_edit_stream->eth_packet.payload);
    
    if (iph->protocol==IPPROTO_ICMP)
        build_hdr_info(info, TEXT("icmp"), 14+ip_hdr_len(iph), icmp_hdr_len(ip_data(iph)));
   else if (iph->protocol==IPPROTO_IGMP)
       build_hdr_info(info, TEXT("igmp"), 14+ip_hdr_len(iph),  sizeof(t_igmp_hdr));
   else if (iph->protocol==IPPROTO_TCP)
       build_hdr_info(info, TEXT("tcp"), 14+ip_hdr_len(iph), tcp_hdr_len(ip_data(iph)));
   else if (iph->protocol==IPPROTO_UDP)
       build_hdr_info(info, TEXT("udp"), 14+ip_hdr_len(iph), sizeof(t_udp_hdr));
       
        set_tvi_text(htv, htvi, info);

}

void tvi_update_ip6_hdr(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
  
    t_ipv6_hdr *ip6h=(void *)(pt_edit_stream->eth_packet.payload);
    
        if (ip_pkt_is_frag(&(pt_edit_stream->eth_packet)))
        {
            if (ip_frag_offset(&(pt_edit_stream->eth_packet)))
            {
                build_hdr_info(info, TEXT("ipv6(frag-x)"), 14, IPV6_HDR_LEN);
                set_tvi_text(htv, htvi, info);
            }
            else
            {
                build_hdr_info(info, TEXT("ipv6(frag-1)"), 14, IPV6_HDR_LEN);
                set_tvi_text(htv, htvi, info);
            }           


        }
        else
        {
            build_hdr_info(info, TEXT("ipv6"), 14, IPV6_HDR_LEN);
            set_tvi_text(htv, htvi, info);
        }

}


void tvi_update_ip6_upper_hdr(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
  
    t_ipv6_hdr *ip6h=(void *)(pt_edit_stream->eth_packet.payload);
    
    if (ip6h->nexthdr==IPPROTO_ICMPV6)
        build_hdr_info(info, TEXT("icmp"),  14+IPV6_HDR_LEN, icmp6_hdr_len(ip6_data(ip6h)));
   else if (ip6h->nexthdr==IPPROTO_TCP)
       build_hdr_info(info, TEXT("tcp"),  14+IPV6_HDR_LEN, tcp_hdr_len(ip6_data(ip6h)));
   else if (ip6h->nexthdr==IPPROTO_UDP)
       build_hdr_info(info, TEXT("udp"),  14+IPV6_HDR_LEN, sizeof(t_udp_hdr));
   else if (ip6h->nexthdr==IPPROTO_FRAGMENT)
       build_hdr_info(info, TEXT("frag"),  14+IPV6_HDR_LEN, 8);

        set_tvi_text(htv, htvi, info);

}

void tvi_update_data(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
    int offset, len;
    if (ntohs(pt_edit_stream->eth_packet.type)==ETH_P_IP)
    {
        t_ip_hdr *iph=(void *)(pt_edit_stream->eth_packet.payload);
        if (ip_pkt_is_frag(&(pt_edit_stream->eth_packet)))
        {
            if (ip_frag_offset(&(pt_edit_stream->eth_packet)))
            {
                offset = 14+ip_hdr_len(iph);
                len = ip_data_len(iph);
                goto exit;
            }
        }

        if (iph->protocol==IPPROTO_ICMP)
        {
            t_icmp_hdr *pt_icmp_hdr=ip_data(iph);
            if ((pt_icmp_hdr->type==8 || pt_icmp_hdr->type==0)
                && pt_icmp_hdr->code==0 )
            {
                offset = 14+ip_hdr_len(iph)+FIXED_ICMP_ECHO_HDR_LEN;
                len = ip_data_len(iph)-FIXED_ICMP_ECHO_HDR_LEN;
                goto exit;
            }
            else
            {
                offset = 14+ip_hdr_len(iph)+FIXED_ICMP_HDR_LEN;
                len = ip_data_len(iph)-FIXED_ICMP_HDR_LEN;
                goto exit;
            }
        }
        else if (iph->protocol==IPPROTO_UDP)
        {
            offset = 14+ip_hdr_len(iph)+sizeof(t_udp_hdr);
            len = ip_data_len(iph)-sizeof(t_udp_hdr);
            goto exit;
        }
        else if (iph->protocol==IPPROTO_TCP)
        {
            t_tcp_hdr *pt_tcp_hdr=ip_data(iph);
            offset = 14+ip_hdr_len(iph)+tcp_hdr_len(pt_tcp_hdr);
            len = ip_data_len(iph)-tcp_hdr_len(pt_tcp_hdr);
            goto exit;

        }
        else
        {
            offset = 14+ip_hdr_len(iph);
            len = ip_data_len(iph)-ip_hdr_len(iph);
            goto exit;
        }



    }
    else if (ntohs(pt_edit_stream->eth_packet.type)==ETH_P_IPV6)
    {
        t_ipv6_hdr *ip6h=(void *)(pt_edit_stream->eth_packet.payload);
        if (ip_pkt_is_frag(&(pt_edit_stream->eth_packet)))
        {
                offset = 14+IPV6_HDR_LEN+8;
                len = ip6_data_len(ip6h)-8;
                goto exit;
        }
        if (ip6h->nexthdr==IPPROTO_ICMPV6)
        {
            t_icmp_hdr *pt_icmp_hdr=ip6_data(ip6h);
            if ((pt_icmp_hdr->type==128 || pt_icmp_hdr->type==129)
                && pt_icmp_hdr->code==0 )
            {
                offset = 14+IPV6_HDR_LEN+FIXED_ICMP_ECHO_HDR_LEN;
                len = ip6_data_len(ip6h)-FIXED_ICMP_ECHO_HDR_LEN;
                goto exit;
            }
            else
            {
                offset = 14+IPV6_HDR_LEN+FIXED_ICMP_HDR_LEN;
                len = ip6_data_len(ip6h)-FIXED_ICMP_HDR_LEN;
                goto exit;
            }
        }

        else if (ip6h->nexthdr==IPPROTO_UDP)
        {
            offset = 14+IPV6_HDR_LEN+sizeof(t_udp_hdr);
            len = ip6_data_len(ip6h)-sizeof(t_udp_hdr);
            goto exit;
        }
        else if (ip6h->nexthdr==IPPROTO_TCP)
        {
            t_udp_hdr *pt_tcp_hdr=ip6_data(ip6h);
            offset = 14+IPV6_HDR_LEN+tcp_hdr_len(pt_tcp_hdr);
            len = ip6_data_len(ip6h)-tcp_hdr_len(pt_tcp_hdr);
            goto exit;

        }
        else
        {
            offset = 14+IPV6_HDR_LEN;
            len = ip6_data_len(ip6h)-IPV6_HDR_LEN;
            goto exit;
        }

    }
    else if (ntohs(pt_edit_stream->eth_packet.type)==ETH_P_ARP)
    {
        t_ipv6_hdr *ip6h=(void *)(pt_edit_stream->eth_packet.payload);
        if (ip_pkt_is_frag(&(pt_edit_stream->eth_packet)))
        {
            offset = 14+FIXED_ARP_HDR_LEN;
            len = pt_edit_stream->len-offset;
            goto exit;
        
        }

    }
    else
    {
            offset = 14;
            len = pt_edit_stream->len-14;
    }


exit:
    sprintf(info, TEXT("data (offset=%d;length=%d)"), offset, len);
    set_tvi_text(htv, htvi, info);
}

void tvi_update_options_tcp(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
    int offset, len;

        t_ip_hdr *iph=(void *)(pt_edit_stream->eth_packet.payload);
        t_tcp_hdr *pt_tcp_hdr=ip_data(iph);

            offset = 14+ip_hdr_len(iph)+sizeof(t_tcp_hdr);
            len = tcp_hdr_len(pt_tcp_hdr)-sizeof(t_tcp_hdr);


    sprintf(info, TEXT("options (offset=%d;length=%d)"), offset, len);
    set_tvi_text(htv, htvi, info);
}

void tvi_update_options_ip(HWND htv, HTREEITEM htvi, t_stream *pt_edit_stream)
{
    char info[128];
    int offset, len;

        t_ip_hdr *iph=(void *)(pt_edit_stream->eth_packet.payload);

        offset = 14+sizeof(t_ip_hdr);
        len = ip_hdr_len(iph)-sizeof(t_ip_hdr);


    sprintf(info, TEXT("options (offset=%d;length=%d)"), offset, len);
    set_tvi_text(htv, htvi, info);
}

static void update_tv_sub(HWND htv, HTREEITEM htvi_p)
{
    HTREEITEM htvi_c;
    update_tvi_text(htv, htvi_p);
    htvi_c=TreeView_GetChild(htv, htvi_p);
    
    while (NULL != htvi_c)
    {
        update_tv_sub(htv, htvi_c);
        htvi_c=TreeView_GetNextSibling(htv,htvi_c);
    }
}

static void update_tv(HWND htv)
{

    HTREEITEM htvi_p, htvi_c;

    htvi_p=TreeView_GetRoot(htv);
    
    while (NULL != htvi_p)
    {
        update_tv_sub(htv, htvi_p);
        htvi_p=TreeView_GetNextSibling(htv,htvi_p);
    }
    
}


void build_tv(HWND hwnd_tree)
{
    HTREEITEM treeItem1, treeItem2;
    int adjust=0;
    char info[64];

    need_rebuild_tv=0;
    Selected=NULL;

    TreeView_DeleteAllItems(hwnd_tree);

    treeItem1=insertItem(hwnd_tree, "ethernet", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_eth_hdr);
    update_tvi_proto_hdr(hwnd_tree, treeItem1);
    build_tvis(hwnd_tree, treeItem1
        , 0, gat_eth_hdr_tvis, ARRAY_SIZE(gat_eth_hdr_tvis));
    
    if (ntohs(gt_edit_stream.eth_packet.type)==ETH_P_ARP)
    {
        t_arp_hdr *pt_arp = (void *)(gt_edit_stream.eth_packet.payload);
        treeItem1=insertItem(hwnd_tree, TEXT("arp"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_arp_hdr);
        update_tvi_proto_hdr(hwnd_tree, treeItem1);
        if (4==pt_arp->ar_pln)
        {
            build_tvis(hwnd_tree, treeItem1
                , 0, gat_arp_tvis, ARRAY_SIZE(gat_arp_tvis));
        }
        else if (16==pt_arp->ar_pln)
        {
            build_tvis(hwnd_tree, treeItem1
                , 0, gat_arp6_tvis, ARRAY_SIZE(gat_arp6_tvis));
        }
        else
        {
            build_tvis(hwnd_tree, treeItem1
                , 0, gat_arp_base_tvis, ARRAY_SIZE(gat_arp_base_tvis));
            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);

        }
    }
    else if (ntohs(gt_edit_stream.eth_packet.type)==ETH_P_IP)
    {
        t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);
        treeItem1=insertItem(hwnd_tree, TEXT("ip"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip_hdr);
        update_tvi_proto_hdr(hwnd_tree, treeItem1);
        adjust = ip_hdr_len(iph)-FIXED_IP_HDR_LEN;


        build_tvis(hwnd_tree, treeItem1
                , 0, gat_ip_hdr_tvis, ARRAY_SIZE(gat_ip_hdr_tvis));

        if (ip_hdr_len(iph)>FIXED_IP_HDR_LEN)
        {

            treeItem2=insertItem(hwnd_tree, TEXT("options"), treeItem1, TVI_LAST, -1, -1, tvi_update_options_ip);
            update_tvi_options(hwnd_tree, treeItem2);
        }
        

        if (ip_pkt_is_frag(&(gt_edit_stream.eth_packet)))
        {
            if (ip_frag_offset(&(gt_edit_stream.eth_packet)))
            {
                treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
                tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);
                return;
            }
            
        }

        if (iph->protocol==IPPROTO_ICMP)
        {
            t_icmp_hdr *pt_icmp_hdr=ip_data(iph);
            treeItem1=insertItem(hwnd_tree, "icmp", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);

            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_icmp_hdr_tvis, ARRAY_SIZE(gat_icmp_hdr_tvis));
            
            if ((pt_icmp_hdr->type==8 || pt_icmp_hdr->type==0)
                && pt_icmp_hdr->code==0 )
            {
                build_tvis(hwnd_tree, treeItem1
                , adjust, gat_icmp_echo_hdr_tvis, ARRAY_SIZE(gat_icmp_echo_hdr_tvis));
            }

            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);

        }
        else if (iph->protocol==IPPROTO_IGMP)
        {
            t_igmp_hdr *pt_igmp_hdr=ip_data(iph);
            treeItem1=insertItem(hwnd_tree, "igmp", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);
            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_igmp_hdr_tvis, ARRAY_SIZE(gat_igmp_hdr_tvis));
        }
        else if (iph->protocol==IPPROTO_UDP)
        {
            treeItem1=insertItem(hwnd_tree, "udp", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);
            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_udp_hdr_tvis, ARRAY_SIZE(gat_udp_hdr_tvis));
            if (udp_data_len(iph)>0)
            {
                treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
                tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);
            }
        }
        else if (iph->protocol==IPPROTO_TCP)
        {
            build_hdr_info(info, TEXT("tcp"), 14+ip_hdr_len(iph), ip_data_len(iph));
            treeItem1=insertItem(hwnd_tree, info, TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);
            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_tcp_hdr_tvis, 5);
treeItem2=insertItem(hwnd_tree, TEXT("flags"), treeItem1, TVI_LAST, -1, -1, NULL);

            build_tvis(hwnd_tree, treeItem2
                , adjust, gat_tcp_hdr_tvis+5, 7);

            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_tcp_hdr_tvis+12, 3);

            if (tcp_hdr_len(ip_data(iph))>sizeof(t_tcp_hdr))
            {

                treeItem2=insertItem(hwnd_tree, TEXT("options"), treeItem1, TVI_LAST, -1, -1, tvi_update_options_tcp);
                update_tvi_options(hwnd_tree, treeItem2);
            }
            
            if (tcp_data_len(iph)>0)
            {
                treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
                tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);
            }
        }
        else
        {
            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);
        }

    }
    else if (ntohs(gt_edit_stream.eth_packet.type)==ETH_P_IPV6)
    {
        t_ipv6_hdr *ip6h=(void *)(gt_edit_stream.eth_packet.payload);
        treeItem1=insertItem(hwnd_tree, TEXT("ipv6"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip6_hdr);
        update_tvi_proto_hdr(hwnd_tree, treeItem1);
        adjust = IPV6_HDR_LEN-FIXED_IP_HDR_LEN;

        build_tvis(hwnd_tree, treeItem1
                , 0, gat_ipv6_hdr_tvis, ARRAY_SIZE(gat_ipv6_hdr_tvis));

        if (ip_pkt_is_frag(&(gt_edit_stream.eth_packet)))
        {

            treeItem1=insertItem(hwnd_tree, "frag", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip6_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);
            build_tvis(hwnd_tree, treeItem1
                , 0, gat_ipv6_frag_hdr_tvis, ARRAY_SIZE(gat_ipv6_frag_hdr_tvis));
            
            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);

        }
        else if (ip6h->nexthdr==IPPROTO_ICMPV6)
        {
            t_icmp_hdr *pt_icmp_hdr=ip6_data(ip6h);
            treeItem1=insertItem(hwnd_tree, "icmpv6", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip6_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);

            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_icmp_hdr_tvis, ARRAY_SIZE(gat_icmp_hdr_tvis));
            
            if ((pt_icmp_hdr->type==128 || pt_icmp_hdr->type==129)
                && pt_icmp_hdr->code==0 )
            {
                build_tvis(hwnd_tree, treeItem1
                , adjust, gat_icmp_echo_hdr_tvis, ARRAY_SIZE(gat_icmp_echo_hdr_tvis));
            }

            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);

        }

        else if (ip6h->nexthdr==IPPROTO_UDP)
        {
            treeItem1=insertItem(hwnd_tree, "udp", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip6_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);
            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_udp_hdr_tvis, ARRAY_SIZE(gat_udp_hdr_tvis));

            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);

        }
        else if (ip6h->nexthdr==IPPROTO_TCP)
        {
            treeItem1=insertItem(hwnd_tree, "tcp", TVI_ROOT, TVI_LAST, -1, -1, tvi_update_ip6_upper_hdr);
            update_tvi_proto_hdr(hwnd_tree, treeItem1);
            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_tcp_hdr_tvis, 5);
            
treeItem2=insertItem(hwnd_tree, TEXT("flags"), treeItem1, TVI_LAST, -1, -1, NULL);

            build_tvis(hwnd_tree, treeItem2
                , adjust, gat_tcp_hdr_tvis+5, 7);

            build_tvis(hwnd_tree, treeItem1
                , adjust, gat_tcp_hdr_tvis+12, 3);
            
            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);


        }
        else
        {
            treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
            tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);
        }

    }
    else
    {
        treeItem1=insertItem(hwnd_tree, TEXT("data"), TVI_ROOT, TVI_LAST, -1, -1, tvi_update_data);
        tvi_update_data(hwnd_tree, treeItem1, &gt_edit_stream);

    }
}


int htvi_edit_able(HWND htv, HTREEITEM htvi)
{
    char info[128];

    get_tvi_text(htv, htvi, info, sizeof(info));

    if (NULL==strchr(info, ':'))
    {
        return 0;
    }
    
    return 1;
}


void append_err_text(char *info, uint32_t err_flags)
{
    if (err_flags==0) return;
    
    strcat(info, "(error:");
    if (err_flags&ERR_IP_CHECKSUM)
    {
        strcat(info, "ip check sum");
        goto exit;
    }

    if (err_flags&ERR_ICMP_CHECKSUM)
    {
        strcat(info, "icmp check sum");
        goto exit;
    }

    if (err_flags&ERR_IGMP_CHECKSUM)
    {
        strcat(info, "igmp check sum");
        goto exit;
    }

    if (err_flags&ERR_TCP_CHECKSUM)
    {
        strcat(info, "tcp check sum");
        goto exit;
    }

    if (err_flags&ERR_UDP_CHECKSUM)
    {
        strcat(info, "udp check sum");
        goto exit;
    }

    if (err_flags&ERR_PKT_LEN)
    {
        strcat(info, "packet length");
        goto exit;
    }
exit:
    strcat(info, ")");
    
}

void get_pkt_desc_info_v4(char *info, void* p_eth_hdr)
{
    t_ether_packet *pt_eth_hdr = p_eth_hdr;
    t_ip_hdr *iph=(void *)(pt_eth_hdr->payload);
    t_icmp_hdr *icmp_hdr = ip_data(iph);
    t_tcp_hdr *tcp_hdr = ip_data(iph);
    char info_2[64];

    switch (iph->protocol)
    {
        case IPPROTO_ICMP:

            if (icmp_hdr->type==8 && icmp_hdr->code==0)
                strcpy(info, "ping request");
            else if (icmp_hdr->type==0 && icmp_hdr->code==0)
                strcpy(info, "ping reply");
            else
                sprintf(info, "type %hhu code %hhu", icmp_hdr->type, icmp_hdr->code);

            return;
            
        case IPPROTO_IGMP:
            strcpy(info, "IGMP");
            return;

        case IPPROTO_TCP:
            sprintf(info, "port %hu->%hu[%s%s%s%s%s%s%s%s]"
                , ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest)
                ,tcp_hdr->cwr?"cwr ":""
                ,tcp_hdr->ece?"ece ":""
                ,tcp_hdr->urg?"urg ":""
                ,tcp_hdr->ack?"ack ":""
                ,tcp_hdr->psh?"psh ":""
                ,tcp_hdr->rst?"rst ":""
                ,tcp_hdr->syn?"syn ":""
                ,tcp_hdr->fin?"fin ":"");

            return;

        case IPPROTO_UDP:
            sprintf(info, "port %hu->%hu", ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest));
            return;

    }

}

void get_pkt_desc_info_v6(char *info, void* p_eth_hdr)
{
    t_ether_packet *pt_eth_hdr = p_eth_hdr;
    t_ipv6_hdr *ip6h=(void *)(pt_eth_hdr->payload);
    t_tcp_hdr *tcp_hdr = ip6_data(ip6h);
    t_icmp_hdr *pt_icmp_hdr=ip6_data(ip6h);

    switch (ip6h->nexthdr)
    {
        case IPPROTO_TCP:
            sprintf(info, "port %hu->%hu[%s%s%s%s%s%s%s%s]"
                , ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest)
                ,tcp_hdr->cwr?"cwr ":""
                ,tcp_hdr->ece?"ece ":""
                ,tcp_hdr->urg?"urg ":""
                ,tcp_hdr->ack?"ack ":""
                ,tcp_hdr->psh?"psh ":""
                ,tcp_hdr->rst?"rst ":""
                ,tcp_hdr->syn?"syn ":""
                ,tcp_hdr->fin?"fin ":"");

            return;

        case IPPROTO_UDP:
            sprintf(info, "port %hu->%hu", ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest));
            return;

        case IPPROTO_ICMPV6:
            if ((pt_icmp_hdr->type==128)  && (pt_icmp_hdr->code==0) )
                strcpy(info, "ping request");
            else if ((pt_icmp_hdr->type==129)  && (pt_icmp_hdr->code==0) )
                strcpy(info, "ping reply");
            else
                sprintf(info, "type %hhu code %hhu", pt_icmp_hdr->type, pt_icmp_hdr->code);

            return;

    }

}

void get_pkt_desc_info(char *info, void* p_eth_hdr, uint32_t err_flags)
{
    t_ether_packet *pt_eth_hdr = p_eth_hdr;
    t_arp_hdr *arp_hdr = (void *)(pt_eth_hdr->payload);
    char info_2[64];
    int eth_type = ntohs(pt_eth_hdr->type);

    info[0]=0;
    
    switch (eth_type)
    {
        case ETH_P_ARP:
            if (ntohs(arp_hdr->ar_op)==1)
            {
                if (4==arp_hdr->ar_pln)
                {
                    ip_n2str(info_2, arp_hdr->ar_tip);
                    sprintf(info, "who has %s? tell ", info_2);
                    ip_n2str(info_2, arp_hdr->ar_sip);
                    strcat(info, info_2);
                }
                else if (16==arp_hdr->ar_pln)
                {
                    ip6_n2str(info_2, arp_hdr->ar_sip+22);
                    sprintf(info, "who has %s? tell ", info_2);
                    ip6_n2str(info_2, arp_hdr->ar_sip);
                    strcat(info, info_2);
                }
            }
            else if (ntohs(arp_hdr->ar_op)==2)
            {
                ip_n2str(info_2, arp_hdr->ar_sip);
                sprintf(info, "%s is at ", info_2);
                mac_n2str(info_2, arp_hdr->ar_sha);
                strcat(info, info_2);
            }
            
            goto append_err_info;
            
        case ETH_P_RARP:
            strcpy(info, "rarp");
            goto append_err_info;

        case ETH_P_LOOP:
            strcpy(info, "Ethernet Loopback packet");
            goto append_err_info;

        case ETH_P_ECHO:
            strcpy(info, "Ethernet Echo packet");
            goto append_err_info;
            
        case ETH_P_PPP_DISC:
            strcpy(info, "PPPoE discovery messages");
            goto append_err_info;
            
        case ETH_P_PPP_SES:
            strcpy(info, "PPPoE session messages");
            goto append_err_info;
            

    }

    if (eth_type!=ETH_P_IP && eth_type!=ETH_P_IPV6)
    {
        sprintf(info, "eth type:0x%04x", ntohs(pt_eth_hdr->type));
            goto append_err_info;
    }

    if (ip_pkt_is_frag(pt_eth_hdr))
    {
        sprintf(info, "frag ");
        goto append_err_info;

    }

    if (eth_type==ETH_P_IP)
    {
        get_pkt_desc_info_v4(info, p_eth_hdr);
    }
    else
    {
        get_pkt_desc_info_v6(info, p_eth_hdr);
    }

append_err_info:
    append_err_text(info, err_flags);

    return;

}

BOOL InitDlgFromStream(HWND hDlg, t_stream* pt_stream)
{
    TCHAR    info[32], info_2[32];
    HWND hwnd_tree, hwnd_edit, hwnd_button, hwnd_comb;
    HTREEITEM treeItem1, treeItem2;
    t_ip_hdr *iph=(void *)(pt_stream->eth_packet.payload);

  hwnd_tree = GetDlgItem(hDlg, ID_SED_TREE_VIEW);
  hwnd_edit = GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT);
  hwnd_button = GetDlgItem(hDlg, ID_SED_DYNAMIC_RULE_BUTTON);
  hwnd_comb = GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB);

    SetDlgItemText(hDlg, ID_SED_NAME, pt_stream->name);
    
    set_int_text(GetDlgItem(hDlg, ID_SED_LEN), pt_stream->len);

    build_tv(hwnd_tree);

    button_check(GetDlgItem(hDlg, ID_SED_IP_CHECKSUM)
        , (pt_stream->flags & CHECK_SUM_IP));

    button_check(GetDlgItem(hDlg, ID_SED_ICMP_CHECKSUM)
        , (pt_stream->flags & CHECK_SUM_ICMP));

    button_check(GetDlgItem(hDlg, ID_SED_TCP_UDP_CHECKSUM)
        , (pt_stream->flags & (CHECK_SUM_TCP|CHECK_SUM_UDP)));

    button_check(GetDlgItem(hDlg, ID_SED_IP_LEN)
        , (pt_stream->flags & IP_LEN));

    button_check(GetDlgItem(hDlg, ID_SED_UDP_LEN)
        , (pt_stream->flags & UDP_LEN));

    return TRUE;
}


int update_data_from_edit(HWND hDlg, HWND htv, HTREEITEM htvi, HWND hedit, int edit_visible)
{
    char info[64], info_2[64];
    t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);

    t_tvi_data *pt_tvi_data;
    int data_changed;
    
    GetWindowText(hedit, info_2, sizeof(info_2));
hide_edit_ui(hDlg);
    pt_tvi_data = get_tvi_lParam(htv, htvi);
    memcpy(info, gt_edit_stream.data+pt_tvi_data->data_offset, pt_tvi_data->len);

    field_str2n(info_2
        , gt_edit_stream.data+pt_tvi_data->data_offset
        , pt_tvi_data->len
        , pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        , pt_tvi_data->flags);
    
    data_changed=memcmp(info, gt_edit_stream.data+pt_tvi_data->data_offset, pt_tvi_data->len);
    if (!data_changed) return 0;

    InvalidateRect(GetDlgItem(hDlg,ID_SED_HEX_EDIT), NULL, TRUE) ;
    //update_tvi_text(htv, htvi);
    update_stream_from_dlg(hDlg);
    update_tv(htv);

    if (pt_tvi_data->flags&FLAG_REBUILD_TV)
    {
        SendMessage(hDlg, WM_COMMAND, ID_SED_UPDATE_NOW, 0);
        show_tip("字段变化规则已经删除，因为报文结构有变化");

        return 1;
    }

    return 1;


}
#if 0
int tvi_char_width(HWND htv)
{
    RECT		rect1;
    *(HTREEITEM*)&rect1 = hti_ethernet;
    SendMessage(htv, TVM_GETITEMRECT, TRUE, (LPARAM)&rect1);
    return (rect1.right-rect1.left)/strlen("ethernet");

}
#endif
void show_edit_ui_for_tvi(HWND hDlg, HWND htv, HTREEITEM htvi)
{
    TVITEM tvi={0};
    t_tvi_data *pt_tvi_data;
    char info[128], info_2[128];
    RECT		rect1, rect2, rect3 ;
    HWND hedit=GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT);

    get_client_rect_screen(hDlg, &rect1);
    GetWindowRect(htv, &rect2);
    get_relative_pos(&rect1, &rect2, &rect3);


    *(HTREEITEM*)&rect1 = htvi;

    SendMessage(htv, TVM_GETITEMRECT, TRUE, (LPARAM)&rect1);

    rect1.left+=rect3.left;
    rect1.right+=rect3.left;
    rect1.top+=rect3.top;
    rect1.bottom+=rect3.top;

    rect1.left+=cxChar*15;
    rect1.right+=3*cxChar;
    rect1.top-=cyChar/10;
    rect1.bottom+=cyChar/10;

    tvi.hItem = htvi;
    tvi.mask=TVIF_PARAM;
    TreeView_GetItem(htv, &tvi);
    pt_tvi_data = tvi.lParam;

    hex_win_sel(GetDlgItem(hDlg, ID_SED_HEX_EDIT), pt_tvi_data->data_offset, pt_tvi_data->len);

    if ((ETH_P_IP==ntohs(gt_edit_stream.eth_packet.type))
        &&
        (pt_tvi_data->data_offset==23))
    {
        t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);
        SendMessage(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB), CB_SETCURSEL, (WPARAM)iph->protocol, (LPARAM)0);
        MoveWindow(	GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB)
        ,rect1.left
        ,rect1.top
        ,cxChar*30
        ,150
        ,TRUE);

        ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB), 1);

    }
    else if ((ETH_P_IPV6==ntohs(gt_edit_stream.eth_packet.type))
        &&
        (pt_tvi_data->data_offset==20))
    {
        t_ipv6_hdr *ip6h=(void *)(gt_edit_stream.eth_packet.payload);
        SendMessage(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB), CB_SETCURSEL, (WPARAM)ip6h->nexthdr, (LPARAM)0);
        MoveWindow(	GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB)
        ,rect1.left
        ,rect1.top
        ,cxChar*30
        ,150
        ,TRUE);

        ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB), 1);

    }
    else if (pt_tvi_data->data_offset==12)
    {
        HWND comb_eth=GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB_ETH);
        int type_idx;
        init_eth_type_comb(comb_eth);
        type_idx = get_eth_type_name(ntohs(gt_edit_stream.eth_packet.type), NULL);
        if (type_idx>=0)
        {
            SendMessage(comb_eth, CB_SETCURSEL, (WPARAM)type_idx, (LPARAM)0);
        }
        else
        {
            sprintf(info, "0x%04hx", ntohs(gt_edit_stream.eth_packet.type));
            ComboBox_AddString(comb_eth,(LPARAM)info);
            SendMessage(comb_eth, CB_SETCURSEL, (WPARAM)ComboBox_GetCount(comb_eth)-1, (LPARAM)0);
        }
        MoveWindow(comb_eth
        ,rect1.left
        ,rect1.top
        ,cxChar*20
        ,150
        ,TRUE);

        ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB_ETH), 1);

    }

    else
    {
        field_n2str(info_2
        , gt_edit_stream.data+pt_tvi_data->data_offset
        ,pt_tvi_data->len
        ,pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        ,pt_tvi_data->flags);
        
        SetDlgItemText(hDlg, ID_SED_DYNAMIC_EDIT, info_2);
        MoveWindow(	hedit
        ,rect1.left
        ,rect1.top
        ,rect1.right-rect1.left
        ,rect1.bottom-rect1.top
        ,TRUE);

        ShowWindow(hedit, 1);

        if (0==(pt_tvi_data->flags&SUPPORT_RULE)) return;
        
        MoveWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_RULE_BUTTON)
            ,rect1.right+5
            ,rect1.top
            ,150
            ,rect1.bottom-rect1.top
            ,TRUE);

        ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_RULE_BUTTON), 1);
    }
}


int htvi_sel_hex(HWND hHexEdit, HWND htv, HTREEITEM htvi)
{
    char info[128];

    get_tvi_text(htv, htvi, info, sizeof(info));


        if (NULL!=strchr(info, '='))
        {
            int off = atoi(strchr(info, '=')+1);
            int len = atoi(strrchr(info, '=')+1);
            hex_win_sel(hHexEdit, off, len);
        }

}

void hide_edit_ui(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT), 0);
    ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB), 0);
    ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB_ETH), 0);
    ShowWindow(GetDlgItem(hDlg, ID_SED_DYNAMIC_RULE_BUTTON), 0);
}

typedef struct
{
    int rule_idx;
    t_rule *pt_rule;
    int valid;
    char min[32];
    char max[32];
    char step[32];
} t_rule_data;

t_rule_data gt_rule_data;

t_rule * get_rule_from_pkt(t_stream *pt_stream, int offset, int *idx)
{
    int i;
    t_rule *pt_rule;
    if (0==pt_stream->rule_num) return NULL;
    for (i=0; i<pt_stream->rule_num; i++)
    {
        pt_rule = &(pt_stream->at_rules[pt_stream->rule_idx[i]]);
        if (pt_rule->valid==1 && offset==pt_rule->offset)
        {
            *idx = i;
            return pt_rule;
        }
    }

    return NULL;
}

t_rule * get_empty_rule_from_pkt(t_stream *pt_stream, int *idx)
{
    int i;
    t_rule *pt_rule;
    if (pt_stream->rule_num>=MAX_FIELD_RULE_NUM) return NULL;
    for (i=0; i<MAX_FIELD_RULE_NUM; i++)
    {
        pt_rule = &(pt_stream->at_rules[i]);
        if (0==pt_rule->valid)
        {
            *idx = i;
            return pt_rule;
        }
    }

    return NULL;
}

void init_rule_data(HWND htv, HTREEITEM htvi)
{
    t_tvi_data *pt_tvi_data=get_tvi_lParam(htv, htvi);
    t_rule *pt_rule=NULL;
    int rule_idx;
    pt_rule=get_rule_from_pkt(&gt_edit_stream, pt_tvi_data->data_offset, &rule_idx);

    if (NULL==pt_rule)
    {
        gt_rule_data.rule_idx=-1;
        gt_rule_data.pt_rule=NULL;
        gt_rule_data.valid=0;

        field_n2str(gt_rule_data.min
        , gt_edit_stream.data+pt_tvi_data->data_offset
        ,pt_tvi_data->len
        ,pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        ,pt_tvi_data->flags);
        strcpy(gt_rule_data.max, gt_rule_data.min);
        sprintf(gt_rule_data.step, "%d", 0);
    }
    else
    {

        gt_rule_data.rule_idx=rule_idx;
        gt_rule_data.pt_rule=pt_rule;
        gt_rule_data.valid=1;

        field_n2str(gt_rule_data.min
        , pt_rule->base_value
        ,pt_tvi_data->len
        ,pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        ,pt_tvi_data->flags);

        field_n2str(gt_rule_data.max
        , pt_rule->max_value
        ,pt_tvi_data->len
        ,pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        ,pt_tvi_data->flags);
        sprintf(gt_rule_data.step, "%d", pt_rule->step_size);
    }
}

int delete_all_rule(t_stream *pt_stream)
{
    int i;
    t_rule *pt_rule;
    int rule_num = pt_stream->rule_num;
    pt_stream->rule_num=0;
    for (i=0; i<MAX_FIELD_RULE_NUM; i++)
    {
        pt_rule = &(pt_stream->at_rules[i]);
        pt_rule->valid=0;
    }

    return rule_num;
}

void delete_rule(t_stream *pt_stream, int idx)
{
    int i;
    t_rule *pt_rule = &(pt_stream->at_rules[pt_stream->rule_idx[idx]]);
    pt_rule->valid=0;
    pt_stream->rule_num--;
    for (i=idx; i<pt_stream->rule_num; i++)
    {
        pt_stream->rule_idx[i]=pt_stream->rule_idx[i+1];
    }
}

void update_from_rule_data(HWND htv, HTREEITEM htvi)
{
    t_tvi_data *pt_tvi_data=get_tvi_lParam(htv, htvi);
    t_rule *pt_rule=NULL;
    int rule_idx;
    
    if (gt_rule_data.rule_idx==-1)
    {
        if (gt_rule_data.valid==0) return;
        pt_rule=get_empty_rule_from_pkt(&gt_edit_stream, &rule_idx);
        if (NULL==pt_rule)
        {
            WinPrintf(GetParent(htv), TEXT("最大只支持 %d 条规则"), MAX_FIELD_RULE_NUM);
            return;
        }

        pt_rule->valid=1;
        pt_rule->offset=pt_tvi_data->data_offset;
        pt_rule->width=pt_tvi_data->len;
        pt_rule->bits_from=pt_tvi_data->bits_from;
        pt_rule->bits_len=pt_tvi_data->bits_len;
        pt_rule->step_size = atoi(gt_rule_data.step);

        field_str2n(gt_rule_data.min
        , pt_rule->base_value
        , pt_tvi_data->len
        , pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        , pt_tvi_data->flags);
        
        field_str2n(gt_rule_data.max
        , pt_rule->max_value
        , pt_tvi_data->len
        , pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        , pt_tvi_data->flags);

        gt_edit_stream.rule_idx[gt_edit_stream.rule_num]=rule_idx;
        gt_edit_stream.rule_num++;
        return;
    }

    if (gt_rule_data.valid==0)
    {
        delete_rule(&gt_edit_stream, gt_rule_data.rule_idx);
        return;
    }

    pt_rule=gt_rule_data.pt_rule;
    pt_rule->step_size = atoi(gt_rule_data.step);
        field_str2n(gt_rule_data.min
        , pt_rule->base_value
        , pt_tvi_data->len
        , pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        , pt_tvi_data->flags);
        
        field_str2n(gt_rule_data.max
        , pt_rule->max_value
        , pt_tvi_data->len
        , pt_tvi_data->bits_from
        , pt_tvi_data->bits_len
        , pt_tvi_data->flags);

}


void check_sum_proc(t_stream *pt_stream)
{
    t_ip_hdr *iph=(void *)(pt_stream->eth_packet.payload);
    if (ntohs(pt_stream->eth_packet.type)!=ETH_P_IP) return;
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
        if (!pt_stream->selected) continue;
        if (pt_stream->rule_num) rule_fileds_init(pt_stream);
    }
}

void rule_cfg_gui_switch(HWND hDlg, int is_valid)
{
    button_check(GetDlgItem(hDlg, ID_RULE_CFG_USE_RULE), is_valid);
    EnableWindow(GetDlgItem(hDlg, ID_RULE_CFG_BASE), is_valid);
    EnableWindow(GetDlgItem(hDlg, ID_RULE_CFG_MAX), is_valid);
    EnableWindow(GetDlgItem(hDlg, ID_RULE_CFG_STEP), is_valid);
}

BOOL CALLBACK RuleCfgDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    TCHAR    info[MAX_FILE_PATH_LEN];
    int ret;
    t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);

     	switch (message)
     	{
     	case 	WM_INITDIALOG :
                center_child_win(hwnd_frame, hDlg);
                SetDlgItemText(hDlg, ID_RULE_CFG_BASE, gt_rule_data.min);
                SetDlgItemText(hDlg, ID_RULE_CFG_MAX, gt_rule_data.max);
                SetDlgItemText(hDlg, ID_RULE_CFG_STEP, gt_rule_data.step);

                rule_cfg_gui_switch(hDlg, gt_rule_data.valid);
          		return FALSE;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
                        GetDlgItemText(hDlg, ID_RULE_CFG_BASE
                            , gt_rule_data.min, sizeof(gt_rule_data.min));
                        
                        GetDlgItemText(hDlg, ID_RULE_CFG_MAX
                            , gt_rule_data.max, sizeof(gt_rule_data.max));
                        
                        GetDlgItemText(hDlg, ID_RULE_CFG_STEP
                            , gt_rule_data.step, sizeof(gt_rule_data.step));
                        
              		case 	IDCANCEL :
               				EndDialog (hDlg, LOWORD(wParam));
               				return TRUE ;

                    case 	ID_RULE_CFG_USE_RULE:
                    {
                        gt_rule_data.valid = button_checked(GetDlgItem(hDlg, ID_RULE_CFG_USE_RULE));
                        rule_cfg_gui_switch(hDlg, gt_rule_data.valid);
                    }
                }
      		break ;
            

     }


  	return FALSE ;
}

void update_check_sum_v4(t_stream *pt_stream)
{
    t_ip_hdr *iph=(void *)(pt_stream->eth_packet.payload);
    if (pt_stream->flags & CHECK_SUM_IP)
        ip_update_check(iph);

    if (ip_pkt_is_frag(&(pt_stream->eth_packet))) return;
    
    if (iph->protocol==IPPROTO_ICMP && (pt_stream->flags & CHECK_SUM_ICMP))
        icmp_igmp_update_check(iph);
    else if (iph->protocol==IPPROTO_IGMP && (pt_stream->flags & CHECK_SUM_IGMP))
        icmp_igmp_update_check(iph);
    else if (iph->protocol==IPPROTO_TCP && (pt_stream->flags & CHECK_SUM_TCP))
        tcp_update_check(iph);
    else if (iph->protocol==IPPROTO_UDP && (pt_stream->flags & CHECK_SUM_UDP))
        udp_update_check(iph);

}

void update_check_sum_v6(t_stream *pt_stream)
{
    t_ipv6_hdr *ip6h=(void *)(pt_stream->eth_packet.payload);

    if (ip_pkt_is_frag(&(pt_stream->eth_packet))) return;

    if (ip6h->nexthdr==IPPROTO_TCP && (pt_stream->flags & CHECK_SUM_TCP))
        tcp_update_check6(ip6h);
    else if (ip6h->nexthdr==IPPROTO_UDP && (pt_stream->flags & CHECK_SUM_UDP))
        udp_update_check6(ip6h);
    else if (ip6h->nexthdr==IPPROTO_ICMPV6 && (pt_stream->flags & CHECK_SUM_ICMP))
        icmp_update_check6(ip6h);

}

void update_check_sum(t_stream *pt_stream)
{
    if (ntohs(pt_stream->eth_packet.type)==ETH_P_IP)
        update_check_sum_v4(pt_stream);
   else if (ntohs(pt_stream->eth_packet.type)==ETH_P_IPV6)
       update_check_sum_v6(pt_stream);

}


void update_len_v4(t_stream *pt_stream)
{
    t_ip_hdr *iph=(void *)(pt_stream->eth_packet.payload);
    t_udp_hdr *udph=ip_data(iph);
    if (pt_stream->flags & IP_LEN)
        iph->tot_len = htons(pt_stream->len - sizeof(pt_stream->eth_packet));
    if (iph->protocol==IPPROTO_UDP && (pt_stream->flags & UDP_LEN))
        udph->len = htons(pt_stream->len - sizeof(pt_stream->eth_packet) - iph->ihl*4);

}

void update_len_v6(t_stream *pt_stream)
{
    t_ipv6_hdr *ip6h =(void *)(pt_stream->eth_packet.payload);
    t_udp_hdr *udph=ip6_data(ip6h);
    if (pt_stream->flags & IP_LEN)
        set_ip6_pkt_len(ip6h, pt_stream->len - sizeof(pt_stream->eth_packet));
    if (ip6h->nexthdr==IPPROTO_UDP && (pt_stream->flags & UDP_LEN))
        udph->len = htons(ip6_data_len(ip6h));

}

void update_len(t_stream *pt_stream)
{
    if (ntohs(pt_stream->eth_packet.type)==ETH_P_IP)
        update_len_v4(pt_stream);
   else if (ntohs(pt_stream->eth_packet.type)==ETH_P_IPV6)
       update_len_v6(pt_stream);
}

uint32_t  build_err_flags_v4(t_ether_packet *pt_eth, int len)
{
    uint32_t err_flags = 0;
    t_ip_hdr *iph = pt_eth->payload;

    if (len<MIN_PKT_LEN)
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }

    if (ip_pkt_len(iph)+14 > len)
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }

    if (ip_hdr_len(iph)<FIXED_IP_HDR_LEN || ip_hdr_len(iph)>ip_pkt_len(iph))
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }

   if (ip_checksum_wrong(iph))
    {
            err_flags |= ERR_IP_CHECKSUM;
            return err_flags; 
    }

    if (ip_pkt_is_frag(pt_eth))
    {
            return err_flags; 
    }

    if (iph->protocol==IPPROTO_TCP)
    {
        if (tcp_checksum_wrong(iph))
        {
            err_flags |= ERR_TCP_CHECKSUM;
            return err_flags; 
        }

    }
    else if (iph->protocol==IPPROTO_UDP)
    {
        t_udp_hdr *pt_udp_hdr = ip_data(iph);
        if (ntohs(pt_udp_hdr->len) != ip_data_len(iph))
        {
                err_flags |= ERR_PKT_LEN;
                return err_flags; 
        }

        if (udp_checksum_wrong(iph))
        {
            err_flags |= ERR_UDP_CHECKSUM;
            return err_flags; 
        }
    }
    else if (iph->protocol==IPPROTO_ICMP)
    {
        if (icmp_igmp_checksum_wrong(iph))
            err_flags |= ERR_ICMP_CHECKSUM;
    }
    else if (iph->protocol==IPPROTO_IGMP)
    {
        if (icmp_igmp_checksum_wrong(iph))
            err_flags |= ERR_IGMP_CHECKSUM;
    }

    return err_flags; 
}

uint32_t  build_err_flags_v6(t_ether_packet *pt_eth, int len)
{
    uint32_t err_flags = 0;
    t_ipv6_hdr *ip6h = pt_eth->payload;

    if (len<40)
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }

    if (ip6_pkt_len(ip6h)+14 > len)
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }



    if (ip_pkt_is_frag(pt_eth))
    {
            return err_flags; 
    }

    if (ip6h->nexthdr==IPPROTO_ICMPV6)
    {
        if (icmp_checksum_wrong6(ip6h))
        {
            err_flags |= ERR_ICMP_CHECKSUM;
            return err_flags; 
        }

    }

    else if (ip6h->nexthdr==IPPROTO_TCP)
    {
        if (tcp_checksum_wrong6(ip6h))
        {
            err_flags |= ERR_TCP_CHECKSUM;
            return err_flags; 
        }

    }
    else if (ip6h->nexthdr==IPPROTO_UDP)
    {
        t_udp_hdr *pt_udp_hdr = ip6_data(ip6h);
        if (ntohs(pt_udp_hdr->len) != ip6_data_len(ip6h))
        {
                err_flags |= ERR_PKT_LEN;
                return err_flags; 
        }

        if (udp_checksum_wrong6(ip6h))
        {
            err_flags |= ERR_UDP_CHECKSUM;
            return err_flags; 
        }
    }


    return err_flags; 
}

uint32_t  build_err_flags_arp(t_ether_packet *pt_eth, int len)
{
    uint32_t err_flags = 0;
    t_arp_hdr *pt_arp_hdr = pt_eth->payload;

    if (len<MIN_PKT_LEN)
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }
    
    if (len<(arp_pkt_len(pt_arp_hdr)+14))
    {
            err_flags |= ERR_PKT_LEN;
            return err_flags; 
    }
    
    return err_flags; 
}

uint32_t  build_err_flags(t_ether_packet *pt_eth, int len)
{
    uint32_t err_flags = 0;
    t_ip_hdr *iph = pt_eth->payload;

    if (ntohs(pt_eth->type)==ETH_P_ARP)
        return build_err_flags_arp(pt_eth, len);

    if (ntohs(pt_eth->type)==ETH_P_IP)
        return build_err_flags_v4(pt_eth, len);
        
    if (ntohs(pt_eth->type)==ETH_P_IPV6)
        return build_err_flags_v6(pt_eth, len);
        
        return err_flags; 

}

void update_stream(t_stream* pt_stream)
{
    update_len(pt_stream);
    update_check_sum(pt_stream);

}

void update_stream_from_dlg(HWND hDlg)
{
    t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);

    if (button_checked(GetDlgItem(hDlg, ID_SED_IP_CHECKSUM)))
        gt_edit_stream.flags |= CHECK_SUM_IP;
    else
        gt_edit_stream.flags &= ~(CHECK_SUM_IP);
        
    if (button_checked(GetDlgItem(hDlg, ID_SED_ICMP_CHECKSUM)))
        gt_edit_stream.flags |= CHECK_SUM_ICMP;
    else
        gt_edit_stream.flags &= ~(CHECK_SUM_ICMP);

    if (button_checked(GetDlgItem(hDlg, ID_SED_TCP_UDP_CHECKSUM)))
        gt_edit_stream.flags |= (CHECK_SUM_TCP|CHECK_SUM_UDP);
    else
        gt_edit_stream.flags &= ~(CHECK_SUM_TCP|CHECK_SUM_UDP);

    if (button_checked(GetDlgItem(hDlg, ID_SED_IP_LEN)))
        gt_edit_stream.flags |= IP_LEN;
    else
        gt_edit_stream.flags &= ~(IP_LEN);
        
    if (button_checked(GetDlgItem(hDlg, ID_SED_UDP_LEN)))
        gt_edit_stream.flags |= UDP_LEN;
    else
        gt_edit_stream.flags &= ~(UDP_LEN);
    
    if (ntohs(gt_edit_stream.eth_packet.type)!=ETH_P_IP
        &&
        ntohs(gt_edit_stream.eth_packet.type)!=ETH_P_IPV6)
        return; 

    update_stream(&gt_edit_stream);
    gt_edit_stream.err_flags = build_err_flags((void *)(gt_edit_stream.data), gt_edit_stream.len);
}

void add_stream(t_stream *pt_stream)
{
    g_apt_streams[nr_cur_stream] = alloc_stream();
    cpy_stream(g_apt_streams[nr_cur_stream], pt_stream);
    nr_cur_stream++;
    doc_modified=1;

}

void *alloc_stream()
{
    t_stream *pt_stream = malloc(sizeof(t_stream));
    pt_stream->err_flags=0;
    return pt_stream;
}

int make_frags_ipv4(const t_stream *pt_stream, int frag_num)
{
    int i, ret;
    t_ip_hdr *iph=(void *)(pt_stream->eth_packet.payload);
    void *p_ip_data=ip_data(iph);
    int data_len = ip_data_len(iph);
    int data_unit_num = data_len/8 + !!(data_len%8);
    int frag_data_unit_num = data_unit_num/frag_num 
        + !!(data_unit_num%frag_num);

    int frag_data_len = frag_data_unit_num*8;
    int frag_len = frag_data_len + ip_hdr_len(iph);
    int frag_frame_len = frag_len + sizeof(pt_stream->eth_packet);

    t_stream t_stream_tmp;
    t_ip_hdr *iph_frag=(void *)(t_stream_tmp.eth_packet.payload);
    void *p_frag_data=(void *)iph_frag + ip_hdr_len(iph);

    cpy_stream(&t_stream_tmp, pt_stream);
    ret=delete_all_rule(&t_stream_tmp);

    t_stream_tmp.len = frag_frame_len;
    iph_frag->tot_len = htons(frag_len);
    for (i=0; i<frag_num-1; i++)
    {
        iph_frag->frag_off =  ((1<<13) | frag_data_unit_num*i);
        iph_frag->frag_off = htons(iph_frag->frag_off);
        memcpy(p_frag_data, p_ip_data+frag_data_len*i, frag_data_len);
        update_check_sum(&t_stream_tmp);
        add_stream(&t_stream_tmp);
    }
    t_stream_tmp.len = pt_stream->len - i*frag_data_len;
    iph_frag->tot_len = htons(t_stream_tmp.len - sizeof(pt_stream->eth_packet));

    iph_frag->frag_off = htons(frag_data_unit_num*i);
    memcpy(p_frag_data, p_ip_data+frag_data_len*i, data_len-frag_data_len*i);
    update_check_sum(&t_stream_tmp);
    add_stream(&t_stream_tmp);
    return ret;
}

int make_frags_ipv6(const t_stream *pt_stream, int frag_num)
{
    int i, ret;
    t_ipv6_hdr *ip6h=(void *)(pt_stream->eth_packet.payload);
    void *p_ip_data=ip6_data(ip6h);
    int data_len = ip6_data_len(ip6h);
    int data_unit_num = data_len/8 + !!(data_len%8);
    int frag_data_unit_num = data_unit_num/frag_num 
        + !!(data_unit_num%frag_num);

    int frag_data_len = frag_data_unit_num*8;
    int frag_len = frag_data_len + 8 + IPV6_HDR_LEN;
    int frag_frame_len = frag_len + sizeof(pt_stream->eth_packet);

    t_stream t_stream_tmp;
    t_ipv6_hdr *ip6h_frag=(void *)(t_stream_tmp.eth_packet.payload);
    t_ipv6_frag_hdr t_ipv6_frag_hdr_tmp={ip6h->nexthdr, 0, 0, 2013};
    void *p_frag_data= (void *)ip6h_frag + IPV6_HDR_LEN;

    cpy_stream(&t_stream_tmp, pt_stream);
    ret=delete_all_rule(&t_stream_tmp);

    t_stream_tmp.len = frag_frame_len;
    ip6h_frag->nexthdr=IPPROTO_FRAGMENT;
    ip6h_frag->payload_len= htons(frag_frame_len-sizeof(pt_stream->eth_packet)-IPV6_HDR_LEN);
    for (i=0; i<frag_num-1; i++)
    {
        t_ipv6_frag_hdr_tmp.frag_off =  (((frag_data_unit_num*i)<<3) | 1);
        t_ipv6_frag_hdr_tmp.frag_off = htons(t_ipv6_frag_hdr_tmp.frag_off);
        memcpy(p_frag_data, &t_ipv6_frag_hdr_tmp, 8);
        memcpy(p_frag_data+8, p_ip_data+frag_data_len*i, frag_data_len);
        update_check_sum(&t_stream_tmp);
        add_stream(&t_stream_tmp);
    }

    t_stream_tmp.len = 8 + pt_stream->len - i*frag_data_len;
    ip6h_frag->payload_len = htons(t_stream_tmp.len - sizeof(pt_stream->eth_packet) - IPV6_HDR_LEN);

    t_ipv6_frag_hdr_tmp.frag_off = htons((frag_data_unit_num*i)<<3);
    memcpy(p_frag_data, &t_ipv6_frag_hdr_tmp, 8);
    memcpy(p_frag_data+8, p_ip_data+frag_data_len*i, data_len-frag_data_len*i);
    update_check_sum(&t_stream_tmp);
    add_stream(&t_stream_tmp);
    return ret;
}

int make_frags(const t_stream *pt_stream, int frag_num)
{
    t_ether_packet *pt_eth_hdr = (void *)(pt_stream->data);
    if (ntohs(pt_eth_hdr->type)==ETH_P_IP)
        return make_frags_ipv4(pt_stream, frag_num);
    else if (ntohs(pt_eth_hdr->type)==ETH_P_IPV6)
        return make_frags_ipv6(pt_stream, frag_num);

    return 0;
}
void init_ui_stream_edit(HWND hDlg)
{
    RECT rect0, rect1, rect2;

    HWND hwnd_tree=GetDlgItem(hDlg,ID_SED_TREE_VIEW);
    HWND hwnd_hexedit=GetDlgItem(hDlg,ID_SED_HEX_EDIT);
    HWND hwnd_button = GetDlgItem(hDlg,ID_SED_DYNAMIC_RULE_BUTTON);
    HWND hwnd_edit = GetDlgItem(hDlg,ID_SED_DYNAMIC_EDIT);
    HWND hwnd_comb = GetDlgItem(hDlg,ID_SED_DYNAMIC_COMB);

   SendMessage(hwnd_tree, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 
   SendMessage(hwnd_edit, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 
   SendMessage(hwnd_button, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 
   SendMessage(hwnd_comb, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 

    change_child_size(hwnd_tree,cxChar*80, cyChar*10);
    move_child_a2b_bottom_left(hwnd_tree, GetDlgItem(hDlg,ID_SED_UPDATE_LEN), 10);

    change_child_size(hwnd_hexedit,cxChar*80, cyChar*10);
    move_child_a2b_bottom_left(hwnd_hexedit, hwnd_tree, 10);

move_child_a2b_bottom_right(GetDlgItem(hDlg,IDCANCEL), hwnd_hexedit, 10);
move_child_a2b_left_top(GetDlgItem(hDlg,IDOK), GetDlgItem(hDlg,IDCANCEL), 10);

    if (a_include_b_right(hwnd_hexedit, GetDlgItem(hDlg,ID_SED_TCP_UDP_CHECKSUM)))
        right_include_child(hwnd_hexedit, 10);
    else
        right_include_child(GetDlgItem(hDlg,ID_SED_TCP_UDP_CHECKSUM), 10);
    bottom_include_child(GetDlgItem(hDlg,IDOK), 10);

}

BOOL CALLBACK StreamEditDlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    HTREEITEM htvi;
    TVITEM tvi;
    HWND hwnd_tree=GetDlgItem(hDlg,ID_SED_TREE_VIEW);
    HWND hwnd_hex_edit=GetDlgItem(hDlg,ID_SED_HEX_EDIT);
    TCHAR    info[32];
    int ret;

     	switch (message)
     	{
     	case 	WM_INITDIALOG :
                init_ui_stream_edit(hDlg);
                center_child_win(hwnd_frame, hDlg);

                old_tv_proc = (WNDPROC) SetWindowLong (hwnd_tree, 
                                 	GWL_WNDPROC, (LONG) my_tv_proc) ;
                hide_edit_ui(hDlg);
                
                InitDlgFromStream(hDlg, &gt_edit_stream);
                init_protocol_comb(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB), 0);


          		return FALSE;

      case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :

            if (HIWORD(wParam)==CBN_SELCHANGE)
            {
                if (LOWORD(wParam)==ID_SED_DYNAMIC_COMB)
                {
                    int new_proto=SendMessage(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB)
                            , (UINT) CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

                    if (ETH_P_IP==ntohs(gt_edit_stream.eth_packet.type))
                    {
                        t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);
                        if (iph->protocol!=new_proto)
                        {
                            iph->protocol=new_proto;
                            goto PROTO_CHNG_PROC;
                        }
                    }
                    else if (ETH_P_IPV6==ntohs(gt_edit_stream.eth_packet.type))
                    {
                        t_ipv6_hdr *ip6h=(void *)(gt_edit_stream.eth_packet.payload);
                        if (ip6h->nexthdr!=new_proto)
                        {
                            ip6h->nexthdr=new_proto;
                            goto PROTO_CHNG_PROC;
                        }
                    }
                   	return TRUE ;
                    

                }
                else
                {
                    int type=get_eth_type_comb(GetDlgItem(hDlg, ID_SED_DYNAMIC_COMB_ETH));
                    if (type!=ntohs(gt_edit_stream.eth_packet.type))
                    {
                        gt_edit_stream.eth_packet.type = htons(type);
                        goto PROTO_CHNG_PROC;
                    }
                    return TRUE ;

                }

                PROTO_CHNG_PROC:
                hide_edit_ui(hDlg);
                SendMessage(hDlg, WM_COMMAND, ID_SED_UPDATE_NOW, 0);
                show_tip("字段变化规则已经删除，因为报文结构有变化");
                return TRUE ;

            }

                if(wParam==IDOK)
                {
                    if (GetFocus()==GetDlgItem(hDlg, ID_SED_LEN))
                    {
                        SetFocus(hDlg);
                        return TRUE;
                    }

                    if (GetFocus()==GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT))
                    {
                        SetFocus(GetDlgItem(hDlg, ID_SED_TREE_VIEW));
                        return TRUE;
                    }

                }
                
          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
                    {
                        doc_modified=1;
                        update_stream_from_dlg(hDlg);
                    }

              		case 	IDCANCEL :
               				EndDialog (hDlg, LOWORD(wParam)) ;
               				return TRUE ;

                    case    ID_SED_LEN:
                    {
                   		if (HIWORD(wParam)==EN_KILLFOCUS)
                        {
                            char info[32];
                            GetDlgItemText(hDlg, ID_SED_LEN, info, sizeof(info));
                            gt_edit_stream.len=strtol(info+0,NULL,10);
                            update_stream_from_dlg(hDlg);
                            update_tv(hwnd_tree);
                            hex_win_reinit(hwnd_hex_edit);

                       	    return TRUE ;
                        }
                        break;
                    }

                    case    ID_SED_NAME:
                    {
                   		if (HIWORD(wParam)==EN_KILLFOCUS)
                        {
                            char info[32];
                            GetDlgItemText(hDlg, ID_SED_NAME, info, sizeof(info));
                            strcpy(gt_edit_stream.name, info);
                       	    return TRUE ;
                        }
                        break;
                    }

                    case    ID_SED_DYNAMIC_EDIT:
                    {
                   		if (HIWORD(wParam)==EN_KILLFOCUS)
                        {
                            update_data_from_edit(hDlg, hwnd_tree
                                ,Selected
                                , GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT)
                                , 0);
                       	    return TRUE ;
                        }
                        break;
                    }

                    case    ID_SED_DYNAMIC_RULE_BUTTON:
                    {
                        HTREEITEM tmp_Selected = Selected;
                        init_rule_data(hwnd_tree, tmp_Selected);
                        ret=DialogBox(g_hInstance, TEXT("RULE_CFG_DLG"), hDlg, RuleCfgDlgProc);
                        if (IDCANCEL==ret) return TRUE ;
                        update_from_rule_data(hwnd_tree, tmp_Selected);
                       	return TRUE ;
                    }

                    case    ID_SED_HEX_EDIT:
                    {
                   		if (HIWORD(wParam)==EN_KILLFOCUS && stream_changed)
                        {
                            SendMessage(hDlg, WM_COMMAND, ID_SED_UPDATE_NOW, 0);
                            show_tip("字段变化规则已经删除，因为直接编辑了缓冲区");
                            stream_changed = 0;
                            return TRUE ;
                        }
                        break;
                    }

                    case    ID_SED_UPDATE_NOW:
                    {
                        delete_all_rule(&gt_edit_stream);
                        update_stream_from_dlg(hDlg);
                        build_tv(hwnd_tree);
                        hex_win_reinit(hwnd_hex_edit);
                        //resize_window(hwnd_hex_edit);
                        //InvalidateRect(hwnd_hex_edit, NULL, TRUE) ;
                        //hex_win_sel(hwnd_hex_edit, 0, 0);

                       	return TRUE ;
                    }

              }
      		break ;

case WM_NOTIFY:
{
   switch(LOWORD(wParam))
   { 
     case ID_SED_TREE_VIEW:
     if (lParam==NULL ||
        ((LPNMHDR)lParam)->code == TVN_ITEMEXPANDED)
    {
            hide_edit_ui(hDlg);
            return TRUE;
    }
     else if(((LPNMHDR)lParam)->code == TVN_SELCHANGED) 
     {
        //if (IsWindowVisible(GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT)))
        hide_edit_ui(hDlg);
        
        htvi=(HTREEITEM)TreeView_GetSelection(hwnd_tree);

        if (htvi_edit_able(hwnd_tree, htvi))
        {
            Selected=htvi;
            show_edit_ui_for_tvi(hDlg, hwnd_tree, Selected);
        }
        else
            htvi_sel_hex(GetDlgItem(hDlg,ID_SED_HEX_EDIT), hwnd_tree, htvi);

        return TRUE;
     }
   }
break;

}

    case WM_LBUTTONDOWN:
    {
        SetFocus(hDlg);
        return TRUE;
    }
    
    case WM_CTLCOLOREDIT:
    {
        if (lParam==GetDlgItem(hDlg, ID_SED_DYNAMIC_EDIT))
        {
            HDC hdc = (HDC)wParam;
            SetBkMode(hdc, TRANSPARENT);
            return (INT_PTR)GetSysColorBrush(COLOR_INFOBK);
        }
        break;
    }

    case STREAM_EDIT_DATA_CHANGE:
    {
        stream_changed=1;
        return TRUE;
    }
        

     }
  	return FALSE ;
}


TCHAR *pkt_view_lv_col_names[] = 
{
    TEXT("索引"),
    TEXT("时间"),
    TEXT("源地址"),
    TEXT("目的地址"),
    TEXT("协议"),
    TEXT("长度"),
    TEXT("信息"),
};

BOOL InitLvColumns(HWND hWndListView, int *col_width) 
{ 
    TCHAR szText[256];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol, col_num = ARRAY_SIZE(pkt_view_lv_col_names);

int lv_width = win_width(hWndListView);

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < col_num; iCol++)
    {
        lvc.iSubItem = iCol;
        lvc.pszText = pkt_view_lv_col_names[iCol];
        lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.

        if (iCol!= (col_num-1))
        {
            lvc.cx = col_width[iCol]; 
            lv_width -= lvc.cx;
        }
        else
        {
            lvc.cx = lv_width-35; 
        }

        // Insert the columns into the list view.
        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
        {
        MessageBox (NULL, TEXT ("ListView_InsertColumn"),
          szAppName, MB_ICONERROR) ;
            return FALSE;
        }
    }

    return TRUE;
} 

BOOL InsertItemFromPkt(HWND hWndListView, t_dump_pkt *pt_pkt, struct timeval *base)
{
    LVITEM lvI;
    int index=ListView_GetItemCount(hWndListView);
    int iCol;
    TCHAR    info[128];
    struct tm ltime;
    char timestr[32];
    time_t local_tv_sec;
    struct timeval tmp=pt_pkt->header.ts;
    t_ether_packet *pt_eth_hdr = pt_pkt->pkt_data;

    // Initialize LVITEM members that are different for each item.
    {
        lvI.iItem  = index;
        lvI.iSubItem  = 0;
    
        lvI.mask      = LVIF_TEXT|LVIF_PARAM;

        sprintf(info, "%d", index+1);

        lvI.pszText   = info;
        lvI.lParam = (unsigned long)(void *)pt_pkt;

        // Insert items into the list.
        if (ListView_InsertItem(hWndListView, &lvI) == -1)
            return FALSE;

    local_tv_sec = pt_pkt->header.ts.tv_sec;
    
    strftime( timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&local_tv_sec));
    #if 0
        sprintf(info, "%s.%.06d"
            ,timestr,  pt_pkt->header.ts.tv_usec);
    #else
        tmp.tv_sec-=base->tv_sec;
        if (tmp.tv_usec<base->tv_usec)
        {
            tmp.tv_sec--;
            tmp.tv_usec+=1000000;

        }
        tmp.tv_usec-=base->tv_usec;
        sprintf(info, "%u.%.06u"
            ,(unsigned)(tmp.tv_sec),  (unsigned)tmp.tv_usec);
    #endif
        ListView_SetItemText(hWndListView, index, 1, info);

        get_src_addr(info, pt_eth_hdr);
        ListView_SetItemText(hWndListView, index, 2, info);

        get_dst_addr(info, pt_eth_hdr);
        ListView_SetItemText(hWndListView, index, 3, info);

        get_proto_name(info, pt_eth_hdr);
        ListView_SetItemText(hWndListView, index, 4, info);





        sprintf(info, "%d", pt_pkt->header.caplen);
        ListView_SetItemText(hWndListView, index, 5, info);

        get_pkt_desc_info(info, pt_pkt->pkt_data, pt_pkt->err_flags);
        ListView_SetItemText(hWndListView, index, 6, info);

    }

    return TRUE;
}


int init_lv_from_dump_file(HWND hlv, char *file_path)
{
pcap_t *fp;
char errbuf[PCAP_ERRBUF_SIZE];
char source[PCAP_BUF_SIZE];
struct pcap_pkthdr *header;
const u_char *pkt_data;
t_dump_pkt    *pt_pkt;
struct timeval 	base={(time_t)0};
    /* Create the source string according to the new WinPcap syntax */
    if ( pcap_createsrcstr( source,         // variable that will keep the source string
                            PCAP_SRC_FILE,  // we want to open a file
                            NULL,           // remote host
                            NULL,           // port on the remote host
                            file_path,        // name of the file we want to open
                            errbuf          // error buffer
                            ) != 0)
    {
        WinPrintf(GetParent(hlv), "Error creating a source string");
        return -1;
    }
    
    /* Open the capture file */
    if ( (fp= pcap_open(source,         // name of the device
                        65536,          // portion of the packet to capture
                                        // 65536 guarantees that the whole packet will be captured on all the link layers
                         PCAP_OPENFLAG_PROMISCUOUS,     // promiscuous mode
                         1000,              // read timeout
                         NULL,              // authentication on the remote machine
                         errbuf         // error buffer
                         ) ) == NULL)
    {
        WinPrintf(GetParent(hlv), "打开文件 %s 失败\n", source);
        return -1;
    }

        while (pcap_next_ex( fp, &header, &pkt_data)>0)
        {
            pt_pkt = malloc(sizeof(t_dump_pkt)+header->caplen);
            memcpy(pt_pkt, header, sizeof(struct pcap_pkthdr));
            memcpy(pt_pkt->pkt_data, pkt_data, header->caplen);
            if (base.tv_sec==(time_t)0) base=header->ts;
            pt_pkt->err_flags = build_err_flags((void *)(pt_pkt->pkt_data), header->caplen);
            InsertItemFromPkt(hlv, pt_pkt, &base);
        }

    return 0;
}

char pcap_file_to_view[MAX_FILE_PATH_LEN];
int  cur_view_pkt_idx;

void *get_lvi_lparam(HWND hlv, int idx)
{
    LVITEM lvI;
    lvI.iItem  = idx;
    lvI.iSubItem  = 0;
    lvI.mask      = LVIF_PARAM;
    ListView_GetItem(hlv,  &lvI);
    return (void *)(unsigned long)(lvI.lParam);

}

void rel_sel_pkt(HWND hList)
{
  int i, n;
  t_dump_pkt    *pt_pkt;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (ListView_GetCheckState(hList, i))
        {
          pt_pkt=get_lvi_lparam(hList, i);
          free(pt_pkt);
        }
    }

  return 0;

}

void sel_pkt_to_stream(HWND hList)
{
  int i, n;
  t_dump_pkt    *pt_pkt;
  t_stream t_stream_tmp;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (ListView_GetCheckState(hList, i))
        {
            if (nr_cur_stream>=MAX_STREAM_NUM) break;
            pt_pkt=get_lvi_lparam(hList, i);
            init_stream(&t_stream_tmp);
            t_stream_tmp.len=pt_pkt->header.caplen;
            memcpy(t_stream_tmp.data, pt_pkt->pkt_data, pt_pkt->header.caplen);
            t_stream_tmp.err_flags = pt_pkt->err_flags;
            add_stream(&t_stream_tmp);
            re_populate_items();
        }
    }

  return 0;

}

void sel_pkt_to_pcap_dump(HWND hList, char *dump_file)
{
  int i, n;
  t_dump_pkt    *pt_pkt;
  
  	pcap_t *fp;
    pcap_t *fp_rcv;
	char errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *header;
const u_char *pkt_data;
int saved_cnt=0;
pcap_dumper_t *dumpfile;
    
	/* Open the adapter */
	if ((fp = pcap_open_offline(pcap_file_to_view, errbuf)) == NULL)
	{
		WinPrintf(GetParent(hList), TEXT("Unable to open the adapter. %s is not supported by WinPcap"), pcap_file_to_view);
		return 2;
	}

dumpfile = pcap_dump_open(fp, dump_file);
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (ListView_GetCheckState(hList, i))
        {
            pt_pkt=get_lvi_lparam(hList, i);
            pcap_dump(dumpfile, &(pt_pkt->header), pt_pkt->pkt_data);
        }
    }

    pcap_dump_close(dumpfile);
	pcap_close(fp);	

  return 0;

}

void init_ui_pkt_view(HWND hDlg)
{
    RECT rect0, rect1, rect2;

    HWND hlv=GetDlgItem(hDlg,ID_VIEW_STREAM_LV);
    HWND hwnd_tree=GetDlgItem(hDlg,ID_VIEW_STREAM_TREE_VIEW);
    HWND hwnd_hexedit=GetDlgItem(hDlg,ID_VIEW_STREAM_HEX_EDIT);
    int lv_width=cxChar*120, lv_height=cxChar*28;
    int col_width[] = {cxChar*10, cxChar*15, cxChar*20, cxChar*20, cxChar*9, cxChar*7, cxChar*34};

    SendMessage(hwnd_tree, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 
    SendMessage(hlv, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 
    ListView_SetExtendedListViewStyle(hlv, LVS_EX_CHECKBOXES|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

    change_child_size(hlv, lv_width, lv_height);
    move_child_pos(hlv, 10, 10);

    InitLvColumns(hlv, col_width);

    change_child_size(hwnd_tree,cxChar*40, cyChar*16);
    move_child_a2b_bottom_left(hwnd_tree, hlv, 10);

    change_child_size(hwnd_hexedit,cxChar*80, cyChar*16);
    move_child_a2b_right_top(hwnd_hexedit, hwnd_tree, 10);

    if (a_include_b_right(hwnd_hexedit, hlv))
        right_include_child(hwnd_hexedit, 10);
    else
        right_include_child(hlv, 10);
    bottom_include_child(hwnd_hexedit, 10);

}

BOOL CALLBACK PktViewDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    HTREEITEM htvi;
    TVITEM tvi={0};
    t_tvi_data *pt_tvi_data;
    HWND hlv=GetDlgItem(hDlg,ID_VIEW_STREAM_LV);
    HWND hwnd_tree=GetDlgItem(hDlg,ID_VIEW_STREAM_TREE_VIEW);
    TCHAR    info[32];
    int ret;

    static HMENU	hMenu ;
    POINT point ;

    t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);
    t_dump_pkt    *pt_pkt;

     	switch (message)
     	{
     	case 	WM_INITDIALOG :
                init_ui_pkt_view(hDlg);
                center_child_win(hwnd_frame, hDlg);
                SetFocus(GetDlgItem(hDlg,ID_VIEW_STREAM_TREE_VIEW));
            old_lv_proc = (WNDPROC) SetWindowLong (hlv, 
                                 	GWL_WNDPROC, (LONG)my_lv_proc) ;

                if (init_lv_from_dump_file(hlv, pcap_file_to_view))
                {
                    SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
                    return FALSE;
                }
                gt_edit_stream.len=0;
                hex_win_reinit(GetDlgItem(hDlg,ID_VIEW_STREAM_HEX_EDIT));
                cur_view_pkt_idx=-1;
                is_read_only=1;

            hMenu = LoadMenu (g_hInstance, TEXT("pkt_view_popup_menu")) ;
            hMenu = GetSubMenu (hMenu, 0) ;
            add_tip(hwndTip, hlv, TEXT("点击鼠标右键进行操作"));
          		return FALSE;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
                if(wParam==IDOK)
                {
                    if (GetFocus()!=GetDlgItem(hDlg, IDOK))
                    {
                        return TRUE ;
                    }

                }

          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
                    {
                        ;

                    }

              		case 	IDCANCEL :
                            SelAll(hlv);
                            rel_sel_pkt(hlv);

               				EndDialog (hDlg, LOWORD(wParam));
                            is_read_only=0;
               				return TRUE ;

                    case ID_VIEW_STREAM_SEL_ALL:
                        SelAll(hlv);
                        return TRUE ;
                        
                    case ID_VIEW_STREAM_SEL_RSV:
                        SelRvs(hlv);
                        return TRUE ;
                                
                    case ID_VIEW_STREAM_DEL_SEL:
                        rel_sel_pkt(hlv);
                        DelSel(hlv);
                        cur_view_pkt_idx=-1;

                        return TRUE ;
                        
                    case ID_VIEW_STREAM_SEL2STREAM:
                        sel_pkt_to_stream(hlv);
                        return TRUE ;

                    case ID_VIEW_STREAM_SAVE_AS:
                    {
                        char info[1024];
                        char file_name[MAX_FILE_PATH_LEN];
                        ret=get_save_file_name(file_name, hDlg, "pcap file(*.pcap)\0*.pcap\0\0", "pcap");
                        if (ret) return TRUE;

                        sel_pkt_to_pcap_dump(hlv, file_name);
                        return TRUE ;
                    }



              }
      		break ;
            
        case WM_NOTIFY:

    {
       switch(LOWORD(wParam))
       { 
         case ID_VIEW_STREAM_LV:

         {
            NMHDR *pt_nmhdr=(void *)lParam;
            int iItem = ListView_GetNextItem(hlv, -1, LVNI_SELECTED);

            if (iItem>=0 && iItem!=cur_view_pkt_idx) 
            {
                cur_view_pkt_idx=iItem;
                pt_pkt=get_lvi_lparam(hlv, cur_view_pkt_idx);
                gt_edit_stream.len=pt_pkt->header.caplen;
                memcpy(gt_edit_stream.data, pt_pkt->pkt_data, gt_edit_stream.len);
                build_tv(GetDlgItem(hDlg,ID_VIEW_STREAM_TREE_VIEW));
                hex_win_reinit(GetDlgItem(hDlg,ID_VIEW_STREAM_HEX_EDIT));

            }

            if (pt_nmhdr->code == NM_RCLICK)
            {
                point = ((LPNMITEMACTIVATE)lParam)->ptAction;
                ClientToScreen (hlv, &point) ;
          		TrackPopupMenu (hMenu, TPM_LEFTBUTTON, point.x, point.y, 0, hDlg, NULL) ;
                return 0;

            }

                return TRUE;
         }
        
        break;

     case ID_VIEW_STREAM_TREE_VIEW:
         if(((LPNMHDR)lParam)->code == TVN_SELCHANGED) 
         {
            
            htvi=(HTREEITEM)TreeView_GetSelection(hwnd_tree);

            if (htvi_edit_able(hwnd_tree, htvi))
            {
                tvi.hItem = htvi;
                tvi.mask=TVIF_PARAM;
                TreeView_GetItem(hwnd_tree, &tvi);
                pt_tvi_data = tvi.lParam;
                hex_win_sel(GetDlgItem(hDlg, ID_VIEW_STREAM_HEX_EDIT), pt_tvi_data->data_offset, pt_tvi_data->len);
            }
            else
                htvi_sel_hex(GetDlgItem(hDlg, ID_VIEW_STREAM_HEX_EDIT), hwnd_tree, htvi);
            return TRUE;
         }

         break;


       }

       break;
    }

        case 	WM_INITMENUPOPUP:
        {
            int sel_cnt=GetSelCnt(hlv);
            int item_cnt=ListView_GetItemCount(hlv);
            if (lParam == 0)
            {
                EnableMenuItem ((HMENU) wParam, ID_VIEW_STREAM_SEL_ALL, item_cnt ? MF_ENABLED : MF_GRAYED);     EnableMenuItem ((HMENU) wParam, ID_VIEW_STREAM_SEL_RSV, item_cnt ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, ID_VIEW_STREAM_DEL_SEL, sel_cnt ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, ID_VIEW_STREAM_SEL2STREAM, sel_cnt ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, ID_VIEW_STREAM_SAVE_AS, sel_cnt ? MF_ENABLED : MF_GRAYED);


                return 0;

            }

            break;
        }
     }


  	return FALSE ;
}


HWND hPktCapDlg;
void update_pkt_cap_stats_ip(t_ether_packet *pt_pkt)
{
    t_ip_hdr *iph=(void *)(pt_pkt->payload);

    switch (iph->protocol)
    {
        case IPPROTO_ICMP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_ICMP), 1);
            break;

        case IPPROTO_UDP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_UDP), 1);
            break;
            
        case IPPROTO_TCP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_TCP), 1);
            break;

        default:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_OTHER), 1);

    }
}

void update_pkt_cap_stats_ip6(t_ether_packet *pt_pkt)
{
    t_ipv6_hdr *ip6h=(void *)(pt_pkt->payload);

    switch (ip6h->nexthdr)
    {
        case IPPROTO_ICMPV6:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_ICMP6), 1);
            break;

        case IPPROTO_UDP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_UDP6), 1);
            break;
            
        case IPPROTO_TCP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_TCP6), 1);
            break;

        default:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_OTHER6), 1);

    }
}


void update_pkt_cap_stats(t_ether_packet *pt_pkt)
{
    inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_TOTAL), 1);

    switch (ntohs(pt_pkt->type))
    {
        case ETH_P_IP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_IP), 1);
            update_pkt_cap_stats_ip(pt_pkt);
            break;
        case ETH_P_IPV6:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_IPv6), 1);
            update_pkt_cap_stats_ip6(pt_pkt);
            break;


        case ETH_P_ARP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_L2), 1);
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_ARP), 1);
            break;

        case ETH_P_RARP:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_L2), 1);
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_RARP), 1);
            break;

        default:
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_L2), 1);
            inc_int_text(GetDlgItem(hPktCapDlg, ID_PKT_CAP_OTHER_L2), 1);

    }
}

void clr_cap_dlg_stat(HWND hDlg)
{
    int i;
    for (i=ID_PKT_CAP_TOTAL; i<=ID_PKT_CAP_OTHER6; i++)
    set_int_text(GetDlgItem(hDlg, i), 0);
}

BOOL CALLBACK PktCapDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    TCHAR    info[MAX_FILE_PATH_LEN];
    int ret;
    t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);
    t_dump_pkt    *pt_pkt;

     	switch (message)
     	{
     	case 	WM_INITDIALOG :
                hPktCapDlg=hDlg;
                center_child_win(hwnd_frame, hDlg);
                delete_file_f(PKT_CAP_FILE_ONLY_CAP);
                //init_pkt_cap_info(hDlg, info);
          		return FALSE;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
              		case 	IDCANCEL :
                            if (!cap_stopped)
                                SendMessage(hDlg, WM_COMMAND, ID_PKT_CAP_CAP, 0);
               				EndDialog (hDlg, LOWORD(wParam));
               				return TRUE ;

              		case 	ID_PKT_CAP_CFG:
                            if (!cap_stopped)
                                SendMessage(hDlg, WM_COMMAND, ID_PKT_CAP_CAP, 0);

               				DialogBox(g_hInstance, TEXT("PKT_CAP_CFG_DLG"), hDlg, PktCapCfgDlgProc);
                            //init_pkt_cap_info(hDlg, info);
               				return TRUE ;

              		case 	ID_PKT_CAP_CAP:
                        if (cap_stopped)
                        {
                            if (0==get_int_text(GetDlgItem(hDlg, ID_PKT_CAP_TOTAL))) 
                                goto CLR_STAT;
                            ret=AskConfirmation_3state(hDlg, TEXT("即将清除已有的抓包。\r\n是否保存?"), szAppName);
                            if (IDCANCEL == ret) return TRUE;
                            
                            if (IDYES == ret)
                            {
                                char file_name[MAX_FILE_PATH_LEN];
                                file_name[0]=0;
                                if (0==get_save_file_name(file_name, hDlg, "pcap file(*.pcap)\0*.pcap\0\0", "pcap"))
                                CopyFile(PKT_CAP_FILE_ONLY_CAP,  file_name,  FALSE);
                            }
CLR_STAT:
                            clr_cap_dlg_stat(hDlg);
                            SetDlgItemText(hDlg, ID_PKT_CAP_CAP, "停止");
                            need_cap_stop=0;
                            cap_stopped=0;
                            launch_thread(rcv_pkt_2, NULL);
                        }
                        else
                        {
                            need_cap_stop=1;
                            while (cap_stopped==0) Sleep(1);
                            SetDlgItemText(hDlg, ID_PKT_CAP_CAP, "抓包");
                        }
               				return TRUE ;
                            
              		case 	ID_PKT_CAP_VIEW_RESULT:
                            if (!cap_stopped)
                                SendMessage(hDlg, WM_COMMAND, ID_PKT_CAP_CAP, 0);

                            if (0==get_int_text(GetDlgItem(hDlg, ID_PKT_CAP_TOTAL)) || 
                                !file_exists(PKT_CAP_FILE_ONLY_CAP))
                            {
                                WinPrintf(hDlg, TEXT("暂无数据"));
                                return 0;
                            }

                            strcpy(pcap_file_to_view, PKT_CAP_FILE_ONLY_CAP);
                            ShowWindow(hDlg, 0);
                            DialogBox(g_hInstance, TEXT("PKT_VIEW_DLG"), hDlg, PktViewDlgProc);
                            ShowWindow(hDlg, 1);
               				return TRUE ;


              }
      		break ;
            

     }


  	return FALSE ;
}




BOOL CALLBACK FragDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    HTREEITEM htvi;
    TVITEM tvi={0};
    t_tvi_data *pt_tvi_data;
    HWND hlv=GetDlgItem(hDlg,ID_VIEW_STREAM_LV);
    HWND hwnd_tree=GetDlgItem(hDlg,ID_VIEW_STREAM_TREE_VIEW);
    TCHAR    info[32];
    int ret;
    t_ip_hdr *iph=(void *)(gt_edit_stream.eth_packet.payload);
    t_dump_pkt    *pt_pkt;

     	switch (message)
     	{
         	case 	WM_INITDIALOG :
                    center_child_win(hwnd_frame, hDlg);
              		return FALSE;

            case 	WM_CLOSE:
       				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
       				return TRUE ;

         	case 	WM_COMMAND :
          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
                    {
                        int frag_num=0;
                        frag_num=get_int_text(GetDlgItem(hDlg, ID_FRAG_NUM));

                        if (frag_num<=1)
                        {
                            WinPrintf(hDlg, "输入值 %d 非法。请输入大于1的整数", frag_num);
                            return TRUE;
                        }
                        if (nr_cur_stream+frag_num>MAX_STREAM_NUM)
                        {
                            WinPrintf(hDlg, "分片后，将超过支持的最大流数 %d。请减小分片数，或删除部分流", MAX_STREAM_NUM);
                            return TRUE;
                        }
                    
                        ret=make_frags(g_apt_streams[GetIndex(hwnd_lv)], frag_num);
                        re_populate_items();
                        if (ret) show_tip("注意，字段变化规则已经从分片报文中删除");
                    }

              		case 	IDCANCEL :
               				EndDialog (hDlg, LOWORD(wParam));
               				return TRUE ;
              }
      		break ;
     }


  	return FALSE ;
}


