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
#include "kg_game.h"
#include "km_menu.h"
#include "../../v_video.h"
#include "../../z_zone.h"

// two key codes (or virtual key) per game control
INT32 menucontrolreserved[NUM_GAMECONTROLS][MAXINPUTMAPPING];

static INT32 g_gamekeydown_device0[NUMINPUTS];

static INT32 g_available_gamepad_devices;
static INT32 g_gamepad_device_ids[MAXGAMEPADS];
static INT32* g_gamepad_gamekeydown[MAXGAMEPADS];
static boolean g_device0_responding;
static boolean g_gamepad_responding[MAXGAMEPADS];
static INT32 g_player_devices[MAXSPLITSCREENPLAYERS] = {-1, -1};

void G_RegisterAvailableGamepad(INT32 device_id)
{
	I_Assert(device_id >= 1);

	if (g_available_gamepad_devices == MAXGAMEPADS)
	{
		// too many!
		return;
	}

	g_gamepad_device_ids[g_available_gamepad_devices] = device_id;

	g_gamepad_gamekeydown[g_available_gamepad_devices] = Z_CallocAlign(NUMINPUTS * sizeof(INT32), PU_STATIC, NULL, 4);

	g_gamepad_responding[g_available_gamepad_devices] = false;

	g_available_gamepad_devices += 1;
}

void G_UnregisterAvailableGamepad(INT32 device_id)
{
	int i = 0;

	I_Assert(device_id >= 1);

	if (g_available_gamepad_devices <= 0)
	{
		return;
	}

	for (i = 0; i < g_available_gamepad_devices; i++)
	{
		if (g_gamepad_device_ids[i] == device_id)
		{
			int32_t *old_gamekeydown = g_gamepad_gamekeydown[i];
			g_gamepad_device_ids[i] = g_gamepad_device_ids[g_available_gamepad_devices - 1];
			g_gamepad_gamekeydown[i] = g_gamepad_gamekeydown[g_available_gamepad_devices - 1];
			g_gamepad_responding[i] = g_gamepad_responding[g_available_gamepad_devices - 1];
			Z_Free(old_gamekeydown);
			g_available_gamepad_devices -= 1;
			return;
		}
	}
}

INT32 G_GetNumAvailableGamepads(void)
{
	return g_available_gamepad_devices;
}

INT32 G_GetAvailableGamepadDevice(INT32 available_index)
{
	if (available_index < 0 || available_index >= G_GetNumAvailableGamepads())
	{
		return -1;
	}

	return g_gamepad_device_ids[available_index];
}

INT32 G_GetPlayerForDevice(INT32 device_id)
{
	INT32 i;

	for (i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		if (device_id == g_player_devices[i])
		{
			return i;
		}
	}

	return -1;
}

INT32 G_GetDeviceForPlayer(INT32 player)
{
	int i;

	if (G_GetPlayerForDevice(KEYBOARD_MOUSE_DEVICE) == player)
	{
		return KEYBOARD_MOUSE_DEVICE;
	}

	for (i = 0; i < G_GetNumAvailableGamepads() + 1; i++)
	{
		INT32 device = G_GetAvailableGamepadDevice(i);
		if (G_GetPlayerForDevice(device) == player)
		{
			return device;
		}
	}

	return -1;
}

void G_SetDeviceForPlayer(INT32 player, INT32 device)
{
	int i;

	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);
	I_Assert(device >= -1);

	g_player_devices[player] = device;

	if (device == -1)
	{
		return;
	}

	if (device != KEYBOARD_MOUSE_DEVICE)
	{
		I_SetGamepadPlayerIndex(device, player);
	}

	for (i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		if (i == player)
		{
			continue;
		}

		if (g_player_devices[i] == device)
		{
			g_player_devices[i] = -1;
			if (device > 0)
			{
				I_SetGamepadPlayerIndex(device, -1);
				I_GamepadRumble(device, 0, 0);
				I_GamepadRumbleTriggers(device, 0, 0);
			}
		}
	}
}

void G_SetPlayerGamepadIndicatorToPlayerColor(INT32 player)
{
	INT32 device;
	UINT16 skincolor;
	byteColor_t byte_color;

	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);

	device = G_GetDeviceForPlayer(player);

	if (device <= 0)
	{
		return;
	}

	skincolor = M_GetCvPlayerColor(player);

	byte_color = V_GetColor(skincolors[skincolor].ramp[8]).s;

	I_SetGamepadIndicatorColor(device, byte_color.red, byte_color.green, byte_color.blue);
}

