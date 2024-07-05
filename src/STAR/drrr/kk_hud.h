// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  kk_hud.h
/// \brief HUD drawing functions exclusive to Kart

#ifndef __KK_HUD__
#define __KK_HUD__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../v_video.h"

extern patch_t *kp_button_a[2][2];
extern patch_t *kp_button_b[2][2];
extern patch_t *kp_button_x[2][2];

void K_LoadKartHUDGraphics(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KK_HUD__
