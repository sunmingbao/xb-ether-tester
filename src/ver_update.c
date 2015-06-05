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
#include <wininet.h>
#include <mmsystem.h>
#include "common.h"
#include "global_info.h"
#include "gui.h"
#include "res.h"
#include "debug.h"

char  new_version_notice[8];

static char new_version[4];

int download_file(const char *url, const char *save_file)
{
    DWORD byteread=0;
    char buffer[100];
    FILE *fp=NULL;
    HINTERNET internetopen;
    HINTERNET internetopenurl;
    BOOL hwrite;
    DWORD written;
    HANDLE createfile;
    BOOL internetreadfile;
    int ret = -1;

dbg_print("==");
    internetopen=InternetOpen("Testing",INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
    if (internetopen==NULL){
        //err_msg_box("Internet open failed!");
        goto err_exit;
    }

dbg_print("=%s", url);
    internetopenurl=InternetOpenUrl(internetopen,url,NULL,0,INTERNET_FLAG_RELOAD,0);
    if (internetopenurl==NULL){
       //err_msg_box("Internet open url failed!");
       goto err_exit;
    }
 dbg_print("==");
    fp=fopen(save_file, "wb");
    if (fp==NULL){
       //err_msg_box("Create File failed!");
       goto err_exit;
    }

dbg_print("==");
    while(1){
       internetreadfile=InternetReadFile(internetopenurl,buffer,sizeof(buffer),&byteread);
       if(byteread==0)    break;
       hwrite=fwrite(buffer,1,byteread,fp);
       if (hwrite!=byteread){
            //err_msg_box("Write to file failed!");
            goto err_exit;
       }
    }
    
    fflush(fp);
    ret = 0;

dbg_print("==");
err_exit:
   if (internetopenurl) InternetCloseHandle(internetopenurl);


   if (internetopen) InternetCloseHandle(internetopen);
   if (fp) fclose(fp);


   return ret;
}




    static  int fix_width;
    static  int fix_height;


#define    COL_NUM    (18)
#define    LINE_NUM    (8)
#define    CLOSE_BUTTOM_SIZE    (25)




LRESULT CALLBACK ver_update_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT rt ;
    HBRUSH hBrush;
    RECT rect ;

    static int timer_msg_cnt_1, timer_msg_cnt_2;
    int cxClient, cyClient, line_num_cur_page, i;
    static int hieght, life;
    static TRACKMOUSEEVENT tme;
    ULONG   ulScrollLines ;
    TCHAR  info[64];
SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);




    switch (message)
    {
        case WM_CREATE:
            CreateWindow ( TEXT("button"),TEXT("X"),
   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    fix_width-CLOSE_BUTTOM_SIZE, 0, CLOSE_BUTTOM_SIZE, CLOSE_BUTTOM_SIZE,
    hwnd, (HMENU)ID_VER_UPDATE_CLOSE_BUTTON,
               g_hInstance, NULL) ;


CreateWindow ( TEXT("button"),TEXT("不用再提醒"),
   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
    (fix_width-12*cxChar_2)/2, CLOSE_BUTTOM_SIZE + 6*cyChar_2+5, cxChar_2*12, cyChar_2+5,
    hwnd, (HMENU)ID_VER_UPDATE_NONEED_NOTICE,
               g_hInstance, NULL) ;


             hieght = 0;
             life = 5;
             timer_msg_cnt_1 = 0;
             timer_msg_cnt_2 = 0;
             SetTimer(hwnd, TIMER_VER_UPDATE_1, TIMER_VER_UPDATE_1_GAP, NULL);


            tme.cbSize      = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags     = TME_HOVER | TME_LEAVE;
            tme.hwndTrack   = hwnd;
            tme.dwHoverTime = 10;
            TrackMouseEvent(&tme);
            //Beep( 750, 300 );
            //PlaySound("Notice", NULL, SND_SYNC);
            //MessageBeep(MB_ICONEXCLAMATION);
             return 0 ;


        case WM_TIMER:
        {
            if (wParam==TIMER_VER_UPDATE_1)
            {
                timer_msg_cnt_1++;
                if (hieght>=fix_height)
                {
                    if (0==(timer_msg_cnt_1%10))
                        refresh_window(hwnd);
                    return 0;
                }


                hieght += fix_height/(1000/TIMER_VER_UPDATE_1_GAP);


                MoveWindow(hwnd, rt.right - fix_width,
                            rt.bottom - hieght,
                  fix_width, fix_height, TRUE) ;

                if (hieght>=fix_height)
                {
                     SetTimer(hwnd, TIMER_VER_UPDATE_2, TIMER_VER_UPDATE_2_GAP, NULL);
                }
                return 0;
            }
            else if (wParam==TIMER_VER_UPDATE_2)
            {
                timer_msg_cnt_2++;
                if (1==timer_msg_cnt_2) return 0;
                
                if (life<=0) return 0;

                life-=1;

                if (life<=0)
                {
                    PostMessage(hwnd, WM_COMMAND, ID_VER_UPDATE_CLOSE_BUTTON, 0);
                }


                return 0;
            }


            return 0;


        }


        case   WM_LBUTTONDOWN:
            PostMessage(hwnd_frame, WM_COMMAND, IDM_GET_NEW_VERSION, 0);
            return 0;


        case   WM_MOUSEMOVE:
            TrackMouseEvent(&tme);
        //dbg_print("===");
            return 0;


        case   WM_MOUSEHOVER:
            KillTimer (hwnd, TIMER_VER_UPDATE_2);
            life = 5;
            return 0;


        case   WM_MOUSELEAVE:
            SetTimer(hwnd, TIMER_VER_UPDATE_2, TIMER_VER_UPDATE_2_GAP, NULL);
            return 0;
            
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
               case    ID_VER_UPDATE_NONEED_NOTICE:
                    if (strcmp(new_version_notice, "yes")==0)
                        PostMessage(hwnd_frame, WM_COMMAND, IDM_NEW_VERSION_NOTICE,0);
                    PostMessage(hwnd, WM_COMMAND, ID_VER_UPDATE_CLOSE_BUTTON, 0);
                    return 0 ;
                  
                case    ID_VER_UPDATE_CLOSE_BUTTON:
                    KillTimer (hwnd, TIMER_VER_UPDATE_1);
                    KillTimer (hwnd, TIMER_VER_UPDATE_2);
                    DestroyWindow(hwnd);
                    return 0 ;
             }
            break;


        case    WM_DESTROY :
            PostQuitMessage(0) ;
            return 0 ;
            
        case WM_PAINT :
            hdc = BeginPaint (hwnd, &ps) ;

            SetRect(&rect, 0, 0,
          fix_width - CLOSE_BUTTOM_SIZE, CLOSE_BUTTOM_SIZE) ;
        hBrush = CreateSolidBrush (
RGB (rand () % 256, rand () % 256, rand () % 256));


   FillRect(hdc, &rect, hBrush);
      DeleteObject (hBrush) ;


            SelectObject(hdc, char_font_2) ;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0x00,0x00,0x00)) ;
            //SetBkColor (hdc, RGB(0xFF,0xFF,0xFF)) ;
            TextOutA(hdc, (COL_NUM-16)*cxChar_2/2, CLOSE_BUTTOM_SIZE + 1*cyChar_2
                , "小兵以太网测试仪", 16) ; 

            sprintf(info, "v%c.%c.%c  发布"
                , new_version[0]
                , new_version[1]
                , new_version[2]);
            SetTextColor(hdc, RGB (0, 0, 255));
            TextOutA(hdc, (COL_NUM-12)*cxChar_2/2, CLOSE_BUTTOM_SIZE + 3*cyChar_2
                , info, 12) ; 

            sprintf(info, "点击下载 %d秒后关闭", life);
            SetTextColor(hdc, RGB(0x00,0x00,0x00)) ;
            TextOutA(hdc, 0, CLOSE_BUTTOM_SIZE + 5*cyChar_2
                , info, 18) ; 


            SelectObject (hdc, GetStockObject(SYSTEM_FONT)) ;
            EndPaint (hwnd, &ps) ;
             return 0 ;
    }


    return DefWindowProc (hwnd, message, wParam, lParam) ;
}




