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
#include <tchar.h>     
#include <stdio.h>
#include "debug.h"

int WinPrintf(HWND hwnd, TCHAR * szFormat, ...)
{
	TCHAR   szBuffer [1024] ;
	va_list pArgList ;

	// The va_start macro (defined in STDARG.H) is usually equivalent to:
	// pArgList = (char *) &szFormat + sizeof (szFormat) ;

	va_start (pArgList, szFormat) ;

	// The last argument to wvsprintf points to the arguments

	_vsntprintf (	szBuffer, sizeof (szBuffer) / sizeof (TCHAR), 
			szFormat, pArgList) ;

	// The va_end macro just zeroes out pArgList for no good reason
	va_end (pArgList) ;
	return MessageBox (hwnd, szBuffer, TEXT("ERROR"), 0) ;
}

#if 0
HWND DoCreateFlatTabControl(HINSTANCE hInstance, HWND hwndParent, TCHAR *tab_class_name, int nr_tabs, TCHAR *labels[]) 
{ 
    RECT rcClient; 
    TCITEM tie; 
    HWND htab;
    int i;
    
             HIMAGELIST imageList;
            HICON icon;

    // Get the dimensions of the parent window's client area, and 
    // create a tab control child window of that size. Note that g_hInst
    // is the global instance handle.
    GetClientRect(hwndParent, &rcClient); 
    htab = CreateWindow(tab_class_name, NULL, WS_CHILD|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        hwndParent, NULL, hInstance, NULL); 
    if (htab == NULL)
    { 
        return NULL; 
    }


    for (i=0; i<nr_tabs;i++)
    {
        // Add tabs for each day of the week. 
        tie.mask = TCIF_TEXT | TCIF_IMAGE; 
        tie.iImage = i; 
        tie.pszText = labels[i]; 
     
        // Load the day string from the string resources. Note that
        // g_hInst is the global instance handle.
        if (TabCtrl_InsertItem(htab, i, &tie) == -1) 
        { 
            DestroyWindow(htab); 
            return NULL; 
        } 
    }
    return htab; 
}
#endif

HWND DoCreateTabControl(HINSTANCE hInstance, HWND hwndParent, DWORD dwStyle, int nr_tabs, TCHAR *icons[], TCHAR *labels[]) 
{ 
    RECT rcClient; 
    TCITEM tie; 
    HWND htab;
    int i;
    
             HIMAGELIST imageList;
            HICON icon;

    // Get the dimensions of the parent window's client area, and 
    // create a tab control child window of that size. Note that g_hInst
    // is the global instance handle.
    GetClientRect(hwndParent, &rcClient); 
    htab = CreateWindow(WC_TABCONTROL, NULL, dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
        hwndParent, NULL, hInstance, NULL); 
    if (htab == NULL)
    { 
        return NULL; 
    }

    imageList = ImageList_Create(32, 32,
                                          ILC_COLORDDB | ILC_MASK,
                                          nr_tabs,
                                          0);

    for (i=0; i<nr_tabs;i++)
    {
        icon = (HICON)LoadImage(hInstance,
                                  icons[i],
                                  IMAGE_ICON, 32, 32,
                                  LR_DEFAULTCOLOR);
        ImageList_AddIcon(imageList, icon);
    }

    SendMessage(htab, TCM_SETIMAGELIST,
                  (WPARAM)0, (LPARAM)imageList);
    for (i=0; i<nr_tabs;i++)
    {
        // Add tabs for each day of the week. 
        tie.mask = TCIF_TEXT | TCIF_IMAGE; 
        tie.iImage = i; 
        tie.pszText = labels[i]; 
     
        // Load the day string from the string resources. Note that
        // g_hInst is the global instance handle.
        if (TabCtrl_InsertItem(htab, i, &tie) == -1) 
        { 
            DestroyWindow(htab); 
            return NULL; 
        } 
    }
    return htab; 
} 

HWND create_tab_win(HINSTANCE hInstance, HWND hwin_parent, TCHAR *WinClassName, int style)
{
            HWND hTabWindow = CreateWindow (WinClassName, TEXT (""),
                style,
                10, 10,
                50, 50,
                hwin_parent, NULL, hInstance, NULL) ;

            UpdateWindow (hTabWindow) ;
            return hTabWindow;
}

