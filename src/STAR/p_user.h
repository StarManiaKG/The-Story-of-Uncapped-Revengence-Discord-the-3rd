// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  p_user.h
/// \brief TSoURDt3rd's player functions and data, globalized

#ifndef __STAR_P_USER__
#define __STAR_P_USER__

#include "../d_player.h"

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_P_MovingPlayerSetup(INT32 playernum);

boolean TSoURDt3rd_P_DeathThink(player_t *player);
void TSoURDt3rd_P_PlayerThink(player_t *player);
void TSoURDt3rd_P_Ticker(boolean run);

boolean TSoURDt3rd_P_DamageMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, INT32 damage, UINT8 damagetype);
void TSoURDt3rd_P_KillMobj(mobj_t *target, mobj_t *inflictor, mobj_t *source, UINT8 damagetype);

boolean TSoURDt3rd_P_ControlShieldOverlay(mobj_t *thing);
boolean TSoURDt3rd_P_PlayerShieldThink(player_t *player, ticcmd_t *cmd, mobj_t *lockonthok, mobj_t *visual);
boolean TSoURDt3rd_P_SuperReady(player_t *player);

#endif // __STAR_P_USER__
