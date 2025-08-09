// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_func.c
/// \brief Unique TSoURDt3rd menu routines and structures

#include "smkg-m_sys.h"
#include "../smkg_g_inputs.h"
#include "../core/smkg-s_audio.h"
#include "../core/smkg-s_jukebox.h"

#include "../../console.h"
#include "../../d_main.h"
#include "../../g_game.h"
#include "../../i_time.h"
#include "../../i_system.h"
#include "../../m_random.h"
#include "../../m_misc.h"
#include "../../v_video.h"
#include "../../z_zone.h"

#include "../../netcode/d_netfil.h"

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
	TSOURDT3RD_QUITSMSG4,
	TSOURDT3RD_QUITSMSG5,

	TSOURDT3RD_QUITFBGMSG1,
	TSOURDT3RD_QUITFBGMSG2,
	TSOURDT3RD_QUITFBGMSG3,
	TSOURDT3RD_QUITFBGMSG4,
	TSOURDT3RD_QUITFBGMSG5,
	TSOURDT3RD_QUITFBGMSG6,
	TSOURDT3RD_QUITFBGMSG7,

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
INT16 vanilla_itemOn; // vanilla menu item skill is on, Hack by StarManiaKG on unknown date

tsourdt3rd_menu_t *tsourdt3rd_currentMenu = NULL; // pointer to the current unique menu

menu_t *vanilla_prevMenu = NULL; // pointer to the previous vanilla menu
tsourdt3rd_menu_t *tsourdt3rd_prevMenu = NULL; // pointer to the previous unique menu

INT16 tsourdt3rd_skullAnimCounter = 1; // skull animation counter
struct menutransition_s menutransition; // Menu transition properties

INT32 menuKey = -1; // keyboard key pressed for menu
INT32 gameKey = GC_NULL; // game key pressed for menu

// finish wipes between screens
boolean menuwipe = false;

// lock out further input in a tic when important buttons are pressed
// (in other words -- stop bullshit happening by mashing buttons in fades)
boolean tsourdt3rd_noFurtherInput = false;

// List mode types for different menus
tsourdt3rd_levellist_mode_t tsourdt3rd_levellistmode = TSOURDT3RD_LLM_CREATESERVER;

// Menu controller
menucmd_t menucmd[MAXSPLITSCREENPLAYERS];

// Registered list of menu keys we can use
typedef struct menukeylist_s {
	INT32 key;
	boolean global;
} menukeylist_t;
INT32 key_table[] = {
	KEY_UPARROW,
	KEY_DOWNARROW,
	KEY_LEFTARROW,
	KEY_RIGHTARROW,
	KEY_ENTER,
	KEY_ESCAPE,
	KEY_NULL
};
menukeylist_t menukey_list[] = {
	// -- Overriders
	// Recording
	{ GC_SCREENSHOT, true },
	{ GC_RECORDGIF, true },
	//{ GC_RECORDLOSSLESS, true },
	// Jukebox
	{ JB_OPENJUKEBOX, true },
	{ JB_INCREASEMUSICSPEED, true },
	{ JB_DECREASEMUSICSPEED, true },
	{ JB_INCREASEMUSICPITCH, true },
	{ JB_DECREASEMUSICPITCH, true },
	{ JB_PLAYMOSTRECENTTRACK, true },
	{ JB_STOPJUKEBOX, true },
	// -- Movement
	{ GC_FORWARD, false },
	{ GC_BACKWARD, false },
	{ GC_STRAFELEFT, false }, { GC_TURNLEFT, false },
	{ GC_STRAFERIGHT, false }, { GC_TURNRIGHT, false },
	{ GC_JUMP, false },
	{ GC_SPIN, false }, { GC_SYSTEMMENU, false },
	// -- Misc
	{ GC_FIRENORMAL, false },
	{ GC_CUSTOM1, false }, { GC_CUSTOM2, false },
	{ GC_LOOKUP, false },
	{ GC_LOOKDOWN, false },
	{ GC_TOSSFLAG, false },
	// -- end
	{ GC_NULL, false }
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_InitQuitMessages(const char **msg_table)
//
// Adds vanilla quit messages into our quit message table, while
//	also making space for our unique quit messages too.
//
void TSoURDt3rd_M_InitQuitMessages(const char **msg_table)
{
	static boolean init_vanilla_static_messages = false, init_static_messages = false;
	char *maptitle = G_BuildMapTitle(gamemap);

	// Add vanilla quit messages here please
	if (init_vanilla_static_messages == false)
	{
		if (!msg_table)
			return;
		for (INT32 i = QUITMSG; msg_table[i]; i++)
			tsourdt3rd_quitmsgs[i] = msg_table[i];
		init_vanilla_static_messages = true;
	}

	// Now, add our custom quit messages
	if (init_static_messages == false)
	{
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG1] = M_GetText("Every time you press 'Y',\nthe TSoURDt3rd Devs cry...\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG2] = M_GetText("Who do you think you are?\nItaly?\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG3] = M_GetText("You know, I have to say\nsomething cool here in order to\nclose the game...\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG4] = M_GetText("There's always modding.\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITSMSG5] = M_GetText("It's 'hear', not 'here'.\n\nHuh?\n\nOh, sorry,\njust correcting an error\nI made in v4.0.\n\n(Press 'Y' to quit)");

		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG1] = M_GetText("\x82 Flashback Guy says:\x80\n\nSomeone's getting ticked off with Black Hole Zone.\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG2] = M_GetText("\x82 Flashback Guy says:\x80\n\nCome on, you're SO close to getting that emblem!\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG3] = M_GetText("\x82 Flashback Guy says:\x80\n\nOh sweet, you're finally going to touch grass?\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG4] = M_GetText("\x82 Flashback Guy says:\x80\n\nSonic never made it back after the Black Rock incident.\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG5] = M_GetText("\x82 Flashback Guy says:\x80\n\nDon't let this make you forget\nthe fact that Mr. Krabs sold\nSpongeBob's soul for 62 cents.\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG6] = M_GetText("\x82 Flashback Guy says:\x80\n\nHear me out:\nWhat if you used Tails?\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_QUITFBGMSG7] = M_GetText("\x82 Flashback Guy says:\x80\n\nI guess this isn't happenin'.\n\n(Press 'Y' to quit)");

		tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG1] = M_GetText("Aww, was April Fools\ntoo much for you to handle?\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG2] = M_GetText("Happy April Fools!\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG3] = M_GetText("Wait!\nActivate Ultimate Mode!\n\n(Press 'Y' to quit)");
		tsourdt3rd_quitmsgs[TSOURDT3RD_AF_QUITMSG4] = M_GetText("Could you not deal with\nall the pranks?\n\n(Press 'Y' to quit)");

		init_static_messages = true;
	}

	// Re-initialize the dynamic quit messages
	if (Playing() && gamestate == GS_LEVEL)
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG1] = va(M_GetText("Hehe, was \n%s\ntoo hard for you?\n\n(Press 'Y' to quit)"), maptitle);
	else
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG1] = M_GetText("Hehe, you couldn't even make\nit past the Title Screen,\ncould you, silly?\n\n(Press 'Y' to quit)");
	tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG2] = va(M_GetText("Wait, \x82%s\x80!\nCome back! I need you!\n\n(Press 'Y' to quit)"), TSoURDt3rd_ReturnUsername());
	if (TSoURDt3rd_Jukebox_IsPlaying())
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG3] = va(M_GetText("Come back!\nFinish listening to\n\x82%s\x80!\n\n(Press 'Y' to quit)"), tsourdt3rd_global_jukebox->curtrack->title);
	else
		tsourdt3rd_quitmsgs[TSOURDT3RD_DYN_QUITSMSG3] = M_GetText("Come back!\nYou have more jukebox music to play!\n\n(Press 'Y' to quit)");

	Z_Free(maptitle);
}

