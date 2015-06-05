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
#include <tchar.h>     
#include <stdio.h>
#include "gui.h"
#include "debug.h"

int scrn_width;
int scrn_height;

int cxChar, cyChar;
int cxChar_2, cyChar_2;
int cxChar_20, cyChar_20;
t_gui_size_scale_ref gt_gui_size_scale_ref = 
{
    .scrn_width  = 1440,
    .scrn_height = 900,
    .cxChar      = 10,
    .cyChar      = 20,
};
void get_sys_gui_info()
{
    scrn_width = GetSystemMetrics(SM_CXSCREEN);
    scrn_height = GetSystemMetrics(SM_CYSCREEN);

    dbg_print("screen size: %d %d", scrn_width, scrn_height);
}

HFONT  char_font, char_font_2, char_font_25, fixedsys_font;
HFONT  create_font(int height, TCHAR *font_name, int use_black)
{
    HFONT  h_font;
    LOGFONT lf ;
    
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = 1;

    /* font_name如果是Fixedsys，height不起作用 */
    lf.lfHeight = height;
    
    lf.lfPitchAndFamily = FIXED_PITCH;

    if (use_black)
        lf.lfWeight  = FW_BLACK;
dbg_print("font height: %d", height);
    if (NULL != font_name)
        strcpy(lf.lfFaceName, font_name);
    
    h_font = CreateFontIndirect(&lf);

    return h_font;
}

void get_font_size(HFONT  the_font, int *width, int *height)
{
    HDC hdc;
    TEXTMETRIC textmetric;

    hdc = CreateIC (TEXT ("DISPLAY"), NULL, NULL, NULL) ;
    SelectObject(hdc, the_font) ;
    GetTextMetrics(hdc, &textmetric) ;
    SelectObject (hdc, GetStockObject(SYSTEM_FONT)) ;
    DeleteDC (hdc) ;
    
    *width = textmetric.tmAveCharWidth ;
    *height = textmetric.tmHeight + textmetric.tmExternalLeading ;

}

void init_gui_info()
{
    get_sys_gui_info();
    if (scrn_width==1440 && scrn_height==900)
        char_font = create_font(gt_gui_size_scale_ref.cyChar*WIDTH_COEFFICIENT, "Fixedsys", 0);
    else
        char_font = create_font(gt_gui_size_scale_ref.cyChar*WIDTH_COEFFICIENT, "Courier New", 1);
    char_font_2 = create_font(gt_gui_size_scale_ref.cyChar*7/6*WIDTH_COEFFICIENT, TEXT("宋体"), 1);
    char_font_25 = create_font(25, TEXT("Courier New"), 0);
   fixedsys_font = create_font(0, "Fixedsys", 0);
    get_font_size(char_font, &cxChar, &cyChar);
    get_font_size(char_font_2, &cxChar_2, &cyChar_2);
    dbg_print("Char size: %d %d", cxChar, cyChar);
    dbg_print("Char size 2: %d %d", cxChar_2, cyChar_2);
}

