// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-game.c
/// \brief TSoURDt3rd's gameplay menu options

#include "../smkg-m_sys.h"

#include "../../../r_main.h"

#ifdef HAVE_CURL
#include "../../curl/smkg-curl.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_GameTicker(void);
static void G_CheckForTSoURDt3rdUpdates(INT32 choice);

menuitem_t TSoURDt3rd_OP_GameMenu[] =
{
	{IT_STRING | IT_CALL, NULL, "Check for Updates...",
		G_CheckForTSoURDt3rdUpdates, 0},
	{IT_STRING | IT_CVAR, NULL,	"Automatic Update Checking on Startup",
		&cv_tsourdt3rd_main_checkforupdatesonstartup, 0},

	{IT_SPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "SDL", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Window Title Type",
			&cv_tsourdt3rd_game_sdl_windowtitle_type, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "Custom Window Title",
			&cv_tsourdt3rd_game_sdl_windowtitle_custom, 0},
		{IT_STRING | IT_CVAR, NULL, "Memes on Window Title",
			&cv_tsourdt3rd_game_sdl_windowtitle_memes, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Scenes", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Startup Image",
			&cv_tsourdt3rd_game_startup_image, 0},
		{IT_STRING | IT_CVAR, NULL, "Startup Intro",
			&cv_tsourdt3rd_game_startup_intro, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Loading Screen",
			&cv_tsourdt3rd_game_loadingscreen, 0},
		{IT_STRING | IT_CVAR, NULL, "Loading Screen Image",
			&cv_tsourdt3rd_game_loadingscreen_image, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Pause Screen",
			&cv_tsourdt3rd_game_pausescreen, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Quit Screen",
			&cv_tsourdt3rd_game_quitscreen, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Levels", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Automap Outside Devmode",
			&cv_tsourdt3rd_debug_automapanywhere, 0},
		{IT_STRING | IT_CVAR, NULL, "Allow Time Overs",
			&cv_tsourdt3rd_game_allowtimeover, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Objects", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Shadows",
			&cv_shadow, 0},
		{IT_STRING | IT_CVAR, NULL, "Realistic Shadows",
			&cv_tsourdt3rd_game_shadows_realistic, 0},
		{IT_STRING | IT_CVAR, NULL, "All Objects Have Shadows",
			&cv_tsourdt3rd_game_shadows_forallobjects, 0},
		{IT_STRING | IT_CVAR, NULL, "Shadow Position",
			&cv_tsourdt3rd_game_shadows_positioning, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL,	"Sonic CD Mode",
			&cv_tsourdt3rd_game_soniccd, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Menus", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Is it Called 'Single Player'?",
			&cv_tsourdt3rd_game_isitcalledsingleplayer,	0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_GameMenu[] =
{
	{NULL, "Check for any updates to TSoURDt3rd.", {NULL}, 0, 0},
	{NULL, "Should TSoURDt3rd automatically check for updates?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "What should TSoURDt3rd's window title show?", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Define your perfect window title.", {NULL}, 0, 0},
		{NULL, "Can we show memes on your window title?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "The image to display when starting TSoURDt3rd.", {NULL}, 0, 0},
		{NULL, "Which STJr intro should we display?", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Show loading screens.", {NULL}, 0, 0},
		{NULL, "The image to display on the loading screen.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "The graphic to display while the game is paused.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "The image to display upon quitting TSoURDt3rd.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Should you be allowed to view the automap outside of devmode?", {NULL}, 0, 0},
		{NULL, "Are time-overs allowed?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Should objects have a drop shadow?", {NULL}, 0, 0},
		{NULL, "Should the shadows be 'realistic'?", {NULL}, 0, 0},
		{NULL, "Should all objects have a shadow?", {NULL}, 0, 0},
		{NULL, "How should each shadow be positioned?", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Should destroyed enemies free flowers instead of flickies?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "How should the '1-Player' mode option be referred to as?", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_GameDef =
{
	MTREE2(MN_OP_MAIN, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_GameMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_GameMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	24, 64,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_GameDef = {
	TSoURDt3rd_TM_OP_GameMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	M_Sys_GameTicker,
	NULL,
	NULL,
	NULL,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_GameTicker(void)
{
	INT32 i;

	TSoURDt3rd_M_OptionsTick();

#ifndef HAVE_SDL
	for (INT32 i = op_game_sdl_header; i <= op_game_sdl_windowtitle_memes; i++)
		TSoURDt3rd_OP_GameMenu[i].status = IT_DISABLED;
#else
	TSoURDt3rd_OP_GameMenu[op_game_sdl_windowtitle_custom].status =
		(cv_tsourdt3rd_game_sdl_windowtitle_type.value >= 2 ? IT_CVAR|IT_STRING|IT_CV_STRING : IT_GRAYEDOUT);
	TSoURDt3rd_OP_GameMenu[op_game_sdl_windowtitle_memes].status =
		(cv_tsourdt3rd_game_sdl_windowtitle_type.value == 1 ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
#endif

	TSoURDt3rd_OP_GameMenu[op_scenes_loadingscreen_image].status =
		(cv_tsourdt3rd_game_loadingscreen.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_GameMenu[op_levels_timeover].status =
		(!netgame ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	for (i = op_objects_shadows_realistic; i <= op_objects_shadows_positioning; i++)
		TSoURDt3rd_OP_GameMenu[i].status =
			(cv_shadow.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	TSoURDt3rd_OP_GameMenu[op_general_isitcalledsingleplayer].status =
		(!cv_tsourdt3rd_aprilfools_ultimatemode.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}

static void G_CheckForTSoURDt3rdUpdates(INT32 choice)
{
	(void)choice;
#ifdef HAVE_CURL
	tsourdt3rd_local.checked_version = false;
	TSoURDt3rd_CurlRoutine_FindUpdates();
#endif
}