INT32* G_GetDeviceGameKeyDownArray(INT32 device)
{
	int i;

	I_Assert(device >= 0);

	if (device == KEYBOARD_MOUSE_DEVICE)
	{
		return g_gamekeydown_device0;
	}

	for (i = 0; i < g_available_gamepad_devices; i++)
	{
		if (g_gamepad_device_ids[i] == device)
		{
			return g_gamepad_gamekeydown[i];
		}
	}

	return NULL;
}

void G_ResetAllDeviceGameKeyDown(void)
{
	int i;

	memset(gamekeydown, 0, sizeof(gamekeydown));
	memset(g_gamekeydown_device0, 0, sizeof(g_gamekeydown_device0));

	for (i = 0; i < g_available_gamepad_devices; i++)
	{
		memset(g_gamepad_gamekeydown[i], 0, sizeof(INT32) * NUMINPUTS);
	}
}

boolean G_IsDeviceResponding(INT32 device)
{
	int i;

	I_Assert(device >= 0);

	if (device == KEYBOARD_MOUSE_DEVICE)
	{
		return g_device0_responding;
	}

	for (i = 0; i < g_available_gamepad_devices; i++)
	{
		INT32 device_id = G_GetAvailableGamepadDevice(i);
		if (device_id == device)
		{
			return g_gamepad_responding[i];
		}
	}

	return false;
}

void G_SetDeviceResponding(INT32 device, boolean responding)
{
	int i;

	I_Assert(device >= 0);

	if (device == KEYBOARD_MOUSE_DEVICE)
	{
		g_device0_responding = responding;
		return;
	}

	for (i = 0; i < g_available_gamepad_devices; i++)
	{
		INT32 device_id = G_GetAvailableGamepadDevice(i);
		if (device_id == device)
		{
			g_gamepad_responding[i] = responding;
			return;
		}
	}
}

void G_ResetAllDeviceResponding(void)
{
	int i;
	int num_gamepads;

	g_device0_responding = false;

	num_gamepads = G_GetNumAvailableGamepads();

	for (i = 0; i < num_gamepads; i++)
	{
		g_gamepad_responding[i] = false;
	}
}

void G_PlayerDeviceRumble(INT32 player, UINT16 low_strength, UINT16 high_strength)
{
	INT32 device_id;

	if (cv_rumble[player].value == 0)
	{
		return;
	}

	device_id = G_GetDeviceForPlayer(player);

	if (device_id < 1)
	{
		return;
	}

	I_GamepadRumble(device_id, low_strength, high_strength);
}

void G_PlayerDeviceRumbleTriggers(INT32 player, UINT16 left_strength, UINT16 right_strength)
{
	INT32 device_id;

	if (cv_rumble[player].value == 0)
	{
		return;
	}

	device_id = G_GetDeviceForPlayer(player);

	if (device_id < 1)
	{
		return;
	}

	I_GamepadRumbleTriggers(device_id, left_strength, right_strength);
}

void G_ResetPlayerDeviceRumble(INT32 player)
{
	INT32 device_id;

	device_id = G_GetDeviceForPlayer(player);

	if (device_id < 1)
	{
		return;
	}

	I_GamepadRumble(device_id, 0, 0);
	I_GamepadRumbleTriggers(device_id, 0, 0);
}

void G_ResetAllDeviceRumbles(void)
{
	int i;
	int devices;

	devices = G_GetNumAvailableGamepads();

	for (i = 0; i < devices; i++)
	{
		INT32 device_id = G_GetAvailableGamepadDevice(i);

		I_GamepadRumble(device_id, 0, 0);
		I_GamepadRumbleTriggers(device_id, 0, 0);
	}
}

static boolean AutomaticControllerReassignmentIsAllowed(INT32 device)
{
	boolean device_is_gamepad = device > 0;
	boolean device_is_unassigned = G_GetPlayerForDevice(device) == -1;
	boolean gamestate_is_in_active_play = (gamestate == GS_LEVEL || gamestate == GS_INTERMISSION);

	return device_is_gamepad && device_is_unassigned && gamestate_is_in_active_play;
}

static INT32 AssignDeviceToFirstUnassignedPlayer(INT32 device)
{
	int i;

	for (i = 0; i < splitscreen + 1; i++)
	{
		if (G_GetDeviceForPlayer(i) == -1)
		{
			G_SetDeviceForPlayer(i, device);
			return i;
		}
	}

	return -1;
}

