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
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "net.h"

void ip_str2n(void *field_addr, char *info)
{
    *(unsigned int *)field_addr=inet_addr(info);
}

void ip_n2str(char *info, void * field_addr)
{
    unsigned int addr = ntohl(*(unsigned int*)field_addr);
            sprintf(info, "%d.%d.%d.%d"
            , (addr>>24) & 0xff
            , (addr>>16) & 0xff
            , (addr>>8) & 0xff
            , (addr) & 0xff);

}


int is_addr_char(int c)
{
    if (':'==c || '.'==c || isxdigit(c))
    return 1;
    
    return 0;
}

void trim_addr(char *output, char *input)
{
    char tmp[64] = {0};
    char *pd = tmp;
    char *ps = input;
    
    while (*ps != 0)
    {
	if (is_addr_char(*ps))
	{
		*pd=*ps;
		pd++;		
	}
	
	ps++;
	
    }

    
    
    strcpy(output, tmp);
    
}

void ip6_addr_uniform(char *input)
{
    char *ipv4_begin;
    unsigned char v4_addr[4];
    if (NULL==strchr(input, '.')) return;
    ipv4_begin = strrchr(input, ':');
    ipv4_begin++;
    ip_str2n(v4_addr, ipv4_begin);
    sprintf(ipv4_begin, "%02x", (unsigned)v4_addr[0]);
    sprintf(ipv4_begin+2, "%02x", (unsigned)v4_addr[1]);
    *(ipv4_begin+4)=':';
    sprintf(ipv4_begin+5, "%02x", (unsigned)v4_addr[2]);
    sprintf(ipv4_begin+7, "%02x", (unsigned)v4_addr[3]);
}





void ip6_n2str(char *info, void * field_addr)
{
    unsigned char *src=field_addr;
    int i;
    char tmp[32] = {0};
    sprintf(info, "%02x%02x", (unsigned)src[0], (unsigned)src[1]);
    for (i=2;i<16;i+=2)
    {
        sprintf(tmp, ":%02x%02x", (unsigned)src[i], (unsigned)src[i+1]);
        strcat(info, tmp);

    }

}


unsigned short csum(unsigned char * addr,
					  int count) 
{
           /* Compute Internet Checksum for "count" bytes
            *         beginning at location "addr".
            */
       register long sum = 0;

        while( count > 1 )  {
           /*  This is the inner loop */
               sum += ntohs(*(unsigned short*)addr);
               count -= 2;
               addr+=2;
       }

           /*  Add left-over byte, if any */
       if( count > 0 )
               sum += (((unsigned short)(*addr))<<8);

           /*  Fold 32-bit sum to 16 bits */
       while (sum>>16)
           sum = (sum & 0xffff) + (sum >> 16);

       return htons(~sum);
   }

unsigned short ip_check(t_ip_hdr *iph)
{
	unsigned short ori_check=iph->check;
    unsigned short new_check;
    iph->check = 0;
	new_check = csum((unsigned char *)iph, iph->ihl*4);
    iph->check = ori_check;
    return new_check;
}


void ip_update_check(t_ip_hdr *iph)
{
	iph->check = ip_check(iph);
}

int ip_checksum_wrong(t_ip_hdr *iph)
{
    return iph->check !=ip_check(iph);
}

unsigned short tcp_udp_checksum(t_ip_hdr *iph)
{
    t_tcp_hdr *pt_tcp_hdr = ip_data(iph);
    t_udp_hdr *pt_udp_hdr = ip_data(iph);
    short ori_sum = 0;
    register long sum = 0;
    unsigned short tmp;
    t_tcp_udp_pseudo_hdr t_pseudo_hdr = 
    {
        iph->saddr, iph->daddr, 0, iph->protocol, htons(ip_data_len(iph))
    };
    unsigned short* addr = (void *)(&t_pseudo_hdr);
    int count=sizeof(t_tcp_udp_pseudo_hdr);

    if (iph->protocol==IPPROTO_TCP)
    {
        ori_sum = pt_tcp_hdr->check;
        pt_tcp_hdr->check = 0;

    }
    else
    {
        ori_sum = pt_udp_hdr->check;
        pt_udp_hdr->check = 0;

    }

        while( count > 1 )  {
               sum += ntohs(*addr);
               count -= 2;
               addr++;
       }

       addr = ip_data(iph);
       count=ip_data_len(iph);

       while( count > 1 )  {
               sum += ntohs(*addr);
               count -= 2;
               addr++;
       }
       
       if( count > 0 )
       {
               tmp = *((unsigned char *)addr);
               sum += (tmp<<8);
       }

           /*  Fold 32-bit sum to 16 bits */
       while (sum>>16)
           sum = (sum & 0xffff) + (sum >> 16);

    if (iph->protocol==IPPROTO_TCP)
    {
        pt_tcp_hdr->check = ori_sum;

    }
    else
    {
        pt_udp_hdr->check = ori_sum;

    }

    return htons(~sum);

}

