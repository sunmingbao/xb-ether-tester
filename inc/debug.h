/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef __DEBUG_H_
#define __DEBUG_H_

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)


void __dbg_print_w(WCHAR * szFormat, ...);
void __dbg_print(TCHAR * szFormat, ...);

#define    dbg_print(fmt, args...) \
    do \
    { \
        __dbg_print("DBG:%s(%d)-%s:\n"fmt"\n", __FILE__,__LINE__,__FUNCTION__,##args); \
    } while (0)

#define    dbg_print_w(fmt, args...) \
    do \
    { \
        __dbg_print_w(L"DBG:%s(%d):\n"fmt"\n", __WFILE__,__LINE__,,##args); \
    } while (0)

#endif