static void update_vkb_axis(INT32 axis)
{
	if (axis > JOYAXISRANGE/2)
		M_SwitchVirtualKeyboard(true);
}

//
// Remaps the inputs to game controls.
//
// A game control can be triggered by one or more keys/buttons.
//
// Each key/mousebutton/joybutton triggers ONLY ONE game control.
//
// STAR NOTE: Features edits to allow for both TSoURDt3rd uniqueness and SRB2 compatibility.
//
void DRRR_G_MapEventsToControls(event_t *ev)
{
	INT32 i;
	INT32 *DeviceGameKeyDownArray;

	if (ev->device >= 0)
	{
		switch (ev->type)
		{
			case ev_keydown:
			//case ev_keyup:
			//case ev_mouse:
			//case ev_joystick:
			//case ev_joystick2:
			//case ev_gamepad_axis:
				G_SetDeviceResponding(ev->device, true);
				break;

			default:
				break;
		}
	}
	else
	{
		return;
	}

	DeviceGameKeyDownArray = G_GetDeviceGameKeyDownArray(ev->device);

	if (!DeviceGameKeyDownArray)
		return;

	switch (ev->type)
	{
		case ev_keydown:
			if (ev->key < NUMINPUTS)
			{
				M_MenuTypingInput(ev->key);

#if 1
				if (ev->x) // OS repeat? We handle that ourselves
				{
					CONS_Printf("STOP\n");
					break;
				}
#endif

				DeviceGameKeyDownArray[ev->key] = JOYAXISRANGE;

				if (AutomaticControllerReassignmentIsAllowed(ev->device))
				{
					INT32 assigned = AssignDeviceToFirstUnassignedPlayer(ev->device);
					if (assigned >= 0)
					{
						CONS_Alert(CONS_NOTICE, "TSoURDt3rd; DRRR Gamepads - Player %d device was reassigned\n", assigned + 1);
					}
				}
			}
#ifdef PARANOIA
			else
			{
				CONS_Debug(DBG_GAMELOGIC, "TSoURDt3rd; DRRR Gamepads - Bad downkey input %d\n", ev->key);
			}
#endif
			break;

		case ev_keyup:
			if (ev->key < NUMINPUTS)
			{
				DeviceGameKeyDownArray[ev->key] = 0;
			}
#ifdef PARANOIA
			else
			{
				CONS_Debug(DBG_GAMELOGIC, "TSoURDt3rd; DRRR Gamepads - Bad upkey input %d\n", ev->key);
			}
#endif
			break;

#if 0
		case ev_mouse: // buttons are virtual keys
			// X axis
			if (ev->x < 0)
			{
				// Left
				DeviceGameKeyDownArray[KEY_MOUSEMOVE + 2] = abs(ev->x);
				DeviceGameKeyDownArray[KEY_MOUSEMOVE + 3] = 0;
			}
			else
			{
				// Right
				DeviceGameKeyDownArray[KEY_MOUSEMOVE + 2] = 0;
				DeviceGameKeyDownArray[KEY_MOUSEMOVE + 3] = abs(ev->x);
			}

			// Y axis
			if (ev->y < 0)
			{
				// Up
				DeviceGameKeyDownArray[KEY_MOUSEMOVE] = abs(ev->y);
				DeviceGameKeyDownArray[KEY_MOUSEMOVE + 1] = 0;
			}
			else
			{
				// Down
				DeviceGameKeyDownArray[KEY_MOUSEMOVE] = 0;
				DeviceGameKeyDownArray[KEY_MOUSEMOVE + 1] = abs(ev->y);
			}
			break;
#endif

		case ev_gamepad_axis: // buttons are virtual keys
			if (ev->key >= JOYAXISSETS)
			{
#ifdef PARANOIA
				CONS_Debug(DBG_GAMELOGIC, "TSoURDt3rd; DRRR Gamepads - Bad joystick axis event %d\n", ev->key);
#endif
				break;
			}

			i = ev->key;

			if (i >= JOYANALOGS)
			{
				// The trigger axes are handled specially.
				i -= JOYANALOGS;

				if (AutomaticControllerReassignmentIsAllowed(ev->device)
					&& (abs(ev->x) > JOYAXISRANGE/2 || abs(ev->y) > JOYAXISRANGE/2))
				{
					INT32 assigned = AssignDeviceToFirstUnassignedPlayer(ev->device);
					if (assigned >= 0)
					{
						CONS_Alert(CONS_NOTICE, "TSoURDt3rd; DRRR Gamepads - Player %d device was reassigned\n", assigned + 1);
					}
				}

				if (ev->x != INT32_MAX)
				{
					DeviceGameKeyDownArray[KEY_AXIS1 + (JOYANALOGS * 4) + (i * 2)] = max(0, ev->x);
					update_vkb_axis(max(0, ev->x));
				}

				if (ev->y != INT32_MAX)
				{
					DeviceGameKeyDownArray[KEY_AXIS1 + (JOYANALOGS * 4) + (i * 2) + 1] = max(0, ev->y);
					update_vkb_axis(max(0, ev->y));
				}
			}
			else
			{
				// We used to only allow this assignment for triggers, but it caused some confusion in vote screen.
				// In case of misebhaving devices, break glass.
				if (AutomaticControllerReassignmentIsAllowed(ev->device)
					&& (abs(ev->x) > JOYAXISRANGE/2 || abs(ev->y) > JOYAXISRANGE/2))
				{
					INT32 assigned = AssignDeviceToFirstUnassignedPlayer(ev->device);
					if (assigned >= 0)
					{
						CONS_Alert(CONS_NOTICE, "TSoURDt3rd; DRRR Gamepads - Player %d device was reassigned\n", assigned + 1);
					}
				}

				// Actual analog sticks
				if (ev->x != INT32_MAX)
				{
					if (ev->x < 0)
					{
						// Left
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4)] = abs(ev->x);
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4) + 1] = 0;
					}
					else
					{
						// Right
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4)] = 0;
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4) + 1] = abs(ev->x);
					}
					update_vkb_axis(abs(ev->x));
				}

				if (ev->y != INT32_MAX)
				{
					if (ev->y < 0)
					{
						// Up
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4) + 2] = abs(ev->y);
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4) + 3] = 0;
					}
					else
					{
						// Down
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4) + 2] = 0;
						DeviceGameKeyDownArray[KEY_AXIS1 + (i * 4) + 3] = abs(ev->y);
					}
					update_vkb_axis(abs(ev->y));
				}
			}
			break;

		default:
			break;
	}
}

