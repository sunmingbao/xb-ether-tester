/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef __GUI_H_
#define __GUI_H_

extern int scrn_width;
extern int scrn_height;

extern int cxChar, cyChar;
extern int cxChar_2, cyChar_2;
extern int cxChar_20, cyChar_20;
void init_gui_info();

typedef struct
{
    int scrn_width;
    int scrn_height;
    int cxChar;
    int cyChar;

} t_gui_size_scale_ref;

#define    WIDTH_COEFFICIENT   scrn_width/gt_gui_size_scale_ref.scrn_width
#define    HEIGHT_COEFFICIENT  scrn_height/gt_gui_size_scale_ref.scrn_height

extern t_gui_size_scale_ref gt_gui_size_scale_ref;
extern HFONT  char_font, char_font_2, char_font_25, fixedsys_font;
#endif

