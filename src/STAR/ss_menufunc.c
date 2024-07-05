// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_menufunc.c
/// \brief TSoURDt3rd unique menu functions

#include "ss_inputs.h"
#include "m_menu.h"
#include "../command.h"
#include "../g_input.h"
#include "../s_sound.h"

#include "drrr/km_menu.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_HandleTyping(INT32 choice, size_t cachelen, consvar_t *cvar, boolean (*closeroutine)(void), void (*abortroutine)(void))
// Handles typing, DRRR style, with some minor additions.
//
// Ported and reworked from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_M_HandleTyping(INT32 choice, size_t cachelen, consvar_t *cvar, boolean (*closeroutine)(void), void (*abortroutine)(void))
{
	UINT8 pid = 0;
	boolean exitmenu = false;

	currentMenu->lastOn = itemOn;

	if (!menutyping.active)
	{
		switch (choice)
		{
			case KEY_UPARROW:
				M_PrevOpt();
				S_StartSound(NULL, sfx_menu1);
				break;
			case KEY_DOWNARROW:
				M_NextOpt();
				S_StartSound(NULL, sfx_menu1);
				break;
			case KEY_ESCAPE:
				exitmenu = true;
				break;

			default:
				// If we're hovering over a IT_CV_STRING option, pressing A/X opens the typing submenu
				if (M_MenuConfirmPressed(pid)
#if 1
				// STAR NOTE: improve DRRR input system, then remove :) //
				|| choice == KEY_ENTER
#endif
				)
				{
					// If we entered this menu by pressing a menu Key, default to keyboard typing, otherwise use controller.
					M_OpenVirtualKeyboard(
						cachelen, // Max length for discord strings
						M_QueryCvarAction,
						NULL,
						cvar,
						closeroutine,
						abortroutine
					);
				}
				break;
		}

		if (exitmenu)
		{
			if (currentMenu->prevMenu)
				M_SetupNextMenu(currentMenu->prevMenu);
			else
				M_ClearMenus(true);
		}
		return;
	}

	if (M_MenuExtraPressed(pid))
	{
		//if (!(currentMenu->behaviourflags & MBF_SOUNDLESS))
			S_StartSound(NULL, sfx_s3k5b);

		M_ChangeCvarDirect(-1, cvar);
		M_SetMenuDelay(pid);
	}
}
