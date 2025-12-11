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
/// \file  smkg-p_pads.c
/// \brief TSoURDt3rd's controller routines

#include "smkg-p_pads.h"

#include "../smkg-cvars.h"
#include "../smkg-i_sys.h"
#include "../menus/smkg-m_sys.h" // TSoURDt3rd_M_GetCvPlayerColor() //

#include "../../g_game.h"
#include "../../p_local.h" // P_IsLocalPlayer()
#include "../../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

TSoURDt3rd_ControllerInfo tsourdt3rd_controllers[TSOURDT3RD_NUM_GAMEPADS];

consvar_t *tsourdt3rd_joystick_index[TSOURDT3RD_NUM_GAMEPADS] = {
	&cv_usejoystick,
	&cv_usejoystick2
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// INT16 TSoURDt3rd_P_Pads_GetPadIndex(player_t *player)
//
// Returns a player's gamepad index, even if it's disabled.
// Gamepad indexes correspond to the local player index.
//
INT16 TSoURDt3rd_P_Pads_GetPadIndex(player_t *player)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	player_t *cmp_player = NULL;

	if (player == NULL || !Playing())
		return (player - players);

	for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
	{
		switch (i)
		{
			case 0: // It's our initial player!
				cmp_player = &players[consoleplayer];
				break;
			case 1: // It's our secondary player!
				if (splitscreen)
					cmp_player = &players[secondarydisplayplayer];
				if (playeringame[1] && players[1].bot == BOT_2PHUMAN)
					cmp_player = &players[1];
				break;
			default:
				break;
		}
		if (player == cmp_player)
			return i;
	}
	return -1;
#endif
}

void TSoURDt3rd_P_Pads_SetIndicatorColor(INT32 player, UINT8 red, UINT8 green, UINT8 blue)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	INT32 device_id = TSoURDt3rd_P_Pads_GetPadIndex(&players[player]);

	if (player == -1)
	{
		// Change the lights of all controllers
		for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
			TSoURDt3rd_I_Pads_SetIndicatorColor(i, red, green, blue);
	}
	else if (device_id >= 0)
	{
		// Change the lights of one controller
		TSoURDt3rd_I_Pads_SetIndicatorColor(device_id, red, green, blue);
	}
#endif
}

void TSoURDt3rd_P_Pads_SetIndicatorToPlayerColor(INT32 player)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	INT32 device_id = TSoURDt3rd_P_Pads_GetPadIndex(&players[player]);
	UINT16 skincolor = TSoURDt3rd_M_GetCvPlayerColor(player);
	byteColor_t byte_color = V_GetColor(skincolors[skincolor].ramp[8]).s;

	if (player == -1)
	{
		// Change the lights of all controllers
		for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
			TSoURDt3rd_I_Pads_SetIndicatorColor(i, byte_color.red, byte_color.green, byte_color.blue);
	}
	else if (device_id >= 0)
	{
		// Change the lights of one controller
		TSoURDt3rd_I_Pads_SetIndicatorColor(device_id, byte_color.red, byte_color.green, byte_color.blue);
	}
#endif
}

void TSoURDt3rd_P_Pads_PlayerDeviceRumble(player_t *player, fixed_t low_strength, fixed_t high_strength, tic_t duration_tics)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	INT32 device_id = TSoURDt3rd_P_Pads_GetPadIndex(player);

	if (low_strength == 0 || high_strength == 0)
		return;

	if (player == NULL)
	{
		// Rumble every gamepad
		for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
			TSoURDt3rd_I_Pads_Rumble(i, low_strength, high_strength, duration_tics);
	}
	else if (device_id >= 0)
	{
		// Rumble a specific gamepad
		TSoURDt3rd_I_Pads_Rumble(device_id, low_strength, high_strength, duration_tics);
	}
#endif
}

void TSoURDt3rd_P_Pads_PlayerDeviceRumbleTriggers(player_t *player, fixed_t left_strength, fixed_t right_strength, tic_t duration_tics)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	INT32 device_id = TSoURDt3rd_P_Pads_GetPadIndex(player);

	if (left_strength == 0 || right_strength == 0)
		return;

	if (player == NULL)
	{
		// Rumble every gamepad's trigger
		for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
			TSoURDt3rd_I_Pads_RumbleTriggers(i, left_strength, right_strength, duration_tics);
	}
	else if (device_id >= 0)
	{
		// Rumble a specific gamepad's trigger
		TSoURDt3rd_I_Pads_RumbleTriggers(device_id, left_strength, right_strength, duration_tics);
	}
#endif
}

void TSoURDt3rd_P_Pads_PauseDeviceRumble(player_t *player, boolean rumbling_paused, boolean trigger_rumbling_paused)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	INT32 device_id = TSoURDt3rd_P_Pads_GetPadIndex(player);

	if (player == NULL)
	{
		// Pause the rumbling of every gamepad
		for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
		{
			TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[i];
			controller_data->rumble.paused = rumbling_paused;
			controller_data->trigger_rumble.paused = trigger_rumbling_paused;
			TSoURDt3rd_I_Pads_Rumble(i, 0, 0, 0);
			TSoURDt3rd_I_Pads_RumbleTriggers(i, 0, 0, 0);
		}
	}
	else if (device_id >= 0)
	{
		// Pause the rumbling of a specific gamepad
		TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[device_id];
		controller_data->rumble.paused = rumbling_paused;
		controller_data->trigger_rumble.paused = trigger_rumbling_paused;
		TSoURDt3rd_I_Pads_Rumble(device_id, 0, 0, 0);
		TSoURDt3rd_I_Pads_RumbleTriggers(device_id, 0, 0, 0);
	}
