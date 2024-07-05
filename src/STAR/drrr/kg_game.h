// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  kg_game.h
/// \brief Game loop, events handling.

#ifndef __KG_GAME__
#define __KG_GAME__

#include "../../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

INT32 G_PlayerInputAnalog(UINT8 p, INT32 gc, UINT8 menuPlayers);
boolean G_PlayerInputDown(UINT8 p, INT32 gc, UINT8 menuPlayers);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KG_GAME__
