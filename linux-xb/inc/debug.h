/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef  __DEBUG_H__
#define  __DEBUG_H__

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define    DBG_PRINT(fmt, args...) \
    do \
    { \
        printf("DBG:%s(%d)-%s:\n"fmt"\n", __FILE__,__LINE__,__FUNCTION__,##args); \
    } while (0)

#define    DBG_PRINT_S(fmt, args...) \
    do \
    { \
        printf("DBG:%s(%d)-%s:\n"fmt"\n", strrchr(__FILE__, '/')+1,__LINE__,__FUNCTION__,##args); \
    } while (0)

#define    DBG_PRINT_QUIT(fmt, args...) \
    do \
    { \
        printf("DBG:%s(%d)-%s:\n"fmt"\n", __FILE__,__LINE__,__FUNCTION__,##args); \
        exit(1); \
    } while (0)

#define    ERR_DBG_PRINT(fmt, args...) \
    do \
    { \
        printf("ERR_DBG:%s(%d)-%s:\n"fmt": %s\n", __FILE__,__LINE__,__FUNCTION__,##args, strerror(errno)); \
    } while (0)

#define    ERR_DBG_PRINT_QUIT(fmt, args...) \
    do \
    { \
        printf("ERR_DBG_QUIT:%s(%d)-%s:\n"fmt": %s\n", __FILE__,__LINE__,__FUNCTION__,##args, strerror(errno)); \
        exit(1); \
    } while (0)

void print_mem(void *start_addr, uint32_t length);

static inline void get_tmp_file_name(char *buf, int len)
{
    snprintf(buf, len, "/tmp/tmp_file.%d", getpid());
}

#endif

