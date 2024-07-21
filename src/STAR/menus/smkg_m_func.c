// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_m_func.c
/// \brief Unique TSoURDt3rd menu routines and structures

#include "smkg_m_func.h"
#include "smkg_m_draw.h"

#include "../drrr/k_menu.h"

#include "../../console.h"
#include "../../d_main.h"
#include "../../g_game.h"
#include "../../i_time.h"
#include "../../m_misc.h" // M_Ftrim //
#include "../../v_video.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

enum
{
	// Fade wipes
	tsourdt3rd_wipe_menu_toblack,
	TSOURDT3RD_NUMWIPEDEFS
};

UINT8 tsourdt3rd_wipedefs[TSOURDT3RD_NUMWIPEDEFS] = {
	1  // tsourdt3rd_wipe_menu_toblack
};

INT16 tsourdt3rd_itemOn = 1; // menu item skull is on, Hack by Tails 09-18-2002
INT16 tsourdt3rd_skullAnimCounter = 1; // skull animation counter
struct menutransition_s menutransition; // Menu transition properties

INT32 menuKey = -1; // keyboard key pressed for menu
static INT32 accurate_menuKey = -1; // helps accurately check for menu inputs
static tic_t joywait = 0, mousewait = 0; // joystick and mouse menu controls

// finish wipes between screens
boolean menuwipe = false;

// lock out further input in a tic when important buttons are pressed
// (in other words -- stop bullshit happening by mashing buttons in fades)
boolean tsourdt3rd_noFurtherInput = false;

// lock-on for menus, allowing for unique menu features
static tsourdt3rd_menu_t *tsourdt3rd_allMenus;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_AddNewMenu(tsourdt3rd_menu_t *newmenu, menu_t *menup)
// Adds unique TSoURDt3rd menu data, based on the new data and existing menu given.
//
void TSoURDt3rd_M_AddNewMenu(tsourdt3rd_menu_t *newmenu, menu_t *menup)
{
	if (!newmenu || !menup)
		return;

	newmenu->menu = menup;
	newmenu->nextmenu = tsourdt3rd_allMenus;
	tsourdt3rd_allMenus = newmenu;
};

//
// void TSoURDt3rd_M_SetupNextMenu(menu_t *menudef, boolean notransition)
// Wrapper for M_SetupNextMenu, allowing for setting up unique TSoURDt3rd menu data.
//
void TSoURDt3rd_M_SetupNextMenu(menu_t *menudef, boolean notransition)
{
	tsourdt3rd_menu_t *menus;
	tsourdt3rd_menu_t *prevMenu = tsourdt3rd_currentMenu;

	for (menus = tsourdt3rd_allMenus; menus; menus = menus->nextmenu)
	{
		if (!menus->menu)
			continue;
		if (menus->menu == menudef)
			break;
	}
	tsourdt3rd_currentMenu = menus;

	M_ShiftMessageQueueDown();

	if (tsourdt3rd_currentMenu == NULL)
		return;

	if (!notransition)
	{
		if (prevMenu != NULL && prevMenu->transitionID == tsourdt3rd_currentMenu->transitionID
			&& tsourdt3rd_currentMenu->transitionTics)
		{
			menutransition.startmenu = prevMenu;
			menutransition.endmenu = tsourdt3rd_currentMenu;

			menutransition.tics = 0;
			menutransition.dest = tsourdt3rd_currentMenu->transitionTics;
			menutransition.in = false;
			return; // Don't change menu yet, the transition will call this again
		}
		else if (menuactive)
		{
			menuwipe = true;
			M_FlipKartGamemodeMenu(false);
			F_WipeStartScreen();
			V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);
			F_WipeEndScreen();
			F_RunWipe(tsourdt3rd_wipedefs[tsourdt3rd_wipe_menu_toblack], false);
		}
	}
	menutransition.enabled = false;

	if (prevMenu != NULL && prevMenu->quitroutine)
	{
		// If you're going from a menu to itself, why are you running the quitroutine? You're not quitting it! -SH
		if (prevMenu != tsourdt3rd_currentMenu && !prevMenu->quitroutine())
			return; // we can't quit this menu (also used to set parameter from the menu)
	}

	if (tsourdt3rd_currentMenu->initroutine != NULL)
	{
		// Moving to a new menu, reinitialize.
		tsourdt3rd_currentMenu->initroutine();
	}

	//M_UpdateMenuBGImage(false);
	//M_PlayMenuJam();

