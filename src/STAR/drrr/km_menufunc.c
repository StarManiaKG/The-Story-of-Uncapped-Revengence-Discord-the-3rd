// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2016 by Kay "Kaito" Sinclaire.
// Copyright (C) 2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  km_menufunc.c
/// \brief SRB2Kart's menu functions

#include "k_menu.h"
#include "kg_input.h"
#include "../../m_misc.h"

#include "../m_menu.h"
#include "../smkg_g_inputs.h"

menucmd_t menucmd[MAXSPLITSCREENPLAYERS];

void M_SetMenuDelay(UINT8 i)
{
	menucmd[i].delayCount++;
	if (menucmd[i].delayCount < 1)
	{
		// Shouldn't happen, but for safety.
		menucmd[i].delayCount = 1;
	}

	menucmd[i].delay = (MENUDELAYTIME - min(MENUDELAYTIME - 1, menucmd[i].delayCount));
	if (menucmd[i].delay < MENUMINDELAY)
	{
		menucmd[i].delay = MENUMINDELAY;
	}
}

boolean M_MenuButtonPressed(UINT8 pid, UINT32 bt)
{
	if (menucmd[pid].buttonsHeld & bt)
	{
		return false;
	}

	return !!(menucmd[pid].buttons & bt);
}

boolean M_MenuButtonHeld(UINT8 pid, UINT32 bt)
{
	return !!(menucmd[pid].buttons & bt);
}

// Returns true if we press the confirmation button
boolean M_MenuConfirmPressed(UINT8 pid)
{
	 return M_MenuButtonPressed(pid, MBT_A);
}

boolean M_MenuConfirmHeld(UINT8 pid)
{
	 return M_MenuButtonHeld(pid, MBT_A);
}

// Returns true if we press the Cancel button
boolean M_MenuBackPressed(UINT8 pid)
{
	 return (M_MenuButtonPressed(pid, MBT_B) || M_MenuButtonPressed(pid, MBT_X));
}

boolean M_MenuBackHeld(UINT8 pid)
{
	 return (M_MenuButtonHeld(pid, MBT_B) || M_MenuButtonHeld(pid, MBT_X));
}

// Retrurns true if we press the tertiary option button (C)
boolean M_MenuExtraPressed(UINT8 pid)
{
	 return M_MenuButtonPressed(pid, MBT_C);
}

boolean M_MenuExtraHeld(UINT8 pid)
{
	 return M_MenuButtonHeld(pid, MBT_C);
}
