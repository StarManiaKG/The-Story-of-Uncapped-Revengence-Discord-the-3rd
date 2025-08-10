// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2022-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-multi-connection.c
/// \brief Extended data for the multiplayer connection menu.

#include "../smkg-m_sys.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_HandleMasterServerResetChoice(INT32 choice)
// Handles resetting the master server address.
//
// Ported from SRB2Kart!
//
void TSoURDt3rd_M_HandleMasterServerResetChoice(INT32 choice)
{
	if (choice == 'y' || choice == KEY_ENTER)
	{
		CV_Set(&cv_masterserver, cv_masterserver.defaultvalue);
		S_StartSoundFromEverywhere(sfx_s221);
	}
	tsourdt3rd_local.ms_address_changed = true;
}
