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

#ifdef _DEBUG
void __dbg_print(TCHAR * szFormat, ...)
{
    TCHAR   szBuffer[1024] ;
    va_list pArgList ;
    int len;
    static FILE *fp = NULL;

    if (NULL==fp) fp=fopen("dbg_info.txt", "wb");

    va_start(pArgList, szFormat) ;
    len=_vsntprintf(szBuffer, sizeof (szBuffer) / sizeof (TCHAR), 
			szFormat, pArgList) ;

    va_end (pArgList) ;
    
    szBuffer[len] = '\r';
    szBuffer[len+1] = '\n';
    len+=2;
    
    fwrite(szBuffer, 1, len, fp);
    fflush(fp);
}

void __dbg_print_w(WCHAR * szFormat, ...)
{
    WCHAR   szBuffer[1024] ;
    va_list pArgList ;
    int len;
    static FILE *fp = NULL;

    if (NULL==fp) fp=fopen("dbg_info_w.txt", "wb");

    va_start(pArgList, szFormat) ;
    len=_snwprintf(szBuffer, sizeof (szBuffer) / sizeof (WCHAR), 
			szFormat, pArgList) ;

    va_end(pArgList) ;
    
    szBuffer[len] = L'\r';
    szBuffer[len+1] = L'\n';
    len+=2;
    
    fwrite(szBuffer, 1, len*2, fp);
    fflush(fp);
}


#else
void __dbg_print(TCHAR * szFormat, ...)
{
    ;
}
#endif

