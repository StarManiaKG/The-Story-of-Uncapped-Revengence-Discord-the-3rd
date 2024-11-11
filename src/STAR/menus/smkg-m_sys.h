// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_sys.h
/// \brief Globalizes all of TSoURDt3rd's cool menu data, plus a little more

#ifndef __STAR_M_MENU__
#define __STAR_M_MENU__

#include "../star_vars.h"
#include "../smkg-cvars.h"

#include "../../m_menu.h"
#include "../../m_cond.h"
#include "../../v_video.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// =============
// SMKG-M_BIOS.C
// =============

// ------------------------ //
//        Variables
// ------------------------ //

#define MTREE5(a,b,c,d,e) MTREE2(a, MTREE4(b,c,d,e)) // just in case

// FIXME: C++ template
#define M_EaseWithTransition(EasingFunc, N) \
	(menutransition.tics != menutransition.dest ? EasingFunc(menutransition.in ?\
		TSoURDt3rd_M_ReverseTimeFrac(menutransition.tics, menutransition.endmenu->transitionTics) :\
		TSoURDt3rd_M_TimeFrac(menutransition.tics, menutransition.startmenu->transitionTics), 0, N) : 0)

extern INT16 tsourdt3rd_itemOn;
extern INT16 tsourdt3rd_skullAnimCounter;
extern boolean tsourdt3rd_noFurtherInput;

enum
{
	tsourdt3rd_wipe_init_tsourdt3rd_menu_toblack,
	tsourdt3rd_wipe_menu_toblack,
	tsourdt3rd_wipe_menu_final,
	TSOURDT3RD_NUMWIPEDEFS
};
extern UINT8 tsourdt3rd_wipedefs[TSOURDT3RD_NUMWIPEDEFS];

typedef union
{
	struct tsourdt3rd_menu_s *submenu; // IT_SUBMENU
	consvar_t *cvar;                   // IT_CVAR
	void (*routine)(INT32 choice);     // IT_CALL, IT_KEYHANDLER, IT_ARROWS
} tsourdt3d_itemaction_t;

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
	const char *tooltip; // description of option used by TSoURDt3rd_M_DrawMenuTooltips

	tsourdt3d_itemaction_t itemaction;

	// extra variables
	INT32 mvar1;
	INT32 mvar2;
} tsourdt3rd_menuitem_t;

typedef struct tsourdt3rd_menu_s
{
	tsourdt3rd_menuitem_t    *menuitems;          // custom tsourdt3rd menu items

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
} tsourdt3rd_menu_t;

extern struct menutransition_s {
	INT16 tics;
	INT16 dest;
	tsourdt3rd_menu_t *startmenu;
	tsourdt3rd_menu_t *endmenu;
	menu_t *vanilla_endmenu;
	boolean in;
} menutransition;

#define MAXMENUMESSAGE 256
#define MENUMESSAGECLOSE 2

typedef enum
{
	MA_NONE = 0,
	MA_YES,
	MA_NO
} manswer_e;

extern struct menumessage_s
{
	boolean active;
	UINT8 closing;

	INT32 flags;		// MM_
	const char *header;
	char message[MAXMENUMESSAGE];	// message to display

	SINT8 fadetimer;	// opening
	INT32 x;
	INT32 y;
	INT16 timer;

	void (*routine)(INT32 choice);	// Normal routine
	//void (*eroutine)(event_t *ev);	// Event routine	(MM_EVENTHANDLER)
	INT32 answer;

	const char *defaultstr;
	const char *confirmstr;
} menumessage;

#define MENUDELAYTIME 7
#define MENUMINDELAY 2

typedef enum
{
	MBT_A = 1,
	MBT_B = 1<<1,
	MBT_C = 1<<2,
	MBT_X = 1<<3,
	MBT_Y = 1<<4,
	MBT_Z = 1<<5,
	MBT_L = 1<<6,
	MBT_R = 1<<7,
	MBT_START = 1<<8,
	MBT_SCREENSHOT = 1<<9,
	MBT_STARTMOVIE = 1<<10,
	MBT_STARTLOSSLESS = 1<<11,
} menuButtonCode_t;

typedef struct menucmd_s
{
	// Current frame's data
	SINT8 dpad_ud; // up / down dpad
	SINT8 dpad_lr; // left / right
	UINT32 buttons; // buttons

	// Previous frame's data
	SINT8 prev_dpad_ud;
	SINT8 prev_dpad_lr;
	UINT32 buttonsHeld;

	UINT16 delay; // menu wait
	UINT32 delayCount; // num times ya did menu wait (to make the wait shorter each time)
} menucmd_t;
extern menucmd_t menucmd[MAXSPLITSCREENPLAYERS];

