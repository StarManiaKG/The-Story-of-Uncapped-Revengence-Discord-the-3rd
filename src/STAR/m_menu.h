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
#include "smkg-cvars.h"
#include "../m_menu.h"
#include "../m_cond.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

extern INT16 MessageMenuDisplay[3][256]; // TO HACK

extern INT16 tsourdt3rd_itemOn;
extern INT16 tsourdt3rd_skullAnimCounter;

extern INT32 (*setupcontrols)[2]; // pointer to the gamecontrols of the player being edited

extern boolean tsourdt3rd_noFurtherInput;

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

// Fade wipes
enum
{
	tsourdt3rd_wipe_menu_toblack,
	tsourdt3rd_wipe_menu_final,
	TSOURDT3RD_NUMWIPEDEFS
};
extern UINT8 tsourdt3rd_wipedefs[TSOURDT3RD_NUMWIPEDEFS];

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

typedef enum
{
	MBF_UD_LR_FLIPPED		= 1,    // flip up-down and left-right axes
	MBF_SOUNDLESS		 	= 1<<1, // do not play base menu sounds
	MBF_NOLOOPENTRIES		= 1<<2, // do not loop STAR_M_NextOpt/STAR_M_PrevOpt
	MBF_DRAWBGWHILEPLAYING	= 1<<3, // run backroutine() outside of GS_MENU
} menubehaviourflags_t;

typedef struct tsourdt3rd_menuitems_s
{
	const char *patch; // image of option used by K_MenuPreviews
	const char *tooltip; // description of option used by K_MenuTooltips

	// extra variables
	INT32 mvar1;
	INT32 mvar2;
} tsourdt3rd_menuitems_t;

typedef struct tsourdt3rd_menu_s
{
	menu_t                   *menu;                // pointer to the current menu_t
	tsourdt3rd_menuitems_t   *menuitems;           // custom tsourdt3rd menu items

	INT16                    extra1, extra2;	   // Can be whatever really! Options menu uses extra1 for bg colour.
	INT16                    behaviourflags;       // menubehaviourflags_t

	INT16                    transitionID;         // only transition if IDs match
	INT16          			 transitionTics;       // tics for transitions out

	void                     (*drawroutine)(void); // draw routine
	void                     (*tickroutine)(void); // ticker routine
	void                     (*initroutine)(void); // called when starting a new menu
	boolean                  (*quitroutine)(void); // called before quit a menu return true if we can
	boolean		             (*inputroutine)(INT32); // if set, called every frame in the input handler. Returning true overwrites normal input handling.

	struct tsourdt3rd_menu_s *nextmenu; // pointer to the next supported tsourdt3rd menu
} tsourdt3rd_menu_t;
extern tsourdt3rd_menu_t *tsourdt3rd_currentMenu;

extern menu_t OP_MainDef;
extern menuitem_t MainMenu[];

extern menuitem_t SP_MainMenu[];

extern menuitem_t MPauseMenu[];
extern menu_t MPauseDef;
extern menuitem_t SPauseMenu[];
extern menu_t SPauseDef;

extern menu_t MessageDef;
extern menuitem_t MessageMenu[256];

extern menu_t SR_SoundTestDef;

extern menu_t OP_TSoURDt3rdOptionsDef;
extern menuitem_t OP_Tsourdt3rdOptionsMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_OP_TSoURDt3rdOptionsDef;

extern menu_t OP_TSoURDt3rdJukeboxDef, OP_Tsourdt3rdJukeboxControlsDef;

extern menuitem_t defaultMenuTitles[256][256];
extern gtdesc_t defaultGametypeTitles[NUMGAMETYPES];

#ifdef HAVE_DISCORDSUPPORT
extern menu_t OP_DiscordOptionsDef;
extern tsourdt3rd_menu_t TSoURDt3rd_MISC_DiscordRequestsDef;

extern menu_t MISC_DiscordRequestsDef;
extern tsourdt3rd_menu_t TSoURDt3rd_OP_DiscordOptionsDef;
#endif

// ------------------------ //
//        Functions
// ------------------------ //

fixed_t M_TimeFrac(tic_t tics, tic_t duration);
fixed_t M_ReverseTimeFrac(tic_t tics, tic_t duration);
fixed_t M_DueFrac(tic_t start, tic_t duration);

// FIXME: C++ template
#define M_EaseWithTransition(EasingFunc, N) \
	(menutransition.tics != menutransition.dest ? EasingFunc(menutransition.in ?\
		M_ReverseTimeFrac(menutransition.tics, menutransition.endmenu->transitionTics) :\
		M_TimeFrac(menutransition.tics, menutransition.startmenu->transitionTics), 0, N) : 0)

// =======
// DRAWING
// =======

void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed);
void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic);

void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall);

// =====
// MENUS
// =====

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

#ifdef HAVE_DISCORDSUPPORT
void M_DiscordOptions(INT32 choice);

extern struct discordrequestmenu_s {
	tic_t ticker;
	tic_t confirmDelay;
	tic_t confirmLength;
	boolean confirmAccept;
	boolean removeRequest;
} discordrequestmenu;

void M_DrawDiscordRequests(void);
void M_DiscordRequests(INT32 choice);
const char *M_GetDiscordName(discordRequest_t *r);
#endif

void M_TSoURDt3rdOptions(INT32 choice);

// ======
// I QUIT
// ======

void STAR_M_InitQuitMessages(void);
void STAR_M_InitDynamicQuitMessages(void);

INT32 STAR_M_SelectQuitMessage(void);

// =======
// JUKEBOX
// =======

void M_CacheSoundTest(void);

boolean TSoURDt3rd_M_IsJukeboxUnlocked(TSoURDt3rdJukebox_t *TSoURDt3rdJukebox);
void TSoURDt3rd_Jukebox_InitMenu(INT32 choice);

void TSoURDt3rd_Jukebox_InitControlsMenu(INT32 choice);

// =====
// SNAKE
// =====

void TSoURDt3rd_Snake_InitMenu(INT32 choice);

#endif // __STAR_M_MENU__
