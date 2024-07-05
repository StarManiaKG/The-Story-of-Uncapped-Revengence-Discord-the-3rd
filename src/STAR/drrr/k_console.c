// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_console.c
/// \brief Console drawing and input

#include "k_console.h"
#include "../../hu_stuff.h"

#include "../ss_inputs.h"

INT32 CON_ShiftChar(INT32 ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		// Standard Latin-script uppercase translation
		if (shiftdown ^ capslock)
			ch = shiftxform[ch];
	}
	else if (ch >= KEY_KEYPAD7 && ch <= KEY_KPADDEL)
	{
		// Numpad keycodes mapped to printable equivalent
		const char keypad_translation[] =
		{
			'7','8','9','-',
			'4','5','6','+',
			'1','2','3',
			'0','.'
		};

		ch = keypad_translation[ch - KEY_KEYPAD7];
	}
	else if (ch == KEY_KPADSLASH)
	{
		// Ditto, but non-contiguous keycode
		ch = '/';
	}
	else
	{
		// QWERTY keycode translation
		if (shiftdown)
			ch = shiftxform[ch];
	}

	return ch;
}
