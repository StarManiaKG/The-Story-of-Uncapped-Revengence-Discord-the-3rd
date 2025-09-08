// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-i_sys.h
/// \brief TSoURDt3rd system specific interface stuff.

#ifndef __SMKG_I_SYS__
#define __SMKG_I_SYS__

#include "../doomstat.h"

#ifdef HAVE_SDL
#include "SDL.h"
#include "../sdl/sdlmain.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_GAMEPAD_INIT_FLAGS (SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER)

extern INT32 window_x;
extern INT32 window_y;

// ------------------------ //
//        Functions
// ------------------------ //

#ifdef HAVE_SDL
void TSoURDt3rd_I_Pads_InitControllers(void);

void TSoURDt3rd_I_Pads_SetIndicatorColor(INT32 device_id, UINT8 red, UINT8 green, UINT8 blue);
void TSoURDt3rd_I_Pads_Rumble(INT32 device_id, fixed_t low_strength, fixed_t high_strength, tic_t duration_tics);
void TSoURDt3rd_I_Pads_RumbleTriggers(INT32 device_id, fixed_t left_strength, fixed_t right_strength, tic_t duration_tics);

const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped);

void TSoURDt3rd_I_CursedWindowMovement(int xd, int yd);
void TSoURDt3rd_I_QuakeWindow(int xd, int yd);

void TSoURDt3rd_I_GetEvent(SDL_Event *evt);
void TSoURDt3rd_I_ShutdownSystem(void);
#endif

void TSoURDt3rd_I_FinishUpdate(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_I_SYS__
