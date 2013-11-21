/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef __COMMON_H_
#define __COMMON_H_

#include <tchar.h> 
#include <commctrl.h>
#include <windows.h>
#include <windowsx.h>
#include <stdint.h>
#include "defs.h"

#define    ARRAY_SIZE(array_name)    (sizeof(array_name)/sizeof(array_name[0]))
static inline int AskConfirmation(HWND hwnd, TCHAR *str, TCHAR *cap_str)
{
    return MessageBox(hwnd
                     ,str
                     ,cap_str
                     ,MB_YESNO | MB_ICONQUESTION) ;
}

static inline int AskConfirmation_3state(HWND hwnd, TCHAR *str, TCHAR *cap_str)
{
    return MessageBox(hwnd
                     ,str
                     ,cap_str
                     ,MB_YESNOCANCEL | MB_ICONQUESTION) ;
}

int WinPrintf(HWND hwnd, TCHAR * szFormat, ...);
static inline int win_width(HWND hwnd)
{

    RECT		rect;
    GetWindowRect (hwnd, &rect);
    return rect.right - rect.left;
}

static inline int win_height(HWND hwnd)
{

    RECT		rect;
    GetWindowRect (hwnd, &rect);
    return rect.bottom - rect.top;
}

static inline int win_left(HWND hwnd)
{

    RECT		rect;
    GetWindowRect (hwnd, &rect);
    return rect.left;
}

static inline int win_top(HWND hwnd)
{

    RECT		rect;
    GetWindowRect (hwnd, &rect);
    return rect.top;
}

static inline int win_bottom(HWND hwnd)
{

    RECT		rect;
    GetWindowRect (hwnd, &rect);
    return rect.bottom;
}

static inline void get_relative_pos(RECT *rect1, RECT *rect2, RECT *rect3)
{
    RECT rect_tmp;
    RECT* rect=&rect_tmp;
    rect->left=rect2->left - rect1->left;
    rect->right=rect2->right - rect1->left;
    rect->top=rect2->top - rect1->top;
    rect->bottom=rect2->bottom - rect1->top;
    *rect3 = rect_tmp;
}

static inline void get_relative_pos_self(RECT *rect1, RECT *rect)
{

    rect->right=rect1->right - rect1->left;
    rect->left=0;
    rect->bottom=rect1->bottom - rect1->top;
    rect->top=0;

}

void center_child_win(HWND hparent, HWND hchild);

static inline void get_client_rect_screen(HWND hparent, RECT *rect)
{

    RECT		rect_tmp;
    POINT Point = {0, 0};
    ClientToScreen(hparent, &Point);


    GetClientRect (hparent, &rect_tmp);
    rect_tmp.left = Point.x;
    rect_tmp.right += Point.x;
    rect_tmp.top = Point.y;
    rect_tmp.bottom += Point.y;
    *rect = rect_tmp;
    
}

static inline HANDLE launch_thread(LPTHREAD_START_ROUTINE ThreadFunc, LPVOID lpParameter)
{
    HANDLE hThrd;
    DWORD threadId;
    
    hThrd = CreateThread(NULL,
        0,
        ThreadFunc,
        lpParameter,
        0,
        &threadId );
    return hThrd;

}

HWND DoCreateTabControl(HINSTANCE hInstance, HWND hwndParent, DWORD dwStyle, int nr_tabs, TCHAR *icons[], TCHAR *labels[]);
HWND DoCreateFlatTabControl(HINSTANCE hInstance, HWND hwndParent, TCHAR *tab_class_name, int nr_tabs, TCHAR *labels[]);
HWND create_tab_win(HINSTANCE hInstance, HWND hwin_parent, TCHAR *WinClassName, int style);

HTREEITEM insertItem(HWND  hwnd_treeview, const wchar_t* str, HTREEITEM parent, HTREEITEM insertAfter,
                     int imageIndex, int selectedImageIndex, LPARAM lParam);

int get_open_file_name(char *file_name, HWND hwnd, char *filter_str);
int get_save_file_name(char *file_name, HWND hwnd, char *filter, char *ext);

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

#define ETH_P_LOOP	0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_ECHO	0x0200		/* Ethernet Echo packet		*/
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/

#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define ETH_P_RARP  0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_IPV6	0x86DD		/* IPv6		*/

#define  IPPROTO_IP    0
#define  IPPROTO_ICMP  1
#define  IPPROTO_IGMP  2
#define  IPPROTO_IPIP  4
#define  IPPROTO_TCP   6
#define  IPPROTO_UDP  17

typedef struct 
{
	__u8	ihl:4,
		version:4;
	__u8	tos;
	__u16	tot_len;
	__u16	id;
	__u16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__u16	check;
	__u32	saddr;
	__u32	daddr;
	/*The options start here. */
} __attribute__ ((aligned (1))) t_ip_hdr ;

#define FIXED_IP_HDR_LEN    20
static inline int ip_hdr_len(t_ip_hdr *iph)
{
    return iph->ihl*4;
}

static inline int ip_pkt_len(t_ip_hdr *iph)
{
    return ntohs(iph->tot_len);
}

static inline void * ip_data(t_ip_hdr *iph)
{
    return (void *)iph+ip_hdr_len(iph);
}

static inline int ip_data_len(t_ip_hdr *iph)
{
    return ip_pkt_len(iph)- ip_hdr_len(iph);
}

static inline void set_ip_pkt_len(t_ip_hdr *iph, int len)
{
    iph->tot_len=htons(len);
}

