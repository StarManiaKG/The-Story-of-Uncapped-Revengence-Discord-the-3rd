// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_pad_command.c
/// \brief Gamepad refactor command data

#include "smkg_pad_game.h"
#include "../../command.h"

#ifndef OLD_GAMEPAD_AXES
boolean CV_ConvertOldJoyAxisVars(consvar_t *v, const char *valstr)
{
	static struct {
		const char *old;
		const char *new;
	} axis_names[] = {
		{"X-Axis",    "Left Stick X"},
		{"Y-Axis",    "Left Stick Y"},
		{"X-Axis-",   "Left Stick X-"},
		{"Y-Axis-",   "Left Stick Y-"},
		{"X-Rudder",  "Right Stick X"},
		{"Y-Rudder",  "Right Stick Y"},
		{"X-Rudder-", "Right Stick X-"},
		{"Y-Rudder-", "Right Stick Y-"},
		{"Z-Axis",    "Left Trigger"},
		{"Z-Rudder",  "Right Trigger"},
		{"Z-Axis-",   "Left Trigger"},
		{"Z-Rudder-", "Right Trigger"},
		{NULL, NULL}
	};

	if (v->PossibleValue != joyaxis_cons_t)
		return true;

	for (unsigned i = 0;; i++)
	{
		if (axis_names[i].old == NULL)
		{
			CV_SetCVar(v, "None", false);
			return false;
		}
		else if (!stricmp(valstr, axis_names[i].old))
		{
			CV_SetCVar(v, axis_names[i].new, false);
			return false;
		}
	}

	return true;
}
#endif
