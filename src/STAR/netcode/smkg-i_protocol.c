// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-i_protocol.c
/// \brief TSoURDt3rd's extended network protocol data

#include "../../netcode/i_tcp_detail.h"

#include "../../netcode/d_net.h"
#include "../../netcode/i_addrinfo.h"

#include "smkg-net.h"
#include "../smkg-cvars.h"

#include "../../i_time.h"
#include "../../m_swap.h"

/* See ../doc/Holepunch-Protocol.txt */
static const INT32 hole_punch_magic = MSBF_LONG (0x52eb11);
void (*I_NetRequestHolePunch)(INT32 node) = NULL;
void (*I_NetRegisterHolePunch)(void) = NULL;

/* not one of the reserved "local" addresses */
// https://github.com/jameds/holepunch/blob/master/holepunch.c#L75
static boolean SOCK_IsExternalAddress(UINT32 p)
{
	UINT8 a = (p & 255);
	UINT8 b = ((p >> 8) & 255);

	if (p == (UINT32)~0) /* 255.255.255.255 */
		return 0;

	switch (a)
	{
		case 0:
		case 10:
		case 127:
			return false;
		case 172:
			return (b & ~15) != 16; /* 16 - 31 */
		case 192:
			return b != 168;
		default:
			return true;
	}
}

//
// boolean hole_punch(PTRPACKET c)
// Sends out holepunching data needed in order to send a request.
//
boolean hole_punch(PTRPACKET c)
{
	mysockaddr_t addr;
	UINT8 mask = 0;

#if 0
	if (c == 10 && holepunchpacket->magic == hole_punch_magic)
	{
		if (holepunchpacket->family == AF_INET && SOCK_IsExternalAddress(ntohl(holepunchpacket->ip4.sin_addr.s_addr)))
		{
			addr.ip4.sin_family = AF_INET;
			addr.ip4.sin_addr.s_addr = holepunchpacket->ip4.sin_addr.s_addr;
			addr.ip4.sin_port = holepunchpacket->ip4.sin_port;
			sendto(mysockets[0], NULL, 0, 0, &addr.any, sizeof(addr.ip4));
			CONS_Debug(DBG_NETPLAY, "hole punching ipv4 request from %s\n", SOCK_AddrToStr(&addr));
			return true;
		}
#if 1
#ifdef HAVE_IPV6
		//else if (holepunchpacket->family == AF_INET6 && SOCK_IsExternalAddress(ntohl(holepunchpacket->ip6.sin6_addr.s6_addr[mask / 8])))
		else if (holepunchpacket->family == AF_INET6 && SOCK_IsExternalAddress(ntohs(holepunchpacket->ip6.sin6_addr.s6_addr[mask / 8])))
		{
			addr.ip6.sin6_family = AF_INET6;
			memcpy(addr.ip6.sin6_addr.s6_addr, holepunchpacket->ip6.sin6_addr.s6_addr, sizeof(holepunchpacket->ip6.sin6_addr.s6_addr));
			addr.ip6.sin6_port = holepunchpacket->ip6.sin6_port;
			sendto(mysockets[0], NULL, 0, 0, &addr.any, sizeof(addr.ip6));
			CONS_Debug(DBG_NETPLAY, "hole punching ipv6 request from %s\n", SOCK_AddrToStr(&addr));
			return true;
		}
#endif
#endif
	}
#else
	(void)mask;
	if (c == 10 && holepunchpacket->magic == hole_punch_magic
		&& SOCK_IsExternalAddress(ntohl(holepunchpacket->addr)))
	{
		addr.ip4.sin_family      = AF_INET;
		addr.ip4.sin_addr.s_addr = holepunchpacket->addr;
		addr.ip4.sin_port        = holepunchpacket->port;
		sendto(mysockets[0], NULL, 0, 0, &addr.any, sizeof addr.ip4);

		CONS_Debug(DBG_NETPLAY,
				"hole punching request from %s\n", SOCK_AddrToStr(&addr));

		return true;
	}
	else
	{
		return false;
	}
#endif

	CONS_Debug(DBG_NETPLAY, "failed to hole punch request from %s\n", SOCK_AddrToStr(&addr));
	return false;
}

