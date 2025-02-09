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
#include <time.h>

#include "common.h"
#include "global_info.h"
#include "res.h"
#include "gui.h"

TCHAR szBottomWinClassName[] = TEXT ("bottom_win") ;
HWND    hwnd_bottom;

HWND  hwnd_button_bottom_close;
#define    ID_BUTTON_BOTTOM_CLOSE    (45002)

HWND    hwnd_b_we_splitter;
int     b_we_pos;

HWND    hwnd_b2_tab;
HWND    hwnd_b2_tab1;

TCHAR *tab_b2_icons[] = {TEXT("info_icon")};
TCHAR *tab_b2_labels[] = {TEXT("Info")};

HWND    hwnd_b_tab;
HWND    hwnd_b_tab1;
TCHAR sz_bTab1WinClassName[] = TEXT ("tab1_win") ;

TCHAR *tab_b_icons[] = {TEXT("stats_icon")};
TCHAR *tab_b_labels[] = {TEXT("statistics")};
int err_cnt, info_cnt;
LRESULT CALLBACK bottom_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT		rect ;
    int cxClient, cyClient;
    
    switch (message)
    {
        case WM_CREATE:
            hwnd_bottom = hwnd;
            hwnd_button_bottom_close =	CreateWindow ( TEXT("button"),TEXT("X"),
   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    20, 20, 30, 30,
    hwnd, (HMENU)ID_BUTTON_BOTTOM_CLOSE,
               g_hInstance, NULL) ;

#if 0
           hwnd_b_we_splitter = CreateWindow (szSpltWeClassName, TEXT ("sub win"),
                WS_CHILD|WS_VISIBLE,
                SPLT_WIDTH,   point.y,  SPLT_WIDTH, SPLT_WIDTH,
                hwnd, NULL, g_hInstance, NULL) ;
#endif
#if 1
            hwnd_b_tab=DoCreateTabControl(g_hInstance, hwnd_bottom
                ,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_BOTTOM
                , ARRAY_SIZE(tab_b_icons), tab_b_icons, tab_b_labels);
#else
            hwnd_b_tab=DoCreateFlatTabControl(g_hInstance, hwnd_bottom
                ,szFlatTabWinClassName
                , ARRAY_SIZE(tab_b_labels), tab_b_labels);
#endif
            hwnd_b_tab1=create_tab_win(g_hInstance, hwnd_bottom, szStatsWinClassName, WS_CHILD | WS_VISIBLE | WS_BORDER);
#if 0
            hwnd_b2_tab=DoCreateTabControl(g_hInstance, hwnd_bottom
                ,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_BOTTOM
                , ARRAY_SIZE(tab_b2_icons), tab_b2_icons, tab_b2_labels);

            hwnd_b2_tab1=create_tab_win(g_hInstance, hwnd_bottom, TEXT("edit")
                , WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN);
#endif
            return 0 ;

        case WM_SPLITTER_X:
        {
            b_we_pos=wParam;
            resize_window(hwnd);
            return 0 ;
        }

        case WM_SIZE:

      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;
            
            MoveWindow	(hwnd_button_bottom_close, 	cxClient-25, 0,
                25, 25, TRUE) ;



#if 1

            MoveWindow	(hwnd_b_tab, 	0, 0,
                cxClient-25, cyClient, TRUE) ;
            GetClientRect(hwnd_b_tab,&rect);
            TabCtrl_AdjustRect(hwnd_b_tab, FALSE, &rect); 

            MoveWindow(hwnd_b_tab1, 	0, 0,
                rect.right-rect.left, rect.bottom-rect.top, TRUE) ;
#else
            if (b_we_pos==0) b_we_pos = cxClient-100;

            //tab_height=win_height(hwnd_b_tab);
            MoveWindow	(hwnd_b_tab, 	0, cyClient-flat_tab_height,
                cxClient-25, flat_tab_height, TRUE) ;

            MoveWindow(hwnd_b_tab1, 	0, 0,
                cxClient-25, cyClient-flat_tab_height, TRUE) ;
            MoveWindow(hwnd_b_tab2, 	0, 0,
                cxClient-25, cyClient-flat_tab_height, TRUE) ;

      	    MoveWindow	(hwnd_b_we_splitter, 	b_we_pos-SPLT_WIDTH,
                            0,
                  			SPLT_WIDTH, cyClient, TRUE) ;

            MoveWindow	(hwnd_b2_tab, 	b_we_pos, 0,
                cxClient-b_we_pos-25, cyClient, TRUE) ;
            GetClientRect(hwnd_b2_tab,&rect);
            TabCtrl_AdjustRect(hwnd_b2_tab, FALSE, &rect); 

            MoveWindow(hwnd_b2_tab1, 	b_we_pos, 0,
                cxClient-b_we_pos-25, rect.bottom-rect.top, TRUE) ;

#endif


            return 0 ;

            
        case 	WM_COMMAND:
            switch (LOWORD(wParam))
            {
                  
                case    ID_BUTTON_BOTTOM_CLOSE:
                    SetFocus(hwnd_right);
       				SendMessage(hwnd_frame, WM_COMMAND, MAKEWPARAM(IDM_APP_BOTTOM_WIN, 0),0);
       				return 0 ;
             }
            break;

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
                    int iPage = TabCtrl_GetCurSel(hwnd_b_tab); 

                    if (0==iPage)
                    {
                        ShowWindow (hwnd_b_tab1, 1) ;
                        ShowWindow (hwnd_b2_tab1, 0) ;

                    }
                    else
                    {
                        ShowWindow (hwnd_b_tab1, 0) ;
                        ShowWindow (hwnd_b2_tab1, 1) ;

                    }
                    return 0;
                } 
        }

        break;

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_bottom_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= bottom_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szBottomWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

