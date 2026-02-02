// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_sys.h
/// \brief Unique TSoURDt3rd menu routines, structures, and cool data globalizer

#ifndef __SMKG_M_SYS__
#define __SMKG_M_SYS__

#include "../smkg-defs.h"
#include "../star_vars.h"
#include "../smkg-cvars.h"
#include "../core/smkg-g_game.h" // tsourdt3rd_local //

#include "../../g_input.h"
#include "../../m_menu.h"
#include "../../m_cond.h"
#include "../../v_video.h"

#ifdef __cplusplus
extern "C" {
#endif

// =============
// SMKG-M_BIOS.C
// =============

// ------------------------ //
//          Macros
// ------------------------ //

#define MTREE5(a,b,c,d,e) MTREE2(a, MTREE4(b,c,d,e)) // just in case

// FIXME: C++ template
#define M_EaseWithTransition(EasingFunc, N) \
	(menutransition.tics != menutransition.dest ? EasingFunc(menutransition.in ?\
		TSoURDt3rd_M_ReverseTimeFrac(menutransition.tics, menutransition.endmenu->transitionTics) :\
		TSoURDt3rd_M_TimeFrac(menutransition.tics, menutransition.startmenu->transitionTics), 0, N) : 0)

// ------------------------ //
//        Variables
// ------------------------ //

extern INT16 tsourdt3rd_itemOn;
extern INT16 tsourdt3rd_skullAnimCounter;
extern boolean tsourdt3rd_noFurtherInput;

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
	MBF_NOLOOPENTRIES		= 1<<2, // do not loop TSoURDt3rd_M_NextOpt/TSoURDt3rd_M_PrevOpt
	MBF_DRAWBGWHILEPLAYING	= 1<<3, // run backroutine() outside of GS_MENU
} menubehaviourflags_t;

typedef struct tsourdt3rd_menuitem_s
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
#if 0
	// Doesn't exist because we can just use vanilla's menu_t drawroutine function for rendering backgrounds.
	// At least, until we merge this with that and all.
	void          			(*bgroutine)(void); // draw routine, but, like, for the background
#endif
	void                    (*tickroutine)(void); // ticker routine
	void                    (*initroutine)(void); // called when starting a new menu
	boolean                 (*quitroutine)(void); // called before quit a menu return true if we can
	boolean		            (*inputroutine)(INT32); // if set, called every frame in the input handler. Returning true overwrites normal input handling.
	boolean                 (*eventroutine)(event_t *); // if set, called every frame in the responder handler. Returning true overwrites the menu responder.

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
#define MENUMESSAGEHEADERLEN 54

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

#define NUMVIRTUALKEYSINROW (10+2) // 1-9, 0, and a right-side gutter of two keys' width
extern INT16 virtualKeyboard[5][NUMVIRTUALKEYSINROW];
extern INT16 shift_virtualKeyboard[5][NUMVIRTUALKEYSINROW];

typedef const char *(*vkb_query_fn_t)(const char *replace);
extern struct menutyping_s
{
	boolean active;				// Active
	boolean menutypingclose;	// Closing
	boolean keyboardtyping;		// If true, all keystrokes are treated as typing (ignores MBT_A etc). This is unset if you try moving the cursor on the virtual keyboard or use your controller
	SINT8 menutypingfade;		// fade in and out

	SINT8 keyboardx;
	SINT8 keyboardy;
	boolean keyboardcapslock;
	boolean keyboardshift;

	vkb_query_fn_t queryfn; // callback on open and close
	menu_t *dummymenu;
	size_t cachelen;
	char *cache; // cached string
} menutyping; // While typing, we'll have a fade strongly darken the screen to overlay the typing menu instead

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
	MBT_JUKEBOX_OPEN = 1<<12,
	MBT_JUKEBOX_INCREASESPEED = 1<<13,
	MBT_JUKEBOX_DECREASESPEED = 1<<14,
	MBT_JUKEBOX_INCREASEPITCH = 1<<15,
	MBT_JUKEBOX_DECREASEPITCH = 1<<16,
	MBT_JUKEBOX_PLAYRECENT = 1<<17,
	MBT_JUKEBOX_CLOSE = 1<<18
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

