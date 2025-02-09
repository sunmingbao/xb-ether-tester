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
#include <sys/time.h>
#include "common.h"
#include "global_info.h"
#include "res.h"
#include "gui.h"

TCHAR szStatsWinClassName[] = TEXT("stats_win") ;
HWND hwnd_stats;
HWND hwnd_hdr_time;
HWND hwnd_hdr_bps, hwnd_hdr_pps, hwnd_hdr_bps_avg, hwnd_hdr_pps_avg, hwnd_hdr_pkt, hwnd_hdr_bits;

HWND hwnd_snd, hwnd_snd_fail, hwnd_rcv;
HWND hwnd_snd_bps, hwnd_snd_pps, hwnd_snd_bps_avg, hwnd_snd_pps_avg, hwnd_snd_pkt, hwnd_snd_bits;
HWND hwnd_snd_fail_bps, hwnd_snd_fail_pps, hwnd_snd_fail_bps_avg, hwnd_snd_fail_pps_avg, hwnd_snd_fail_pkt, hwnd_snd_fail_bits;
HWND hwnd_rcv_bps, hwnd_rcv_pps, hwnd_rcv_bps_avg, hwnd_rcv_pps_avg, hwnd_rcv_pkt, hwnd_rcv_bits;

HWND hwnd_stats_handles[4][7];
void update_stats()
{
    TCHAR info[32];
    struct timeval cur_tv, tmp_tv;
    t_pkt_stat gt_pkt_stat_tmp;
    t_pkt_stat *pt_pkt_stat = &gt_pkt_stat_tmp;

    unsigned long    total_ms;
    unsigned long    gap_ms, time_gap;

    gettimeofday(&cur_tv, NULL);
    gt_pkt_stat_tmp=gt_pkt_stat;
    
    time_gap=time_a_between_b(&last_timer_tv, &cur_tv);
    time_elapsed.tv_usec+=time_gap;
    if (time_elapsed.tv_usec>=1000000)
    {
        time_elapsed.tv_usec-=1000000;
        time_elapsed.tv_sec+=1;

    }
    last_timer_tv=cur_tv;
    
    _stprintf (	info, TEXT("time %lu.%lu"), time_elapsed.tv_sec, (time_elapsed.tv_usec/100000)) ;
    SetWindowText(hwnd_hdr_time, info); 

    if ((gt_pkt_stat_tmp.send_total==gt_pkt_stat_pre.send_total)  &&
        (gt_pkt_stat_tmp.send_fail==gt_pkt_stat_pre.send_fail)  &&
        (gt_pkt_stat_tmp.rcv_total==gt_pkt_stat_pre.rcv_total)) 
        return;

    total_ms = (time_elapsed.tv_sec*1000 + time_elapsed.tv_usec/1000);

if (gt_pkt_stat_tmp.send_total!=gt_pkt_stat_pre.send_total)
{
    tmp_tv = time_a_between_b2(last_stat_tv_snd, cur_tv);
    gap_ms = tmp_tv.tv_sec*1000 + tmp_tv.tv_usec/1000;

    set_I64u_text(hwnd_snd_bps, (1000*8*(pt_pkt_stat->send_total_bytes-gt_pkt_stat_pre.send_total_bytes)/gap_ms)); 
    set_I64u_text(hwnd_snd_pps, (1000*(pt_pkt_stat->send_total-gt_pkt_stat_pre.send_total)/gap_ms)); 
    set_I64u_text(hwnd_snd_bps_avg, (1000*8*(pt_pkt_stat->send_total_bytes)/total_ms)); 
    set_I64u_text(hwnd_snd_pps_avg, (1000*(pt_pkt_stat->send_total)/total_ms)); 
    set_I64u_text(hwnd_snd_bits, pt_pkt_stat->send_total_bytes); 
    set_I64u_text(hwnd_snd_pkt, pt_pkt_stat->send_total); 
        last_stat_tv_snd= cur_tv;
}

if (gt_pkt_stat_tmp.send_fail!=gt_pkt_stat_pre.send_fail)
{
    tmp_tv = time_a_between_b2(last_stat_tv_sndf, cur_tv);
    gap_ms = tmp_tv.tv_sec*1000 + tmp_tv.tv_usec/1000;

    set_I64u_text(hwnd_snd_fail_bps, (1000*8*(pt_pkt_stat->send_fail_bytes-gt_pkt_stat_pre.send_fail_bytes)/gap_ms)); 
    set_I64u_text(hwnd_snd_fail_pps, (1000*(pt_pkt_stat->send_fail-gt_pkt_stat_pre.send_fail)/gap_ms)); 
    set_I64u_text(hwnd_snd_fail_bps_avg, (1000*8*(pt_pkt_stat->send_fail_bytes)/total_ms)); 
    set_I64u_text(hwnd_snd_fail_pps_avg, (1000*(pt_pkt_stat->send_fail)/total_ms)); 
    set_I64u_text(hwnd_snd_fail_bits, pt_pkt_stat->send_fail_bytes); 
    set_I64u_text(hwnd_snd_fail_pkt, pt_pkt_stat->send_fail); 
        last_stat_tv_sndf= cur_tv;
}

if (gt_pkt_stat_tmp.rcv_total!=gt_pkt_stat_pre.rcv_total)
{
    tmp_tv = time_a_between_b2(last_stat_tv_rcv, cur_tv);
    gap_ms = tmp_tv.tv_sec*1000 + tmp_tv.tv_usec/1000;

    set_I64u_text(hwnd_rcv_bps, (1000*8*(pt_pkt_stat->rcv_total_bytes-gt_pkt_stat_pre.rcv_total_bytes)/gap_ms)); 
    set_I64u_text(hwnd_rcv_pps, (1000*(pt_pkt_stat->rcv_total-gt_pkt_stat_pre.rcv_total)/gap_ms)); 
    set_I64u_text(hwnd_rcv_bps_avg, (1000*8*(pt_pkt_stat->rcv_total_bytes)/total_ms)); 
    set_I64u_text(hwnd_rcv_pps_avg, (1000*(pt_pkt_stat->rcv_total)/total_ms)); 
    set_I64u_text(hwnd_rcv_bits, pt_pkt_stat->rcv_total_bytes); 
    set_I64u_text(hwnd_rcv_pkt, pt_pkt_stat->rcv_total); 
        last_stat_tv_rcv= cur_tv;
}
    gt_pkt_stat_pre= *pt_pkt_stat;
}