// ------------------------ //
//        Functions
// ------------------------ //

fixed_t TSoURDt3rd_M_TimeFrac(tic_t tics, tic_t duration);
fixed_t TSoURDt3rd_M_ReverseTimeFrac(tic_t tics, tic_t duration);
fixed_t TSoURDt3rd_M_DueFrac(tic_t start, tic_t duration);

void TSoURDt3rd_M_UpdateItemOn(void);

void TSoURDt3rd_M_StartMessage(const char *header, const char *string, void (*routine)(INT32), menumessagetype_t itemtype, const char *confirmstr, const char *defaultstr);
boolean TSoURDt3rd_M_MenuMessageTick(void);
void TSoURDt3rd_M_MenuMessageShouldTick(boolean run);
void TSoURDt3rd_M_HandleMenuMessage(void);
void TSoURDt3rd_M_StopMessage(INT32 choice);
void TSoURDt3rd_M_DrawMenuMessage(void);
void TSoURDt3rd_M_DrawMenuMessageOnTitle(INT32 count);
boolean TSoURDt3rd_M_OverwriteIntroResponder(event_t *event);

void TSoURDt3rd_M_SetMenuDelay(UINT8 i);

boolean TSoURDt3rd_M_MenuButtonPressed(UINT8 pid, UINT32 bt);
boolean TSoURDt3rd_M_MenuButtonHeld(UINT8 pid, UINT32 bt);

boolean TSoURDt3rd_M_MenuConfirmPressed(UINT8 pid);
boolean TSoURDt3rd_M_MenuConfirmHeld(UINT8 pid);
boolean TSoURDt3rd_M_MenuBackPressed(UINT8 pid);
boolean TSoURDt3rd_M_MenuBackHeld(UINT8 pid);
boolean TSoURDt3rd_M_MenuExtraPressed(UINT8 pid);
boolean TSoURDt3rd_M_MenuExtraHeld(UINT8 pid);

// =============
// SMKG-M_DEFS.C
// =============

extern tsourdt3rd_menu_t *tsourdt3rd_currentMenu;

#if 1
extern menu_t OP_MainDef;
extern menuitem_t MainMenu[];

extern menuitem_t SP_MainMenu[];

extern menuitem_t MPauseMenu[];
extern menu_t MPauseDef;
extern menuitem_t SPauseMenu[];
extern menu_t SPauseDef;
#endif

#ifdef HAVE_DISCORDSUPPORT
extern menu_t DISCORD_OP_MainDef;
extern menuitem_t DISCORD_OP_MainMenu[];
extern tsourdt3rd_menu_t DISCORD_TM_OP_MainDef;
extern tsourdt3rd_menuitem_t DISCORD_TM_OP_MainMenu[];
enum
{
	op_drpcs_header = 3,
	op_drpcs_asks,
	op_drpcs_invites,
	op_drpcs_showonstatus = 7,
	op_drpcs_statusmemes,
	op_drpcs_charimgtype = 10,

	op_cps_header = 12,
	op_cps_details,
	op_cps_state,

	op_cpli_header = 16,
	op_cpli_largeimgtext,
	op_cpli_largeimgtype = 19,
	op_cpli_largeimg,

	op_cpls_header = 22,
	op_cpls_smallimgtext,
	op_cpls_smallimgtype = 25,
	op_cpls_smallimg
};

extern menu_t DISCORD_MISC_RequestsDef;
extern menuitem_t DISCORD_MISC_RequestsMenu[];
extern tsourdt3rd_menu_t DISCORD_TM_MISC_RequestsDef;
extern tsourdt3rd_menuitem_t DISCORD_TM_MISC_RequestsMenu[];
#endif

extern menu_t TSoURDt3rd_OP_MainMenuDef;
extern menuitem_t TSoURDt3rd_OP_MainMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_MainMenuDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_MainMenu[];
enum
{
	op_main_eventsmenu,
	op_main_gamemenu,
	op_main_controlsmenu,
	op_main_videomenu,
	op_main_audiomenu,
	op_main_playermenu,
	op_main_savedatamenu,
	op_main_servermenu,
	op_main_jukeboxmenu,
	op_main_extrasmenu
};

extern menu_t TSoURDt3rd_OP_EventsDef;
extern menuitem_t TSoURDt3rd_OP_EventsMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_EventsDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_EventsMenu[];
enum
{
	op_easter,
	op_easter_egghunt,
	op_easter_bonuses,

	op_aprilfools = 4,
	op_aprilfools_ultimatemode
};

