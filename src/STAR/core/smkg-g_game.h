// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-g_game.h
/// \brief TSoURDt3rd's game data

#ifndef __SMKG_G_GAME__
#define __SMKG_G_GAME__

#include "../smkg-defs.h"
#include "smkg-p_setup.h"

#include "../../g_game.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct tsourdt3rd_s
{
	struct {
		boolean time_over;
	} game;
} tsourdt3rd_t;
extern tsourdt3rd_t tsourdt3rd[MAXPLAYERS];

extern struct tsourdt3rd_local_s
{
	UINT8 major_version;
	UINT8 minor_version;
	UINT8 sub_version;

	boolean ms_address_changed;
	boolean autoloading_mods;
	boolean autoloaded_mods;

#ifdef HAVE_CURL
	struct {
		boolean checked_version;
	} curl;
#endif

	struct {
		tsourdt3rd_world_scenarios_t scenario;
		tsourdt3rd_world_scenarios_types_t scenario_types;
		boolean gameover;
	} world;

	struct  {
		boolean apply_effect;
		boolean disable_effect;
		boolean in_effect;
		float   prev_music_speed, prev_music_pitch;
		INT32   prev_music_volume, prev_sfx_volume;
		float   music_speed, music_pitch;
		INT32   music_volume, sfx_volume;
	} water_muffling;
} tsourdt3rd_local;

extern tsourdt3rd_gamedata_t *tsourdt3rd_client_gamedata;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_G_SetSaveDirectory(const char *new_directory);
void TSoURDt3rd_G_UpdateSaveDirectory(void);

void TSoURDt3rd_G_SaveGamedata(void);
void TSoURDt3rd_G_LoadGameData(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_G_GAME__
