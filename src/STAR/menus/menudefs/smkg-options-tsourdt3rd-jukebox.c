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
/// \file  menus/menudefs/smkg-options-tsourdt3rd-jukebox.c
/// \brief TSoURDt3rd's jukebox menu options

#include "../../m_menu.h"

// ------------------------ //
//        Variables
// ------------------------ //

static menuitem_t TSoURDt3rd_OP_JukeboxMenu[] =
{
	{IT_STRING | IT_CALL, NULL, "Enter Jukebox...",
		TSoURDt3rd_Jukebox_InitMenu, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Misc.", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Jukebox HUD",
			&cv_jukeboxhud, 0},
		{IT_STRING | IT_CVAR, NULL, "Lua Can Stop The Jukebox",
			&cv_luacanstopthejukebox, 0},
};

static tsourdt3rd_menuitems_t TSoURDt3rd_TM_OP_JukeboxMenu[] =
{
	{NULL, "Enter the Jukebox and play some tracks!", 0, 0},

	{NULL, NULL, 128, 128},

	{NULL, NULL, 0, 0},
	{NULL, "Should a HUD appear whenever you're using the Jukebox?", 0, 0},
	{NULL, "Can Lua scripts stop the music you're playing in the Jukebox?", 0, 0},
};

menu_t TSoURDt3rd_OP_JukeboxDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_JukeboxMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_JukeboxMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_JukeboxDef = {
	&TSoURDt3rd_OP_JukeboxDef,
	TSoURDt3rd_TM_OP_JukeboxMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef,
	NULL
};
