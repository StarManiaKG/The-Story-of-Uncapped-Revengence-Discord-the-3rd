// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  m_menu.h
/// \brief Globalizes all of TSoURDt3rd's cool menu data, plus a little more

#ifndef __STAR_M_MENU__
#define __STAR_M_MENU__

#include "star_vars.h"
#include "ss_cmds.h"
#include "../m_menu.h"
#include "../m_cond.h"

// ------------------------ //
//        Variables
// ------------------------ //

extern INT32 (*setupcontrols)[2]; // pointer to the gamecontrols of the player being edited

extern INT16 skullAnimCounter; // skull animation counter; Prompts: Chevron animation

extern INT16 MessageMenuDisplay[3][256]; // TO HACK

extern INT16 itemOn; // menu item skull is on, Hack by Tails 09-18-2002

enum
{
	op_eventoptionsheader = 0,
	op_alloweasteregghunt,
	op_easteregghuntbonuses,

	op_aprilfools,

	op_loadingscreenimage = 8,

	op_isitcalledsingleplayer = 10,

	op_fpscountercolor = 13,
	op_tpscountercolor,

	op_shadowsrotate = 17,

	op_allowtypicaltimeover,

	op_checkfortsourdt3rdupdates = 23,
	
	op_defaultmaptrack = 35,

	op_shieldblockstransformation = 37,

	op_alwaysoverlayinvuln = 39,

	op_storesavesinfolders = 41,

	op_perfectsave,
	op_perfectsavestripe1,
	op_perfectsavestripe2,
	op_perfectsavestripe3,

	op_continues,

	op_snake = 59,
	op_dispensergoingup,

	op_windowtitletype,
	op_customwindowtitle,
	op_memesonwindowtitle
};

// ================================================================
// QUIT MESSAGES
// 	Now externed, and are defined here instead, of inside m_menu.c!
// ================================================================

typedef enum
{
	QUITMSG = 0,
	QUITMSG1,
	QUITMSG2,
	QUITMSG3,
	QUITMSG4,
	QUITMSG5,
	QUITMSG6,
	QUITMSG7,

	QUIT2MSG,
	QUIT2MSG1,
	QUIT2MSG2,
	QUIT2MSG3,
	QUIT2MSG4,
	QUIT2MSG5,
	QUIT2MSG6,

	QUIT3MSG,
	QUIT3MSG1,
	QUIT3MSG2,
	QUIT3MSG3,
	QUIT3MSG4,
	QUIT3MSG5,
	QUIT3MSG6,

	TSOURDT3RD_QUITSMSG1,
	TSOURDT3RD_QUITSMSG2,
	TSOURDT3RD_QUITSMSG3,
	TSOURDT3RD_QUITSMSG4,
	TSOURDT3RD_QUITSMSG5,
	TSOURDT3RD_QUITSMSG6,

	TSOURDT3RD_AF_QUITAMSG1,
	TSOURDT3RD_AF_QUITAMSG2,
	TSOURDT3RD_AF_QUITAMSG3,
	TSOURDT3RD_AF_QUITAMSG4,

	NUM_QUITMESSAGES
} text_enum;

extern const char *quitmsg[NUM_QUITMESSAGES];

// =======
// JUKEBOX
// =======

extern musicdef_t *curplaying;

extern fixed_t st_time;
extern INT32 st_sel, st_cc;

extern patch_t* st_radio[9];
extern patch_t* st_launchpad[4];

extern UINT8 skyRoomMenuTranslations[MAXUNLOCKABLES];

// =====
// MENUS
// =====

#define MTREE5(a,b,c,d,e) MTREE2(a, MTREE4(b,c,d,e)) // just in case

extern menu_t OP_MainDef;
extern menuitem_t MainMenu[];

extern menuitem_t MPauseMenu[];
extern menu_t MPauseDef;
extern menuitem_t SPauseMenu[];

extern menu_t MessageDef;
extern menuitem_t MessageMenu[256];

extern menu_t SR_SoundTestDef;
extern menu_t OP_TSoURDt3rdOptionsDef, OP_TSoURDt3rdJukeboxDef, OP_Tsourdt3rdJukeboxControlsDef;

extern menuitem_t OP_Tsourdt3rdOptionsMenu[];

extern menuitem_t defaultMenuTitles[256][256];

// ------------------------ //
//        Functions
// ------------------------ //

// =======
// DRAWING
// =======

void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed);
void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic);

void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall);

// =====
// MENUS
// =====

void M_NextOpt(void);
void M_PrevOpt(void);

void M_DrawGenericMenu(void);
void M_DrawGenericScrollMenu(void);
void M_DrawControl(void);

void M_ChangeControl(INT32 choice);

void M_ShiftMessageQueueDown(void);

void M_HandleMasterServerResetChoice(INT32 choice);

void M_PreStartServerMenuChoice(INT32 choice);
void M_PreConnectMenuChoice(INT32 choice);

void M_StartServerMenu(INT32 choice);
void M_ConnectMenuModChecks(INT32 choice);

// ======
// I QUIT
// ======

void STAR_M_InitQuitMessages(void);
void STAR_M_InitDynamicQuitMessages(void);

INT32 STAR_M_SelectQuitMessage(void);
void STAR_M_DrawQuitGraphic(void);

// =============
// JUKEBOX MENUS
// =============

void M_CacheSoundTest(void);
boolean TSoURDt3rd_M_IsJukeboxUnlocked(TSoURDt3rdJukebox_t *TSoURDt3rdJukebox);
void M_TSoURDt3rdJukebox(INT32 choice);

#endif // __STAR_M_MENU__
