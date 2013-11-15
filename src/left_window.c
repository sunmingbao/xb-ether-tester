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
#include <windowsx.h>
#include "common.h"
#include "global_info.h"
#include "res.h"

TCHAR szSubWinClassName[] = TEXT ("left_win") ;
HWND    hwnd_left;

HWND    hwnd_tab;
HWND    hwnd_tab1;
TCHAR szTab1WinClassName[] = TEXT ("tab1_win") ;
HWND    hwnd_tab2;
TCHAR szTab2WinClassName[] = TEXT ("tab2_win") ;

TCHAR *tab_icons[] = {TEXT("my_frame_icon"), TEXT("my_frame_icon")};
TCHAR *tab_labels[] = {TEXT("tab1"), TEXT("tab2")};

LRESULT CALLBACK tab1_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK tab2_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND  hwnd_button;
#define    ID_BUTTON    (45000)

HWND  hwnd_button_left_close;
#define    ID_BUTTON_LEFT_CLOSE    (45001)

HWND  hwnd_tv;
#define    ID_TV    (45003)



LRESULT CALLBACK sub_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    HWND    hwnd_sub ;
    RECT		rect ;
    int cxClient, cyClient;

    
    switch (message)
    {
        case WM_CREATE:
            hwnd_left = hwnd;
                        hwnd_button_left_close =	CreateWindow ( TEXT("button"),TEXT("X"),
   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    20, 20,
    30, 30,
                               hwnd, (HMENU)ID_BUTTON_LEFT_CLOSE,
               g_hInstance, NULL) ;

            hwnd_tab=DoCreateTabControl(g_hInstance, hwnd_left
                , WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_BOTTOM
                , ARRAY_SIZE(tab_icons), tab_icons, tab_labels);
            register_tab1_win();
            //register_tab2_win();
            hwnd_tab1=create_tab_win(g_hInstance, hwnd_tab, szTab1WinClassName, WS_CHILD | WS_VISIBLE | WS_BORDER);
            //hwnd_tab2=create_tab_win(g_hInstance, hwnd_tab, szTab2WinClassName, WS_CHILD | WS_BORDER);
            hwnd_tab2 = hwnd_tab1;
            return 0 ;

        case WM_SIZE:
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

            MoveWindow	(hwnd_button_left_close, 	cxClient-25, 0,
                25, 25, TRUE) ;

      	    MoveWindow	(hwnd_tab, 	0, 25,
                cxClient, cyClient-25, TRUE) ;
            GetClientRect(hwnd_tab,&rect);
            TabCtrl_AdjustRect(hwnd_tab, FALSE, &rect); 
#if 0
            MoveWindow(hwnd_tab1, 	0, 0,
                rect.right, rect.bottom, TRUE) ;
            MoveWindow(hwnd_tab2, 	0, 0,
                rect.right, rect.bottom, TRUE) ;
#else
            MoveWindow(hwnd_tab1, 	0, 25,
                cxClient, cyClient-25, TRUE) ;
#endif
            return 0 ;

        case  WM_NOTIFY:
    switch (((LPNMHDR)lParam)->code)
        {
            case TCN_SELCHANGING:
                {
                    // Return FALSE to allow the selection to change.
                    break;
                }

            case TCN_SELCHANGE:
                { 
                    int iPage = TabCtrl_GetCurSel(hwnd_tab); 

                    if (0==iPage)
                    {
                    ShowWindow (hwnd_tab1, 1) ;
                    ShowWindow (hwnd_tab2, 0) ;

                    }
                    else
                    {
                    ShowWindow (hwnd_tab1, 0) ;
                    ShowWindow (hwnd_tab2, 1) ;

                    }
                    return 0;
                } 
        }

        break;

        case 	WM_COMMAND:
            switch (LOWORD(wParam))
            {
                  
                case    ID_BUTTON_LEFT_CLOSE:
                    SetFocus(hwnd_right);
       				SendMessage(hwnd_frame, WM_COMMAND, MAKEWPARAM(IDM_APP_LEFT_WIN, 0),0);
       				return 0 ;
             }
            break;

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_left_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= sub_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    //sub_wndclass.hbrBackground  = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    //sub_wndclass.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(0,0,200));
    sub_wndclass.hbrBackground  = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szSubWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

int register_tab1_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= tab1_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szTab1WinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

void enable_speed_value(HWND hDlg, int is_enable)
{
    EnableWindow(GetDlgItem(hDlg, ID_FC_SPEED_INFO_PREFFIX), is_enable);
    EnableWindow(GetDlgItem(hDlg, ID_FC_SPEED_INFO_SUFFIX), is_enable);
    EnableWindow(GetDlgItem(hDlg, ID_FC_SPEED_VALUE), is_enable);

}

void enable_mode_value(HWND hDlg, int is_enable)
{
    EnableWindow(GetDlgItem(hDlg, ID_FC_SND_MODE_PREFFIX), is_enable);
    EnableWindow(GetDlgItem(hDlg, ID_FC_SND_MODE_SUFFIX), is_enable);
    EnableWindow(GetDlgItem(hDlg, ID_FC_SND_TIMES_CNT), is_enable);

}

void ui_update_fc_cfg(HWND hDlg)
{
    int ret=SendMessage(GetDlgItem(hDlg, ID_FC_SPEED_TYPE)
     , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

    if (ret==SPEED_TYPE_FASTEST)
    {
        enable_speed_value(hDlg, 0);
    }
    else
    {
        enable_speed_value(hDlg, 1);

        if (SPEED_TYPE_HIGH==ret)
        {
            SetDlgItemText(hDlg, ID_FC_SPEED_INFO_PREFFIX, "每秒");
            SetDlgItemText(hDlg, ID_FC_SPEED_INFO_SUFFIX, "次");
        }
        else 
        {
            SetDlgItemText(hDlg, ID_FC_SPEED_INFO_PREFFIX, "每");
            SetDlgItemText(hDlg, ID_FC_SPEED_INFO_SUFFIX, "秒一次");
        }
    }
    ret=SendMessage(GetDlgItem(hDlg, ID_FC_SND_MODE)
     , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

    if (ret==SND_MODE_CONTINUE)
    {
        enable_mode_value(hDlg, 0);
    }
    else
        enable_mode_value(hDlg, 1);


}


BOOL CALLBACK FcCfgDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    TCHAR    info[32];
    int ret;

 	switch (message)
 	{
     	case 	WM_INITDIALOG :
                center_child_win(hwnd_frame, hDlg);

        SendMessage(GetDlgItem(hDlg, ID_FC_SPEED_TYPE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"包率");
        SendMessage(GetDlgItem(hDlg, ID_FC_SPEED_TYPE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"发包间隔");
        SendMessage(GetDlgItem(hDlg, ID_FC_SPEED_TYPE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"最大速率");

        SendMessage(GetDlgItem(hDlg, ID_FC_SND_MODE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"continue");
        SendMessage(GetDlgItem(hDlg, ID_FC_SND_MODE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"burst");

        //sprintf(info, "%d", gt_fc_cfg.speed_value);
        set_int_text(GetDlgItem(hDlg, ID_FC_SPEED_VALUE), gt_fc_cfg.speed_value);
        set_int_text(GetDlgItem(hDlg, ID_FC_SND_TIMES_CNT), gt_fc_cfg.snd_times_cnt);
        SendMessage(GetDlgItem(hDlg, ID_FC_SPEED_TYPE), CB_SETCURSEL, (WPARAM)gt_fc_cfg.speed_type, (LPARAM)0);
        SendMessage(GetDlgItem(hDlg, ID_FC_SND_MODE), CB_SETCURSEL, (WPARAM)gt_fc_cfg.snd_mode, (LPARAM)0);
ui_update_fc_cfg(hDlg);

  		return TRUE ;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
                    {
                            gt_fc_cfg.speed_type=SendMessage(GetDlgItem(hDlg, ID_FC_SPEED_TYPE)
                                , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

                            gt_fc_cfg.snd_mode=SendMessage(GetDlgItem(hDlg, ID_FC_SND_MODE)
                                , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

                            ret=get_int_text(GetDlgItem(hDlg, ID_FC_SPEED_VALUE));

                            if (ret<=0 && gt_fc_cfg.speed_type!=SPEED_TYPE_FASTEST)
                            {
                                WinPrintf(hDlg, "速率值 %d 非法，请输入大于0的数", ret);
                                return TRUE;
                            }

                            gt_fc_cfg.speed_value=ret;
                            ret=get_int_text(GetDlgItem(hDlg, ID_FC_SND_TIMES_CNT));

                            if (ret<=0 && gt_fc_cfg.snd_mode!=SND_MODE_CONTINUE)
                            {
                                WinPrintf(hDlg, "burst值 %d 非法，请输入大于0的数", ret);
                                return TRUE;
                            }

                            gt_fc_cfg.snd_times_cnt = ret;

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
                                snd_gap_s = gt_fc_cfg.speed_value;
                                snd_gap_us = 0;

                            }
                            update_statusbar();
                            doc_modified=1;
                    }

              		case 	IDCANCEL :
               				EndDialog (hDlg, LOWORD(wParam)) ;
               				return TRUE ;

                    case ID_FC_SPEED_TYPE:
                    case ID_FC_SND_MODE:
                        if ( HIWORD(wParam)==CBN_SELCHANGE )
                        {
                            ui_update_fc_cfg(hDlg);
                            return TRUE ;
                        }
                        break ;

              }
      		break ;

     }
  	return FALSE ;
}

void append_filter_str(char *filter_str, char *str)
{
    if (filter_str[0])
        strcat(filter_str, " and");

    strcat(filter_str, " ");
    strcat(filter_str, str);
}

void ui_switch_CAP_CFG_MODE(HWND hDlg, int is_to_normal)
{
    int i;
    int ret;
    for (i=ID_PKT_CAP_STR1; i<=ID_PKT_CAP_DPORT; i++)
    ShowWindow(GetDlgItem(hDlg, i),is_to_normal);
    
    ShowWindow(GetDlgItem(hDlg, ID_PKT_CAP_FILTER_STR),!is_to_normal);
    if (!is_to_normal)
        SetFocus(GetDlgItem(hDlg, ID_PKT_CAP_FILTER_STR));
    else
    {
            ret=SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PKT_TYPE)
             , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

            if (2==ret)
            {
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_SIP), 1);
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_DIP), 1);
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_PROTOCOL), 1);
                ret=SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PROTOCOL)
                 , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0)-1;

                if (ret==IPPROTO_TCP || ret==IPPROTO_UDP)
                {
                    EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_SPORT), 1);
                    EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_DPORT), 1);

                }
                else
                {
                    EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_SPORT), 0);
                    EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_DPORT), 0);

                }

            }
            else
            {
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_SIP), 0);
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_DIP), 0);
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_PROTOCOL), 0);
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_SPORT), 0);
                EnableWindow(GetDlgItem(hDlg, ID_PKT_CAP_DPORT), 0);


            }


    }
    
}


