// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_menu.h
/// \brief Discord Rich Presence menus, globalized

#ifndef __DISCORD_MENU__
#define __DISCORD_MENU__

#include "../m_menu.h"
#include "../p_tick.h"

#ifdef HAVE_DISCORDSUPPORT

// ------------------------ //
// 		   Structs
// ------------------------ //

extern struct discordrequestmenu_s {
	tic_t ticker;
	tic_t confirmDelay;
	tic_t confirmLength;
	boolean confirmAccept;
	boolean removeRequest;
} discordrequestmenu;

// ------------------------ //
// 		  Variables
// ------------------------ //

// =====
// MENUS
// =====

extern menu_t OP_DiscordOptionsDef;
extern menu_t MISC_DiscordRequestsDef;

// ------------------------ //
//        Functions
// ------------------------ //

void M_DiscordOptions(INT32 choice);
void M_DiscordOptionsTicker(void);

void M_DiscordRequests(INT32 choice);
void M_DiscordRequestTick(void);

#endif // HAVE_DISCORDSUPPORT
#endif // __DISCORD_MENU__
