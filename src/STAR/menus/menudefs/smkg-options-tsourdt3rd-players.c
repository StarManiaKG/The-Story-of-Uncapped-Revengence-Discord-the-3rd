// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-players.c
/// \brief TSoURDt3rd's player menu options

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

menuitem_t TSoURDt3rd_OP_PlayerMenu[] =
{
	{IT_HEADER, NULL, "Shields", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Shield Blocks Transformation",
			&cv_tsourdt3rd_players_shieldblockstransformation, 0},
		{IT_STRING | IT_CVAR, NULL, "Armageddon Nuke While Super",
			&cv_tsourdt3rd_players_nukewhilesuper, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Cosmetic", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Setup Player While Moving",
			&cv_tsourdt3rd_players_setupwhilemoving, 0},
		{IT_STRING | IT_CVAR, NULL, "Always Overlay Invuln. Sparks",
			&cv_tsourdt3rd_players_alwaysoverlayinvulnsparks, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_PlayerMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Having a shield blocks you from transforming.", {NULL}, 0, 0},
		{NULL, "You can use the armageddon shield while in a super form.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Allows the modification of your character while moving.", {NULL}, 0, 0},
		{NULL, "Should we always overlay the invincibility sparkles?", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_PlayerDef =
{
	MTREE2(MN_OP_MAIN, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_PlayerMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_PlayerMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	24, 72,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_PlayerDef = {
	TSoURDt3rd_TM_OP_PlayerMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef
};