int get_pkt_cap_cfg(HWND hDlg, char *str_filter_output)
{
    int ret;
    char info[32], info_2[32];
    str_filter_output[0]=0;

    gt_pkt_cap_cfg.need_save_capture=button_checked(GetDlgItem(hDlg, ID_PKT_CAP_CFG_SAVE2FILE));

    if (button_checked(GetDlgItem(hDlg, ID_PKT_CAP_CFG_MODE_ADVANCED)))
    {
        gt_pkt_cap_cfg.pkt_cap_cfg_mode=PKT_CAP_CFG_MODE_ADVANCED;
        
        GetDlgItemText(hDlg
        , ID_PKT_CAP_FILTER_STR
        , gt_pkt_cap_cfg.filter_str_usr, sizeof(gt_pkt_cap_cfg.filter_str_usr));
        
        strcpy(str_filter_output, gt_pkt_cap_cfg.filter_str_usr);
        
        goto CHECK_FILTER;

    }
    
    gt_pkt_cap_cfg.pkt_cap_cfg_mode=PKT_CAP_CFG_MODE_NORMAL;

    ret=SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PKT_TYPE)
             , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);

    if (0==ret)
    {
        gt_pkt_cap_cfg.pkt_cap_pkt_type = PKT_CAP_PKT_TYPE_ALL;
        pkt_cap_filter_str[0]=0;
        return 0 ;
    }
    
    if (1==ret)
    {
        gt_pkt_cap_cfg.pkt_cap_pkt_type = PKT_CAP_PKT_TYPE_L2;
        append_filter_str(str_filter_output, "not ip and not ip6");
        goto CHECK_FILTER;
    }
    
    if (2==ret)
    {
        gt_pkt_cap_cfg.pkt_cap_pkt_type = PKT_CAP_PKT_TYPE_IP;
    }

    GetDlgItemText(hDlg, ID_PKT_CAP_SIP, info, sizeof(info));
    if (isdigit(info[0]))
    {
        ip_str2n(&gt_pkt_cap_cfg.pkt_cap_sip, info);
    }
    else
    {
        gt_pkt_cap_cfg.pkt_cap_sip=0;
    }

    GetDlgItemText(hDlg, ID_PKT_CAP_DIP, info, sizeof(info));
    if (isdigit(info[0]))
    {
        ip_str2n(&gt_pkt_cap_cfg.pkt_cap_dip, info);
    }
    else
    {
        gt_pkt_cap_cfg.pkt_cap_dip=0;
    }

    gt_pkt_cap_cfg.pkt_cap_protocol = SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PROTOCOL)
             , (UINT) CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0) - 1;

    if (gt_pkt_cap_cfg.pkt_cap_protocol!=IPPROTO_TCP && gt_pkt_cap_cfg.pkt_cap_protocol!=IPPROTO_UDP)
            goto GET_PORT_OVER;

    GetDlgItemText(hDlg, ID_PKT_CAP_SPORT, info, sizeof(info));
    if (isdigit(info[0]))
        gt_pkt_cap_cfg.pkt_cap_sport=get_int_text(GetDlgItem(hDlg, ID_PKT_CAP_SPORT));
    else
        gt_pkt_cap_cfg.pkt_cap_sport=-1;

    GetDlgItemText(hDlg, ID_PKT_CAP_DPORT, info, sizeof(info));
    if (isdigit(info[0]))
        gt_pkt_cap_cfg.pkt_cap_dport=get_int_text(GetDlgItem(hDlg, ID_PKT_CAP_DPORT));
    else
        gt_pkt_cap_cfg.pkt_cap_dport=-1;