//
// static boolean SOCK_GetHolepunchAddr(struct sockaddr_in *sin, const char *address, const char *port, boolean test)
//
// A routine made to resemble SOCK_NetMakeNodewPort in order to get holepunching addresses.
// PORTED FROM DR.ROBOTNIK'S RING RACERS!
//
#if 0
static SINT8 SOCK_NetMakeNodewPort(const char *address, const char *port)
{
	SINT8 newnode = -1;
	struct my_addrinfo *ai = NULL, *runp, hints;
	int gaie;
	size_t i;

	 if (!port || !port[0])
		port = DEFAULTPORT;

	DEBFILE(va("Creating new node: %s@%s\n", address, port));

	memset (&hints, 0x00, sizeof (hints));
	hints.ai_flags = AI_ADDRCONFIG;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	gaie = I_getaddrinfo(address, port, &hints, &ai);
	if (gaie == 0)
	{
		newnode = getfreenode();
	}
	if (newnode == -1)
	{
		I_freeaddrinfo(ai);
		return -1;
	}
	else
		runp = ai;

	while (runp != NULL)
	{
		// test ip address of server
		for (i = 0; i < mysocketses; ++i)
		{
			if (runp->ai_addr->sa_family == myfamily[i])
			{
				memcpy(&clientaddress[newnode], runp->ai_addr, runp->ai_addrlen);
				break;
			}
		}

		if (i >= mysocketses)
			runp = runp->ai_next;
		else
			break;
	}
	I_freeaddrinfo(ai);
	return newnode;
}
#endif
//static boolean SOCK_GetHolepunchAddr(mysockaddr_t *sin, int family, const char *address, const char *port, boolean test)
static boolean SOCK_GetHolepunchAddr(struct sockaddr_in *sin, const char *address, const char *port, boolean test)
{
	struct my_addrinfo *ai = NULL, *runp, hints;
	int gaie;

	if (!port || !port[0])
		port = DEFAULTPORT;

	memset (&hints, 0x00, sizeof (hints));
	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	gaie = I_getaddrinfo(address, port, &hints, &ai);

	if (gaie != 0)
	{
		I_freeaddrinfo(ai);
		return false;
	}

	runp = ai;

	if (test)
	{
		while (runp != NULL)
		{
			if (sendto(mysockets[0], NULL, 0, 0, runp->ai_addr, runp->ai_addrlen) == 0)
				break;
			runp = runp->ai_next;
		}
	}

#if 0
	if (runp != NULL)
	{
		if (family == AF_INET)
			memcpy(&sin->ip4, runp->ai_addr, runp->ai_addrlen);
		else if (family == AF_INET6)
			memcpy(&sin->ip6, runp->ai_addr, runp->ai_addrlen);
	}
#else
	if (runp != NULL)
		memcpy(sin, runp->ai_addr, runp->ai_addrlen);
#endif

	I_freeaddrinfo(ai);
	return (runp != NULL);
}

