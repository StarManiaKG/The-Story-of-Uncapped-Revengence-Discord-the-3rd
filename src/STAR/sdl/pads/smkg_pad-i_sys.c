// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
//
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Changes by Graue <graue@oceanbase.org> are in the public domain.
//
//-----------------------------------------------------------------------------
/// \file  smkg_pad_i_sys.c
/// \brief TSoURDt3rd gamepad system stuff for SDL

#ifdef _MSC_VER
#pragma warning(disable : 4214 4244)
#endif

#ifdef HAVE_SDL
#include "SDL.h"

#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include "../../smkg-i_sys.h"

#include "../../drrr/kg_input.h"

#include "../../../sdl/sdlmain.h"
#include "../../../d_event.h"

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_Pads_I_SetGamepadPlayerIndex(INT32 device_id, INT32 player)
{
#if !(SDL_VERSION_ATLEAST(2,0,12))
	(void)device_id;
	(void)player;
	return;
#endif

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher
	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerSetPlayerIndex(controller, player);
}

void TSoURDt3rd_Pads_I_SetGamepadIndicatorColor(INT32 device_id, UINT8 red, UINT8 green, UINT8 blue)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)red;
	(void)green;
	(void)blue;
	return;
#endif

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerSetLED(controller, red, green, blue);
}

void TSoURDt3rd_Pads_I_GamepadRumble(INT32 device_id, UINT16 low_strength, UINT16 high_strength)
{
#if !(SDL_VERSION_ATLEAST(2,0,9))
	(void)device_id;
	(void)low_strength;
	(void)high_strength;
	return;
#endif

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerRumble(controller, low_strength, high_strength, 0);
}

void TSoURDt3rd_Pads_I_GamepadRumbleTriggers(INT32 device_id, UINT16 left_strength, UINT16 right_strength)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)left_strength;
	(void)right_strength;
	return;
#endif

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerRumbleTriggers(controller, left_strength, right_strength, 0);
}

#endif // HAVE_SDL
