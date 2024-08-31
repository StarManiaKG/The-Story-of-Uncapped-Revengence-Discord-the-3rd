// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  kg_input.h
/// \brief handle mouse/keyboard/joystick inputs,
///        maps inputs to game controls (forward, spin, jump...)

#ifndef __KG_INPUT__
#define __KG_INPUT__

#include "../../d_ticcmd.h"
#include "../../d_player.h"
#include "../../g_input.h"
#include "../../i_joy.h"
#include "../../d_net.h" // MAXSPLITSCREENPLAYERS //

#ifdef __cplusplus
extern "C" {
#endif

void G_SetPlayerGamepadIndicatorToPlayerColor(INT32 player);

extern consvar_t cv_tsourdt3rd_ctrl_drrr_rumble[MAXSPLITSCREENPLAYERS];

void TSoURDt3rd_Pads_G_PlayerDeviceRumble(player_t *player, fixed_t low_strength, fixed_t high_strength);
#if 1
// STAR NOTE: NOT USED ANYWHERE ELSE FOR NOW //
void TSoURDt3rd_Pads_G_PlayerDeviceRumbleTriggers(player_t *player, fixed_t left_strength, fixed_t right_strength);
void G_ResetPlayerDeviceRumble(INT32 player);
void G_ResetAllDeviceRumbles(void);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KG_INPUT__
