// 
// 本软件为免费、开源软件。
// 本软件的版权(包括源码及二进制发布版本)归一切公众所有。
// 您可以自由使用、传播本软件。
// 您也可以以任何形式、任何目的使用本软件(包括源码及二进制发布版本)，而不受任何版权限制。
// =====================
// 作者: 孙明保
// 邮箱: sunmingbao@126.com
//

#define IDC_STATIC                 (-1)
// frame window menu ids
#define IDM_FILE_NEW               40001
#define IDM_FILE_OPEN              40002
#define IDM_VIEW_PCAP_FILE         40003
#define IDM_FILE_SAVE              40004
#define IDM_FILE_SAVE_AS           40005
#define IDM_APP_EXIT               40008

#define IDM_APP_LEFT_WIN           40020
#define IDM_APP_BOTTOM_WIN         40021
#define IDM_APP_TOOLBAR_WIN        40022
#define IDM_APP_STATUSBAR_WIN      40023

#define IDM_APP_QUERY_CLR_STATS    40031
#define IDM_APP_CLR_STATS          40032
#define IDM_APP_NOT_CLR_STATS      40033
#define IDM_APP_QUERY_SAVE_CAPTURED_PKS   40034
#define IDM_APP_CLR_CAPTURED_PKS   40035
#define IDM_APP_QUERY_SAVE_CAPTURED_PKS_2   40036

#define IDM_GET_NEW_VERSION        40041
#define IDM_OPEN_OFFICIAL_SITE     40042
#define IDM_GET_SOURCE             40043
#define IDM_APP_ABOUT              40044
#define IDM_NEW_VERSION_NOTICE     40045
#define IDM_USER_MANUAL            40046
#define IDM_USER_MANUAL_LINUX      40047
#define ID_FILE_RECENT_CFG_FILE_BEGIN     40050
#define ID_FILE_RECENT_PCAP_FILE_BEGIN    40060


// stream window menu ids
#define IDM_STREAM_NEW             40101
#define IDM_STREAM_NEW_HEX         40121
#define IDM_STREAM_NEW_BIN         40122
#define IDM_STREAM_NEW_PCAP        40123

#define IDM_STREAM_EDIT            40102

#define IDM_STREAM_MAKE_FRAGS      40103
#define IDM_STREAM_SEL2PCAP        40104

#define IDM_STREAM_COPY            40105
#define IDM_STREAM_PASTE           40106
#define IDM_STREAM_DEL             40107
#define IDM_STREAM_DEL_SEL         40108

#define IDM_STREAM_SEL_ALL         40109
#define IDM_STREAM_SEL_RVS         40110

#define IDM_STREAM_2_BIN           40111
#define IDM_STREAM_2_TEXT          40112

// stats window menu && pkt edit menu ids
#define IDM_STATS_WIN_COPY2CLIP_BOARD        40201
#define IDM_STATS_WIN_CLR                    40202

#define IDM_PKT_PING_REQ                    40203
#define IDM_PKT_PING_REPLY                  40204
#define IDM_PKT_ARP_REQ                     40205
#define IDM_PKT_ARP_REPLY                   40206
#define IDM_PKT_TCP_SYN                     40207
#define IDM_PKT_UDP                         40208

#define IDM_PKT_ARP_REQ_IPV6                     40209
#define IDM_PKT_ARP_REPLY_IPV6                   40210
#define IDM_PKT_ND_REQ_IPV6                      40211
#define IDM_PKT_ND_REPLY_IPV6                    40212
#define IDM_PKT_PING_REQ_IPV6                    40213
#define IDM_PKT_PING_REPLY_IPV6                  40214
#define IDM_PKT_MAX                              40214
// toolbar ids
#define IDT_TOOLBAR_STOP           40301
#define IDT_TOOLBAR_START          40302
#define IDT_TOOLBAR_SEL_NET_CARD   40303
#define IDT_TOOLBAR_CAPTURE        40304
#define IDT_TOOLBAR_VIEW_CAPTURE   40305
#define IDT_TOOLBAR_CAP_WHILE_SND  40306

// stream edit dlg ids
#define  ID_SED_DYNAMIC_EDIT        40499
#define  ID_SED_LEN                 40400
#define  ID_SED_NAME                40401
#define  ID_SED_HEX_EDIT            40404
#define  ID_SED_TREE_VIEW           40405
#define  ID_SED_IP_CHECKSUM         40406
#define  ID_SED_ICMP_CHECKSUM       40407
#define  ID_SED_TCP_UDP_CHECKSUM    40408
#define  ID_SED_IP_LEN              40409
#define  ID_SED_UDP_LEN             40410
#define  ID_SED_DYNAMIC_COMB        40411
#define  ID_SED_DYNAMIC_RULE_BUTTON 40412
#define  ID_SED_UPDATE_LEN          40413
#define  ID_SED_UPDATE_NOW          40414
#define  ID_SED_DYNAMIC_COMB_ETH    40415