//
// const char *TSoURDt3rd_M_GenerateQuitMessage(void)
// Generates a quit message for the user when asked to.
//
const char *TSoURDt3rd_M_GenerateQuitMessage(void)
{
	INT32 randomMessage = M_RandomKey(NUM_TSOURDT3RD_QUITMESSAGES);
	INT32 mrand;

	TSoURDt3rd_M_InitQuitMessages(NULL);

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
	// to others? ~toast 181223 (Ported from Dr.Robotnik's Ring Racers, by StarManiaKG, 060824)
	switch (randomMessage)
	{
		case QUITMSG4:
			S_StartSound(NULL, sfx_adderr);
			break;
		case QUITMSG5:
			S_StartSound(NULL, sfx_cgot);
			break;
		case QUIT2MSG1: case QUIT2MSG2:
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
			mrand = M_RandomKey(sizeof(tsourdt3rd_quitsounds) / sizeof(INT32));
			if (tsourdt3rd_quitsounds[mrand])
				S_StartSound(NULL, tsourdt3rd_quitsounds[mrand]);
			break;
	}

	return tsourdt3rd_quitmsgs[randomMessage];
}

//
// boolean TSoURDt3rd_M_OverwriteResponder(event_t *event)
// If returned true, prevents any given responder from eating inputs until after the current situation is handled.
//
boolean TSoURDt3rd_M_OverwriteResponder(event_t *event)
{
	INT32 key = event->key;
	boolean eat_inputs = false;

	TSoURDt3rd_M_Responder(&key, event);

	if (!menuactive)
	{
		if (menumessage.active)
			eat_inputs = true;
	}
	else if (tsourdt3rd_global_jukebox && tsourdt3rd_global_jukebox->in_menu)
		eat_inputs = true;

	if (eat_inputs && menumessage.active)
		TSoURDt3rd_M_HandleMenuMessage();
	return eat_inputs;
}

//
// INT32 TSoURDt3rd_M_DoesMenuHaveKeyHandler(void)
//
// Checks whether or not the current menu option has specific routines for keys.
// Returns a non-zero value if so, or a below-zero value otherwise
//
// Returns '1' if the menu option requires inputting text.
// Returns '2' if the menu option is actually a message box.
// Returns '3' if the menu option has its own way of handling keys.
// Returns a value greater than or equal to '4' if the menu option is TSoURDt3rd unique.
//
INT32 TSoURDt3rd_M_DoesMenuHaveKeyHandler(void)
{
	menuitem_t *curmenuitem = &currentMenu->menuitems[tsourdt3rd_itemOn];

	if (menutyping.active)
		return 4;
	else if (menumessage.active)
		return 5;

	if (!curmenuitem || !menuactive)
		return -1;

	if ((curmenuitem->status & IT_CVARTYPE) == IT_CV_STRING)
		return 1;
	else if (curmenuitem->status == IT_MSGHANDLER)
		return 2;
	else if ((curmenuitem->status & IT_TYPE) == IT_KEYHANDLER)
		return 3;

	return -1;
}

//
// void TSoURDt3rd_M_SetMenuDelay(UINT8 i)
// Sets a menu delay, where the menu can't be moved for a certain amount of time.
//
void TSoURDt3rd_M_SetMenuDelay(UINT8 i)
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

