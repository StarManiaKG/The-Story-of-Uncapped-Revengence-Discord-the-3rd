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

#include "k_doomdef.h"
#include "k_cvars.h"
#include "kg_input.h"
#include "k_menu.h"

#include "../padrefactor/smkg_pad_i_sys.h"

#include "../../console.h" // CONS_Ready() //
#include "../../hu_stuff.h" // chat_on //
#include "../../v_video.h"
#include "../../z_zone.h"

#define JOYANALOGS 2 // 2 sets of analog stick axes, with positive and negative each

void G_SetDeviceForPlayer(INT32 player)
{
	INT32 device = (player == consoleplayer ? cv_usejoystick.value : cv_usejoystick2.value);

	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);
	I_Assert(device >= -1);

	if (device < 0)
	{
		TSoURDt3rd_GamepadR_I_SetGamepadPlayerIndex(device, player);
		return;
	}

	TSoURDt3rd_GamepadR_I_SetGamepadPlayerIndex(device, -1);
	TSoURDt3rd_GamepadR_I_GamepadRumble(device, 0, 0);
	TSoURDt3rd_GamepadR_I_GamepadRumbleTriggers(device, 0, 0);
}

void G_SetPlayerGamepadIndicatorToPlayerColor(INT32 player)
{
	INT32 device;
	UINT16 skincolor;
	byteColor_t byte_color;

	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);

	device = (player == consoleplayer ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device <= 0)
	{
		return;
	}

	skincolor = M_GetCvPlayerColor(player);

	byte_color = V_GetColor(skincolors[skincolor].ramp[8]).s;

	TSoURDt3rd_GamepadR_I_SetGamepadIndicatorColor(device, byte_color.red, byte_color.green, byte_color.blue);
}

void G_PlayerDeviceRumble(INT32 player, UINT16 low_strength, UINT16 high_strength)
{
	INT32 device_id;

	if (cv_tsourdt3rd_drrr_rumble[player].value == 0)
	{
		return;
	}

	device_id = (player == consoleplayer ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device_id < 1)
	{
		return;
	}

	TSoURDt3rd_GamepadR_I_GamepadRumble(device_id, low_strength, high_strength);
}

void G_PlayerDeviceRumbleTriggers(INT32 player, UINT16 left_strength, UINT16 right_strength)
{
	INT32 device_id;

	if (cv_tsourdt3rd_drrr_rumble[player].value == 0)
	{
		return;
	}

	device_id = (player == consoleplayer ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device_id < 1)
	{
		return;
	}

	TSoURDt3rd_GamepadR_I_GamepadRumbleTriggers(device_id, left_strength, right_strength);
}

void G_ResetPlayerDeviceRumble(INT32 player)
{
	INT32 device_id;

	device_id = (player == consoleplayer ? cv_usejoystick.value : cv_usejoystick2.value);

	if (device_id < 1)
	{
		return;
	}

	TSoURDt3rd_GamepadR_I_GamepadRumble(device_id, 0, 0);
	TSoURDt3rd_GamepadR_I_GamepadRumbleTriggers(device_id, 0, 0);
}

void G_ResetAllDeviceRumbles(void)
{
	TSoURDt3rd_GamepadR_I_GamepadRumble(cv_usejoystick.value, 0, 0);
	TSoURDt3rd_GamepadR_I_GamepadRumbleTriggers(cv_usejoystick.value, 0, 0);

	TSoURDt3rd_GamepadR_I_GamepadRumble(cv_usejoystick2.value, 0, 0);
	TSoURDt3rd_GamepadR_I_GamepadRumbleTriggers(cv_usejoystick2.value, 0, 0);
}

static void update_vkb_axis(INT32 axis)
{
	if (axis > JOYAXISRANGE/2)
		M_SwitchVirtualKeyboard(true);
}

//
// boolean STAR_G_MapEventsToControls(event_t *ev)
//
// A lock-on function to G_MapEventsToControls(), featuring edits to allow for
//	both TSoURDt3rd uniqueness and SRB2 compatibility.
// Returns true if it shouldn't run the main event mapper, false otherwise.
//
boolean STAR_G_MapEventsToControls(event_t *ev)
{
	INT32 i;

	if (snake && Snake_Joy_Grabber(ev))
		return true;

	switch (ev->type)
	{
		case ev_keydown:
			if (ev->key < NUMINPUTS)
				M_MenuTypingInput(ev->key);
			break;

		case ev_joystick:
		case ev_joystick2: // buttons are virtual keys
			i = ev->key;
			if (i >= JOYAXISSET)
				break;

			if (i >= JOYANALOGS)
			{
				if (ev->x != INT32_MAX)
					update_vkb_axis(max(0, ev->x));

				if (ev->y != INT32_MAX)
					update_vkb_axis(max(0, ev->y));
			}
			else
			{
				// Actual analog sticks
				if (ev->x != INT32_MAX)
					update_vkb_axis(abs(ev->x));

				if (ev->y != INT32_MAX)
					update_vkb_axis(abs(ev->y));
			}
			break;

		default:
			break;
	}

	return false;
}

//
// boolean STAR_G_BuildTiccmd(ticcmd_t *cmd, INT32 realtics, UINT8 ssplayer)
//
// A lock-on function to G_BuildTiccmd(), featuring edits to allow for
//	both TSoURDt3rd uniqueness and SRB2 compatibility.
// Returns true if it shouldn't run the main ticcmd builder, false otherwise.
//
boolean STAR_G_BuildTiccmd(ticcmd_t *cmd, INT32 realtics, UINT8 ssplayer)
{
	(void)cmd;
	(void)realtics;
	(void)ssplayer;

	if (!Playing())
		return true;
	else if (Playing() && (menuactive || CON_Ready() || chat_on))
		return false;

	return false;
}