void tcp_update_check(t_ip_hdr *iph)
{
    t_tcp_hdr *pt_tcp_hdr = ip_data(iph);
	pt_tcp_hdr->check = tcp_udp_checksum(iph);
}

void udp_update_check(t_ip_hdr *iph)
{
    t_udp_hdr *pt_udp_hdr = ip_data(iph);
	pt_udp_hdr->check = tcp_udp_checksum(iph);
}

int tcp_checksum_wrong(t_ip_hdr *iph)
{
    t_tcp_hdr *pt_tcp_hdr = ip_data(iph);
    return pt_tcp_hdr->check !=tcp_udp_checksum(iph);
}

int udp_checksum_wrong(t_ip_hdr *iph)
{
    t_udp_hdr *pt_udp_hdr = ip_data(iph);
    return pt_udp_hdr->check !=tcp_udp_checksum(iph);
}

unsigned short icmp_igmp_check(t_ip_hdr *iph)
{
    t_icmp_hdr *icmph = ip_data(iph);
    unsigned short ori_check=icmph->checksum;
    unsigned short new_check;
	icmph->checksum = 0;
	new_check = csum((unsigned char *)icmph, ip_data_len(iph));
    icmph->checksum=ori_check;
    return new_check;
}
void icmp_igmp_update_check(t_ip_hdr *iph)
{
    t_icmp_hdr *icmph = ip_data(iph);
	icmph->checksum = icmp_igmp_check(iph);
}

int icmp_igmp_checksum_wrong(t_ip_hdr *iph)
{
    t_icmp_hdr *icmph = ip_data(iph);
    return icmph->checksum !=icmp_igmp_check(iph);
}

typedef struct
{
    char *name;
    int   value;
} t_eth_type;

t_eth_type gat_eth_type[]=
{
    {"IP",   ETH_P_IP},
    {"ARP",  ETH_P_ARP},
    {"RARP", ETH_P_RARP},
    {"IPV6", ETH_P_IPV6},
    {"VLAN", ETH_P_VLAN},
    {"PPP_DISC", ETH_P_PPP_DISC},
    {"PPP_SESS", ETH_P_PPP_SES},
    {"LOOP", ETH_P_LOOP},
    {"ECHO", ETH_P_ECHO},

};

int get_eth_type_name(int type, char *info)
{
    int i;
    for (i=0;i<ARRAY_SIZE(gat_eth_type);i++)
    {
        if (type==gat_eth_type[i].value)
        {
            if (info!=NULL)
            strcpy(info, gat_eth_type[i].name);
            return i;
        }
    }

    if (info!=NULL)
        sprintf(info, "0x%04x", type);
    return -1;

}


char *protocol_name_map[] = 
{
"HOPOPT",
"ICMP",
"IGMP",
"GGP",
"IP-ENCAP",
"ST",
"TCP",
"CBT",
"EGP",
"IGP",
"BBN-RCC-MON",
"NVP-II",
"PUP",
"ARGUS",
"EMCON",
"XNET",
"CHAOS",
"UDP",
"MUX",
"DCN-MEAS",
"HMP",
"PRM",
"XNS-IDP",
"TRUNK-1",
"TRUNK-2",
"LEAF-1",
"LEAF-2",
"RDP",
"IRTP",
"ISO-TP4",
"NETBLT",
"MFE-NSP",
"MERIT-INP",
"DCCP",
"3PC",
"IDPR",
"XTP",
"DDP",
"IDPR-CMTP"      ,
"TP++"           ,
"IL"             ,
"IPv6"           ,
"SDRP"           ,
"IPv6-Route"     ,
"IPv6-Frag"      ,
"IDRP"           ,
"RSVP"           ,
"GRE"            ,
"DSR"            ,
"BNA"            ,
"ESP/IPv6-Crypt" ,
"AH/IPv6-Auth"   ,
"I-NLSP "        ,
"SWIPE"          ,
"NARP"           ,
"MOBILE",
"TLSP",
"SKIP",
"IPv6-ICMP",
"IPv6-NoNxt",
"IPv6-Opts",
"unknown",
"CFTP",
"unknown",
"SAT-EXPAK",
"KRYPTOLAN",
"RVD",
"IPPC",
"unknown",
"SAT-MON",
"VISA",
"IPCV",
"CPNX",
"CPHB",
"WSN",
"PVP",
"BR-SAT-MON",
"SUN-ND",
"WB-MON",
"WB-EXPAK",
"ISO-IP",
"VMTP",
"SECURE-VMTP",
"VINES",
"TTP",
"NSFNET-IGP",
"DGP",
"TCF",
"EIGRP",
"OSPFIGP",
"Sprite-RPC",
"LARP",
"MTP",
"AX.25",
"IPIP",
"MICP",
"SCC-SP",
"ETHERIP",
"ENCAP",
"unknown",
"GMTP",
"IFMP",
"PNNI",
"PIM",
"ARIS",
"SCPS",
"QNX",
"A/N",
"IPComp",
"SNP",
"Compaq-Peer",
"IPX-in-IP",
"VRRP",
"PGM ",
"unknown",
"L2TP",
"DDX",
"IATP",
"STP",
"SRP",
"UTI",
"SMP",
"SM",
"PTP",
"ISIS",
"FIRE",
"CRTP",
"CRUDP",
"SSCOPMCE",
"IPLT",
"SPS ",
"PIPE",
"SCTP",
"FC",
"RSVP-E2E-IGNORE",
"unknown",
"UDPLite",
"MPLS-in-IP",
"manet",
"HIP",
"Shim6",
"WESP",
"ROHC",


};