HTREEITEM insertItem(HWND  hwnd_treeview, const TCHAR* str, HTREEITEM parent, HTREEITEM insertAfter,
                     int imageIndex, int selectedImageIndex, LPARAM lParam)
{
    // build TVINSERTSTRUCT
    TVINSERTSTRUCT insertStruct;
    insertStruct.hParent = parent;
    insertStruct.hInsertAfter = insertAfter;
    insertStruct.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE|TVIF_PARAM;
    insertStruct.item.pszText = (void *)str;
    insertStruct.item.cchTextMax = sizeof(str)/sizeof(str[0]);
    insertStruct.item.iImage = imageIndex;
    insertStruct.item.iSelectedImage = selectedImageIndex;
    insertStruct.item.lParam = lParam;

    // insert the item
    return (HTREEITEM)SendMessage(hwnd_treeview, TVM_INSERTITEM,
                                    0, (LPARAM)&insertStruct);
}

int get_open_file_name(char *file_name, HWND hwnd, char *filter_str)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[MAX_FILE_PATH_LEN]=TEXT("");       // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.nMaxFile = MAX_FILE_PATH_LEN;
    ofn.lpstrFilter = filter_str;
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Display the Open dialog box. 

    if (GetOpenFileName(&ofn)) 
    {
        strcpy(file_name, ofn.lpstrFile);
        return 0;
    }
        return 1;

}


int get_save_file_name(char *file_name, HWND hwnd, char *filter, char *ext)
{
    TCHAR szFile[MAX_FILE_PATH_LEN]=TEXT("");
    OPENFILENAME  Ofn;
    
    memset(&Ofn, 0, sizeof(OPENFILENAME));
    Ofn.lStructSize = sizeof(OPENFILENAME); 
    Ofn.hwndOwner = hwnd; 
    
    Ofn.lpstrFilter = filter;
    Ofn.nFilterIndex = 2;
    Ofn.lpstrDefExt = ext;

    Ofn.lpstrFile= szFile; 
    Ofn.nMaxFile = MAX_FILE_PATH_LEN; 
    Ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT  | OFN_NOCHANGEDIR; 
    Ofn.lpstrTitle = "save to"; 
 

    if (GetSaveFileName(&Ofn)) 
    {
        strcpy(file_name, Ofn.lpstrFile);
        return 0;
    }

    return 1;


}

int GetIndex(HWND hList)
{
  int i, n;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    if (ListView_GetItemState(hList, i, LVIS_FOCUSED) == LVIS_FOCUSED)
      return i;

    return -1;
} 

int DelSel(HWND hList)
{
  int i, n;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (ListView_GetCheckState(hList, i))
        {
          ListView_DeleteItem(hList, i);
          i--;
          n--;
        }
    }

  return 0;

} 

int SelAll(HWND hList)
{
  int i, n;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (!ListView_GetCheckState(hList, i))
        {
          ListView_SetCheckState(hList, i, 1);
        }
    }

  return 0;

} 

int SelRvs(HWND hList)
{
  int i, n;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (!ListView_GetCheckState(hList, i))
        {
          ListView_SetCheckState(hList, i, 1);
        }
        else
        {
          ListView_SetCheckState(hList, i, 0);
        }
    }

  return 0;

} 

int GetSelCnt(HWND hList)
{
  int i, n, cnt=0;
  n = ListView_GetItemCount(hList);
  for (i = 0; i < n; i++)
    {
        if (ListView_GetCheckState(hList, i))
        {
          cnt++;
        }
    }

  return cnt;

} 

void delete_file_f(char *file_path)
{
    SetFileAttributes(file_path,FILE_ATTRIBUTE_NORMAL);
    DeleteFile(file_path);
}

int file_exists(char *file_path)
{
    FILE *file=fopen(file_path, "r");
    if (NULL!=file)
    {
        fclose(file);
        return 1;
    }

    return 0;

}

int save_file_path_valid(char *file_path)
{
    FILE *file;
    if (file_exists(file_path))
    {
        return 1;
    }

    file=fopen(file_path, "w");
    if (NULL!=file)
    {
        fclose(file);
        delete_file_f(file_path);
        return 1;
    }

    return 0;
}


void set_int_text(HWND hwnd, int value)
{
    char info[32];
    sprintf(info, "%d", value);
    SetWindowText(hwnd, info);
}

int get_int_text(HWND hwnd)
{
    char info[32];
    GetWindowText(hwnd, info, sizeof(info));
    return strtol(info+0,NULL,10);
}

