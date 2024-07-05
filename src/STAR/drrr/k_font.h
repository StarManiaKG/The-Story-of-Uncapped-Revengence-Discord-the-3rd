// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by James Robert Roman.
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2018 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_font.h
/// \brief Font setup

#ifndef __KFONT_H__
#define __KFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FONTS 32

typedef struct font_s
{
	patch_t **font;

	UINT8     start;
	UINT8     size;

	char      prefix[8];/* 7 used at most */
	unsigned  digits : 2;
} font_t;

extern font_t fontv[MAX_FONTS];
extern int    fontc;

/*
Reloads already registered fonts.
*/
void Font_Load (void);

/*
Registers and loads a new font.
*/
int Font_Register     (const font_t *);

/*
Register a new font, but do not load it yet.
*/
int Font_DumbRegister (const font_t *);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
