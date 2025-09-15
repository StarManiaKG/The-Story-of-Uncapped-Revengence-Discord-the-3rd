// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2025 by Louis-Antoine de Moulins de Rochefort.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  snake.h
/// \brief Snake minigame for the download screen.

#ifndef __SNAKE__
#define __SNAKE__

#include "d_event.h"

#define SPEED 5

#define NUM_BLOCKS_X 20
#define NUM_BLOCKS_Y 8
#define BLOCK_SIZE 12
#define BORDER_SIZE 12

#define MAP_WIDTH  (NUM_BLOCKS_X * BLOCK_SIZE)
#define MAP_HEIGHT (NUM_BLOCKS_Y * BLOCK_SIZE)

#define LEFT_X ((BASEVIDWIDTH - MAP_WIDTH) / 2 - BORDER_SIZE)
#define RIGHT_X (LEFT_X + MAP_WIDTH + BORDER_SIZE * 2 - 1)
#define BOTTOM_Y (BASEVIDHEIGHT - 76)
#define TOP_Y (BOTTOM_Y - MAP_HEIGHT - BORDER_SIZE * 2 + 1)

enum bonustype_s {
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

typedef struct snake_s
{
	boolean paused;
	boolean pausepressed;
	tic_t time;
	tic_t nextupdate;
	boolean gameover;
	UINT8 background;

	UINT16 snakelength;
	enum bonustype_s snakebonus;
	tic_t snakebonustime;
	UINT8 snakex[NUM_BLOCKS_X * NUM_BLOCKS_Y];
	UINT8 snakey[NUM_BLOCKS_X * NUM_BLOCKS_Y];
	UINT8 snakedir[NUM_BLOCKS_X * NUM_BLOCKS_Y];

	UINT8 applex;
	UINT8 appley;

	enum bonustype_s bonustype;
	UINT8 bonusx;
	UINT8 bonusy;

	event_t *joyevents[MAXEVENTS];
	UINT16 joyeventcount;
} snake_t;

void Snake_Allocate(void **opaque);
void Snake_Update(void *opaque);
void Snake_Draw(void *opaque);
void Snake_Free(void **opaque);
boolean Snake_JoyGrabber(void *opaque, event_t *ev);

#endif
