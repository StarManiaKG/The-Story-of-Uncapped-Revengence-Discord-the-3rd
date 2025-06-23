// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-video.c
/// \brief TSoURDt3rd's video menu options

#include "../smkg-m_sys.h"

#include "../../../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_VideoTicker(void);

menuitem_t TSoURDt3rd_OP_VideoMenu[] =
{
	{IT_HEADER, NULL, "SDL", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Window Shaking",
			&cv_tsourdt3rd_video_sdl_window_shaking, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Diagnostic", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Show TPS",
			&cv_tsourdt3rd_video_showtps, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Flair", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Menu Color",
			&cv_tsourdt3rd_video_coloring_menus, 81},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "FPS Counter Color",
			&cv_tsourdt3rd_video_coloring_fpsrate, 0},
		{IT_STRING | IT_CVAR, NULL, "FPS Counter Font",
			&cv_tsourdt3rd_video_font_fps, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "TPS Counter Color",
			&cv_tsourdt3rd_video_coloring_tpsrate, 0},
		{IT_STRING | IT_CVAR, NULL, "TPS Counter Font",
			&cv_tsourdt3rd_video_font_tps, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

#ifdef STAR_LIGHTING
	{IT_HEADER, NULL, "Advanced", NULL, 0},
		{IT_STRING | IT_CALL, NULL, "Lighting...",
			TSoURDt3rd_M_CoronaLighting_Init, 0},
#endif
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_VideoMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "The game window is allowed to shake during certain events.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Displays a counter that calculates game tics.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Changes the color of menu highlights.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Changes the color of the FPS counter.", {NULL}, 0, 0},
		{NULL, "Changes the font of the FPS counter.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Changes the color of the TPS counter.", {NULL}, 0, 0},
		{NULL, "Changes the font of the TPS counter.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

#ifdef STAR_LIGHTING
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Browse TSoURDt3rd's lighting effects.", { NULL }, 0, 0},
#endif
};

menu_t TSoURDt3rd_OP_VideoDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_VideoMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_VideoMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	48, 72,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_VideoDef = {
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
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_VideoTicker(void)
{
	TSoURDt3rd_M_OptionsTick();

#ifndef HAVE_SDL
	for (INT32 i = op_video_sdl_header; i <= op_video_sdl_window_shaking; i++)
		TSoURDt3rd_OP_GameMenu[i].status = IT_DISABLED;
#endif

	TSoURDt3rd_OP_VideoMenu[op_video_flair_fpscolor].status = (cv_ticrate.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	TSoURDt3rd_OP_VideoMenu[op_video_flair_fpsfont].status = (cv_ticrate.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_VideoMenu[op_video_flair_tpscolor].status = (cv_tsourdt3rd_video_showtps.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	TSoURDt3rd_OP_VideoMenu[op_video_flair_tpsfont].status = (cv_tsourdt3rd_video_showtps.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}