typedef enum
{
	TSOURDT3RD_LLM_CREATESERVER,
	TSOURDT3RD_LLM_LEVELSELECT,
	TSOURDT3RD_LLM_RECORDATTACK,
	TSOURDT3RD_LLM_NIGHTSATTACK
} tsourdt3rd_levellist_mode_t;
extern tsourdt3rd_levellist_mode_t tsourdt3rd_levellistmode;

enum
{
	T3RDM_HANDLER_NONE = 0,

	T3RDM_HANDLER_VANILLA = 1<<3,
	T3RDM_KEYHANDLER_CVARSTRING = 1<<4,
	T3RDM_KEYHANDLER_MSGHANDLER = 1<<5,
	T3RDM_KEYHANDLER_KEYHANDLER = 1<<6,

	T3RDM_HANDLER_UNIQUE = 1<<7,
	T3RDM_KEYHANDLER_MENUTYPING = 1<<8,
	T3RDM_KEYHANDLER_MENUMESSAGE = 1<<9,
	T3RDM_KEYHANDLER_OPTBIND = 1<<10,

	T3RDM_HANDLER_WRITABLE = 1<<15
};

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_M_InitQuitMessages(const char **msg_table);
const char *TSoURDt3rd_M_GenerateQuitMessage(void);

fixed_t TSoURDt3rd_M_TimeFrac(tic_t tics, tic_t duration);
fixed_t TSoURDt3rd_M_ReverseTimeFrac(tic_t tics, tic_t duration);
fixed_t TSoURDt3rd_M_DueFrac(tic_t start, tic_t duration);

boolean TSoURDt3rd_M_StartControlPanel(void);
boolean TSoURDt3rd_M_Responder(INT32 *ch_p, event_t *ev);
void TSoURDt3rd_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter, INT32 levellistmode);

void TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_menu_t *tsourdt3rd_menudef, menu_t *menudef, boolean notransition);
void TSoURDt3rd_M_ClearMenus(void);

INT32 TSoURDt3rd_M_KeyHandlerType(void);
void TSoURDt3rd_M_SetWritable(boolean set);
boolean TSoURDt3rd_M_HasImportantHandler(void);

void TSoURDt3rd_M_GoBack(INT32 choice);
boolean TSoURDt3rd_M_NextOpt(void);
boolean TSoURDt3rd_M_PrevOpt(void);
void TSoURDt3rd_M_UpdateItemOn(void);

void TSoURDt3rd_M_ChangeCvar(INT32 choice, consvar_t *cv);
void TSoURDt3rd_M_ChangeCvarDirect(INT32 amount, float amount_f, consvar_t *cv);

void TSoURDt3rd_M_PlayMenuJam(void);

void TSoURDt3rd_M_StartMessage(const char *header, const char *string, void (*routine)(INT32), menumessagetype_t itemtype, const char *confirmstr, const char *defaultstr);
#define TSoURDt3rd_M_StartBasicMessage(header, string, confirmstr, defaultstr) TSoURDt3rd_M_StartMessage(header, string, NULL, MM_NOTHING, confirmstr, defaultstr)
#define TSoURDt3rd_M_StartPlainMessage(header, string) TSoURDt3rd_M_StartBasicMessage(header, string, NULL, NULL)
boolean TSoURDt3rd_M_MenuMessageTick(void);
void TSoURDt3rd_M_HandleMenuMessage(void);
void TSoURDt3rd_M_StopMessage(INT32 choice);
void TSoURDt3rd_M_DrawMenuMessage(void);

void TSoURDt3rd_M_OpenVirtualKeyboard(size_t cachelen, vkb_query_fn_t queryfn, menu_t *dummymenu);
boolean TSoURDt3rd_M_VirtualStringMeetsLength(void);
void TSoURDt3rd_M_AbortVirtualKeyboard(void);
void TSoURDt3rd_M_MenuTypingInput(INT32 key);
void TSoURDt3rd_M_SwitchVirtualKeyboard(boolean gamepad);

void TSoURDt3rd_M_SetMenuDelay(UINT8 i);

