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

uint64_t rdtsc()
{
        uint32_t lo,hi;

        __asm__ __volatile__
        (
         "rdtsc":"=a"(lo),"=d"(hi)
        );
        return (uint64_t)hi<<32|lo;
}

uint64_t get_cpu_freq()
{
        uint64_t begin = rdtsc();
        sleep(1);
        uint64_t end = rdtsc();

        return 1000000*((end - begin)/1000000);
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

