// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-st_hud.h
/// \brief Globalizes TSoURDt3rd's cool HUD data

#ifndef __SMKG_ST_HUD__
#define __SMKG_ST_HUD__

#include "../st_stuff.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct tsourdt3rd_trackingResult_s
{
	fixed_t x, y;
	fixed_t scale;
	boolean onScreen;
	INT32 angle, pitch;
	fixed_t fov;
} tsourdt3rd_trackingResult_t;

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

void TSoURDt3rd_SCR_CalculateTPS(void);
void TSoURDt3rd_SCR_DisplayTPS(void);

INT32 TSoURDt3rd_SCR_SetPingHeight(INT32 *y);

void TSoURDt3rd_ST_ObjectTracking(player_t *player, tsourdt3rd_trackingResult_t *result, const vector3_t *point, boolean object_reverse);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_ST_HUD__
