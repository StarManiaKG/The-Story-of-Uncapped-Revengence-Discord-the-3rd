// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Original Copyright (C) 1998-2000 by DooM Legacy Team.
// Original Copyright (C) 1999-2023 by Sonic Team Junior.
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-video-lighting.c
/// \brief TSoURDt3rd's lighting options

#include "../smkg-m_sys.h"

#ifdef STAR_LIGHTING

#include "../../../hardware/hw_main.h"

// ------------------------ //
//        Variables
// ------------------------ //

static menu_t *last_menu_reference = NULL;
static tsourdt3rd_menu_t *last_tsourdt3rd_menu_reference = NULL;

static boolean M_Sys_HandleLighting(INT32 choice);

menuitem_t TSoURDt3rd_OP_Video_LightingMenu[] =
{
	{IT_STRING | IT_CVAR, NULL, "Coronas",
		&cv_tsourdt3rd_video_lighting_coronas, 0},
	{IT_STRING | IT_CVAR, NULL, "Corona Size",
		&cv_tsourdt3rd_video_lighting_coronas_size, 0},
	{IT_STRING | IT_CVAR, NULL, "Corona Lighting Type",
		&cv_tsourdt3rd_video_lighting_coronas_lightingtype, 0},
	{IT_STRING | IT_CVAR, NULL, "Corona Drawing Mode",
		&cv_tsourdt3rd_video_lighting_coronas_drawingmode, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Video_LightingMenu[] =
{
	{NULL, "Enable or disable corona lighting.", {NULL}, 0, 0},
	{NULL, "Change the size of the coronas that appear.", {NULL}, 0, 0},
	{NULL, "Do you want static, sprite based coronas, or dynamic coronas?", {NULL}, 0, 0},
	{NULL, "The drawing mode of the coronas.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_Video_LightingDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_Video_LightingMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_Video_LightingDef,
	TSoURDt3rd_OP_Video_LightingMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	48, 76,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Video_LightingDef = {
	TSoURDt3rd_TM_OP_Video_LightingMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	NULL,
	TSoURDt3rd_M_OptionsTick,
	NULL,
	NULL,
	M_Sys_HandleLighting,
	&TSoURDt3rd_TM_OP_VideoDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static boolean M_Sys_HandleLighting(INT32 choice)
{
	const UINT8 pid = 0;
	(void)choice;

	if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		TSoURDt3rd_M_SetupNextMenu(last_tsourdt3rd_menu_reference, last_menu_reference, false);
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	return false;
}

void TSoURDt3rd_M_CoronaLighting_Init(void)
{
	// Let's prevent the game from sending us to the wrong menu, shall we?
	last_tsourdt3rd_menu_reference = tsourdt3rd_currentMenu;
	last_menu_reference = currentMenu;

	if (last_tsourdt3rd_menu_reference != NULL)
		TSoURDt3rd_TM_OP_Video_LightingDef.music = last_tsourdt3rd_menu_reference->music;
	else
		TSoURDt3rd_M_ResetOptions();

	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_Video_LightingDef, &TSoURDt3rd_OP_Video_LightingDef, false);
	if (last_menu_reference == &TSoURDt3rd_OP_Video_LightingDef)
		TSoURDt3rd_OP_Video_LightingDef.lastOn = 0;

	optionsmenu.ticker = 0;
	TSoURDt3rd_M_OptionsTick();
}

#endif // STAR_LIGHTING