void copy_stats_2_clip_board()
{
    TCHAR info[32], info_2[32];
    char * pchData;
    HGLOBAL hClipboardData;
    int i, j;
    int ret, cur_len=0;

    if (OpenClipboard(NULL))
    {

        EmptyClipboard();


        hClipboardData = GlobalAlloc(0, 1024);


        pchData = (char*)GlobalLock(hClipboardData);
        memset(pchData, 0, 1024);
        	  

        for (i=0;i<4;i++)
        {
            for (j=0;j<7;j++)
            {
                GetWindowText(hwnd_stats_handles[i][j], info, sizeof(info));
                if (0==j)
                    ret=sprintf(info_2, "%-15s", info);
                else
                    ret=sprintf(info_2, "%15s", info);
                
                sprintf(pchData+cur_len, info_2);
                cur_len+=ret;
            }

            sprintf(pchData+cur_len, "\r\n");
            cur_len+=2;

        }

        GlobalUnlock(hClipboardData);
        	  

        SetClipboardData(CF_TEXT,hClipboardData);
        	  

        CloseClipboard();
    }


}

void clear_stats()
{

    int i, j;
    
    memset(&gt_pkt_stat, 0, sizeof(t_pkt_stat));
    memset(&gt_pkt_stat_pre, 0, sizeof(t_pkt_stat));
    time_elapsed.tv_sec =0;
    time_elapsed.tv_usec =0;

    SetWindowText(hwnd_stats_handles[0][0], TEXT("time 0.0")); 
        for (i=1;i<4;i++)
            for (j=1;j<7;j++)
            {
                    SetWindowText(hwnd_stats_handles[i][j], TEXT("0"));
            }

}

