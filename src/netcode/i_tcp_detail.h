// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2025 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file i_tcp_detail.h

#ifndef i_tcp_detail_h
#define i_tcp_detail_h

#ifdef __cplusplus
extern "C" {
#endif

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

	#if (defined (__unix__) && !defined (MSDOS)) || defined(__APPLE__) || defined (UNIXCOMMON)
		#include <sys/time.h>
	#endif // UNIXCOMMON
#endif

#ifdef USE_WINSOCK
	// some undefined under win32
	#undef errno
	//#define errno WSAGetLastError() //Alam_GBC: this is the correct way, right?
	#define errno h_errno // some very strange things happen when not using h_error?!?
	#ifdef EWOULDBLOCK
	#undef EWOULDBLOCK
	#endif
	#define EWOULDBLOCK WSAEWOULDBLOCK
	#ifdef EMSGSIZE
	#undef EMSGSIZE
	#endif
	#define EMSGSIZE WSAEMSGSIZE
	#ifdef ECONNREFUSED
	#undef ECONNREFUSED
	#endif
	#define ECONNREFUSED WSAECONNREFUSED
	#ifdef ETIMEDOUT
	#undef ETIMEDOUT
	#endif
	#define ETIMEDOUT WSAETIMEDOUT
	#ifdef EHOSTUNREACH
	#undef EHOSTUNREACH
	#endif
	#define EHOSTUNREACH WSAEHOSTUNREACH
	#ifdef ENETUNREACH
	#undef ENETUNREACH
	#endif
	#define ENETUNREACH WSAENETUNREACH
	#ifndef IOC_VENDOR
	#define IOC_VENDOR 0x18000000
	#endif
	#ifndef _WSAIOW
	#define _WSAIOW(x,y) (IOC_IN|(x)|(y))
	#endif
	#ifndef SIO_UDP_CONNRESET
	#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)
	#endif
	#ifndef AI_ADDRCONFIG
	#define AI_ADDRCONFIG 0x00000400
	#endif
	#ifndef STATUS_INVALID_PARAMETER
	#define STATUS_INVALID_PARAMETER 0xC000000D
	#endif
#endif // USE_WINSOCK

#include "../doomdef.h"
#include "../doomtype.h"

#ifdef HAVE_MINIUPNPC
	#include "miniupnpc/miniwget.h"
	#include "miniupnpc/miniupnpc.h"
	#include "miniupnpc/upnpcommands.h"
#endif // HAVE_MINIUPNC

#include "i_tcp.h"
#include "d_net.h"

#define DEFAULTPORT "5029"

// win32 or djgpp
#ifdef USE_WINSOCK
	// winsock stuff (in winsock a socket is not a file)
	#define ioctl ioctlsocket
	#define close closesocket
#endif

#include "i_addrinfo.h"

#ifdef USE_WINSOCK
	typedef SOCKET SOCKET_TYPE;
	#define ERRSOCKET (SOCKET_ERROR)
#else
	#if (defined (__unix__) && !defined (MSDOS)) || defined (__APPLE__) || defined (__HAIKU__)
		typedef int SOCKET_TYPE;
	#else
		typedef unsigned long SOCKET_TYPE;
	#endif
	#define ERRSOCKET (-1)
#endif

#define IPV6_MULTICAST_ADDRESS "ff15::57e1:1a12"

// define socklen_t in Windows if it is not already defined
#ifdef USE_WINSOCK1
	typedef int socklen_t;
#endif

extern SOCKET_TYPE mysockets[MAXNETNODES+1];

typedef union
{
	struct sockaddr     any;
	struct sockaddr_in  ip4;
#ifdef HAVE_IPV6
	struct sockaddr_in6 ip6;
#endif
} mysockaddr_t;

extern mysockaddr_t clientaddress[MAXNETNODES+1];

const char *SOCK_AddrToStr(mysockaddr_t *sk);
mysockaddr_t SOCK_DirectNodeToAddr(UINT8 node);
boolean SOCK_cmpaddr(mysockaddr_t *a, mysockaddr_t *b, UINT8 mask);

//#define PTRPACKET ptrdiff_t
#define PTRPACKET ssize_t

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* i_tcp_detail_h */
