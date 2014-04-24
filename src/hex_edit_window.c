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

extern t_stream gt_edit_stream;

TCHAR szHexEditWinClassName[] = TEXT ("hex_edit") ;
HWND hwnd_hex_edit;
TEXTMETRIC textmetric;
int cxChar, cyChar;
int is_read_only;

#define LINE_DATA_LEN    (16)
#define LINE_NUMBER_CHAR_NUM    (7)
#define LINE_DATA_CHAR_NUM    (LINE_DATA_LEN*3)
#define LINE_SEP_CHAR_NUM    (3)
#define LINE_DATA_READABLE_OFFSET    (LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM+LINE_SEP_CHAR_NUM)
#define LINE_DATA_READABLE_CHAR_NUM    (LINE_DATA_LEN)
#define LINE_CHAR_NUM    (LINE_DATA_READABLE_OFFSET+LINE_DATA_READABLE_CHAR_NUM)

int cur_strm_idx;
int line_width;


char  *test_buf = gt_edit_stream.data;
int cur_data_len;
int line_num;

int cur_hdr_line;

#define  WM_HEX_EDIT_SEL     (WM_USER + 1)

void hex_win_sel(HWND  hwnd, int offset, int len)
{
    SendMessage(hwnd, WM_HEX_EDIT_SEL, offset, len);
}


int sel_offset;
int sel_len;
void format_line(char *buf, int line_idx, void *start_addr, int length)
{
    char str_addr[32];
    char str_data[64];
    char str_readable[32];
    unsigned char *cur_pos = start_addr;
    int i;

    buf[0]=0;
    sprintf(str_addr, "%05x: ", line_idx*16);

        for (i = 0; i < length; i++)
        {
            sprintf(str_data + i*3, "%02hhx ", cur_pos[i]);
            if (cur_pos[i] > 31 &&  cur_pos[i] < 127)
            sprintf(str_readable + i, "%c", (char)(cur_pos[i]));
            else
            sprintf(str_readable + i, "%c", '.');
        }

        for (i = length; i < 16; i++)
        {
            sprintf(str_data + i*3, "%s", "   ");
            sprintf(str_readable + i, "%c", ' ');
        }

        sprintf(buf, "%s%s%s", str_addr, str_data, str_readable);


}

void init_char_size_info()
{
            HDC hdc = CreateIC (TEXT ("DISPLAY"), NULL, NULL, NULL) ;
            SelectObject (hdc, GetStockObject(SYSTEM_FIXED_FONT)) ;
            GetTextMetrics(hdc, &textmetric) ;
            DeleteObject (SelectObject (hdc, GetStockObject(SYSTEM_FONT))) ;
            DeleteDC (hdc) ;

            cxChar = textmetric.tmAveCharWidth ;
            cyChar = textmetric.tmHeight + textmetric.tmExternalLeading ;

}

void get_a_b(int offset,int len, int cur_line, int *a, int *b)
{
    int min_line = offset/16;
    int max_line = (offset+len-1)/16;
    if (min_line==max_line)
    {
        *a = offset%16;
        *b = len;
        return;
    }
    else if (min_line+1==max_line)
    {
        if (cur_line==min_line)
        {
            *a = offset%16;
            *b = 16-*a;
        }
        else
        {
            *a = 0;
            *b = 1+(offset+len-1)%16;
        }
        return;
    }
    else
    {
        if (cur_line==min_line)
        {
            *a = offset%16;
            *b = 16-*a;
        }
        else if (cur_line==max_line)
        {
            *a = 0;
            *b = 1+(offset+len-1)%16;
        }
        else
        {
            *a = 0;
            *b = 16;
        }
        return;
    }

}

static int row=0, col=LINE_NUMBER_CHAR_NUM;
static SCROLLINFO  si ;

void move_caret_right()
{

int cur_edit_line = cur_hdr_line+row;
int line_data_idx = (col-LINE_NUMBER_CHAR_NUM - 48 - 3);
int data_idx = cur_edit_line*16+line_data_idx;
                if (data_idx<cur_data_len-1)
                {
                    if (col<LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM+3+16-1)
                    {
                        col++;
                    }
                    else
                    {
                        col = LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM+3;
                        row++;
                    }
                }

                

                SetCaretPos(col*cxChar, row*cyChar);

                if (row >= si.nPage)
                    SendMessage(hwnd_hex_edit, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);


                InvalidateRect (hwnd_hex_edit, NULL, TRUE) ;

}

