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
#include "../smkg-jukebox.h"

#include "../drrr/k_menu.h"

#include "../../console.h"
#include "../../d_main.h"
#include "../../g_game.h"
#include "../../i_time.h"
#include "../../i_system.h"
#include "../../m_random.h"
#include "../../m_misc.h"
#include "../../snake.h"
#include "../../v_video.h"
#include "../../z_zone.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

typedef enum
{
	QUITMSG = 0,
	QUITMSG1,
	QUITMSG2,
	QUITMSG3,
	QUITMSG4,
	QUITMSG5,
	QUITMSG6,
	QUITMSG7,

	QUIT2MSG,
	QUIT2MSG1,
	QUIT2MSG2,
	QUIT2MSG3,
	QUIT2MSG4,
	QUIT2MSG5,
	QUIT2MSG6,

	QUIT3MSG,
	QUIT3MSG1,
	QUIT3MSG2,
	QUIT3MSG3,
	QUIT3MSG4,
	QUIT3MSG5,
	QUIT3MSG6,
	NUM_VANILLA_QUITMESSAGES,

	TSOURDT3RD_QUITSMSG1 = NUM_VANILLA_QUITMESSAGES,
	TSOURDT3RD_QUITSMSG2,
	TSOURDT3RD_QUITSMSG3,

	TSOURDT3RD_AF_QUITMSG_START,
	TSOURDT3RD_AF_QUITMSG1 = TSOURDT3RD_AF_QUITMSG_START,
	TSOURDT3RD_AF_QUITMSG2,
	TSOURDT3RD_AF_QUITMSG3,
	TSOURDT3RD_AF_QUITMSG4,
	TSOURDT3RD_AF_QUITMSG_END,

	TSOURDT3RD_DYN_QUITSMSG1 = TSOURDT3RD_AF_QUITMSG_END,
	TSOURDT3RD_DYN_QUITSMSG2,
	TSOURDT3RD_DYN_QUITSMSG3,

	NUM_TSOURDT3RD_QUITMESSAGES
} tsourdt3rd_text_enum;

static INT32 tsourdt3rd_quitsounds[] =
{
	// srb2: holy shit we're changing things up!
	sfx_itemup, // Tails 11-09-99
	sfx_jump, // Tails 11-09-99
	sfx_skid, // Inu 04-03-13
	sfx_spring, // Tails 11-09-99
	sfx_pop,
	sfx_spdpad, // Inu 04-03-13
	sfx_wdjump, // Inu 04-03-13
	sfx_mswarp, // Inu 04-03-13
	sfx_splash, // Tails 11-09-99
	sfx_floush, // Tails 11-09-99
	sfx_gloop, // Tails 11-09-99
	sfx_s3k66, // Inu 04-03-13
	sfx_s3k6a, // Inu 04-03-13
	sfx_s3k73, // Inu 04-03-13
	sfx_chchng, // Tails 11-09-99

	// srb2kart: you ain't seen nothing yet
	sfx_kc2e,
	sfx_kc2f,
	sfx_cdfm01,
	//sfx_ddash,
	sfx_s3ka2,
	sfx_s3k49,
	//sfx_slip,
	//sfx_tossed,
	sfx_s3k7b,
	//sfx_itrolf,
	//sfx_itrole,
	sfx_cdpcm9,
	sfx_s3k4e,
	sfx_s259,
	sfx_3db06,
	sfx_s3k3a,
	//sfx_peel,
	sfx_cdfm28,
	sfx_s3k96,
	sfx_s3kc0s,
	sfx_cdfm39,
	//sfx_hogbom,
	sfx_kc5a,
	sfx_kc46,
	sfx_s3k92,
	sfx_s3k42,
	//sfx_kpogos,
	//sfx_screec,

	// tsourdt3rd: this is to go even further beyond!
	sfx_cdpcm3, // Star 04-11-23
};

const char *tsourdt3rd_quitmsgs[NUM_TSOURDT3RD_QUITMESSAGES];

INT16 tsourdt3rd_itemOn = 1; // menu item skull is on, Hack by Tails 09-18-2002
INT16 tsourdt3rd_skullAnimCounter = 1; // skull animation counter
struct menutransition_s menutransition; // Menu transition properties