//
// Returns true if we press the button specified
//
boolean TSoURDt3rd_M_MenuButtonPressed(UINT8 pid, UINT32 bt)
{
	if (menucmd[pid].buttonsHeld & bt)
	{
		return false;
	}

	return !!(menucmd[pid].buttons & bt);
}
boolean TSoURDt3rd_M_MenuButtonHeld(UINT8 pid, UINT32 bt)
{
	return !!(menucmd[pid].buttons & bt);
}

//
// Returns true if we press the confirmation button
//
boolean TSoURDt3rd_M_MenuConfirmPressed(UINT8 pid)
{
	 return TSoURDt3rd_M_MenuButtonPressed(pid, MBT_A);
}
boolean TSoURDt3rd_M_MenuConfirmHeld(UINT8 pid)
{
	 return TSoURDt3rd_M_MenuButtonHeld(pid, MBT_A);
}

//
// Returns true if we press the Cancel button
//
boolean TSoURDt3rd_M_MenuBackPressed(UINT8 pid)
{
	 return (TSoURDt3rd_M_MenuButtonPressed(pid, MBT_B) || TSoURDt3rd_M_MenuButtonPressed(pid, MBT_X));
}
boolean TSoURDt3rd_M_MenuBackHeld(UINT8 pid)
{
	 return (TSoURDt3rd_M_MenuButtonHeld(pid, MBT_B) || TSoURDt3rd_M_MenuButtonHeld(pid, MBT_X));
}

//
// Returns true if we press the tertiary option button (C)
//
boolean TSoURDt3rd_M_MenuExtraPressed(UINT8 pid)
{
	 return TSoURDt3rd_M_MenuButtonPressed(pid, MBT_C);
}
boolean TSoURDt3rd_M_MenuExtraHeld(UINT8 pid)
{
	 return TSoURDt3rd_M_MenuButtonHeld(pid, MBT_C);
}

//
// static boolean M_Sys_DoTransition(boolean setup_menu, tsourdt3rd_menu_t *tsourdt3rd_menudef, menu_t *menudef)
// Performs the cool transition process for our menus.
//
static boolean M_Sys_DoTransition(boolean setup_menu, tsourdt3rd_menu_t *tsourdt3rd_menudef, menu_t *menudef)
{
	const boolean in_menu = menuactive;
	tsourdt3rd_menu_t *trans_start;
	tsourdt3rd_menu_t *trans_end;
	INT16 trans_tics;

	if (setup_menu)
	{
		trans_start = tsourdt3rd_currentMenu;
		trans_end = tsourdt3rd_menudef;
		trans_tics = trans_start->transitionTics;
	}
	else
	{
		trans_start = menutransition.startmenu;
		trans_end = menutransition.endmenu;
		trans_tics = menutransition.endmenu->transitionTics;
	}

	if (trans_start && trans_end)
	{
		if ((trans_start->transitionID == trans_end->transitionID) && trans_tics)
		{
			if (setup_menu)
			{
				menutransition.startmenu = tsourdt3rd_currentMenu;
				menutransition.endmenu = tsourdt3rd_menudef;
				menutransition.vanilla_endmenu = menudef;
				menutransition.tics = 0;
				menutransition.dest = tsourdt3rd_currentMenu->transitionTics;
				menutransition.in = false;
			}
			else
			{
				menutransition.tics = menutransition.endmenu->transitionTics;
				menutransition.dest = 0;
				menutransition.in = true;
			}
			return true;
		}
	}

	if (in_menu)
	{
		if (!setup_menu)
		{
			memset(&menutransition, 0, sizeof(menutransition));
		}
		menuwipe = true;
		F_WipeStartScreen();
		V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31);
		F_WipeEndScreen();
		F_RunWipe(tsourdt3rd_wipedefs[tsourdt3rd_wipe_menu_toblack], false);
		return true;
	}

	// Complete our transition once and for all!
	return false;
}

