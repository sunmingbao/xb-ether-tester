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
#include "net.h"

const char version[4]={'2','5','7','0'};

TCHAR szRightWinClassName[] = TEXT ("right_win") ;
HWND    hwnd_right;

TCHAR szStreamWinClassName[] = TEXT ("stream_win") ;
HWND    hwnd_stream;

TCHAR szFcWinClassName[] = TEXT ("fc_win") ;
HWND    hwnd_fc;

HWND    hwnd_lv;
#define ID_LV    (41234)


int lv_in_op;
HWND CreateListView (HWND hwndParent) 
{
    RECT rcClient;                       // The parent window's client area.
    HWND hWndListView;


    GetClientRect (hwndParent, &rcClient); 

    // Create the list-view window in report view with label editing enabled.
    hWndListView = CreateWindow(WC_LISTVIEW, 
                                     TEXT(""),
                                     WS_CHILD | LVS_REPORT |LVS_SINGLESEL,
                                     0, 0,
                                     rcClient.right - rcClient.left,
                                     rcClient.bottom - rcClient.top,
                                     hwndParent,
                                     (HMENU)ID_LV,
                                     g_hInstance,
                                     NULL); 

ListView_SetExtendedListViewStyle(hWndListView, 
    LVS_EX_CHECKBOXES|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

    return (hWndListView);
}

BOOL InitListViewImageLists(HWND hWndListView) 
{ 
    HICON hiconItem;     // Icon for list-view items.
    HIMAGELIST hLarge;   // Image list for icon view.
    HIMAGELIST hSmall;   // Image list for other views.

    // Create the full-sized icon image lists. 
    hLarge = ImageList_Create(25, 
                              25, 
                              ILC_COLORDDB | ILC_MASK, 2, 1); 

    hSmall = ImageList_Create(25, 
                              25, 
                              ILC_COLORDDB | ILC_MASK, 2, 1); 
    
    // Add an icon to each image list.  
    //hiconItem = LoadIcon(g_hInstance, TEXT("haha1"));
    ImageList_AddIcon(hLarge, hiconItem);
    ImageList_AddIcon(hSmall, hiconItem);
    DestroyIcon(hiconItem);
    //hiconItem = LoadIcon(g_hInstance, TEXT("haha2"));
    ImageList_AddIcon(hLarge, hiconItem);
    ImageList_AddIcon(hSmall, hiconItem);

    DestroyIcon(hiconItem);



 
// When you are dealing with multiple icons, you can use the previous four lines of 
// code inside a loop. The following code shows such a loop. The 
// icons are defined in the application's header file as resources, which 
// are numbered consecutively starting with IDS_FIRSTICON. The number of 
// icons is defined in the header file as C_ICONS.
/*    
    for(index = 0; index < C_ICONS; index++)
    {
        hIconItem = LoadIcon (g_hinst, MAKEINTRESOURCE(IDS_FIRSTICON + index));
        ImageList_AddIcon(hSmall, hIconItem);
        ImageList_AddIcon(hLarge, hIconItem);
        Destroy(hIconItem);
    }
*/
    
    // Assign the image lists to the list-view control. 
    ListView_SetImageList(hWndListView, hLarge, LVSIL_NORMAL); 
    ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL); 
    
    return TRUE; 
}

TCHAR *col_names[] = 
{
    NULL,
    NULL,
    TEXT("索引"),
    TEXT("名称"),
    TEXT("源地址"),
    TEXT("目的地址"),
    TEXT("协议"),
    TEXT("长度"),
    TEXT("信息"),
};