INT32 menuKey = -1; // keyboard key pressed for menu
static gamecontrols_e gameKey = GC_NULL; // game key pressed for menu

// finish wipes between screens
boolean menuwipe = false;

// lock out further input in a tic when important buttons are pressed
// (in other words -- stop bullshit happening by mashing buttons in fades)
boolean tsourdt3rd_noFurtherInput = false;

// List mode types for different menus
tsourdt3rd_levellist_mode_t tsourdt3rd_levellistmode = TSOURDT3RD_LLM_NONE;

// lock-on technology for menus, allowing for unique menu features
static tsourdt3rd_menu_t *tsourdt3rd_allMenus = NULL;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_InitQuitMessages(const char **msg_table)
//
// Adds vanilla quit messages into our quit message table, while
//	also making space for our unique quit messages too
//
void TSoURDt3rd_M_InitQuitMessages(const char **msg_table)
{
	// Add vanilla quit messages here please
	if (!msg_table)
		return;

	for (INT32 i = QUITMSG; msg_table[i]; i++)
		tsourdt3rd_quitmsgs[i] = msg_table[i];

	// Add our custom quit messages now
	tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG1] = M_GetText("Every time you press 'Y',\nthe TSoURDt3rd Devs cry...\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG2] = M_GetText("Who do you think you are?\nItaly?\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG3] = M_GetText("You know, I have to say\nsomething cool here in order to\nclose the game...\n\n(Press 'Y' to quit)");

	tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG1] = M_GetText("Aww, was April Fools\ntoo much for you to handle?\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG2] = M_GetText("Happy April Fools!\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG3] = M_GetText("Wait!\nActivate Ultimate Mode!\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG4] = M_GetText("Could you not deal with\nall the pranks?\n\n(Press 'Y' to quit)");

	tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG1] = M_GetText("Hehe, you couldn't even make\nit past <insert stage name here>,\ncould you, silly?\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG2] = M_GetText("Wait, <insert player name here>!\nCome back! I need you!\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG3] = M_GetText("Come back!\nYou have more jukebox music to play!\n<insert jukebox music name somewhere>\n\n(Press 'Y' to quit)");
}

//
// const char *STAR_M_SelectQuitMessage(void)
// Generates a quit message for the user when asked to.
//
static void M_ReinitializeDynamicQuitMessages(void)
{
	char *maptitle = G_BuildMapTitle(gamemap);

	if (Playing() && gamestate == GS_LEVEL)
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG1] = va(M_GetText("Hehe, was \n%s\ntoo hard for you?\n\n(Press 'Y' to quit)"), maptitle);
	else
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG1] = M_GetText("Hehe, you couldn't even make\nit past the Title Screen,\ncould you, silly?\n\n(Press 'Y' to quit)");

	tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG2] = va(M_GetText("Wait, \x82%s\x80!\nCome back! I need you!\n\n(Press 'Y' to quit)"), TSoURDt3rd_ReturnUsername());

	if (tsourdt3rd_global_jukebox->curtrack)
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG3] = va(M_GetText("Come back!\nFinish listening to\n\x82%s\x80!\n\n(Press 'Y' to quit)"), tsourdt3rd_global_jukebox->curtrack->title);
	else
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG3] = M_GetText("Come back!\nYou have more jukebox music to play!\n\n(Press 'Y' to quit)");

	Z_Free(maptitle);
}

