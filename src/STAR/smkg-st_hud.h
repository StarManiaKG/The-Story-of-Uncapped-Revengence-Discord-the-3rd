// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-st_hud.h
/// \brief TSoURDt3rd cool HUD data, global header

#include "../st_stuff.h"

// ------------------------ //
//        Variables
// ------------------------ //

extern patch_t *tsourdt3rd_easter_leveleggs;
extern patch_t *tsourdt3rd_easter_totaleggs;

extern patch_t *kp_button_a[2][2];
extern patch_t *kp_button_b[2][2];
extern patch_t *kp_button_x[2][2];

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_ST_LoadGraphics(void);

void TSoURDt3rd_Easter_ST_drawEggs(void);

#ifdef HAVE_DISCORDSUPPORT
// Called when you have Discord asks
void TSoURDt3rd_ST_AskToJoinEnvelope(void);
#endif

void TSoURDt3rd_SCR_DisplayTpsRate(void);
INT32 TSoURDt3rd_SCR_SetPingHeight(void);
