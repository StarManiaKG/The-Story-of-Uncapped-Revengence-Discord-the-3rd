// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief Unique TSoURDt3rd device routines for SDL

#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include "../smkg-i_sys.h"

#include "../smkg-cvars.h"
#include "../smkg-st_hud.h"
#include "../star_vars.h" // STAR_CONS_Printf() //
#include "../core/smkg-p_pads.h"
#include "../core/smkg-s_jukebox.h"
#include "../menus/smkg-m_sys.h"

#include "../../i_system.h"
#include "../../p_local.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

// platform independant window location data
INT32 window_x;
INT32 window_y;

// ------------------------ //
//        Function
// ------------------------ //

#ifdef HAVE_SDL

// Returns the name of a controller from its index
static char *Impl_TSoURDt3rd_Pads_GetName(INT32 joyindex)
{
	const char *gamec_tempname = SDL_GameControllerNameForIndex(joyindex-1);

	if (SDL_WasInit(TSOURDT3RD_GAMEPAD_INIT_FLAGS) != TSOURDT3RD_GAMEPAD_INIT_FLAGS)
		return NULL;
	if (gamec_tempname)
		return strdup(gamec_tempname);
	return strdup(I_GetJoyName(joyindex));
}

// ====================================
// EVENTS
// ====================================

static void Impl_TSoURDt3rd_Pads_Added(void)
{
	// The game is always interested in controller events, even if they aren't internally assigned to a player.
	// Thus, we *always* open SDL controllers as they become available, to begin receiving their events.

	for (UINT8 user = 0; user < TSOURDT3RD_NUM_GAMEPADS; user++)
	{
		TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[user];
		INT32 device_id = (tsourdt3rd_joystick_index[user]->value - 1);

		boolean controller_rumble_supported, controller_trigger_rumble_supported;
		boolean joystick_rumble_supported, joystick_trigger_rumble_supported;

		SDL_GameController *controller = SDL_GameControllerOpen(device_id);
		SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);

		if (controller == NULL)// || joystick == NULL)
		{
			// ...Aw, dang it.
			continue;
		}
		if (controller_data->game_device != NULL || controller_data->joy_device != NULL || controller_data->active)
		{
			continue;
		}

		controller_rumble_supported = (SDL_GameControllerHasRumble(controller) == SDL_TRUE);
		controller_trigger_rumble_supported = (SDL_GameControllerHasRumbleTriggers(controller) == SDL_TRUE);
		joystick_rumble_supported = (SDL_JoystickHasRumble(joystick) == SDL_TRUE);
		joystick_trigger_rumble_supported = (SDL_JoystickHasRumbleTriggers(joystick) == SDL_TRUE);

		controller_data->active = true;
		controller_data->game_device = controller;
		controller_data->joy_device = joystick;
		controller_data->id = device_id;
		controller_data->real_id = (device_id + 1);
		controller_data->name = Impl_TSoURDt3rd_Pads_GetName(controller_data->real_id);
		controller_data->rumble.supported = (controller_rumble_supported || joystick_rumble_supported);
		controller_data->trigger_rumble.supported = (controller_trigger_rumble_supported || joystick_trigger_rumble_supported);

		if (controller_data->game_device)
		{
			SDL_GameControllerSetPlayerIndex(controller_data->game_device, user);
		}
		if (controller_data->joy_device)
		{
			SDL_JoystickSetPlayerIndex(controller_data->joy_device, user);
		}

		TSoURDt3rd_P_Pads_ResetDeviceRumble(user);
		TSoURDt3rd_P_Pads_SetIndicatorToPlayerColor(user);

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE,
			"Gamepad device (%s) has been added for Player %d.\n",
			controller_data->name,
			controller_data->real_id
		);
	}
}

static void Impl_TSoURDt3rd_Pads_Removed(void)
{
	for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
	{
		TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[i];

		if (controller_data->game_device == NULL || controller_data->joy_device == NULL || !controller_data->active)
		{
			continue;
		}
		if (SDL_GameControllerGetAttached(controller_data->game_device))
		{
			// The controller is still connected, dude!
			continue;
		}

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE,
			"Gamepad device (%s) has been removed for Player %d.\n",
			controller_data->name,
			controller_data->real_id
		);

		SDL_GameControllerOpen(controller_data->id);
		TSoURDt3rd_I_Pads_SetIndicatorColor(i, 0, 0, 255);
		TSoURDt3rd_P_Pads_ResetDeviceRumble(i);
		SDL_GameControllerClose(controller_data->game_device);

		controller_data->active = false;
		controller_data->game_device = NULL;
		controller_data->joy_device = NULL;
		controller_data->id = -1;
		controller_data->real_id = 0;

		free(controller_data->name);
		controller_data->name = NULL;
	}
}

//
// void TSoURDt3rd_I_GetEvent(SDL_Event *evt)
// Gets SDL events for TSoURDt3rd.
//
void TSoURDt3rd_I_GetEvent(SDL_Event *evt)
{
	switch (evt->type)
	{
		case SDL_JOYDEVICEADDED:
		case SDL_CONTROLLERDEVICEADDED:
			Impl_TSoURDt3rd_Pads_Added();
			break;
		case SDL_JOYDEVICEREMOVED:
		case SDL_CONTROLLERDEVICEREMOVED:
			Impl_TSoURDt3rd_Pads_Removed();
			break;
	}
}

#endif // HAVE_SDL

//
// void TSoURDt3rd_I_FinishUpdate(void)
// Updates the contents of the screen.
//
void TSoURDt3rd_I_FinishUpdate(void)
{
#ifdef HAVE_DISCORDSUPPORT
	if (discordRequestList != NULL)
		TSoURDt3rd_ST_AskToJoinEnvelope();
#endif

	TSoURDt3rd_Jukebox_ST_drawJukebox();
}
