// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  kk_hud.c
/// \brief HUD drawing functions exclusive to Kart

#include "khu_stuff.h"
#include "kk_hud.h"

//{ 	Patch Definitions
patch_t *kp_button_a[2][2];
patch_t *kp_button_b[2][2];
patch_t *kp_button_x[2][2];

static void K_LoadButtonGraphics(patch_t *kp[2], int letter)
{
	HU_UpdatePatch(&kp[0], "TLB_%c", letter);
	HU_UpdatePatch(&kp[1], "TLB_%cB", letter);
}

void K_LoadKartHUDGraphics(void)
{
    K_LoadButtonGraphics(kp_button_a[0], 'A');
	K_LoadButtonGraphics(kp_button_a[1], 'N');
	K_LoadButtonGraphics(kp_button_b[0], 'B');
	K_LoadButtonGraphics(kp_button_b[1], 'O');
	K_LoadButtonGraphics(kp_button_x[0], 'D');
	K_LoadButtonGraphics(kp_button_x[1], 'Q');
}