const char *TSoURDt3rd_M_GenerateQuitMessage(void)
{
	INT32 randomMessage = M_RandomKey(NUM_TSOURDT3RD_QUITMESSAGES);

	M_ReinitializeDynamicQuitMessages();
	if (!TSoURDt3rd_AprilFools_ModeEnabled())
	{
		// No April Fools messages when it's not April Fools!
		while (randomMessage >= TSOURDT3RD_AF_QUITMSG_START && randomMessage <= TSOURDT3RD_AF_QUITMSG_END)
			randomMessage = M_RandomKey(NUM_TSOURDT3RD_QUITMESSAGES);
	}

	// We pick index 0 which is language sensitive, or one at random,
	// between 1 and maximum number.
	// ------------------------------------------------------------//
	// ...no we don't! We haven't for ages!
	// But I'm leaving that comment in, unmodified, because it dates
	// ALL the way back to the original 1993 Doom source publication.
	// One act of kindness has far-reaching consequences for so many
	// people. It's a week until christmas as I'm writing this --
	// for those who read this, what act of kindness can you bring
	// to others? ~toast 181223 (Ported from DRRR by StarManiaKG, 060824)
	switch (randomMessage)
	{
		case QUITMSG4:
			S_StartSound(NULL, sfx_adderr);
			break;
		case QUITMSG5:
			S_StartSound(NULL, sfx_cgot);
			break;

		case QUIT2MSG1:
		case QUIT2MSG2:
			S_StartSound(NULL, sfx_pop);
			break;
		case QUIT2MSG3:
			S_StartSound(NULL, (M_RandomKey(1) ? sfx_supert : sfx_cgot));
			break;
		case QUIT2MSG4:
			S_StartSound(NULL, sfx_spin);
			break;
		case QUIT2MSG5:
			S_StartSound(NULL, sfx_cdpcm0+M_RandomKey(9));
			break;

		case QUIT3MSG2:
			S_StartSound(NULL, sfx_supert);
			break;
		case QUIT3MSG3:
			S_StartSound(NULL, sfx_s3k95);
			break;

		default:
			INT32 mrand = M_RandomKey(sizeof(tsourdt3rd_quitsounds) / sizeof(INT32));
			if (tsourdt3rd_quitsounds[mrand])
				S_StartSound(NULL, tsourdt3rd_quitsounds[mrand]);
			break;
	}

	// Return our random message and we're done :)
	return tsourdt3rd_quitmsgs[randomMessage];
}

//
// void TSoURDt3rd_M_AddNewMenu(tsourdt3rd_menu_t *newmenu, menu_t *menup)
// Adds unique TSoURDt3rd menu data, based on the new data and existing menu given.
//
void TSoURDt3rd_M_AddNewMenu(tsourdt3rd_menu_t *newmenu, menu_t *menup)
{
	if (!newmenu || !menup)
		return;

	newmenu->menu = menup;
	newmenu->next_menu = tsourdt3rd_allMenus;
	tsourdt3rd_allMenus = newmenu;
};

//
// void TSoURDt3rd_M_SetupNextMenu(menu_t *menudef)
//
// Wrapper for M_SetupNextMenu, allowing for setting up unique TSoURDt3rd menu data,
//	with some features taken from DRRR!
//
void TSoURDt3rd_M_SetupNextMenu(menu_t *menudef)
{
	tsourdt3rd_menu_t *new_menu = tsourdt3rd_allMenus;

	if (menudef == NULL)
		return;

	while (new_menu)
	{
		if (new_menu->menu == NULL)
			continue;
		if (new_menu->menu == menudef)
			break;
		new_menu = new_menu->next_menu;
	}
	gameKey = GC_NULL; // prevents weird double-tapping junk from happening

	if (tsourdt3rd_currentMenu && new_menu)
	{
#if 0
		if (!menutransition.transitions_enabled)
		{
			if (tsourdt3rd_currentMenu->transitionID == new_menu->transitionID
				&& tsourdt3rd_currentMenu->transitionTics)
			{
				menutransition.startmenu = tsourdt3rd_currentMenu;
				menutransition.endmenu = new_menu;

				menutransition.tics = 0;
				menutransition.dest = tsourdt3rd_currentMenu->transitionTics;
				menutransition.in = false;
				return; // Don't change menu yet, the transition will call this again
			}
			else if (menuactive)
			{
				menuwipe = true;
				F_WipeStartScreen();
				V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);
				F_WipeEndScreen();
				F_RunWipe(tsourdt3rd_wipedefs[tsourdt3rd_wipe_menu_toblack], false);
			}
		}
#endif

		if (tsourdt3rd_currentMenu->quitroutine)
		{
			// If you're going from a menu to itself, why are you running the quitroutine? You're not quitting it! -SH
			if (currentMenu != menudef && !tsourdt3rd_currentMenu->quitroutine())
				return; // we can't quit this menu (also used to set parameter from the menu)
		}

		if (new_menu->initroutine != NULL)
		{
			// Moving to a new menu, reinitialize.
			new_menu->initroutine();
		}
	}

	tsourdt3rd_currentMenu = new_menu;
	tsourdt3rd_itemOn = menudef->lastOn;

	//M_UpdateMenuBGImage(false);
	TSoURDt3rd_M_PlayMenuJam();