void char_input(char wParam)
{
                int line_data_idx;
                int cur_edit_line;
                int data_idx;
                char old_value;

                cur_edit_line = cur_hdr_line+row;
        
                line_data_idx = (col-LINE_NUMBER_CHAR_NUM - 48 - 3);
                data_idx = cur_edit_line*16+line_data_idx;
                old_value = test_buf[data_idx];

                    

                test_buf[data_idx]= wParam;
                move_caret_right();
                //SetCaretPos(cur_caret_x, cur_caret_y);
                if (old_value != test_buf[data_idx])
                stream_edit_data_change(GetParent(hwnd_hex_edit), data_idx);

}

void half_char_input(char wParam)
{
                int line_data_idx;
                int write_high_bit = 1;
                int cur_edit_line;
                int data_idx;
                char old_value;

                cur_edit_line = cur_hdr_line+row;
        
                if (1==(col-LINE_NUMBER_CHAR_NUM)%3) write_high_bit = 0;
                line_data_idx = (col-LINE_NUMBER_CHAR_NUM)/3;
                data_idx = cur_edit_line*16+line_data_idx;
                old_value = test_buf[data_idx];

                if (wParam>='a' && wParam<='f')
                    {
                        wParam= wParam - 'a'+10;

                    }
                else if (wParam>='A' && wParam<='F')
                    {
                        wParam= wParam - 'A'+10;

                    } 
                else if (wParam>='0' && wParam<='9')
                    {
                        wParam= wParam - '0';

                    }
                    

                if (write_high_bit)
                {
              		test_buf[data_idx]&=0xf;
                    test_buf[data_idx]|= wParam<<4;
                    col+=1;
                }
                else
                {
          		test_buf[data_idx]&=0xf0;
                test_buf[data_idx]|= wParam;
                if (data_idx<cur_data_len-1)
                {
                    if (col<LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM-2)
                    {
                        col+=2;
                    }
                }

                

                }
                SetCaretPos(col*cxChar, row*cyChar);
                InvalidateRect (hwnd_hex_edit, NULL, TRUE) ;
                //SetCaretPos(cur_caret_x, cur_caret_y);
                if (old_value != test_buf[data_idx])
                stream_edit_data_change(GetParent(hwnd_hex_edit), data_idx);

}

void input_proc(char wParam)
{
    if (col<(LINE_NUMBER_CHAR_NUM+16*3)) 
        half_char_input(wParam);
    else
        char_input(wParam);
}

