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

#include "km_menu.h"
#include "kg_input.h"
#include "kg_game.h"
#include "../../m_misc.h"

#include "../m_menu.h"
#include "../ss_inputs.h"

menucmd_t menucmd[MAXSPLITSCREENPLAYERS];

void M_ChangeCvarDirect(INT32 choice, consvar_t *cv)
{
	// Backspace sets values to default value
	if (choice == -1)
	{
		CV_Set(cv, cv->defaultvalue);
		return;
	}

	choice = (choice == 0 ? -1 : 1);

	if (((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_SLIDER)
		|| ((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_INVISSLIDER)
		|| ((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_NOMOD))
	{
		CV_SetValue(cv, cv->value+choice);
	}
	else if (cv->flags & CV_FLOAT)
	{
		if (((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_SLIDER)
			||((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_INVISSLIDER)
			||((currentMenu->menuitems[itemOn].status & IT_CVARTYPE) == IT_CV_NOMOD)
			|| !(currentMenu->menuitems[itemOn].status & IT_CV_INTEGERSTEP))
		{
			char s[20];
			float n = FIXED_TO_FLOAT(cv->value)+(choice)*(1.0f/16.0f);
			sprintf(s,"%ld%s",(long)n,M_Ftrim(n));
			CV_Set(cv,s);
		}
		else
			CV_SetValue(cv,FIXED_TO_FLOAT(cv->value)+(choice));
	}
	else
	{
		CV_AddValue(cv, choice);
	}
}

const char *M_QueryCvarAction(const char *replace, consvar_t *cvar)
{
    if (replace)
		CV_Set(cvar, replace);
	return cvar->string;
}

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

void M_UpdateMenuCMD(UINT8 i, boolean bailrequired)
{
	UINT8 setup_numplayers = 1; // STAR NOTE: true setup_numplayers doesn't exist right now. try again later. //
	UINT8 mp = max(1, setup_numplayers);

	menucmd[i].prev_dpad_ud = menucmd[i].dpad_ud;
	menucmd[i].prev_dpad_lr = menucmd[i].dpad_lr;

	menucmd[i].dpad_ud = 0;
	menucmd[i].dpad_lr = 0;

	menucmd[i].buttonsHeld = menucmd[i].buttons;
	menucmd[i].buttons = 0;

	if (G_PlayerInputDown(i, GC_SCREENSHOT,    mp)) { menucmd[i].buttons |= MBT_SCREENSHOT; }
	if (G_PlayerInputDown(i, GC_RECORDGIF,    mp)) { menucmd[i].buttons |= MBT_STARTMOVIE; }
	//if (G_PlayerInputDown(i, GC_RECORDLOSSLESS, mp)) { menucmd[i].buttons |= MBT_STARTLOSSLESS; }

	// Screenshot et al take priority
	if (menucmd[i].buttons != 0)
		return;

	if (G_PlayerInputDown(i, GC_FORWARD,     mp)) { menucmd[i].dpad_ud--; }
	if (G_PlayerInputDown(i, GC_BACKWARD,    mp)) { menucmd[i].dpad_ud++; }

	if (G_PlayerInputDown(i, GC_STRAFELEFT,  mp)) { menucmd[i].dpad_lr--; }
	if (G_PlayerInputDown(i, GC_STRAFERIGHT, mp)) { menucmd[i].dpad_lr++; }

	if (G_PlayerInputDown(i, GC_JUMP, mp) || G_GetDeviceGameKeyDownArray(0)[KEY_ENTER]) { menucmd[i].buttons |= MBT_A; }
	if (G_PlayerInputDown(i, GC_SPIN, mp) || G_GetDeviceGameKeyDownArray(0)[KEY_ESCAPE]) { menucmd[i].buttons |= MBT_B; }

	if (G_PlayerInputDown(i, GC_FIRE, mp)) { menucmd[i].buttons |= MBT_C; }
	if (G_PlayerInputDown(i, GC_FIRENORMAL, mp)) { menucmd[i].buttons |= MBT_X; }
	if (G_PlayerInputDown(i, GC_TOSSFLAG, mp)) { menucmd[i].buttons |= MBT_Y; }
	if (G_PlayerInputDown(i, GC_CUSTOM1, mp)) { menucmd[i].buttons |= MBT_Z; }
	

	if (G_PlayerInputDown(i, GC_TURNLEFT, mp)) { menucmd[i].buttons |= MBT_L; }
	if (G_PlayerInputDown(i, GC_TURNRIGHT, mp)) { menucmd[i].buttons |= MBT_R; }

	if (G_GetDeviceGameKeyDownArray(0)[KEY_ENTER] || G_GetDeviceGameKeyDownArray(0)[KEY_ESCAPE]) { menucmd[i].buttons |= MBT_START; }

	if (bailrequired && i == 0)
	{
		if (G_GetDeviceGameKeyDownArray(0)[KEY_ESCAPE]) { menucmd[i].buttons |= MBT_B; }
	}

	if (menucmd[i].dpad_ud == 0 && menucmd[i].dpad_lr == 0 && menucmd[i].buttons == 0)
	{
		// Reset delay count with no buttons.
		menucmd[i].delay = min(menucmd[i].delay, MENUMINDELAY);
		menucmd[i].delayCount = 0;
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