//
// static void rendezvous(int size)
// Contacts the server provided by 'cv_tsourdt3rd_servers_holepunchrendezvous'.
//
static void rendezvous(int size)
{
	static mysockaddr_t rzv;
	char *addrs = strdup(cv_tsourdt3rd_servers_holepunchrendezvous.string);
	char *host = strtok(addrs, ":");
	char *port = strtok(NULL,  ":");

	static tic_t refreshtic = (tic_t)-1;
	tic_t tic = I_GetTime();

#if 0
	if (tic != refreshtic)
	{
		if (SOCK_GetHolepunchAddr(&rzv, AF_INET, host, (port ? port : "7777"), false))
		{
			holepunchpacket->type = AF_INET;
			refreshtic = tic;
		}
#ifdef HAVE_IPV6
		else if (SOCK_GetHolepunchAddr(&rzv, AF_INET6, host, (port ? port : "7777"), false))
		{
			holepunchpacket->type = AF_INET6;
			refreshtic = tic;
		}
#endif
		else
		{
			CONS_Alert(CONS_ERROR, "Failed to contact rendezvous server (%s).\n",
					cv_tsourdt3rd_servers_holepunchrendezvous.string);
		}
	}

	if (tic == refreshtic)
	{
		holepunchpacket->magic = hole_punch_magic;
		if (holepunchpacket->type == AF_INET)
			sendto(mysockets[0], doomcom->data, size, 0, &rzv.any, sizeof rzv.ip4);
		else if (holepunchpacket->type == AF_INET6)
			sendto(mysockets[0], doomcom->data, size, 0, &rzv.any, sizeof rzv.ip6);
	}
#else
	if (tic != refreshtic)
	{
		if (SOCK_GetHolepunchAddr(&rzv.ip4, host, (port ? port : "7777"), false))
		{
			refreshtic = tic;
		}
		else
		{
			CONS_Alert(CONS_ERROR, "Failed to contact rendezvous server (%s).\n",
					cv_tsourdt3rd_servers_holepunchrendezvous.string);
		}
	}

	if (tic == refreshtic)
	{
		holepunchpacket->magic = hole_punch_magic;
		sendto(mysockets[0], doomcom->data, size, 0, &rzv.any, sizeof rzv.ip4);
	}
#endif

	free(addrs);
}

//
// void TSoURDt3rd_SOCK_OpenSockets(void)
// Opens socket data, allowing for certain routines to be stored in the sockets.
//
#if 0
static void SOCK_RequestHolePunch(INT32 node)
{
	mysockaddr_t *addr = &clientaddress[node];
	const char *straddr = SOCK_AddrToStr(addr);

	if (addr->any.sa_family == AF_INET)
	{
		holepunchpacket->ip4.sin_addr.s_addr = addr->ip4.sin_addr.s_addr;
		holepunchpacket->ip4.sin_port = addr->ip4.sin_port;
	}
#ifdef HAVE_IPV6
	else if (addr->any.sa_family == AF_INET6)
	{
		memcpy(holepunchpacket->ip6.sin6_addr.s6_addr, addr->ip6.sin6_addr.s6_addr, sizeof(addr->ip6.sin6_addr.s6_addr));
		//holepunchpacket->ip6.sin6_addr.s6_addr = addr->ip6.sin6_addr.s6_addr;
		holepunchpacket->ip6.sin6_port = addr->ip6.sin6_port;
	}
#endif
	else
	{
		CONS_Debug(DBG_NETPLAY, "address family check failed, can't hole punch node %s\n", straddr);
		return;
	}
	holepunchpacket->family = addr->any.sa_family;

	CONS_Debug(DBG_NETPLAY, "requesting hole punch to node %s\n", straddr);
	rendezvous(10);
}
#else
static void SOCK_RequestHolePunch(INT32 node)
{
	mysockaddr_t * addr = &clientaddress[node];
	holepunchpacket->addr = addr->ip4.sin_addr.s_addr;
	holepunchpacket->port = addr->ip4.sin_port;
	CONS_Debug(DBG_NETPLAY, "requesting hole punch to node %s\n", SOCK_AddrToStr(addr));
	rendezvous(10);
}
#endif

static void SOCK_RegisterHolePunch(void)
{
	CONS_Debug(DBG_NETPLAY, "registering hole punch to nodes...\n");
	rendezvous(4);
}

void TSoURDt3rd_SOCK_OpenSockets(void)
{
	I_NetRequestHolePunch = SOCK_RequestHolePunch;
	I_NetRegisterHolePunch = SOCK_RegisterHolePunch;
}
