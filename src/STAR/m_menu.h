
#ifndef __STAR_M_MENU__
#define __STAR_M_MENU__

#include "star_vars.h"
#include "ss_cmds.h"
#include "../m_menu.h"
#include "../s_sound.h"
#include "../r_defs.h"
#include "../v_video.h"
#include "../m_cond.h"

// ------------------------ //
//       Definitions
// ------------------------ //
#define MTREE5(a,b,c,d,e) MTREE2(a, MTREE4(b,c,d,e)) // (just in case)

// ===================================================================
// Dynamic Video Colors
// 	Remember this from some previous TSoURDt3rd commits of st_stuff.c?
// 		Yeah, I reworked it! Just like I said I would!
//		But this time, I've reworked it even more!
// ===================================================================
#define V_MENUCOLORMAP (cv_menucolor.value)
#define V_FPSCOLORMAP (cv_fpscountercolor.value)
#define V_TPSCOLORMAP (cv_tpscountercolor.value)

// ------------------------ //
//        Variables
// ------------------------ //
extern INT32   (*setupcontrols)[2];  // pointer to the gamecontrols of the player being edited

extern INT16 skullAnimCounter; // skull animation counter; Prompts: Chevron animation

extern INT16 MessageMenuDisplay[3][256]; // TO HACK

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
// Quit Messages
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

	// TSoURDt3rd unique messages! //
	QUITSMSG1,
	QUITSMSG2,
	QUITSMSG3,
	QUITSMSG4,
	QUITSMSG5,
	QUITSMSG6,

	// April Fools unique messages //
	QUITAMSG1,
	QUITAMSG2,
	QUITAMSG3,
	QUITAMSG4,

	NUM_QUITMESSAGES
} text_enum;

extern const char *quitmsg[NUM_QUITMESSAGES];

// =======
// Jukebox
// =======
extern musicdef_t *curplaying;

extern fixed_t st_time;
extern INT32 st_sel, st_cc;

extern patch_t* st_radio[9]; /* STAR NOTE: now externed in STAR/m_menu.h */
extern patch_t* st_launchpad[4]; /* STAR NOTE: now externed in STAR/m_menu.h */

extern UINT8 skyRoomMenuTranslations[MAXUNLOCKABLES];

// ------------------------ //
//        	Menus
// ------------------------ //
extern menu_t OP_MainDef;
extern menuitem_t MainMenu[];

extern menuitem_t MPauseMenu[];
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
void M_DrawGenericScrollMenu(void);
void M_DrawControl(void);

void M_ChangeControl(INT32 choice);

void M_ShiftMessageQueueDown(void);

void M_HandleMasterServerResetChoice(INT32 choice);

void M_PreStartServerMenuChoice(INT32 choice);
void M_PreConnectMenuChoice(INT32 choice);

void M_StartServerMenu(INT32 choice);
void M_ConnectMenuModChecks(INT32 choice);

// =========
// Quit Game
// =========
void STAR_M_InitQuitMessages(void);
void STAR_M_InitDynamicQuitMessages(void);

INT32 STAR_M_SelectQuitMessage(void);
const char *STAR_M_SelectQuitGraphic(void);

// =======
// Jukebox
// =======
void M_CacheSoundTest(void);
boolean TSoURDt3rd_M_IsJukeboxUnlocked(TSoURDt3rdJukebox_t *TSoURDt3rdJukebox);
void M_TSoURDt3rdJukebox(INT32 choice);

#endif // __STAR_M_MENU__
