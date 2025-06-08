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

// The following replicas of the snake struct are for entertainment purposes.
// Technically it's only used to check one thing but I've come so far now,
// so I may as well just leave it all here for the future.

enum FAKEbonustype_s {
	BONUS_NONE = 0,
	BONUS_SLOW,
	BONUS_FAST,
	BONUS_GHOST,
	BONUS_NUKE,
	BONUS_SCISSORS,
	BONUS_REVERSE,
	BONUS_EGGMAN,
	NUM_BONUSES,
};

typedef struct FAKEsnake_s
{
	boolean paused;
	boolean pausepressed;
	tic_t time;
	tic_t nextupdate;
	boolean gameover;
	UINT8 background;

	UINT16 snakelength;
	enum FAKEbonustype_s snakebonus;
	tic_t snakebonustime;
	UINT8 snakex[20 * 10];
	UINT8 snakey[20 * 10];
	UINT8 snakedir[20 * 10];

	UINT8 applex;
	UINT8 appley;

	enum FAKEbonustype_s bonustype;
	UINT8 bonusx;
	UINT8 bonusy;

	event_t *joyevents[MAXEVENTS];
	UINT16 joyeventcount;
} FAKEsnake_t;

void *tsourdt3rd_snake = NULL;
FAKEsnake_t *tsourdt3rd_real_snake = NULL;

static void M_Sys_DrawSnake(void);
static void M_Sys_SnakeTicker(void);
static void M_Sys_InitSnake(void);
static boolean M_Sys_QuitSnake(void);
static boolean M_Sys_HandleSnake(INT32 choice);

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
	MTREE4(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_SNAKE, MN_OP_TSOURDT3RD_SNAKE),
	NULL,
	1,
	&TSoURDt3rd_OP_ExtrasDef,
	TSoURDt3rd_OP_Extras_SnakeMenu,
	M_Sys_DrawSnake,
	0, 0,
	0,
	M_Sys_QuitSnake
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_SnakeDef = {
	TSoURDt3rd_TM_OP_Extras_SnakeMenu,
	0, 0,
	0,
	NULL,
	0, 0,
	NULL,
	M_Sys_SnakeTicker,
	M_Sys_InitSnake,
	NULL,
	M_Sys_HandleSnake,
	&TSoURDt3rd_TM_OP_ExtrasDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_DrawSnake(void)
{
	if (!tsourdt3rd_snake || !tsourdt3rd_real_snake)
		return;
	Snake_Draw(tsourdt3rd_snake);

	// Draw background fade
	if (tsourdt3rd_real_snake->paused)
	{
		V_DrawFadeScreen(0xFF00, 16);
		F_TitleScreenDrawer();
		V_DrawCenteredString(BASEVIDWIDTH/2, BASEVIDHEIGHT/2, V_MENUCOLORMAP, "PAUSED");
	}

	// Draw quit text too obviously
	V_DrawRightAlignedString(
		BASEVIDWIDTH-4,
		BASEVIDHEIGHT-12,
		V_ALLOWLOWERCASE,
		"\x86""Press ""\x82""ESC""\x86"" to quit."
	);
}

static void M_Sys_SnakeTicker(void)
{
	tsourdt3rd_real_snake = (FAKEsnake_t *)tsourdt3rd_snake;
	memcpy(tsourdt3rd_real_snake, tsourdt3rd_snake, sizeof(FAKEsnake_t));
	Snake_Update(tsourdt3rd_snake);
}

static void M_Sys_InitSnake(void)
{
	if (tsourdt3rd_snake)
	{
		Snake_Free(&tsourdt3rd_snake);
		tsourdt3rd_snake = NULL;
	}

	if (tsourdt3rd_real_snake)
		tsourdt3rd_real_snake = NULL;

	memset(gamekeydown, 0, NUMKEYS);
	Snake_Allocate(&tsourdt3rd_snake);
}

static boolean M_Sys_QuitSnake(void)
{
	Snake_Free(&tsourdt3rd_snake);
	tsourdt3rd_snake = NULL;
	tsourdt3rd_real_snake = NULL;
	return true;
}

static boolean M_Sys_HandleSnake(INT32 choice)
{
	const UINT8 pid = 0;
	(void)choice;
	return (!TSoURDt3rd_M_MenuBackPressed(pid));
}
