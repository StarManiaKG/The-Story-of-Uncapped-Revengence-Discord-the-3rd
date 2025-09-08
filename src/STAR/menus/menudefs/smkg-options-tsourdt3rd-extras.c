// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-extras.c
/// \brief TSoURDt3rd's unique extra menu options and routines

#include "../smkg-m_sys.h"

#include "../../smkg-i_sys.h"

#include "../../../i_system.h" // I_ClipboardCopy()

// ------------------------ //
//        Variables
// ------------------------ //

static char gitlab_mods_url[256];

static void M_Sys_OpenGitlabModsURL(INT32 choice);
static void M_Sys_SpawnTF2Dispenser(INT32 choice);

menuitem_t TSoURDt3rd_OP_ExtrasMenu[] =
{
	{IT_STRING | IT_CALL, NULL, "Mods",
		M_Sys_OpenGitlabModsURL, 0},
	{IT_STRING | IT_CALL, NULL, "Jukebox",
		TSoURDt3rd_M_Jukebox_Init, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Play Snake",
		&TSoURDt3rd_OP_Extras_SnakeDef, 0},
	{IT_STRING | IT_CALL, NULL, "Dispenser Goin' Up",
		M_Sys_SpawnTF2Dispenser, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ExtrasMenu[] =
{
	{NULL, "Visit TSoURDt3rd's repository to find some mods to load!", {NULL}, 0, 0},
	{NULL, "Enter the Jukebox to broaden up your gameplay experience!", {NULL}, 0, 0},
	{NULL, "Play a quick match of snake, why don'cha?", { .submenu = &TSoURDt3rd_TM_OP_Extras_SnakeDef }, 0, 0},
	{NULL, "Spawn a TF2 dispenser.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_ExtrasDef = {
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_ExtrasMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_ExtrasMenu,
	TSoURDt3rd_M_DrawOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ExtrasDef = {
	TSoURDt3rd_TM_OP_ExtrasMenu,
	0, 0,
	0,
	NULL,
	0, 0,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	TSoURDt3rd_M_OptionsInputs,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_OpenGitlabModsURL_Select(INT32 choice)
{
	if (choice != MA_YES) return;
	I_OpenURL(gitlab_mods_url);
}

static void M_Sys_OpenGitlabModsURL(INT32 choice)
{
	(void)choice;
	sprintf(gitlab_mods_url, "https://git.do.srb2.org/StarManiaKG/tsourdt3rd-assets/-/tree/%s/extras?ref_type=heads", compbranch);
	if (I_CanOpenURL())
	{
		TSoURDt3rd_M_StartMessage("TSoURDt3rd-Provided Mods",
			"This will open a page in your default browser.\nAre you ok with that?",
			M_Sys_OpenGitlabModsURL_Select,
			MM_YESNO,
			"Let's go!",
			"Not now..."
		);
	}
	else
	{
		I_ClipboardCopy(gitlab_mods_url, strlen(gitlab_mods_url));
		TSoURDt3rd_M_StartMessage("TSoURDt3rd-Provided Mods",
			"A URL to the repository hosting some TSoURDt3rd-Provided Mods has been copied to your clipboard.",
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
	}
}

static void M_Sys_SpawnTF2Dispenser(INT32 choice)
{
	(void)choice;

	if (!Playing())
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "You need to be in a game in order to spawn this.\n");
		return;
	}
	else if (netgame)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "Sorry, you can't spawn this while in a netgame.\n");
		return;
	}

	// Lua for the win!
	//// \todo lua for the loss
	SpawnTheDispenser = true;
}
