/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include "debug.h"
#include "socket.h"

char * get_ipstr(struct sockaddr_in *sock_addr, char *ip)
{
    char *tmp_ipstr=(char *)inet_ntoa(sock_addr->sin_addr);
    if (ip != NULL)
        strcpy(ip, tmp_ipstr);

    return tmp_ipstr;

}

uint16_t get_port(struct sockaddr_in *sock_addr, uint16_t *port)
{
    uint16_t tmp_port=ntohs(sock_addr->sin_port);
    if (port != NULL)
    *port=tmp_port;

    return tmp_port;
}

int sockaddr_equal(struct sockaddr_in *sock_addr1, struct sockaddr_in *sock_addr2)
{
    return (sock_addr1->sin_family == sock_addr2->sin_family) &&
        (sock_addr1->sin_port == sock_addr2->sin_port) &&
        (sock_addr1->sin_addr.s_addr == sock_addr2->sin_addr.s_addr);
}


void make_sockaddr(struct sockaddr_in *sock_addr, uint32_t ip, uint16_t port)
{
    memset(sock_addr, 0, sizeof(struct sockaddr_in));
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_addr.s_addr = ip;
    sock_addr->sin_port = port;

}


void set_socket_timeout(int sockfd, int snd, int rcv)
{
    struct timeval timeout; 

    if (snd != -1)
    {
        timeout.tv_sec = snd;
        timeout.tv_usec = 0;

        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,  sizeof(timeout));
    }

    if (rcv != -1)
    {
        timeout.tv_sec = rcv;
        timeout.tv_usec = 0;

        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,  sizeof(timeout));
    }

}


void set_useful_sock_opt(int sockfd)
{
    int nRecvBuf=256*1024;
    int nSendBuf=256*1024;
    int reuse_addr = 1;
    socklen_t optlen = sizeof(int);
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&nRecvBuf,optlen); 
    setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&nSendBuf,optlen);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse_addr,optlen);
}

int get_if_idx(const char *if_name)
{
    struct ifreq       ifr;
    int ret, sockfd;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

     memset ((void*)&ifr, 0, sizeof (ifr));
 
     snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), if_name);
 
     ret = ioctl(sockfd, SIOCGIFINDEX, &ifr);
     if (ret < 0) {
        
        ERR_DBG_PRINT_QUIT("get idx of interface %s failed", if_name);
     }

     ret = ifr.ifr_ifindex;
     close(sockfd);

     return ret;

}

int create_l2_raw_socket(const char *if_name)
{
    int ret;
    struct sockaddr_ll sock_addr = {
        .sll_family = AF_PACKET,
        .sll_protocol = 0,
        .sll_ifindex = get_if_idx(if_name)
    };
    
    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd<0)
        ERR_DBG_PRINT_QUIT("Create L2 socket failed");

    ret = bind(fd, (struct sockaddr *)&sock_addr, sizeof(struct sockaddr_ll));
    if (ret<0)
        ERR_DBG_PRINT_QUIT("bind socket failed");

    return fd;
}

