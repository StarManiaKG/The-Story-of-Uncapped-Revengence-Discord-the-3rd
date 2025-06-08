// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-audio.c
/// \brief TSoURDt3rd's audio menu options

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

menuitem_t TSoURDt3rd_OP_AudioMenu[] =
{
	{IT_HEADER, NULL, "General", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Water Muffling",
			&cv_tsourdt3rd_audio_watermuffling, 0},
		{IT_STRING | IT_CVAR, NULL, "Vape Mode",
			&cv_tsourdt3rd_audio_vapemode, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Post Boss Music",
			&cv_tsourdt3rd_audio_bosses_postboss, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_STRING | IT_CALL, NULL, "EXMusic...",
		TSoURDt3rd_M_EXMusic_LoadMenu, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_AudioMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Should water deafen and muffle audio?", {NULL}, 0, 0},
		{NULL, "If enabled, sets the speed and pitch at which music should play.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Should different music play once you've defeated a boss?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, "Configure the EXMusic library.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_AudioDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_AudioMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_AudioMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	24, 64,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_AudioDef = {
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
	&TSoURDt3rd_TM_OP_MainMenuDef
};
