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
#include "../v_video.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

extern INT16 tsourdt3rd_itemOn;
extern INT16 tsourdt3rd_skullAnimCounter;
extern boolean tsourdt3rd_noFurtherInput;

// Fade wipes
enum
{
	tsourdt3rd_wipe_menu_toblack,
	tsourdt3rd_wipe_menu_final,
	TSOURDT3RD_NUMWIPEDEFS
};
extern UINT8 tsourdt3rd_wipedefs[TSOURDT3RD_NUMWIPEDEFS];

// Snake
extern void *tsourdt3rd_snake;

// =====
// MENUS
// =====

#define MTREE5(a,b,c,d,e) MTREE2(a, MTREE4(b,c,d,e)) // just in case

#define AUTOLOADSTRING "Press \x83Right-Shift\x80 to mark addons to Autoload!"

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

	INT32 mvar1; // extra variables
	INT32 mvar2;
} tsourdt3rd_menuitems_t;

typedef struct tsourdt3rd_menu_s
{
	menu_t                   *menu;               // pointer to the current menu_t
	tsourdt3rd_menuitems_t   *menuitems;          // custom tsourdt3rd menu items

	INT16                     extra1, extra2;	  // Can be whatever really! Options menu uses extra1 for bg colour.
	INT16                     behaviourflags;     // menubehaviourflags_t
	const char               *music;              // Track to play in TSoURDt3rd_M_PlayMenuJam. NULL for default, "." to stop

	INT16                     transitionID;       // only transition if IDs match
	INT16          			  transitionTics;     // tics for transitions out

	void                    (*drawroutine)(void); // draw routine
	void                    (*tickroutine)(void); // ticker routine
	void                    (*initroutine)(void); // called when starting a new menu
	boolean                 (*quitroutine)(void); // called before quit a menu return true if we can
	boolean		            (*inputroutine)(INT32); // if set, called every frame in the input handler. Returning true overwrites normal input handling.

	struct tsourdt3rd_menu_s *prev_menu; // pointer to the last accessed tsourdt3rd menu
	struct tsourdt3rd_menu_s *next_menu; // pointer to the next supported tsourdt3rd menu
} tsourdt3rd_menu_t;

// FIXME: C++ template
#define M_EaseWithTransition(EasingFunc, N) \
	(menutransition.tics != menutransition.dest ? EasingFunc(menutransition.in ?\
		M_ReverseTimeFrac(menutransition.tics, menutransition.endmenu->transitionTics) :\
		M_TimeFrac(menutransition.tics, menutransition.startmenu->transitionTics), 0, N) : 0)

extern struct menutransition_s {
	INT16 tics;
	INT16 dest;
	tsourdt3rd_menu_t *startmenu;
	tsourdt3rd_menu_t *endmenu;
	boolean in;
	boolean transitions_enabled;
} menutransition;

// SMKG-M_MENUDEF.C
extern tsourdt3rd_menu_t *tsourdt3rd_currentMenu;

#if 1
extern menuitem_t defaultMenuTitles[256][256];
extern gtdesc_t defaultGametypeTitles[NUMGAMETYPES];

extern menu_t OP_MainDef;
extern menuitem_t MainMenu[];

extern menuitem_t SP_MainMenu[];

extern menuitem_t MPauseMenu[];
extern menu_t MPauseDef;
extern menuitem_t SPauseMenu[];
extern menu_t SPauseDef;
#endif

#ifdef HAVE_DISCORDSUPPORT
extern menu_t OP_DiscordOptionsDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_MISC_DiscordRequestsDef;

extern menu_t MISC_DiscordRequestsDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_DiscordOptionsDef;
#endif

extern menu_t TSoURDt3rd_OP_MainMenuDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_MainMenuDef;

extern menu_t TSoURDt3rd_OP_EventsDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_EventsDef;

extern menu_t TSoURDt3rd_OP_GameDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_GameDef;

extern menu_t TSoURDt3rd_OP_ControlsDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ControlsDef;

extern menu_t TSoURDt3rd_OP_VideoDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_VideoDef;

extern menu_t TSoURDt3rd_OP_AudioDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_AudioDef;

extern menu_t TSoURDt3rd_OP_SavefileDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_SavefileDef;

extern menu_t TSoURDt3rd_OP_PlayerDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_PlayerDef;

extern menu_t TSoURDt3rd_OP_ServerDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ServerDef;

extern menu_t TSoURDt3rd_OP_JukeboxDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_JukeboxDef;

extern menu_t OP_TSoURDt3rdJukeboxDef, OP_Tsourdt3rdJukeboxControlsDef;

extern menu_t TSoURDt3rd_OP_ExtrasDef;
extern menuitem_t TSoURDt3rd_OP_ExtrasMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ExtrasDef;

extern menu_t TSoURDt3rd_OP_Extras_SnakeDef;
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_SnakeDef;

// SMKG-M_MENUDRAW.C
// flags for text highlights
#define tsourdt3rd_highlightflags V_MENUCOLORMAP
#define tsourdt3rd_recommendedflags V_GREENMAP
#define tsourdt3rd_warningflags V_ORANGEMAP

#define TSOURDT3RD_M_ALTCOLOR V_ORANGEMAP

// ------------------------ //
//        Functions
// ------------------------ //

fixed_t M_TimeFrac(tic_t tics, tic_t duration);
fixed_t M_ReverseTimeFrac(tic_t tics, tic_t duration);
fixed_t M_DueFrac(tic_t start, tic_t duration);

void TSoURDt3rd_M_UpdateItemOn(void);

// =======
// DRAWING
// =======

void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed);
void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic);

void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall);

void TSoURDt3rd_M_DrawGenericOptions(void);
void TSoURDt3rd_M_DrawMediocreKeyboardKey(const char *text, INT32 *workx, INT32 worky, boolean push, boolean rightaligned);

// =====
// MENUS
// =====

void M_HandleMasterServerResetChoice(INT32 choice);

void M_PreStartServerMenuChoice(INT32 choice);
void M_PreConnectMenuChoice(INT32 choice);

void M_StartServerMenu(INT32 choice);
void M_ConnectMenuModChecks(INT32 choice);

void TSoURDt3rd_M_DrawColorResetOption
(
	INT32 x, INT32 *y, INT32 *cursory,
	player_t *setupm_player,
	consvar_t *setupm_cvdefaultskin, consvar_t *setupm_cvdefaultcolor, menucolor_t *setupm_fakecolor
);
void TSoURDt3rd_M_HandleColorResetOption
(
	player_t *setupm_player,
	INT32 *setupm_fakeskin, consvar_t *setupm_cvdefaultskin,
	consvar_t *setupm_cvdefaultcolor, menucolor_t *setupm_fakecolor
);

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

void TSoURDt3rd_M_InitOptionsMenu(INT32 choice);
void TSoURDt3rd_M_DrawOptions(void);
void TSoURDt3rd_M_OptionsTick(void);
boolean TSoURDt3rd_M_OptionsInputs(INT32 ch);

// =======
// JUKEBOX
// =======

void TSoURDt3rd_Jukebox_InitMenu(INT32 choice);

#endif // __STAR_M_MENU__
