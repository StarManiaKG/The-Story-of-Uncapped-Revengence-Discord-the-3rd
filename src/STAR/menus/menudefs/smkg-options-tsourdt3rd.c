// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Original Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Original Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd.c
/// \brief TSoURDt3rd's unique main menu options

#include "../smkg_m_draw.h"
#include "../smkg_m_func.h"
#include "../../smkg-cvars.h"
#include "../../m_menu.h"

#include "../../drrr/k_menu.h"

#include "../../../i_time.h"
#include "../../../m_easing.h"
#include "../../../r_draw.h"
#include "../../../r_main.h"
#include "../../../v_video.h"
#include "../../../w_wad.h"
#include "../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

static boolean TSoURDt3rd_M_OptionsQuit(void);

static void M_Sys_OptionsMenuGoto(menu_t *assignment, tsourdt3rd_menu_t *tsourdt3rd_assignment);
static void M_Sys_LoadControlsMenu(INT32 choice);
static void M_Sys_LoadExtrasMenu(INT32 choice);
static void M_Sys_LoadVideoMenu(INT32 choice);
static void M_Sys_LoadAudioMenu(INT32 choice);

menuitem_t TSoURDt3rd_OP_MainMenu[] =
{
	{IT_STRING | IT_SUBMENU, NULL, "Event Options",
		&TSoURDt3rd_OP_EventsDef, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Game Options",
		&TSoURDt3rd_OP_GameDef, 0},
	{IT_STRING | IT_CALL, NULL, "Control Options",
		M_Sys_LoadControlsMenu, 0},
	{IT_STRING | IT_CALL, NULL, "Video Options",
		M_Sys_LoadVideoMenu, 0},
	{IT_STRING | IT_CALL, NULL, "Audio Options",
		M_Sys_LoadAudioMenu, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Player Options",
		&TSoURDt3rd_OP_PlayerDef, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Savedata Options",
		&TSoURDt3rd_OP_SavefileDef, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Server Options",
		&TSoURDt3rd_OP_ServerDef, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Jukebox Options",
		&TSoURDt3rd_OP_JukeboxDef, 0},
	{IT_STRING | IT_CALL, NULL, "Extras",
		M_Sys_LoadExtrasMenu, 0},
};