void get_protocol_name(int protocol, char *name)
{
    name[0]=0;
    if (protocol<ARRAY_SIZE(protocol_name_map))
    {
        strcpy(name, protocol_name_map[protocol]);
    }    
    else if (protocol>=143 && protocol<=252)
    {
        strcpy(name, "Unassigned");
    }
    else if (protocol == 253 || protocol == 254)
    {
        strcpy(name, "experiment");
    }
    else
    {
        strcpy(name, "Reserved");
    }
}


void get_proto_name(char *info, t_ether_packet *pt_eth_hdr)
{
    __u16 type = eth_type(pt_eth_hdr);
    char    info2[128];
        if (type==ETH_P_IP)
        {
            t_ip_hdr *iph=eth_data(pt_eth_hdr);
            get_protocol_name(iph->protocol, info2);

        }
        else if (type==ETH_P_IPV6)
        {
            t_ipv6_hdr *ip6h=eth_data(pt_eth_hdr);
            get_protocol_name(ip6h->nexthdr, info2);
        }
        else
        {
            get_eth_type_name(type, info2);
        }

    if (eth_is_vlan(pt_eth_hdr))
        sprintf(info, "[VLAN] %s", info2);
    else
        sprintf(info, "%s", info2);
}


void get_pkt_desc_info_v4(char *info, void* p_eth_hdr)
{
    t_ip_hdr *iph=eth_data(p_eth_hdr);
    t_icmp_hdr *icmp_hdr = ip_data(iph);
    t_tcp_hdr *tcp_hdr = ip_data(iph);

    switch (iph->protocol)
    {
        case IPPROTO_ICMP:

            if (icmp_hdr->type==8 && icmp_hdr->code==0)
                strcpy(info, "ping request");
            else if (icmp_hdr->type==0 && icmp_hdr->code==0)
                strcpy(info, "ping reply");
            else
                sprintf(info, "type %u code %u", (unsigned)icmp_hdr->type, (unsigned)icmp_hdr->code);

            return;
            
        case IPPROTO_IGMP:
            strcpy(info, "IGMP");
            return;

        case IPPROTO_TCP:
            sprintf(info, "port %hu->%hu[%s%s%s%s%s%s%s%s]"
                , ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest)
                ,tcp_hdr->cwr?"cwr ":""
                ,tcp_hdr->ece?"ece ":""
                ,tcp_hdr->urg?"urg ":""
                ,tcp_hdr->ack?"ack ":""
                ,tcp_hdr->psh?"psh ":""
                ,tcp_hdr->rst?"rst ":""
                ,tcp_hdr->syn?"syn ":""
                ,tcp_hdr->fin?"fin ":"");

            return;

        case IPPROTO_UDP:
            sprintf(info, "port %hu->%hu", ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest));
            return;

    }

}

void get_pkt_desc_info_v6(char *info, void* p_eth_hdr)
{
    t_ipv6_hdr *ip6h=eth_data(p_eth_hdr);
    t_tcp_hdr *tcp_hdr = ip6_data(ip6h);
    t_icmp_hdr *pt_icmp_hdr=ip6_data(ip6h);

    switch (ip6h->nexthdr)
    {
        case IPPROTO_TCP:
            sprintf(info, "port %hu->%hu[%s%s%s%s%s%s%s%s]"
                , ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest)
                ,tcp_hdr->cwr?"cwr ":""
                ,tcp_hdr->ece?"ece ":""
                ,tcp_hdr->urg?"urg ":""
                ,tcp_hdr->ack?"ack ":""
                ,tcp_hdr->psh?"psh ":""
                ,tcp_hdr->rst?"rst ":""
                ,tcp_hdr->syn?"syn ":""
                ,tcp_hdr->fin?"fin ":"");

            return;

        case IPPROTO_UDP:
            sprintf(info, "port %hu->%hu", ntohs(tcp_hdr->source), ntohs(tcp_hdr->dest));
            return;

        case IPPROTO_ICMPV6:
            if ((pt_icmp_hdr->type==128)  && (pt_icmp_hdr->code==0) )
                strcpy(info, "ping request");
            else if ((pt_icmp_hdr->type==129)  && (pt_icmp_hdr->code==0) )
                strcpy(info, "ping reply");
            else
                sprintf(info, "type %u code %u", (unsigned)pt_icmp_hdr->type, (unsigned)pt_icmp_hdr->code);

            return;

    }

}



