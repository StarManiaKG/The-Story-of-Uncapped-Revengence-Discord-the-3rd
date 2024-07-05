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
/// \file  ki_system.h
/// \brief System specific interface stuff.

#ifndef __KI_SYSTEM__
#define __KI_SYSTEM__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_SDL
#include "../../sdl/sdlmain.h"
void DRRR_I_GetEvent(SDL_Event evt);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KI_SYSTEM__
