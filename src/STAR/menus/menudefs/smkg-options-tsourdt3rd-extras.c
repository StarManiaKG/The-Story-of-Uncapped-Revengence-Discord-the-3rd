// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-extras.c
/// \brief TSoURDt3rd's unique extra options

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_SpawnTF2Dispenser(INT32 choice);

menuitem_t TSoURDt3rd_OP_ExtrasMenu[] =
{
	{IT_STRING | IT_CALL, NULL, "Jukebox",
		TSoURDt3rd_M_InitJukebox, 0},
	{IT_STRING | IT_SUBMENU, NULL, "Play Snake",
		&TSoURDt3rd_OP_Extras_SnakeDef, 0},
	{IT_STRING | IT_CALL, NULL, "Dispenser Goin' Up",
		M_Sys_SpawnTF2Dispenser, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ExtrasMenu[] =
{
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

	SpawnTheDispenser = true;
}
