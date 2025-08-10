// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-i_protocol.c
/// \brief TSoURDt3rd's extended network protocol data

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef __GNUC__
#include <unistd.h>
#endif

#ifndef NO_IPV6
	#define HAVE_IPV6
#endif

#ifdef _WIN32
	#define USE_WINSOCK
	#if defined (_WIN64) || defined (HAVE_IPV6)
		#define USE_WINSOCK2
	#else //_WIN64/HAVE_IPV6
		#define USE_WINSOCK1
	#endif
#endif //WIN32 OS

#ifdef USE_WINSOCK2
	#include <ws2tcpip.h>
#endif

#ifdef USE_WINSOCK1
	#include <winsock.h>
#else
	#ifndef USE_WINSOCK
		#include <arpa/inet.h>
		#ifdef __APPLE_CC__
			#ifndef _BSD_SOCKLEN_T_
				#define _BSD_SOCKLEN_T_
			#endif //_BSD_SOCKLEN_T_
		#endif //__APPLE_CC__
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <netdb.h>
		#include <sys/ioctl.h>
	#endif //normal BSD API

	#include <errno.h>
	#include <time.h>

	#if defined (__unix__) || defined (__APPLE__) || defined (UNIXCOMMON)
		#include <sys/time.h>
	#endif // UNIXCOMMON
#endif

typedef union
{
	struct sockaddr     any;
	struct sockaddr_in  ip4;
#ifdef HAVE_IPV6
	struct sockaddr_in6 ip6;
#endif
} mysockaddr_t;

#include "drrr-m_swap.h"
#include "smkg-net.h"
#include "../smkg-cvars.h"

#include "../../i_time.h"

#include "../../netcode/d_net.h"
#include "../../netcode/i_addrinfo.h"

#ifdef USE_STUN
#include "../stun/stun.h" // STUN_got_response() //
#endif

// ------------------------ //
//        Variables
// ------------------------ //

#define DEFAULTPORT "5029"

#ifdef USE_WINSOCK
	typedef SOCKET SOCKET_TYPE;
	#define ERRSOCKET (SOCKET_ERROR)
#else
	#if defined (__unix__) || defined (__APPLE__) || defined (__HAIKU__)
		typedef int SOCKET_TYPE;
	#else
		typedef unsigned long SOCKET_TYPE;
	#endif
	#define ERRSOCKET (-1)
#endif

static SOCKET_TYPE mysockets[MAXNETNODES+1] = {ERRSOCKET};
static mysockaddr_t clientaddress[MAXNETNODES+1];

/* See ../doc/Holepunch-Protocol.txt */
void (*I_NetRequestHolePunch)(INT32 node) = NULL;
void (*I_NetRegisterHolePunch)(void) = NULL;

static const INT32 hole_punch_magic = MSBF_LONG (0x52eb11);

// ------------------------ //
//        Functions
// ------------------------ //

static const char *SOCK_AddrToStr(mysockaddr_t *sk)
{
	static char s[64]; // 255.255.255.255:65535 or
	// [ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff]:65535
#ifdef HAVE_NTOP
#ifdef HAVE_IPV6
	int v6 = (sk->any.sa_family == AF_INET6);
#else
	int v6 = 0;
#endif
	void *addr;

	if(sk->any.sa_family == AF_INET)
		addr = &sk->ip4.sin_addr;
#ifdef HAVE_IPV6
	else if(sk->any.sa_family == AF_INET6)
		addr = &sk->ip6.sin6_addr;
#endif
	else
		addr = NULL;

	if(addr == NULL)
		sprintf(s, "No address");
	else if(inet_ntop(sk->any.sa_family, addr, &s[v6], sizeof (s) - v6) == NULL)
		sprintf(s, "Unknown family type, error #%u", errno);
#ifdef HAVE_IPV6
	else if(sk->any.sa_family == AF_INET6)
	{
		s[0] = '[';
		strcat(s, "]");

		if (sk->ip6.sin6_port != 0)
			strcat(s, va(":%d", ntohs(sk->ip6.sin6_port)));
	}
#endif
	else if(sk->any.sa_family == AF_INET  && sk->ip4.sin_port  != 0)
		strcat(s, va(":%d", ntohs(sk->ip4.sin_port)));

#else
	if (sk->any.sa_family == AF_INET)
	{
		strcpy(s, inet_ntoa(sk->ip4.sin_addr));
		if (sk->ip4.sin_port != 0) strcat(s, va(":%d", ntohs(sk->ip4.sin_port)));
	}
	else
		sprintf(s, "Unknown type");
#endif
	return s;
}