#ifdef HAVE_DISCORDSUPPORT
	if (menuactive && !Playing())
	{
		// currentMenu changed during menuactive
		DRPC_UpdatePresence();
	}
#endif
}

//
// void TSoURDt3rd_M_UpdateMenuCMD(UINT8 i)
// Updates the menu buttons needed for unique menu movement.
//
void TSoURDt3rd_M_UpdateMenuCMD(UINT8 i)
{
	menucmd[i].prev_dpad_ud = menucmd[i].dpad_ud;
	menucmd[i].prev_dpad_lr = menucmd[i].dpad_lr;

	menucmd[i].dpad_ud = 0;
	menucmd[i].dpad_lr = 0;

	menucmd[i].buttonsHeld = menucmd[i].buttons;
	menucmd[i].buttons = 0;

	if (gameKey == GC_SCREENSHOT) { menucmd[i].buttons |= MBT_SCREENSHOT; }
	if (gameKey == GC_RECORDGIF) { menucmd[i].buttons |= MBT_STARTMOVIE; }
	//if (gameKey == GC_RECORDLOSSLESS) { menucmd[i].buttons |= MBT_STARTLOSSLESS; }

	// Screenshot et al take priority
	if (menucmd[i].buttons != 0)
		return;

	if (gameKey == GC_FORWARD) { menucmd[i].dpad_ud--; }
	if (gameKey == GC_BACKWARD) { menucmd[i].dpad_ud++; }

	if (gameKey == GC_STRAFELEFT || gameKey == GC_TURNLEFT) { menucmd[i].dpad_lr--; }
	if (gameKey == GC_STRAFERIGHT || gameKey == GC_TURNRIGHT) { menucmd[i].dpad_lr++; }

	if (gameKey == GC_JUMP) { menucmd[i].buttons |= MBT_A; }
	if (gameKey == GC_SPIN) { menucmd[i].buttons |= MBT_B; }
	if (gameKey == GC_FIRE) { menucmd[i].buttons |= MBT_C; }

	if (gameKey == GC_FIRENORMAL) { menucmd[i].buttons |= MBT_X; }
	if (gameKey == GC_CUSTOM1) { menucmd[i].buttons |= MBT_Y; }
	if (gameKey == GC_CUSTOM2) { menucmd[i].buttons |= MBT_Z; }

	if (gameKey == GC_LOOKUP) { menucmd[i].buttons |= MBT_L; }
	if (gameKey == GC_LOOKDOWN) { menucmd[i].buttons |= MBT_R; }

	if (gameKey == GC_TOSSFLAG) { menucmd[i].buttons |= MBT_START; }	

	if (menucmd[i].dpad_ud == 0 && menucmd[i].dpad_lr == 0 && menucmd[i].buttons == 0)
	{
		// Reset delay count with no buttons.
		menucmd[i].delay = min(menucmd[i].delay, MENUMINDELAY);
		menucmd[i].delayCount = 0;
	}
}

//
// INT32 STAR_M_DoesMenuHaveKeyHandler(void)
// Returns whether or not the current menu option has specific routines for keys.
//
// Returns '1' if the menu option requires inputting text.
// Returns '2' if the menu option is actually a message box.
// Returns '3' if the menu option has its own way of handling keys.
//
INT32 STAR_M_DoesMenuHaveKeyHandler(void)
{
	menuitem_t *curmenuitem = &currentMenu->menuitems[tsourdt3rd_itemOn];

	if (!curmenuitem)
		return 0;
	else if ((curmenuitem->status & IT_CVARTYPE) == IT_CV_STRING)
		return 1;
	else if (curmenuitem->status == IT_MSGHANDLER)
		return 2;
	else if ((curmenuitem->status & IT_TYPE) == IT_KEYHANDLER)
		return 3;

	return 0;
}

