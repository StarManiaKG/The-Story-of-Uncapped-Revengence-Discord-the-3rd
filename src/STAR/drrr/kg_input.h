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
/// \file  kg_input.h
/// \brief handle mouse/keyboard/joystick inputs,
///        maps inputs to game controls (forward, spin, jump...)

#ifndef __KG_INPUT__
#define __KG_INPUT__

#include "ki_joy.h"
#include "../../g_input.h"
#include "../../i_joy.h"

#ifdef __cplusplus
extern "C" {
#endif

// current state of the keys: JOYAXISRANGE or 0 when boolean.
// Or anything inbetween for analog values
#define KEYBOARD_MOUSE_DEVICE (0)
#define UNASSIGNED_DEVICE (-1)
#define NO_BINDS_REACHABLE (-1)

#define MAXINPUTMAPPING 1 //4 // STAR NOTE: srb2 has it at two lol //

//
// mouse and joystick buttons are handled as 'virtual' keys
//
typedef enum
{
	DRRR_KEY_JOY1 = NUMKEYS,
	DRRR_KEY_HAT1 = DRRR_KEY_JOY1 + 11, // macro for SDL_CONTROLLER_BUTTON_DPAD_UP
	KEY_AXIS1 = DRRR_KEY_JOY1 + JOYBUTTONS,
	JOYINPUTEND = KEY_AXIS1 + JOYAXES,

	DRRR_KEY_MOUSE1 = JOYINPUTEND,
	DRRR_KEY_MOUSEMOVE = DRRR_KEY_MOUSE1 + MOUSEBUTTONS,
	DRRR_KEY_MOUSEWHEELUP = DRRR_KEY_MOUSEMOVE + 4,
	DRRR_KEY_MOUSEWHEELDOWN = DRRR_KEY_MOUSEWHEELUP + 1,
	DRRR_MOUSEINPUTEND = DRRR_KEY_MOUSEWHEELDOWN + 1,

	DRRR_NUMINPUTS = DRRR_MOUSEINPUTEND,
} drrr_key_input_e;

// several key codes (or virtual key) per game control
extern INT32 menucontrolreserved[NUM_GAMECONTROLS][MAXINPUTMAPPING];

/// Register a device index (from ev_gamepad_device_added) as an Available Gamepad
void G_RegisterAvailableGamepad(INT32 device_id);
/// Unregister a device index (from ev_gamepad_device_removed) as an Available Gamepad
void G_UnregisterAvailableGamepad(INT32 device_id);
/// Get the number of Available Gamepads registered.
INT32 G_GetNumAvailableGamepads(void);
/// Get the device ID for a given Available Gamepad Index, or -1. 0 <= available_index < G_GetNumAvailableGamepads()
INT32 G_GetAvailableGamepadDevice(INT32 available_index);

INT32 G_GetPlayerForDevice(INT32 deviceID);
/// Get gamepad device for given player, or -1.
INT32 G_GetDeviceForPlayer(INT32 player);

/// Set the given player index's assigned device. If the device is in use by another player, that player is unassigned.
void G_SetDeviceForPlayer(INT32 player, INT32 device);

#if 1
// STAR NOTE: NOT USED ANYWHERE ELSE FOR NOW //
extern consvar_t cv_rumble[MAXSPLITSCREENPLAYERS];
#endif

void G_PlayerDeviceRumble(INT32 player, UINT16 low_strength, UINT16 high_strength);
#if 1
// STAR NOTE: NOT USED ANYWHERE ELSE FOR NOW //
void G_PlayerDeviceRumbleTriggers(INT32 player, UINT16 left_strength, UINT16 right_strength);
void G_ResetPlayerDeviceRumble(INT32 player);
void G_ResetAllDeviceRumbles(void);
#endif

/// Get the gamekeydown array (NUMINPUTS values) for the given device, or NULL if the device id is invalid.
INT32* G_GetDeviceGameKeyDownArray(INT32 device);
void G_ResetAllDeviceGameKeyDown(void);

#if 1
// STAR NOTE: NOT USED ANYWHERE ELSE FOR NOW //
boolean G_IsDeviceResponding(INT32 device);
#endif
void G_SetDeviceResponding(INT32 device, boolean responding);
void G_ResetAllDeviceResponding(void);

void HandleGamepadDeviceEvents(event_t *ev);

boolean AutomaticControllerReassignmentIsAllowed(INT32 device);
INT32 AssignDeviceToFirstUnassignedPlayer(INT32 device);

void update_vkb_axis(INT32 axis);

boolean G_KeyBindIsNecessary(INT32 gc);
boolean G_KeyIsAvailable(INT32 key, INT32 deviceID);

/// STAR STUFF: set menu reserved controls ///
void DRRR_G_DefineDefaultControls(void);

/// STAR STUFF: Handle unique DRRR gamepad events ///
void DRRR_G_MapEventsToControls(event_t *ev);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KG_INPUT__