LRESULT CALLBACK hex_edit_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    RECT		rect ;
    int cxClient, cyClient, line_num_cur_page, i;
    static 		int		iDeltaPerLine, iAccumDelta ;
    ULONG  				ulScrollLines ;

    char buf[LINE_CHAR_NUM+16];


    POINT point ;
    
    switch (message)
    {
        case WM_CREATE:
            hwnd_hex_edit = hwnd;
            line_width = LINE_CHAR_NUM*cxChar;
            cur_data_len=gt_edit_stream.len;
            line_num = cur_data_len/16 + !!(cur_data_len%16);
            cur_hdr_line=0;
            sel_len=0;

            si.fMask = SIF_POS ;
            si.nPos = 0 ;
	        SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;

            //为了让下面的初始化代码能走到，这里不返回了。
            //return 0 ;

        case 	WM_SETTINGCHANGE:
          		SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0) ;
          		if (ulScrollLines)
                            //这里一次滚动一行，与常见的实现不一样。
               				iDeltaPerLine = WHEEL_DELTA; //WHEEL_DELTA / ulScrollLines ;
          		else
               				iDeltaPerLine = 0 ;

        		return 0 ;

        case WM_SIZE:
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

	si.cbSize 	= sizeof (si) ;
	si.fMask  	= SIF_RANGE | SIF_PAGE ;
	si.nPage  	= cyClient / cyChar ;
	si.nMin   	= 0 ;
	si.nMax   	= line_num - 1 + si.nPage-1;
	SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;

            return 0 ;

        case WM_PAINT :
        {
            int cur_edit_line = cur_hdr_line+row;
            int line_data_idx = (col-LINE_NUMBER_CHAR_NUM)/3;
            hdc = BeginPaint (hwnd, &ps) ;
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT)) ;

            //draw some thing here
            GetClientRect(hwnd, &rect) ;
            line_num_cur_page = rect.bottom/cyChar;
            for (i=0;i<line_num_cur_page;i++)
            {
                int cur_line=cur_hdr_line+i;
                int cur_line_data_len = 16;
                if (cur_line>=line_num ) break;
                if ((cur_line==(line_num - 1)) && (cur_data_len%16))
                {
                    cur_line_data_len = (cur_data_len%16);
                

                }
                format_line(buf, cur_line
                    , test_buf+cur_line*LINE_DATA_LEN
                    , cur_line_data_len);
                SetBkColor (hdc, RGB(0x80,0x80,0x80)) ;
                SetTextColor(hdc, RGB(0x00,0x00,0x00)) ;
    		    TextOutA(hdc, 0, i*cyChar, buf, LINE_NUMBER_CHAR_NUM) ; 

                    SetBkColor (hdc, RGB(0xFF,0xFF,0xFF)) ;
        		    TextOutA(hdc, cxChar*LINE_NUMBER_CHAR_NUM, i*cyChar, buf+LINE_NUMBER_CHAR_NUM, LINE_DATA_CHAR_NUM) ; 

        		    TextOutA(hdc, cxChar*(LINE_DATA_READABLE_OFFSET-3), i*cyChar, " | ", 3) ; 

                    SetBkColor (hdc, RGB(0xFF,0xFF,0x00)) ;
        		    TextOutA(hdc, cxChar*(LINE_DATA_READABLE_OFFSET), i*cyChar
                    , buf+LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM, LINE_DATA_READABLE_CHAR_NUM) ; 

                if (!sel_len 
                    || cur_line<sel_offset/16
                    || cur_line>(sel_offset+sel_len-1)/16)
                {
                    ;
                }
                else
                {
                    int a=4, b=5;
                    get_a_b(sel_offset, sel_len, cur_line, &a, &b);

                    SetBkColor (hdc, RGB(0xFF,0x55,0xFF)) ;
        		    TextOutA(hdc, cxChar*(LINE_NUMBER_CHAR_NUM+a*3), i*cyChar, buf+LINE_NUMBER_CHAR_NUM+a*3, b*3) ; 

                    SetBkColor (hdc, RGB(0xFF,0xFF,0xFF)) ;
        		    TextOutA(hdc, cxChar*(LINE_DATA_READABLE_OFFSET-3), i*cyChar, " | ", 3) ; 

                    SetBkColor (hdc, RGB(0xFF,0x55,0xFF)) ;
        		    TextOutA(hdc, cxChar*(LINE_DATA_READABLE_OFFSET+a), i*cyChar
                    , buf+LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM+a, b) ; 

                }

                if (cur_edit_line==cur_line && GetFocus()==hwnd)
                {
                    SetBkColor(hdc, RGB(0x00,0x00,0x00));
                    SetTextColor(hdc, RGB(0xff,0xff,0xff)) ;

        		    TextOutA(hdc, cxChar*(LINE_DATA_READABLE_OFFSET+line_data_idx), i*cyChar
                    , buf+LINE_NUMBER_CHAR_NUM+LINE_DATA_CHAR_NUM+line_data_idx, 1) ; 

                }

            }
            DeleteObject(SelectObject (hdc, GetStockObject(SYSTEM_FONT))) ;
  			EndPaint (hwnd, &ps) ;

			return 0 ;
        }
            
case WM_VSCROLL:
		// Get all the vertical scroll bar information
	si.cbSize 	= sizeof (si) ;
	si.fMask  	= SIF_ALL ;
	GetScrollInfo (hwnd, SB_VERT, &si) ;
		// Save the position for comparison later on
	cur_hdr_line = si.nPos ;
	switch (LOWORD (wParam))
	{
	case	SB_TOP:
		si.nPos 	= si.nMin ;
		break ;
               
	case 	SB_BOTTOM:
       		si.nPos 	= si.nMax ;
 	 	break ;
               
	case SB_LINEUP:
		si.nPos -= 1 ;
		break ;
               
	case 	SB_LINEDOWN:
		si.nPos += 1 ;
		break ;
               
	case 	SB_PAGEUP:
		si.nPos -= si.nPage ;
		break ;

	case 	SB_PAGEDOWN:
		si.nPos += si.nPage ;
		break ;
               
	case 	SB_THUMBTRACK:
		si.nPos = si.nTrackPos ;
		break ;
               
	default:
	break ;         
	}
		// Set the position and then retrieve it.  Due to adjustments
		//  by Windows it may not be the same as the value set.

	si.fMask = SIF_POS ;
	SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;
	GetScrollInfo (hwnd, SB_VERT, &si) ;

		// If the position has changed, scroll the window and update it
	if (si.nPos != cur_hdr_line)
   	{
		InvalidateRect (hwnd, NULL, TRUE) ;
        //SetFocus(GetParent(hwnd));
        row-=(si.nPos-cur_hdr_line);
        SetCaretPos(col*cxChar, row*cyChar);
        //ShowCaret(hwnd);
        cur_hdr_line = si.nPos;
	}
	return 0 ;

        case 	WM_MOUSEWHEEL:
      		if (iDeltaPerLine == 0)
           				break ;

      		iAccumDelta += (short) HIWORD (wParam) ;     // 120 or -120

      		while (iAccumDelta >= iDeltaPerLine)
      		{               
           				SendMessage (hwnd, WM_VSCROLL, SB_LINEUP, 0) ;
           				iAccumDelta -= iDeltaPerLine ;
      		}

      		while (iAccumDelta <= -iDeltaPerLine)
      		{
           				SendMessage (hwnd, WM_VSCROLL, SB_LINEDOWN, 0) ;
           				iAccumDelta += iDeltaPerLine ;
      		}

      		return 0 ;