GET_PORT_OVER:
    if (gt_pkt_cap_cfg.pkt_cap_pkt_type == PKT_CAP_PKT_TYPE_IP)
    {
        append_filter_str(str_filter_output, "ip");
        if (gt_pkt_cap_cfg.pkt_cap_sip)
        {
            GetDlgItemText(hDlg, ID_PKT_CAP_SIP, info_2, sizeof(info_2));
            sprintf(info, "src %s", info_2);
            append_filter_str(str_filter_output, info);
        }
        if (gt_pkt_cap_cfg.pkt_cap_dip)
        {
            GetDlgItemText(hDlg, ID_PKT_CAP_DIP, info_2, sizeof(info_2));
            sprintf(info, "dst %s", info_2);
            append_filter_str(str_filter_output, info);
        }

        if (gt_pkt_cap_cfg.pkt_cap_protocol>=0)
        {
            sprintf(info, "proto %d", gt_pkt_cap_cfg.pkt_cap_protocol);
            append_filter_str(str_filter_output, info);
        }

        if (gt_pkt_cap_cfg.pkt_cap_protocol!=IPPROTO_TCP && gt_pkt_cap_cfg.pkt_cap_protocol!=IPPROTO_UDP)
            goto CHECK_FILTER;

        if (gt_pkt_cap_cfg.pkt_cap_sport>=0)
        {
            sprintf(info, "src port %d", gt_pkt_cap_cfg.pkt_cap_sport);
            append_filter_str(str_filter_output, info);
        }

        if (gt_pkt_cap_cfg.pkt_cap_dport>=0)
        {
            sprintf(info, "dst port %d", gt_pkt_cap_cfg.pkt_cap_dport);
            append_filter_str(str_filter_output, info);
        }

    }
    
