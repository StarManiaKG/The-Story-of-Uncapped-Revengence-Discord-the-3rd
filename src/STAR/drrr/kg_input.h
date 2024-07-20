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

#include "../../d_ticcmd.h" // ticcmd_t //
#include "../../g_input.h"
#include "../../i_joy.h"
#include "../../d_net.h" // MAXSPLITSCREENPLAYERS //

#ifdef __cplusplus
extern "C" {
#endif

/// Set the given player index's assigned device. If the device is in use by another player, that player is unassigned.
void G_SetDeviceForPlayer(INT32 player);

void G_SetPlayerGamepadIndicatorToPlayerColor(INT32 player);

extern consvar_t cv_tsourdt3rd_drrr_rumble[MAXSPLITSCREENPLAYERS];

void G_PlayerDeviceRumble(INT32 player, UINT16 low_strength, UINT16 high_strength);
#if 1
// STAR NOTE: NOT USED ANYWHERE ELSE FOR NOW //
void G_PlayerDeviceRumbleTriggers(INT32 player, UINT16 left_strength, UINT16 right_strength);
void G_ResetPlayerDeviceRumble(INT32 player);
void G_ResetAllDeviceRumbles(void);
#endif

/// STAR STUFF: Handle unique DRRR gamepad events ///
boolean STAR_G_MapEventsToControls(event_t *ev);
boolean STAR_G_BuildTiccmd(ticcmd_t *cmd, INT32 realtics, UINT8 ssplayer);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KG_INPUT__
