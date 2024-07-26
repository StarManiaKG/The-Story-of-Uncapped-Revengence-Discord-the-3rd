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


// ------------------------ //
//        Variables
// ------------------------ //

//#define APRIL_FOOLS // MARKED FOR REMOVAL //

#define TSOURDT3RD_DEBUGGING /* Debugging */

#define AUTOLOADCONFIGFILENAME "autoload.cfg"

#define SAVEGAMEFOLDER "saves"
extern char savegamefolder[256];

//#define ALAM_LIGHTING /* EXTREME BETA: Enable Alam's Lighting, With Touch-ups by Star */

//#define _DEBUG /* Debugging */
//#define DEBUGFILE /* Debugging */

#define TSOURDT3RD_DRRR_GAMEPAD_REFACTOR // pretty sure it works perfectly, but i don't wanna take chances //

// An extra lock-on to the current gamestate system
typedef enum
{
	STAR_GS_NULL = 0, // no star gamestate correspondant can be found...
	STAR_GS_NIGHTSMENU, // GS_TIMEATTACK, but in the nights menu
} star_gamestate_t;

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

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_Init(void);

void STAR_CONS_Printf(star_messagetype_t starMessageType, const char *fmt, ...);
const char *TSoURDt3rd_CON_DrawStartupScreen(void);

void TSoURDt3rd_D_Display(void);

void STAR_G_GamestateManager(star_gamestate_t star_gamestate);

void STAR_M_StartMessage(const char *header, INT32 headerflags, const char *string, void *routine, menumessagetype_t itemtype);

const char *TSoURDt3rd_ReturnUsername(void);

// ======
// EVENTS
// ======

boolean TSoURDt3rd_Easter_AllEggsCollected(void);

boolean TSoURDt3rd_AprilFools_ModeEnabled(void);
void TSoURD3rd_AprilFools_OnChange(void);

// =======
// SERVERS
// =======

void TSoURDt3rd_MovePlayerStructure(INT32 node, INT32 newplayernode, INT32 prevnode);
void TSoURDt3rd_HandleCustomPackets(INT32 node);

// ======
// LEVELS
// ======

mobj_t *TSoURDt3rd_BossInMap(void);

void TSoURDt3rd_LoadLevel(boolean reloadinggamestate);

// ======
// SCENES
// ======

void TSoURDt3rd_GameEnd(INT32 *timetonext);

// ======
// SCREEN
// ======

void TSoURDt3rd_SCR_DisplayTpsRate(void);
INT32 TSoURDt3rd_SCR_SetPingHeight(void);

#endif // __SS_MAIN__
