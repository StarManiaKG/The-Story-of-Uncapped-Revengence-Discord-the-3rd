// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_pads.h
/// \brief TSoURDt3rd's controller routines, globalized

#ifndef __SMKG_P_PADS__
#define __SMKG_P_PADS__

#include "../../doomtype.h"
#include "../../d_player.h"
#include "../../command.h"

#ifdef HAVE_SDL
#include "SDL.h"
#include "../../sdl/sdlmain.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_NUM_GAMEPADS 2

typedef struct TSoURDt3rd_ControllerInfo_s
{
#ifdef HAVE_SDL
	// SDL
	SDL_Joystick *joy_device;
	SDL_GameController *game_device;
#endif
	// Main
	boolean active;
	INT16 id;
	INT16 real_id;
	char *name;
	// Rumbling
	struct
	{
		boolean supported;
		UINT16 large_magnitude;
		UINT16 small_magnitude;
		tic_t duration;
		boolean paused;
	} rumble;
	struct
	{
		boolean supported;
		UINT16 left_magnitude;
		UINT16 right_magnitude;
		tic_t duration;
		boolean paused;
	} trigger_rumble;
} TSoURDt3rd_ControllerInfo;
extern TSoURDt3rd_ControllerInfo tsourdt3rd_controllers[TSOURDT3RD_NUM_GAMEPADS];

extern consvar_t *tsourdt3rd_joystick_index[TSOURDT3RD_NUM_GAMEPADS];

// ------------------------ //
//        Functions
// ------------------------ //

INT16 TSoURDt3rd_P_Pads_GetPadIndex(player_t *player);

void TSoURDt3rd_P_Pads_SetIndicatorColor(INT32 player, UINT8 red, UINT8 green, UINT8 blue);
void TSoURDt3rd_P_Pads_SetIndicatorToPlayerColor(INT32 player);

void TSoURDt3rd_P_Pads_PlayerDeviceRumble(player_t *player, fixed_t low_strength, fixed_t high_strength, tic_t duration_tics);
void TSoURDt3rd_P_Pads_PlayerDeviceRumbleTriggers(player_t *player, fixed_t left_strength, fixed_t right_strength, tic_t duration_tics);
void TSoURDt3rd_P_Pads_PauseDeviceRumble(player_t *player, boolean rumbling_paused, boolean trigger_rumbling_paused);
void TSoURDt3rd_P_Pads_ResetDeviceRumble(INT32 player);

void TSoURDt3rd_P_Pads_PadRumbleThink(mobj_t *origin, mobj_t *target);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_P_PADS__
