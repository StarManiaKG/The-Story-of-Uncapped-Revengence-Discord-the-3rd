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

#include "../p_mobj.h"
#include "../s_sound.h"

// ------------------------ //
//        Variables
// ------------------------ //

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

// ------------------------ //
//        Functions
// ------------------------ //

#define STAR_CONS_NONE              0x0001
#define STAR_CONS_TSOURDT3RD        0x0002
#define STAR_CONS_DISCORD           0x0004

#define STAR_CONS_NOTICE            0x0008
#define STAR_CONS_ERROR             0x0010
#define STAR_CONS_WARNING           0x0020
#define STAR_CONS_COLORWHOLELINE    0x0040

#define STAR_CONS_APRILFOOLS        0x0080
#define STAR_CONS_EASTER            0x0100
#define STAR_CONS_JUKEBOX           0x0200

#define STAR_CONS_DEBUG             0x0400

void STAR_CONS_Printf(INT32 message_type, const char *fmt, ...);

const char *TSoURDt3rd_CON_DrawStartupScreen(void);

const char *TSoURDt3rd_ReturnUsername(void);

// ======
// EVENTS
// ======

boolean TSoURDt3rd_Easter_AllEggsCollected(void);

boolean TSoURDt3rd_AprilFools_ModeEnabled(void);
void TSoURDt3rd_AprilFools_ManageSaveData(void);
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