#ifdef HAVE_DISCORDSUPPORT
	if (menuactive && !Playing())
	{
		// currentMenu changed during menuactive
		DRPC_UpdatePresence();
	}
#endif
}

//
// static void M_UpdateMenuCMD(UINT8 i)
// Updates the menu buttons needed for unique menu movement.
//
static void M_UpdateMenuCMD(UINT8 i)
{
	INT32 p = i+1;

	menucmd[i].prev_dpad_ud = menucmd[i].dpad_ud;
	menucmd[i].prev_dpad_lr = menucmd[i].dpad_lr;

	menucmd[i].dpad_ud = 0;
	menucmd[i].dpad_lr = 0;

	menucmd[i].buttonsHeld = menucmd[i].buttons;
	menucmd[i].buttons = 0;

	if (PLAYERINPUTDOWN(p, GC_SCREENSHOT)) { menucmd[i].buttons |= MBT_SCREENSHOT; }
	if (PLAYERINPUTDOWN(p, GC_RECORDGIF)) { menucmd[i].buttons |= MBT_STARTMOVIE; }
	//if (PLAYERINPUTDOWN(p, GC_RECORDLOSSLESS)) { menucmd[i].buttons |= MBT_STARTLOSSLESS; }

	// Screenshot et al take priority
	if (menucmd[i].buttons != 0)
		return;

	if (PLAYERINPUTDOWN(p, GC_FORWARD) || accurate_menuKey == KEY_UPARROW) { menucmd[i].dpad_ud--; }
	if (PLAYERINPUTDOWN(p, GC_BACKWARD) || accurate_menuKey == KEY_DOWNARROW) { menucmd[i].dpad_ud++; }

	if (PLAYERINPUTDOWN(p, GC_STRAFELEFT) || accurate_menuKey == KEY_LEFTARROW) { menucmd[i].dpad_lr--; }
	if (PLAYERINPUTDOWN(p, GC_STRAFERIGHT) || accurate_menuKey == KEY_RIGHTARROW) { menucmd[i].dpad_lr++; }

	if (accurate_menuKey == KEY_ENTER) { menucmd[i].buttons |= MBT_A; }
	if (accurate_menuKey == KEY_ESCAPE) { menucmd[i].buttons |= MBT_B; }
	if (accurate_menuKey == KEY_BACKSPACE) { menucmd[i].buttons |= MBT_C; }

	if (PLAYERINPUTDOWN(p, GC_FIRENORMAL) || accurate_menuKey == KEY_LSHIFT) { menucmd[i].buttons |= MBT_X; }
	if (PLAYERINPUTDOWN(p, GC_CUSTOM1)) { menucmd[i].buttons |= MBT_Y; }
	if (PLAYERINPUTDOWN(p, GC_CUSTOM2)) { menucmd[i].buttons |= MBT_Z; }

	if (PLAYERINPUTDOWN(p, GC_TURNLEFT)) { menucmd[i].buttons |= MBT_L; }
	if (PLAYERINPUTDOWN(p, GC_TURNRIGHT)) { menucmd[i].buttons |= MBT_R; }

	if (PLAYERINPUTDOWN(p, GC_TOSSFLAG)) { menucmd[i].buttons |= MBT_START; }	

	if (menucmd[i].dpad_ud == 0 && menucmd[i].dpad_lr == 0 && menucmd[i].buttons == 0)
	{
		// Reset delay count with no buttons.
		menucmd[i].delay = min(menucmd[i].delay, MENUMINDELAY);
		menucmd[i].delayCount = 0;
	}

	if (joywait >= I_GetTime() || mousewait >= I_GetTime() || (menuKey > 0 && menuKey < NUMKEYS))
		accurate_menuKey = -1;
}