unsigned short tcp_udp_checksum6(t_ipv6_hdr *ip6h)
{
    t_tcp_hdr *pt_tcp_hdr = ip6_data(ip6h);
    t_udp_hdr *pt_udp_hdr = ip6_data(ip6h);
    t_icmp_hdr *pt_icmp_hdr = ip6_data(ip6h);
    short ori_sum = 0;
    register long sum = 0;
    unsigned short tmp;
    t_tcp_udp_pseudo_hdr6 t_pseudo_hdr6 = 
    {
        {0}, {0}, 0, ip6h->nexthdr, htons(ip6_data_len(ip6h))
    };
    unsigned short* addr = (void *)(&t_pseudo_hdr6);
    int count=sizeof(t_pseudo_hdr6);
    memcpy(t_pseudo_hdr6.saddr, ip6h->saddr, IPV6_ADDR_LEN);
    memcpy(t_pseudo_hdr6.daddr, ip6h->daddr, IPV6_ADDR_LEN);

    if (ip6h->nexthdr==IPPROTO_TCP)
    {
        ori_sum = pt_tcp_hdr->check;
        pt_tcp_hdr->check = 0;

    }
    else if(ip6h->nexthdr==IPPROTO_UDP)
    {
        ori_sum = pt_udp_hdr->check;
        pt_udp_hdr->check = 0;

    }
    else if (ip6h->nexthdr==IPPROTO_ICMPV6)
    {
        ori_sum = pt_icmp_hdr->checksum;
        pt_icmp_hdr->checksum = 0;

    }


        while( count > 1 )  {
               sum += ntohs(*addr);
               count -= 2;
               addr++;
       }

       addr = ip6_data(ip6h);
       count=ip6_data_len(ip6h);

       while( count > 1 )  {
               sum += ntohs(*addr);
               count -= 2;
               addr++;
       }
       
       if( count > 0 )
       {
               tmp = *((unsigned char *)addr);
               sum += (tmp<<8);
       }

           /*  Fold 32-bit sum to 16 bits */
       while (sum>>16)
           sum = (sum & 0xffff) + (sum >> 16);

    if (ip6h->nexthdr==IPPROTO_TCP)
    {
        pt_tcp_hdr->check = ori_sum;

    }
    else if (ip6h->nexthdr==IPPROTO_UDP)
    {
        pt_udp_hdr->check = ori_sum;

    }
    else if (ip6h->nexthdr==IPPROTO_ICMPV6)
    {
        pt_icmp_hdr->checksum = ori_sum;

    }

    return htons(~sum);

}

void icmp_update_check6(t_ipv6_hdr *ip6h)
{
    t_icmp_hdr *pt_icmp_hdr = ip6_data(ip6h);
	pt_icmp_hdr->checksum = tcp_udp_checksum6(ip6h);
}


void tcp_update_check6(t_ipv6_hdr *ip6h)
{
    t_tcp_hdr *pt_tcp_hdr = ip6_data(ip6h);
	pt_tcp_hdr->check = tcp_udp_checksum6(ip6h);
}

void udp_update_check6(t_ipv6_hdr *ip6h)
{
    t_udp_hdr *pt_udp_hdr = ip6_data(ip6h);
	pt_udp_hdr->check = tcp_udp_checksum6(ip6h);
}

int tcp_checksum_wrong6(t_ipv6_hdr *ip6h)
{
    t_tcp_hdr *pt_tcp_hdr = ip6_data(ip6h);
    return pt_tcp_hdr->check !=tcp_udp_checksum6(ip6h);
}

int udp_checksum_wrong6(t_ipv6_hdr *ip6h)
{
    t_udp_hdr *pt_udp_hdr = ip6_data(ip6h);
    return pt_udp_hdr->check !=tcp_udp_checksum6(ip6h);
}

int icmp_checksum_wrong6(t_ipv6_hdr *ip6h)
{
    t_icmp_hdr *pt_icmp_hdr = ip6_data(ip6h);
    return pt_icmp_hdr->checksum !=tcp_udp_checksum6(ip6h);
}

