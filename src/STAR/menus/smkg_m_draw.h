// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_m_draw.h
/// \brief Unique TSoURDt3rd menu drawing routines, global header

#ifndef __SMKG_M_DRAW__
#define __SMKG_M_DRAW__

#include "../drrr/kg_input.h"
#include "../m_menu.h"

typedef struct menu_anim_s
{
	tic_t start;
	INT16 dist;
} menu_anim_t;

#define M_OPTIONS_OFSTIME 5
// Keep track of some options properties
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
	//profile_t *profile;			// Pointer to the profile we're editing

	// Temporary buffer where we're gonna store game controls.
	// This is only applied to the profile when you exit out of the controls menu.

	INT16 controlscroll;		// scrolling for the control menu....
	INT16 bindtimer;			// Timer until binding is cancelled (5s)
	UINT16 bindben;				// Hold right timer
	UINT8 bindben_swallow;		// (bool) control is about to be cleared; (int) swallow/pose animation timer

	INT16 trycontroller;		// Starts at 3*TICRATE, holding B lowers this, when at 0, cancel controller try mode.

	// Used for horrible axis shenanigans
	INT32 lastkey;
	tic_t keyheldfor;

	// controller coords...
	// Works the same as (t)opt
	INT16 contx;
	INT16 conty;
	INT16 tcontx;
	INT16 tconty;

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

extern struct menutransition_s {
	INT16 tics;
	INT16 dest;
	tsourdt3rd_menu_t *startmenu;
	tsourdt3rd_menu_t *endmenu;
	boolean in;
	boolean enabled;
} menutransition;

extern boolean menuwipe;

void STAR_M_PreDrawer(void);
void STAR_M_PostDrawer(void);

void STAR_M_DrawMenuTooltips(void);

void STAR_M_DrawQuitGraphic(void);

#endif // __SMKG_M_DRAW__