void inc_int_text(HWND hwnd, int value)
{
    set_int_text(hwnd, get_int_text(hwnd) + value);
}

void rect_client2scrn(HWND h, RECT *rect_s, RECT *rect_d)
{
    POINT point;

    point.x=rect_s->left;
    point.y=rect_s->top;
    ClientToScreen(h, &point);
    rect_d->left=point.x;
    rect_d->top=point.y;

    point.x=rect_s->right;
    point.y=rect_s->bottom;
    ClientToScreen(h, &point);
    rect_d->right=point.x;
    rect_d->bottom=point.y;

}

void get_client_rect_scrn(HWND h, RECT *rect)
{
    RECT rect1;
    GetClientRect(h, &rect1);
    rect_client2scrn(h, &rect1, rect);
}

void change_child_width(HWND child, int width)
{
    HWND p=GetParent(child);
    RECT rect0, rect1;
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(child, &rect1);
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(child, 	rect1.left, rect1.top,
                width, rect1.bottom-rect1.top, TRUE) ;
}

void change_child_height(HWND child, int height)
{
    HWND p=GetParent(child);
    RECT rect0, rect1;
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(child, &rect1);
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(child, 	rect1.left, rect1.top,
                rect1.right-rect1.left, height, TRUE) ;
}

void change_child_size(HWND child, int width, int height)
{
    change_child_width(child, width);
    change_child_height(child,height);
}

void move_child_pos(HWND child, int x, int y)
{
    HWND p=GetParent(child);
    RECT rect0, rect1;
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(child, &rect1);
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(child, 	x, y,
                rect1.right-rect1.left, rect1.bottom-rect1.top, TRUE) ;
}

void move_child_a2b_left(HWND a, HWND b, int offset)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.left = rect2.left-offset-width;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void move_child_a2b_right(HWND a, HWND b, int offset)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.left = rect2.right+offset;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void move_child_a2b_up(HWND a, HWND b, int offset)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.top = rect2.top-offset-height;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void move_child_a2b_bottom(HWND a, HWND b, int offset)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.top = rect2.bottom+offset;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void child_a2b_xalign(HWND a, HWND b)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.left = rect2.left;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void child_a2b_yalign(HWND a, HWND b)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.top = rect2.top;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void child_a2b_balign(HWND a, HWND b)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.top += rect2.bottom-rect1.bottom;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void child_a2b_ralign(HWND a, HWND b)
{
    HWND p=GetParent(a);
    RECT rect0, rect1, rect2;
    int width = win_width(a);
    int height = win_height(a);
    get_client_rect_scrn(p, &rect0);
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    rect1.left = rect2.right - width;
    get_relative_pos(&rect0, &rect1, &rect1);
    MoveWindow	(a, 	rect1.left, rect1.top,
                width, height, TRUE) ;
}

void move_child_a2b_bottom_left(HWND a, HWND b, int offset)
{
    move_child_a2b_bottom(a, b, offset);
    child_a2b_xalign(a, b);
}

void move_child_a2b_bottom_right(HWND a, HWND b, int offset)
{
    move_child_a2b_bottom(a, b, offset);
    child_a2b_ralign(a, b);
}

void move_child_a2b_right_top(HWND a, HWND b, int offset)
{
    move_child_a2b_right(a, b, offset);
    child_a2b_yalign(a, b);
}

void move_child_a2b_right_bottom(HWND a, HWND b, int offset)
{
    move_child_a2b_right(a, b, offset);
    child_a2b_balign(a, b);
}

void move_child_a2b_left_top(HWND a, HWND b, int offset)
{
    move_child_a2b_left(a, b, offset);
    child_a2b_yalign(a, b);
}

void move_child_a2b_up_right(HWND a, HWND b, int offset)
{
    move_child_a2b_up(a, b, offset);
    child_a2b_ralign(a, b);
}

void right_include_child(HWND child, int offset)
{
    HWND p=GetParent(child);
    HWND pp=GetParent(p);
    RECT rect0, rect1, rect2;
    GetWindowRect(child, &rect0);
    GetWindowRect(p, &rect1);
    get_client_rect_scrn(pp, &rect2);
    rect1.right = rect0.right + offset;
    get_relative_pos(&rect2, &rect1, &rect1);
    MoveWindow	(p, 	rect1.left, rect1.top,
                rect1.right-rect1.left, rect1.bottom-rect1.top, TRUE) ;
}

