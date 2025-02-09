/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */


#ifndef __GLOBAL_INFO_H_
#define __GLOBAL_INFO_H_

#include <windows.h>
#include "defs.h"
#include "common.h"
#include "net.h"

extern const char version[4];

extern HINSTANCE g_hInstance;
extern TCHAR szAppName[];
extern int init_over;

#define TIMER_STATUS_BAR 1
#define TIMER_STATUS_GAP 100

#define TIMER_TIP_WIN_SHOW    2
#define TIMER_TIP_WIN_SHOW_GAP 3

#define TIMER_TIP_WIN_HIDE    3
#define TIMER_TIP_WIN_HIDE_GAP 30

#define TIMER_TIP_WIN_LAST    4
#define TIMER_TIP_WIN_LAST_GAP 500

#define  TIMER_VER_UPDATE_1   5
#define  TIMER_VER_UPDATE_1_GAP  50

#define  TIMER_VER_UPDATE_2   6
#define  TIMER_VER_UPDATE_2_GAP  1000

extern HWND    hwnd_frame;
extern HWND hwndTip;
extern int  doc_modified;

extern TCHAR szSubWinClassName[];
extern HWND  hwnd_left;
extern HWND  hwnd_tv;


extern TCHAR szRightWinClassName[];
extern HWND    hwnd_right;
extern HWND    hwnd_stream;
extern HWND    hwnd_fc;
extern HWND    hwnd_lv;

extern TCHAR szBottomWinClassName[];
extern HWND    hwnd_bottom;

#define    SPLT_WIDTH    (6)
extern TCHAR szSpltWeClassName[];
extern TCHAR szSpltNsClassName[];

extern HWND    hwnd_splt_we;
extern HWND    hwnd_splt_ns;

extern int we_pos;
extern int ns_pos;

extern HWND    hwnd_toolbar;
extern int toolbar_height;

extern HWND    hwnd_statusbar;
extern int statusbar_height;

extern TCHAR szHexEditWinClassName[];
extern char  *test_buf;
extern int cur_data_len;
extern int is_read_only;

extern int cur_strm_idx;
extern HWND    hwnd_hex_edit_pcap;
extern HWND    hwnd_hex_edit_strm;
extern HWND hwnd_dynamic_edit;

extern TCHAR szTipWinClassName[];
extern HWND    hwnd_tip;
void show_tip(TCHAR *info);
void err_msg_box(TCHAR * szFormat, ...);

extern TCHAR szStatsWinClassName[];
extern HWND hwnd_stats;

extern int need_stop;
extern struct timeval last_timer_tv;
extern struct timeval time_elapsed;
extern struct timeval last_stat_tv_snd, last_stat_tv_rcv, last_stat_tv_sndf;
extern int snd_stopped, rcv_stopped;
extern int snd_started, rcv_started;

extern uint64_t cap_save_cnt, cap_save_bytes_cnt;

DWORD WINAPI  wpcap_snd_test(LPVOID lpParameter);
DWORD WINAPI  wpcap_rcv_test(LPVOID lpParameter);
void cp_stream2sndbuf();

extern int cap_stopped;
extern int need_cap_stop;
extern char last_nic_name[128];
DWORD WINAPI  rcv_pkt_2(LPVOID lpParameter);

int reg_sys_win_classes();
int register_frame();
int register_left_win();
int register_right_win();
int register_bottom_win();
int register_splt_wins();
int register_hex_edit_win();
int register_stats_win();
int register_tip_win();
int CreateToolbar();
int CreateStatusBar();
int create_windows(int iCmdShow);
void add_tip(HWND htip_control, HWND hwnd, TCHAR *tips);

void resize_window(HWND hwnd);
void hex_win_sel(HWND  hwnd, int offset, int len);
void hex_win_desel(HWND  hwnd, int offset, int len);
void update_statusbar();

DWORD WINAPI  ver_update(LPVOID lpParameter);

#define  WM_SPLITTER_X     (WM_USER + 3)
#define  WM_SPLITTER_Y     (WM_USER + 4)
#define  WM_PROC_PCAP_FILE (WM_USER + 5)
static inline void send_splitter_x(HWND  hwnd, int pos)
{
    SendMessage(hwnd, WM_SPLITTER_X, pos, 0);
}

static inline void send_splitter_y(HWND  hwnd, int pos)
{
    SendMessage(hwnd, WM_SPLITTER_Y, pos, 0);
}

typedef struct
{
    uint64_t send_total, send_fail, rcv_total;
    uint64_t send_total_bytes, send_fail_bytes, rcv_total_bytes;
} t_pkt_stat;
extern t_pkt_stat gt_pkt_stat, gt_pkt_stat_pre;

void update_stats();
void clear_stats();
void init_stats_ui();
void stream_edit_data_change(HWND  hwnd, int offset);


#define    ALL_FILE_FILTER    "all(*.*)\0*.*\0\0"

#define    CFG_FILE_FILTER    "ethernet test config(*.etc)\0*.etc\0\0"
#define    CFG_FILE_SUFFIX    "etc"

#define    PCAP_FILE_FILTER    "wireshark/tcpdump(*.pcap)\0*.pcap\0\0"
#define    PCAP_FILE_SUFFIX    "pcap"

#define    TEXT_FILE_FILTER    "text file(*.txt)\0*.txt\0\0"
#define    TEXT_FILE_SUFFIX    "txt"

#define    BIN_FILE_FILTER    "binary file(*.bin)\0*.bin\0\0"
#define    BIN_FILE_SUFFIX    "bin"

