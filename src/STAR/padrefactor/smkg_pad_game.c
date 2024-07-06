// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_pad_g_game.c
/// \brief Gamepad refactor game loop functions and event handling

#include "smkg_pad_game.h"
#include "../drrr/kg_input.h"
#include "../../g_demo.h"

#if 0
CV_PossibleValue_t joyaxis_cons_t[] = {{0, "None"},
#ifndef OLD_GAMEPAD_AXES
	{1, "Left Stick X"}, {2, "Left Stick Y"},
	{3, "Right Stick X"},{4, "Right Stick Y"},
	{-1, "Left Stick X-"}, {-2, "Left Stick Y-"},
	{-3, "Right Stick X-"}, {-4, "Right Stick Y-"},
	{5, "Left Trigger"}, {6, "Right Trigger"},
#else
	{1, "X-Axis"}, {2, "Y-Axis"}, {-1, "X-Axis-"}, {-2, "Y-Axis-"},
	#if JOYAXISSET > 1
	{3, "Z-Axis"}, {4, "X-Rudder"}, {-3, "Z-Axis-"}, {-4, "X-Rudder-"},
	#endif
	#if JOYAXISSET > 2
	{5, "Y-Rudder"}, {6, "Z-Rudder"}, {-5, "Y-Rudder-"}, {-6, "Z-Rudder-"},
	#endif
	#if JOYAXISSET > 3
	{7, "U-Axis"}, {8, "V-Axis"}, {-7, "U-Axis-"}, {-8, "V-Axis-"},
	#endif
#endif
	{0, NULL}
};
#endif

void STAR_G_ApplyGamepads(event_t *ev)
{
	if (gameaction == ga_nothing && ((demoplayback && !modeattacking && !multiplayer) || gamestate == GS_TITLESCREEN))
	{
		// any other key pops up menu if in demos
		if (ev->type == ev_keydown
		|| (ev->type == ev_gamepad_axis && ev->key >= JOYANALOGS
			&& ((abs(ev->x) > JOYAXISRANGE/2 || abs(ev->y) > JOYAXISRANGE/2))
		))
		{
			if (ev->device > 0)
			{
				G_SetDeviceForPlayer(0, ev->device);
			}
		}
	}
}