//
// boolean STAR_M_DoesMenuHaveKeyHandler(void)
// Returns whether or not the current menu option has specific routines for keys.
//
boolean STAR_M_DoesMenuHaveKeyHandler(void)
{
	menuitem_t *curmenuitem = &currentMenu->menuitems[tsourdt3rd_itemOn];

	if (!curmenuitem)
		return false;

	if ((curmenuitem->status & IT_CVARTYPE) == IT_CV_STRING)
		return true;
	else if (curmenuitem->status == IT_MSGHANDLER)
		return true;
	else if ((curmenuitem->status & IT_TYPE) == IT_KEYHANDLER)
		return true;

	return false;
}

//
// void STAR_M_RevampedMenuInputWrapper(INT32 *key)
// Emulates our revamped menu input system, for the regular SRB2 menus :p.
//
/// \todo rework, then add to M_Responder
//
void STAR_M_RevampedMenuInputWrapper(INT32 *key)
{
	if (!menuactive)
		return;

	if (*key != -1)
		return;

	if (STAR_M_DoesMenuHaveKeyHandler())
		return; // we may need to type or manually move, so calm down right quick

	if (PLAYERINPUTDOWN(1, GC_FORWARD))
		*key = KEY_UPARROW;
	else if (PLAYERINPUTDOWN(1, GC_BACKWARD))
		*key = KEY_DOWNARROW;
	else if (PLAYERINPUTDOWN(1, GC_STRAFELEFT) || PLAYERINPUTDOWN(1, GC_TURNLEFT))
		*key = KEY_LEFTARROW;
	else if (PLAYERINPUTDOWN(1, GC_STRAFERIGHT) || PLAYERINPUTDOWN(1, GC_TURNRIGHT))
		*key = KEY_RIGHTARROW;
	else if (PLAYERINPUTDOWN(1, GC_JUMP))
		*key = KEY_ENTER;
	else if (PLAYERINPUTDOWN(1, GC_SPIN) || PLAYERINPUTDOWN(1, GC_SYSTEMMENU))
		*key = KEY_ESCAPE;
}

