// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
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

//#define LISTENING_TRACK_ENABLED

#ifdef LISTENING_TRACK_ENABLED
static boolean listening_track = false;
#endif

static void M_Sys_AudioDrawer(void);
static boolean M_Sys_AudioInput(INT32 choice);

menuitem_t TSoURDt3rd_OP_AudioMenu[] =
{
	{IT_HEADER, NULL, "General", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Water Muffling",
			&cv_tsourdt3rd_audio_watermuffling, 0},
		{IT_STRING | IT_CVAR, NULL, "Vape Mode",
			&cv_tsourdt3rd_audio_vapemode, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Default Map Track",
			&cv_tsourdt3rd_audio_defaultmaptrack, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Levels", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Boss Music",
			&cv_tsourdt3rd_audio_bosses_bossmusic, 0},
		{IT_STRING | IT_CVAR, NULL, "Final Boss Music",
			&cv_tsourdt3rd_audio_bosses_finalboss, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "True Final Boss Music",
			&cv_tsourdt3rd_audio_bosses_truefinalboss, 0},
		{IT_STRING | IT_CVAR, NULL, "Pinch Music",
			&cv_tsourdt3rd_audio_bosses_pinch, 0},
		{IT_STRING | IT_CVAR, NULL, "Post Boss Music",
			&cv_tsourdt3rd_audio_bosses_postboss, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Act Clear Music",
			&cv_tsourdt3rd_audio_clearing_act, 0},
		{IT_STRING | IT_CVAR, NULL, "Boss Clear Music",
			&cv_tsourdt3rd_audio_clearing_boss, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Game Over Music",
			&cv_tsourdt3rd_audio_gameover, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_AudioMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Should water deafen and muffle audio?", {NULL}, 0, 0},
		{NULL, "If enabled, sets the speed and pitch at which music should play.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Maps that don't have tracks selected will play this instead.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Music that plays when a boss is present within a level.", {NULL}, 0, 0},
		{NULL, "Music that plays when a final boss is present within a level.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "The above option, but the music changes if you have all 7 emeralds.", {NULL}, 0, 0},
		{NULL, "Music that plays when a boss is on its last legs.", {NULL}, 0, 0},
		{NULL, "Music that plays once you've defeated a boss.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays once you've completed an act.", {NULL}, 0, 0},
		{NULL, "Music that plays once you've fully completed an act containing a boss.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when you get a game over.", {NULL}, 0, 0},
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
	M_Sys_AudioDrawer,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	M_Sys_AudioInput,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_AudioDrawer(void)
{
#ifdef LISTENING_TRACK_ENABLED
	fixed_t x = BASEVIDWIDTH/2, y = currentMenu->y;

	if (tsourdt3rd_itemOn < op_audio_levels)
		return;

	if (!listening_track)
	{
		V_DrawCenteredThinString(x, y+10, V_ALLOWLOWERCASE|V_MENUCOLORMAP, "Press 'Jump', 'Enter', or 'A' to take a listen!");
		V_DrawCharacter(x+64, y+10, V_GREENMAP, '\x1D');
	}
	else
		V_DrawCenteredThinString(x, y+10, V_ALLOWLOWERCASE|V_MENUCOLORMAP, "Press 'Spin', 'Escape', or 'B' to stop listening!");
#endif
}

#if 0
TSoURDt3rdBossMusic_t bossMusic[] = {
	[1] = {"_s1b",	NULL, 		0},	// Sonic 1
	{"_scdb", 		NULL, 		0},	// Sonic CD
	{"_s2b", 		NULL, 		0},	// Sonic 2
	{"_s3b1", 		NULL, 		0},	// Sonic 3
	{"_&kb1", 		NULL, 		0},	// & Knuckles
	{"_s3kb", 		NULL, 		0},	// Sonic 3 & Knuckles

	{"_a2b", 	 "_a2bp", 		0},	// Sonic Advance 2 - R
	{"_a27b", 	"_a27bp", 		0},	// Sonic Advance 2 - Z7
	{"_a3b", 	 "_a3bp", 		0},	// Sonic Advance 3 - R
	{"_a37b", 	"_a37bp", 		0},	// Sonic Advance 3 - Z7
	{"_rms", 	 "_rmsp", 		0},	// Sonic Rush - MS
	{"_rvn", 	 "_rvnp", 		0},	// Sonic Rush - VN
	{"_rab", 	 	NULL,	 5500},	// Rush Adventure - R
	{"_ra7b", 	 	NULL, 		0},	// Rush Adventure - Z7

	{"_smrd", 		NULL, 		0},	// Sonic Mania: RD
	{"_smhp", 		NULL, 		0},	// Sonic Mania: HP
	{"_smhbh", 		NULL, 		0}	// Sonic Mania: HBH
};

TSoURDt3rdFinalBossMusic_t finalBossMusic[] = {
	[1] = {"_s1fb",	NULL,	   	  NULL,		   NULL},	// Sonic 1
	{"_scdfb",	  	NULL,		  NULL,    	   NULL},	// Sonic CD
	{"_s2fb",	  	NULL,	   	  NULL,    	   NULL},	// Sonic 2
	{"_s3kfb",	  	NULL,	   "_&kdz",	 	   NULL},	// Sonic 3&K

	{"_a2fb",	"_a2fbp",	"	_a253",    	   NULL},	// Sonic Advance 2
	{"_a3fb",   "_a3fbp",  	   "_a3eb",    "_a3ebp"},	// Sonic Advance 3
	{"_rbb", 	 "_rbbp",      "_rwib",    "_rwibp"},	// Sonic Rush
	{"_rafb", 	"_rafbp",      "_radc",    "_radcp"},	// Rush Adventure

	{"_smri",	  	NULL,	   "_smer",    "_smerp"}	// Sonic Mania
};

static TSoURDt3rdActClearMusic_t actClearMusic[] = {
	[1] = {"_s12ac",NULL,	  	NULL, 	   	   NULL},	// Sonic 1&2
	{"_scdac", 	   	NULL,	  	NULL,		   NULL},	// Sonic CD
	{"_s3kac",	   	NULL,    "_s3fc",		"_&kfc"},	// Sonic 3&K

	{"_a2ac",	 "_a2bc",	"_a2fbc",	   "_a2tfb"},	// Sonic Advance 2
	{"_a3ac",	 "_a3bc",	"_a3fbc",	   "_a3tfb"},	// Sonic Advance 3
	{"_rac",	  "_rbc",    "_rfbc",      "_rtfbc"},	// Sonic Rush
	{"_raac",	 "_rabc",   "_rafbc",      "_ratfb"},	// Rush Adventure

	{"_smac",	   	NULL,	  	NULL,		   NULL},	// Sonic Mania

	{"_btsac",	   	NULL, 	  	NULL,		   NULL}	// Sonic BTS (Before the Sequel)
};

TSoURDt3rdDefaultMusicTracks_t defaultMusicTracks[] = {
	[1] = {"gfz1"},	// GFZ1
	{"_runin"}		// DooM Wad Anthem
};
#endif

static boolean M_Sys_AudioInput(INT32 choice)
{
#ifdef LISTENING_TRACK_ENABLED
	const UINT8 pid = 0;
	consvar_t *mus_cv = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;

	(void)choice;
	if (tsourdt3rd_itemOn < op_audio_levels)
		return false;

	if (TSoURDt3rd_M_MenuConfirmPressed(pid))
	{
		listening_track = true;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid) && listening_track)
	{
		listening_track = false;
		return true;
	}

	return listening_track;
#else
	(void)choice;
	return false;
#endif
}
