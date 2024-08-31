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
/// \file  menus/menudefs/smkg-options-audio.c
/// \brief TSoURDt3rd's audio menu options

#include "../../m_menu.h"

// ------------------------ //
//        Variables
// ------------------------ //

menuitem_t TSoURDt3rd_OP_AudioMenu[] =
{
	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "General", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Water Muffling",
			&cv_tsourdt3rd_audio_watermuffling, 0},
		{IT_STRING | IT_CVAR, NULL, "Vape Mode",
			&cv_vapemode, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Default Map Track",
			&cv_defaultmaptrack, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Levels", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Boss Music",
			&cv_bossmusic, 0},
		{IT_STRING | IT_CVAR, NULL, "Final Boss Music",
			&cv_finalbossmusic, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "True Final Boss Music",
			&cv_truefinalbossmusic, 0},
		{IT_STRING | IT_CVAR, NULL, "Pinch Music",
			&cv_bosspinchmusic, 0},
		{IT_STRING | IT_CVAR, NULL, "Post Boss Music",
			&cv_postbossmusic, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Act Clear Music",
			&cv_actclearmusic, 0},
		{IT_STRING | IT_CVAR, NULL, "Boss Clear Music",
			&cv_bossclearmusic, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Game Over Music",
			&cv_gameovermusic, 0},
};

static tsourdt3rd_menuitems_t TSoURDt3rd_TM_OP_AudioMenu[] =
{
	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Should water deafen and muffle audio?", 0, 0},
	{NULL, "If enabled, sets the speed and pitch at which music should play.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Maps that don't have tracks selected will play this instead.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, NULL, 0, 0},
	{NULL, "Music that plays when a boss is present within a level.", 0, 0},
	{NULL, "Music that plays when a final boss is present within a level.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "The above option, but the music changes if you have all 7 emeralds.", 0, 0},
	{NULL, "Music that plays when a boss is on its last legs.", 0, 0},
	{NULL, "Music that plays once you've defeated a boss.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Music that plays once you've completed an act.", 0, 0},
	{NULL, "Music that plays once you've fully completed an act containing a boss.", 0, 0},

	{NULL, NULL, 0, 0},

	{NULL, "Music that plays when you get a game over.", 0, 0},
};

menu_t TSoURDt3rd_OP_AudioDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_AudioMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_AudioMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_AudioDef = {
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_TM_OP_AudioMenu,
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
