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
/// \file  menus/menudefs/smkg-options-tsourdt3rd-events.c
/// \brief TSoURDt3rd's event menu options

#include "../../m_menu.h"

#include "../../../d_main.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_EventTicker(void);

static menuitem_t TSoURDt3rd_OP_EventsMenu[] =
{
	{IT_HEADER, NULL, "Easter", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Egg Hunt",
			&cv_easter_allowegghunt, 0},
		{IT_STRING | IT_CVAR, NULL, "Egg Hunt Bonuses",
			&cv_easter_egghuntbonuses, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Secret TSoURDt3rd Unlockables", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Ultimate Mode!",
			&cv_tsourdt3rd_aprilfools_ultimatemode, 21},
};

static tsourdt3rd_menuitems_t TSoURDt3rd_TM_OP_EventsMenu[] =
{
	{NULL, NULL, 0, 0},
	{NULL, "Can TSoURDt3rd hold an egg hunt for you?", 0, 0},
	{NULL, "Enables the bonuses you earned from mastering the Easter event.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Enable the super-duper Ultimate Mode!", 0, 0},
};

menu_t TSoURDt3rd_OP_EventsDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_EventsMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_EventsMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_EventsDef = {
	&TSoURDt3rd_OP_EventsDef,
	TSoURDt3rd_TM_OP_EventsMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_EventTicker,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef,
	NULL
};

enum
{
	op_easter,
	op_easter_egghunt,
	op_easter_bonuses,

	op_aprilfools = 4,
	op_aprilfools_ultimatemode,
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_EventTicker(void)
{
	INT32 i;

	TSoURDt3rd_M_OptionsTick();

	if (netgame || TSoURDt3rd_NoMoreExtras || autoloaded)
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

	if (!eastermode)
	{
		for (i = op_easter; i <= op_easter_bonuses; i++)
			TSoURDt3rd_OP_EventsMenu[i].status = IT_DISABLED;
	}
	else
	{
		TSoURDt3rd_OP_EventsMenu[op_easter].status = (eastermode ? IT_HEADER : IT_GRAYEDOUT);
		TSoURDt3rd_OP_EventsMenu[op_easter_egghunt].status =
			(eastermode ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
		TSoURDt3rd_OP_EventsMenu[op_easter_bonuses].status =
			(eastermode ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	}

	if (!aprilfoolsmode)
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
