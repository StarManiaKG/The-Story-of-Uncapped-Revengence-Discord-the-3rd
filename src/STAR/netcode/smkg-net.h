// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
// Copyright (C) 2024-2025 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-net.h
/// \brief TSoURDt3rd's system specific network interface junk

#ifndef __SMKG_NET__
#define __SMKG_NET__

#include "../../netcode/i_net.h"

#ifdef _WIN32
	#define USE_WINSOCK
	#if defined (_WIN64) || defined (HAVE_IPV6)
		#define USE_WINSOCK2
	#else //_WIN64/HAVE_IPV6
		#define USE_WINSOCK1
	#endif
#endif //WIN32 OS

#ifndef USE_WINSOCK1
	#ifndef USE_WINSOCK
		#include <netdb.h>
		#include <sys/socket.h>
	#endif
#endif //normal BSD API

/**	\brief send a hole punching request
*/
extern void (*I_NetRequestHolePunch)(INT32 node);

/**	\brief register this machine on the hole punching server
*/
extern void (*I_NetRegisterHolePunch)(void);

//#define PTRPACKET ptrdiff_t
#define PTRPACKET ssize_t
boolean hole_punch(PTRPACKET c);

void TSoURDt3rd_MovePlayerStructure(INT32 node, INT32 newplayernode, INT32 prevnode);
void TSoURDt3rd_HandleCustomPackets(INT32 node);

void TSoURDt3rd_D_AskForHolepunch(INT32 node);
void TSoURDt3rd_D_RenewHolePunch(void);

void TSoURDt3rd_SOCK_OpenSockets(void);

#endif // __SMKG_NET__