// fc dlg ids
#define  ID_FC_SPEED_TYPE           40550
#define  ID_FC_SPEED_INFO_PREFFIX   40551
#define  ID_FC_SPEED_INFO_SUFFIX    40552
#define  ID_FC_SPEED_VALUE          40553
#define  ID_FC_SND_MODE             40554
#define  ID_FC_SND_TIMES_CNT        40555
#define  ID_FC_SND_MODE_PREFFIX   40556
#define  ID_FC_SND_MODE_SUFFIX    40557

// pkt cap cfg dlg ids
#define ID_PKT_CAP_FILTER_STR    40609

#define ID_PKT_CAP_STR1          40610
#define ID_PKT_CAP_PKT_TYPE      40611
#define ID_PKT_CAP_STR2          40612
#define ID_PKT_CAP_SIP           40613
#define ID_PKT_CAP_STR3          40614
#define ID_PKT_CAP_DIP           40615
#define ID_PKT_CAP_STR4          40616
#define ID_PKT_CAP_PROTOCOL      40617

#define ID_PKT_CAP_STR5          40618
#define ID_PKT_CAP_SPORT         40619
#define ID_PKT_CAP_STR6          40622
#define ID_PKT_CAP_DPORT         40623

#define ID_PKT_CAP_CFG_MODE_NORMAL     40650
#define ID_PKT_CAP_CFG_MODE_ADVANCED   40651
#define ID_PKT_CAP_CFG_STAT_ONLY       40653
#define ID_PKT_CAP_CFG_SAVE2FILE       40654

// pkt view dlg ids
#define  ID_VIEW_STREAM_LV            40703
#define  ID_VIEW_STREAM_HEX_EDIT      40704
#define  ID_VIEW_STREAM_TREE_VIEW     40705
#define  ID_VIEW_STREAM_SEL_ALL       40706
#define  ID_VIEW_STREAM_SEL_RSV       40707
#define  ID_VIEW_STREAM_DEL_SEL       40708
#define  ID_VIEW_STREAM_SEL2STREAM    40709
#define  ID_VIEW_STREAM_SAVE_AS       40710

// pkt cap dlg ids
#define ID_PKT_CAP_CAP          40848
#define ID_PKT_CAP_VIEW_RESULT  40849
#define ID_PKT_CAP_CFG_INFO     40850
#define ID_PKT_CAP_CFG          40851

#define ID_PKT_CAP_TOTAL        40852

#define ID_PKT_CAP_L2           40853

#define ID_PKT_CAP_ARP          40854
#define ID_PKT_CAP_RARP         40855
#define ID_PKT_CAP_OTHER_L2     40856

#define ID_PKT_CAP_IP           40857
#define ID_PKT_CAP_ICMP         40858
#define ID_PKT_CAP_UDP          40859
#define ID_PKT_CAP_TCP          40860
#define ID_PKT_CAP_OTHER        40861

#define ID_PKT_CAP_IPv6         40862
#define ID_PKT_CAP_ICMP6         40863
#define ID_PKT_CAP_UDP6          40864
#define ID_PKT_CAP_TCP6          40865
#define ID_PKT_CAP_OTHER6        40866
#define ID_PKT_CAP_CFG_FILTERS_MENU_BTN   40867
#define IDM_PCAP_FILTER_PORT              40868
#define IDM_PCAP_FILTER_TCP               40869
#define IDM_PCAP_FILTER_TCP_PORT          40870
#define IDM_PCAP_FILTER_SRC_IP            40871
#define IDM_PCAP_FILTER_DST_IP            40872
#define IDM_PCAP_FILTER_HOST              40873
#define IDM_PCAP_FILTER_SRC_IP_UDP_PORT   40874
#define IDM_PCAP_FILTER_ETHER_PROTO       40875
#define IDM_PCAP_FILTER_ETHER_SRC         40876
#define IDM_PCAP_FILTER_ETHER_DST         40877
#define IDM_PCAP_FILTER_ETHER_HOST        40878
#define IDM_PCAP_FILTER_ETHER_SRC_PROTO   40879
#define IDM_PCAP_FILTER_MAX               40879
// about dlg ids
#define  ID_ABOUT_INFO          40900
#define  ID_AB_SOURCE           40901
#define  ID_AB_SITE             40902
#define  ID_AB_UPDATE           40903
#define  ID_AB_LINUX            40904
// rule cfg dlg ids
#define ID_RULE_CFG_BASE        41000
#define ID_RULE_CFG_MAX         41001
#define ID_RULE_CFG_STEP        41002
#define ID_RULE_CFG_USE_RULE    41003

// frag dlg ids
#define ID_FRAG_PAYLOAD             41200


// misc ids
#define ID_DYNAMIC_EDIT              45000
#define ID_VER_UPDATE_CLOSE_BUTTON   45001
#define ID_VER_UPDATE_NONEED_NOTICE  45002
#define ID_PKT_EDIT_MENU_BTN         45003