// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2016 by Kay "Kaito" Sinclaire.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  k_menu.h
/// \brief Menu widget stuff, selection and such

#ifndef __K_MENU_H__
#define __K_MENU_H__

#include "../../m_menu.h"

#ifdef __cplusplus
extern "C" {
#endif

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
} menutyping;
// While typing, we'll have a fade strongly darken the screen to overlay the typing menu instead

typedef enum
{
	MA_NONE = 0,
	MA_YES,
	MA_NO
} manswer_e;

#define MAXMENUMESSAGE 256
#define MENUMESSAGECLOSE 2
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

void DRRR_M_StartMessage(const char *header, const char *string, void (*routine)(INT32), menumessagetype_t itemtype, const char *confirmstr, const char *defaultstr);
boolean M_MenuMessageTick(void);
void M_HandleMenuMessage(void);
void DRRR_M_StopMessage(INT32 choice);
void M_DrawMenuMessage(void);

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

void M_DrawMenuTyping(void);

void M_OpenVirtualKeyboard(size_t cachelen, vkb_query_fn_t queryfn, menu_t *dummymenu);
boolean M_CheckVirtualStringLength(void);
void M_AbortVirtualKeyboard(void);
void M_MenuTypingInput(INT32 key);
void M_SwitchVirtualKeyboard(boolean gamepad);

void DRRR_M_DrawMessageMenu(void);

void M_SetMenuDelay(UINT8 i);

boolean M_MenuButtonPressed(UINT8 pid, UINT32 bt);
boolean M_MenuButtonHeld(UINT8 pid, UINT32 bt);

boolean M_MenuConfirmPressed(UINT8 pid);
boolean M_MenuConfirmHeld(UINT8 pid);
boolean M_MenuBackPressed(UINT8 pid);
boolean M_MenuBackHeld(UINT8 pid);
boolean M_MenuExtraPressed(UINT8 pid);
boolean M_MenuExtraHeld(UINT8 pid);

// For some menu highlights
UINT16 M_GetCvPlayerColor(UINT8 pnum);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __K_MENU_H__