//
// boolean STAR_M_Responder(INT32 *ch, event_t *ev)
// Unique menu input responder for TSoURDt3rd menus.
//
boolean STAR_M_Responder(INT32 *ch, event_t *ev)
{
	INT32 key, main_key_table[] = {
		KEY_UPARROW,
		KEY_DOWNARROW,
		KEY_LEFTARROW,
		KEY_RIGHTARROW,
		KEY_ENTER,
		KEY_ESCAPE
	};
	INT32 key_to_choice = (*ch);

	gamecontrols_e gc, gc_table[] = {
		GC_SCREENSHOT,
		GC_RECORDGIF,

		GC_FORWARD,
		GC_BACKWARD,
		GC_STRAFELEFT, GC_TURNLEFT,
		GC_STRAFERIGHT, GC_TURNRIGHT,
		GC_JUMP,
		GC_SPIN, GC_SYSTEMMENU,

		GC_FIRENORMAL,
		GC_CUSTOM1,
		GC_CUSTOM2,

		GC_LOOKUP,
		GC_LOOKDOWN,

		GC_TOSSFLAG,
		GC_NULL
	};

#if 1
	/// \todo make this work perfectly
	if (menutyping.active || (titlemapinaction && tsourdt3rd_levellistmode == -1) || (Playing() && (!netgame && !multiplayer)))
	{
		// update keys current state
		G_MapEventsToControls(ev);
	}
#endif

	key_to_choice = (*ch);
	if (ev->type == ev_keydown && ev->key > 0 && ev->key < NUMKEYS)
	{
		// Record keyboard presses
		menuKey = ev->key;
	}

	// For control mapping, we take the WHOLE EVENT for convenience.
	if (optionsmenu.bindtimer)
	{
		TSoURDt3rd_M_Controls_MapProfileControl(ev);
		return true;
	}

	// Translate menu keys, or in-game keys too, to work in our menus
	switch (key_to_choice)
	{
		case KEY_UPARROW:
			gameKey = GC_FORWARD;
			break;
		case KEY_DOWNARROW:
			gameKey = GC_BACKWARD;
			break;
		case KEY_LEFTARROW:
			gameKey = GC_STRAFELEFT;
			break;
		case KEY_RIGHTARROW:
			gameKey = GC_STRAFERIGHT;
			break;
		case KEY_ENTER:
			gameKey = GC_JUMP;
			break;
		case KEY_ESCAPE:
			gameKey = GC_SPIN;
			break;

		default:
		{
			for (gc = 0; gc_table[gc]; gc++)
				if (PLAYERINPUTDOWN(1, gc_table[gc]))
					break;

			gameKey = gc_table[gc];
			break;
		}
	}

	CONS_Printf("gamekey is %d, menukey is %d, other key is %d\n", gameKey, menuKey, key_to_choice);
	if (tsourdt3rd_currentMenu == NULL)
	{
		// We're not in our unique menu system!
		// Why run anything else unique then?
		// Let's just clean up some things and move on...

		// if we're about to start typing,
		// ignore ev_keydown events if the key maps to a character, since
		// the ev_text event will follow immediately after in that case.
		if (I_GetTextInputMode() && ev->type == ev_keydown)
		{
			if (key_to_choice >= 32 && key_to_choice <= 127)
				return false;
		}

		// Translate our unique menu controls to vanilla menus too!
		for (key = 0; main_key_table[key]; key++)
		{
			if (key_to_choice == -1)
				break;
			if (main_key_table[key] == key_to_choice)
				return false;
		}

		switch (gameKey)
		{
			case GC_FORWARD:
				key_to_choice = KEY_UPARROW;
				break;
			case GC_BACKWARD:
				key_to_choice = KEY_DOWNARROW;
				break;
			case GC_STRAFELEFT:
			case GC_TURNLEFT:
				key_to_choice = KEY_LEFTARROW;
				break;
			case GC_STRAFERIGHT:
			case GC_TURNRIGHT:
				key_to_choice = KEY_RIGHTARROW;
				break;
			case GC_JUMP:
				key_to_choice = KEY_ENTER;
				break;
			case GC_SPIN:
			case GC_SYSTEMMENU:
				key_to_choice = KEY_ESCAPE;
				break;
			default:
				break;
		}
		CONS_Printf("key_to_choice is %d\n", key_to_choice);
		(*ch) = key_to_choice;

		return false;
	}

	return (I_GetTextInputMode() || gameKey);
}

