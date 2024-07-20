// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_g_inputs.h
/// \brief TSoURDt3rd unique input data and structures

#ifndef __SMKG_G_INPUTS__
#define __SMKG_G_INPUTS__

#include "../g_input.h"

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct star_gamekey_s {
	boolean tapReady;
	INT32 keyDown;
	boolean pressed;
	boolean held;
} star_gamekey_t;
extern star_gamekey_t STAR_GameKey[1][NUM_GAMECONTROLS];

// ------------------------ //
//        Functions
// ------------------------ //

void STAR_G_KeyResponder(UINT8 player, UINT8 key);

boolean STAR_G_KeyPressed(UINT8 player, UINT8 key);
boolean STAR_G_KeyHeld(UINT8 player, UINT8 key);

void STAR_G_DefineDefaultControls(void);

#endif // __SMKG_G_INPUTS__
