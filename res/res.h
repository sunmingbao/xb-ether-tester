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

#define IDM_FILE_NEW               40001
#define IDM_FILE_OPEN              40002
#define IDM_VIEW_PCAP_FILE         40003
#define IDM_FILE_SAVE              40004
#define IDM_FILE_SAVE_AS           40005
#define IDM_APP_EXIT               40006
#define IDM_FILE_EXPORT            40007

#define IDM_EDIT_COPY              40008
#define IDM_EDIT_PASTE             40009

#define IDM_APP_CLR_STATS          40010
#define IDM_APP_CLR_CAPTURED_PKS   40011
#define IDM_APP_QUERY_CLR_STATS    40012
#define IDM_APP_NOT_CLR_STATS      40013
#define IDM_APP_QUERY_SAVE_CAPTURED_PKS   40014

#define IDM_GET_NEW_VERSION        40015
#define IDM_OPEN_OFFICIAL_SITE     40016
#define IDM_GET_SOURCE             40017
#define IDM_APP_HELP               40018
#define IDM_APP_ABOUT              40019

#define IDM_APP_LEFT_WIN           40020
#define IDM_APP_BOTTOM_WIN         40021
#define IDM_APP_TOOLBAR_WIN        40022
#define IDM_APP_STATUSBAR_WIN      40023

#define IDM_STREAM_NEW             40024
#define IDM_STREAM_EDIT            40025
#define IDM_STREAM_MAKE_FRAGS      40026
#define IDM_STREAM_DEL             40027
#define IDM_STREAM_DEL_SEL         40028
#define IDM_STREAM_SEL_ALL         40029
#define IDM_STREAM_SEL_RVS         40030
#define IDM_STREAM_SEL2PCAP        40031

#define IDM_STREAM_COPY            40032
#define IDM_STREAM_PASTE           40033

#define IDM_STATS_WIN_COPY2CLIP_BOARD        40034
#define IDM_STATS_WIN_CLR                    40035

#define IDT_TOOLBAR_STOP           40040
#define IDT_TOOLBAR_START          40041
#define IDT_TOOLBAR_SEL_NET_CARD   40042
#define IDT_TOOLBAR_CAPTURE        40043
#define IDT_TOOLBAR_VIEW_CAPTURE   40044
#define IDT_CAPTURE_CHECKBOX       40045
#define IDT_TOOLBAR_CAP_WHILE_SND  40050

#define  ID_SED_DYNAMIC_EDIT        40099
#define  ID_SED_LEN                 40100
#define  ID_SED_NAME                40101
#define  ID_SED_HEX_EDIT            40104
#define  ID_SED_TREE_VIEW           40105
#define  ID_SED_IP_CHECKSUM         40106
#define  ID_SED_ICMP_CHECKSUM       40107
#define  ID_SED_TCP_UDP_CHECKSUM    40108
#define  ID_SED_IP_LEN              40109
#define  ID_SED_UDP_LEN             40110
#define  ID_SED_DYNAMIC_COMB        40111
#define  ID_SED_DYNAMIC_RULE_BUTTON 40112
#define  ID_SED_UPDATE_LEN           40113
#define  ID_SED_UPDATE_NOW           40114

#define  ID_DYNAMIC_EDIT            40200

#define  ID_FC_SPEED_TYPE           40250
#define  ID_FC_SPEED_INFO_PREFFIX   40251
#define  ID_FC_SPEED_INFO_SUFFIX    40252
#define  ID_FC_SPEED_VALUE          40253
#define  ID_FC_SND_MODE             40254
#define  ID_FC_SND_TIMES_CNT        40255
#define  ID_FC_SND_MODE_PREFFIX   40256
#define  ID_FC_SND_MODE_SUFFIX    40257

#define ID_PKT_CAP_FILE_PATH     40300
#define ID_PKT_CAP_FILE_BROWSE   40301

#define ID_PKT_CAP_FILTER_STR    40309

#define ID_PKT_CAP_STR1          40310
#define ID_PKT_CAP_PKT_TYPE      40311
#define ID_PKT_CAP_STR2          40312
#define ID_PKT_CAP_SIP           40313
#define ID_PKT_CAP_STR3          40314
#define ID_PKT_CAP_DIP           40315
#define ID_PKT_CAP_STR4          40316
#define ID_PKT_CAP_PROTOCOL      40317

#define ID_PKT_CAP_STR5          40318
#define ID_PKT_CAP_SPORT         40319
#define ID_PKT_CAP_STR6          40322
#define ID_PKT_CAP_DPORT         40323

#define ID_PKT_CAP_CFG_MODE_NORMAL     40350
#define ID_PKT_CAP_CFG_MODE_ADVANCED   40351
#define ID_PKT_CAP_CFG_STAT_ONLY       40353
#define ID_PKT_CAP_CFG_SAVE2FILE       40354

#define  ID_VIEW_STREAM_LV            40403
#define  ID_VIEW_STREAM_HEX_EDIT      40404
#define  ID_VIEW_STREAM_TREE_VIEW     40405
#define  ID_VIEW_STREAM_SEL_ALL       40406
#define  ID_VIEW_STREAM_SEL_RSV       40407
#define  ID_VIEW_STREAM_DEL_SEL       40408
#define  ID_VIEW_STREAM_SEL2STREAM    40409
#define  ID_VIEW_STREAM_SAVE_AS       40410

#define ID_PKT_CAP_CAP          40448
#define ID_PKT_CAP_VIEW_RESULT  40449
#define ID_PKT_CAP_CFG_INFO     40450
#define ID_PKT_CAP_CFG          40451
#define ID_PKT_CAP_TOTAL        40452
#define ID_PKT_CAP_ARP          40453
#define ID_PKT_CAP_RARP         40454
#define ID_PKT_CAP_OTHER_L2     40455
#define ID_PKT_CAP_IP           40456
#define ID_PKT_CAP_ICMP         40457
#define ID_PKT_CAP_UDP          40458
#define ID_PKT_CAP_TCP          40459
#define ID_PKT_CAP_IPv6         40460
#define ID_PKT_CAP_OTHER        40461
#define ID_PKT_CAP_L2           40462

#define  ID_ABOUT_INFO          40500
#define  ID_AB_SOURCE           40501
#define  ID_AB_SITE             40502
#define  ID_AB_UPDATE           40503

#define ID_RULE_CFG_BASE        40550
#define ID_RULE_CFG_MAX         40551
#define ID_RULE_CFG_STEP        40552
#define ID_RULE_CFG_USE_RULE    40553

#define ID_FRAG_NUM             40600