boolean TSoURDt3rd_M_MenuButtonPressed(UINT8 pid, UINT32 bt);
boolean TSoURDt3rd_M_MenuButtonHeld(UINT8 pid, UINT32 bt);
	boolean TSoURDt3rd_M_MenuConfirmPressed(UINT8 pid);
	boolean TSoURDt3rd_M_MenuConfirmHeld(UINT8 pid);
	boolean TSoURDt3rd_M_MenuBackPressed(UINT8 pid);
	boolean TSoURDt3rd_M_MenuBackHeld(UINT8 pid);
	boolean TSoURDt3rd_M_MenuExtraPressed(UINT8 pid);
	boolean TSoURDt3rd_M_MenuExtraHeld(UINT8 pid);

void TSoURDt3rd_M_UpdateMenuCMD(UINT8 i);

// =============
// SMKG-M_DEFS.C
// =============

extern tsourdt3rd_menu_t *tsourdt3rd_currentMenu;

extern menu_t *vanilla_prevMenu;
extern tsourdt3rd_menu_t *tsourdt3rd_prevMenu;

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
	op_main_readmemenu,
	op_main_eventsmenu,
	op_main_gamemenu,
	op_main_controlsmenu,
	op_main_videomenu,
	op_main_audiomenu,
	op_main_playermenu,
	op_main_savedatamenu,
	op_main_servermenu,
	op_main_extrasmenu,
	op_main_debuggingmenu
};

extern menu_t TSoURDt3rd_OP_ReadMEDef;
extern menuitem_t TSoURDt3rd_OP_ReadMEMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ReadMEDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ReadMEMenu[];

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

	op_levels_timeover = 21,
	op_levels_refreshmaplighting = 24,

	op_objects_shadows_realistic = 28,
	op_objects_shadows_forallobjects,
	op_objects_shadows_positioning,
	op_objects_shadows_offset,
	op_objects_shadows_end = op_objects_shadows_offset,

	op_general_isitcalledsingleplayer = 36
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

	op_video_flair_fpscolor = 9,
	op_video_flair_fpsfont,

	op_video_flair_tpscolor = 12,
	op_video_flair_tpsfont
};

#ifdef STAR_LIGHTING
extern menu_t TSoURDt3rd_OP_Video_LightingDef;
extern menuitem_t TSoURDt3rd_OP_Video_LightingMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Video_LightingDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Video_LightingMenu[];
#endif

extern menu_t TSoURDt3rd_OP_AudioDef;
extern menuitem_t TSoURDt3rd_OP_AudioMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_AudioDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_AudioMenu[];

extern menu_t TSoURDt3rd_OP_Audio_EXMusicDef;
extern menuitem_t TSoURDt3rd_OP_Audio_EXMusicMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Audio_EXMusicDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Audio_EXMusicMenu[];
enum
{
	op_levels_start = 0,
	op_levels_end = 3,

	op_bosses_start,
	op_bosses_end = 15,

	op_intermission_start,
	op_intermission_end = 20
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
enum
{
	op_extras_jukebox = 1
};

extern menu_t TSoURDt3rd_OP_Extras_SnakeDef;
extern menuitem_t TSoURDt3rd_OP_Extras_SnakeMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_SnakeDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_SnakeMenu[];

extern menu_t TSoURDt3rd_OP_Extras_JukeboxDef;
extern menuitem_t TSoURDt3rd_OP_Extras_JukeboxMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_JukeboxDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_JukeboxMenu[];
enum
{
	op_music_speed = 2,
	op_music_pitch
};

extern menu_t TSoURDt3rd_OP_DebugDef;
extern menuitem_t TSoURDt3rd_OP_DebugMenu[];
extern tsourdt3rd_menu_t TSoURDt3rd_TM_OP_DebugDef;
extern tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_DebugMenu[];

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

//
// So uh, for reasons that I kinda know, menu fades are wonky.
// I assume it's because our menu tickers are like split apart and all.
// Eventually everything will be merged together but in the meantime we should just restrict this.
//
#define NO_MENU_WIPE

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

	boolean bindmenuactive;		// If true, we're in the control binding menu.
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

	// Erase data menu
	UINT8 erasecontext;
	UINT8 eraseprofilen;

	// background:
	INT16 currcolour;
	INT16 lastcolour;
	tic_t fade;
} optionsmenu;