#define    APP_PROFILE_FILE    ".\\profile.ini"
#define    VER_UPDATE_FILE    ".\\version_update.ini"

#define    VER_UPDATE_NOTICE_RCD    ".\\ver_updt_notice_rcd.ini"

extern HWND    hwnd_net_card_comb;
extern HWND    hwnd_capture_checkbox;
extern int need_capture;

void rx_tx_init();
void *get_lvi_lparam(HWND hlv, int idx);
void fc_and_pkt_cap_init();
void del_all_streams();
void init_net_card_combbox(HWND hwnd_comb);
int select_if(int idx);
int is_filter_valid(char *packet_filter);
void update_fc_gap();
void get_pkt_by_idx(uint32_t idx, t_dump_pkt *pt_pkt);

int re_populate_items();
LRESULT CALLBACK my_lv_proc (HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
extern WNDPROC old_lv_proc;

int stream2dump(char *file_name);
int stream_2_bin(char *file_path);
int stream_2_text(char *file_path);
int save_stream(char *file_path);
int load_stream(char *file_path);
int load_stream_from_dump_file(char *file_path);
void add_stream(t_stream *pt_stream);
void cpy_stream(t_stream *dst, const t_stream *src);
void mac_str2n(unsigned char *mac, char *info_usr);
void mac_n2str(char *info, void *field_addr);
void update_check_sum(t_stream *pt_stream);
void update_len(t_stream *pt_stream);
int stream_fragable(t_stream *pt_stream);
int get_eth_type_comb(HWND comb);
void init_eth_type_comb(HWND comb);

extern t_stream gt_edit_stream;
BOOL CALLBACK StreamEditDlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);
void rule_fileds_init(t_stream *pt_stream);
void rule_fileds_update(t_stream *pt_stream);
void rule_fileds_init_all_pkt();
BOOL CALLBACK PktViewDlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PktCapDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PktCapCfgDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);
void build_filter(char *str_filter_output);
BOOL CALLBACK FragDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);
void *alloc_stream();
void init_protocol_comb(HWND comb, int add_all);
extern HWND hPktCapDlg;
void update_pkt_cap_stats(t_ether_packet *pt_pkt);

void populate_recent_cfg_files(HMENU hMenu);
void update_cfg_file_history(char *file_name);
int get_history_cfg_file_by_idx(int idx, char *field_value);
int get_history_pcap_file_by_idx(int idx, char *field_value);
void update_pcap_file_history(char *file_name);
void populate_recent_pcap_files(HMENU hMenu);

void * get_nic_FriendlyName(const char *name);


#define    SPEED_TYPE_HIGH      0
#define    SPEED_TYPE_LOW       1
#define    SPEED_TYPE_FASTEST   2

#define    SND_MODE_CONTINUE    0
#define    SND_MODE_BURST       1
typedef struct
{
    int32_t speed_type;
    int32_t speed_value;
    int32_t snd_mode;
    int32_t snd_times_cnt;
    char rsv[32];
} __attribute__((packed)) t_fc_cfg;
extern t_fc_cfg  gt_fc_cfg;

extern int snd_gap_s;
extern int snd_gap_us;

#define PKT_CAP_CFG_MODE_NORMAL     0
#define PKT_CAP_CFG_MODE_ADVANCED   1
#define PKT_CAP_FILE_WHILE_SND    ".\\pkt_cap_while_snd.pcap"
#define PKT_CAP_FILE_ONLY_CAP     ".\\cap_independent.pcap"
#define PKT_CAP_PKT_TYPE_L2    1
#define PKT_CAP_PKT_TYPE_IP    2
#define PKT_CAP_PKT_TYPE_ALL    3
#define PKT_CAP_PKT_TYPE_IPV6    4

#define PKT_CAP_PORT_MIN    0
#define PKT_CAP_PORT_MAX    65535

#define    MAX_CAP_FILTER_STR_LEN    256
typedef struct
{
    int32_t need_save_capture;
    int32_t pkt_cap_cfg_mode;
    int32_t pkt_cap_pkt_type;
    int32_t pkt_cap_sip, pkt_cap_dip, pkt_cap_protocol;
    int32_t pkt_cap_sport, pkt_cap_dport;
    int32_t filter_str_len;
    char rsv[32];
    char filter_str_usr[MAX_CAP_FILTER_STR_LEN];
} __attribute__((packed)) t_pkt_cap_cfg;
#define    PKT_CAP_CFG_FIX_LEN    ((unsigned long)(void *)(&(((t_pkt_cap_cfg *)NULL)->filter_str_usr)))

extern t_pkt_cap_cfg  gt_pkt_cap_cfg;


extern char pkt_cap_filter_str[MAX_CAP_FILTER_STR_LEN];
extern char pcap_file_to_view[MAX_FILE_PATH_LEN];
extern int  cur_view_pkt_idx;

extern char query_save_captured_pkts_2[8];
extern char  new_version_notice[8];
extern HWND    hwnd_b_tab;
extern int err_cnt, info_cnt;
void PrintText(TCHAR * szFormat, ...);

#define FILE_PCAP_CACHE_INFO    ".\\pcap_cache_info"
#define FILE_PCAP_CACHE_DATA    ".\\pcap_cache_data"

typedef struct
{
    uint32_t offset;
    uint32_t len;

} __attribute__((packed)) t_data_seg_info;


typedef struct
{
    void *pkt_data;
    int len;

} t_sample_pkt;

extern t_sample_pkt gat_sample_pkts[];

#endif

