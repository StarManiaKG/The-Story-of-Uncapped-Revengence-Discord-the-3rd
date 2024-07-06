// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_pad_i_system.c
/// \brief Gamepad refactor system stuff for SDL and SRB2

#ifdef HAVE_SDL

#include "../smkg_pad_i_system.h"
#include "../../drrr/kg_input.h"

INT32 STAR_I_GetGamepadKey(void)
{
   // Warning: I_GetKey empties the event queue till next keypress
	event_t *ev;
	INT32 rc = 0;

	G_ResetAllDeviceResponding();

	// return the first keypress from the event queue
	for (; eventtail != eventhead; eventtail = (eventtail+1)&(MAXEVENTS-1))
	{
		ev = &events[eventtail];

		HandleGamepadDeviceEvents(ev);

		if (ev->type == ev_keydown || ev->type == ev_console)
		{
			rc = ev->key;
			continue;
		}
	}

	return rc;
}

#endif