extern menu_t TSoURDt3rd_OP_GameDef;
extern menuitem_t TSoURDt3rd_OP_GameMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_GameDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_GameMenu[];
enum
{
	op_game_sdl_header = 3,
	op_game_sdl_windowtitle_type,
	op_game_sdl_space,
	op_game_sdl_windowtitle_custom,
	op_game_sdl_windowtitle_memes,

	op_scenes_loadingscreen_image = 14,

	op_levels_timeover = 22,

	op_objects_shadows_realistic = 26,
	op_objects_shadows_forallobjects,
	op_objects_shadows_positioning,

	op_general_isitcalledsingleplayer = 33
};

extern menu_t TSoURDt3rd_OP_ControlsDef;
extern menuitem_t TSoURDt3rd_OP_ControlsMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ControlsDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ControlsMenus[];

extern menu_t TSoURDt3rd_OP_VideoDef;
extern menuitem_t TSoURDt3rd_OP_VideoMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_VideoDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_VideoMenu[];
enum
{
	op_video_sdl_header,
	op_video_sdl_window_shaking,

	op_flair_fpscolor = 9,
	op_flair_tpscolor
};

extern menu_t TSoURDt3rd_OP_AudioDef;
extern menuitem_t TSoURDt3rd_OP_AudioMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_AudioDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_AudioMenu[];
enum
{
	op_audio_levels = 7
};

extern menu_t TSoURDt3rd_OP_SavedataDef;
extern menuitem_t TSoURDt3rd_OP_SavedataMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_SavedataDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_SavedataMenu[];
enum
{
	op_general_limitedcontinues = 1,

	op_cosmetic_perfectsave_stripe1 = 7,
	op_cosmetic_perfectsave_stripe2,
	op_cosmetic_perfectsave_stripe3
};

extern menu_t TSoURDt3rd_OP_PlayerDef;
extern menuitem_t TSoURDt3rd_OP_PlayerMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_PlayerDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_PlayerMenu[];

extern menu_t TSoURDt3rd_OP_ServerDef;
extern menuitem_t TSoURDt3rd_OP_ServerMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ServerDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ServerMenu[];

extern menu_t TSoURDt3rd_OP_ExtrasDef;
extern menuitem_t TSoURDt3rd_OP_ExtrasMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ExtrasDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ExtrasMenu[];

extern menu_t TSoURDt3rd_OP_DebugDef;
extern menuitem_t TSoURDt3rd_OP_DebugMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_DebugDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_DebugMenu[];

extern menu_t TSoURDt3rd_OP_Extras_SnakeDef;
extern menuitem_t TSoURDt3rd_OP_Extras_SnakeMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_SnakeDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_SnakeMenu[];

extern menu_t TSoURDt3rd_OP_Extras_JukeboxDef;
extern menuitem_t TSoURDt3rd_OP_Extras_JukeboxMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_JukeboxDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_JukeboxMenu[];

// =============
// SMKG-M_DRAW.C
// =============

// ------------------------ //
//        Variables
// ------------------------ //

// flags for text highlights
#define tsourdt3rd_highlightflags V_MENUCOLORMAP
#define tsourdt3rd_recommendedflags V_GREENMAP
#define tsourdt3rd_warningflags V_ORANGEMAP
#define TSOURDT3RD_M_ALTCOLOR V_ROSYMAP

extern boolean menuwipe;

#ifdef HAVE_DISCORDSUPPORT
extern struct discordrequestmenu_s {
	tic_t ticker;
	tic_t confirmDelay;
	tic_t confirmLength;
	boolean confirmAccept;
	boolean removeRequest;
} discordrequestmenu;
#endif

typedef struct menu_anim_s
{
	tic_t start;
	INT16 dist;
} menu_anim_t;

// Keep track of some options properties
#define M_OPTIONS_OFSTIME 5
#define M_OPTIONS_BINDBEN_QUICK 106

