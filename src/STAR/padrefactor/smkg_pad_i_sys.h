// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_pad_i_sys.h
/// \brief Gamepad refactor system specific interface stuff.

#ifndef __SMKG_PAD_I_SYS__
#define __SMKG_PAD_I_SYS__

#include "../../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

extern INT32 window_x;
extern INT32 window_y;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_I_CursedWindowMovement(int xd, int yd);

void TSoURDt3rd_GamepadR_I_SetGamepadPlayerIndex(INT32 device_id, INT32 index);
void TSoURDt3rd_GamepadR_I_SetGamepadIndicatorColor(INT32 device_id, UINT8 red, UINT8 green, UINT8 blue);
void TSoURDt3rd_GamepadR_I_GamepadRumble(INT32 device_id, UINT16 low_strength, UINT16 high_strength);
void TSoURDt3rd_GamepadR_I_GamepadRumbleTriggers(INT32 device_id, UINT16 left_strength, UINT16 right_strength);

#ifdef HAVE_SDL
#include "SDL.h"
#include "../../sdl/sdlmain.h"

void TSoURDt3rd_I_GetEvent(SDL_Event *evt);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_PAD_I_SYS__
