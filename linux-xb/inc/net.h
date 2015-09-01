/* 
 * 本软件为免费、开源软件。
 * 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
 * 您可以自由使用、传播本软件。
 * 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
 * =====================
 * 作者: 孙明保
 * 邮箱: sunmingbao@126.com
 */

#ifndef __NET_H_
#define __NET_H_

#include <stdint.h>
#include <arpa/inet.h>
#include "defs.h"


#define    ETHERNET_HDR_LEN  14
#define    ETHERNET_TAG_LEN  4
#define    MAX_IP_PACKET_LEN 65535
#define    MAX_PACKET_LEN    (ETHERNET_HDR_LEN+ETHERNET_TAG_LEN+MAX_IP_PACKET_LEN)
#define    MIN_PACKET_LEN    48

typedef struct
{
    unsigned char dst[6];
    unsigned char src[6];
    unsigned short type;
} __attribute__ ((packed)) t_ether_packet;

#define ETH_P_VLAN	0x8100		/* Ethernet Loopback packet	*/
#define ETH_P_LOOP	0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_ECHO	0x0200		/* Ethernet Echo packet		*/
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/

#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/
#define ETH_P_ARP	0x0806		/* Address Resolution packet	*/
#define ETH_P_RARP  0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_IPV6	0x86DD		/* IPv6		*/

typedef struct
{
    unsigned char dst[6];
    unsigned char src[6];
    char     tag_802_1Q[4];
    unsigned short type;
} __attribute__ ((aligned (1))) t_ether_vlan_packet;

static inline uint16_t get_u16_from_pkt(const void *p_eth_hdr, int offset)
{
    uint16_t *var_addr =  p_eth_hdr + offset;
    return ntohs(*var_addr);
}


static inline uint16_t get_u32_from_pkt(const void *p_eth_hdr, int offset)
{
    uint32_t *var_addr =  p_eth_hdr + offset;
    return ntohl(*var_addr);
}


static inline __u16 get_eth_type_from_addr(void *addr)
{
    __u16 *p_type = addr;
    return ntohs(*p_type);
}


static inline __u16 set_eth_type_to_addr(__u16 type, void *addr)
{
    __u16 *p_type = addr;
    return *p_type = htons(type);
}


static inline int get_vlan_tag_nr(const void *p_eth_hdr)
{
    int ret = 0;
    int offset = 12;
    while (get_u16_from_pkt(p_eth_hdr, offset)==ETH_P_VLAN)
    {
         ret++;
         offset += 4;
    }


    return ret;
}


static inline int eth_is_vlan(const void *p_eth_hdr)
{
    const t_ether_packet *pt_eth  = p_eth_hdr;
    __u16 type = ntohs(pt_eth->type);
    return (type == ETH_P_VLAN);
}


static inline __u16 eth_type(const void *p_eth_hdr)
{
    __u16 ret = 0;
    int offset = 12;
    while ((ret=get_u16_from_pkt(p_eth_hdr, offset))==ETH_P_VLAN)
    {
         offset += 4;
    }


    return ret;
}


static inline int eth_hdr_len(const void *p_eth_hdr)
{
    int ret = sizeof(t_ether_packet);
    if (!eth_is_vlan(p_eth_hdr))
        return ret;

    return ret + get_vlan_tag_nr(p_eth_hdr)*4;
}


static inline void * eth_data(const void *p_eth_hdr)
{
    return (void *)p_eth_hdr+eth_hdr_len(p_eth_hdr);
}



typedef struct 
{
	__u8	ihl:4,
		version:4;
	__u8	tos;
	__u16	tot_len;
	__u16	id;
	__u16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__u16	check;
	__u32	saddr;
	__u32	daddr;
	/*The options start here. */
} __attribute__((packed)) t_ip_hdr ;

#define FIXED_IP_HDR_LEN    20
#define MIN_PKT_LEN         (FIXED_IP_HDR_LEN+14)
static inline int ip_hdr_len(t_ip_hdr *iph)
{
    return iph->ihl*4;
}

static inline int ip_pkt_len(t_ip_hdr *iph)
{
    return ntohs(iph->tot_len);
}

static inline void * ip_data(t_ip_hdr *iph)
{
    return (void *)iph+ip_hdr_len(iph);
}

