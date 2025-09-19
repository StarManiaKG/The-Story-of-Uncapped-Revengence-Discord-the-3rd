// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-debug.c
/// \brief TSoURDt3rd's debugging options

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

menuitem_t TSoURDt3rd_OP_DebugMenu[] =
{
	{IT_HEADER, NULL, "General", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Use the Automap Anywhere", &cv_tsourdt3rd_debug_automapanywhere, 0},
		{IT_STRING | IT_CVAR, NULL, "Always use the Virtual Keyboard", &cv_tsourdt3rd_debug_drrr_virtualkeyboard, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL, NULL, 0},

	{IT_HEADER, NULL, "Audio", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Keep Song Credits on-screen", &cv_songcredits_debug, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_DebugMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Enable the automap everywhere you go!", {NULL}, 0, 0},
		{NULL, "Always show virtual keyboard instead of using real keyboard input.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "When song credits are shown, keep them on screen until unloaded.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_DebugDef = {
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_DEBUGGING),
	NULL,
	sizeof (TSoURDt3rd_OP_DebugMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_DebugMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	24, 72,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_DebugDef = {
	TSoURDt3rd_TM_OP_DebugMenu,
	0, 0,
	0,
	NULL,
	2, 5,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef
};
