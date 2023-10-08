/*
 * Copyright (C) 1999-2023 Maksim Feoktistov.
 *
 * This file is part of Small HTTP server project.
 * Author: Maksim Feoktistov
 *
 *
 * Small HTTP server is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * Small HTTP server is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see https://www.gnu.org/licenses/
 *
 * Contact addresses for Email:  support@smallsrv.com
 *
 *
 */


#ifndef VPN_H
#define VPN_H
#ifdef VPN_LINUX
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <net/ethernet.h>
#include <net/route.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#else
// TODO: not Linux TUN/TAP
#endif

#include "mdef.h"
#include "srv.h"

//#define MAX_MTU  1602
#define MIN_MTU  1024
#define MAX_MTU  16384

extern "C" {

struct VPN_TUNPacket
{
  ushort len;
  ushort tun_flags;
  ushort tun_proto;
#if 0
  def BIG_ENDIAN

#define ETHERTYPE_IP_LE     ETHERTYPE_IP
#define ETHERTYPE_IPV6_LE   ETHERTYPE_IPV6
#define ETHERTYPE_ARP_LE    ETHERTYPE_ARP
#define ETHERTYPE_REVARP_LE ETHERTYPE_REVARP

#else

#define ETHERTYPE_IP_LE          8
#define ETHERTYPE_IPV6_LE   0xDD86
#define ETHERTYPE_ARP_LE    0x0608
#define ETHERTYPE_REVARP_LE	0x3580

#endif
  union {
    struct iphdr  ip4;
    struct ip6_hdr ip6;
  };
} PACKED ;

struct VPN_TAPPacket
{
  ushort len;
  struct ether_header eth;
  union {
    struct iphdr   ip4;
    struct ip6_hdr ip6;
  };
} PACKED ;

#if __BYTE_ORDER__  ==  __ORDER_BIG_ENDIAN__

#define MCAST_DETECT  0xE0000000
#define MCAST_MASK    0xF0000000

#else

#define MCAST_DETECT  0xE0
#define MCAST_MASK    0xF0

#endif

#define BROADCAST_ANY 0xFFFFffff

struct VPNclient : public Req
{

  OpenSSLConnection tls;

  union{
    long long macl;
    uint mac[2];
    uchar macb[6];
  };
  uint  ipv4;
  uint  ipv4bcast;
  uint  ipv4gw;

#ifdef USE_IPV6
  in6_addr ipv6[2];
#endif
  int  pos_pkt;
  union {
    ushort pkt_len;
    uchar pkt[MAX_MTU+ 2 + sizeof(struct ether_header) ];
    VPN_TUNPacket tunpkt;
    VPN_TAPPacket tappkt;
  };
#define tun_index postsize

  int RecvPkt();
  int SendIsUs(uchar *pktl, int tuntap);
#define PKT_NOT_US  0
#define PKT_US      1
#define PKT_BCAST   2

  int ClientConnect(OpenSSLConnection *x);

};

extern VPNclient **vpn_list;

extern maxFdSet maxVPNset;
#define VPNset maxVPNset.set
#define vpn_max_fd maxVPNset.max_fd

extern int vpn_max;
extern int vpn_count;
//volatile
extern int vpn_mutex;
extern char *tundev;
extern int  tuntap_number[3];

extern char* tuntap_ipv4[3];
extern char* tuntap_ipv4nmask[3];
extern char* tuntap_ipv6[3];
extern uint tuntap_ipv6plen[3];
extern uint vpn_rescan_us;
extern char* vpn_first_remote_ipc[2];
extern uint vpn_first_remote_ip[2];
extern uint vpn_total_remote_ip[2];
extern uint vpn_next_remote_ip[2];
//unsigned long long vpn_amask_remote_ip[2];
extern uint vpn_allocated_remote_ip;
extern uint vpn_gw[2];
extern uint vpn_nmask[2];
extern char *vpn_dns[2];
extern char *vpn_scripts_up[3];
extern char *vpn_scripts_down[3];

extern uint vpn_rescan_us;
extern unsigned long long VPNdroped;
extern unsigned long long VPNsendet;
extern unsigned long long VPNreceved;

extern char *vpn_remote_host;
extern char *vpn_passw;
extern char *vpn_user;
extern VPNclient * vpn_cln_connected;

extern const char * TUNTAPNames[3];
extern int vpn_mtu[3];


int tun_alloc(int idx);
void CloseVPNClient(int i);
int ReInitTUNTAP(char *reason, int i);
int VPN_Thread(void *);
void  CloseTunTap();
int VPN_Init();
void VPN_Done();


int VPNClient(void *);

}
#endif
