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
#include <ctype.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include "net.h"
#include "global_info.h"

void ip_str2n(void *field_addr, char *info)
{
    *(unsigned int *)field_addr=inet_addr(info);
}

void* ip_n2str(char *info, void * field_addr)
{
    static char ip_str[3*4+3+1];
    uint32_t addr = ntohl(*(unsigned int*)field_addr);
    char *output = info?info:ip_str;
            sprintf(output, "%d.%d.%d.%d"
            , (addr>>24) & 0xff
            , (addr>>16) & 0xff
            , (addr>>8) & 0xff
            , (addr) & 0xff);

    return output;
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

void ip6_addr_no_mh(char *dst, char *src)
{
    int i=0;
	char *p_digit = src;
	while (*p_digit!=0)
	{
		if (*p_digit==':') *p_digit=0;
		p_digit++;		
	}
    
	p_digit = src;
    dst[0]=0;
	while (isxdigit(*p_digit))
	{
	    sprintf(dst+i*4, "%04s", p_digit);
	    p_digit += strlen(p_digit)+1;
        i++;
		
	}
}

void ip6_str2n(void *field_addr, char *info)
{
    char str_addr[64];
    char *db_mh;
    char pure_digits[33];
    char pure_digits_hdr[64]={0};
    char pure_digits_tail[64]={0};
    char tmp_str[3] = {0};
    int i;
    unsigned char *dst = field_addr;
    trim_addr(str_addr, info);
    ip6_addr_uniform(str_addr);
    memset(pure_digits, '0', sizeof(pure_digits));
    pure_digits[32]=0;
    db_mh=strstr(str_addr, "::");

    *(str_addr + strlen(str_addr) + 1)=0;
    if (NULL==db_mh)
    {
    	ip6_addr_no_mh(pure_digits_hdr, str_addr);
    }
    else
    {
        *db_mh = 0;
        *(db_mh+1) = 0;
        ip6_addr_no_mh(pure_digits_hdr, str_addr);
        ip6_addr_no_mh(pure_digits_tail, db_mh+2);
    }

    memcpy(pure_digits, pure_digits_hdr, strlen(pure_digits_hdr));
    memcpy(pure_digits+32-strlen(pure_digits_tail)
        , pure_digits_tail
        , strlen(pure_digits_tail));

	for (i=0;i<32;i+=2)
	{
		tmp_str[0] = pure_digits[i];
		tmp_str[1] = pure_digits[i+1];
		dst[i/2]=strtol(tmp_str,NULL,16);
	}

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

int get_eth_type_value(char *name)
{
    int i;
    for (i=0;i<ARRAY_SIZE(gat_eth_type);i++)
    {
        if (0==strcmp(gat_eth_type[i].name, name))
        {
            return gat_eth_type[i].value;
        }
    }

    return str2int(name);

}

void init_eth_type_comb(HWND comb)
{
    int i;
    clear_comb(comb);

    for (i=0;i<ARRAY_SIZE(gat_eth_type);i++)
    {
        ComboBox_AddString(comb,(LPARAM)gat_eth_type[i].name);
    }
}

int get_eth_type_comb(HWND comb)
{
    char info[64];

    ComboBox_GetText(comb, info, sizeof(info));

    return get_eth_type_value(info);
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


void get_src_addr(char *info, t_ether_packet *pt_eth_hdr)
{
    __u16 type = eth_type(pt_eth_hdr);
        if (type==ETH_P_IP)
        {
            t_ip_hdr *iph=eth_data(pt_eth_hdr);
                ip_n2str(info, &(iph->saddr));

        }
        else if (type==ETH_P_IPV6)
        {
            t_ipv6_hdr *ip6h=eth_data(pt_eth_hdr);
                ip6_n2str(info, &(ip6h->saddr));
        }
        else
        {
            mac_n2str(info, pt_eth_hdr->src);
        }

}

void get_dst_addr(char *info, t_ether_packet *pt_eth_hdr)
{
    __u16 type = eth_type(pt_eth_hdr);
        if (type==ETH_P_IP)
        {
            t_ip_hdr *iph=eth_data(pt_eth_hdr);
                ip_n2str(info, &(iph->daddr));

        }
        else if (type==ETH_P_IPV6)
        {
            t_ipv6_hdr *ip6h=eth_data(pt_eth_hdr);
                ip6_n2str(info, &(ip6h->daddr));
        }
        else
        {
            mac_n2str(info, pt_eth_hdr->dst);
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

void append_err_text(char *info, uint32_t err_flags)
{
    if (err_flags==0) return;
    
    strcat(info, "(error:");
    if (err_flags&ERR_IP_CHECKSUM)
    {
        strcat(info, "ip check sum");
        goto exit;
    }

    if (err_flags&ERR_ICMP_CHECKSUM)
    {
        strcat(info, "icmp check sum");
        goto exit;
    }

    if (err_flags&ERR_IGMP_CHECKSUM)
    {
        strcat(info, "igmp check sum");
        goto exit;
    }

    if (err_flags&ERR_TCP_CHECKSUM)
    {
        strcat(info, "tcp check sum");
        goto exit;
    }

    if (err_flags&ERR_UDP_CHECKSUM)
    {
        strcat(info, "udp check sum");
        goto exit;
    }

    if (err_flags&ERR_PKT_LEN)
    {
        strcat(info, "packet length");
        goto exit;
    }
exit:
    strcat(info, ")");
    
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
            if ((pt_icmp_hdr->type==135))
                strcpy(info, "Neighbor Solicitation");
            else if ((pt_icmp_hdr->type==136))
                strcpy(info, "Neighbor Advertisement");
            else if ((pt_icmp_hdr->type==128)  && (pt_icmp_hdr->code==0) )
                strcpy(info, "ping request");
            else if ((pt_icmp_hdr->type==129)  && (pt_icmp_hdr->code==0) )
                strcpy(info, "ping reply");
            else
                sprintf(info, "type %u code %u", (unsigned)pt_icmp_hdr->type, (unsigned)pt_icmp_hdr->code);

            return;

    }

}

void get_pkt_desc_info(char *info, void* p_eth_hdr, uint32_t err_flags)
{
    t_ether_packet *pt_eth_hdr = p_eth_hdr;
    t_arp_hdr *arp_hdr = eth_data(pt_eth_hdr);
    char info_2[64];
    int type = eth_type(pt_eth_hdr);

    info[0]=0;
    
    switch (type)
    {
        case ETH_P_ARP:
            if (ntohs(arp_hdr->ar_op)==1)
            {
                if (4==arp_hdr->ar_pln)
                {
                    ip_n2str(info_2, arp_hdr->ar_tip);
                    sprintf(info, "who has %s? tell ", info_2);
                    ip_n2str(info_2, arp_hdr->ar_sip);
                    strcat(info, info_2);
                }
                else if (16==arp_hdr->ar_pln)
                {
                    ip6_n2str(info_2, arp_hdr->ar_sip+22);
                    sprintf(info, "who has %s? tell ", info_2);
                    ip6_n2str(info_2, arp_hdr->ar_sip);
                    strcat(info, info_2);
                }
            }
            else if (ntohs(arp_hdr->ar_op)==2)
            {
                if (4==arp_hdr->ar_pln)
                {
                    ip_n2str(info_2, arp_hdr->ar_sip);
                    sprintf(info, "%s is at ", info_2);
                    mac_n2str(info_2, arp_hdr->ar_sha);
                    strcat(info, info_2);
                }
                else if (16==arp_hdr->ar_pln)
                {
                    ip6_n2str(info_2, arp_hdr->ar_sip);
                    sprintf(info, "%s is at ", info_2);
                    mac_n2str(info_2, arp_hdr->ar_sha);
                    strcat(info, info_2);
                }
            }
            
            goto append_err_info;
            
        case ETH_P_RARP:
            strcpy(info, "rarp");
            goto append_err_info;

        case ETH_P_LOOP:
            strcpy(info, "Ethernet Loopback packet");
            goto append_err_info;

        case ETH_P_ECHO:
            strcpy(info, "Ethernet Echo packet");
            goto append_err_info;
            
        case ETH_P_PPP_DISC:
            strcpy(info, "PPPoE discovery messages");
            goto append_err_info;
            
        case ETH_P_PPP_SES:
            strcpy(info, "PPPoE session messages");
            goto append_err_info;
            

    }

    if (type!=ETH_P_IP && type!=ETH_P_IPV6)
    {
        sprintf(info, "eth type:0x%04x", type);
            goto append_err_info;
    }

    if (ip_pkt_is_frag(pt_eth_hdr))
    {
        sprintf(info, "frag ");
        goto append_err_info;

    }

    if (type==ETH_P_IP)
    {
        get_pkt_desc_info_v4(info, p_eth_hdr);
    }
    else
    {
        get_pkt_desc_info_v6(info, p_eth_hdr);
    }

append_err_info:
    append_err_text(info, err_flags);

    return;

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

void update_check_sum_v4(t_stream *pt_stream)
{
    t_ip_hdr *iph=eth_data(pt_stream->data);
    if (pt_stream->flags & CHECK_SUM_IP)
        ip_update_check(iph);

    if (ip_pkt_is_frag(&(pt_stream->eth_packet))) return;
    
    if (iph->protocol==IPPROTO_ICMP && (pt_stream->flags & CHECK_SUM_ICMP))
        icmp_igmp_update_check(iph);
    else if (iph->protocol==IPPROTO_IGMP && (pt_stream->flags & CHECK_SUM_IGMP))
        icmp_igmp_update_check(iph);
    else if (iph->protocol==IPPROTO_TCP && (pt_stream->flags & CHECK_SUM_TCP))
        tcp_update_check(iph);
    else if (iph->protocol==IPPROTO_UDP && (pt_stream->flags & CHECK_SUM_UDP))
        udp_update_check(iph);

}

void update_check_sum_v6(t_stream *pt_stream)
{
    t_ipv6_hdr *ip6h=eth_data(pt_stream->data);

    if (ip_pkt_is_frag(&(pt_stream->eth_packet))) return;

    if (ip6h->nexthdr==IPPROTO_TCP && (pt_stream->flags & CHECK_SUM_TCP))
        tcp_update_check6(ip6h);
    else if (ip6h->nexthdr==IPPROTO_UDP && (pt_stream->flags & CHECK_SUM_UDP))
        udp_update_check6(ip6h);
    else if (ip6h->nexthdr==IPPROTO_ICMPV6 && (pt_stream->flags & CHECK_SUM_ICMP))
        icmp_update_check6(ip6h);

}

void update_check_sum(t_stream *pt_stream)
{
    int type = eth_type(pt_stream->data);
    if (type==ETH_P_IP)
        update_check_sum_v4(pt_stream);
   else if (type==ETH_P_IPV6)
       update_check_sum_v6(pt_stream);

}


void update_len_v4(t_stream *pt_stream)
{
    t_ip_hdr *iph=eth_data(pt_stream->data);
    t_udp_hdr *udph=ip_data(iph);
    if (pt_stream->flags & IP_LEN)
        iph->tot_len = htons(pt_stream->len - eth_hdr_len(pt_stream->data));
    if (iph->protocol==IPPROTO_UDP && (pt_stream->flags & UDP_LEN))
        udph->len = htons(pt_stream->len - eth_hdr_len(pt_stream->data) - ip_hdr_len(iph));

}

void update_len_v6(t_stream *pt_stream)
{
    t_ipv6_hdr *ip6h =eth_data(pt_stream->data);
    t_udp_hdr *udph=ip6_data(ip6h);
    if (pt_stream->flags & IP_LEN)
        set_ip6_pkt_len(ip6h, pt_stream->len - eth_hdr_len(pt_stream->data));
    if (ip6h->nexthdr==IPPROTO_UDP && (pt_stream->flags & UDP_LEN))
        udph->len = htons(ip6_data_len(ip6h));

}

void update_len(t_stream *pt_stream)
{
int type = eth_type(pt_stream->data);
    if (type==ETH_P_IP)
        update_len_v4(pt_stream);
   else if (type==ETH_P_IPV6)
       update_len_v6(pt_stream);
}

int ipv4_fragable(t_ip_hdr *iph)
{
    if(ntohs(iph->frag_off)&((1<<14) - 1))
        return 0;
        
    if (ip_data_len(iph)<=8)
        return 0;

    return 1;
}

int ipv6_fragable(t_ipv6_hdr *ipv6h)
{
    if (IPPROTO_FRAGMENT==ipv6h->nexthdr)
        return 0;
    
    if (ip6_data_len(ipv6h)<=8)
        return 0;

    return 1;
}

int stream_fragable(t_stream *pt_stream)
{
    int type = eth_type(pt_stream->data);
    if (type==ETH_P_IP)
        return ipv4_fragable(eth_data(pt_stream->data));
   else if (type==ETH_P_IPV6)
       return ipv6_fragable(eth_data(pt_stream->data));

    return 0;
}


void * get_nic_FriendlyName(const char *name)
{
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    char buf[15000];
    char FriendlyName[128]="retrieve NIC name failed";
    unsigned int i = 0;

    ULONG family = AF_UNSPEC;
    PIP_ADAPTER_ADDRESSES pAddresses = (void *)buf;
    ULONG outBufLen = sizeof(buf);
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;

    dwRetVal = GetAdaptersAddresses(family, 0, NULL, pAddresses, &outBufLen);


    if (NULL==name || name[0]!='{') goto EXIT;
    if (dwRetVal != NO_ERROR) goto EXIT;

    pCurrAddresses = pAddresses;
    while (pCurrAddresses) 
    {
        if (0==memcmp(pCurrAddresses->AdapterName, name, 38))
        {
            /* pCurrAddresses->FriendlyNameΪUNICODE���룬�洢����ΪWCHAR 
               ����ת����GBK���� */
            WideCharToMultiByte(CP_ACP, 0, pCurrAddresses->FriendlyName,
                -1,  FriendlyName, 128,  NULL,  NULL);

            break;

        }
        
        pCurrAddresses = pCurrAddresses->Next;
    }

EXIT:
    return FriendlyName;
}