//
// void TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_menu_t *tsourdt3rd_menudef, menu_t *menudef, boolean notransition)
//
// Wrapper for M_SetupNextMenu, allowing for setting up unique TSoURDt3rd menu data,
//	with some features taken from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_menu_t *tsourdt3rd_menudef, menu_t *menudef, boolean notransition)
{
	const boolean in_menu = menuactive;

	if (tsourdt3rd_currentMenu != NULL && tsourdt3rd_menudef != NULL && menudef != NULL)
	{
		if (!notransition)
		{
			M_Sys_DoTransition(true, tsourdt3rd_menudef, menudef);
			if ((tsourdt3rd_currentMenu->transitionID == tsourdt3rd_menudef->transitionID) && tsourdt3rd_currentMenu->transitionTics)
			{
				// Don't change menu yet, the transition will call this again
				return;
			}
		}
	}

	if (tsourdt3rd_currentMenu != NULL)
	{
		if (tsourdt3rd_currentMenu->quitroutine)
		{
			// If you're going from a menu to itself, why are you running the quitroutine? You're not quitting it! -SH
			if (tsourdt3rd_currentMenu != tsourdt3rd_menudef && !tsourdt3rd_currentMenu->quitroutine())
				return; // -- we can't quit this menu (also used to set parameter from the menu)
		}
	}

	if (tsourdt3rd_menudef != NULL)
	{
		if (tsourdt3rd_menudef->initroutine != NULL)
		{
			// Moving to a new menu, reinitialize.
			tsourdt3rd_menudef->initroutine();
		}
	}

	// Set the menu!
	tsourdt3rd_prevMenu = (in_menu ? tsourdt3rd_currentMenu : NULL);
	vanilla_prevMenu = (in_menu ? currentMenu : NULL);
	tsourdt3rd_currentMenu = tsourdt3rd_menudef;

	if (menudef == NULL)
	{
		// We need at least a regular menu definition to move into!
		M_ClearMenus((currentMenu != NULL));
		return;
	}

	if (!in_menu)
	{
		// We should probably make sure that the menu is opened too...
		M_StartControlPanel();
	}
	tsourdt3rd_itemOn = menudef->lastOn;
	vanilla_itemOn = tsourdt3rd_itemOn;

	if (tsourdt3rd_itemOn >= menudef->numitems)
	{
		// in case of weirdness...
		tsourdt3rd_itemOn = menudef->numitems - 1;
	}

	if (((menudef->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) & IT_SPACE))
	{
		// The curent item can be disabled, so this code will go until an enabled item is found...
		for (INT16 i = 0; i < menudef->numitems; i++)
		{
			if (!((menudef->menuitems[i].status & IT_TYPE) & IT_SPACE))
			{
				tsourdt3rd_itemOn = i;
				break;
			}
		}
	}

	M_SetupNextMenu(menudef);
	//M_UpdateMenuBGImage(false);
	TSoURDt3rd_M_PlayMenuJam();

	// Set the menu delay!
	if (tsourdt3rd_prevMenu == NULL)
	{
		// We're on a Vanilla Menu, so let's set some delay stuff right quick...
		const UINT8 pid = 0;
		TSoURDt3rd_M_SetMenuDelay(pid);
	}

#ifdef HAVE_DISCORDSUPPORT
	if (in_menu)
	{
		// currentMenu changed during menuactive
		DISC_UpdatePresence();
	}
#endif
}

//
// void TSoURDt3rd_M_UpdateMenuCMD(UINT8 i)
// Updates the menu buttons needed for unique menu movement and routines.
//
void TSoURDt3rd_M_UpdateMenuCMD(UINT8 i)
{
	menukeylist_t thisMenuList = menukey_list[0];
	INT32 gc = GC_NULL, thisGameKey = GC_NULL;
	const INT32 currentGameKey = gameKey;

	menucmd[i].prev_dpad_ud = menucmd[i].dpad_ud;
	menucmd[i].prev_dpad_lr = menucmd[i].dpad_lr;

	menucmd[i].dpad_ud = 0;
	menucmd[i].dpad_lr = 0;

	menucmd[i].buttonsHeld = menucmd[i].buttons;
	menucmd[i].buttons = 0;

#if 0
	while (thisMenuList.key)
	{
		thisGameKey = thisMenuList.key;
		if (currentGameKey == thisGameKey) break;
		if (thisMenuList.global && STAR_G_KeyPressed(0, thisGameKey)) break;
		thisMenuList = menukey_list[++gc];
	}
#else
	(void)thisMenuList;
	(void)gc;
	thisGameKey = currentGameKey;
#endif

	// Check extra gamekeys first
	if (thisGameKey == GC_SCREENSHOT) { menucmd[i].buttons |= MBT_SCREENSHOT; }
	if (thisGameKey == GC_RECORDGIF) { menucmd[i].buttons |= MBT_STARTMOVIE; }
	//if (thisGameKey == GC_RECORDLOSSLESS) { menucmd[i].buttons |= MBT_STARTLOSSLESS; }

	if (thisGameKey == JB_OPENJUKEBOX) { menucmd[i].buttons |= MBT_JUKEBOX_OPEN; }
	if (thisGameKey == JB_INCREASEMUSICSPEED) { menucmd[i].buttons |= MBT_JUKEBOX_INCREASESPEED; }
	if (thisGameKey == JB_DECREASEMUSICSPEED) { menucmd[i].buttons |= MBT_JUKEBOX_DECREASESPEED; }
	if (thisGameKey == JB_INCREASEMUSICPITCH) { menucmd[i].buttons |= MBT_JUKEBOX_INCREASEPITCH; }
	if (thisGameKey == JB_DECREASEMUSICPITCH) { menucmd[i].buttons |= MBT_JUKEBOX_DECREASEPITCH; }
	if (thisGameKey == JB_PLAYMOSTRECENTTRACK) { menucmd[i].buttons |= MBT_JUKEBOX_PLAYRECENT; }
	if (thisGameKey == JB_STOPJUKEBOX) { menucmd[i].buttons |= MBT_JUKEBOX_CLOSE; }

#if 0
	/// \todo STAR TODO NOTE or something, rework this
	if (tsourdt3rd_noFurtherInput == false)
	{
		if (TSoURDt3rd_M_DoesMenuHaveKeyHandler() == -1 && tsourdt3rd_global_jukebox != NULL)
		{
			if (/*thisGameKey == JB_OPENJUKEBOX || */STAR_G_KeyPressed(0, JB_OPENJUKEBOX))
			{
				// A shortcut to open the Jukebox menu.
				TSoURDt3rd_M_Jukebox_Init(op_extras_jukebox);
			}
			if (/*thisGameKey == JB_INCREASEMUSICSPEED || */STAR_G_KeyPressed(0, JB_INCREASEMUSICSPEED))
			{
				// Increase the speed of the jukebox track
				if (S_GetSpeedMusic() > 0.0f)
					TSoURDt3rd_M_ChangeCvarDirect(-1, (1)*(1.0f/16.0f), &cv_tsourdt3rd_jukebox_speed);
				else
					S_StartSound(NULL, sfx_lose);
			}
			if (/*thisGameKey == JB_DECREASEMUSICSPEED || */STAR_G_KeyPressed(0, JB_DECREASEMUSICSPEED))
			{
				// Decrease the speed of the jukebox track
				if (S_GetSpeedMusic() > 0.0f)
					TSoURDt3rd_M_ChangeCvarDirect(-1, (-1)*(1.0f/16.0f), &cv_tsourdt3rd_jukebox_speed);
				else
					S_StartSound(NULL, sfx_lose);
			}
			if (/*thisGameKey == JB_INCREASEMUSICPITCH || */STAR_G_KeyPressed(0, JB_INCREASEMUSICPITCH))
			{
				// Increase the pitch of the jukebox track
				if (TSoURDt3rd_Jukebox_IsPlaying() && (S_GetPitchMusic() < 0.0f))
					S_StartSound(NULL, sfx_lose);
				else
					TSoURDt3rd_M_ChangeCvarDirect(-1, (1)*(1.0f/16.0f), &cv_tsourdt3rd_jukebox_pitch);
			}
			if (/*thisGameKey == JB_DECREASEMUSICPITCH || */STAR_G_KeyPressed(0, JB_DECREASEMUSICPITCH))
			{
				// Decrease the pitch of the jukebox track
				if (TSoURDt3rd_Jukebox_IsPlaying() && (S_GetPitchMusic() < 0.0f))
					S_StartSound(NULL, sfx_lose);
				else
					TSoURDt3rd_M_ChangeCvarDirect(-1, (-1)*(1.0f/16.0f), &cv_tsourdt3rd_jukebox_pitch);
			}
			if (/*thisGameKey == JB_PLAYMOSTRECENTTRACK || */STAR_G_KeyPressed(0, JB_PLAYMOSTRECENTTRACK))
			{
				// Replay the most recent jukebox track
				TSoURDt3rd_Jukebox_Play(NULL);
			}
			if (/*thisGameKey == JB_STOPJUKEBOX || */STAR_G_KeyPressed(0, JB_STOPJUKEBOX))
			{
				// Stop and reset the jukebox
				if (!TSoURDt3rd_Jukebox_IsPlaying())
				{
					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "Nothing is currently playing in the jukebox!\n");
					S_StartSound(NULL, sfx_lose);
				}
				else
				{
					S_StopSounds();
					S_StopMusic();
					S_StartSound(NULL, sfx_skid);
					TSoURDt3rd_S_RefreshMusic();
				}
			}
		}
	}
