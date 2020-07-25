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
#include "gui.h"
#include "res.h"


TCHAR   szFrameWinClassName[] = TEXT ("fsb_frame") ;
HWND    hwnd_frame;

HWND hwndTip;
int  doc_modified;
HWND    hwnd_splt_we;
HWND    hwnd_splt_ns;

int we_pos;
int ns_pos;


int display_left=1;
int display_bottom=1;
int display_toolbar=1;
int display_statusbar=1;

//int query_clr_stats=1;
char auto_clr_stats[8];
char last_nic_name[128];
char query_save_captured_pkts[8];
char query_save_captured_pkts_2[8];
char cfg_file_path[MAX_FILE_PATH_LEN];
char file_to_open[MAX_FILE_PATH_LEN];

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam);
void set_frame_title(TCHAR *file_name)
{
    TCHAR info[128];
    sprintf(info, TEXT("%s - %s"), szAppName, file_name);
    SetWindowText(hwnd_frame, info);
}

void ui_switch(int is_sending)
{
    SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_START, is_sending);
    EnableWindow(hwnd_tv, !is_sending);
    EnableWindow(hwnd_right, !is_sending);
    EnableWindow(hwnd_net_card_comb, !is_sending);
    SendMessage(hwnd_toolbar, TB_ENABLEBUTTON, IDT_TOOLBAR_VIEW_CAPTURE, !is_sending);
    SendMessage(hwnd_toolbar, TB_ENABLEBUTTON, IDT_TOOLBAR_CAPTURE, !is_sending);
    SendMessage(hwnd_toolbar, TB_ENABLEBUTTON, IDT_TOOLBAR_CAP_WHILE_SND, !is_sending);
}

int doc_save_proc()
{
    int ret;
    if (doc_modified)
    {
        ret=AskConfirmation_3state(hwnd_frame, TEXT("save current config?"), szAppName);
        if (IDCANCEL == ret) return 1;
        if (IDYES == ret)
        {
            SendMessage(hwnd_frame, WM_COMMAND, IDM_FILE_SAVE, 0);
            return doc_modified;
        }
    }

    return 0;
}

int stats_captured_pkts_proc()
{
    char file_name[MAX_FILE_PATH_LEN];
    int ret;

//PKT_SAVE_PROC:
    if (!cap_save_cnt) goto STATS_PROC;
    if (strcmp(query_save_captured_pkts, "no")==0) goto CLR_CAP_PKTS;

    ret=AskConfirmation_3state(hwnd_frame, TEXT("packets captured would be deleted\r\n\r\nwanna save it?\r\n\r\n[This notice can be muted through menu options]"), szAppName);
    if (IDCANCEL == ret) return 1;
    if (IDYES == ret)
    {
        file_name[0]=0;
        if (0==get_save_file_name(file_name, hwnd_frame, "pcap file(*.pcap)\0*.pcap\0\0", "pcap"))
            CopyFile(PKT_CAP_FILE_WHILE_SND,  file_name,  FALSE);
        else
            return 1;
    }

CLR_CAP_PKTS:
        cap_save_cnt= 0;
        cap_save_bytes_cnt = 0;

STATS_PROC:
    if (!gt_pkt_stat.send_total && !gt_pkt_stat.rcv_total)
        return 0;

    if (strcmp(auto_clr_stats,"query")==0)
    {
        ret= AskConfirmation_3state(hwnd_frame, TEXT("clear existing statistics?\r\n\r\n[This notice can be muted through menu options]"), szAppName);
        if (IDCANCEL == ret) return 1;
        if (IDNO == ret) return 0;

    }
    else if (strcmp(auto_clr_stats,"no")==0)
    {
        return 0;
    }

    
//CLR_STAT:
    clear_stats();

    return 0;
}


void open_file()
{
    if (doc_save_proc()) return;

    if (load_stream(file_to_open)) return;
    re_populate_items();
    strcpy(cfg_file_path, file_to_open);
    set_frame_title(strrchr(cfg_file_path, '\\')+1);
    update_statusbar();
    update_cfg_file_history(file_to_open);
}