/* not one of the reserved "local" addresses */
// https://github.com/jameds/holepunch/blob/master/holepunch.c#L75
static boolean SOCK_IsExternalAddress(UINT32 p)
{
	UINT8 a = (p & 255);
	UINT8 b = ((p >> 8) & 255);

	if (p == (UINT32)~0)/* 255.255.255.255 */
		return 0;

	switch (a)
	{
		case 0:
		case 10:
		case 127:
			return false;
		case 172:
			return (b & ~15) != 16;/* 16 - 31 */
		case 192:
			return b != 168;
		default:
			return true;
	}
}

//
// static boolean hole_punch(ssize_t c)
// Sends out holepunching data needed in order to send a request.
//
static boolean hole_punch(ssize_t c)
{
	if (c == 10 && holepunchpacket->magic == hole_punch_magic
		&& SOCK_IsExternalAddress(ntohl(holepunchpacket->addr)))
	{
		mysockaddr_t addr;
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
}

//
// static boolean SOCK_GetHolepunchAddr(struct sockaddr_in *sin, const char *address, const char *port, boolean test)
//
// A routine made to resemble SOCK_NetMakeNodewPort from Dr.Robotnik's Ring Racers,
// 	in order to get holepunching addresses.
//
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

	if (runp != NULL)
		memcpy(sin, runp->ai_addr, runp->ai_addrlen);

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

	free(addrs);
}

//
// boolean TSoURDt3rd_SOCK_Get(doomcom_t *doomcom_p, ssize_t c, void *addresstable, void *sockets)
//
// Extended wrapper for SOCK_Get in i_tpc.c.
// Stops the main function in its tracks if a function here returns true.
//
boolean TSoURDt3rd_SOCK_Get(doomcom_t *doomcom_p, ssize_t c, void *addresstable, void *sockets)
{
	memcpy(clientaddress, (mysockaddr_t *)addresstable, sizeof(addresstable));
	memcpy(mysockets, (SOCKET_TYPE *)sockets, sizeof(sockets));

#if 0
	if (c <= 0)
		return false;
#endif

#ifdef USE_STUN
	if (STUN_got_response(doomcom_p->data, c))
	{
		return true;
	}
#else
	(void)doomcom_p;
#endif

#if 0
	if (hole_punch(c))
	{
		return true;
	}
#endif

	return false;
}

//
// void TSoURDt3rd_SOCK_OpenSockets(void)
// Opens socket data, allowing for certain routines to be stored in the sockets.
//
static void SOCK_RequestHolePunch(INT32 node)
{
	mysockaddr_t * addr = &clientaddress[node];

	holepunchpacket->addr = addr->ip4.sin_addr.s_addr;
	holepunchpacket->port = addr->ip4.sin_port;

	CONS_Debug(DBG_NETPLAY,
			"requesting hole punch to node %s\n", SOCK_AddrToStr(addr));

	//rendezvous(10);
}

static void SOCK_RegisterHolePunch(void)
{
	//rendezvous(4);
}

void TSoURDt3rd_SOCK_OpenSockets(void)
{
	I_NetRequestHolePunch = SOCK_RequestHolePunch;
	I_NetRegisterHolePunch = SOCK_RegisterHolePunch;
}