#endif

	// Screenshot et al take priority
	if (menucmd[i].buttons != 0)
		return;

	// Check main gamekeys now
	if (thisGameKey == GC_FORWARD) { menucmd[i].dpad_ud--; }
	if (thisGameKey == GC_BACKWARD) { menucmd[i].dpad_ud++; }

	if (thisGameKey == GC_STRAFELEFT || thisGameKey == GC_TURNLEFT) { menucmd[i].dpad_lr--; }
	if (thisGameKey == GC_STRAFERIGHT || thisGameKey == GC_TURNRIGHT) { menucmd[i].dpad_lr++; }

	if (thisGameKey == GC_JUMP) { menucmd[i].buttons |= MBT_A; }
	if (thisGameKey == GC_SPIN) { menucmd[i].buttons |= MBT_B; }
	if (thisGameKey == GC_FIRE) { menucmd[i].buttons |= MBT_C; }

	if (thisGameKey == GC_FIRENORMAL) { menucmd[i].buttons |= MBT_X; }
	if (thisGameKey == GC_CUSTOM1) { menucmd[i].buttons |= MBT_Y; }
	if (thisGameKey == GC_CUSTOM2) { menucmd[i].buttons |= MBT_Z; }

	if (thisGameKey == GC_LOOKUP) { menucmd[i].buttons |= MBT_L; }
	if (thisGameKey == GC_LOOKDOWN) { menucmd[i].buttons |= MBT_R; }

	if (thisGameKey == GC_TOSSFLAG) { menucmd[i].buttons |= MBT_START; }

	// If applicable, reset delay count with no buttons.
	if (menucmd[i].dpad_ud == 0 && menucmd[i].dpad_lr == 0 && menucmd[i].buttons == 0)
	{
		menucmd[i].delay = min(menucmd[i].delay, MENUMINDELAY);
		menucmd[i].delayCount = 0;
	}
}

