// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_m_draw.c
/// \brief Unique TSoURDt3rd menu drawing routines

#include "smkg_m_func.h"
#include "smkg_m_draw.h"
#include "../m_menu.h"

#include "../drrr/k_menu.h"
#include "../drrr/kv_video.h"

#include "../../console.h"
#include "../../i_time.h"
#include "../../m_easing.h"
#include "../../r_main.h"
#include "../../w_wad.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

UINT8 tsourdt3rd_wipedefs[TSOURDT3RD_NUMWIPEDEFS] = {
	1,  // tsourdt3rd_wipe_menu_toblack
	1,  // tsourdt3rd_wipe_menu_final
};

// ------------------------ //
//        Functions
// ------------------------ //

fixed_t M_TimeFrac(tic_t tics, tic_t duration)
{
	return tics < duration ? (tics * FRACUNIT + rendertimefrac) / duration : FRACUNIT;
}

fixed_t M_ReverseTimeFrac(tic_t tics, tic_t duration)
{
	return FRACUNIT - M_TimeFrac(duration - tics, duration);
}

fixed_t M_DueFrac(tic_t start, tic_t duration)
{
	tic_t t = I_GetTime();
	tic_t n = t - start;
	return M_TimeFrac(min(n, duration), duration);
}

//
// void STAR_M_PreDrawer(void)
// Draws graphics in the back, before all the main M_Drawer() graphics are rendered.
//
void STAR_M_PreDrawer(void)
{
	if (menuwipe)
		F_WipeStartScreen();

	if (menuactive)
	{
		V_DrawThinString(vid.dupx, vid.height - 41*vid.dupy, V_NOSCALESTART|V_TRANSLUCENT|V_ALLOWLOWERCASE, TSOURDT3RDVERSIONSTRING);
		V_DrawThinString(vid.dupx, vid.height - 33*vid.dupy, V_NOSCALESTART|V_TRANSLUCENT|V_ALLOWLOWERCASE, TSOURDT3RDBYSTARMANIAKGSTRING);
	}
}

//
// void STAR_M_PostDrawer(void)
// Draws graphics in the very front, after all the main M_Drawer() graphics have been rendered.
//
void STAR_M_PostDrawer(void)
{
	// foreground layer
	if (menuactive)
	{
		if (tsourdt3rd_currentMenu != NULL && tsourdt3rd_currentMenu->drawroutine)
			tsourdt3rd_currentMenu->drawroutine(); // call current menu Draw routine

		//M_DrawMenuForeground();

		// Draw typing overlay when needed, above all other menu elements.
		if (menutyping.active)
			M_DrawMenuTyping();

		// Draw message overlay when needed
		M_DrawMenuMessage();
	}

	if (menuwipe)
	{
		F_WipeEndScreen();
		F_RunWipe(tsourdt3rd_wipedefs[tsourdt3rd_wipe_menu_final], false);
		menuwipe = false;
	}
}

//
// void STAR_M_DrawMenuTooltips(void)
// Draw a banner across the top of the screen, with a description of the current option displayed.
//
// Inspired by M_DrawMenuTooltips() from DRRR!
//
void STAR_M_DrawMenuTooltips(void)
{
	INT32 width;

	if (tsourdt3rd_currentMenu == NULL || tsourdt3rd_currentMenu->menuitems == NULL)
		return;

	if (tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].tooltip != NULL)
	{
		width = V_ThinStringWidth(tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].tooltip, V_SNAPTOTOP);

		V_DrawFixedPatch(0, 0, FRACUNIT, V_SNAPTOTOP, W_CachePatchName("MENUHINT", PU_CACHE), NULL);
		if (width > BASEVIDWIDTH)
			V_DrawCenteredSmallThinString(BASEVIDWIDTH/3, 15, V_SNAPTOTOP, tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].tooltip);
		else
			V_DrawCenteredThinString(BASEVIDWIDTH/2, 15, V_SNAPTOTOP, tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].tooltip);
	}
}

//
// void STAR_M_DrawQuitGraphic(void)
// Draws a quit graphic for us.
//
void STAR_M_DrawQuitGraphic(void)
{
	const char *quitgfx;

	switch (cv_quitscreen.value)
	{
		case 1: // aseprite moment
			quitgfx = "SS_QCOLR";
			break;
		case 2: // funny aseprite moment
			quitgfx = "SS_QSMUG";
			break;
		case 3: // kel world aseprite moment
			quitgfx = "SS_QKEL";
			break;
		case 4: // secret aseprite moment
			quitgfx = "SS_QATRB";
			break;
		default: // Demo 3 Quit Screen Tails 06-16-2001
			quitgfx = "GAMEQUIT";
			break;
	}

	V_DrawScaledPatch(0, 0, 0, W_CachePatchName(quitgfx, PU_PATCH));
	if (cv_quitscreen.value)
	{
		// psst, disclaimer; this game should not be sold :p
		V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QDISC", PU_PATCH));
	}
}