//
// static void M_SetAccurateMenuKey(INT32 *key, event_t *ev)
// Sets the accurate keys required for menu movement, generally by setting the inputs into specific keys.
//
static void M_SetAccurateMenuKey(INT32 *key, event_t *ev)
{
	static INT32 pjoyx = 0, pjoyy = 0;
	static INT32 pmousex = 0, pmousey = 0;
	static INT32 lastx = 0, lasty = 0;

	if (menuKey > 0 && menuKey < NUMKEYS)
	{
		if (ev->type == ev_keydown)
			*key = menuKey;
		else
			*key = -1;
		return;
	}

	if (ev->type == ev_keydown)
	{
		switch (ev->key)
		{
			case KEY_JOY1:
			case KEY_MOUSE1:
				*key = KEY_ENTER;
				break;
			case KEY_JOY1 + 3:
				*key = KEY_LSHIFT;
				break;
			case KEY_JOY1 + 1:
			case KEY_MOUSE1 + 1:
				*key = KEY_ESCAPE;
				break;
			case KEY_JOY1 + 2:
				*key = KEY_BACKSPACE;
				break;
			case KEY_HAT1:
				*key = KEY_UPARROW;
				break;
			case KEY_HAT1 + 1:
				*key = KEY_DOWNARROW;
				break;
			case KEY_HAT1 + 2:
				*key = KEY_LEFTARROW;
				break;
			case KEY_HAT1 + 3:
				*key = KEY_RIGHTARROW;
				break;
			default:
				if (*key)
					*key = -1;
				break;
		}
	}
	else if (ev->type == ev_joystick && ev->key == 0 && joywait < I_GetTime())
	{
		const INT32 jdeadzone = (JOYAXISRANGE * cv_digitaldeadzone.value) / FRACUNIT;

		if (ev->y != INT32_MAX)
		{
			if (Joystick.bGamepadStyle || abs(ev->y) > jdeadzone)
			{
				if (ev->y < 0 && pjoyy >= 0)
				{
					*key = KEY_UPARROW;
					joywait = I_GetTime() + NEWTICRATE/7;
				}
				else if (ev->y > 0 && pjoyy <= 0)
				{
					*key = KEY_DOWNARROW;
					joywait = I_GetTime() + NEWTICRATE/7;
				}
				pjoyy = ev->y;
			}
			else
				pjoyy = 0;
		}

		if (ev->x != INT32_MAX)
		{
			if (Joystick.bGamepadStyle || abs(ev->x) > jdeadzone)
			{
				if (ev->x < 0 && pjoyx >= 0)
				{
					*key = KEY_LEFTARROW;
					joywait = I_GetTime() + NEWTICRATE/7;
				}
				else if (ev->x > 0 && pjoyx <= 0)
				{
					*key = KEY_RIGHTARROW;
					joywait = I_GetTime() + NEWTICRATE/7;
				}
				pjoyx = ev->x;
			}
			else
				pjoyx = 0;
		}
	}
	else if (ev->type == ev_mouse && mousewait < I_GetTime())
	{
		pmousey -= ev->y;
		if (pmousey < lasty-30)
		{
			*key = KEY_DOWNARROW;
			mousewait = I_GetTime() + NEWTICRATE/7;
			pmousey = lasty -= 30;
		}
		else if (pmousey > lasty + 30)
		{
			*key = KEY_UPARROW;
			mousewait = I_GetTime() + NEWTICRATE/7;
			pmousey = lasty += 30;
		}

		pmousex += ev->x;
		if (pmousex < lastx - 30)
		{
			*key = KEY_LEFTARROW;
			mousewait = I_GetTime() + NEWTICRATE/7;
			pmousex = lastx -= 30;
		}
		else if (pmousex > lastx+30)
		{
			*key = KEY_RIGHTARROW;
			mousewait = I_GetTime() + NEWTICRATE/7;
			pmousex = lastx += 30;
		}
	}
	else if (ev->type == ev_keyup && *key != -1)
		*key = -1;
	else
	{
		if (PLAYERINPUTDOWN(1, GC_JUMP)) { *key = KEY_ENTER; }
		else if (PLAYERINPUTDOWN(1, GC_SPIN) || PLAYERINPUTDOWN(1, GC_SYSTEMMENU)) { *key = KEY_ESCAPE; }
	}
}

//
// boolean STAR_M_Responder(event_t *ev)
// Unique menu input responder for TSoURDt3rd menus.
//
boolean STAR_M_Responder(event_t *ev)
{
	if (tsourdt3rd_noFurtherInput)
	{
		// Ignore input after enter/escape/other buttons
		// (but still allow shift keyup so caps doesn't get stuck
		return false;
	}

	if (menuactive)
		G_MapEventsToControls(ev); // update keys current state

	if (ev->type == ev_keydown && ev->key > 0 && ev->key < NUMKEYS)
	{
		// Record keyboard presses
		menuKey = ev->key;
	}

	if (tsourdt3rd_currentMenu == NULL)
	{
		// We're not in our unique menu system!
		// Why run this then?
		return false;
	}

	M_SetAccurateMenuKey(&accurate_menuKey, ev);
	return true;
}

//
// boolean STAR_M_StartControlPanel(void)
//
// Runs our unique control panel data.
// Returns true if it needs to restrict usage
//	of the regular control panel, false otherwise.
//
boolean STAR_M_StartControlPanel(void)
{
	memset(menucmd, 0, sizeof (menucmd));
	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		menucmd[i].delay = MENUDELAYTIME;
	}

	if (gamestate == GS_TITLESCREEN)
	{
		if (menumessage.active)
		{
			if (!menumessage.closing && menumessage.fadetimer == 9)
			{
				// The following doesn't work with MM_YESNO.
				// However, because there's no guarantee a profile
				// is selected or controls set up to our liking,
				// we can't call M_HandleMenuMessage.
				DRRR_M_StopMessage(MA_NONE);
			}

			return true;
		}
	}

#ifdef HAVE_DISCORDSUPPORT
	if (!Playing())
	{
		// currentMenu changed during menuactive
		DRPC_UpdatePresence();
	}
