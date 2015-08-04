/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef  __MISC_UTILS_H__
#define  __MISC_UTILS_H__

#include <pthread.h>
#include <stdint.h>

void nano_sleep(long sec, long nsec);
int set_thread_cpu_affinity(pthread_t thread, int cpu_begin, int cpu_end);

uint64_t rdtsc();
uint64_t get_cpu_freq();
int  get_data_from_file(unsigned char *buf, const char *file, int len);
void mac_str2n(unsigned char *mac, char *info_usr);
#endif