#endif
}

void TSoURDt3rd_P_Pads_ResetDeviceRumble(INT32 player)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)player;
#else
	INT32 device_id = TSoURDt3rd_P_Pads_GetPadIndex(&players[player]);

	if (player == -1)
	{
		// Pause the rumbling of every gamepad
		for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
		{
			TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[i];
			controller_data->rumble.paused = false;
			controller_data->trigger_rumble.paused = false;
			TSoURDt3rd_I_Pads_Rumble(i, 0, 0, 0);
			TSoURDt3rd_I_Pads_RumbleTriggers(i, 0, 0, 0);
		}
	}
	else if (device_id >= 0)
	{
		// Pause the rumbling of a specific gamepad
		TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[device_id];
		controller_data->rumble.paused = false;
		controller_data->trigger_rumble.paused = false;
		TSoURDt3rd_I_Pads_Rumble(device_id, 0, 0, 0);
		TSoURDt3rd_I_Pads_RumbleTriggers(device_id, 0, 0, 0);
	}
#endif
}

void TSoURDt3rd_P_Pads_PadRumbleThink(mobj_t *origin, mobj_t *target)
{
#ifndef TSOURDT3RD_PADS_ENABLED
	(void)origin;
	(void)target;
#else
	player_t *player;

	fixed_t low = 0;
	fixed_t high = 0;
	tic_t duration_tics = 0;

	UINT16 sneaker_vibration;
	static boolean used_ability;

	if (demoplayback)
		return;

	if (origin == NULL || origin->player == NULL)
		return;
	player = origin->player;

	// World Effects //
	if (quake.time)
	{
		low += quake.x;
		high += quake.y;
		duration_tics = quake.time;
	}

	// Player Effects //
	if (P_IsLocalPlayer(player) && !player->exiting)
	{
		sneaker_vibration = player->powers[pw_sneakers];

		if (sneaker_vibration > 0)
		{
			if (sneaker_vibration > 30)
				sneaker_vibration = 30;
			low = high += ((FRACUNIT / 3) * sneaker_vibration);
			duration_tics = player->powers[pw_sneakers];
		}

		if (P_PlayerInPain(player))
		{
			if (player->powers[pw_super])
				low = high += (FRACUNIT / 3);
			else
				low = high += (FRACUNIT / 5);
			duration_tics = (TICRATE / 3);
		}
		else if (origin->health <= 0 && player->deadtimer <= 0)
		{
			if (player->powers[pw_super])
				low = high += (FRACUNIT / 2);
			else
				low = high += (FRACUNIT / 4);
			duration_tics = TICRATE;
		}

		if (origin->eflags & MFE_JUSTHITFLOOR)
		{
			if ((player->pflags & PF_GLIDING) || (player->pflags & PF_BOUNCING) || (player->powers[pw_strong] & STR_TWINSPIN))
			{
				low = high += (FRACUNIT / 4);
				duration_tics = (TICRATE / 4);
			}
			else if (player->pflags & PF_DRILLING)
			{
				low = high += (FRACUNIT / 5);
				duration_tics = (TICRATE / 7);
			}
		}
		else if (P_IsObjectOnGround(origin) && !(origin->eflags & MFE_ONGROUND))
		{
			if (player->pflags & PF_DRILLING)
			{
				low = high += (FRACUNIT / 6);
				duration_tics = 0;
			}
		}

		if ((player->pflags & PF_THOKKED) && !used_ability)
		{
			switch (player->charability)
			{
				case CA_THOK:
				case CA_HOMINGTHOK:
				case CA_JUMPTHOK:
					low = high += (FRACUNIT / 3);
					break;
				case CA_DOUBLEJUMP:
					low = high += (FRACUNIT / 7);
					break;
				case CA_TELEKINESIS:
					low = high += (FRACUNIT / 6);
					break;
				default:
					break;
			}
			duration_tics = (TICRATE / 5);
			used_ability = true;
		}
		else if (!(player->pflags & PF_THOKKED))
			used_ability = false;

		if (target)
		{
			if ((target->flags & MF_ENEMY) || (target->flags & MF_BOSS) || (target->flags & MF_MONITOR))
			{
				low = high += (FRACUNIT / 5);
				duration_tics = (TICRATE / 5);
			}
			if (target->flags & MF_SPECIAL)
			{
				low = high += (FRACUNIT / 7);
				duration_tics = (TICRATE / 4);
			}
		}
	}

	// Rumble the pad! //
	TSoURDt3rd_P_Pads_PlayerDeviceRumble(player, low, high, duration_tics);
#endif
}