void bottom_include_child(HWND child, int offset)
{
    HWND p=GetParent(child);
    HWND pp=GetParent(p);
    RECT rect0, rect1, rect2;
    GetWindowRect(child, &rect0);
    GetWindowRect(p, &rect1);
    get_client_rect_scrn(pp, &rect2);
    rect1.bottom = rect0.bottom + offset;
    get_relative_pos(&rect2, &rect1, &rect1);
    MoveWindow	(p, 	rect1.left, rect1.top,
                rect1.right-rect1.left, rect1.bottom-rect1.top, TRUE) ;
}

void h_arrange_win(HWND hleader, int nr_member, HWND hmembers[], int offset)
{
    int i;
    HWND a, b=hleader;
    for (i=0; i<nr_member; i++)
    {
        a=hmembers[i];
        move_child_a2b_right_top(a,b,10);
        b=a;
    }
}

void h_arrange_win_id(HWND hleader, int nr_member, int hmember_ids[], int offset)
{
    int i;
    HWND p=GetParent(hleader);
    HWND hmembers[nr_member];
    for (i=0;i<nr_member; i++)
    {
        hmembers[i]=GetDlgItem(p, hmember_ids[i]);
    }
    h_arrange_win(hleader, nr_member, hmembers, offset);
}

void v_arrange_win(HWND hleader, int nr_member, HWND hmembers[], int offset)
{
    int i;
    HWND a, b=hleader;
    for (i=0; i<nr_member; i++)
    {
        a=hmembers[i];
        move_child_a2b_bottom_left(a,b,10);
        b=a;
    }
}

void v_arrange_win_id(HWND hleader, int nr_member, int hmember_ids[], int offset)
{
    int i;
    HWND p=GetParent(hleader);
    HWND hmembers[nr_member];
    for (i=0;i<nr_member; i++)
    {
        hmembers[i]=GetDlgItem(p, hmember_ids[i]);
    }
    v_arrange_win(hleader, nr_member, hmembers, offset);
}

int a_include_b_right(HWND a, HWND b)
{
    RECT rect1, rect2;
    GetWindowRect(a, &rect1);
    GetWindowRect(b, &rect2);
    return rect1.right > rect2.right;
}

#if 0
void center_child_win(HWND hparent, HWND hchild)
{

    RECT		rect_p;
    RECT		rect_c;
    GetWindowRect (hparent, &rect_p);
    get_relative_pos_self(&rect_p, &rect_p);
    GetWindowRect (hchild, &rect_c);
    get_relative_pos_self(&rect_c, &rect_c);
    MoveWindow(	hchild
        ,(win_width(hparent)-win_width(hchild))/2
        ,(win_height(hparent)-win_height(hchild))/2
        ,win_width(hchild)
        ,win_height(hchild)
        ,TRUE) ;
}

#else
void center_child_win(HWND hwndParent, HWND hwndWindow)
{
     RECT rectWindow, rectParent;

    if (NULL==hwndParent)
        hwndParent = GetParent(hwndWindow);
        
     // make the window relative to its parent
     if (hwndParent != NULL)
     {
         GetWindowRect(hwndWindow, &rectWindow);
         GetWindowRect(hwndParent, &rectParent);
 
         int nWidth = rectWindow.right - rectWindow.left;
         int nHeight = rectWindow.bottom - rectWindow.top;
 
         int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
         int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;
 
         int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
         int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
 
         // make sure that the dialog box never moves outside of the screen
         if (nX < 0) nX = 0;
         if (nY < 0) nY = 0;
         if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
         if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;
 
         MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);
 
     }
}

#endif

void clear_comb(HWND hwndCtl)
{
    while(ComboBox_GetCount(hwndCtl)>0)
    {
        ComboBox_DeleteString(hwndCtl, 0);
    }
}

int str2int(char *info)
{
    if (strchr(info, 'x') || strchr(info, 'X'))
        return strtol(info+0,NULL,16);
    else        
        return strtol(info+0,NULL,10);
}

int read_file_to_buf(char *buf, int buflen, const char *file)
{
    int ret;
    FILE *fp=fopen(file, "rb");
    if (fp == NULL )
    {
        return -1;
    }

    ret=fread(buf, 1, buflen, fp);
    fclose(fp);
    if (ret>0)
    {
        buf[ret] = 0;
        return ret;
    }
    
    return -1;
}