menu_t TSoURDt3rd_OP_MainMenuDef =
{
	MTREE2(MN_OP_MAIN, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_MainMenu)/sizeof (menuitem_t),
	&OP_MainDef,
	TSoURDt3rd_OP_MainMenu,
	TSoURDt3rd_M_DrawOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_MainMenuDef = {
	&TSoURDt3rd_OP_MainMenuDef,
	NULL,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	TSoURDt3rd_M_OptionsInputs,
	&TSoURDt3rd_TM_OP_MainMenuDef,
	NULL
};

enum
{
	op_main_eventsmenu,
	op_main_gamemenu,
	op_main_controlsmenu,
	op_main_videomenu,
	op_main_audiomenu,
	op_main_playermenu,
	op_main_savedatamenu,
	op_main_servermenu,
	op_main_jukeboxmenu,
	op_main_extrasmenu
};

struct optionsmenu_s optionsmenu;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_InitOptionsMenu(INT32 choice)
// Initializes TSoURDt3rd's main menu options.
//
static void M_Sys_ResetOptions(void)
{
	optionsmenu.ticker = 0;
	optionsmenu.offset.start = 0;

	optionsmenu.optx = 0;
	optionsmenu.opty = 0;
	optionsmenu.toptx = 0;
	optionsmenu.topty = 0;

	// BG setup:
	optionsmenu.currcolour = TSoURDt3rd_TM_OP_MainMenuDef.extra1;
	optionsmenu.lastcolour = 0;
	optionsmenu.fade = 0;

	// For profiles:
#if 0
	memset(setup_player, 0, sizeof(setup_player));
#endif
	optionsmenu.profile = NULL;
}

void TSoURDt3rd_M_InitOptionsMenu(INT32 choice)
{
	(void)choice;

	M_Sys_ResetOptions();
	M_SetupNextMenu(&TSoURDt3rd_OP_MainMenuDef);
}

//
// void TSoURDt3rd_M_DrawOptions(INT32 choice)
// Drawing routine for the main TSoURDt3rd options menu.
//
void TSoURDt3rd_M_DrawOptions(void)
{
	UINT8 i;
	fixed_t t = Easing_OutSine(M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t x = (140 - (48*tsourdt3rd_itemOn))*FRACUNIT + t;
	fixed_t y = 70*FRACUNIT + t;
	fixed_t tx = M_EaseWithTransition(Easing_InQuart, 5 * 64 * FRACUNIT);
	patch_t *buttback = W_CachePatchName("OPT_BUTT", PU_CACHE);
	UINT8 *c = NULL;

	for (i = 0; i < tsourdt3rd_currentMenu->menu->numitems; i++)
	{
		fixed_t py = y - (tsourdt3rd_itemOn*48)*FRACUNIT;
		fixed_t px = x - tx;
		INT32 tflag = 0;

		if (i == tsourdt3rd_itemOn)
			c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
		else
			c = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_BLACK, GTC_CACHE);

		if (tsourdt3rd_currentMenu->menu->menuitems[i].status & IT_TRANSTEXT)
			tflag |= V_TRANSLUCENT;

		if (!(menutransition.tics != menutransition.dest && i == tsourdt3rd_itemOn))
		{
			V_DrawFixedPatch(px, py, FRACUNIT, 0, buttback, c);

			const char *s = tsourdt3rd_currentMenu->menu->menuitems[i].text;
			fixed_t w = V_FontStringWidth(
				s,
				tflag,
				lt_font
			);
			V_DrawFontStringAtFixed(
				px - 3*FRACUNIT - ((w/2)<<FRACBITS),
				py - 16*FRACUNIT,
				tflag,
				FRACUNIT,
				FRACUNIT,
				s,
				lt_font
			);
		}

		y += 48*FRACUNIT;
		x += 48*FRACUNIT;
	}

	TSoURDt3rd_M_DrawMenuTooltips(
		0, 0, V_SNAPTOTOP, NULL,
		BASEVIDWIDTH/2, 15, true
	);

	if (menutransition.tics != menutransition.dest)
		TSoURDt3rd_M_DrawOptionsMovingButton();
}

//
// void TSoURDt3rd_M_OptionsTick(void)
// Ticking routine for the main TSoURDt3rd options menu.
//
void TSoURDt3rd_M_OptionsTick(void)
{
	boolean instanttransmission = optionsmenu.ticker == 0 && menuwipe;

	optionsmenu.ticker++;

	// Move the button for cool animations
	if (currentMenu == &TSoURDt3rd_OP_MainMenuDef || currentMenu == &TSoURDt3rd_OP_ExtrasDef)
	{
		if (currentMenu == &TSoURDt3rd_OP_MainMenuDef && (!eastermode && !aprilfoolsmode))
			TSoURDt3rd_OP_MainMenu[0].status = IT_DISABLED;
		else if (currentMenu == &TSoURDt3rd_OP_ExtrasDef && (!eastermode && !aprilfoolsmode))
			TSoURDt3rd_OP_ExtrasMenu[1].status = (!netgame ? IT_CALL|IT_STRING : IT_GRAYEDOUT);

		TSoURDt3rd_M_OptionsQuit(); // ...So now this is used here.
	}
	else if (optionsmenu.profile == NULL)	// Not currently editing a profile (otherwise we're using these variables for other purposes....)
	{
		// I don't like this, it looks like shit but it needs to be done..........
		if (optionsmenu.profilemenu)
		{
			optionsmenu.toptx = 440;
			optionsmenu.topty = 70+1;
		}
		else
		{
			optionsmenu.toptx = 160;
			optionsmenu.topty = 50;
		}
	}

	// Handle the background stuff:
	if (optionsmenu.fade)
		optionsmenu.fade--;

	// change the colour if we aren't matching the current menu colour
	if (instanttransmission)
	{
		optionsmenu.currcolour = tsourdt3rd_currentMenu->extra1;
		optionsmenu.offset.start = optionsmenu.fade = 0;

		optionsmenu.optx = optionsmenu.toptx;
		optionsmenu.opty = optionsmenu.topty;
	}
	else
	{
		if (optionsmenu.fade)
			optionsmenu.fade--;

		if (optionsmenu.currcolour != tsourdt3rd_currentMenu->extra1)
		{
			optionsmenu.fade = 10;
			optionsmenu.lastcolour = optionsmenu.currcolour;
			optionsmenu.currcolour = tsourdt3rd_currentMenu->extra1;
		}

		if (optionsmenu.optx != optionsmenu.toptx || optionsmenu.opty != optionsmenu.topty)
		{
			tic_t t = I_GetTime();
			tic_t n = t - optionsmenu.topt_start;

			if (n == M_OPTIONS_OFSTIME)
			{
				optionsmenu.optx = optionsmenu.toptx;
				optionsmenu.opty = optionsmenu.topty;
			}
			else if (n > M_OPTIONS_OFSTIME)
			{
				optionsmenu.topt_start = I_GetTime();
			}
		}
	}
}

//
// boolean TSoURDt3rd_M_OptionsInputs(INT32 ch)
// Input routine for the main TSoURDt3rd options menu.
//
boolean TSoURDt3rd_M_OptionsInputs(INT32 ch)
{
	const UINT8 pid = 0;
	(void)ch;

	if (menucmd[pid].dpad_ud > 0)
	{
		M_SetMenuDelay(pid);
		optionsmenu.offset.dist = 48;

		STAR_M_NextOpt();
		S_StartSound(NULL, sfx_s3k5b);

		if (tsourdt3rd_itemOn == 0)
			optionsmenu.offset.dist -= currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();
		return true;
	}
	else if (menucmd[pid].dpad_ud < 0)
	{
		M_SetMenuDelay(pid);
		optionsmenu.offset.dist = -48;

		STAR_M_PrevOpt();
		S_StartSound(NULL, sfx_s3k5b);

		if (tsourdt3rd_itemOn == currentMenu->numitems-1)
			optionsmenu.offset.dist += currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();
		return true;
	}
	else if (M_MenuConfirmPressed(pid))
	{
		if (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TRANSTEXT)
			return true;	// No.

		optionsmenu.optx = 140;
		optionsmenu.opty = 70;	// Default position for the currently selected option.
		return false;	// Don't eat.
	}
	return false;
}

//
// static boolean TSoURDt3rd_M_OptionsQuit(void)
// Quitting routine for the main TSoURDt3rd options menu.
//
static boolean TSoURDt3rd_M_OptionsQuit(void)
{
	optionsmenu.toptx = 140;
	optionsmenu.topty = 70;

	// Reset button behaviour because profile menu is different, since of course it is.
	if (optionsmenu.resetprofilemenu)
	{
		optionsmenu.profilemenu = false;
		optionsmenu.profile = NULL;
		optionsmenu.resetprofilemenu = false;
	}

	return true;	// Always allow quitting, duh.
}


//
// static void M_Sys_OptionsMenuGoto(menu_t *assignment, tsourdt3rd_menu_t *tsourdt3rd_assignment)
// Loads and changes the current menu, using some extra parameters.
//
// Inspired by M_OptionsMenuGoto from DRRR!
//
static void M_Sys_OptionsMenuGoto(menu_t *assignment, tsourdt3rd_menu_t *tsourdt3rd_assignment)
{
	assignment->prevMenu = currentMenu;
	tsourdt3rd_assignment->music = tsourdt3rd_currentMenu->music;

	M_SetupNextMenu(assignment);

	if (currentMenu != &TSoURDt3rd_OP_MainMenuDef && currentMenu != &TSoURDt3rd_OP_ExtrasDef)
	{
		optionsmenu.ticker = 0;
		TSoURDt3rd_M_OptionsTick();
	}
}

static void M_Sys_LoadExtrasMenu(INT32 choice)
{
	(void)choice;

	M_Sys_ResetOptions();
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_OP_ExtrasDef, &TSoURDt3rd_TM_OP_ExtrasDef);
}

static void M_Sys_LoadControlsMenu(INT32 choice)
{
	(void)choice;
	TSoURDt3rd_TM_OP_ControlsDef.music = tsourdt3rd_currentMenu->music;

	memcpy(&optionsmenu.tempcontrols, gamecontrol, sizeof(optionsmenu.tempcontrols));
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_OP_ControlsDef, &TSoURDt3rd_TM_OP_ControlsDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_controlsmenu;
}

static void M_Sys_LoadAudioMenu(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_TM_OP_AudioDef.music = tsourdt3rd_currentMenu->music;
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_OP_AudioDef, &TSoURDt3rd_TM_OP_AudioDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_audiomenu;
}

static void M_Sys_LoadVideoMenu(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_TM_OP_VideoDef.music = tsourdt3rd_currentMenu->music;
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_OP_VideoDef, &TSoURDt3rd_TM_OP_VideoDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_videomenu;
}
