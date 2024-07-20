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
#include "../../z_zone.h"
#include "../../r_main.h"
#include "../../r_draw.h"
#include "../../w_wad.h"

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
		F_RunWipe(1, false); // wipe_menu_final
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

struct optionsmenu_s optionsmenu;

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

// FIXME: C++ template
#define M_EaseWithTransition(EasingFunc, N) \
	(menutransition.tics != menutransition.dest ? EasingFunc(menutransition.in ?\
		M_ReverseTimeFrac(menutransition.tics, menutransition.endmenu->transitionTics) :\
		M_TimeFrac(menutransition.tics, menutransition.startmenu->transitionTics), 0, N) : 0)

void M_DrawOptionsMovingButton(void)
{
	patch_t *butt = W_CachePatchName("OPT_BUTT", PU_CACHE);
	UINT8 *c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
	fixed_t t = M_DueFrac(optionsmenu.topt_start, M_OPTIONS_OFSTIME);
	fixed_t z = Easing_OutSine(M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t tx = Easing_OutQuad(t, optionsmenu.optx * FRACUNIT, optionsmenu.toptx * FRACUNIT) + z;
	fixed_t ty = Easing_OutQuad(t, optionsmenu.opty * FRACUNIT, optionsmenu.topty * FRACUNIT) + z;

	V_DrawFixedPatch(tx, ty, FRACUNIT, 0, butt, c);

	const char *s = OP_TSoURDt3rdOptionsDef.menuitems[OP_TSoURDt3rdOptionsDef.lastOn].text;
	fixed_t w = V_StringScaledWidth(
		FRACUNIT,
		FRACUNIT,
		FRACUNIT,
		V_ALLOWLOWERCASE,
		GM_FONT,
		s
	);
	DRRR_V_DrawStringScaled(
		tx - 3*FRACUNIT - (w/2),
		ty - 16*FRACUNIT,
		FRACUNIT,
		FRACUNIT,
		FRACUNIT,
		V_ALLOWLOWERCASE,
		c,
		GM_FONT,
		s
	);
}

void M_DrawTSoURDt3rdOptions(void)
{
	UINT8 i;
	fixed_t t = Easing_OutSine(M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t x = (140 - (48*tsourdt3rd_itemOn))*FRACUNIT + t;
	fixed_t y = 70*FRACUNIT + t;
	fixed_t tx = M_EaseWithTransition(Easing_InQuart, 5 * 64 * FRACUNIT);
	patch_t *buttback = W_CachePatchName("OPT_BUTT", PU_CACHE);

	UINT8 *c = NULL;

	for (i=0; i < currentMenu->numitems; i++)
	{
		fixed_t py = y - (tsourdt3rd_itemOn*48)*FRACUNIT;
		fixed_t px = x - tx;
		INT32 tflag = 0;

		if (i == tsourdt3rd_itemOn)
			c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
		else
			c = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_BLACK, GTC_CACHE);

		if (currentMenu->menuitems[i].status & IT_TRANSTEXT)
			tflag = V_TRANSLUCENT;

		if (!(menutransition.tics != menutransition.dest && i == tsourdt3rd_itemOn))
		{
			V_DrawFixedPatch(px, py, FRACUNIT, 0, buttback, c);

			const char *s = currentMenu->menuitems[i].text;
			fixed_t w = V_StringScaledWidth(
				FRACUNIT,
				FRACUNIT,
				FRACUNIT,
				V_ALLOWLOWERCASE,
#if 0
				GM_FONT,
#else
				TINY_FONT,
#endif
				s
			);
#if 0
			DRRR_V_DrawStringScaled(
				px - 3*FRACUNIT - (w/2),
				py - 16*FRACUNIT,
				FRACUNIT,
				FRACUNIT,
				FRACUNIT,
				V_ALLOWLOWERCASE|tflag,
				(i == tsourdt3rd_itemOn ? c : NULL),
#if 0
				GM_FONT,
#else
				TINY_FONT,
#endif
				s
			);
#else
			V_DrawThinString(
				px - 3*FRACUNIT - (w/2),
				py - 16*FRACUNIT,
				V_ALLOWLOWERCASE|tflag,
				s
			);
#endif
		}

		y += 48*FRACUNIT;
		x += 48*FRACUNIT;
	}

	STAR_M_DrawMenuTooltips();

	if (menutransition.tics != menutransition.dest)
		M_DrawOptionsMovingButton();

}

static void M_DrawOptionsBoxTerm(INT32 x, INT32 top, INT32 bottom)
{
	INT32 px = x - 20;

	V_DrawFill(px, top + 4, 2, bottom - top, orangemap[0]);
	V_DrawFill(px + 1, top + 5, 2, bottom - top, 31);

	V_DrawFill(BASEVIDWIDTH - px - 2, top + 4, 2, bottom - top, orangemap[0]);
	V_DrawFill(BASEVIDWIDTH - px, top + 5, 1, bottom - top, 31);

	V_DrawFill(px, bottom + 2, BASEVIDWIDTH - (2 * px), 2, orangemap[0]);
	V_DrawFill(px, bottom + 3, BASEVIDWIDTH - (2 * px), 2, 31);
}

void M_ResetOptions(void)
{
	optionsmenu.ticker = 0;
	optionsmenu.offset.start = 0;

	optionsmenu.optx = 0;
	optionsmenu.opty = 0;
	optionsmenu.toptx = 0;
	optionsmenu.topty = 0;

	// BG setup:
	optionsmenu.currcolour = SKINCOLOR_COBALT; //OP_TSoURDt3rdOptionsDef.extra1;
	optionsmenu.lastcolour = 0;
	optionsmenu.fade = 0;

#if 0
	// For profiles:
	memset(setup_player, 0, sizeof(setup_player));
	optionsmenu.profile = NULL;
#endif
}

void M_InitOptions(INT32 choice)
{
	(void)choice;

	M_ResetOptions();

	// So that pause doesn't go to the main menu...
	OP_TSoURDt3rdOptionsDef.prevMenu = currentMenu;

	M_SetupNextMenu(&OP_TSoURDt3rdOptionsDef);
}

boolean M_OptionsQuit(void)
{
	optionsmenu.toptx = 140;
	optionsmenu.topty = 70;

#if 0
	// Reset button behaviour because profile menu is different, since of course it is.
	if (optionsmenu.resetprofilemenu)
	{
		optionsmenu.profilemenu = false;
		optionsmenu.profile = NULL;
		optionsmenu.resetprofilemenu = false;
	}
#endif

	return true;	// Always allow quitting, duh.
}

void M_OptionsTick(void)
{
	boolean instanttransmission = optionsmenu.ticker == 0 && menuwipe;

	optionsmenu.ticker++;

	// Move the button for cool animations
	if (currentMenu == &OP_TSoURDt3rdOptionsDef)
	{
		M_OptionsQuit();	// ...So now this is used here.
	}
#if 0
	else if (optionsmenu.profile == NULL)	// Not currently editing a profile (otherwise we're using these variables for other purposes....)
#else
	else
#endif
	{
		// I don't like this, it looks like shit but it needs to be done..........
		if (optionsmenu.profilemenu)
		{
			optionsmenu.toptx = 440;
			optionsmenu.topty = 70+1;
		}
#if 0
		else if (currentMenu == &OPTIONS_GameplayItemsDef)
		{
			optionsmenu.toptx = -160; // off the side of the screen
			optionsmenu.topty = 50;
		}
#endif
		else
		{
			optionsmenu.toptx = 160;
			optionsmenu.topty = 50;
		}
	}

	// Handle the background stuff:
	if (optionsmenu.fade)
		optionsmenu.fade--;

	// change the colour if we aren't matching the current menu colour
	if (instanttransmission)
	{
		optionsmenu.currcolour = SKINCOLOR_GALAXY; //currentMenu->extra1;
		optionsmenu.offset.start = optionsmenu.fade = 0;

		optionsmenu.optx = optionsmenu.toptx;
		optionsmenu.opty = optionsmenu.topty;
	}
	else
	{
		if (optionsmenu.fade)
			optionsmenu.fade--;
#if 0
		if (optionsmenu.currcolour != currentMenu->extra1)
			M_OptionsChangeBGColour(currentMenu->extra1);
#endif

		if (optionsmenu.optx != optionsmenu.toptx || optionsmenu.opty != optionsmenu.topty)
		{
			tic_t t = I_GetTime();
			tic_t n = t - optionsmenu.topt_start;
			if (n == M_OPTIONS_OFSTIME)
			{
				optionsmenu.optx = optionsmenu.toptx;
				optionsmenu.opty = optionsmenu.topty;
			}
			else if (n > M_OPTIONS_OFSTIME)
			{
				optionsmenu.topt_start = I_GetTime();
			}
		}

		//M_GonerCheckLooking();
	}
}

boolean M_OptionsInputs(INT32 ch)
{
	const UINT8 pid = 0;
	(void)ch;

	if (menucmd[pid].dpad_ud > 0)
	{
		M_SetMenuDelay(pid);
		optionsmenu.offset.dist = 48;
		STAR_M_NextOpt();
		S_StartSound(NULL, sfx_s3k5b);

		if (tsourdt3rd_itemOn == 0)
			optionsmenu.offset.dist -= currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();

		return true;
	}
	else if (menucmd[pid].dpad_ud < 0)
	{
		M_SetMenuDelay(pid);
		optionsmenu.offset.dist = -48;
		STAR_M_PrevOpt();
		S_StartSound(NULL, sfx_s3k5b);

		if (tsourdt3rd_itemOn == currentMenu->numitems-1)
			optionsmenu.offset.dist += currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();

		return true;
	}
	else if (M_MenuConfirmPressed(pid))
	{
		if (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TRANSTEXT)
			return true;	// No.

		optionsmenu.optx = 140;
		optionsmenu.opty = 70;	// Default position for the currently selected option.
		return false;	// Don't eat.
	}
	return false;
}

#define GM_STARTX 128
#define GM_STARTY 80
#define GM_XOFFSET 17
#define GM_YOFFSET 34
#define GM_FLIPTIME 5

static tic_t gm_flipStart;

//
// M_DrawKartGamemodeMenu
// Huge gamemode-selection list for main menu
//
void M_DrawKartGamemodeMenu(void)
{
	UINT8 n = 0;
	INT32 i, x, y;
	INT32 tx = M_EaseWithTransition(Easing_Linear, 5 * 48);

	for (i = 0; i < currentMenu->numitems; i++)
	{
		if (currentMenu->menuitems[i].status == IT_DISABLED)
		{
			continue;
		}

		n++;
	}

	n--;
	x = GM_STARTX - ((GM_XOFFSET / 2) * (n-1)) + tx;
	y = GM_STARTY - ((GM_YOFFSET / 2) * (n-1));

	STAR_M_DrawMenuTooltips();

	for (i = 0; i < currentMenu->numitems; i++)
	{
		INT32 type;

		if (currentMenu->menuitems[i].status == IT_DISABLED)
		{
			continue;
		}

		if (i >= currentMenu->numitems-1)
		{
			x = GM_STARTX + (GM_XOFFSET * 5 / 2) + tx;
			y = GM_STARTY + (GM_YOFFSET * 5 / 2);

		}

		INT32 cx = x;
		boolean selected = (i == tsourdt3rd_itemOn && menutransition.tics == menutransition.dest);

		if (selected)
		{
			fixed_t f = M_DueFrac(gm_flipStart, GM_FLIPTIME);
			cx -= Easing_OutSine(f, 0, (GM_XOFFSET / 2));
		}

		type = (currentMenu->menuitems[i].status & IT_DISPLAY);

		switch (type)
		{
			case IT_STRING:
			case IT_TRANSTEXT2:
				{
					UINT8 *colormap = NULL;

					if (selected)
					{
						colormap = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
					}
					else
					{
						colormap = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_MOSS, GTC_CACHE);
					}

					V_DrawFixedPatch(cx*FRACUNIT, y*FRACUNIT, FRACUNIT, 0, W_CachePatchName("MENUPLTR", PU_CACHE), colormap);
#if 0
					DRRR_V_DrawGamemodeString(cx + 16, y - 3,
						(type == IT_TRANSTEXT2
							? V_TRANSLUCENT
							: 0
						),
						colormap,
						currentMenu->menuitems[i].text);
#else
					V_DrawThinString(cx + 16, y - 3,
						(type == IT_TRANSTEXT2
							? V_TRANSLUCENT
							: 0
						),
						currentMenu->menuitems[i].text);
#endif
				}
				break;
		}

		x += GM_XOFFSET;
		y += GM_YOFFSET;
	}
}

void M_FlipKartGamemodeMenu(boolean slide)
{
	gm_flipStart = slide ? I_GetTime() : 0;
}
