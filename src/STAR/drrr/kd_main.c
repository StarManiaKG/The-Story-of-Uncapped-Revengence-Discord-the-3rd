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
/// \file  kd_main.c
/// \brief SRB2 main program
///
///        SRB2 main program (D_SRB2Main) and game loop (D_SRB2Loop),
///        plus functions to parse command line parameters, configure game
///        parameters, and call the startup functions.

#include "kg_input.h"
#include "../../doomstat.h"
#include "../../console.h"
#include "../../d_net.h"
#include "../../d_event.h"
#include "../../g_demo.h"
#include "../../m_menu.h" // M_StartControlPanel

static void HandleGamepadDeviceAdded(event_t *ev)
{
	char guid[64];
	char name[256];

	I_Assert(ev != NULL);
	I_Assert(ev->type == ev_gamepad_device_added);

	G_RegisterAvailableGamepad(ev->device);
	I_GetGamepadGuid(ev->device, guid, sizeof(guid));
	I_GetGamepadName(ev->device, name, sizeof(name));
	CONS_Alert(CONS_NOTICE, "DRRR - Gamepad device %d connected: %s (%s)\n", ev->device, name, guid);
}

static void HandleGamepadDeviceRemoved(event_t *ev)
{
	int i = 0;
	I_Assert(ev != NULL);
	I_Assert(ev->type == ev_gamepad_device_removed);
	CONS_Alert(CONS_NOTICE, "DRRR - Gamepad device %d disconnected\n", ev->device);

	boolean playerinterrupted = false;

	for (i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		INT32 device = G_GetDeviceForPlayer(i);
		if (device == ev->device)
		{
			G_SetDeviceForPlayer(i, -1);
			playerinterrupted = true;
		}
	}

	// Downstream responders need to update player gamepad assignments, pause, etc
	G_UnregisterAvailableGamepad(ev->device);

	if (playerinterrupted && Playing() && !netgame && !demoplayback)
	{
		M_StartControlPanel();
	}
}

/// Respond to added/removed device events, for bookkeeping available gamepads.
void HandleGamepadDeviceEvents(event_t *ev)
{
	I_Assert(ev != NULL);

	switch (ev->type)
	{
	case ev_gamepad_device_added:
		HandleGamepadDeviceAdded(ev);
		break;
	case ev_gamepad_device_removed:
		HandleGamepadDeviceRemoved(ev);
		break;
	default:
		break;
	}
}
