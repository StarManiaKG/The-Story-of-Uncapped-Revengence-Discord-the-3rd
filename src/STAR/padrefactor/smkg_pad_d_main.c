// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_pad_d_main.c
/// \brief Gamepad refactor main program data

#include "smkg_pad_d_main.h"
#include "../drrr/kg_input.h"
#include "../drrr/km_menu.h"
#include "../../d_net.h"

void STAR_GamepadR_D_ProcessEvents(event_t *ev)
{
	G_ResetAllDeviceResponding();

	HandleGamepadDeviceEvents(ev);

	if (!snake)
		G_MapEventsToControls(ev);
}

void STAR_GamepadR_D_UpdateMenuControls(void)
{
	INT32 i;

	// Update menu CMD
	for (i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		M_UpdateMenuCMD(i, false);
	}
}

void STAR_GamepadR_D_ResetGamepadData(event_t *ev)
{
	G_ResetAllDeviceGameKeyDown();
	G_ResetAllDeviceResponding();

	if (ev)
		HandleGamepadDeviceEvents(ev);
}
