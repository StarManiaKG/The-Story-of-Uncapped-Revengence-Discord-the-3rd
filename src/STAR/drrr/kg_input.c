// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  kg_input.c
/// \brief handle mouse/keyboard/joystick inputs,
///        maps inputs to game controls (forward, spin, jump...)

#include "kg_input.h"

#include "../smkg-cvars.h"
#include "../smkg-i_sys.h"
#include "../menus/smkg_m_draw.h"

#include "../../g_game.h"
#include "../../v_video.h"
#include "../../z_zone.h"

void G_SetPlayerGamepadIndicatorToPlayerColor(INT32 player)
{
	INT32 device;
	UINT16 skincolor;
	byteColor_t byte_color;

	I_Assert(player >= 0 && player < MAXPLAYERS);

	device = ((player == consoleplayer || player == 0) ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device <= 0)
	{
		return;
	}

	skincolor = TSoURDt3rd_M_GetCvPlayerColor(player);

	byte_color = V_GetColor(skincolors[skincolor].ramp[8]).s;

	TSoURDt3rd_Pads_I_SetGamepadIndicatorColor(device, byte_color.red, byte_color.green, byte_color.blue);
}

void TSoURDt3rd_Pads_G_PlayerDeviceRumble(player_t *player, fixed_t low_strength, fixed_t high_strength)
{
	INT32 device_id;

	// Rumble every gamepad
	if (player == NULL)
	{
		if (cv_tsourdt3rd_ctrl_drrr_rumble[0].value && cv_usejoystick.value < 1)
			TSoURDt3rd_Pads_I_GamepadRumble(cv_usejoystick.value, low_strength, high_strength);
		if (cv_tsourdt3rd_ctrl_drrr_rumble[1].value && cv_usejoystick2.value < 1)
			TSoURDt3rd_Pads_I_GamepadRumble(cv_usejoystick2.value, low_strength, high_strength);
		return;
	}

	// Rumble a specific gamepad
	if (cv_tsourdt3rd_ctrl_drrr_rumble[player - players].value == 0)
	{
		return;
	}

	if (!P_IsLocalPlayer(player))
	{
		return;
	}

	device_id = ((consoleplayer == player - players) ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device_id < 1)
	{
		return;
	}

	TSoURDt3rd_Pads_I_GamepadRumble(device_id, low_strength, high_strength);
}

void TSoURDt3rd_Pads_G_PlayerDeviceRumbleTriggers(player_t *player, fixed_t left_strength, fixed_t right_strength)
{
	INT32 device_id;

	// Rumble every gamepad
	if (player == NULL)
	{
		if (cv_tsourdt3rd_ctrl_drrr_rumble[0].value && cv_usejoystick.value < 1)
			TSoURDt3rd_Pads_I_GamepadRumbleTriggers(cv_usejoystick.value, left_strength, right_strength);
		if (cv_tsourdt3rd_ctrl_drrr_rumble[1].value && cv_usejoystick2.value < 1)
			TSoURDt3rd_Pads_I_GamepadRumbleTriggers(cv_usejoystick2.value, left_strength, right_strength);
		return;
	}

	// Rumble a specific gamepad
	if (cv_tsourdt3rd_ctrl_drrr_rumble[(player - players)].value == 0)
	{
		return;
	}

	if (!P_IsLocalPlayer(player))
	{
		return;
	}

	device_id = ((consoleplayer == player - players) ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device_id < 1)
	{
		return;
	}

	TSoURDt3rd_Pads_I_GamepadRumbleTriggers(device_id, left_strength, right_strength);
}

void G_ResetPlayerDeviceRumble(INT32 player)
{
	INT32 device_id;

	device_id = (player == consoleplayer ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device_id < 1)
	{
		return;
	}

	TSoURDt3rd_Pads_I_GamepadRumble(device_id, 0, 0);
	TSoURDt3rd_Pads_I_GamepadRumbleTriggers(device_id, 0, 0);
}

void G_ResetAllDeviceRumbles(void)
{
	TSoURDt3rd_Pads_I_GamepadRumble(cv_usejoystick.value, 0, 0);
	TSoURDt3rd_Pads_I_GamepadRumbleTriggers(cv_usejoystick.value, 0, 0);

	TSoURDt3rd_Pads_I_GamepadRumble(cv_usejoystick2.value, 0, 0);
	TSoURDt3rd_Pads_I_GamepadRumbleTriggers(cv_usejoystick2.value, 0, 0);
}