case WM_GETDLGCODE:
   if(lParam)
   {
        LPMSG lpmsg = (LPMSG)lParam;
        if( lpmsg->message == WM_CHAR)
        {
             return DLGC_WANTCHARS;
        }
        
        if( lpmsg->message == WM_KEYDOWN)
        {
             return DLGC_WANTARROWS;
        }

   }
   return 0;

case WM_KEYDOWN:
{

            switch (wParam)
            {
                case VK_LEFT:
                {
                    //move_caret_left();
                    return 0;
                }
                case VK_RIGHT:
                {
                    move_caret_right();
                    return 0;
                }
                case VK_UP:
                {
                    return 0;
                }
                case VK_DOWN:
                {
                    return 0;
                }

            }

            
            break;
}

     	case 	 WM_CHAR:
        {

                if (is_read_only) return 0;
                input_proc(wParam);

          		return 0 ;
          }

     	case 	 WM_SETFOCUS:
        {
            CreateCaret(hwnd, NULL, cxChar, cyChar);
            SetCaretPos(col*cxChar, row*cyChar);
            ShowCaret(hwnd);
            InvalidateRect (hwnd, NULL, TRUE) ;
          	return 0 ;
          }

     	case 	 WM_KILLFOCUS:
        {
          SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(GetWindowLong(hwnd, GWL_ID), EN_KILLFOCUS),0);
          HideCaret(hwnd);
          DestroyCaret();
          		return 0 ;
          }
case WM_LBUTTONDOWN:
    {
        POINT pt_tmp;
        int line_data_len=16;
        pt_tmp.x = (short)LOWORD(lParam); 
        pt_tmp.y = (short)HIWORD(lParam);
        col = pt_tmp.x/cxChar;
        row = pt_tmp.y/cyChar;
        if (row+cur_hdr_line>=line_num) row=line_num-cur_hdr_line-1;
        if(row+cur_hdr_line==line_num-1)
        {
            line_data_len = (cur_data_len%16)?(cur_data_len%16):16;
        }

        if (col<(LINE_NUMBER_CHAR_NUM+16*3 + 2)) 
        {
            if (col<LINE_NUMBER_CHAR_NUM) 
                col=LINE_NUMBER_CHAR_NUM;
            else if (col>=(LINE_NUMBER_CHAR_NUM+line_data_len*3))
                col=(LINE_NUMBER_CHAR_NUM+line_data_len*3)-1;

            if (2==(col-LINE_NUMBER_CHAR_NUM)%3) 
                col--;

        }
        else if (col < (LINE_NUMBER_CHAR_NUM+16*3 + 3))
        {
            col = (LINE_NUMBER_CHAR_NUM+16*3 + 3);

        }
        else if (col >= (LINE_NUMBER_CHAR_NUM+16*3 + 3+line_data_len))
        {
            col = LINE_NUMBER_CHAR_NUM+16*3 + 3+line_data_len-1;

        }

        
        if (GetFocus()!=hwnd)
        {
            SetFocus(hwnd);
        }
        else
        {

          SetCaretPos(col*cxChar, row*cyChar);
          InvalidateRect (hwnd, NULL, TRUE) ;
        }
        
        return 0;


    }
        case WM_HEX_EDIT_SEL:
        {
            int sel_row;
            sel_offset = wParam;
            sel_len = lParam;
            sel_row=sel_offset/16;

            if (sel_row<cur_hdr_line || sel_row>=cur_hdr_line+si.nPage)
            {
            	si.cbSize 	= sizeof (si) ;
            	si.fMask  	= SIF_ALL ;
            	GetScrollInfo (hwnd, SB_VERT, &si);
            	si.fMask = SIF_POS ;
            	si.nPos = sel_row;
            	SetScrollInfo (hwnd, SB_VERT, &si, TRUE) ;
                cur_hdr_line = si.nPos;
            }
            InvalidateRect (hwnd, NULL, TRUE) ;
            row=sel_row;
            col=LINE_NUMBER_CHAR_NUM+3*(sel_offset%16);
            return 0;

        }


    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_hex_edit_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS;
    sub_wndclass.lpfnWndProc= hex_edit_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szHexEditWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}








