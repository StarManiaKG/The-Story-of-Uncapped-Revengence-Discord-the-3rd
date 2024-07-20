// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_padr_main.c
/// \brief DRRR Gamepad refactor main program data

#include "smkg_padr_main.h"
#include "../menus/smkg_m_func.h"

#include "../../g_game.h"

// ------------------------ //
//        Functions
// ------------------------ //

void STAR_GamepadR_D_UpdateMenuControls(void)
{
	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
		STAR_M_UpdateMenuCMD(i);
}