CHECK_FILTER:
    if (!is_filter_valid(str_filter_output))
    {
        WinPrintf(hDlg, "invalid packet capture filter:%s", str_filter_output);
        return 1 ;
    }
    strcpy(pkt_cap_filter_str, str_filter_output);
    return 0 ;

}


void init_protocol_comb(HWND comb, int add_all)
{
    int i;
    char info[64], info_2[32];
    if (add_all)
        SendMessage(comb,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"all");
    for (i=0;i<=255;i++)
    {
        get_protocol_name(i, info_2);
        if (info_2[0])
            sprintf(info, "%d - %s", i, info_2);
        else
            sprintf(info, "%d", i);
        SendMessage(comb,(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)info);
    }

}

BOOL CALLBACK PktCapCfgDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    TCHAR    info[MAX_FILE_PATH_LEN], info_2[32];
    int ret;


 	switch (message)
 	{
     	case 	WM_INITDIALOG :
                center_child_win(hwnd_frame, hDlg);

                if (0==gt_pkt_cap_cfg.need_save_capture)
                {
                    Button_SetCheck(GetDlgItem(hDlg, ID_PKT_CAP_CFG_STAT_ONLY),BST_CHECKED);
                }
                else
                {
                    Button_SetCheck(GetDlgItem(hDlg, ID_PKT_CAP_CFG_SAVE2FILE),BST_CHECKED);
                    SetFocus(GetDlgItem(hDlg, ID_PKT_CAP_CFG_SAVE2FILE));
                }
                    
                SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PKT_TYPE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"all");
                SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PKT_TYPE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"二层(arp, rarp, ...)");
                SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PKT_TYPE),(UINT) CB_ADDSTRING,(WPARAM) 0,(LPARAM)"IP");

                init_protocol_comb(GetDlgItem(hDlg, ID_PKT_CAP_PROTOCOL), 1);

                SetDlgItemText(hDlg, ID_PKT_CAP_FILTER_STR, gt_pkt_cap_cfg.filter_str_usr);
                    
            //SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_DIRECTION), CB_SETCURSEL, (WPARAM)pkt_cap_dir%PKT_CAP_DIR_ALL, (LPARAM)0);
            SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PKT_TYPE), CB_SETCURSEL, (WPARAM)gt_pkt_cap_cfg.pkt_cap_pkt_type%PKT_CAP_PKT_TYPE_ALL, (LPARAM)0);

            ip_n2str(info, &gt_pkt_cap_cfg.pkt_cap_sip);
            if (0==gt_pkt_cap_cfg.pkt_cap_sip)
                SetDlgItemText(hDlg, ID_PKT_CAP_SIP, "all");
            else
                SetDlgItemText(hDlg, ID_PKT_CAP_SIP, info);

            ip_n2str(info, &gt_pkt_cap_cfg.pkt_cap_dip);
            if (0==gt_pkt_cap_cfg.pkt_cap_dip)
                SetDlgItemText(hDlg, ID_PKT_CAP_DIP, "all");
            else
                SetDlgItemText(hDlg, ID_PKT_CAP_DIP, info);

            SendMessage(GetDlgItem(hDlg, ID_PKT_CAP_PROTOCOL), CB_SETCURSEL, (WPARAM)gt_pkt_cap_cfg.pkt_cap_protocol+1, (LPARAM)0);


            if (-1==gt_pkt_cap_cfg.pkt_cap_sport)
                SetDlgItemText(hDlg, ID_PKT_CAP_SPORT, "all");
            else
                set_int_text(GetDlgItem(hDlg, ID_PKT_CAP_SPORT), gt_pkt_cap_cfg.pkt_cap_sport);
            
            if (-1==gt_pkt_cap_cfg.pkt_cap_dport)
                SetDlgItemText(hDlg, ID_PKT_CAP_DPORT, "all");
            else
                set_int_text(GetDlgItem(hDlg, ID_PKT_CAP_DPORT), gt_pkt_cap_cfg.pkt_cap_dport);

            if (gt_pkt_cap_cfg.pkt_cap_cfg_mode== PKT_CAP_CFG_MODE_NORMAL)
            {
                ui_switch_CAP_CFG_MODE(hDlg, 1);
                Button_SetCheck(GetDlgItem(hDlg, ID_PKT_CAP_CFG_MODE_NORMAL),BST_CHECKED);
            }
            else
            {
                ui_switch_CAP_CFG_MODE(hDlg, 0);
                Button_SetCheck(GetDlgItem(hDlg, ID_PKT_CAP_CFG_MODE_ADVANCED),BST_CHECKED);
            }

  		return FALSE;

        case 	WM_CLOSE:
   				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
   				return TRUE ;

     	case 	WM_COMMAND :
          		switch (LOWORD(wParam))
          		{
              		case 	IDOK :
                    {
                        if (get_pkt_cap_cfg(hDlg, info))
                        {

                            return TRUE;
                        }
                        doc_modified=1;
                    }

              		case 	IDCANCEL :
               				EndDialog (hDlg, LOWORD(wParam)) ;
                            update_statusbar();
               				return TRUE ;
                            

                    case 	ID_PKT_CAP_CFG_MODE_NORMAL:
                    {
                        ui_switch_CAP_CFG_MODE(hDlg, 1);
                        return TRUE ;
                    }

                    case 	ID_PKT_CAP_CFG_MODE_ADVANCED:
                    {
                        ui_switch_CAP_CFG_MODE(hDlg, 0);
                        return TRUE ;
                    }
                    
                    case 	ID_PKT_CAP_PKT_TYPE:
                    case    ID_PKT_CAP_PROTOCOL:
                    {
                        if (HIWORD(wParam)==CBN_SELCHANGE)
                        ui_switch_CAP_CFG_MODE(hDlg, 1);
                        return TRUE ;
                    }


              }
      		break ;

     }
  	return FALSE ;
}