// If keybind is necessary to navigate menus, it's on this list.
boolean G_KeyBindIsNecessary(INT32 gc)
{
	switch (gc)
	{
		case GC_JUMP:
		case GC_FIRE:
		case GC_FIRENORMAL:
		case GC_FORWARD:
		case GC_BACKWARD:
		case GC_STRAFELEFT:
		case GC_STRAFERIGHT:
		//case KEY_ENTER: // Is necessary, but handled special.
			return true;
		default:
			return false;
	}
	return false;
}

// Returns false if a key is deemed unreachable for this device.
boolean G_KeyIsAvailable(INT32 key, INT32 deviceID)
{
	boolean gamepad_key = false;

	// Invalid key number.
	if (key <= 0 || key >= DRRR_NUMINPUTS)
	{
		return false;
	}

	// Only allow gamepad keys for gamepad devices,
	// and vice versa.
	gamepad_key = (key >= KEY_JOY1 && key < JOYINPUTEND);
	if (deviceID == KEYBOARD_MOUSE_DEVICE)
	{
		if (gamepad_key == true)
		{
			return false;
		}
	}
	else
	{
		if (gamepad_key == false)
		{
			return false;
		}
	}

	return true;
}

void DRRR_G_DefineDefaultControls(void)
{
	// Menu reserved controls
	menucontrolreserved[GC_FORWARD    ][0] = KEY_UPARROW;
	menucontrolreserved[GC_BACKWARD   ][0] = KEY_DOWNARROW;
	menucontrolreserved[GC_STRAFELEFT ][0] = KEY_LEFTARROW;
	menucontrolreserved[GC_STRAFERIGHT][0] = KEY_RIGHTARROW;
	menucontrolreserved[GC_JUMP       ][0] = KEY_ENTER;
	menucontrolreserved[GC_FIRE       ][0] = KEY_BACKSPACE;
	menucontrolreserved[GC_FIRENORMAL ][0] = KEY_ESCAPE;
#if 0
	menucontrolreserved[gamekeydown[KEY_ENTER]][0] = KEY_ESCAPE; // Handled special
#endif
}