//
// void TSoURDt3rd_M_PlayMenuJam(void)
// Plays menu music.
//
// Inspired by M_PlayMenuJam from DRRR!
//
void TSoURDt3rd_M_PlayMenuJam(void)
{
	tsourdt3rd_menu_t *refMenu = (menuactive ? tsourdt3rd_currentMenu : tsourdt3rd_currentMenu->prev_menu);
	const boolean profilemode = (optionsmenu.profilemenu && !optionsmenu.resetprofilemenu);

	if (tsourdt3rd_currentMenu == NULL || tsourdt3rd_global_jukebox->playing)
		return;

	if (!profilemode && Playing())
	{
		if (optionsmenu.resetprofilemenu)
			S_StopMusic();
		return;
	}

	if (refMenu != NULL && refMenu->music != NULL)
	{
		if (refMenu->music[0] == '.' && refMenu->music[1] == '\0')
		{
			S_StopMusic();
			return;
		}

		if (strcmp(refMenu->music, S_MusicName()))
			M_ChangeMenuMusic(refMenu->music, true);
		return;
	}

	if (!strcmp("_inter", S_MusicName()))
		return;
	M_ChangeMenuMusic("_inter", true);
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
// void TSoURDt3rd_M_ClearMenus(boolean callexitmenufunc)
// Clears the screen of any menus open, running any quit routines if necessary.
//
void TSoURDt3rd_M_ClearMenus(boolean callexitmenufunc)
{
	if (!menuactive)
		return;

	if (tsourdt3rd_currentMenu && tsourdt3rd_currentMenu->quitroutine && callexitmenufunc && !tsourdt3rd_currentMenu->quitroutine())
		return; // we can't quit this menu (also used to set parameter from the menu)

	M_AbortVirtualKeyboard();
	menumessage.active = false;
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
		if (cv == &cv_maxsend)
			choice *= 512;
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
	consvar_t *cvar = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
	if (replace)
		CV_Set(cvar, replace);
	return cvar->string;
}

//
// void TSoURDt3rd_M_UpdateItemOn(void)
// If the item we're on needs text inputs, update the current text mode then!
//
void TSoURDt3rd_M_UpdateItemOn(void)
{
	I_SetTextInputMode((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_STRING ||
		(currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_KEYHANDLER);
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

	TSoURDt3rd_M_UpdateItemOn();
	//M_UpdateMenuBGImage(false);

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

	TSoURDt3rd_M_UpdateItemOn();
	//M_UpdateMenuBGImage(false);

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
					(tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1 ?
						tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1 :
						MAXSTRINGLENGTH
					),
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
// void STAR_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter, INT32 levellistmode)
// Handles menu ticking.
//
// Inspired by M_Ticker() from Dr.Robotnik's Ring Racers!
//
void STAR_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter, INT32 levellistmode)
{
	INT16 ms = 0, menu_statuses[] = {
		IT_DISPLAY,
		IT_TYPE,
		-1
	};
	INT16 mt = 0, menu_types[2][3] = {
		[0] = {
			IT_SPACE,
			-1
		},
		[1] = {
			IT_HEADERTEXT,
			-1
		}
	};

	tsourdt3rd_noFurtherInput = *input;
	tsourdt3rd_skullAnimCounter = skullAnimCounter;
	tsourdt3rd_levellistmode = (tsourdt3rd_levellist_mode_t)levellistmode;

	if (!menuactive)
		return;

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
			menutransition.transitions_enabled = true;

			if (tsourdt3rd_currentMenu != menutransition.endmenu)
			{
				if (menutransition.startmenu->transitionID == menutransition.endmenu->transitionID
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

				TSoURDt3rd_M_SetupNextMenu(menutransition.endmenu->menu);
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

#if 0
	// The curent item can be disabled,
	// so this code will scroll through the menu,
	// until an enabled item can be found
	while (menu_statuses[ms] != -1)
	{
		if (menu_types[ms][mt] == -1)
		{
			ms++;
			mt = 0;
		}

		if ((currentMenu->menuitems[tsourdt3rd_itemOn].status & menu_statuses[ms]) & menu_types[ms][mt++])
		{
			ms = mt = 0;
			tsourdt3rd_itemOn++;
			continue;
		}
	}
#endif

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