static inline int ip_pkt_is_frag(t_ip_hdr *iph)
{
    return ntohs(iph->frag_off)&((1<<14) - 1);
}

void ip_update_check(t_ip_hdr *iph);
int ip_checksum_wrong(t_ip_hdr *iph);

typedef struct 
{
  __u8		type;
  __u8		code;
  __u16		checksum;
  union {
	struct {
		__u16	id;
		__u16	sequence;
	} echo;
	__u32	gateway;
	struct {
		__u16	__unused;
		__u16	mtu;
	} frag;
  } un;
} __attribute__ ((aligned (1))) t_icmp_hdr;

typedef struct
{
	__u8 type;
	__u8 code;		/* For newer IGMP */
	__u16 csum;
	__u32 group;
} t_igmp_hdr;

void icmp_igmp_update_check(t_ip_hdr *iph);
int icmp_igmp_checksum_wrong(t_ip_hdr *iph);

typedef struct 
{
	__u32	saddr;
	__u32	daddr;
	__u8	bz;
	__u8	protocol;
	__u16	len;
} __attribute__ ((aligned (1))) t_tcp_udp_pseudo_hdr;


typedef struct 
{
	__u16	source;
	__u16	dest;
	__u32	seq;
	__u32	ack_seq;

	__u16	res1:4,
		doff:4,
		fin:1,
		syn:1,
		rst:1,
		psh:1,
		ack:1,
		urg:1,
		ece:1,
		cwr:1;

	__u16	window;
	__u16	check;
	__u16	urg_ptr;
} __attribute__ ((aligned (1))) t_tcp_hdr ;

typedef struct 
{
	__u16	source;
	__u16	dest;
	__u16	len;
	__u16	check;
} __attribute__ ((aligned (1))) t_udp_hdr;

void tcp_update_check(t_ip_hdr *iph);
void udp_update_check(t_ip_hdr *iph);
int tcp_checksum_wrong(t_ip_hdr *iph);
int udp_checksum_wrong(t_ip_hdr *iph);

typedef struct
{
	unsigned short	ar_hrd;		/* format of hardware address	*/
	unsigned short	ar_pro;		/* format of protocol address	*/
	unsigned char	ar_hln;		/* length of hardware address	*/
	unsigned char	ar_pln;		/* length of protocol address	*/
	unsigned short	ar_op;		/* ARP opcode (command)		*/

#if 1
	 /*
	  *	 Ethernet looks like this : This bit is variable sized however...
	  */
	unsigned char		ar_sha[6];	/* sender hardware address	*/
	unsigned char		ar_sip[4];		/* sender IP address		*/
	unsigned char		ar_tha[6];	/* target hardware address	*/
	unsigned char		ar_tip[4];		/* target IP address		*/
#endif

} __attribute__ ((aligned (1)))  t_arp_hdr;

void get_protocol_name(int protocol, char *name);
void get_eth_type_name(int type, char *info);

int SelRvs(HWND hList);
int SelAll(HWND hList);
int DelSel(HWND hList);
int GetIndex(HWND hList);
int GetSelCnt(HWND hList);

void delete_file_f(char *file_path);
int save_file_path_valid(char *file_path);
int file_exists(char *file_path);

static inline int time_a_smaller_than_b(struct timeval *a, struct timeval *b)
{
    if (a->tv_sec < b->tv_sec) return 1;
    if (a->tv_sec > b->tv_sec) return 0;

    return a->tv_usec < b->tv_usec;
}


static inline unsigned long time_a_between_b(struct timeval *a, struct timeval *b)
{
    if (a->tv_sec == b->tv_sec) return b->tv_usec - a->tv_usec;
    return 1000000 + b->tv_usec - a->tv_usec;
}

void ip_str2n(void *field_addr, char *info);
void ip_n2str(char *info, void * field_addr);
void set_int_text(HWND hwnd, int value);
int get_int_text(HWND hwnd);
void inc_int_text(HWND hwnd, int value);

static inline int button_checked(HWND hwnd)
{
    return (Button_GetState(hwnd) & BST_CHECKED)!=0;
}


static inline void button_check(HWND hwnd, int is_check)
{
    WPARAM wParam=(is_check?BST_CHECKED:BST_UNCHECKED);
    return SendMessage(hwnd, BM_SETCHECK, wParam, 0);
}

static inline void set_I64u_text(HWND hwnd, uint64_t value)
{
    char info[32];
    sprintf(info, "%I64u", value);
    SetWindowText(hwnd, info);
}

void change_child_height(HWND child, int height);
void change_child_width(HWND child, int width);
void change_child_size(HWND child, int width, int height);
void move_child_pos(HWND child, int x, int y);
void move_child_a2b_bottom_left(HWND a, HWND b, int offset);
void move_child_a2b_bottom_right(HWND a, HWND b, int offset);
void move_child_a2b_right_top(HWND a, HWND b, int offset);
void move_child_a2b_right_bottom(HWND a, HWND b, int offset);
void move_child_a2b_left_top(HWND a, HWND b, int offset);
void right_include_child(HWND child, int offset);
void bottom_include_child(HWND child, int offset);
void h_arrange_win(HWND hleader, int nr_member, HWND hmembers[], int offset);
void h_arrange_win_id(HWND hleader, int nr_member, int hmember_ids[], int offset);
void v_arrange_win_id(HWND hleader, int nr_member, int hmember_ids[], int offset);
int a_include_b_right(HWND a, HWND b);


#endif