HTREEITEM parentItem,childItem1, childItem2, childItem3;

LRESULT CALLBACK tab1_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    HWND    hwnd_sub ;
    RECT		rect ;
    int cxClient, cyClient;
    int ret;

    
    switch (message)
    {
        case WM_CREATE:
        {
            HIMAGELIST imageList;
            HICON icon;

            hwnd_tv =	CreateWindow (WC_TREEVIEW,TEXT("haha"),
   WS_CHILD | WS_VISIBLE|TVS_HASLINES|TVS_HASBUTTONS|TVS_LINESATROOT|TVS_SHOWSELALWAYS,
    20, 20,
    50, 50,
                               hwnd, (HMENU)ID_TV,
               g_hInstance, NULL) ;

            imageList = ImageList_Create(32,
                                          32,
                                          ILC_COLORDDB | ILC_MASK,
                                          4,
                                          0);

// load icons and add them to ImageList
// PARAMS: hInstance, filename, type, width, height, flag
icon = (HICON)LoadImage(g_hInstance,
                          TEXT("config_icon"),
                          IMAGE_ICON, 32, 32,
                          LR_DEFAULTCOLOR);
ImageList_AddIcon(imageList, icon);
icon = (HICON)LoadImage(g_hInstance,
                          TEXT("stream_icon"),
                          IMAGE_ICON, 32, 32,
                          LR_DEFAULTCOLOR);
ImageList_AddIcon(imageList, icon);
icon = (HICON)LoadImage(g_hInstance,
                          TEXT("speed_icon"),
                          IMAGE_ICON, 32, 32,
                          LR_DEFAULTCOLOR);
ImageList_AddIcon(imageList, icon);
icon = (HICON)LoadImage(g_hInstance,
                          TEXT("cap_cfg_icon"),
                          IMAGE_ICON, 32, 32,
                          LR_DEFAULTCOLOR);
ImageList_AddIcon(imageList, icon);
SendMessage(hwnd_tv, TVM_SETIMAGELIST,
                  (WPARAM)TVSIL_NORMAL, (LPARAM)imageList);

    // add root item
    parentItem = insertItem(hwnd_tv, TEXT("配置"), TVI_ROOT, TVI_LAST, 0, 0, NULL);

    // add some children
    childItem1 = insertItem(hwnd_tv, TEXT("流"), parentItem, TVI_LAST, 1, 1, NULL);
    childItem2 = insertItem(hwnd_tv, TEXT("流控"), parentItem, TVI_LAST, 2, 2, NULL);
    childItem3 = insertItem(hwnd_tv, TEXT("抓包"), parentItem, TVI_LAST, 3, 3, NULL);

    TreeView_Expand(hwnd_tv, parentItem, TVM_EXPAND);


            ShowWindow (hwnd_tv, 1) ;
            UpdateWindow (hwnd_tv) ;
    TreeView_Select(hwnd_tv, childItem1, TVGN_CARET);

            return 0 ;
        }

        case WM_SIZE:
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            MoveWindow(hwnd_tv, 	0, 0,
                cxClient, cyClient, TRUE) ;

            return 0 ;

        case WM_PAINT :
            hdc = BeginPaint (hwnd, &ps) ;
            
            //draw some thing here
            GetClientRect (hwnd, &rect) ;
		    DrawText (hdc, TEXT ("tab1 win!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;
  			EndPaint (hwnd, &ps) ;
			return 0 ;

case WM_NOTIFY:
{
   switch(LOWORD(wParam))
   { 
     case ID_TV:
     // if code == NM_CLICK - Single click on an item
     if(((LPNMHDR)lParam)->code == TVN_SELCHANGED) 
     {
        HTREEITEM Selected;
        TV_ITEM tvi;
        char Text[255]="";
        memset(&tvi,0,sizeof(tvi));
        Selected=(HTREEITEM)SendDlgItemMessage(hwnd_tab1,
           ID_TV,TVM_GETNEXTITEM,TVGN_CARET,(LPARAM)Selected);

       if (!init_over) break;
       
       if (childItem1==Selected)
        {
            ShowWindow (hwnd_stream, 1) ;
            ShowWindow (hwnd_fc, 0) ;
        }

       else if (childItem2==Selected)
        {
            //ShowWindow (hwnd_stream, 0) ;
            //ShowWindow (hwnd_fc, 1) ;
            ret=DialogBox(g_hInstance, TEXT("FC_CFG_DLG"), hwnd, FcCfgDlgProc);

            TreeView_SelectItem(hwnd_tv, childItem1);
        }
      else if (childItem3==Selected)
        {
            //ShowWindow (hwnd_stream, 0) ;
            //ShowWindow (hwnd_fc, 1) ;
            ret=DialogBox(g_hInstance, TEXT("PKT_CAP_CFG_DLG"), hwnd, PktCapCfgDlgProc);

            TreeView_SelectItem(hwnd_tv, childItem1);
        }


     }
   }
break;

}

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}
#if 0
int register_tab2_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= tab2_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(GRAY_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szTab2WinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

LRESULT CALLBACK tab2_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    HWND    hwnd_sub ;
    RECT		rect ;
    int cxClient, cyClient;

    
    switch (message)
    {
        case WM_CREATE:
            hwnd_button =	CreateWindow ( TEXT("button"),TEXT("haha"),
   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    20, 20,
    50, 50,
                               hwnd, (HMENU)ID_BUTTON,
               g_hInstance, NULL) ;

CreateWindow ( TEXT("edit"),TEXT("haha"),
   WS_CHILD | WS_VISIBLE,
    20, 80,
    50, 50,
                               hwnd, NULL,
               g_hInstance, NULL) ;

            return 0 ;

        case WM_SIZE:
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            return 0 ;

        case WM_PAINT :
            hdc = BeginPaint (hwnd, &ps) ;
            
            //draw some thing here
            GetClientRect (hwnd, &rect) ;
		    DrawText (hdc, TEXT ("tab2 win!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;
  			EndPaint (hwnd, &ps) ;
			return 0 ;
            
        case 	WM_COMMAND:
            switch (LOWORD(wParam))
            {
                  
                case    ID_BUTTON:
       				 MessageBox (NULL, TEXT ("haha pressed!"),
          szAppName, MB_ICONERROR) ;;
       				return 0 ;
             }
            break;

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}
#endif
//int a[10]={[0 ... 2]={1,2,3}};