BOOL InitListViewColumns(HWND hWndListView) 
{ 
    TCHAR szText[256];     // Temporary buffer.
    LVCOLUMN lvc;
    int iCol, col_num = ARRAY_SIZE(col_names);
int order[] = { 1, 0, 2, 3, 4, 5, 6, 7, 8}; 
int lv_width = GetSystemMetrics(SM_CXSCREEN) - 240;
int col_width[] = {40, 20, cxChar*7, cxChar*10, cxChar*20, cxChar*20
    , cxChar*9, cxChar*7, cxChar*30};

SendMessage(hWndListView, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Add the columns.
    for (iCol = 0; iCol < col_num; iCol++)
    {
        lvc.fmt = LVCFMT_LEFT;
        lvc.iSubItem = iCol;
        lvc.pszText = col_names[iCol];
        if ( iCol == 0 || iCol == 1)
        {
            lvc.fmt = LVCFMT_RIGHT;  // Left-aligned column.
        }

        if ( iCol != (col_num-1))
        {
            lvc.cx = col_width[iCol];
            lv_width -= lvc.cx;
        }
        else
        {
            lvc.cx = lv_width-25;

        }

        // Insert the columns into the list view.
        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
        {
        MessageBox (NULL, TEXT ("ListView_InsertColumn"),
          szAppName, MB_ICONERROR) ;
            return FALSE;
        }
    }

    
ListView_SetColumnOrderArray(hWndListView, ARRAY_SIZE(order), order); 

    return TRUE;
} 

BOOL InsertListViewItems(HWND hWndListView, int cItems)
{
    LVITEM lvI;
    int index;
    int iCol;

    // Initialize LVITEM members that are different for each item.
    for (index = 0; index < cItems; index++)
    {
        lvI.iItem  = index;
        lvI.iSubItem  = 0;
    
        lvI.mask      = LVIF_TEXT|LVIF_IMAGE;

        lvI.pszText   = TEXT("");;
        lvI.iImage = -1; /* 若不要图片，就设置为-1 */

        // Insert items into the list.
        if (ListView_InsertItem(hWndListView, &lvI) == -1)
            return FALSE;

        lvI.iItem  = index;
        lvI.iSubItem  = 1;
    
        lvI.mask      = LVIF_TEXT;
        lvI.pszText   = TEXT("");
        //lvI.pszText   = NULL;
        lvI.iImage = -1; /* 若不要图片，就设置为-1 */
        ListView_SetItem(hWndListView, &lvI);

        lvI.iItem  = index;
        lvI.iSubItem  = 2;
    
        lvI.mask      = LVIF_TEXT|LVIF_IMAGE;
        lvI.pszText   = TEXT(" ");
        //lvI.pszText   = NULL;
        lvI.iImage = 0; /* 若不要图片，就设置为-1 */
        ListView_SetItem(hWndListView, &lvI);

        for (iCol = 3; iCol < ARRAY_SIZE(col_names); iCol++)
        {

            ListView_SetItemText(hWndListView, index, iCol, TEXT("12345"));
        }

    }

    return TRUE;
}

BOOL InsertItemFromStream(HWND hWndListView, t_stream* pt_stream)
{
    LVITEM lvI;
    int index=ListView_GetItemCount(hWndListView);
    int iCol;
    TCHAR    info[128];
    t_ether_packet *pt_eth_hdr = pt_stream->data;

    // Initialize LVITEM members that are different for each item.
    {
        lvI.iItem  = index;
        lvI.iSubItem  = 0;
    
        lvI.mask      = LVIF_TEXT|LVIF_IMAGE;

        lvI.pszText   = TEXT(" ");
        lvI.iImage = -1; /* 若不要图片，就设置为-1 */

        // Insert items into the list.
        if (ListView_InsertItem(hWndListView, &lvI) == -1)
            return FALSE;

        if (pt_stream->selected)
            ListView_SetCheckState(hwnd_lv, index, 1);

        lvI.iItem  = index;
        lvI.iSubItem  = 1;
    
        lvI.mask      = LVIF_TEXT|LVIF_IMAGE;
        lvI.pszText   = TEXT(" ");
        //lvI.pszText   = NULL;
        lvI.iImage = -1; /* 若不要图片，就设置为-1 */
        ListView_SetItem(hWndListView, &lvI);
#if 0
        lvI.iItem  = index;
        lvI.iSubItem  = 2;
    
        lvI.mask      = LVIF_TEXT|LVIF_IMAGE;
        lvI.pszText   = TEXT(" ");
        //lvI.pszText   = NULL;
        lvI.iImage = -1; /* 若不要图片，就设置为-1 */
        ListView_SetItem(hWndListView, &lvI);

#else
        sprintf(info, "%d", index+1);
        ListView_SetItemText(hWndListView, index, 2, info);
#endif

        ListView_SetItemText(hWndListView, index, 3, pt_stream->name);

        get_src_addr(info, pt_eth_hdr);
        ListView_SetItemText(hWndListView, index, 4, info);

        get_dst_addr(info, pt_eth_hdr);
        ListView_SetItemText(hWndListView, index, 5, info);

        get_proto_name(info, pt_eth_hdr);
        ListView_SetItemText(hWndListView, index, 6, info);


        sprintf(info, "%d", pt_stream->len);
        ListView_SetItemText(hWndListView, index, 7, info);

        get_pkt_desc_info(info, pt_stream->data, pt_stream->err_flags);
        ListView_SetItemText(hWndListView, index, 8, info);

    }

    return TRUE;
}

int re_populate_items()
{
    int i, cnt=0;
lv_in_op=1;
    ListView_DeleteAllItems(hwnd_lv);
    for(i=0;i<nr_cur_stream;i++)
    {
        InsertItemFromStream(hwnd_lv, g_apt_streams[i]);
        cnt++;

    }
    update_statusbar();
lv_in_op=0;
    return cnt;

}

#if 0
void update_item(HWND hWndListView, int index)
{
    static int img_idx=0;
    LVITEM lvI;

    img_idx = !img_idx;
    
    lvI.iItem  = index;
    lvI.iSubItem  = 0;

    lvI.mask      = LVIF_IMAGE ;
    if (ListView_GetCheckState( hWndListView, index))
    {
        lvI.iImage = 1;
    }
    else
    {
        lvI.iImage = 0;
    }
    ListView_SetItem(hWndListView, &lvI);
    UpdateWindow (hWndListView) ;

}
#endif
void update_grid_from_edit(int edit_iItem, int edit_iSubItem)
{
    TCHAR buf[64];
    t_stream* pt_stream=g_apt_streams[edit_iItem];
    t_ether_packet *pt_eth_hdr = pt_stream->data;
    t_ip_hdr *iph=eth_data(pt_eth_hdr);
    t_ipv6_hdr *ip6h=(void *)iph;
    int type = eth_type(pt_eth_hdr);

    GetWindowText(hwnd_dynamic_edit, buf, sizeof(buf));
    ShowWindow (hwnd_dynamic_edit, 0);
    ListView_SetItemText(hwnd_lv, edit_iItem, edit_iSubItem, buf);
    if (edit_iSubItem==3)
    {
        strcpy(pt_stream->name, buf);
        return;
    }

    if (edit_iSubItem==7)
    {
        pt_stream->len = atoi(buf);
        goto exit;
    }

    if (type!=ETH_P_IP && type!=ETH_P_IPV6)
    {
        if (edit_iSubItem==4)
        {
            mac_str2n(pt_stream->eth_packet.src, buf);
        }
        else if (edit_iSubItem==5)
        {
            mac_str2n(pt_stream->eth_packet.dst, buf);
        }
        return;
    }

    if (edit_iSubItem==4)
    {
        if (type==ETH_P_IP)
            ip_str2n(&(iph->saddr), buf);
        else
            ip6_str2n(ip6h->saddr, buf);
    }
    else if (edit_iSubItem==5)
    {
        if (type==ETH_P_IP)
            ip_str2n(&(iph->daddr), buf);
        else
            ip6_str2n(ip6h->daddr, buf);

    }

exit:
    update_stream(pt_stream);
}


void cpy_stream(t_stream *dst, t_stream *src)
{
    memcpy(dst, src, STREAM_HDR_LEN);
    memcpy(dst->data, src->data, src->len);
    dst->err_flags = src->err_flags;
}

void delete_stream(int idx)
{
    nr_cur_stream--;

    free(g_apt_streams[idx]);
    while (idx < nr_cur_stream)
    {
        g_apt_streams[idx] = g_apt_streams[idx+1];
        idx++;
    }

    g_apt_streams[idx] = NULL;

    doc_modified=1;
    copy_idx=-1;
}

int delete_sel_stream()
{
  int i;
  for (i = 0; i < nr_cur_stream; i++)
    {
        if (g_apt_streams[i]->selected)
        {
            delete_stream(i);
            i--;
        }
    }

  return 0;

} 

HWND hwnd_dynamic_edit;
#define    MAX_LV_COLOR_NUM    (4)
COLORREF    lv_row_color[MAX_LV_COLOR_NUM];
COLORREF colorShade (COLORREF c, float fPercent)
//    create a lighter shade (by fPercent %) of a given colour
{
    return RGB((BYTE) ((float) GetRValue (c) * fPercent / 100.0),
        (BYTE) ((float) GetGValue (c) * fPercent / 100.0),
        (BYTE) ((float) GetBValue (c) * fPercent / 100.0));
}

int get_row_color_idx(HWND hWnd, int idx)
{
    t_ether_packet *pt_eth_hdr;
    uint32_t err_flags;
    t_dump_pkt *pt_pkt;
    
    if (idx>=ListView_GetItemCount(hWnd)) return 0;
    
    if (hWnd==hwnd_lv)
    {
        pt_eth_hdr = (void *)(g_apt_streams[idx]->data);
        err_flags = g_apt_streams[idx]->err_flags;

    }
    else
    {
        pt_pkt=get_lvi_lparam(hWnd, idx);
        pt_eth_hdr = (void *)(pt_pkt->pkt_data);
        err_flags = pt_pkt->err_flags;
    }

    //if (!is_ip_pkt(pt_eth_hdr))  return 0;
    if (err_flags) return 2;
    if (ip_pkt_is_frag(pt_eth_hdr))
    {
        return 3;
    }

    return  0; //(idx % 2) ? 1:0;
}

COLORREF get_row_color(HWND hWnd, int idx)
{
    return lv_row_color[get_row_color_idx(hWnd, idx)];
}

void lv_row_color_init()
{
    lv_row_color[0]=GetSysColor (COLOR_WINDOW);
    lv_row_color[1]=colorShade(GetSysColor (COLOR_WINDOW), 95.0);
    lv_row_color[2]=RGB(0xA9, 0x13, 0x30);
    lv_row_color[3]=RGB(0xE6, 0x94, 0x1A);
}


WNDPROC old_lv_proc;
void EraseAlternatingRowBkgnds (HWND hWnd, HDC hDC)
//    re-draw row backgrounds with the appropriate background colour
{
    int i;
    RECT    rect;        //    row rectangle
    POINT    pt;
    int     iItems,
            iTop;
    HBRUSH    brushes[MAX_LV_COLOR_NUM];

//    create coloured brushes
    brushes[0]=CreateSolidBrush(lv_row_color[0]);
    brushes[1]=CreateSolidBrush(lv_row_color[1]);
    brushes[2]=CreateSolidBrush(lv_row_color[2]);
    brushes[3]=CreateSolidBrush(lv_row_color[3]);

//    get horizontal dimensions of row
    GetClientRect (hWnd, &rect);
//    number of displayed rows
    iItems = ListView_GetCountPerPage (hWnd);
//    first visible row
    iTop = ListView_GetTopIndex (hWnd);
    ListView_GetItemPosition (hWnd, iTop, &pt);

    for (i=iTop ; i<=iTop+iItems ; i++) {
//        set row vertical dimensions
        rect.top = pt.y;
        ListView_GetItemPosition (hWnd, i+1, &pt);
        rect.bottom = pt.y;
//        fill row with appropriate colour
        FillRect (hDC, &rect, brushes[get_row_color_idx(hWnd, i)]);
    }
    
//    cleanup
    DeleteObject (brushes[0]);
    DeleteObject (brushes[1]);
    DeleteObject (brushes[2]);
    DeleteObject (brushes[3]);
}  
void PaintAlternatingRows (HWND hWnd)
//    re-draw rows with the appropriate background colour
{
    int i;
    RECT    rectUpd,        	//    rectangle to update
            rectDestin,        	//    temporary storage
            rect;            	//    row rectangle
    POINT    pt;
    int     iItems,
            iTop;
    COLORREF    c;            	//    temporary storage

//    get the rectangle to be updated
    GetUpdateRect (hWnd, &rectUpd, FALSE);
//    allow default processing first
    CallWindowProc (
        (FARPROC) old_lv_proc, hWnd, WM_PAINT, 0, 0);
//    set the row horizontal dimensions
    SetRect (&rect, rectUpd.left, 0, rectUpd.right, 0);
//    number of displayed rows
    iItems = ListView_GetCountPerPage (hWnd);
//    first visible row
    iTop = ListView_GetTopIndex (hWnd);

    ListView_GetItemPosition (hWnd, iTop, &pt);
    for (i=iTop ; i<=iTop+iItems ; i++) {
//        set row vertical dimensions
        rect.top = pt.y;
        ListView_GetItemPosition (hWnd, i+1, &pt);
        rect.bottom = pt.y;
//        if row rectangle intersects update rectangle then it requires
//        re-drawing
        if (IntersectRect (&rectDestin, &rectUpd, &rect)) {
//            change text background colour accordingly
            c = get_row_color(hWnd, i);
            ListView_SetTextBkColor (hWnd, c);
//            invalidate the row rectangle then...
            InvalidateRect (hWnd, &rect, FALSE);
//            ...force default processing
            CallWindowProc (
                (FARPROC) old_lv_proc, hWnd, WM_PAINT, 0, 0);
        }
    }
}

LRESULT CALLBACK my_lv_proc (HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    switch (iMessage) {
        case WM_PAINT:
//    intercept the WM_PAINT message which is called each time an area
//    of the control's client area requires re-drawing
            PaintAlternatingRows (hWnd);
            return 0;
        case WM_ERASEBKGND:
//    intercept the WM_ERASEBKGRN message which is called each time an area
//    of the control's client area background requires re-drawing
            EraseAlternatingRowBkgnds (hWnd, (HDC) wParam);
            return 0;
    }

// continue with default message processing
    return CallWindowProc (
        (FARPROC) old_lv_proc, hWnd, iMessage, wParam, lParam);
}

LRESULT CALLBACK stream_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    HWND    hwnd_sub ;
    RECT		rect ;
    int cxClient, cyClient;
    static HMENU	hMenu ;
    POINT point ;
    int ret, idx;
    TCHAR  buf[64];

    static int edit_iItem=-1 ;
    static int edit_iSubItem;

     LVITEM lv_item;
    switch (message)
    {
        case WM_CREATE:
            hwnd_stream= hwnd;
            hMenu = LoadMenu (g_hInstance, TEXT("my_popup_menu")) ;
            hMenu = GetSubMenu (hMenu, 0) ;

            hwnd_dynamic_edit=CreateWindow (TEXT("edit"), TEXT(""),
                WS_CHILD|ES_AUTOHSCROLL,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                hwnd, ID_DYNAMIC_EDIT, g_hInstance, NULL) ;

            SendMessage(hwnd_dynamic_edit, WM_SETFONT, (WPARAM)GetStockObject(SYSTEM_FIXED_FONT), 0); 

            hwnd_lv = CreateListView(hwnd);
            //InitListViewImageLists(hwnd_lv);
            InitListViewColumns(hwnd_lv);
            lv_row_color_init();
            old_lv_proc = (WNDPROC) SetWindowLong (hwnd_lv, 
                                 	GWL_WNDPROC, (LONG)my_lv_proc) ;



            ShowWindow (hwnd_lv, 1) ;
            UpdateWindow (hwnd_lv) ;

           add_tip(hwndTip, hwnd_lv, TEXT("点击鼠标右键进行操作"));

            return 0 ;

        case WM_SIZE:
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

            MoveWindow	(hwnd_lv, 	0, 0,
                cxClient, cyClient, TRUE) ;

            return 0 ;

case WM_NOTIFY:
{

   NMHDR *pt_nmhdr=(void *)lParam;
   switch(LOWORD(wParam))
   { 
     case ID_LV:

     // if code == NM_CLICK - Single click on an item
     if(pt_nmhdr->code == NM_CLICK || pt_nmhdr->code == NM_DBLCLK) 
     {
        int iItem = ((LPNMITEMACTIVATE)lParam)->iItem;
        int iSubItem=((LPNMITEMACTIVATE)lParam)->iSubItem;

        if (iItem>=0 && ((iSubItem>=3 && iSubItem<=5) || iSubItem==7)) 
        {

            ListView_GetSubItemRect(hwnd_lv,iItem,iSubItem,LVIR_LABEL,&rect);
            ListView_GetItemText(hwnd_lv, iItem, iSubItem, buf, sizeof(buf));
            MoveWindow	(hwnd_dynamic_edit
                ,rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, TRUE);
            SetWindowText(hwnd_dynamic_edit, buf);
            ShowWindow (hwnd_dynamic_edit, 1) ;
            SetFocus(hwnd_dynamic_edit);
            SendMessage(hwnd_dynamic_edit, EM_SETSEL, (WPARAM)0, (LPARAM)-1);

            edit_iItem = iItem;
            edit_iSubItem = iSubItem;

        }

            return 0;
     }
    else if (pt_nmhdr->code == NM_RCLICK)
    {
        point = ((LPNMITEMACTIVATE)lParam)->ptAction;
        ListView_GetItem(hwnd_lv, &lv_item);

        ClientToScreen (hwnd_lv, &point) ;
  		TrackPopupMenu (hMenu, TPM_LEFTBUTTON, point.x, point.y, 0, hwnd_stream, NULL) ;
        return 0;

    }
    else if (pt_nmhdr->code == LVN_ITEMCHANGED)
    {
        if (!init_over) break;
        if (lv_in_op)   break;
        //if((((LPNMLISTVIEW)lParam)->uOldState&LVIS_STATEIMAGEMASK) != 
        //    (((LPNMLISTVIEW)lParam)->uNewState&LVIS_STATEIMAGEMASK)) 
        {
            int iItem = ((LPNMITEMACTIVATE)lParam)->iItem;
            int selected = ListView_GetCheckState(hwnd_lv, iItem);

            if (g_apt_streams[iItem]->selected!=selected)
            {
                doc_modified=1;
                g_apt_streams[iItem]->selected=selected;
                update_statusbar();
            }
        }
        return 0;

    }
    
    break;

   }

   break;
}

        case 	WM_COMMAND:
            switch (LOWORD(wParam))
            {
                  
                case    IDM_STREAM_NEW:
                    init_stream(&gt_edit_stream);
                    gt_edit_stream.len=sizeof(sample_pkt);
                    memcpy(gt_edit_stream.data, sample_pkt, sizeof(sample_pkt));

                    ret=DialogBox(g_hInstance, TEXT("STREAM_EDIT_DLG"), hwnd, StreamEditDlgProc);
                    if (IDOK==ret)
                    {
                        add_stream(&gt_edit_stream);
                        re_populate_items();
                    }
               	return 0 ;

                case    IDM_STREAM_DEL:
                    idx=GetIndex(hwnd_lv);
       				//ListView_DeleteItem(hwnd_lv, idx);
                    delete_stream(idx);
                    re_populate_items();
                    update_statusbar();
       				return 0 ;
                case    IDM_STREAM_EDIT:
                    cur_strm_idx=GetIndex(hwnd_lv);
                    if (cur_strm_idx<0) return 0;
                    cpy_stream(&gt_edit_stream, g_apt_streams[cur_strm_idx]);
                    ret=DialogBox(g_hInstance, TEXT("STREAM_EDIT_DLG"), hwnd, StreamEditDlgProc);
                    if (IDOK==ret)
                    {
                        doc_modified=1;
                        cpy_stream(g_apt_streams[cur_strm_idx], &gt_edit_stream);
                        re_populate_items();
                    }
       				//ListView_DeleteAllItems(hwnd_lv);
       				return 0 ;
                case    IDM_STREAM_DEL_SEL:
       				//DelSel(hwnd_lv);
                    delete_sel_stream();
                    re_populate_items();
                    update_statusbar();
       				return 0 ;

                case    IDM_STREAM_SEL_ALL:
       				SelAll(hwnd_lv);
       				return 0 ;

                case    IDM_STREAM_SEL_RVS:
       				SelRvs(hwnd_lv);
       				return 0 ;
                    
                case    IDM_STREAM_COPY:
       				copy_idx = GetIndex(hwnd_lv);
       				return 0 ;

                case    IDM_STREAM_PASTE:
       				cpy_stream(&gt_edit_stream, g_apt_streams[copy_idx]);
                    add_stream(&gt_edit_stream);
                    re_populate_items();
       				return 0 ;

                case    IDM_STREAM_MAKE_FRAGS:
                {
                    if (!ip_pkt_can_frag(&(g_apt_streams[GetIndex(hwnd_lv)]->eth_packet)))
                    {
                        WinPrintf(hwnd, TEXT("暂不支持对该类报文进行分片"));
                        return 0 ;
                    }
                    
       				ret=DialogBox(g_hInstance, TEXT("FRAG_DLG"), hwnd, FragDlgProc);

       				return 0 ;
                }

                case    IDM_STREAM_SEL2PCAP:
                {
                    char file_name[MAX_FILE_PATH_LEN];
                    ret=get_save_file_name(file_name, hwnd, PCAP_FILE_FILTER, PCAP_FILE_SUFFIX);
                    if (ret) return 0 ;

       				stream2dump(file_name);
       				return 0 ;
                }

                case    ID_DYNAMIC_EDIT:
       				if (HIWORD(wParam)==EN_KILLFOCUS)
                    {

            update_grid_from_edit(edit_iItem, edit_iSubItem);
            edit_iItem=-1;

           				return 0 ;
                    }

            }
            break;

case   WM_KEYDOWN:
    if (VK_RETURN==wParam)
    {

        SetFocus(hwnd);

         return 0;

    }
    break;

        case 	WM_INITMENUPOPUP:
        {
            int idx=GetIndex(hwnd_lv);
            int sel_cnt=GetSelCnt(hwnd_lv);
            int item_cnt=ListView_GetItemCount(hwnd_lv);
            if (lParam == 0)
            {
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_NEW, nr_cur_stream<MAX_STREAM_NUM ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_EDIT, idx>=0 ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_DEL, idx>=0 ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_COPY, idx>=0 ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_PASTE, copy_idx>=0 && item_cnt>copy_idx ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_DEL_SEL, sel_cnt ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_SEL_ALL, item_cnt ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_SEL_RVS, item_cnt ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_MAKE_FRAGS, idx>=0 ? MF_ENABLED : MF_GRAYED);
                EnableMenuItem ((HMENU) wParam, IDM_STREAM_SEL2PCAP, sel_cnt ? MF_ENABLED : MF_GRAYED);

                return 0;

            }

            break;
        }

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

LRESULT CALLBACK fc_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    HWND    hwnd_sub ;
    RECT		rect ;
    int cxClient, cyClient;
    static HMENU	hMenu ;
    POINT point ;
    int ret;
    TCHAR  buf[32];

    static int edit_iItem=-1 ;
    static int edit_iSubItem;

     LVITEM lv_item;
    switch (message)
    {
        case WM_CREATE:
            hwnd_right = hwnd;
            return 0 ;

        case WM_SIZE:
            return 0 ;

        case WM_PAINT :
            hdc = BeginPaint (hwnd, &ps) ;
            
            //draw some thing here
            GetClientRect (hwnd, &rect) ;
		    DrawText (hdc, TEXT ("fc win!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;
  			EndPaint (hwnd, &ps) ;
			return 0 ;

    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

LRESULT CALLBACK right_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc ;
    PAINTSTRUCT ps ;
    HWND    hwnd_sub ;
    RECT		rect ;
    int cxClient, cyClient;
    static HMENU	hMenu ;
    POINT point ;
    int ret;
    TCHAR  buf[32];

    static int edit_iItem=-1 ;
    static int edit_iSubItem;

     LVITEM lv_item;
    switch (message)
    {
        case WM_CREATE:
            hwnd_right = hwnd;
            register_stream_win();
            register_fc_win();
hwnd_stream =CreateWindowEx(NULL, szStreamWinClassName, NULL,
                      WS_CHILD,
                      CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                         hwnd, 	//Handle of the Parent window.
				//(i.e. Window for which you want Tooltip)
                         NULL,
                         g_hInstance, 
                         NULL);
            ShowWindow (hwnd_stream, 1) ;
            UpdateWindow (hwnd_stream) ;


            return 0 ;

        case WM_SIZE:
      		cxClient = LOWORD (lParam) ;
      		cyClient = HIWORD (lParam) ;

            MoveWindow	(hwnd_stream, 	0, 0,
                cxClient, cyClient, TRUE) ;
            MoveWindow	(hwnd_fc, 	0, 0,
                cxClient, cyClient, TRUE) ;

            return 0 ;


    }
    
    return DefWindowProc (hwnd, message, wParam, lParam) ;
}

int register_right_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= right_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szRightWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}


int register_stream_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= stream_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szStreamWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

int register_fc_win()
{
    WNDCLASS    sub_wndclass;
    sub_wndclass.style      = CS_HREDRAW | CS_VREDRAW;
    sub_wndclass.lpfnWndProc= fc_WndProc;
    sub_wndclass.cbClsExtra = 0;
    sub_wndclass.cbWndExtra = 0;
    sub_wndclass.hInstance  = g_hInstance;
    sub_wndclass.hIcon      = LoadIcon (g_hInstance, TEXT("my_frame_icon"));
    sub_wndclass.hCursor    = LoadCursor (NULL, IDC_ARROW);
    sub_wndclass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
    sub_wndclass.lpszMenuName   = NULL;
    sub_wndclass.lpszClassName  = szFcWinClassName;


    if (!RegisterClass (&sub_wndclass))
     {
        MessageBox (NULL, TEXT ("Program requires Windows NT!"),
          szAppName, MB_ICONERROR) ;
        return FAIL;
     }

    return SUCCESS;

}

