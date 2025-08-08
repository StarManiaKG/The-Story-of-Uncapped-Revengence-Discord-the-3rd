// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_g_inputs.c
/// \brief Handle unique TSoURDt3rd mouse/keyboard/joystick inputs, etc.

#include "smkg_g_inputs.h"
#include "menus/smkg-m_sys.h" // menutyping junk //

#include "../console.h"
#include "../g_game.h"
#include "../hu_stuff.h"
#include "../i_joy.h"
#include "../m_menu.h"
#include "../snake.h"

#include "../netcode/d_net.h"

// ------------------------ //
//        Variables
// ------------------------ //

star_gamekey_t STAR_GameKey[1][NUM_GAMECONTROLS];

// ------------------------ //
//        Functions
// ------------------------ //

//
// void STAR_G_KeyResponder(UINT8 player, UINT8 key)
// Checks if the given key is being pressed.
//
void STAR_G_KeyResponder(UINT8 player, UINT8 key)
{
	star_gamekey_t *game_key = &STAR_GameKey[player][key];
	INT32 (*urGameControl)[2] = (player == 0 ? gamecontrol : gamecontrolbis);

	// Check game inputs.
	for (INT32 i = 0; i < 2; i++)
	{
		game_key->pressed = gamekeydown[urGameControl[key][i]];
		if (game_key->pressed) break;
	}
	if (!game_key->pressed)
	{
		// -- Reset everything if not tapping.
		game_key->keyDown = 0;
		game_key->tapReady = false;
		game_key->held = false;
		return;
	}

	// Keydown protection.
	if (game_key->tapReady)
	{
		game_key->held = (game_key->keyDown > TICRATE/2);
		game_key->pressed = false;
		game_key->keyDown++;
	}
	else
	{
		game_key->pressed = true;
		game_key->tapReady = true;
	}
}

//
// boolean STAR_G_KeyPressed(UINT8 player, UINT8 key)
// Checks if the given key, for the given player, is only being pressed.
//
boolean STAR_G_KeyPressed(UINT8 player, UINT8 key)
{
	STAR_G_KeyResponder(player, key);
	const star_gamekey_t *game_key = &STAR_GameKey[player][key];
	return (game_key->pressed && !game_key->held);
}

//
// boolean STAR_G_KeyHeld(UINT8 player, UINT8 key)
// Checks if the given key, for the given player, is being held.
//
boolean STAR_G_KeyHeld(UINT8 player, UINT8 key)
{
	STAR_G_KeyResponder(player, key);
	const star_gamekey_t *game_key = &STAR_GameKey[player][key];
	return (game_key->pressed || game_key->held);
}

//
// void TSoURDt3rd_D_ProcessEvents(void)
// Processes our unique key and pad events too!
//
void TSoURDt3rd_D_ProcessEvents(void)
{
	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		TSoURDt3rd_M_UpdateMenuCMD(i);
	}
}

//
// boolean TSoURDt3rd_G_MapEventsToControls(event_t *ev)
//
// A lock-on function to G_MapEventsToControls(), featuring edits to allow for
//	both TSoURDt3rd uniqueness and SRB2 compatibility.
// Returns true if it shouldn't run the main event mapper, false otherwise.
//
static void update_vkb_axis(INT32 axis)
{
	if (axis > JOYAXISRANGE/2)
		TSoURDt3rd_M_SwitchVirtualKeyboard(true);
}

boolean TSoURDt3rd_G_MapEventsToControls(event_t *ev)
{
	INT32 i;

	if (Snake_JoyGrabber(tsourdt3rd_snake, ev))
		return true;

	switch (ev->type)
	{
		case ev_keydown:
			if (ev->key < NUMINPUTS)
				TSoURDt3rd_M_MenuTypingInput(ev->key);
			break;

		case ev_joystick:
		case ev_joystick2: // buttons are virtual keys
			i = ev->key;
			if (i >= JOYAXISSET)
				break;

			if (i >= 2) // 2 sets of analog stick axes, with positive and negative each
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
// void TSoURDt3rd_G_DefineDefaultControls(void)
// Defines the default controls for some unique TSoURDt3rd inputs.
//
void TSoURDt3rd_G_DefineDefaultControls(void)
{
	for (INT32 i = 1; i < num_gamecontrolschemes; i++) // skip gcs_custom (0)
	{
		gamecontroldefault   [i][JB_OPENJUKEBOX        ][0] = 'j';
		gamecontroldefault   [i][JB_INCREASEMUSICSPEED ][0] = '=';
		gamecontroldefault   [i][JB_DECREASEMUSICSPEED ][0] = '-';
		gamecontroldefault   [i][JB_INCREASEMUSICPITCH ][0] = ']';
		gamecontroldefault   [i][JB_DECREASEMUSICPITCH ][0] = '[';
		gamecontroldefault   [i][JB_PLAYMOSTRECENTTRACK][0] = 'l';
		gamecontroldefault   [i][JB_STOPJUKEBOX        ][0] = 'k';

		gamecontrolbisdefault[i][JB_OPENJUKEBOX        ][0] = 'j';
		gamecontrolbisdefault[i][JB_INCREASEMUSICSPEED ][0] = '=';
		gamecontrolbisdefault[i][JB_DECREASEMUSICSPEED ][0] = '-';
		gamecontrolbisdefault[i][JB_INCREASEMUSICPITCH ][0] = ']';
		gamecontrolbisdefault[i][JB_DECREASEMUSICPITCH ][0] = '[';
		gamecontrolbisdefault[i][JB_PLAYMOSTRECENTTRACK][0] = 'l';
		gamecontrolbisdefault[i][JB_STOPJUKEBOX        ][0] = 'k';
	}
}
