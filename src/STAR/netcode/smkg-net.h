// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
// Copyright (C) 2024 by Kart Krew.
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

#include "../../i_net.h"

// ------------------------ //
//        Variables
// ------------------------ //

// HOLEPUNCHING PORTED FROM DR. ROBOTNIK'S RING RACERS
typedef struct
{
	INT32 magic;
	INT32 addr;
	INT16 port;
} ATTRPACK holepunch_t;

extern holepunch_t *holepunchpacket;

// ------------------------ //
//        Functions
// ------------------------ //

/**	\brief send a hole punching request
*/
extern void (*I_NetRequestHolePunch)(INT32 node);

/**	\brief register this machine on the hole punching server
*/
extern void (*I_NetRegisterHolePunch)(void);

void TSoURDt3rd_MovePlayerStructure(INT32 node, INT32 newplayernode, INT32 prevnode);
void TSoURDt3rd_HandleCustomPackets(INT32 node);

void TSoURDt3rd_D_CheckNetgame(doomcom_t *doomcom_p);
void TSoURDt3rd_D_AskForHolepunch(INT32 node);
void TSoURDt3rd_D_RenewHolePunch(void);

boolean TSoURDt3rd_SOCK_Get(doomcom_t *doomcom_p, ssize_t c, void *addresstable, void *sockets);
void TSoURDt3rd_SOCK_OpenSocket(void);

#endif // __SMKG_NET__