void err_msg_box(TCHAR * szFormat, ...)
{
    TCHAR   szBuffer[1024] ;
    va_list pArgList ;
    int len;

    va_start(pArgList, szFormat) ;
    len=_vsntprintf(szBuffer, sizeof (szBuffer) / sizeof (TCHAR), 
			szFormat, pArgList) ;

    va_end (pArgList) ;
    MessageBox (hwnd_frame, szBuffer, szAppName, MB_ICONERROR) ;
}

#if 0

void edit_append_text(HWND hwnd, TCHAR   *szBuffer)
{
    int ndx = GetWindowTextLength (hwnd);
    SendMessage (hwnd, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
    SendMessage (hwnd, EM_REPLACESEL, 0, (LPARAM) ((LPSTR) szBuffer));

}

void WriteInfo(TCHAR * szFormat, ...)
{
	TCHAR   szBuffer [1024] ;
	va_list pArgList ;
    int len1, len2;
    time_t log_time = time(NULL);

    len1 = strftime(szBuffer,  sizeof(szBuffer), "[%Y%m%d %H:%M:%S]: ", localtime(&log_time));

	va_start (pArgList, szFormat) ;

	// The last argument to wvsprintf points to the arguments

	len2=_vsntprintf (	szBuffer+len1, sizeof (szBuffer) / sizeof (TCHAR), 
			szFormat, pArgList) ;

	// The va_end macro just zeroes out pArgList for no good reason
	va_end (pArgList) ;
    szBuffer[len1 + len2] = '\r';
    szBuffer[len1 + len2 + 1] = '\n';
    szBuffer[len1 + len2 + 2] = '\0';
    
    edit_append_text(hwnd_b2_tab1, szBuffer);

}

void PrintText(TCHAR * szFormat, ...)
{
	TCHAR   szBuffer [1024] ;
	va_list pArgList ;
    int len;

	va_start (pArgList, szFormat) ;

	// The last argument to wvsprintf points to the arguments

	len=_vsntprintf (	szBuffer, sizeof (szBuffer) / sizeof (TCHAR), 
			szFormat, pArgList) ;

	// The va_end macro just zeroes out pArgList for no good reason
	va_end (pArgList) ;
    //szBuffer[len]='\r';
    //szBuffer[len+1]='\n';
    //szBuffer[len+2]=0;
    
    edit_append_text(hwnd_b2_tab1, szBuffer);

}
#endif
