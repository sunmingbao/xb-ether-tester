/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#include <stdio.h>
#include "debug.h"

void print_mem(void *start_addr, uint32_t length)
{
    char str_addr[32];
    char str_data[64];
    char str_readable[32];
    unsigned char *cur_pos = start_addr;
    int i;

    while (length >= 16)
    {
        sprintf(str_addr, "%-16lx", (unsigned long)(void *)cur_pos);
        for (i = 0; i < 16; i++)
        {
            sprintf(str_data + i*3, "%02hhx ", cur_pos[i]);
            if (cur_pos[i] > 31 &&  cur_pos[i] < 127)
            sprintf(str_readable + i, "%c", (char)(cur_pos[i]));
            else
            sprintf(str_readable + i, "%c", '*');
        }
        length -= 16;
        cur_pos += 16;
        printf("%s: %s: %s\n", str_addr, str_data, str_readable);
    }

    if (length > 0)
    {
        sprintf(str_addr, "%-16lx", (unsigned long)(void *)cur_pos);
        for (i = 0; i < length; i++)
        {
            sprintf(str_data + i*3, "%02hhx ", cur_pos[i]);
            if (cur_pos[i] > 31 &&  cur_pos[i] < 127)
            sprintf(str_readable + i, "%c", (char)(cur_pos[i]));
            else
            sprintf(str_readable + i, "%c", '*');
        }
        for (i = length; i < 16; i++)
        {
            sprintf(str_data + i*3, "%s", "   ");
            sprintf(str_readable + i, "%c", ' ');
        }
        printf("%s: %s: %s\n", str_addr, str_data, str_readable);
    }


}

int get_call_links(unsigned long *records, int nr, unsigned long *usr_provide_bp)
{
    unsigned long *bp;
    int depth=0;
    if (usr_provide_bp!=NULL)
    {
        bp=usr_provide_bp;
    }
    else
    {
#if defined(__i386)
        asm ("movl %%ebp, %0":"=qm"(bp));
#elif defined( __x86_64)
        asm ("movq %%rbp, %0":"=qm"(bp));
#else
#error unsupported arch
#endif
    }


    while (depth<nr && (*bp)!=0)
    {
        records[depth]=*(bp+1);
        bp=(void*)(*bp);
        depth++;
    }


    return depth;
}

