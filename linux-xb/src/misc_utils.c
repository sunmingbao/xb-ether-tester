/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */




#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <endian.h>

#include "debug.h"
#include "defs.h"

void nano_sleep(long sec, long nsec)
{
    struct timespec remain  = (struct timespec) {sec, nsec};
    struct timespec tmp = (struct timespec) {0, 0};
    while (remain.tv_sec || remain.tv_nsec)
    {
        if (0==nanosleep(&remain, &tmp)) return;
        remain = tmp;
        tmp = (struct timespec) {0, 0};
    }

}

int set_thread_cpu_affinity(pthread_t thread, int cpu_begin, int cpu_end)
{
           int j;
           cpu_set_t cpuset;

           CPU_ZERO(&cpuset);
           for (j = cpu_begin; j <= cpu_end; j++)
               CPU_SET(j, &cpuset);

           return pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}


int set_fd_nonblock(int fd)
{
    int ret;
    if((ret = fcntl(fd, F_GETFL,0))==-1)
    {   
        return ret;
    }

    ret |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, ret);
}


int  get_data_from_file(unsigned char *buf, const char *file, int len)
{
    FILE *fp = fopen(file, "rb");
    int ret;
    if (NULL==fp)  return -1;
    
    ret = fread(buf, 1, len, fp);
    fclose(fp);
    
    return ret;
}

void mac_str2n(unsigned char *mac, char *info_usr)
{
    char info[32];
    strcpy(info, info_usr);
            info[2]=0;
            mac[0]=strtol(info+0,NULL,16);
            info[5]=0;
            mac[1]=strtol(info+3,NULL,16);
            info[8]=0;
            mac[2]=strtol(info+6,NULL,16);
            info[11]=0;
            mac[3]=strtol(info+9,NULL,16);
            info[14]=0;
            mac[4]=strtol(info+12,NULL,16);
            info[17]=0;
            mac[5]=strtol(info+15,NULL,16);
}

void le2host_32(void *p)
{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint8_t tmp[4];
	uint8_t *pdest = p;
	uint32_t *p1 = (void *)tmp;
	*p1 = *(uint32_t *)p;
	pdest[0] = tmp[3];
	pdest[1] = tmp[2];
	pdest[2] = tmp[1];
	pdest[3] = tmp[0];
	
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	;
#else
#error "unknown endian"
#endif

}

void le2host_16(void *p)
{
#if __BYTE_ORDER == __BIG_ENDIAN
	uint8_t tmp[2];
	uint8_t *pdest = p;
	uint16_t *p1 = (void *)tmp;
	*p1 = *(uint16_t *)p;
	pdest[0] = tmp[1];
	pdest[1] = tmp[0];
	
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	;
#else
#error "unknown endian"
#endif

}