//
// boolean TSoURDt3rd_M_Responder(INT32 *ch, event_t *ev)
// Extended menu input responder for SRB2 and TSoURDt3rd menus.
//
boolean TSoURDt3rd_M_Responder(INT32 *ch, event_t *ev)
{
	const UINT8 pid = 0;
	INT32 key = 0, gc = 0;

	boolean in_event_eatable_situation = (menutyping.active || titlemapinaction || (menuactive && !menucmd[pid].delay) || tsourdt3rd_snake || optionsmenu.bindtimer);
	boolean hold_event_hostage = (tsourdt3rd_snake || optionsmenu.bindtimer);

	if (in_event_eatable_situation)
	{
		// update keys current state
		G_MapEventsToControls(ev);
		if (optionsmenu.bindtimer)
		{
			// For control mapping, we take the WHOLE EVENT for convenience.
			TSoURDt3rd_M_Controls_MapProfileControl(ev);
			memset(gamekeydown, 0, sizeof(gamekeydown)); // Reset the keys so the game doesn't bug out.
			return true;
		}
	}

	// Emulate and convert menu inputs, please.
	if (ev->type == ev_keydown || ev->type == ev_text)
	{
		menuKey = ev->key; // -- Record keyboard presses
		if (ev->type == ev_keydown && ev->key > 0)
		{
			// -- Translate menu keys and in-game keys to work in our menus
			switch (menuKey)
			{
				case KEY_UPARROW:
				case KEY_HAT1:
				case KEY_JOY1 + 11:
					gameKey = GC_FORWARD;
					break;
				case KEY_DOWNARROW:
				case KEY_HAT1 + 1:
				case KEY_JOY1 + 12:
					gameKey = GC_BACKWARD;
					break;
				case KEY_LEFTARROW:
				case KEY_HAT1 + 2:
				case KEY_JOY1 + 13:
					gameKey = GC_STRAFELEFT;
					break;
				case KEY_RIGHTARROW:
				case KEY_HAT1 + 3:
				case KEY_JOY1 + 14:
					gameKey = GC_STRAFERIGHT;
					break;
				case KEY_ENTER:
				case KEY_MOUSE1:
				case KEY_JOY1:
					gameKey = GC_JUMP;
					break;
				case KEY_ESCAPE:
				case KEY_MOUSE1 + 1:
				case KEY_JOY1 + 1:
					gameKey = GC_SPIN;
					break;
				case KEY_BACKSPACE:
				case KEY_JOY1 + 2:
					gameKey = GC_FIRE;
					break;
				case KEY_JOY1 + 3:
					menuKey = 'n';
					gameKey = GC_FIRENORMAL;
					break;
				default:
					while (menukey_list[gc].key)
					{
						if (PLAYERINPUTDOWN(pid+1, menukey_list[gc].key)) break;
						gc++;
					}
					gameKey = menukey_list[gc].key;
					break;
			}
		}
	}
	else if ((ev->type == ev_joystick && ev->key == 0) || ev->type == ev_mouse)
	{
		menuKey = (*ch); // -- Record gamepad/mouse presses
		// Translate controller keys to work in our menus
		switch (menuKey)
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
			default:
				break;
		}
	}

	if (in_event_eatable_situation && !hold_event_hostage)
	{
		// Reset keys used, so the game doesn't freak out.
		memset(gamekeydown, 0, sizeof(gamekeydown));
	}

	if (ev->type == ev_keyup || ((*ch) == -1) || menuKey == -1)
	{
		gameKey = GC_NULL;
	}

	if (tsourdt3rd_currentMenu == NULL)
	{
		// We're not in our unique menu system!
		// But, we can still provide some cool features!

		if (menuKey <= 0 || (*ch) <= 0)
		{
			// We have no inputs! Why continue?
			return false;
		}

		if (TSoURDt3rd_M_DoesMenuHaveKeyHandler() >= 4)
		{
			// Our responder is better-fit to handle this Vanilla-To-TSoURDt3rd conversion.
			return true;
		}

		if (I_GetTextInputMode())
		{
			// Since we're about to start typing,
			// ignore ev_keydown events and just quit right now.
			if (*ch >= 32 && *ch <= 127)
				return false;
		}

		// Translate our unique menu controls to vanilla menus too!
		for (key = 0; key_table[key]; key++)
			if (key_table[key] == *ch) return false;
		if (menuactive)
		{
			switch (gameKey)
			{
				case GC_FORWARD:
					(*ch) = KEY_UPARROW;
					break;
				case GC_BACKWARD:
					(*ch) = KEY_DOWNARROW;
					break;
				case GC_STRAFELEFT:
				case GC_TURNLEFT:
					(*ch) = KEY_LEFTARROW;
					break;
				case GC_STRAFERIGHT:
				case GC_TURNRIGHT:
					(*ch) = KEY_RIGHTARROW;
					break;
				case GC_JUMP:
					(*ch) = KEY_ENTER;
					break;
				case GC_SPIN:
				case GC_SYSTEMMENU:
					(*ch) = KEY_ESCAPE;
					break;
				default:
					break;
			}
		}

		return false;
	}

	return (I_GetTextInputMode() || (gameKey != GC_NULL));
}

//
// void TSoURDt3rd_M_PlayMenuJam(void)
// Plays menu music.
//
// Inspired by M_PlayMenuJam from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_M_PlayMenuJam(void)
{
	tsourdt3rd_menu_t *refMenu = (menuactive ? tsourdt3rd_currentMenu : tsourdt3rd_currentMenu->prev_menu);
	const boolean profilemode = (optionsmenu.profilemenu && !optionsmenu.resetprofilemenu);

	if (tsourdt3rd_currentMenu == NULL || TSoURDt3rd_Jukebox_IsPlaying())
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
		if (strnicmp(refMenu->music, S_MusicName(), 7))
			M_ChangeMenuMusic(refMenu->music, true);
		return;
	}

	if (!strnicmp("_inter", S_MusicName(), 7))
		return;
	M_ChangeMenuMusic("_inter", true);
}

//
// boolean TSoURDt3rd_M_StartControlPanel(void)
//
// Runs our unique control panel data.
// Returns true if it needs to restrict usage of the regular control panel, false otherwise.
//
boolean TSoURDt3rd_M_StartControlPanel(void)
{
	const UINT8 pid = 0;

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
				// we can't call TSoURDt3rd_M_HandleMenuMessage.
				TSoURDt3rd_M_StopMessage(MA_NONE);
			}
			return true;
		}
	}

#ifdef HAVE_DISCORDSUPPORT
	if (menuactive)
	{
		// currentMenu changed during menuactive
		DISC_UpdatePresence();
	}
