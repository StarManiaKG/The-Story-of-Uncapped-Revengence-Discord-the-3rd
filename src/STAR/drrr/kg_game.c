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
/// \file  kg_game.c
/// \brief game loop functions, events handling

#include "kg_game.h"
#include "kg_input.h"
#include "ki_joy.h"
#include "../../g_game.h"

#include "../ss_inputs.h"

static INT32 G_GetValueFromControlTable(INT32 deviceID, INT32 deadzone, INT32 *controltable)
{
	INT32 i, failret = NO_BINDS_REACHABLE;

	if (deviceID <= UNASSIGNED_DEVICE)
	{
		// An invalid device can't have any binds!
		return failret;
	}

	for (i = 0; i < MAXINPUTMAPPING; i++)
	{
		INT32 key = controltable[i];
		INT32 value = 0;

		// Invalid key number.
		if (G_KeyIsAvailable(key, deviceID) == false)
		{
			continue;
		}

		value = G_GetDeviceGameKeyDownArray(deviceID)[key];

		if (value >= deadzone)
		{
			return value;
		}

		failret = 0;
	}

	// Not pressed.
	return failret;
}

INT32 G_PlayerInputAnalog(UINT8 p, INT32 gc, UINT8 menuPlayers)
{
#if 0
	const INT32 deadzone = (JOYAXISRANGE * cv_deadzone[p].value) / FRACUNIT;
#else
	// STAR NOTE: SRB2 has different deadzone types, so just do the bare minimum here for now //
	const INT32 deadzone = (JOYAXISRANGE * (p == 0 ? cv_deadzone.value : cv_deadzone2.value)) / FRACUNIT;
#endif
	const INT32 keyboard_player = G_GetPlayerForDevice(KEYBOARD_MOUSE_DEVICE);
	const boolean in_menu = (menuPlayers > 0);
	const boolean main_player = (p == 0);
	INT32 deviceID = UNASSIGNED_DEVICE;
	INT32 value = -1;
	INT32 avail_gamepad_id = 0;
	INT32 i;
	boolean bind_was_reachable = false;

	INT32 (*urGameControl)[2] = (p == 0 ? gamecontrol : gamecontrolbis);

	if (p >= MAXSPLITSCREENPLAYERS)
	{
#ifdef PARANOIA
		CONS_Debug(DBG_GAMELOGIC, "G_PlayerInputAnalog: Invalid player ID %d\n", p);
#endif
		return 0;
	}

	deviceID = G_GetDeviceForPlayer(p);

	if ((in_menu == true && G_KeyBindIsNecessary(gc) == true) // In menu: check for all unoverrideable menu default controls.
		|| (in_menu == false && gc == GC_SYSTEMMENU)) // In gameplay: check for the unoverrideable start button to be able to bring up the menu.
	{
		value = G_GetValueFromControlTable(KEYBOARD_MOUSE_DEVICE, JOYAXISRANGE/4, &(menucontrolreserved[gc][0]));
		if (value > 0) // Check for press instead of bound.
		{
			// This is only intended for P1.
			if (main_player == true)
			{
				return value;
			}
			else
			{
				return 0;
			}
		}
	}

	// Player 1 is always allowed to use the keyboard in 1P, even if they got disconnected.
	if (main_player == true && keyboard_player == -1 && deviceID == UNASSIGNED_DEVICE)
	{
		deviceID = KEYBOARD_MOUSE_DEVICE;
	}

	// First, try our actual binds.
	value = G_GetValueFromControlTable(deviceID, deadzone, &(urGameControl[gc][0]));
	if (value > 0)
	{
		return value;
	}
	if (value != NO_BINDS_REACHABLE)
	{
		bind_was_reachable = true;
	}

	// If you're on gamepad in 1P, and you didn't have a gamepad bind for this, then try your keyboard binds.
	if (main_player == true && keyboard_player == -1 && deviceID > KEYBOARD_MOUSE_DEVICE)
	{
		value = G_GetValueFromControlTable(KEYBOARD_MOUSE_DEVICE, deadzone, &(urGameControl[gc][0]));
		if (value > 0)
		{
			return value;
		}
		if (value != NO_BINDS_REACHABLE)
		{
			bind_was_reachable = true;
		}
	}

	if (in_menu == true)
	{
		if (main_player == true)
		{
			// We are P1 controlling menus. We should be able to
			// control the menu with any unused gamepads, so
			// that gamepads are able to navigate to the player
			// setup menu in the first place.
			for (avail_gamepad_id = 0; avail_gamepad_id < G_GetNumAvailableGamepads(); avail_gamepad_id++)
			{
				INT32 tryDevice = G_GetAvailableGamepadDevice(avail_gamepad_id);
				if (tryDevice <= KEYBOARD_MOUSE_DEVICE)
				{
					continue;
				}

				for (i = 0; i < menuPlayers; i++)
				{
					if (tryDevice == G_GetDeviceForPlayer(i))
					{
						// Don't do this for already taken devices.
						break;
					}
				}

				if (i == menuPlayers)
				{
					// This gamepad isn't being used, so we can
					// use it for P1 menu navigation.
					value = G_GetValueFromControlTable(tryDevice, deadzone, &(urGameControl[gc][0]));
					if (value > 0)
					{
						return value;
					}
					if (value != NO_BINDS_REACHABLE)
					{
						bind_was_reachable = true;
					}
				}
			}
		}

		if (bind_was_reachable == false)
		{
			// Still nothing bound after everything. Try default gamepad controls.
			for (i = 0; i < num_gamecontrolschemes; i++)
			{
				value = G_GetValueFromControlTable(deviceID, deadzone, &(gamecontroldefault[i][gc][0]));

				if (value > 0)
				{
					return value;
				}
			}
		}
	}

	// Literally not bound at all, so it can't be pressed at all.
	return 0;
}

#undef KEYBOARDDEFAULTSSPLIT

boolean G_PlayerInputDown(UINT8 p, INT32 gc, UINT8 menuPlayers)
{
	return (abs(G_PlayerInputAnalog(p, gc, menuPlayers)) >= JOYAXISRANGE/2);
}
