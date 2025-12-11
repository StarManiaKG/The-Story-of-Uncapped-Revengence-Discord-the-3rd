// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd.c
/// \brief TSoURDt3rd's unique main menu options

#include "../smkg-m_sys.h"

#include "../../smkg-cvars.h"

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

struct optionsmenu_s optionsmenu;

static void M_Sys_OptionsMenuGoto(tsourdt3rd_menu_t *tsourdt3rd_assignment, menu_t *assignment);
static void M_Sys_LoadEventsMenu(INT32 choice);
static void M_Sys_LoadControlsMenu(INT32 choice);
static void M_Sys_LoadVideoMenu(INT32 choice);
static void M_Sys_LoadAudioMenu(INT32 choice);
static void M_Sys_LoadExtrasMenu(INT32 choice);

menuitem_t TSoURDt3rd_OP_MainMenu[] =
{
	{IT_STRING | IT_SUBMENU, NULL, "Read ME!",
		&TSoURDt3rd_OP_ReadMEDef, 0},
	{IT_STRING | IT_CALL, NULL, "Event Options",
		M_Sys_LoadEventsMenu, 0},
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
		&TSoURDt3rd_OP_SavedataDef, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Server Options",
		&TSoURDt3rd_OP_ServerDef, 0},
	{IT_STRING | IT_CALL, NULL, "Extras",
		M_Sys_LoadExtrasMenu, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Debugging",
		&TSoURDt3rd_OP_DebugDef, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_MainMenu[] =
{
	{NULL, "Hey, check this out!", { .submenu = &TSoURDt3rd_TM_OP_ReadMEDef }, 0, 0},
	{NULL, "Access TSoURDt3rd's fun events!", { NULL }, 0, 0},
	{NULL, "Change up TSoURDt3rd's engine with cool gameplay options!", { .submenu = &TSoURDt3rd_TM_OP_GameDef }, 0, 0},
	{NULL, "Set keybinds for TSoURDt3rd's features!", {NULL}, 0, 0},
	{NULL, "Change up TSoURDt3rd's display!", {NULL}, 0, 0},
	{NULL, "Control TSoURDt3rd's audio engineering system!", {NULL}, 0, 0},
	{NULL, "Use these options to further improve your player experience!", { .submenu = &TSoURDt3rd_TM_OP_PlayerDef }, 0, 0},
	{NULL, "Saved data can be improved for ease-of-use with these options!", { .submenu = &TSoURDt3rd_TM_OP_SavedataDef }, 0, 0},
	{NULL, "Improve the server-hosting experience with these options!", { .submenu = &TSoURDt3rd_TM_OP_ServerDef }, 0, 0},
	{NULL, "Mess around with TSoURDt3rd's unique side content!", {NULL}, 0, 0},
	{NULL, "Manipulate the depths of TSoURDt3rd's inner workings!", { .submenu = &TSoURDt3rd_TM_OP_DebugDef }, 0, 0},
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
	TSoURDt3rd_TM_OP_MainMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	TSoURDt3rd_M_OptionsInputs,
	NULL,
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_M_Main_InitOptions(INT32 choice)
// Initializes TSoURDt3rd's main menu options.
//
void TSoURDt3rd_M_Main_InitOptions(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_M_ResetOptions();
	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_MainMenuDef, &TSoURDt3rd_OP_MainMenuDef, true);
}

//
// void TSoURDt3rd_M_DrawOptions(INT32 choice)
// Drawing routine for the main TSoURDt3rd options menu.
//
void TSoURDt3rd_M_DrawOptions(void)
{
	UINT8 i;
	fixed_t t = Easing_OutSine(TSoURDt3rd_M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t x = (140 - (48*tsourdt3rd_itemOn))*FRACUNIT + t;
	fixed_t y = 70*FRACUNIT + t;
	fixed_t tx = M_EaseWithTransition(Easing_InQuart, 5 * 64 * FRACUNIT);
	patch_t *buttback = W_CachePatchName("OPT_BUTT", PU_CACHE);
	UINT8 *c = NULL;

	for (i = 0; i < currentMenu->numitems; i++)
	{
		fixed_t py, px;
		INT32 tflag = V_ALLOWLOWERCASE;

		if ((currentMenu->menuitems[i].status & IT_DISPLAY) == IT_GRAYPATCH)
		{
			y += 48*FRACUNIT;
			x += 48*FRACUNIT;
			continue;
		}

		py = y - (tsourdt3rd_itemOn*48)*FRACUNIT;
		px = x - tx;

		if (i == tsourdt3rd_itemOn)
			c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
		else
			c = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_BLACK, GTC_CACHE);

		if (currentMenu->menuitems[i].status & IT_TRANSTEXT)
			tflag |= V_TRANSLUCENT;

		if (!(menutransition.tics != menutransition.dest && i == tsourdt3rd_itemOn))
		{
			V_DrawFixedPatch(px, py, FRACUNIT, 0, buttback, c);

			const char *s = currentMenu->menuitems[i].text;
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

	// Tooltip
	// Draw it at the top of the screen
	{
		menutooltip_t tooltip = {
			{
				0, 0,
				vid.width*FRACUNIT, FRACUNIT,
				V_SNAPTOLEFT|V_SNAPTOTOP,
				NULL,
				false
			},
			{
				BASEVIDWIDTH/2, 13,
				V_SNAPTOTOP|V_ALLOWLOWERCASE,
				aligncenter
			}
		};
		TSoURDt3rd_M_DrawMenuTooltips(tsourdt3rd_currentMenu, tooltip);
	}

	if (menutransition.tics != menutransition.dest)
		TSoURDt3rd_M_DrawOptionsMovingButton();
}

//
// void TSoURDt3rd_M_ResetOptions(void)
// Resetting routine for the drawing system used in some TSoURDt3rd menus.
//
void TSoURDt3rd_M_ResetOptions(void)
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

	// Binding
	optionsmenu.bindmenuactive = false;
}

//
// void TSoURDt3rd_M_OptionsTick(void)
// Ticking routine for the main TSoURDt3rd options menu.
//
void TSoURDt3rd_M_OptionsTick(void)
{
	boolean instanttransmission = optionsmenu.ticker == 0 && menuwipe;

	if (tsourdt3rd_currentMenu == NULL)
	{
		// Somehow possible to crash without this, so let's just keep it here.
		return;
	}
	optionsmenu.ticker++;

	// Move the button for cool animations
	if (tsourdt3rd_currentMenu->inputroutine == TSoURDt3rd_M_OptionsInputs)
		TSoURDt3rd_M_OptionsQuit(); // ...So now this is used here.
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
		TSoURDt3rd_M_SetMenuDelay(pid);
		optionsmenu.offset.dist = 48;

		TSoURDt3rd_M_NextOpt();
		S_StartSoundFromEverywhere(sfx_s3k5b);

		if (tsourdt3rd_itemOn == 0)
			optionsmenu.offset.dist -= currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();
		return true;
	}
	else if (menucmd[pid].dpad_ud < 0)
	{
		TSoURDt3rd_M_SetMenuDelay(pid);
		optionsmenu.offset.dist = -48;

		TSoURDt3rd_M_PrevOpt();
		S_StartSoundFromEverywhere(sfx_s3k5b);

		if (tsourdt3rd_itemOn == currentMenu->numitems-1)
			optionsmenu.offset.dist += currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();
		return true;
	}
	else if (TSoURDt3rd_M_MenuConfirmPressed(pid))
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
// boolean TSoURDt3rd_M_OptionsQuit(void)
// Quitting routine for the main TSoURDt3rd options menu.
//
boolean TSoURDt3rd_M_OptionsQuit(void)
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

	optionsmenu.bindmenuactive = false;

	return true;	// Always allow quitting, duh.
}


//
// static void M_Sys_OptionsMenuGoto(tsourdt3rd_menu_t *tsourdt3rd_assignment, menu_t *assignment)
// Loads and changes the current menu, using some extra parameters.
//
// Inspired by M_OptionsMenuGoto from Dr.Robotnik's Ring Racers!
//
static void M_Sys_OptionsMenuGoto(tsourdt3rd_menu_t *tsourdt3rd_assignment, menu_t *assignment)
{
	assignment->prevMenu = currentMenu;
	tsourdt3rd_assignment->music = tsourdt3rd_currentMenu->music;

	TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_assignment, assignment, false);

	if (currentMenu != &TSoURDt3rd_OP_MainMenuDef && currentMenu != &TSoURDt3rd_OP_ExtrasDef)
	{
		optionsmenu.ticker = 0;
		TSoURDt3rd_M_OptionsTick();
	}
}