int register_ver_update_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS;
    sub_wndclass.lpfnWndProc= ver_update_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_HAND);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = "ver_update";




    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }


    return SUCCESS;


}

int  get_data_from_file(char *buf, const char *file, int len)
{
    FILE *fp = fopen(file, "rb");
    int ret;
    if (NULL==fp)  return -1;
    
    ret = fread(buf, 1, len, fp);
    fclose(fp);
    
    return ret;
}

int  save_data_from_file(char *buf, const char *file, int len)
{
    FILE *fp = fopen(file, "wb");
    int ret;
    if (NULL==fp)  return -1;
    
    ret = fwrite(buf, 1, len, fp);
    fclose(fp);
    
    return ret;
}

#define   TMP_VER_FILE    ".\\test.txt"
int do_update_ver_file()
{
    char buf[1024];
    char *p_hdr, *p_end;
    int len = get_data_from_file(buf, TMP_VER_FILE, ARRAY_SIZE(buf));

    if (len<=0) return -1;

    p_hdr = strstr(buf, "<xb_ver_info>");
    p_end = strstr(buf, "</xb_ver_info>");
    
    if (p_hdr==NULL || p_end==NULL) return -1;
    p_hdr+=strlen("<xb_ver_info>");
    if (p_hdr >= p_end) return -1;

    if (save_data_from_file(p_hdr, VER_UPDATE_FILE, p_end-p_hdr)<=0)
        return -1;

    return 0;
}


