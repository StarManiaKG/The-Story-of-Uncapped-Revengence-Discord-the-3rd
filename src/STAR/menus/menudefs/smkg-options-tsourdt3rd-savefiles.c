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
/// \file  menus/menudefs/smkg-options-tsourdt3rd-savefiles.c
/// \brief TSoURDt3rd's savefile menu options

#include "../../m_menu.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_SavefileTicker(void);

static menuitem_t TSoURDt3rd_OP_SavefileMenu[] =
{
	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Savedata", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Store Saves in Folders",
			&cv_tsourdt3rd_savefiles_storesavesinfolders, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Savefiles", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Saves",
			&cv_tsourdt3rd_savefiles_perfectsave, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Save Stripe 1",
			&cv_tsourdt3rd_savefiles_perfectsavestripe1, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Save Stripe 2",
			&cv_tsourdt3rd_savefiles_perfectsavestripe2, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Save Stripe 3",
			&cv_tsourdt3rd_savefiles_perfectsavestripe3, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Continues",
			&cv_continues, 0},
};

static tsourdt3rd_menuitems_t TSoURDt3rd_TM_OP_SavefileMenu[] =
{
	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Should TSoURDt3rd store our savefiles in folders?", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Displays highlights on savefiles.", 0, 0},
	{NULL, "Color of the first savefile stripe.", 0, 0},
	{NULL, "Color of the second savefile stripe.", 0, 0},
	{NULL, "Color of the third savefile stripe.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Should savefiles have limited or unlimited continues?", 0, 0},
};

menu_t TSoURDt3rd_OP_SavefileDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_SavefileMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_SavefileMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_SavefileDef = {
	&TSoURDt3rd_OP_SavefileDef,
	TSoURDt3rd_TM_OP_SavefileMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_SavefileTicker,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef,
	NULL
};

enum
{
	op_savefiles_perfectsavestripe1 = 6,
	op_savefiles_perfectsavestripe2,
	op_savefiles_perfectsavestripe3,

	op_savefiles_continues = 10
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_SavefileTicker(void)
{
	TSoURDt3rd_M_OptionsTick();

	for (INT32 i = op_savefiles_perfectsavestripe1; i <= op_savefiles_perfectsavestripe3; i++)
		TSoURDt3rd_OP_SavefileMenu[i].status =
			(cv_tsourdt3rd_savefiles_perfectsave.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_SavefileMenu[op_savefiles_continues].status = (!Playing() ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}
