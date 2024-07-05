// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_purefat.h
/// \brief Pure Fat intro 2.2.7+ compatibility layer, global header

#ifndef __SS_PUREFAT__
#define __SS_PUREFAT__

#include "../doomstat.h"
#include "../v_video.h"

// ------------------------ //
//        Functions
// ------------------------ //

void STAR_F_StartIntro(tic_t *introtime);

void STAR_F_PureFatDrawer(char *stjrintro, patch_t *background, void *patch, INT32 intro_scenenum, INT32 finalecount, INT32 bgxoffs);
void STAR_F_PureFatTicker(INT32 intro_scenenum, INT32 intro_curtime, INT32 animtimer);

#endif // __SS_PUREFAT__
