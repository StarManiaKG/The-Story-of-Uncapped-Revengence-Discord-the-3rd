// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-events.c
/// \brief TSoURDt3rd's event menu options

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_EventTicker(void);

menuitem_t TSoURDt3rd_OP_EventsMenu[] =
{
	{IT_HEADER, NULL, "Easter", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Egg Hunt", &cv_tsourdt3rd_easter_egghunt_allowed, 0},
		{IT_STRING | IT_CVAR, NULL, "Egg Hunt Bonuses", &cv_tsourdt3rd_easter_egghunt_bonuses, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL, NULL, 0},

	{IT_HEADER, NULL, "Foolishness", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Ultimate Mode!", &cv_tsourdt3rd_aprilfools_ultimatemode, 21},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_EventsMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Can TSoURDt3rd hold an egg hunt for you?", {NULL}, 0, 0},
		{NULL, "Enables the Easter Event Bonuses you earned.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Enable the super-duper Ultimate Mode!", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_EventsDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_EVENTS),
	NULL,
	sizeof (TSoURDt3rd_OP_EventsMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_EventsMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	48, 64,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_EventsDef = {
	TSoURDt3rd_TM_OP_EventsMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_EventTicker,
	M_Sys_EventTicker,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_EventTicker(void)
{
	INT32 i;

	TSoURDt3rd_M_OptionsTick();

	if (netgame || ((modifiedgame || usedCheats) && savemoddata) || tsourdt3rd_local.autoloaded_mods)
	{
		if (currenteggs != TOTALEGGS)
		{
			TSoURDt3rd_OP_EventsMenu[op_easter_egghunt].status = IT_CVAR|IT_STRING;
			TSoURDt3rd_OP_EventsMenu[op_easter_bonuses].status = IT_GRAYEDOUT;
		}
		else
			TSoURDt3rd_OP_EventsMenu[op_easter_bonuses].status = IT_CVAR|IT_STRING;
	}
	else
	{
		TSoURDt3rd_OP_EventsMenu[op_easter_egghunt].status = IT_GRAYEDOUT;
		TSoURDt3rd_OP_EventsMenu[op_easter_bonuses].status = IT_GRAYEDOUT;
	}

	if (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER))
	{
		for (i = op_easter_egghunt; i <= op_easter_bonuses; i++)
			TSoURDt3rd_OP_EventsMenu[i].status = IT_GRAYEDOUT;
	}
	else
	{
		for (i = op_easter_egghunt; i <= op_easter_bonuses; i++)
			TSoURDt3rd_OP_EventsMenu[i].status = IT_CVAR|IT_STRING;
	}

	if (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_APRILFOOLS))
	{
		for (i = op_aprilfools; i <= op_aprilfools_ultimatemode; i++)
			TSoURDt3rd_OP_EventsMenu[i].status = IT_DISABLED;
	}
	else
	{
		TSoURDt3rd_OP_EventsMenu[op_aprilfools].status = IT_HEADER;
		TSoURDt3rd_OP_EventsMenu[op_aprilfools_ultimatemode].status = IT_CVAR|IT_STRING;
	}
}