#endif

	TSoURDt3rd_M_SetMenuDelay(pid);
	return false;
}

//
// void TSoURDt3rd_M_ClearMenus(boolean callexitmenufunc)
// Clears the screen of any menus open, running any quit routines if necessary.
//
void TSoURDt3rd_M_ClearMenus(boolean callexitmenufunc)
{
	const UINT8 pid = 0;

	if (!menuactive)
		return;

	if (tsourdt3rd_currentMenu != NULL && tsourdt3rd_currentMenu->quitroutine && callexitmenufunc && !tsourdt3rd_currentMenu->quitroutine())
	{
		if (tsourdt3rd_global_jukebox != NULL)
		{
			// Just in case...
			tsourdt3rd_global_jukebox->in_menu = false;
		}
		return; // we can't quit this menu (also used to set parameter from the menu)
	}

	vanilla_prevMenu = NULL;
	tsourdt3rd_prevMenu = NULL;
	tsourdt3rd_currentMenu = NULL;

	memset(&menutransition, 0, sizeof(menutransition));
	menuwipe = false;

	TSoURDt3rd_M_AbortVirtualKeyboard();
	menumessage.active = false;

	TSoURDt3rd_M_SetMenuDelay(pid);

#ifdef HAVE_DISCORDSUPPORT
	DISC_UpdatePresence();
#endif
}

//
// void TSoURDt3rd_M_GoBack(INT32 choice)
// Moves backwards to the previous menu.
//
void TSoURDt3rd_M_GoBack(INT32 choice)
{
	INT16 behaviourflags = 0;
	tsourdt3rd_menu_t *next_tsourdt3rd_menu = NULL;
	menu_t *next_menu = NULL;

	(void)choice;
	tsourdt3rd_noFurtherInput = true;

	if (tsourdt3rd_currentMenu != NULL)
	{
		behaviourflags = tsourdt3rd_currentMenu->behaviourflags;
		if (tsourdt3rd_currentMenu->prev_menu != NULL)
			next_tsourdt3rd_menu = tsourdt3rd_currentMenu->prev_menu;
	}

	if (currentMenu != NULL)
	{
		currentMenu->lastOn = tsourdt3rd_itemOn;
		if (currentMenu->prevMenu != NULL)
		{
			next_menu = currentMenu->prevMenu;

			// If we entered the game search menu, but didn't enter a game,
			// make sure the game doesn't still think we're in a netgame.
			if (!Playing() && netgame && multiplayer)
			{
				netgame = false;
				multiplayer = false;
			}
		}
	}

	TSoURDt3rd_M_SetupNextMenu(next_tsourdt3rd_menu, next_menu, false);

	if (!(behaviourflags & MBF_SOUNDLESS))
		S_StartSound(NULL, sfx_s3k5b);
}

//
// void TSoURDt3rd_M_ChangeCvar(INT32 choice, consvar_t *cv)
// Changes the values of a given cvar, based on the choice.
//
void TSoURDt3rd_M_ChangeCvar(INT32 choice, consvar_t *cv)
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
// void TSoURDt3rd_M_ChangeCvarDirect(INT32 amount, float amount_f, consvar_t *cv)
// Directly changes the values of a given cvar, based on the amount given, and the flags of 'cv'.
//
void TSoURDt3rd_M_ChangeCvarDirect(INT32 amount, float amount_f, consvar_t *cv)
{
	if (cv->flags & CV_FLOAT)
	{
		char s[20];
		float n = FIXED_TO_FLOAT(cv->value)+(amount_f);
		sprintf(s, "%ld%s", (long)n, M_Ftrim(n));
		CV_Set(cv, s);
	}
	else
		CV_AddValue(cv, amount);
}

static void M_ChangeCvar(INT32 choice)
{
	consvar_t *cvar = currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
	if (cvar == NULL)
		cvar = tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].itemaction.cvar;
	TSoURDt3rd_M_ChangeCvar(choice, cvar);
}

static const char *M_QueryCvarAction(const char *replace)
{
	consvar_t *cvar = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
	if (cvar == NULL)
		cvar = tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].itemaction.cvar;
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
// boolean TSoURDt3rd_M_NextOpt(void)
// Finds the next valid menu item, returns true if it can.
//
boolean TSoURDt3rd_M_NextOpt(void)
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
// boolean TSoURDt3rd_M_PrevOpt(void)
// Finds a previous menu item, returns true if it can.
//
boolean TSoURDt3rd_M_PrevOpt(void)
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
// static boolean M_HandleGlobalInput(UINT8 pid, INT32 choice)
//
//
static boolean M_HandleGlobalInput(UINT8 pid, INT32 choice)
{
	if (TSoURDt3rd_M_DoesMenuHaveKeyHandler() != -1)
		return false;

	if (tsourdt3rd_global_jukebox != NULL)
	{
		if (choice == JB_OPENJUKEBOX || STAR_G_KeyPressed(pid, JB_OPENJUKEBOX) || TSoURDt3rd_M_MenuButtonPressed(pid, MBT_JUKEBOX_OPEN))
		{
			// A shortcut to open the Jukebox menu.
			TSoURDt3rd_M_Jukebox_Init(op_extras_jukebox);
			return true;
		}
	}

	return false;
}