static HWND create_statis_grid()
{
    HWND ret = CreateWindow (TEXT ("edit"), TEXT ("0"),
                WS_CHILD|WS_BORDER|WS_VISIBLE|ES_READONLY|SS_RIGHT,
                10, 10,
                300, 25,
                hwnd_stats, NULL, g_hInstance, NULL);

    SendMessage(ret, WM_SETFONT, (WPARAM)char_font_2, 0);

    return ret;
}

static HWND create_statis_button(TCHAR *cap, unsigned align)
{
    HWND ret = CreateWindow (TEXT ("button"), cap,
                WS_CHILD|WS_BORDER|WS_VISIBLE|BS_FLAT|align|BS_TEXT,
                10, 10,
                300, 25,
                hwnd_stats, NULL, g_hInstance, NULL);

    SendMessage(ret, WM_SETFONT, (WPARAM)char_font_2, 0);

    return ret;
}

LRESULT CALLBACK stats_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT		rect1, rect2 ;
    int cxClient, cyClient;
    static HMENU	hMenu ;
    POINT point ;
    
    switch (message)
    {
        case WM_CREATE:

            hwnd_stats = hwnd;

            hwnd_stats_handles[0][0]=hwnd_hdr_time = CreateWindow (TEXT ("button"), TEXT ("time 0.0"),
                WS_CHILD|WS_BORDER|WS_VISIBLE|BS_FLAT|BS_LEFT|BS_TEXT,
                10, 10,
                300, 25,
                hwnd_stats, NULL, g_hInstance, NULL);

            SendMessage(hwnd_hdr_time, WM_SETFONT, (WPARAM)char_font_2, 0);

            hwnd_stats_handles[0][1]=hwnd_hdr_bps= create_statis_button(TEXT ("bps"), BS_CENTER);

            hwnd_stats_handles[0][2]=hwnd_hdr_pps = create_statis_button(TEXT ("pps"), BS_CENTER);

            hwnd_stats_handles[0][3]=hwnd_hdr_bps_avg= create_statis_button(TEXT ("AVG bps"), BS_CENTER);

            hwnd_stats_handles[0][4]=hwnd_hdr_pps_avg= create_statis_button(TEXT ("AVG pps"), BS_CENTER);

            hwnd_stats_handles[0][5]=hwnd_hdr_pkt= create_statis_button(TEXT ("packet cnt"), BS_CENTER);

            hwnd_stats_handles[0][6]=hwnd_hdr_bits= create_statis_button(TEXT ("bytes cnt"), BS_CENTER);

            hwnd_stats_handles[1][0]=hwnd_snd=create_statis_button(TEXT ("TX"), BS_LEFT);


            hwnd_stats_handles[1][1]=hwnd_snd_bps=create_statis_grid();
            hwnd_stats_handles[1][2]=hwnd_snd_pps=create_statis_grid();
            hwnd_stats_handles[1][3]=hwnd_snd_bps_avg=create_statis_grid();
            hwnd_stats_handles[1][4]=hwnd_snd_pps_avg=create_statis_grid();
            hwnd_stats_handles[1][5]=hwnd_snd_pkt=create_statis_grid();
            hwnd_stats_handles[1][6]=hwnd_snd_bits=create_statis_grid();
            
            hwnd_stats_handles[2][0]=hwnd_snd_fail=create_statis_button(TEXT ("TX failed"), BS_LEFT);
            
            hwnd_stats_handles[2][1]=hwnd_snd_fail_bps=create_statis_grid();
            hwnd_stats_handles[2][2]=hwnd_snd_fail_pps=create_statis_grid();
            hwnd_stats_handles[2][3]=hwnd_snd_fail_bps_avg=create_statis_grid();
            hwnd_stats_handles[2][4]=hwnd_snd_fail_pps_avg=create_statis_grid();
            hwnd_stats_handles[2][5]=hwnd_snd_fail_pkt=create_statis_grid();
            hwnd_stats_handles[2][6]=hwnd_snd_fail_bits=create_statis_grid();

            hwnd_stats_handles[3][0]=hwnd_rcv=create_statis_button(TEXT ("capture pkts"), BS_LEFT);

            hwnd_stats_handles[3][1]=hwnd_rcv_bps=create_statis_grid();
            hwnd_stats_handles[3][2]=hwnd_rcv_pps=create_statis_grid();
            hwnd_stats_handles[3][3]=hwnd_rcv_bps_avg=create_statis_grid();
            hwnd_stats_handles[3][4]=hwnd_rcv_pps_avg=create_statis_grid();
            hwnd_stats_handles[3][5]=hwnd_rcv_pkt=create_statis_grid();
            hwnd_stats_handles[3][6]=hwnd_rcv_bits=create_statis_grid();

            clear_stats();


            hMenu = LoadMenu (g_hInstance, TEXT("stats_win_popup_menu")) ;
            hMenu = GetSubMenu (hMenu, 0) ;
            add_tip(hwndTip, hwnd_stats, TEXT("Right click to operate"));
            return 0 ;

        case WM_SIZE:
        {
            int i, j;
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            GetWindowRect(hwnd_stats, &rect1);

#if 0
            MoveWindow	(hwnd_hdr_time, 	10,  5, 180, 25, TRUE) ;
            MoveWindow	(hwnd_hdr_bps,      200, 5, 90, 25, TRUE) ;
            MoveWindow	(hwnd_hdr_pps,      300, 5,  90, 25, TRUE) ;
            MoveWindow	(hwnd_hdr_bps_avg,  400, 5, 90, 25, TRUE) ;
            MoveWindow	(hwnd_hdr_pps_avg,  500, 5, 90, 25, TRUE) ;
            MoveWindow	(hwnd_hdr_pkt, 	    600, 5, 140, 25, TRUE) ;
            MoveWindow	(hwnd_hdr_bits, 	750, 5, 180, 25, TRUE) ;
#else
            MoveWindow	(hwnd_hdr_time,     10 *WIDTH_COEFFICIENT, 5, 170*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
            MoveWindow	(hwnd_hdr_bps,      190*WIDTH_COEFFICIENT, 5,  150*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
            MoveWindow	(hwnd_hdr_pps,      350*WIDTH_COEFFICIENT, 5,  125*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
            MoveWindow	(hwnd_hdr_bps_avg,  485*WIDTH_COEFFICIENT, 5,  150*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
            MoveWindow	(hwnd_hdr_pps_avg,  645*WIDTH_COEFFICIENT, 5,  125*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
            MoveWindow	(hwnd_hdr_pkt, 	    780*WIDTH_COEFFICIENT, 5, 145*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
            MoveWindow	(hwnd_hdr_bits,     935*WIDTH_COEFFICIENT, 5, 205*WIDTH_COEFFICIENT, 35*HEIGHT_COEFFICIENT, TRUE) ;
#endif

        for (i=1;i<4;i++)
            for (j=0;j<7;j++)
            {
                    GetWindowRect(hwnd_stats_handles[i-1][j], &rect2);
                    get_relative_pos(&rect1, &rect2, &rect2);
                    MoveWindow(hwnd_stats_handles[i][j], 	rect2.left, rect2.bottom+10,
                        rect2.right-rect2.left, rect2.bottom-rect2.top, TRUE) ;
            }
            
        return 0 ;

        }

            case WM_RBUTTONDOWN:
            {
                point.x = (short)LOWORD(lParam);  // horizontal position of cursor 
                point.y = (short)HIWORD(lParam);
                ClientToScreen (hwnd, &point) ;
          		TrackPopupMenu (hMenu, TPM_LEFTBUTTON, point.x, point.y, 0, hwnd, NULL) ;
                return 0;

            }

            case 	WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case    IDM_STATS_WIN_COPY2CLIP_BOARD:
                copy_stats_2_clip_board();
               	return 0 ;

                case    IDM_STATS_WIN_CLR:
                clear_stats();
               	return 0 ;


            }
            break;
    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_stats_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS;
    sub_wndclass.lpfnWndProc= stats_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szStatsWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}








