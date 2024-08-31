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
/// \file  menus/menudefs/smkg-options-tsourdt3rd-video.c
/// \brief TSoURDt3rd's video menu options

#include "../../m_menu.h"

#include "../../../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_VideoTicker(void);

static menuitem_t TSoURDt3rd_OP_VideoMenu[] =
{
	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

    {IT_HEADER, NULL, "Diagnostic", NULL, 0},
        {IT_STRING | IT_CVAR, NULL, "Show TPS",
            &cv_tsourdt3rd_video_showtps, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

    {IT_HEADER, NULL, "Flair", NULL, 0},
        {IT_STRING | IT_CVAR, NULL, "Menu Color",
		    &cv_menucolor, 81},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

        {IT_STRING | IT_CVAR, NULL, "FPS Counter Color",
            &cv_fpscountercolor, 0},
        {IT_STRING | IT_CVAR, NULL, "TPS Counter Color",
            &cv_tpscountercolor, 0},
};

static tsourdt3rd_menuitems_t TSoURDt3rd_TM_OP_VideoMenu[] =
{
	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Displays a counter next to the FPS counter, that calculates game tics.", 0, 0},

	{NULL, NULL, 0, 0},

    {NULL, NULL, 0, 0},
	{NULL, "Changes the color of menu highlights.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Changes the color of the FPS counter.", 0, 0},
	{NULL, "Changes the color of the TPS counter.", 0, 0},
};

menu_t TSoURDt3rd_OP_VideoDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_VideoMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_VideoMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_VideoDef = {
	&TSoURDt3rd_OP_VideoDef,
	TSoURDt3rd_TM_OP_VideoMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_VideoTicker,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef,
	NULL
};

enum
{
	op_flair_fpscolor = 7,
	op_flair_tpscolor
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_VideoTicker(void)
{
	TSoURDt3rd_M_OptionsTick();

    TSoURDt3rd_OP_VideoMenu[op_flair_fpscolor].status = (cv_ticrate.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
    TSoURDt3rd_OP_VideoMenu[op_flair_tpscolor].status = (cv_tsourdt3rd_video_showtps.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}