extern struct optionsmenu_s {

	tic_t ticker;			// How long the menu's been open for
	menu_anim_t offset;		// To make the icons move smoothly when we transition!
	menu_anim_t box;

	// For moving the button when we get into a submenu. it's smooth and cool! (normal x/y and target x/y.)
	// this is only used during menu transitions.

	// For profiles specifically, this moves the card around since we don't have the rest of the menu displayed in that case.
	INT16 optx;
	INT16 opty;
	INT16 toptx;
	INT16 topty;
	tic_t topt_start;

	// profile garbage
	boolean profilemenu;		// In profile menu. (Used to know when to get the "PROFILE SETUP" button away....
	boolean resetprofilemenu;	// Reset button behaviour when exiting
	SINT8 profilen;				// # of the selected profile.

	boolean resetprofile;		// After going back from the edit menu, this tells the profile select menu to kill the profile data after the transition.
	void *profile;			    // Pointer to the profile we're editing

	INT32 tempcontrols[NUM_GAMECONTROLS][2];
	// Temporary buffer where we're gonna store game controls.
	// This is only applied to the profile when you exit out of the controls menu.

	INT16 controlscroll;		// scrolling for the control menu....
	INT16 bindtimer;			// Timer until binding is cancelled (5s)
	UINT16 bindben;				// Hold right timer
	UINT8 bindben_swallow;		// (bool) control is about to be cleared; (int) swallow/pose animation timer
	INT32 bindinputs[2];        // Set while binding

	// Used for horrible axis shenanigans
	INT32 lastkey;
	tic_t keyheldfor;

	// for video mode testing:
	INT32 vidm_testingmode;
	INT32 vidm_previousmode;
	INT32 vidm_selected;
	INT32 vidm_nummodes;
	INT32 vidm_column_size;

	UINT8 erasecontext;

	UINT8 eraseprofilen;

	// background:
	INT16 currcolour;
	INT16 lastcolour;
	tic_t fade;
} optionsmenu;

// ------------------------ //
//        Functions
// ------------------------ //

void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed);
void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic);

void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall);

void TSoURDt3rd_M_DrawPauseGraphic(void);
void TSoURDt3rd_M_DrawQuitGraphic(void);

void TSoURDt3rd_M_PreDrawer(void);
void TSoURDt3rd_M_PostDrawer(void);

void TSoURDt3rd_M_DrawGenericOptions(void);
void TSoURDt3rd_M_DrawMediocreKeyboardKey(const char *text, INT32 *workx, INT32 worky, boolean push, boolean rightaligned);

INT32 TSoURDt3rd_M_DrawCaretString
(
	INT32 x, INT32 y,
	INT32 flags,
	fixed_t pscale, fixed_t vscale,
	const char *string, fontdef_t font
);

void TSoURDt3rd_M_DrawMenuTooltips
(
	fixed_t box_x, fixed_t box_y, INT32 box_flags, UINT8 *box_color, boolean box_patch,
	fixed_t string_x, fixed_t string_y, INT32 string_flags, boolean string_centered
);

// For some menu highlights
UINT16 TSoURDt3rd_M_GetCvPlayerColor(UINT8 pnum);

// =============
// SMKG-M_FUNC.C
// =============

// ------------------------ //
//        Variables
// ------------------------ //

#define AUTOLOADSTRING "Press \x83Right-Shift\x80 to mark addons to Autoload!"

extern void *tsourdt3rd_snake;

// ------------------------ //
//        Functions
// ------------------------ //

// Server Setup Menu
void M_HandleMasterServerResetChoice(INT32 choice);

void M_PreStartServerMenuChoice(INT32 choice);
void M_PreConnectMenuChoice(INT32 choice);

void M_StartServerMenu(INT32 choice);
void M_ConnectMenuModChecks(INT32 choice);

// Player Setup Menu
void TSoURDt3rd_M_DrawColorResetOption
(
	INT32 x, INT32 *y, INT32 *cursory,
	player_t *setupm_player,
	INT32 setupm_fakeskin, consvar_t *setupm_cvdefaultskin,
	consvar_t *setupm_cvdefaultcolor, menucolor_t *setupm_fakecolor
);
void TSoURDt3rd_M_HandleColorResetOption
(
	player_t *setupm_player,
	INT32 *setupm_fakeskin, consvar_t *setupm_cvdefaultskin,
	consvar_t *setupm_cvdefaultcolor, menucolor_t *setupm_fakecolor,
	void (*after_routine)(void)
);

#ifdef HAVE_DISCORDSUPPORT
void TSoURDt3rd_M_InitDiscordOptions(INT32 choice);
void TSoURDt3rd_M_InitDiscordRequests(INT32 choice);
#endif

// TSoURDt3rd Options
void TSoURDt3rd_M_DrawOptions(void);
void TSoURDt3rd_M_DrawOptionsMovingButton(void);

void TSoURDt3rd_M_InitMainOptions(INT32 choice);
void TSoURDt3rd_M_OptionsTick(void);
boolean TSoURDt3rd_M_OptionsInputs(INT32 ch);
boolean TSoURDt3rd_M_OptionsQuit(void);
void TSoURDt3rd_M_ResetOptions(void);

// TSoURDt3rd Jukebox
void TSoURDt3rd_M_InitJukebox(INT32 choice);
void TSoURDt3rd_M_JukeboxTicker(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __STAR_M_MENU__