static inline int ip_data_len(t_ip_hdr *iph)
{
    return ip_pkt_len(iph)- ip_hdr_len(iph);
}

static inline void set_ip_pkt_len(t_ip_hdr *iph, int len)
{
    iph->tot_len=htons(len);
}


void ip_update_check(t_ip_hdr *iph);
int ip_checksum_wrong(t_ip_hdr *iph);

typedef struct 
{
  __u8		type;
  __u8		code;
  __u16		checksum;
  union {
	struct {
		__u16	id;
		__u16	sequence;
	} echo;
	__u32	gateway;
	struct {
		__u16	__unused;
		__u16	mtu;
	} frag;
  } un;
} __attribute__((packed)) t_icmp_hdr;
#define  FIXED_ICMP_HDR_LEN    (4)
#define  FIXED_ICMP_ECHO_HDR_LEN    (8)
static inline int icmp_hdr_len(void *p_icmp_hdr)
{
    t_icmp_hdr *pt_icmp_hdr = p_icmp_hdr;
    if ((pt_icmp_hdr->type==8 || pt_icmp_hdr->type==0)
        && pt_icmp_hdr->code==0 )
    {
        return FIXED_ICMP_ECHO_HDR_LEN;

    }
    return FIXED_ICMP_HDR_LEN;
}

static inline int icmp6_hdr_len(void *p_icmp_hdr)
{
    t_icmp_hdr *pt_icmp_hdr = p_icmp_hdr;
    if ((pt_icmp_hdr->type==128 || pt_icmp_hdr->type==129)
        && pt_icmp_hdr->code==0 )
    {
        return FIXED_ICMP_ECHO_HDR_LEN;

    }
    return FIXED_ICMP_HDR_LEN;
}

typedef struct
{
	__u8 type;
	__u8 code;		/* For newer IGMP */
	__u16 csum;
	__u32 group;
}__attribute__((packed)) t_igmp_hdr;

void icmp_igmp_update_check(t_ip_hdr *iph);
int icmp_igmp_checksum_wrong(t_ip_hdr *iph);

typedef struct 
{
	__u32	saddr;
	__u32	daddr;
	__u8	bz;
	__u8	protocol;
	__u16	len;
} __attribute__((packed)) t_tcp_udp_pseudo_hdr;


typedef struct 
{
	__u16	source;
	__u16	dest;
	__u32	seq;
	__u32	ack_seq;

	__u16	res1:4,
		doff:4,
		fin:1,
		syn:1,
		rst:1,
		psh:1,
		ack:1,
		urg:1,
		ece:1,
		cwr:1;

	__u16	window;
	__u16	check;
	__u16	urg_ptr;
} __attribute__((packed)) t_tcp_hdr ;
static inline int tcp_hdr_len(void *pt_tcp_hdr)
{
    return ((t_tcp_hdr *)pt_tcp_hdr)->doff*4;
}

typedef struct 
{
	__u16	source;
	__u16	dest;
	__u16	len;
	__u16	check;
} __attribute__ ((aligned (1))) t_udp_hdr;

static inline int udp_data_len(t_ip_hdr *iph)
{
    return ip_data_len(iph) - sizeof(t_udp_hdr);
}

static inline int tcp_data_len(t_ip_hdr *iph)
{
    return ip_data_len(iph) - tcp_hdr_len(ip_data(iph));
}

void tcp_update_check(t_ip_hdr *iph);
void udp_update_check(t_ip_hdr *iph);
int tcp_checksum_wrong(t_ip_hdr *iph);
int udp_checksum_wrong(t_ip_hdr *iph);

#define    FIXED_ARP_HDR_LEN    (8)
typedef struct
{
	unsigned short	ar_hrd;		/* format of hardware address	*/
	unsigned short	ar_pro;		/* format of protocol address	*/
	unsigned char	ar_hln;		/* length of hardware address	*/
	unsigned char	ar_pln;		/* length of protocol address	*/
	unsigned short	ar_op;		/* ARP opcode (command)		*/

#if 1
	 /*
	  *	 Ethernet looks like this : This bit is variable sized however...
	  */
	unsigned char		ar_sha[6];	/* sender hardware address	*/
	unsigned char		ar_sip[4];		/* sender IP address		*/
	unsigned char		ar_tha[6];	/* target hardware address	*/
	unsigned char		ar_tip[4];		/* target IP address		*/
#endif

} __attribute__((packed))  t_arp_hdr;

