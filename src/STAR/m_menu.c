// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  m_menu.c
/// \brief TSoURDt3rd related menu functions, structures, and data

#include "ss_main.h"
#include "menus/smkg-m_sys.h" // menumessage //
#include "smkg-jukebox.h"

#include "../doomstat.h"
#include "../d_clisrv.h"
#include "../d_player.h"
#include "../g_game.h"
#include "../p_local.h"
#include "../w_wad.h"
#include "../i_time.h"
#include "../i_system.h"
#include "../i_video.h"
#include "../v_video.h"
#include "../m_misc.h"
#include "../r_draw.h"
#include "../r_main.h" // renderdeltatics
#include "../hu_stuff.h"
#include "../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_menu_t *tsourdt3rd_currentMenu = NULL; // pointer to the current unique menu

// ------------------------ //
//        Functions
// ------------------------ //

// =======
// DRAWING
// =======

//
// void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed)
// Draws a button graphic on the screen. Changes upon being pressed.
//
// Ported from Dr. Robotnik's Ring Racers!
//
void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed)
{
	V_DrawFixedPatch(x, y, FRACUNIT, flags, button[(pressed == true) ? 1 : 0], NULL);
}

//
// void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic)
// Draws a button graphic on the screen, and animates it. Changes upon being pressed.
//
// Ported from Dr. Robotnik's Ring Racers!
//
void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic)
{
	const UINT8 anim_duration = 16;
	const boolean anim = ((animtic % (anim_duration * 2)) < anim_duration);
	K_drawButton(x << FRACBITS, y << FRACBITS, flags, button, anim);
}

//
// void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall)
// Draws a sticker graphic on the HUD.
//
// Ported from Dr. Robotnik's Ring Racers!
//
void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall)
{
	patch_t *stickerEnd;
	INT32 height;

	if (isSmall == true)
	{
		stickerEnd = (patch_t*)(W_CachePatchName("K_STIKE2", PU_CACHE));
		height = 6;
	}
	else
	{
		stickerEnd = (patch_t*)(W_CachePatchName("K_STIKEN", PU_CACHE));
		height = 11;
	}

	V_DrawFixedPatch(x*FRACUNIT, y*FRACUNIT, FRACUNIT, flags, stickerEnd, NULL);
	V_DrawFill(x, y, width, height, 24|flags);
	V_DrawFixedPatch((x + width)*FRACUNIT, y*FRACUNIT, FRACUNIT, flags|V_FLIP, stickerEnd, NULL);
}

// =======
// SERVERS
// =======

//
// void M_HandleMasterServerResetChoice(INT32 choice)
// Handles resetting the master server address.
//
// Ported from Kart!
//
void M_HandleMasterServerResetChoice(INT32 choice)
{
	if (choice == 'y' || choice == KEY_ENTER)
	{
		CV_Set(&cv_masterserver, cv_masterserver.defaultvalue);
		S_StartSound(NULL, sfx_s221);
	}
	tsourdt3rd_local.ms_address_changed = true;
}

//
// void M_PreStartServerMenuChoice(INT32 choice)
// Alerts the server menu to reset the master server address.
//
// Ported from Kart!
//
void M_PreStartServerMenuChoice(INT32 choice)
{
	M_HandleMasterServerResetChoice(choice);
	M_StartServerMenu(-1);
}

//
// void M_PreConnectMenuChoice(INT32 choice)
// Alerts the connect menu to reset the master server address.
//
// Ported from Kart!
//
void M_PreConnectMenuChoice(INT32 choice)
{
	M_HandleMasterServerResetChoice(choice);
	M_ConnectMenuModChecks(-1);
}
