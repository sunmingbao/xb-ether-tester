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

TCHAR szTipWinClassName[] = TEXT ("TipWin");
HWND    hwnd_tip;
TCHAR tip_info[128]={"this is a tip. 这是一条提示"};

#define    TIP_WIN_WIDTH    (700)
#define    TIP_WIN_HEIGHT   (cyChar*2)

LRESULT CALLBACK Tip_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps ;
	RECT		rect ;
    TRACKMOUSEEVENT tme;   


    switch (message)
    {
        case WM_CREATE:
            AnimateWindow(hwnd, 2000, AW_BLEND);
            SetWindowPos(hwnd, HWND_TOP
                , (scrn_width-TIP_WIN_WIDTH)/2,(scrn_height-TIP_WIN_HEIGHT)/2
                ,TIP_WIN_WIDTH,TIP_WIN_HEIGHT
                , 0);

            return 0 ;


        case WM_PAINT :
        {
            hdc = BeginPaint (hwnd, &ps) ;
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT)) ;
            GetClientRect (hwnd, &rect) ;

                SetBkColor(hdc, RGB(0x0,0x0,0x0)) ;
                SetTextColor(hdc, RGB(0xff,0xff,0xff)) ;


            DrawText (hdc, tip_info, -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

            DeleteObject(SelectObject (hdc, GetStockObject(SYSTEM_FONT))) ;
  			EndPaint (hwnd, &ps) ;

			return 0 ;
        }

        case WM_TIMER:
        {
            AnimateWindow(hwnd_tip, 2000, AW_BLEND|AW_HIDE);
            KillTimer (hwnd, TIMER_TIP_WIN) ;
            return 0 ;

        }
        
        case WM_MOUSELEAVE:
            SetTimer(hwnd, TIMER_TIP_WIN, TIMER_TIP_WIN_GAP, NULL);
            return 0 ;

        case WM_MOUSEMOVE:
            ShowWindow(hwnd, 1);
            KillTimer (hwnd, TIMER_TIP_WIN) ;
            tme.cbSize=sizeof(TRACKMOUSEEVENT); //监控鼠标离开   
            tme.dwFlags=TME_LEAVE;   
            tme.hwndTrack=hwnd;  
            TrackMouseEvent(&tme);
            return 0 ;

}
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}



int register_tip_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= Tip_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hCursor    = LoadCursor(NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szTipWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}


void show_tip(TCHAR *info)
{
    strcpy(tip_info, info);
    SetWindowPos(hwnd_tip, HWND_TOP
                , (scrn_width-TIP_WIN_WIDTH)/2,(scrn_height-TIP_WIN_HEIGHT)/2
                ,TIP_WIN_WIDTH,TIP_WIN_HEIGHT
                , 0);
    InvalidateRect(hwnd_tip, NULL, TRUE) ;
    ShowWindow(hwnd_tip, 1);
    SetTimer(hwnd_tip, TIMER_TIP_WIN, TIMER_TIP_WIN_GAP, NULL);
}






