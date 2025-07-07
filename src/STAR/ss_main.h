// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_main.h
/// \brief Hosts TSoURDt3rd's main functions and properties

#ifndef __SS_MAIN__
#define __SS_MAIN__

#include "core/smkg-p_setup.h"

#include "../p_mobj.h"
#include "../s_sound.h"

// ------------------------ //
//        Variables
// ------------------------ //

typedef struct tsourdt3rd_s
{
	struct
	{
		boolean time_over;
	} game;
} tsourdt3rd_t;
extern tsourdt3rd_t tsourdt3rd[MAXPLAYERS];

extern struct tsourdt3rd_local_s
{
	boolean ms_address_changed;
	boolean autoloading_mods;
	boolean autoloaded_mods;

#ifdef HAVE_CURL
	struct
	{
		boolean checked_version;
	} curl;
#endif

	struct
	{
		tsourdt3rd_world_scenarios_t scenario;
		tsourdt3rd_world_scenarios_types_t scenario_types;
	} world;

	struct
	{
		boolean apply_effect;
		boolean disable_effect;
		boolean in_effect;
		float   prev_music_speed, prev_music_pitch;
		INT32   prev_music_volume, prev_sfx_volume;
		float   music_speed, music_pitch;
		INT32   music_volume, sfx_volume;
	} water_muffling;
} tsourdt3rd_local;

typedef enum
{
	TSOURDT3RD_EVENT_NONE       = 0,
	TSOURDT3RD_EVENT_EASTER     = 1<<1,
	TSOURDT3RD_EVENT_APRILFOOLS = 1<<2,
	TSOURDT3RD_EVENT_CHRISTMAS  = 1<<3
} tsourdt3rd_timedEvent_t;

extern tsourdt3rd_timedEvent_t tsourdt3rd_currentEvent;
extern boolean AllowEasterEggHunt, EnableEasterEggHuntBonuses;
extern musicdef_t tsourdt3rd_aprilfools_def;

extern struct tsourdt3rd_loadingscreen_s
{
	size_t loadCount;
	UINT8 loadPercentage;
	INT32 bspCount;
	INT32 screenToUse;
	boolean loadComplete;
} tsourdt3rd_loadingscreen;

typedef enum
{
	STAR_CONS_TSOURDT3RD = 1,
	STAR_CONS_TSOURDT3RD_NOTICE,
	STAR_CONS_TSOURDT3RD_ALERT,
	STAR_CONS_TSOURDT3RD_DEBUG,
	STAR_CONS_APRILFOOLS,
	STAR_CONS_EASTER,
	STAR_CONS_JUKEBOX
} tsourdt3rd_messagetype_t;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_Init(void);

void STAR_CONS_Printf(tsourdt3rd_messagetype_t starMessageType, const char *fmt, ...);
const char *TSoURDt3rd_CON_DrawStartupScreen(void);

const char *TSoURDt3rd_ReturnUsername(void);

// ======
// EVENTS
// ======

boolean TSoURDt3rd_Easter_AllEggsCollected(void);

boolean TSoURDt3rd_AprilFools_ModeEnabled(void);
void TSoURD3rd_AprilFools_OnChange(void);
void TSoURDt3rd_AprilFools_StoreDefaultMenuStrings(void);

// ======
// LEVELS
// ======

mobj_t *TSoURDt3rd_BossInMap(void);

// ======
// SCENES
// ======

void TSoURDt3rd_GameEnd(INT32 *timetonext);

#endif // __SS_MAIN__