int update_ver_file()
{
    int ret;
    char url[256];
    char file_data[1024];
    GetPrivateProfileString("url_list", "url_1", "http://sunmingbao.freevar.com/ver_update.html"
            , url, ARRAY_SIZE(url), VER_UPDATE_FILE);
dbg_print("=%s", url);
    ret = download_file(url, TMP_VER_FILE);
    if (0==ret)
        goto UPDATE_VER_UPDATE_FILE;

    GetPrivateProfileString("url_list", "url_2", "http://sunmingbao.freevar.com/ver_update.html"
            , url, ARRAY_SIZE(url), VER_UPDATE_FILE);
dbg_print("=%s", url);
    ret = download_file(url, TMP_VER_FILE);
    if (0==ret)
        goto UPDATE_VER_UPDATE_FILE;

    GetPrivateProfileString("url_list", "url_3", "http://sunmingbao.freevar.com/ver_update.html"
            , url, ARRAY_SIZE(url), VER_UPDATE_FILE);
dbg_print("=%s", url);
    ret = download_file(url, TMP_VER_FILE);
    if (0==ret)
        goto UPDATE_VER_UPDATE_FILE;

    return -1;
    
UPDATE_VER_UPDATE_FILE:
    dbg_print("==");
    do_update_ver_file();
    delete_file_f(TMP_VER_FILE);
    return 0;
}



static int ver_update_running;
DWORD WINAPI  ver_update(LPVOID lpParameter)
{
    MSG msg;
    char  notice[16];
    HWND hWnd;
    int ret;

    HACCEL hAccelTable;
    RECT rt;
    HINSTANCE hInst;
dbg_print("==");
    if (ver_update_running)  return;
    
    ver_update_running = 1;
    Sleep(5000);
dbg_print("==");
    if (strcmp(new_version_notice, "no")==0) goto exit;
dbg_print("==");
    fix_width = cxChar_2*COL_NUM;
    fix_height = cyChar_2*LINE_NUM+CLOSE_BUTTOM_SIZE;

dbg_print("==");
    if (update_ver_file())  goto exit;
dbg_print("==");
GetPrivateProfileString("version", "latest_version", version
        , new_version, ARRAY_SIZE(new_version), VER_UPDATE_FILE);
dbg_print("==%s", new_version);
    if (new_version[0]<version[0]) goto exit;
    if (new_version[0]>version[0]) goto NOTICE_NEW_VERSION;

    if (new_version[1]<version[1]) goto exit;
    if (new_version[1]>version[1]) goto NOTICE_NEW_VERSION;

    if (new_version[2]<version[2]) goto exit;
    if (new_version[2]>version[2]) goto NOTICE_NEW_VERSION;
dbg_print("==");
    goto exit;
dbg_print("==");
NOTICE_NEW_VERSION:
hWnd = CreateWindow ("ver_update", szAppName,
            WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
            1000, 1000,
            300, 300,
            hwnd_frame, NULL, g_hInstance, NULL) ;
SystemParametersInfo(SPI_GETWORKAREA,0,(PVOID)&rt,0);


MoveWindow(hWnd, rt.right - fix_width,
                            rt.bottom,
                  fix_width, fix_height, TRUE) ;
   ShowWindow(hWnd, SW_SHOWNOACTIVATE);


   UpdateWindow(hWnd);
 


   // Main message loop:


   while (GetMessage(&msg, NULL, 0, 0))


   {
      if (!TranslateAccelerator(msg.hwnd, NULL, &msg))


      {


         TranslateMessage(&msg);


         DispatchMessage(&msg);
      }


   }

exit:
    ver_update_running = 0;
    return 0;
}

