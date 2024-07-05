// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
//
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
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
/// \file
/// \brief SRB2 system stuff for SDL

#ifdef HAVE_SDL
#include "SDL.h"

#include "../ki_joy.h"
#include "../../../sdl/sdlmain.h"

void I_SetGamepadPlayerIndex(INT32 device_id, INT32 player)
{
#if !(SDL_VERSION_ATLEAST(2,0,12))
	(void)device_id;
	(void)player;
#else
	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher
	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerSetPlayerIndex(controller, player);
#endif
}

void I_GetGamepadGuid(INT32 device_id, char *out, int out_len)
{
	SDL_GameController *controller;
	SDL_Joystick *joystick;
	SDL_JoystickGUID guid;

	I_Assert(device_id > 0);
	I_Assert(out != NULL);
	I_Assert(out_len > 0);

	if (out_len < 33)
	{
		out[0] = 0;
		return;
	}

	controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		out[0] = 0;
		return;
	}
	joystick = SDL_GameControllerGetJoystick(controller);
	if (joystick == NULL)
	{
		out[0] = 0;
		return;
	}

	guid = SDL_JoystickGetGUID(joystick);
	SDL_JoystickGetGUIDString(guid, out, out_len);
}

void I_GetGamepadName(INT32 device_id, char *out, int out_len)
{
	SDL_GameController *controller;
	const char *name;
	int name_len;

	I_Assert(device_id > 0);
	I_Assert(out != NULL);
	I_Assert(out_len > 0);

	controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		out[0] = 0;
		return;
	}

	name = SDL_GameControllerName(controller);
	name_len = strlen(name) + 1;
	memcpy(out, name, out_len < name_len ? out_len : name_len);
	out[out_len - 1] = 0;
}

void I_GamepadRumble(INT32 device_id, UINT16 low_strength, UINT16 high_strength)
{
#if !(SDL_VERSION_ATLEAST(2,0,9))
	(void)device_id;
	(void)low_strength;
	(void)high_strength;
#else
	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerRumble(controller, low_strength, high_strength, 0);
#endif
}

void I_GamepadRumbleTriggers(INT32 device_id, UINT16 left_strength, UINT16 right_strength)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)left_strength;
	(void)right_strength;
#else
	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_GameController *controller = SDL_GameControllerFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_GameControllerRumbleTriggers(controller, left_strength, right_strength, 0);
#endif
}

#endif