#endif

	return false;
}

//
// void STAR_M_GoBack(INT32 choice)
// Moves backwards to the previous menu.
//
void STAR_M_GoBack(INT32 choice)
{
	const INT16 behaviourflags = tsourdt3rd_currentMenu->behaviourflags;
	(void)choice;

	tsourdt3rd_noFurtherInput = true;
	currentMenu->lastOn = tsourdt3rd_itemOn;

	if (currentMenu->prevMenu)
		M_SetupNextMenu(currentMenu->prevMenu);

	if (!(behaviourflags & MBF_SOUNDLESS))
		S_StartSound(NULL, sfx_s3k5b);
}

//
// void STAR_M_ChangeCvarDirect(INT32 choice, consvar_t *cv)
// Directly changes the values of a given cvar, based on the choice.
//
void STAR_M_ChangeCvarDirect(INT32 choice, consvar_t *cv)
{
	// Backspace sets values to default value
	if (choice == -1)
	{
		CV_Set(cv, cv->defaultvalue);
		return;
	}

	choice = (choice == 0 ? -1 : 1);

	if (((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_SLIDER)
		|| ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_INVISSLIDER)
		|| ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_NOMOD))
	{
		CV_SetValue(cv, cv->value+choice);
	}
	else if (cv->flags & CV_FLOAT)
	{
		if (((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_SLIDER)
			||((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_INVISSLIDER)
			||((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_NOMOD)
			|| !(currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CV_INTEGERSTEP))
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

//
// static void M_ChangeCvar(INT32 choice)
// Wrapper for STAR_M_ChangeCvarDirect().
//
static void M_ChangeCvar(INT32 choice)
{
	consvar_t *cvar = currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
	STAR_M_ChangeCvarDirect(choice, cvar);
}

//
// static const char *M_QueryCvarAction(const char *replace)
// General virtual keyboard caching system.
//
static const char *M_QueryCvarAction(const char *replace)
{
	consvar_t *cvar = currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
	if (replace)
		CV_Set(cvar, replace);
	return cvar->string;
}

//
// boolean STAR_M_NextOpt(void)
// Finds the next valid menu item, returns true if it can.
//
boolean STAR_M_NextOpt(void)
{
	INT16 oldItemOn = tsourdt3rd_itemOn; // prevent infinite loop

	if (tsourdt3rd_currentMenu == NULL)
		return false; // somehow?

	do
	{
		if (tsourdt3rd_itemOn + 1 > currentMenu->numitems - 1)
		{
			// Prevent looparound here
			if (tsourdt3rd_currentMenu->behaviourflags & MBF_NOLOOPENTRIES)
			{
				tsourdt3rd_itemOn = oldItemOn;
				return false;
			}
			tsourdt3rd_itemOn = 0;
		}
		else
			tsourdt3rd_itemOn++;
	} while (oldItemOn != tsourdt3rd_itemOn && (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_SPACE);

	//M_UpdateMenuBGImage(false);
	M_FlipKartGamemodeMenu(true);

	return true;
}

//
// boolean STAR_M_PrevOpt(void)
// Finds a previous menu item, returns true if it can.
//
boolean STAR_M_PrevOpt(void)
{
	INT16 oldItemOn = tsourdt3rd_itemOn; // prevent infinite loop

	if (tsourdt3rd_currentMenu == NULL)
		return false; // could happen?

	do
	{
		if (!tsourdt3rd_itemOn)
		{
			// Prevent looparound here
			if (tsourdt3rd_currentMenu->behaviourflags & MBF_NOLOOPENTRIES)
			{
				tsourdt3rd_itemOn = oldItemOn;
				return false;
			}
			tsourdt3rd_itemOn = currentMenu->numitems - 1;
		}
		else
			tsourdt3rd_itemOn--;
	} while (oldItemOn != tsourdt3rd_itemOn && (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_SPACE);

	//M_UpdateMenuBGImage(false);
	M_FlipKartGamemodeMenu(true);

	return true;
}

//
// static void M_HandleMenuInput(void)
// Handles our menu inputs.
//
// Ported from Dr.Robotnik's Ring Racers!
//
static void M_HandleMenuInput(void)
{
	void (*routine)(INT32 choice); // for some casting problem
	UINT8 pid = 0; // todo: Add ability for any splitscreen player to bring up the menu.
	SINT8 lr = 0, ud = 0;
	INT32 thisMenuKey = menuKey;

	menuKey = -1;

	if (menuactive == false)
	{
		// We're not in the menu.
		return;
	}

	if (menumessage.active)
	{
		M_HandleMenuMessage();
		return;
	}

	// Typing for CV_IT_STRING
	if (menutyping.active)
	{
		M_MenuTypingInput(-1);
		return;
	}

	// Lactozilla did this for us somewhere, so I gotta thank him :)
	if (snake)
	{
		Snake_Handle();
		return;
	}

	if (menucmd[pid].delay > 0)
	{
		return;
	}

	if (tsourdt3rd_currentMenu == NULL)
	{
		return;
	}

	// Handle menu-specific input handling. If this returns true, we skip regular input handling.
	if (tsourdt3rd_currentMenu->inputroutine)
	{
		if (tsourdt3rd_currentMenu->inputroutine(thisMenuKey))
		{
			return;
		}
	}

	routine = currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;

	// BP: one of the more big hack i have never made
	if (routine && (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_CVAR)
	{
		if ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_STRING)
		{
			// Routine is null either way
			routine = NULL;

			// If we're hovering over a IT_CV_STRING option, pressing A/X opens the typing submenu
			if (M_MenuConfirmPressed(pid))
			{
				// If we entered this menu by pressing a menu Key, default to keyboard typing, otherwise use controller.
				M_OpenVirtualKeyboard(
					tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1,
					M_QueryCvarAction,
					NULL
				);
				return;
			}
			else if (M_MenuExtraPressed(pid))
			{
				if (!(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
					S_StartSound(NULL, sfx_s3k5b);

				M_ChangeCvar(-1);
				M_SetMenuDelay(pid);
				return;
			}
		}
		else
		{
			routine = M_ChangeCvar;
		}
	}

	lr = menucmd[pid].dpad_lr;
	ud = menucmd[pid].dpad_ud;

	if (tsourdt3rd_currentMenu->behaviourflags & MBF_UD_LR_FLIPPED)
	{
		ud = menucmd[pid].dpad_lr;
		lr = -menucmd[pid].dpad_ud;
	}

	// LR does nothing in the default menu, just remap as dpad.
	if (menucmd[pid].buttons & MBT_L) { lr--; }
	if (menucmd[pid].buttons & MBT_R) { lr++; }

	if (ud > 0)
	{
		if (STAR_M_NextOpt() && !(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
			S_StartSound(NULL, sfx_s3k5b);
		M_SetMenuDelay(pid);
		return;
	}
	else if (ud < 0)
	{
		if (STAR_M_PrevOpt() && !(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
			S_StartSound(NULL, sfx_s3k5b);
		M_SetMenuDelay(pid);
		return;
	}
	else if (lr < 0)
	{
		if (routine && ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_ARROWS
			|| (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_CVAR))
		{
			if (!(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
				S_StartSound(NULL, sfx_s3k5b);
			routine(0);
			M_SetMenuDelay(pid);
		}
		return;
	}
	else if (lr > 0)
	{
		if (routine && ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_ARROWS
			|| (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_CVAR))
		{
			if (!(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
				S_StartSound(NULL, sfx_s3k5b);
			routine(1);
			M_SetMenuDelay(pid);
		}
		return;
	}
	else if (M_MenuConfirmPressed(pid) /*|| M_MenuButtonPressed(pid, MBT_START)*/)
	{
		tsourdt3rd_noFurtherInput = true;
		currentMenu->lastOn = tsourdt3rd_itemOn;

		if (routine)
		{
			if (!(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
				S_StartSound(NULL, sfx_s3k5b);

			if (((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_CALL
				|| (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_SUBMENU)
				&& (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CALLTYPE))
			{
				if (((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CALLTYPE) & IT_CALL_NOTMODIFIED) && usedCheats)
				{
					DRRR_M_StartMessage("TSoURDt3rd; Modified Game", M_GetText("This cannot be done with complex addons\nor in a cheated game."), NULL, MM_NOTHING, NULL, NULL);
					return;
				}
			}

			switch (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE)
			{
				case IT_CVAR:
				case IT_ARROWS:
					routine(2); // usually right arrow
					break;
				case IT_CALL:
					routine(tsourdt3rd_itemOn);
					break;
				case IT_SUBMENU:
					currentMenu->lastOn = tsourdt3rd_itemOn;
					M_SetupNextMenu((menu_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction);
					break;
			}
		}

		M_SetMenuDelay(pid);
		return;
	}
	else if (M_MenuBackPressed(pid))
	{
		STAR_M_GoBack(0);
		M_SetMenuDelay(pid);
		return;
	}

	return;
}

//
// void STAR_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter)
// Handles menu ticking.
//
// Inspired by M_Ticker() from Dr.Robotnik's Ring Racers!
//
void STAR_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter)
{
	tsourdt3rd_itemOn = *item;
	tsourdt3rd_noFurtherInput = *input;
	tsourdt3rd_skullAnimCounter = skullAnimCounter;	

	if (!menuactive)
	{
		tsourdt3rd_noFurtherInput = false;
		return;
	}

	if (menutransition.tics != 0 || menutransition.dest != 0)
	{
		tsourdt3rd_noFurtherInput = true;

		if (menutransition.tics < menutransition.dest)
			menutransition.tics++;
		else if (menutransition.tics > menutransition.dest)
			menutransition.tics--;

		// If dest is non-zero, we've started transition and want to switch menus
		// If dest is zero, we're mid-transition and want to end it
		if (menutransition.tics == menutransition.dest)
		{
			if (menutransition.endmenu != NULL && menutransition.endmenu->menu != NULL
				&& currentMenu != menutransition.endmenu->menu)
			{
				if (menutransition.startmenu != NULL && menutransition.startmenu->transitionID == menutransition.endmenu->transitionID
					&& menutransition.endmenu->transitionTics)
				{
					menutransition.tics = menutransition.endmenu->transitionTics;
					menutransition.dest = 0;
					menutransition.in = true;
				}
				else if (menuactive)
				{
					memset(&menutransition, 0, sizeof(menutransition));

					menuwipe = true;
					F_WipeStartScreen();
					V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);
					F_WipeEndScreen();
					F_RunWipe(tsourdt3rd_wipedefs[tsourdt3rd_wipe_menu_toblack], false);
				}

				menutransition.enabled = true;
				M_SetupNextMenu(menutransition.endmenu->menu);
			}
			else
			{
				// Menu is done transitioning in
				M_FlipKartGamemodeMenu(true);
			}
		}
	}
	else
	{
		if (menuwipe)
		{
			// try not to let people input during the fadeout
			tsourdt3rd_noFurtherInput = true;
		}
		else
		{
			// reset input trigger
			tsourdt3rd_noFurtherInput = false;
		}
	}

	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		if (menucmd[i].delay > 0)
		{
			menucmd[i].delay--;
		}
	}

	if (tsourdt3rd_noFurtherInput == false)
	{
		M_HandleMenuInput();
	}

	if (tsourdt3rd_currentMenu == NULL)
	{
		return;
	}

	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
		M_UpdateMenuCMD(i);

	if (tsourdt3rd_currentMenu->tickroutine)
	{
		tsourdt3rd_currentMenu->tickroutine();
	}

	*item = tsourdt3rd_itemOn;
	*input = tsourdt3rd_noFurtherInput;
}

//
// void STAR_M_MenuMessageTick(boolean run)
// Handles menu message ticking in certain situations.
//
// Ported from Dr.Robotnik's Ring Racers!
//
void STAR_M_MenuMessageTick(boolean run)
{
	if (run)
	{
		if (menumessage.active)
		{
			M_MenuMessageTick();
		}
	}
}
