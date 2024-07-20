// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_g_inputs.c
/// \brief handle unique TSoURDt3rd mouse/keyboard/joystick inputs, etc.

#include "smkg_g_inputs.h"

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

	// Check game inputs //
	for (INT32 i = 0; gamekeydown[urGameControl[key][i]]; i++)
	{
		if (!gamekeydown[urGameControl[key][i]])
		{
			game_key->pressed = false;
			continue;
		}
		game_key->pressed = true;
		break;
	}

	// Manage keys //
	if (!game_key->pressed)
	{
		// Reset everything if not tapping
		game_key->keyDown = 0;
		game_key->tapReady = false;
		game_key->held = false;
		return;
	}

	// Keydown protection
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
	star_gamekey_t *game_key;

	STAR_G_KeyResponder(player, key);
	game_key = &STAR_GameKey[player][key];

	return (!game_key->held && game_key->pressed);
}

//
// boolean STAR_G_KeyHeld(UINT8 player, UINT8 key)
// Checks if the given key, for the given player, is being held.
//
boolean STAR_G_KeyHeld(UINT8 player, UINT8 key)
{
	star_gamekey_t *game_key;

	STAR_G_KeyResponder(player, key);
	game_key = &STAR_GameKey[player][key];

	return (game_key->pressed || game_key->held);
}

//
// void STAR_G_DefineDefaultControls(void)
// Defines the default controls for some unique TSoURDt3rd inputs.
//
void STAR_G_DefineDefaultControls(void)
{
	INT32 i;

	for (i = 1; i < num_gamecontrolschemes; i++) // skip gcs_custom (0)
	{
		gamecontroldefault[i][JB_OPENJUKEBOX        ][0] = 'j';
		gamecontroldefault[i][JB_INCREASEMUSICSPEED ][0] = '=';
		gamecontroldefault[i][JB_DECREASEMUSICSPEED ][0] = '-';
		gamecontroldefault[i][JB_PLAYMOSTRECENTTRACK][0] = 'l';
		gamecontroldefault[i][JB_STOPJUKEBOX        ][0] = 'k';

		gamecontrolbisdefault[i][JB_OPENJUKEBOX        ][0] = 'j';
		gamecontrolbisdefault[i][JB_INCREASEMUSICSPEED ][0] = '=';
		gamecontrolbisdefault[i][JB_DECREASEMUSICSPEED ][0] = '-';
		gamecontrolbisdefault[i][JB_PLAYMOSTRECENTTRACK][0] = 'l';
		gamecontrolbisdefault[i][JB_STOPJUKEBOX        ][0] = 'k';
	}
}