//
// static void M_HandleMenuInput(void)
// Handles our TSoURDt3rd exclusive menu inputs.
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

	if (menumessage.active)
	{
		TSoURDt3rd_M_HandleMenuMessage();
		return;
	}

	if (menutyping.active)
	{
		// Typing for CV_IT_STRING
		TSoURDt3rd_M_MenuTypingInput(-1);
		return;
	}

	if (M_HandleGlobalInput(pid, thisMenuKey))
	{
		// Global keys take priority.
		return;
	}

	if (menuactive == false)
	{
		// We're not in the menu.
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

	routine = tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].itemaction.routine;
	if (routine == NULL)
		routine = currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;

	// Handle menuitems which need a specific key handling
	if (routine && (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_KEYHANDLER)
	{
		routine(-1);
		return;
	}

	// BP: one of the more big hack i have never made
	if (routine && (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_CVAR)
	{
		if ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_CVARTYPE) == IT_CV_STRING)
		{
			// Routine is null either way
			routine = NULL;

			// If we're hovering over a IT_CV_STRING option, pressing A/X opens the typing submenu
			if (TSoURDt3rd_M_MenuConfirmPressed(pid))
			{
				// If we entered this menu by pressing a menu Key, default to keyboard typing, otherwise use controller.
				TSoURDt3rd_M_OpenVirtualKeyboard(
					(tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1 ?
						tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1 :
						MAXSTRINGLENGTH
					),
					M_QueryCvarAction,
					NULL
				);
				return;
			}
			else if (TSoURDt3rd_M_MenuExtraPressed(pid))
			{
				if (!(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
					S_StartSound(NULL, sfx_s3k5b);

				M_ChangeCvar(-1);
				TSoURDt3rd_M_SetMenuDelay(pid);
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
		if (TSoURDt3rd_M_NextOpt() && !(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
			S_StartSound(NULL, sfx_s3k5b);
		TSoURDt3rd_M_SetMenuDelay(pid);
		return;
	}
	else if (ud < 0)
	{
		if (TSoURDt3rd_M_PrevOpt() && !(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
			S_StartSound(NULL, sfx_s3k5b);
		TSoURDt3rd_M_SetMenuDelay(pid);
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
			TSoURDt3rd_M_SetMenuDelay(pid);
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
			TSoURDt3rd_M_SetMenuDelay(pid);
		}
		return;
	}
	else if (TSoURDt3rd_M_MenuConfirmPressed(pid) /*|| TSoURDt3rd_M_MenuButtonPressed(pid, MBT_START)*/)
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
					TSoURDt3rd_M_StartMessage("TSoURDt3rd; Modified Game", M_GetText("This cannot be done with complex addons\nor in a cheated game."), NULL, MM_NOTHING, NULL, NULL);
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
					TSoURDt3rd_M_SetupNextMenu(
						(tsourdt3rd_menu_t *)tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].itemaction.routine,
						(menu_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction,
						false
					);
					break;
			}
		}

		TSoURDt3rd_M_SetMenuDelay(pid);
		return;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		TSoURDt3rd_M_GoBack(0);
		TSoURDt3rd_M_SetMenuDelay(pid);
		return;
	}
	else if (TSoURDt3rd_M_MenuExtraPressed(pid))
	{
		if (routine && ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_ARROWS
			|| (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TYPE) == IT_CVAR))
		{
			if (!(tsourdt3rd_currentMenu->behaviourflags & MBF_SOUNDLESS))
				S_StartSound(NULL, sfx_s3k5b);

			routine(-1);
			TSoURDt3rd_M_SetMenuDelay(pid);
		}
		return;
	}

	return;
}

//
// void TSoURDt3rd_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter, INT32 levellistmode)
// Handles our unique menu ticking.
//
// Inspired by M_Ticker() from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter, INT32 levellistmode)
{
	vanilla_itemOn = (*item);
	tsourdt3rd_noFurtherInput = (*input);
	tsourdt3rd_skullAnimCounter = skullAnimCounter;
	tsourdt3rd_levellistmode = (tsourdt3rd_levellist_mode_t)levellistmode;

	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
	{
		if (menucmd[i].delay > 0)
		{
			menucmd[i].delay--;
		}
	}

	if (menuactive)
	{
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
				if (tsourdt3rd_currentMenu != menutransition.endmenu)
				{
					M_Sys_DoTransition(false, NULL, NULL);
					TSoURDt3rd_M_SetupNextMenu(menutransition.endmenu, menutransition.vanilla_endmenu, true);
				}
			}
		}
		else
		{
			// true - try not to let people input during the fadeout
			// false - reset input trigger
			tsourdt3rd_noFurtherInput = (menuwipe);
		}
	}

	if (tsourdt3rd_noFurtherInput == false)
	{
		M_HandleMenuInput();
	}

	if (!menuactive)
	{
		// Alright, I stretched it as far as possible, but this is as far as we go.
		return;
	}

	if (tsourdt3rd_currentMenu == NULL)
	{
		// We're not in our unique menu system!
#ifdef HAVE_DISCORDSUPPORT
		if (discordRequestList)
		{
			// Show discord requests menu option if any requests are pending
			MPauseMenu[4].status = IT_STRING | IT_CALL; // mpause_discordrequests
			if (currentMenu == &MPauseDef && (skullAnimCounter % 4))
				V_DrawScaledPatch(currentMenu->x, currentMenu->menuitems[4].alphaKey, 0, W_CachePatchName("ICODIS", PU_CACHE));
		}
#endif
		tsourdt3rd_itemOn = vanilla_itemOn;
		return;
	}

	if (tsourdt3rd_currentMenu->tickroutine)
	{
		tsourdt3rd_currentMenu->tickroutine();
	}

	vanilla_itemOn = tsourdt3rd_itemOn;
	item = &vanilla_itemOn;
	*input = tsourdt3rd_noFurtherInput;
}