static void M_Sys_LoadEventsMenu(INT32 choice)
{
	(void)choice;
	TSoURDt3rd_TM_OP_ControlsDef.music = tsourdt3rd_currentMenu->music;

	if (!tsourdt3rd_currentEvent)
	{
		TSoURDt3rd_M_StartPlainMessage("TSoURDt3rd Events", M_GetText("There's no event happening right now!\n"));
		return;
	}

	M_Sys_OptionsMenuGoto(&TSoURDt3rd_TM_OP_EventsDef, &TSoURDt3rd_OP_EventsDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_eventsmenu;
}

static void M_Sys_LoadControlsMenu(INT32 choice)
{
	(void)choice;
	TSoURDt3rd_TM_OP_ControlsDef.music = tsourdt3rd_currentMenu->music;

	memcpy(&optionsmenu.tempcontrols, gamecontrol[0], sizeof(optionsmenu.tempcontrols));
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_TM_OP_ControlsDef, &TSoURDt3rd_OP_ControlsDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_controlsmenu;
	optionsmenu.bindmenuactive = true;
}

static void M_Sys_LoadAudioMenu(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_TM_OP_AudioDef.music = tsourdt3rd_currentMenu->music;
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_TM_OP_AudioDef, &TSoURDt3rd_OP_AudioDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_audiomenu;
}

static void M_Sys_LoadVideoMenu(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_TM_OP_VideoDef.music = tsourdt3rd_currentMenu->music;
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_TM_OP_VideoDef, &TSoURDt3rd_OP_VideoDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_videomenu;
}

static void M_Sys_LoadExtrasMenu(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_M_ResetOptions();
	M_Sys_OptionsMenuGoto(&TSoURDt3rd_TM_OP_ExtrasDef, &TSoURDt3rd_OP_ExtrasDef);
	TSoURDt3rd_OP_MainMenuDef.lastOn = op_main_extrasmenu;
}
