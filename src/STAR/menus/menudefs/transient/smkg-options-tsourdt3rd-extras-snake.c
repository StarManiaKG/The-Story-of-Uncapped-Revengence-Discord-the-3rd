// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/transient/smkg-options-tsourdt3rd-extras-snake.c
/// \brief TSoURDt3rd's freeplay snake minigame
//          Doesn't do anything useful, but is fun to play.

#include "../../smkg-m_sys.h"
#include "../../../../snake.h"

// ------------------------ //
//        Variables
// ------------------------ //

static snake_t *tsourdt3rd_snake = NULL;

static void M_Sys_DrawSnake(void);
static void M_Sys_SnakeTicker(void);
static boolean M_Sys_QuitSnake(void);
static boolean M_Sys_HandleSnake(INT32 choice);
static boolean M_Sys_SnakeResponder(event_t *ev);

menuitem_t TSoURDt3rd_OP_Extras_SnakeMenu[] =
{
	{IT_NOTHING, NULL, NULL, NULL, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_SnakeMenu[] =
{
	{NULL, "Hey! Listen!", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_Extras_SnakeDef =
{
	MTREE4(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_EXTRAS, MN_OP_TSOURDT3RD_EXTRAS_SNAKE),
	NULL,
	1,
	&TSoURDt3rd_OP_ExtrasDef,
	TSoURDt3rd_OP_Extras_SnakeMenu,
	M_Sys_DrawSnake,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_SnakeDef = {
	TSoURDt3rd_TM_OP_Extras_SnakeMenu,
	0, 0,
	0,
	NULL,
	0, 0,
	NULL,
	M_Sys_SnakeTicker,
	NULL,
	M_Sys_QuitSnake,
	M_Sys_HandleSnake,
	M_Sys_SnakeResponder,
	&TSoURDt3rd_TM_OP_ExtrasDef
};

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_M_Snake_Free(void)
{
	if (tsourdt3rd_snake)
		Snake_Free((void**)&tsourdt3rd_snake);
	tsourdt3rd_snake = NULL;

	memset(gamekeydown, 0, NUMKEYS);
}

boolean TSoURDt3rd_M_Snake_Init(INT32 choice)
{
	(void)choice;
	TSoURDt3rd_M_Snake_Free();
	Snake_Allocate((void**)&tsourdt3rd_snake);
	return (tsourdt3rd_snake != NULL);
}

static void M_Sys_DrawSnake(void)
{
	Snake_Draw((void*)tsourdt3rd_snake);

	// Draw background fade
	if (tsourdt3rd_snake->paused)
	{
		V_DrawFadeScreen(0xFF00, 16);
		F_TitleScreenDrawer();
		V_DrawCenteredString(BASEVIDWIDTH/2, BASEVIDHEIGHT/2, V_MENUCOLORMAP, "PAUSED");
	}

	// Draw quit text too obviously
	V_DrawRightAlignedString(BASEVIDWIDTH-4,
		BASEVIDHEIGHT-12,
		V_ALLOWLOWERCASE,
		"\x86""Press ""\x82""ESC""\x86"" to quit."
	);
}

static void M_Sys_SnakeTicker(void)
{
	Snake_Update(tsourdt3rd_snake);
}

static boolean M_Sys_QuitSnake(void)
{
	TSoURDt3rd_M_Snake_Free();
	return true;
}

static boolean M_Sys_HandleSnake(INT32 choice)
{
	const UINT8 pid = 0;
	(void)choice;
	return (!TSoURDt3rd_M_MenuBackPressed(pid));
}

static boolean M_Sys_SnakeResponder(event_t *ev)
{
	return (Snake_JoyGrabber(tsourdt3rd_snake, ev));
}