typedef struct menutooltip_s
{
	struct {
		fixed_t x, y;
		fixed_t pscale, vscale;
		INT32 flags;
		UINT8 *colormap;
		boolean vflip;
	} box;
	struct {
		fixed_t x, y;
		INT32 flags;
		INT32 align;
	} string;
} menutooltip_t;

// ------------------------ //
//        Functions
// ------------------------ //

INT32 K_DrawGameControl(fixed_t x, fixed_t y, UINT8 player, const char *str, UINT8 alignment, fontdef_t font, UINT32 flags);
void TSoURDt3rd_MK_DrawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed);
void TSoURDt3rd_MK_DrawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic);
void TSoURDt3rd_MK_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall);

void TSoURDt3rd_M_DrawMenuTyping(void);

void TSoURDt3rd_M_DrawPauseGraphic(void);
void TSoURDt3rd_M_DrawQuitGraphic(void);

void TSoURDt3rd_M_ShowMusicCredits(void);

void TSoURDt3rd_M_PreDrawer(boolean *wipe_in_action);
void TSoURDt3rd_M_PostDrawer(void);

void TSoURDt3rd_M_DrawGenericOptions(void);
void TSoURDt3rd_M_DrawMediocreKeyboardKey(const char *text, INT32 *workx, INT32 worky, boolean push, boolean rightaligned);

INT32 TSoURDt3rd_M_DrawCaretString(INT32 x, INT32 y, INT32 flags, fixed_t pscale, fixed_t vscale, const char *string, fontdef_t font);

void TSoURDt3rd_M_DrawMenuTooltips(const tsourdt3rd_menu_t *menu, menutooltip_t menutooltip);

void TSoURDt3rd_M_DrawOptions(void);
void TSoURDt3rd_M_DrawOptionsMovingButton(void);

void TSoURDt3rd_M_DrawColorResetOption(
	INT32 x, INT32 *y, INT32 *cursory,
	player_t *setupm_player,
	INT32 setupm_fakeskin, consvar_t *setupm_cvdefaultskin,
	consvar_t *setupm_cvdefaultcolor, menucolor_t *setupm_fakecolor
);

// For some menu highlights
UINT16 TSoURDt3rd_M_GetCvPlayerColor(UINT8 pnum);

// =============
// SMKG-M_FUNC.C
// =============

// ------------------------ //
//        Variables
// ------------------------ //

#define AUTOLOADSTRING "Press \x83Left-Arrow\x80 to mark addons to Autoload!"

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_M_HandleMasterServerResetChoice(INT32 choice);

void TSoURDt3rd_M_HandleAddonsMenu(INT32 choice);

#ifdef HAVE_DISCORDSUPPORT
void DISC_M_UpdateRequestsMenu(void);
void TSoURDt3rd_M_DiscordOptions_Init(INT32 choice);
void TSoURDt3rd_M_DiscordRequests_Init(INT32 choice);
#endif

void TSoURDt3rd_M_Main_InitOptions(INT32 choice);
void TSoURDt3rd_M_OptionsTick(void);
boolean TSoURDt3rd_M_OptionsInputs(INT32 ch);
boolean TSoURDt3rd_M_OptionsQuit(void);
void TSoURDt3rd_M_ResetOptions(void);

#ifdef STAR_LIGHTING
void TSoURDt3rd_M_CoronaLighting_Init(void);
#endif

void TSoURDt3rd_M_EXMusic_LoadMenu(INT32 choice);

void TSoURDt3rd_M_Jukebox_Init(INT32 choice);
void TSoURDt3rd_M_Jukebox_Ticker(void);
boolean TSoURDt3rd_M_Jukebox_Quit(void);

void TSoURDt3rd_M_Snake_Free(void);
boolean TSoURDt3rd_M_Snake_Init(INT32 choice);

void TSoURDt3rd_M_HandleColorResetOption(
	player_t *setupm_player,
	INT32 *setupm_fakeskin, consvar_t *setupm_cvdefaultskin,
	consvar_t *setupm_cvdefaultcolor, menucolor_t *setupm_fakecolor,
	void (*after_routine)(void)
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_M_SYS__