void new_cfg()
{
    cap_save_cnt= 0;
    cap_save_bytes_cnt = 0;
    clear_stats();

    del_all_streams();
    fc_and_pkt_cap_init();
    update_statusbar();
    cfg_file_path[0]=0;
    set_frame_title(TEXT("Untitled"));
    doc_modified=0;

}

void load_app_profile()
{
    HMENU hMenu = GetMenu(hwnd_frame);
    GetPrivateProfileString("before_send_pkt", "clear_stat", "query"
        , auto_clr_stats, ARRAY_SIZE(auto_clr_stats), APP_PROFILE_FILE);

    CheckMenuItem (hMenu, IDM_APP_QUERY_CLR_STATS, MF_UNCHECKED);
    CheckMenuItem (hMenu, IDM_APP_CLR_STATS, MF_UNCHECKED);
    CheckMenuItem (hMenu, IDM_APP_NOT_CLR_STATS, MF_UNCHECKED);

    if (strcmp(auto_clr_stats,"query")==0)
        CheckMenuItem (hMenu, IDM_APP_QUERY_CLR_STATS, MF_CHECKED) ;
    if (strcmp(auto_clr_stats,"yes")==0)
        CheckMenuItem (hMenu, IDM_APP_CLR_STATS, MF_CHECKED) ;
    if (strcmp(auto_clr_stats,"no")==0)
        CheckMenuItem (hMenu, IDM_APP_NOT_CLR_STATS, MF_CHECKED) ;
    
    GetPrivateProfileString("before_send_pkt", "query_save_captured_pkts", "yes"
        , query_save_captured_pkts, ARRAY_SIZE(query_save_captured_pkts), APP_PROFILE_FILE);
    CheckMenuItem (hMenu, IDM_APP_CLR_CAPTURED_PKS, MF_UNCHECKED);
    CheckMenuItem (hMenu, IDM_APP_QUERY_SAVE_CAPTURED_PKS, MF_UNCHECKED);

    if (strcmp(query_save_captured_pkts,"yes")==0)
        CheckMenuItem (hMenu, IDM_APP_QUERY_SAVE_CAPTURED_PKS, MF_CHECKED) ;
    else
        CheckMenuItem (hMenu, IDM_APP_CLR_CAPTURED_PKS, MF_CHECKED) ;

    GetPrivateProfileString("before_only_cap_pkt", "query_save_captured_pkts", "yes"
        , query_save_captured_pkts_2, ARRAY_SIZE(query_save_captured_pkts_2), APP_PROFILE_FILE);
CheckMenuItem (hMenu, IDM_APP_QUERY_SAVE_CAPTURED_PKS_2
    , (strcmp(query_save_captured_pkts_2, "yes")==0)?MF_CHECKED:MF_UNCHECKED) ;

    GetPrivateProfileString("last_nic", "name", ""
        , last_nic_name, ARRAY_SIZE(last_nic_name), APP_PROFILE_FILE);


    GetPrivateProfileString("update", "new_version_notice", "yes"
        , new_version_notice, ARRAY_SIZE(new_version_notice), APP_PROFILE_FILE);
CheckMenuItem (hMenu, IDM_NEW_VERSION_NOTICE
    , (strcmp(new_version_notice, "yes")==0)?MF_CHECKED:MF_UNCHECKED) ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int  cxClient, cyClient;
    HMENU hMenu;
    POINT point;
    RECT		rect ;
    int  item_id;
    int upper_win_y=toolbar_height, 
        right_win_x=we_pos, 
        right_win_width=cxClient-we_pos, 
        upper_win_height,
        bottom_win_height,
        left_win_height;
    
    int ret;

    switch (message)
    {
        case WM_CREATE:
            hwnd_frame = hwnd;
            load_app_profile();
            CreateToolbar();

            GetClientRect(hwnd, &rect) ;
            cxClient = rect.right;
            cyClient = rect.bottom;

            //point.x=cxClient / 4;
            //point.y=cyClient / 4;
            point.x=0;
            point.y=0;

            //ClientToScreen(hwnd, &point);
           			// Move the buttons to the new center
      hwndTip =CreateWindowEx(0, TOOLTIPS_CLASS, NULL,
                        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP |TTS_BALLOON,
                      CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                         hwnd, 	//Handle of the Parent window.
				//(i.e. Window for which you want Tooltip)
                         NULL,
                         g_hInstance, 
                         NULL);


            hwnd_left = CreateWindow (szSubWinClassName, TEXT ("sub win"),
                WS_CHILD,
                point.x, point.y,
                we_pos-SPLT_WIDTH, ns_pos-SPLT_WIDTH,
                hwnd, NULL, g_hInstance, NULL) ;


            hwnd_splt_we= CreateWindow (szSpltWeClassName, TEXT ("sub win"),
                WS_CHILD,
                we_pos-SPLT_WIDTH,
                            point.y,
                  			SPLT_WIDTH, cyClient,
                hwnd, NULL, g_hInstance, NULL) ;
            ShowWindow (hwnd_splt_we, 1) ;
            UpdateWindow (hwnd_splt_we) ;


            hwnd_right = CreateWindow (szRightWinClassName, TEXT ("sub win"),
                WS_CHILD,
                we_pos,
                            point.y,
                  			cxClient-we_pos, ns_pos-SPLT_WIDTH,
                hwnd, NULL, g_hInstance, NULL) ;


            hwnd_splt_ns= CreateWindow (szSpltNsClassName, TEXT ("sub win"),
                WS_CHILD,
                cxClient-we_pos, ns_pos-SPLT_WIDTH,
                cxClient-we_pos, SPLT_WIDTH,
                hwnd, NULL, g_hInstance, NULL) ;
            
            ShowWindow (hwnd_splt_ns, 1) ;
            UpdateWindow (hwnd_splt_ns) ;

            hwnd_bottom = CreateWindow (szBottomWinClassName, TEXT ("sub win"),
                WS_CHILD,
                we_pos,    ns_pos,
                  			cxClient-we_pos, cyClient-ns_pos,
                hwnd, NULL, g_hInstance, NULL) ;

            ShowWindow (hwnd_left, 1) ;
            UpdateWindow (hwnd_left) ;

            ShowWindow (hwnd_right, 1) ;
            UpdateWindow (hwnd_right) ;

            ShowWindow (hwnd_bottom, 1) ;
            UpdateWindow (hwnd_bottom) ;
            //init_stats_ui();

CreateStatusBar();

            hwnd_tip = CreateWindow (szTipWinClassName, TEXT ("tip win"),
                WS_POPUP,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                hwnd, NULL, g_hInstance, NULL) ;

            ShowWindow (hwnd_tip, 0) ;

            DragAcceptFiles(hwnd, TRUE);

            launch_thread(ver_update, NULL);

            new_cfg();
            ret=get_history_cfg_file_by_idx(1, file_to_open);
            if (ret) 
            {
                return 0 ;
            }
            
            if (!file_exists(file_to_open)) 
            {
                return 0 ;
            }

            open_file();

            return 0 ;


        case WM_SPLITTER_X:
        {
            we_pos=wParam+SPLT_WIDTH/2;
            resize_window(hwnd);
            return 0 ;
        }

        case WM_SPLITTER_Y:
        {
            ns_pos=wParam+SPLT_WIDTH/2;
            resize_window(hwnd);
            return 0 ;
        }

        case WM_SIZE:
#if 1
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

            if (we_pos==0 || ns_pos==0)
            {
                we_pos = 260*WIDTH_COEFFICIENT;
                ns_pos = cyClient-330*HEIGHT_COEFFICIENT;

            }

        left_win_height = cyClient;
        if (display_toolbar)
        {
            left_win_height -= toolbar_height;
        }

        if (display_statusbar)
        {
            left_win_height -= statusbar_height;
        }
        right_win_x=display_left?we_pos:0;
        right_win_width=display_left?cxClient-we_pos:cxClient;

        upper_win_y=display_toolbar?toolbar_height:0;
        if (display_bottom)
        {
            upper_win_height=ns_pos-SPLT_WIDTH;
        }
        else
        {
            upper_win_height=cyClient;
            upper_win_height -= (display_statusbar?statusbar_height:0);

        }
        upper_win_height -= upper_win_y;

        bottom_win_height = cyClient-ns_pos;
        bottom_win_height -= (display_statusbar?statusbar_height:0);

            //ClientToScreen(hwnd, &point);
           			// Move the buttons to the new center

      	    MoveWindow	(	hwnd_toolbar, 	0, 0,
                cxClient, toolbar_height, TRUE) ;

      	    MoveWindow	(	hwnd_left, 	0, upper_win_y,
                we_pos-SPLT_WIDTH, left_win_height, TRUE) ;

      	    MoveWindow	(	hwnd_splt_we, 	we_pos-SPLT_WIDTH,
                            upper_win_y,
                  			SPLT_WIDTH, left_win_height, TRUE) ;

      	    MoveWindow	(	hwnd_right, 	right_win_x,
                            upper_win_y,
                  			right_win_width, upper_win_height, TRUE) ;

      	    MoveWindow	(	hwnd_splt_ns, 	right_win_x, ns_pos-SPLT_WIDTH,
                right_win_width, SPLT_WIDTH, TRUE) ;

      	    MoveWindow	(	hwnd_bottom, right_win_x,    ns_pos,
                  			right_win_width, bottom_win_height, TRUE) ;

      	    MoveWindow	(	hwnd_statusbar, 	0, cyClient-statusbar_height,
                cxClient, statusbar_height, TRUE) ;

#endif
          	return 0 ;

        case WM_INITMENU:

            if (lParam == 0)
            {
                hMenu = GetMenu(hwnd);
                hMenu = GetSubMenu(hMenu, 0);
                hMenu = GetSubMenu(hMenu, 8);
                populate_recent_cfg_files(hMenu);

                hMenu = GetMenu(hwnd);
                hMenu = GetSubMenu(hMenu, 0);
                hMenu = GetSubMenu(hMenu, 9);
                populate_recent_pcap_files(hMenu);

            }


            return 0;

        case WM_DROPFILES :
            DragQueryFile((HDROP)wParam,
                          0,
                          file_to_open,
                          sizeof(file_to_open));
            DragFinish((HDROP)wParam);
            
            if (strstr(file_to_open, "."CFG_FILE_SUFFIX)!=NULL)
                open_file();
            else
            {
                strcpy(pcap_file_to_view, file_to_open);
                DialogBox(g_hInstance, TEXT("PKT_VIEW_DLG"), hwnd, PktViewDlgProc);
            }
            return 0;

        case 	WM_COMMAND:
            hMenu = GetMenu (hwnd) ;
            item_id = LOWORD(wParam);
            switch (item_id)
            {
                case    IDM_APP_ABOUT:
                DialogBox (g_hInstance, TEXT("ABOUTBOX"), hwnd, AboutDlgProc) ;
               	return 0 ;

                case    IDM_USER_MANUAL:
                ShellExecute(NULL, "open"
                    , "http://blog.csdn.net/crazycoder8848/article/details/38964007"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_USER_MANUAL_LINUX:
                ShellExecute(NULL, "open"
                    , "http://blog.csdn.net/crazycoder8848/article/details/47209427"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;
                
                case    IDM_GET_NEW_VERSION:
                ShellExecute(NULL, "open"
                    , "http://sourceforge.net/projects/xb-ether-tester/files/latest/download?source=directory"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_GET_SOURCE:
                ShellExecute(NULL, "open"
                    , "https://github.com/sunmingbao/xb-ether-tester/archive/master.zip"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_OPEN_OFFICIAL_SITE:
                ShellExecute(NULL, "open"
                    , "http://sunmingbao.freevar.com/"
                    , NULL, NULL, SW_SHOWNORMAL);

               	return 0 ;

                case    IDM_APP_EXIT:
       				SendMessage (hwnd, WM_CLOSE, 0, 0) ;
       				return 0 ;

                case    IDM_APP_BOTTOM_WIN:
                    if (display_bottom)
                    {
                        display_bottom = 0;
           				ShowWindow(hwnd_bottom, 0);
                        ShowWindow(hwnd_splt_ns, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;

                    }
                    else
                    {
                        display_bottom= 1;
           				ShowWindow(hwnd_bottom, 1);
                        ShowWindow(hwnd_splt_ns, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;

                case    IDM_APP_LEFT_WIN:
                    if (display_left)
                    {
                        display_left = 0;
           				ShowWindow(hwnd_left, 0);
                        ShowWindow(hwnd_splt_we, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;



                    }
                    else
                    {
                        display_left= 1;
           				ShowWindow(hwnd_left, 1);
                        ShowWindow(hwnd_splt_we, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;

                case    IDM_APP_TOOLBAR_WIN:
                    if (display_toolbar)
                    {
                        display_toolbar = 0;
           				ShowWindow(hwnd_toolbar, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;



                    }
                    else
                    {
                        display_toolbar= 1;
           				ShowWindow(hwnd_toolbar, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;

                case    IDM_APP_STATUSBAR_WIN:
                    if (display_statusbar)
                    {
                        display_statusbar = 0;
           				ShowWindow(hwnd_statusbar, 0);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_UNCHECKED) ;



                    }
                    else
                    {
                        display_statusbar= 1;
           				ShowWindow(hwnd_statusbar, 1);
                        UpdateWindow (hwnd_frame) ;
                        CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    }
                    resize_window(hwnd_frame);
       				return 0 ;

                case    IDM_APP_QUERY_CLR_STATS:
                    strcpy(auto_clr_stats, "query");
                    WritePrivateProfileString("before_send_pkt", "clear_stat", auto_clr_stats, APP_PROFILE_FILE);
                    CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_CLR_STATS, MF_UNCHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_NOT_CLR_STATS, MF_UNCHECKED) ;

       		        return 0 ;

                case    IDM_APP_CLR_STATS:
                    strcpy(auto_clr_stats, "yes");
                    WritePrivateProfileString("before_send_pkt", "clear_stat", auto_clr_stats, APP_PROFILE_FILE);
                    CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_QUERY_CLR_STATS, MF_UNCHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_NOT_CLR_STATS, MF_UNCHECKED) ;
       		        return 0 ;

                case    IDM_APP_NOT_CLR_STATS:
                    strcpy(auto_clr_stats, "no");
                    WritePrivateProfileString("before_send_pkt", "clear_stat", auto_clr_stats, APP_PROFILE_FILE);
                    CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_QUERY_CLR_STATS, MF_UNCHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_CLR_STATS, MF_UNCHECKED) ;
       		        return 0 ;

                case    IDM_APP_QUERY_SAVE_CAPTURED_PKS:
                    strcpy(query_save_captured_pkts, "yes");
                    WritePrivateProfileString("before_send_pkt", "query_save_captured_pkts", query_save_captured_pkts, APP_PROFILE_FILE);
                    CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_CLR_CAPTURED_PKS, MF_UNCHECKED) ;
       		        return 0 ;

                case    IDM_APP_QUERY_SAVE_CAPTURED_PKS_2:
                    if (strcmp(query_save_captured_pkts_2, "no")==0)
                    {
                        strcpy(query_save_captured_pkts_2, "yes");
                        CheckMenuItem (hMenu, IDM_APP_QUERY_SAVE_CAPTURED_PKS_2, MF_CHECKED) ;
                    }
                    else
                    {
                        strcpy(query_save_captured_pkts_2, "no");
                        CheckMenuItem (hMenu, IDM_APP_QUERY_SAVE_CAPTURED_PKS_2, MF_UNCHECKED) ;
                    }
                    WritePrivateProfileString("before_only_cap_pkt", "query_save_captured_pkts", query_save_captured_pkts_2, APP_PROFILE_FILE);

       		        return 0 ;

                case    IDM_APP_CLR_CAPTURED_PKS:
                    strcpy(query_save_captured_pkts, "no");
                    WritePrivateProfileString("before_send_pkt", "query_save_captured_pkts", query_save_captured_pkts, APP_PROFILE_FILE);
                    CheckMenuItem (hMenu, item_id, MF_CHECKED) ;
                    CheckMenuItem (hMenu, IDM_APP_QUERY_SAVE_CAPTURED_PKS, MF_UNCHECKED) ;
       		        return 0 ;

                case    IDM_NEW_VERSION_NOTICE:
                    if (strcmp(new_version_notice, "no")==0)
                    {
                        strcpy(new_version_notice, "yes");
                        CheckMenuItem (hMenu, IDM_NEW_VERSION_NOTICE, MF_CHECKED) ;
                        delete_file_f(VER_UPDATE_NOTICE_RCD);
                        launch_thread(ver_update, NULL);
                    }
                    else
                    {
                        strcpy(new_version_notice, "no");
                        CheckMenuItem (hMenu, IDM_NEW_VERSION_NOTICE, MF_UNCHECKED) ;
                    }
                    WritePrivateProfileString("update", "new_version_notice", new_version_notice, APP_PROFILE_FILE);

       		        return 0 ;

                case    IDT_TOOLBAR_START:
                    if (0==need_stop) return 0;

                    if (nr_cur_stream<=0)
                    {
                        WinPrintf(hwnd, TEXT("please create flow(s) first"));
                        //SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_START, 0);
                        return 0 ;
                    }

                    if ((nr_stream2snd = GetSelCnt(hwnd_lv))<=0)
                    {
                        WinPrintf(hwnd, TEXT("please select flow(s) to TX"));
                        //SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_START, 0);
                        return 0 ;
                    }

                    if (stats_captured_pkts_proc())
                    {
                        return 0;
                    }

//PREPARE_LAUNCH:
                    need_stop=snd_stopped=rcv_stopped=0;
                    snd_started=rcv_started=0;

                    ui_switch(1);
                    cp_stream2sndbuf();
                    rule_fileds_init_all_pkt();
                    update_fc_gap();

                    gettimeofday(&last_timer_tv, NULL);
                    last_stat_tv_snd=last_timer_tv;
                    last_stat_tv_rcv=last_timer_tv;
                    last_stat_tv_sndf=last_timer_tv;
                    SetTimer(hwnd_frame, TIMER_STATUS_BAR, TIMER_STATUS_GAP, NULL);
                    launch_thread(wpcap_rcv_test, NULL);
                    //while (!rcv_started) Sleep(1);
                    launch_thread(wpcap_snd_test, NULL);

               	return 0 ;

                case    IDT_TOOLBAR_STOP:
                if ((1==need_stop) || (snd_stopped && rcv_stopped))  return 0;
                need_stop=1;
                //SendMessage(hwnd_toolbar, TB_ENABLEBUTTON, IDT_TOOLBAR_START, 1);
                ui_switch(0);
                //SendMessage(hwnd_toolbar, TB_ENABLEBUTTON, IDT_TOOLBAR_STOP, 1);
                //SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_STOP, 1);

                while (!snd_stopped || !rcv_stopped) Sleep(1);
                SendMessage(hwnd, WM_TIMER, TIMER_STATUS_BAR, 0);
               	return 0 ;

                case    IDT_TOOLBAR_CAPTURE:
                    DialogBox(g_hInstance, TEXT("PKT_CAP_DLG"), hwnd, PktCapDlgProc);
       				return 0 ;

                case    IDT_TOOLBAR_CAP_WHILE_SND:
                need_capture=!need_capture;
                SendMessage(hwnd_toolbar, TB_CHECKBUTTON, IDT_TOOLBAR_CAP_WHILE_SND, need_capture);
                update_statusbar();
               	return 0 ;
                
                case    IDT_TOOLBAR_VIEW_CAPTURE:
                    if (!cap_save_cnt|| !file_exists(PKT_CAP_FILE_WHILE_SND))
                    {
                        WinPrintf(hwnd, TEXT("no packets captured yet"));
                        return 0;
                    }
                    strcpy(pcap_file_to_view, PKT_CAP_FILE_WHILE_SND);
                    DialogBox(g_hInstance, TEXT("PKT_VIEW_DLG"), hwnd, PktViewDlgProc);
       				return 0 ;


                case    IDM_FILE_SAVE:
                {
                    if (cfg_file_path[0]==0)
                    {
                        SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE_AS, 0);
                        return 0 ;
                    }
                    save_stream(cfg_file_path);
                   	return 0 ;
                }

                case    IDM_FILE_SAVE_AS:
                {
                        ret=get_save_file_name(file_to_open, hwnd, CFG_FILE_FILTER, CFG_FILE_SUFFIX);
                        if (ret) return 0 ;
                        save_stream(file_to_open);
                        open_file();
                       	return 0 ;
                }

                case    IDM_FILE_NEW:
                {
                    if (doc_save_proc()) return 0;
                    new_cfg();
                    return 0 ;
                }
                
                case    IDM_FILE_OPEN:
                {
                    if (get_open_file_name(file_to_open, hwnd, CFG_FILE_FILTER))
                        return 0;
                    open_file();
                   	return 0 ;
                }

                case    ID_FILE_RECENT_CFG_FILE_BEGIN+0:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+1:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+2:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+3:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+4:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+5:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+6:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+7:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+8:
                case    ID_FILE_RECENT_CFG_FILE_BEGIN+9:
                {
                    if (get_history_cfg_file_by_idx(item_id - ID_FILE_RECENT_CFG_FILE_BEGIN+1
                        ,file_to_open))
                    {
                        WinPrintf(hwnd, TEXT("reading historical file items failed."));
                        return 0;
                    }

                    if (file_exists(file_to_open)) 
                        open_file();
                    else
                        WinPrintf(hwnd, TEXT("file %s does not exist"), file_to_open);
                    return 0 ;

                }
                    
                case    IDM_VIEW_PCAP_FILE:
                {
                    if (0==get_open_file_name(file_to_open, hwnd
                        , "tcpdump arch(*.pcap, *.cap)\0*.pcap;*.cap\0"
                        "All Files(*.*)\0*.*\0\0"))
                    {
                        strcpy(pcap_file_to_view, file_to_open);
                        update_pcap_file_history(pcap_file_to_view);
                        DialogBox(g_hInstance, TEXT("PKT_VIEW_DLG"), hwnd, PktViewDlgProc);
                    }
                   	return 0 ;
                }

                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+0:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+1:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+2:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+3:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+4:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+5:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+6:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+7:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+8:
                case    ID_FILE_RECENT_PCAP_FILE_BEGIN+9:
                {
                    if (get_history_pcap_file_by_idx(item_id - ID_FILE_RECENT_PCAP_FILE_BEGIN+1
                        ,file_to_open))

                    return 0 ;

                    if (file_exists(file_to_open)) 
                    {
                        strcpy(pcap_file_to_view, file_to_open);
                        update_pcap_file_history(pcap_file_to_view);
                        DialogBox(g_hInstance, TEXT("PKT_VIEW_DLG"), hwnd, PktViewDlgProc);
                    }
                    else
                        WinPrintf(hwnd, TEXT("file %s does not exist"), file_to_open);
                    return 0 ;

                }


            }

            return 0;

        case 	WM_INITMENUPOPUP:
        {

            if (lParam == 0)
            {
                //EnableMenuItem ((HMENU) wParam, IDM_APP_QUERY_CLR_STATS, !query_clr_stats ? MF_ENABLED : MF_GRAYED);
                //EnableMenuItem ((HMENU) wParam, IDM_APP_CLR_STATS, !query_clr_stats ? MF_ENABLED : MF_GRAYED);
                //EnableMenuItem ((HMENU) wParam, IDM_APP_NOT_CLR_STATS, !query_clr_stats ? MF_ENABLED : MF_GRAYED);

                return 0;

            }

            break;
        }

        case	WM_TIMER:
        {
            if (snd_stopped && rcv_stopped)
            {
                KillTimer (hwnd, TIMER_STATUS_BAR) ;
            }
            
            update_stats();

            return 0;

        }

     	case	WM_CLOSE:
            if (doc_save_proc()) return 0;
            DestroyWindow (hwnd) ;
            return 0 ;

        case    WM_DESTROY :
            PostQuitMessage (0) ;
            return 0 ;
    }
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_frame()
{
    WNDCLASS    wndclass;
    wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc= WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance  = g_hInstance;
    wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    //wndclass.hCursor    = LoadCursor (g_hInstance, TEXT("my_cursor"));
    wndclass.hCursor      = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground  = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wndclass.lpszMenuName   = TEXT("MY_MENU");
    wndclass.lpszClassName  = szFrameWinClassName;

    if (!RegisterClass (&wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}
int reg_sys_win_classes()
{
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_STANDARD_CLASSES|ICC_BAR_CLASSES|ICC_TAB_CLASSES
        |ICC_TREEVIEW_CLASSES|ICC_LISTVIEW_CLASSES;


    InitCommonControlsEx(&icex);
    InitCommonControls();
    return 0;
}


int create_windows(int iCmdShow)
{
    hwnd_frame = CreateWindow (szFrameWinClassName, szAppName,
            WS_OVERLAPPEDWINDOW|WS_MAXIMIZE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, g_hInstance, NULL) ;

    ShowWindow (hwnd_frame, SW_MAXIMIZE) ;
    UpdateWindow (hwnd_frame) ;

    return 0;

}

void resize_window(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd,&rect);
    SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.right, rect.bottom));
}

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    TCHAR info[1024];

    sprintf(info
        , "%s    V%c.%c.%c\r\n"
        "(build time: %s) \r\n\r\n"
        "author: Mingbao Sun \r\n\r\n"
        "======================\r\n"
        "This is a free and open-source software.\r\n"
        "Anyone can use and distribute the software freely in any form without any restriction.\r\n"
        "======================\r\n"
        
        "This software is an ethernet tester based on NIC(s) on PC.\r\n"
        "It sends and captures packets through winpcap.\r\n\r\n"
        "======================\r\n"
        "welcome to contact the author for any issue about this software.\r\n"
        "sunmingbao@126.com\r\n"

        , szAppName
        , version[0] , version[1] , version[2]
        , BUILD_TIME);

     	switch (message)
     	{
     	case 	WM_INITDIALOG :
                center_child_win(hwnd_frame, hDlg);
                SetDlgItemText(hDlg, ID_ABOUT_INFO, info);
                SetFocus(GetDlgItem(hDlg, IDOK));
          		return FALSE ;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
          		switch (LOWORD (wParam))
          		{

                case 	ID_AB_SOURCE:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_GET_SOURCE, 0);
               		return TRUE ;
               		
               	case 	ID_AB_SITE:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_OPEN_OFFICIAL_SITE, 0);
               		return TRUE ;
               		
               	case 	ID_AB_UPDATE:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_GET_NEW_VERSION, 0);
               		return TRUE ;
                    
               	case 	ID_AB_LINUX:
               		SendMessage(hwnd_frame, WM_COMMAND, IDM_USER_MANUAL_LINUX, 0);
               		return TRUE ;
                    
          		case 	IDOK :
          		case 	IDCANCEL :
               				EndDialog (hDlg, 0) ;
               				return TRUE ;
          }
          		break ;
     }
  	return FALSE ;
}

void add_tip(HWND htip_control, HWND hwnd, TCHAR *tips)
{
    TOOLINFO        ti;

    ti.cbSize = sizeof(TOOLINFO);
    //Flags TTF_IDISHWND show that the uID is the handle to control
    ti.uFlags =  TTF_IDISHWND | TTF_SUBCLASS;  

    //Handle of the Control for which you want to Tooltip to be displayed
    ti.uId    =(UINT)hwnd;

    //Handle of the parent window (The window in which the Control resides)
    ti.hwnd   = hwnd;
    ti.hinst  = g_hInstance;
    ti.lpszText  = tips;         //Text you want as a Tooltip
    	//Below is the Rectangle for ToolTip. But It will be ignored 
    	//If you use TTF_IDISHWND in Flag
    //ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

    //Will Activate the tooltip window
    SendMessage(htip_control,TTM_ACTIVATE,TRUE,0);  

    //will Add the Tooltip Window
    SendMessage(htip_control,TTM_ADDTOOL,0,(LPARAM)&ti);
}


