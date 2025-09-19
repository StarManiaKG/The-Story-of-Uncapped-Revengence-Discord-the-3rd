// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-savedata.c
/// \brief TSoURDt3rd's savefile menu options

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_SavefileTicker(void);

menuitem_t TSoURDt3rd_OP_SavedataMenu[] =
{
	{IT_HEADER, NULL, "General", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Limited Continues", &cv_tsourdt3rd_savefiles_limitedcontinues, 0},
		{IT_STRING | IT_CVAR, NULL, "Store Saves in Folders", &cv_tsourdt3rd_savefiles_storesavesinfolders, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL, NULL, 0},

	{IT_HEADER, NULL, "Cosmetic", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Saves", &cv_tsourdt3rd_savefiles_perfectsave, 0},

		{IT_SPACE, NULL, NULL, NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Perfect Save Stripe 1", &cv_tsourdt3rd_savefiles_perfectsave_stripe1, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Save Stripe 2", &cv_tsourdt3rd_savefiles_perfectsave_stripe2, 0},
		{IT_STRING | IT_CVAR, NULL, "Perfect Save Stripe 3", &cv_tsourdt3rd_savefiles_perfectsave_stripe3, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_SavedataMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Allows for limited continues, like in old SRB2 versions.", {NULL}, 0, 0},
		{NULL, "Should TSoURDt3rd store our savefiles in folders?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Displays highlights on savefiles.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Color of the first savefile stripe.", {NULL}, 0, 0},
		{NULL, "Color of the second savefile stripe.", {NULL}, 0, 0},
		{NULL, "Color of the third savefile stripe.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_SavedataDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_SAVEDATA),
	NULL,
	sizeof (TSoURDt3rd_OP_SavedataMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_SavedataMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	48, 72,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_SavedataDef = {
	TSoURDt3rd_TM_OP_SavedataMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_SavefileTicker,
	NULL,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_SavefileTicker(void)
{
	TSoURDt3rd_M_OptionsTick();

	TSoURDt3rd_OP_SavedataMenu[op_general_limitedcontinues].status = (!Playing() ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	for (INT32 i = op_cosmetic_perfectsave_stripe1; i <= op_cosmetic_perfectsave_stripe3; i++)
		TSoURDt3rd_OP_SavedataMenu[i].status =
			(cv_tsourdt3rd_savefiles_perfectsave.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}