static inline int arp_pkt_len(t_arp_hdr *pt_arp_hdr)
{
    return FIXED_ARP_HDR_LEN + 12 + pt_arp_hdr->ar_pln*2;
}

#define IPV6_HDR_LEN    40
#define IPV6_ADDR_LEN   16
typedef struct 
{
	__u8			priority:4,
				    version:4;
	__u8			flow_lbl[3];

	__u16			payload_len;
	__u8			nexthdr;
	__u8			hop_limit;

	__u8	saddr[IPV6_ADDR_LEN];
	__u8    daddr[IPV6_ADDR_LEN];
} __attribute__((packed))  t_ipv6_hdr;

typedef struct 
{
	__u8			nexthdr;
	__u8			rsv;

	__u16	frag_off;
	__u32   id;
} __attribute__((packed))  t_ipv6_frag_hdr;

static inline void * ip6_data(t_ipv6_hdr *ip6h)
{
    return (void *)ip6h+IPV6_HDR_LEN;
}

static inline int ip6_data_len(t_ipv6_hdr *ip6h)
{
    return ntohs(ip6h->payload_len);
}
static inline int ip6_pkt_len(t_ipv6_hdr *ip6h)
{
    return ip6_data_len(ip6h)+IPV6_HDR_LEN;
}
static inline void set_ip6_pkt_len(t_ipv6_hdr *ip6h, int len)
{
    ip6h->payload_len=htons(len-IPV6_HDR_LEN);
}

typedef struct 
{
	__u8	saddr[IPV6_ADDR_LEN];
	__u8	daddr[IPV6_ADDR_LEN];
	__u8	bz;
	__u8	protocol;
	__u16	len;
} __attribute__((packed)) t_tcp_udp_pseudo_hdr6;


static inline int ip_pkt_is_frag(t_ether_packet *pt_eth_hdr)
{
    int type = eth_type(pt_eth_hdr);
    t_ip_hdr *iph = eth_data(pt_eth_hdr);
    t_ipv6_hdr *ipv6h = eth_data(pt_eth_hdr);
    if (type==ETH_P_IP)
        return ntohs(iph->frag_off)&((1<<14) - 1);
        
    if (type==ETH_P_IPV6)
        return (IPPROTO_FRAGMENT==ipv6h->nexthdr);
    
    return 0;
}

static inline int ip_frag_offset(t_ether_packet *pt_eth_hdr)
{
    t_ip_hdr *iph = eth_data(pt_eth_hdr);
    t_ipv6_hdr *ipv6h = (void *)iph;
    t_ipv6_frag_hdr *frag_hdr;
    
    if (eth_type(pt_eth_hdr)==ETH_P_IP)
        return ntohs(iph->frag_off)&((1<<13) - 1);
    
    frag_hdr=ip6_data(ipv6h);
    return ntohs(frag_hdr->frag_off)>>3;
}

void get_protocol_name(int protocol, char *name);
int get_eth_type_name(int type, char *info);






void ip_str2n(void *field_addr, char *info);
void ip_n2str(char *info, void * field_addr);
void ip6_n2str(char *info, void * field_addr);
void ip6_str2n(void *field_addr, char *info);



void get_src_addr(char *info, t_ether_packet *pt_eth_hdr);
void get_dst_addr(char *info, t_ether_packet *pt_eth_hdr);
void get_proto_name(char *info, t_ether_packet *pt_eth_hdr);

int udp_checksum_wrong6(t_ipv6_hdr *ip6h);
int tcp_checksum_wrong6(t_ipv6_hdr *ip6h);
int icmp_checksum_wrong6(t_ipv6_hdr *ip6h);

void udp_update_check6(t_ipv6_hdr *ip6h);
void tcp_update_check6(t_ipv6_hdr *ip6h);
void icmp_update_check6(t_ipv6_hdr *ip6h);

unsigned short tcp_udp_checksum6(t_ipv6_hdr *ip6h);

extern char *protocol_name_map[];


#endif

