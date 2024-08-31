// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ss_main.h
/// \brief Hosts TSoURDt3rd's main functions and properties

#ifndef __SS_MAIN__
#define __SS_MAIN__

#include "../doomtype.h"
#include "../p_mobj.h"
#include "../m_menu.h"
#include "../s_sound.h"

// ------------------------ //
//        Variables
// ------------------------ //

#define TSOURDT3RD_DEBUGGING /* Debugging */

#define AUTOLOADCONFIGFILENAME "autoload.cfg"

#define SAVEGAMEFOLDER "saves"
extern char savegamefolder[256];

//#define ALAM_LIGHTING /* EXTREME BETA: Enable Alam's Lighting, With Touch-ups by Star */

//#define _DEBUG /* Debugging */
//#define DEBUGFILE /* Debugging */

// ======
// EVENTS
// ======

typedef enum
{
	STAR_CONS_TSOURDT3RD = 1,
	STAR_CONS_TSOURDT3RD_NOTICE,
	STAR_CONS_TSOURDT3RD_ALERT,

	STAR_CONS_TSOURDT3RD_DEBUG,

	STAR_CONS_APRILFOOLS,
	STAR_CONS_EASTER,

	STAR_CONS_JUKEBOX
} star_messagetype_t;

typedef enum
{
	TSOURDT3RD_EASTER = 1,
	TSOURDT3RD_APRILFOOLS,
	TSOURDT3RD_CHRISTMAS
} tsourdt3rdevent_t;

extern tsourdt3rdevent_t TSoURDt3rd_CurrentEvent;

// here for compatibility reasons (for the meantime) //
extern boolean aprilfoolsmode;
extern boolean eastermode;
extern boolean xmasmode, xmasoverride;

extern musicdef_t tsourdt3rd_aprilfools_def;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_Init(void);

void STAR_CONS_Printf(star_messagetype_t starMessageType, const char *fmt, ...);
const char *TSoURDt3rd_CON_DrawStartupScreen(void);

void TSoURDt3rd_D_Display(void);

const char *TSoURDt3rd_ReturnUsername(void);

// ======
// EVENTS
// ======

boolean TSoURDt3rd_Easter_AllEggsCollected(void);

boolean TSoURDt3rd_AprilFools_ModeEnabled(void);
void TSoURD3rd_AprilFools_OnChange(void);

// ======
// LEVELS
// ======

mobj_t *TSoURDt3rd_BossInMap(void);

void TSoURDt3rd_LoadLevel(boolean reloadinggamestate);

// ======
// SCENES
// ======

void TSoURDt3rd_GameEnd(INT32 *timetonext);

#endif // __SS_MAIN__
