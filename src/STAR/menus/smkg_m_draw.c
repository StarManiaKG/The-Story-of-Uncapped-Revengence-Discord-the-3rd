// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_draw.c
/// \brief Unique TSoURDt3rd menu drawing routines

#include "smkg-m_sys.h"

#include "../../am_map.h"
#include "../../console.h"
#include "../../g_game.h"
#include "../../i_time.h"
#include "../../m_easing.h"
#include "../../r_main.h"
#include "../../w_wad.h"
#include "../../r_draw.h"
#include "../../v_video.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

UINT8 tsourdt3rd_wipedefs[TSOURDT3RD_NUMWIPEDEFS] = {
	99, // tsourdt3rd_wipe_init_tsourdt3rd_menu_toblack
	1,  // tsourdt3rd_wipe_menu_toblack
	1,  // tsourdt3rd_wipe_menu_final
};

#define SKULLXOFF -32
#define LINEHEIGHT 13
#define STRINGHEIGHT 9
#define SMALLLINEHEIGHT 9
#define SLIDER_RANGE 10
#define SLIDER_WIDTH (8*SLIDER_RANGE+6)

// ------------------------ //
//        Functions
// ------------------------ //

UINT16 TSoURDt3rd_M_GetCvPlayerColor(UINT8 pnum)
{
	UINT8 main_player = (pnum == consoleplayer || pnum == 0);

	if (pnum >= MAXPLAYERS)
		return SKINCOLOR_NONE;

	UINT16 color = (main_player ? cv_playercolor.value : cv_playercolor2.value);
	if (color != SKINCOLOR_NONE)
		return color;

	INT32 skin = R_SkinAvailable((main_player ? cv_skin.string : cv_skin2.string));
	if (skin == -1)
		return SKINCOLOR_NONE;

	return skins[skin].prefcolor;
}

fixed_t TSoURDt3rd_M_TimeFrac(tic_t tics, tic_t duration)
{
	return tics < duration ? (tics * FRACUNIT + rendertimefrac) / duration : FRACUNIT;
}

fixed_t TSoURDt3rd_M_ReverseTimeFrac(tic_t tics, tic_t duration)
{
	return FRACUNIT - TSoURDt3rd_M_TimeFrac(duration - tics, duration);
}

fixed_t TSoURDt3rd_M_DueFrac(tic_t start, tic_t duration)
{
	tic_t t = I_GetTime();
	tic_t n = t - start;
	return TSoURDt3rd_M_TimeFrac(min(n, duration), duration);
}

INT32 TSoURDt3rd_M_DrawCaretString
(
	INT32 x, INT32 y,
	INT32 flags,
	fixed_t pscale, fixed_t vscale,
	const char *string, fontdef_t font
)
{
	V_DrawFontString(x, y, flags, pscale, vscale, string, font);
	return V_FontStringWidth(string, flags, font);
}

static INT32 M_SliderX(INT32 range)
{
	if (range < 0)
		range = 0;
	if (range > 100)
		range = 100;

	return -4 + (((SLIDER_RANGE)*8 + 4)*range)/100;
}

//  A smaller 'Thermo', with range given as percents (0-100)
static void M_DrawSlider(INT32 x, INT32 y, const consvar_t *cv, boolean ontop)
{
	x = BASEVIDWIDTH - x - SLIDER_WIDTH;
	V_DrawFill(x - 5, y + 3, SLIDER_WIDTH + 3, 5, 31);
	V_DrawFill(x - 4, y + 4, SLIDER_WIDTH, 2, orangemap[0]);

	if (ontop)
	{
		V_DrawString(x - 16 - (tsourdt3rd_skullAnimCounter/5), y,
			tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1C"); // left arrow
		V_DrawString(x+(SLIDER_RANGE*8) + 8 + (tsourdt3rd_skullAnimCounter/5), y,
			tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1D"); // right arrow
	}

	INT32 range = cv->PossibleValue[1].value - cv->PossibleValue[0].value;
	INT32 val = atoi(cv->defaultvalue);

	val = (val - cv->PossibleValue[0].value) * 100 / range;
	// draw the default tick
	V_DrawFill(x + M_SliderX(val), y + 2, 3, 4, 31);

	val = (cv->value - cv->PossibleValue[0].value) * 100 / range;
	INT32 px = x + M_SliderX(val);

	// draw the slider cursor
	V_DrawFill(px - 1, y - 1, 5, 11, 31);
	V_DrawFill(px, y, 2, 8, aquamap[0]);
}

static void M_DrawCursorHand(INT32 x, INT32 y)
{
	V_DrawScaledPatch(x - 24 - (I_GetTime() % 16 < 8), y, 0, W_CachePatchName("M_CURSOR", PU_CACHE));
}

static void M_DrawUnderline(INT32 left, INT32 right, INT32 y)
{
	if (menutransition.tics == menutransition.dest)
		V_DrawFill(left - 1, y + 5, (right - left) + 11, 2, 31);
}

//
// void TSoURDt3rd_M_PreDrawer(void)
// Draws graphics in the back, before all the main M_Drawer() graphics are rendered.
//
void TSoURDt3rd_M_PreDrawer(void)
{
	if (menuwipe)
		F_WipeStartScreen();

	TSoURDt3rd_M_Jukebox_Ticker();
}

//
// void TSoURDt3rd_M_PostDrawer(void)
// Draws graphics in the foreground layer, after all the main M_Drawer() graphics have been rendered.
//
void TSoURDt3rd_M_PostDrawer(void)
{
	if (menuactive)
	{
		if (tsourdt3rd_currentMenu && tsourdt3rd_currentMenu->drawroutine)
			tsourdt3rd_currentMenu->drawroutine(); // call current menu Draw routine

		// Draw version down in corner
		// ...but still only in the MAIN MENU.
		// It looks out of place anywhere else, and gets in the way.
		if (currentMenu == &MainDef)
		{
			V_DrawThinString(vid.dupx, vid.height - 41*vid.dupy, V_NOSCALESTART|V_TRANSLUCENT|V_ALLOWLOWERCASE, TSOURDT3RDVERSIONSTRING);
			V_DrawThinString(vid.dupx, vid.height - 33*vid.dupy, V_NOSCALESTART|V_TRANSLUCENT|V_ALLOWLOWERCASE, "(By StarManiaKG!)");
		}

		// Draw typing overlay when needed, above all other menu elements.
		if (menutyping.active)
			TSoURDt3rd_M_DrawMenuTyping();
	}

	// Draw message overlay when needed
	TSoURDt3rd_M_DrawMenuMessage();

	if (menuwipe)
	{
		F_WipeEndScreen();
		F_RunWipe(tsourdt3rd_wipedefs[tsourdt3rd_wipe_menu_final], false);
		menuwipe = false;
	}
}

//
// void TSoURDt3rd_M_DrawMenuTooltips
// (
//	 fixed_t box_x, fixed_t box_y, INT32 box_flags, UINT8 *box_color, boolean box_vflip,
//	 fixed_t string_x, fixed_t string_y, INT32 string_flags, boolean string_centered
// )
//
// Draw a banner across the top of the screen, with a description of the current option displayed.
//
// Inspired by M_DrawMenuTooltips() from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_M_DrawMenuTooltips
(
	fixed_t box_x, fixed_t box_y, INT32 box_flags, UINT8 *box_color, boolean box_vflip,
	fixed_t string_x, fixed_t string_y, INT32 string_flags, boolean string_centered
)
{
	tsourdt3rd_menuitem_t *item = NULL;
	patch_t *box_patch = W_CachePatchName("MENUHINT", PU_CACHE);

	if (tsourdt3rd_currentMenu == NULL || tsourdt3rd_currentMenu->menuitems == NULL)
		return;
	item = &tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn];

	if (item != NULL && item->tooltip != NULL)
	{
		if (box_flags <= 0)
			box_flags |= V_SNAPTOTOP;
		if (box_vflip)
			box_patch = W_CachePatchName("VMNUHINT", PU_CACHE);

		if (string_flags <= 0)
			string_flags |= V_SNAPTOTOP|V_ALLOWLOWERCASE|V_MENUCOLORMAP;

		V_DrawMappedPatch(box_x, box_y, box_flags, box_patch, box_color);
		if (string_centered)
			V_DrawCenteredThinString(string_x, string_y, string_flags, item->tooltip);
		else
			V_DrawThinString(string_x, string_y, string_flags, item->tooltip);
	}
}

void TSoURDt3rd_M_DrawMediocreKeyboardKey(const char *text, INT32 *workx, INT32 worky, boolean push, boolean rightaligned)
{
	INT32 buttonwidth = V_StringWidth(text, V_ALLOWLOWERCASE) + 2;

	if (rightaligned)
	{
		(*workx) -= buttonwidth;
	}

	if (push)
	{
		worky += 2;
	}
	else
	{
		V_DrawFill((*workx)-1, worky+10, buttonwidth, 2, 24);
	}

	V_DrawFill((*workx)-1, worky, buttonwidth, 10, 16);
	V_DrawString(
		(*workx), worky + 1,
		V_ALLOWLOWERCASE, text
	);
}

//
// void TSoURDt3rd_M_DrawPauseGraphic(void)
// Draws a pause graphic on the screen when the game is paused,
//	determined by cv_tsourdt3rd_game_pausescreen.
//
void TSoURDt3rd_M_DrawPauseGraphic(void)
{
	INT32 py;
	patch_t *patch = W_CachePatchName("M_PAUSE", PU_PATCH);

	switch (cv_tsourdt3rd_game_pausescreen.value)
	{
		case 1: // Legacy
			py = 4;
			if (!automapactive)
				py += viewwindowy;
			V_DrawScaledPatch(viewwindowx + (BASEVIDWIDTH - patch->width)/2, py, 0, patch);
			break;
		default: // Default
			if (!automapactive)
				py = BASEVIDHEIGHT/2;
			else
				py = 32;
			M_DrawTextBox((BASEVIDWIDTH/2) - (60), py - (16), 13, 2);
			V_DrawCenteredString(BASEVIDWIDTH/2, py - (4), V_MENUCOLORMAP, "Game Paused");
			break;
	}
}

//
// void TSoURDt3rd_M_DrawQuitGraphic(void)
// Draws a quit graphic for us.
//
void TSoURDt3rd_M_DrawQuitGraphic(void)
{
	switch (cv_tsourdt3rd_game_quitscreen.value)
	{
		case 1: // aseprite moment
			V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QCOLR", PU_PATCH));
			break;
		case 2: // funny aseprite moments #5
			V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QSMUG", PU_PATCH));
			break;
		case 3: // kel world aseprite moment
			V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QKEL", PU_PATCH));
			break;
		case 4: // secret aseprite moment
			V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QATRB", PU_PATCH));
			break;
		default: // Demo 3 Quit Screen Tails 06-16-2001
			V_DrawScaledPatch(0, 0, 0, W_CachePatchName("GAMEQUIT", PU_PATCH));
			return;
	}

	// psst, disclaimer; this game should not be sold :p
	V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QDISC", PU_PATCH));
}

// ==========================================================================
// GENERIC MENUS
// ==========================================================================

//
// void TSoURDt3rd_M_DrawGenericOptions(void)
//
// Text-based list menu, seperating various options by using boxes.
// Used for TSoURDt3rd's various unique options.
//
// Ported from DRRR!
//
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

static void M_DrawLinkArrow(INT32 x, INT32 y, INT32 i)
{
	UINT8 ch = currentMenu->menuitems[i].text[0];

	V_DrawString(
		x + (i == tsourdt3rd_itemOn ? 1 + tsourdt3rd_skullAnimCounter/5 : 0),
		y - 1,
		// Use color of first character in text label
		i == tsourdt3rd_itemOn ? tsourdt3rd_highlightflags : (((max(ch, 0x80) - 0x80) & 15) << V_CHARCOLORSHIFT)|V_ALLOWLOWERCASE,
		"\x1D"
	);
}

void TSoURDt3rd_M_DrawOptionsMovingButton(void)
{
	patch_t *butt = W_CachePatchName("OPT_BUTT", PU_CACHE);
	UINT8 *c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
	fixed_t t = TSoURDt3rd_M_DueFrac(optionsmenu.topt_start, M_OPTIONS_OFSTIME);
	fixed_t z = Easing_OutSine(TSoURDt3rd_M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t tx = Easing_OutQuad(t, optionsmenu.optx * FRACUNIT, optionsmenu.toptx * FRACUNIT) + z;
	fixed_t ty = Easing_OutQuad(t, optionsmenu.opty * FRACUNIT, optionsmenu.topty * FRACUNIT) + z;
	const char *s = NULL;

	V_DrawFixedPatch(tx, ty, FRACUNIT, 0, butt, c);

	if (currentMenu)
	{
#ifdef HAVE_DISCORDSUPPORT
		if (currentMenu == &DISCORD_OP_MainDef)
			s = "Discord Options";
		else
#endif
		if (currentMenu == &TSoURDt3rd_OP_Extras_JukeboxDef)
			s = TSoURDt3rd_OP_ExtrasDef.menuitems[op_extras_jukebox].text;
#ifdef STAR_LIGHTING
		else if (currentMenu == &TSoURDt3rd_OP_Video_LightingDef)
			s = "Lighting Options";
#endif
		else
			s = TSoURDt3rd_OP_MainMenuDef.menuitems[TSoURDt3rd_OP_MainMenuDef.lastOn].text;
	}

	fixed_t w = V_FontStringWidth(
		s,
		V_ALLOWLOWERCASE,
		lt_font
	);
	V_DrawFontStringAtFixed(
		tx - 3*FRACUNIT - ((w/2)<<FRACBITS),
		ty - 16*FRACUNIT,
		V_ALLOWLOWERCASE,
		FRACUNIT,
		FRACUNIT,
		s,
		lt_font
	);
}

void TSoURDt3rd_M_DrawGenericOptions(void)
{
	INT32 x = currentMenu->x - M_EaseWithTransition(Easing_Linear, 5 * 48), y = currentMenu->y, w, i, cursory = -100;
	INT32 expand = -1;
	INT32 boxy = 0;
	boolean collapse = false;
	boolean opening = false;
	fixed_t boxt = 0;

	TSoURDt3rd_M_DrawMenuTooltips(
		0, 0, 0, NULL, false,
		BASEVIDWIDTH/2, 13, 0, true
	);
	TSoURDt3rd_M_DrawOptionsMovingButton();

	for (i = tsourdt3rd_itemOn; i >= 0; --i)
	{
		switch (currentMenu->menuitems[i].status & IT_DISPLAY)
		{
			case IT_DYBIGSPACE:
				goto box_found;

			case IT_HEADERTEXT:
				expand = i;
				goto box_found;
		}
	}
box_found:
	if (optionsmenu.box.dist != expand)
	{
		optionsmenu.box.dist = expand;
		optionsmenu.box.start = I_GetTime();
	}

	for (i = 0; i < currentMenu->numitems; i++)
	{
		boolean term = false;

		switch (currentMenu->menuitems[i].status & IT_DISPLAY)
		{
			case IT_DYBIGSPACE:
				collapse = false;
				term = (boxy != 0);
				break;

			case IT_HEADERTEXT:
				if (i != expand)
				{
					collapse = true;
					term = (boxy != 0);
				}
				else
				{
					if (collapse)
						y += 2;

					collapse = false;

					if (menutransition.tics == menutransition.dest)
					{
						INT32 px = x - 20;
						V_DrawFill(px, y + 4, BASEVIDWIDTH - (2 * px), 2, orangemap[0]);
						V_DrawFill(px + 1, y + 5, BASEVIDWIDTH - (2 * px), 2, 31);
					}

					boxy = y;

					boxt = optionsmenu.box.dist == expand ? TSoURDt3rd_M_DueFrac(optionsmenu.box.start, 5) : FRACUNIT;
					opening = boxt < FRACUNIT;
				}
				break;

			default:
				if (collapse)
					continue;
		}

		if (term)
		{
			if (menutransition.tics == menutransition.dest)
				M_DrawOptionsBoxTerm(x, boxy, Easing_Linear(boxt, boxy, y));

			y += SMALLLINEHEIGHT;
			boxy = 0;
			opening = false;
		}

		if (i == tsourdt3rd_itemOn && !opening)
		{
			cursory = y;
			M_DrawUnderline(x, BASEVIDWIDTH - x, y);
		}

		switch (currentMenu->menuitems[i].status & IT_DISPLAY)
		{
			case IT_PATCH:
				if (currentMenu->menuitems[i].patch && currentMenu->menuitems[i].patch[0])
				{
					if (currentMenu->menuitems[i].status & IT_CENTER)
					{
						patch_t *p;
						p = W_CachePatchName(currentMenu->menuitems[i].patch, PU_CACHE);
						V_DrawScaledPatch((BASEVIDWIDTH - SHORT(p->width))/2, y, 0, p);
					}
					else
					{
						V_DrawScaledPatch(x, y, 0,
							W_CachePatchName(currentMenu->menuitems[i].patch, PU_CACHE));
					}
				}
				/* FALLTHRU */
			case IT_NOTHING:
				y += SMALLLINEHEIGHT;
				break;

			case IT_DYBIGSPACE:
				y += SMALLLINEHEIGHT/2;
				break;
			case IT_STRING: {
				boolean textBox = (currentMenu->menuitems[i].status & IT_TYPE) == IT_CVAR &&
					(currentMenu->menuitems[i].status & IT_CVARTYPE) == IT_CV_STRING;

				if (textBox)
				{
					if (opening)
						y += LINEHEIGHT;
					else
						V_DrawFill(x+5, y+5, MAXSTRINGLENGTH*7+6, 9+6, 159);
				}

				if (opening)
				{
					y += STRINGHEIGHT;
					break;
				}

				INT32 px = x + ((currentMenu->menuitems[i].status & IT_TYPE) == IT_SUBMENU ? 8 : 0);

				if (i == tsourdt3rd_itemOn)
					cursory = y;

				if (i == tsourdt3rd_itemOn)
					V_DrawString(px + 1, y, tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
				else
					V_DrawString(px, y, (textBox ? V_GRAYMAP : 0)|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);

				// Cvar specific handling
				switch (currentMenu->menuitems[i].status & IT_TYPE)
				{
					case IT_SUBMENU:
					{
						M_DrawLinkArrow(x, y, i);
						break;
					}

					case IT_CVAR:
					{
						consvar_t *cv = (consvar_t *)currentMenu->menuitems[i].itemaction;
						if (cv == NULL)
							cv = tsourdt3rd_currentMenu->menuitems[i].itemaction.cvar;
						switch (currentMenu->menuitems[i].status & IT_CVARTYPE)
						{
							case IT_CV_SLIDER:
								M_DrawSlider(x, y, cv, (i == tsourdt3rd_itemOn));
							case IT_CV_NOPRINT: // color use this
							case IT_CV_INVISSLIDER: // monitor toggles use this
								break;
							case IT_CV_STRING:
								{
									INT32 xoffs = 6;
									char string[MAXSTRINGLENGTH+16+5] = "";

									if (tsourdt3rd_itemOn == i)
									{
										xoffs = 8;
										V_DrawString(x + (tsourdt3rd_skullAnimCounter/5) + 7, y + 9, tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1D");
									}

									for (INT32 stringlen = 0; cv->string[stringlen]; stringlen++)
									{
										if (stringlen <= MAXSTRINGLENGTH+16)
										{
											string[stringlen] = cv->string[stringlen];
											continue;
										}
										else if (stringlen <= MAXSTRINGLENGTH+16+3)
										{
											string[stringlen] = '.';
											continue;
										}

										string[stringlen] = '\0';
										break;
									}
									TSoURDt3rd_M_DrawCaretString(x + xoffs + 8, y + 9,
										V_ALLOWLOWERCASE,
										FRACUNIT, FRACUNIT,
										string, tny_font
									);

									y += LINEHEIGHT;
								}
								break;
							default:
							{
								boolean isDefault = CV_IsSetToDefault(cv);

								w = V_StringWidth(cv->string, V_ALLOWLOWERCASE);

								if (tsourdt3rd_currentMenu->extra2 && ((INT16)strlen(cv->string) >= tsourdt3rd_currentMenu->extra2))
								{
									w = V_ThinStringWidth(cv->string, V_ALLOWLOWERCASE);

									V_DrawThinString(BASEVIDWIDTH - x - w, y,
										(!isDefault ? tsourdt3rd_warningflags : tsourdt3rd_highlightflags)|V_ALLOWLOWERCASE, cv->string);

									if (i == tsourdt3rd_itemOn)
									{
										V_DrawThinString(BASEVIDWIDTH - x - 10 - w - (tsourdt3rd_skullAnimCounter/5), y - 1,
												tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1C"); // left arrow
										V_DrawThinString(BASEVIDWIDTH - x + 2 + (tsourdt3rd_skullAnimCounter/5), y - 1,
												tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1D"); // right arrow
									}

									if (!isDefault)
									{
										V_DrawThinString(BASEVIDWIDTH - x + (i == tsourdt3rd_itemOn ? 13 : 5), y - 2, tsourdt3rd_warningflags|V_ALLOWLOWERCASE, ".");
									}
									break;
								}

								V_DrawString(BASEVIDWIDTH - x - w, y,
									(!isDefault ? tsourdt3rd_warningflags : tsourdt3rd_highlightflags)|V_ALLOWLOWERCASE, cv->string);

								if (i == tsourdt3rd_itemOn)
								{
									V_DrawString(BASEVIDWIDTH - x - 10 - w - (tsourdt3rd_skullAnimCounter/5), y - 1,
											tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1C"); // left arrow
									V_DrawString(BASEVIDWIDTH - x + 2 + (tsourdt3rd_skullAnimCounter/5), y - 1,
											tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1D"); // right arrow
								}

								if (!isDefault)
								{
									V_DrawString(BASEVIDWIDTH - x + (i == tsourdt3rd_itemOn ? 13 : 5), y - 2, tsourdt3rd_warningflags|V_ALLOWLOWERCASE, ".");
								}
								break;
							}
						}
						break;
					}
				}

				y += STRINGHEIGHT;
				break;
			}
			case IT_STRING2:
				V_DrawString(x, y, V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
				/* FALLTHRU */
			case IT_DYLITLSPACE:
			case IT_SPACE:
				y += (tsourdt3rd_currentMenu->menuitems[i].mvar1 ? tsourdt3rd_currentMenu->menuitems[i].mvar1 : SMALLLINEHEIGHT);
				break;
			case IT_GRAYPATCH:
				if (currentMenu->menuitems[i].patch && currentMenu->menuitems[i].patch[0])
					V_DrawMappedPatch(x, y, 0,
						W_CachePatchName(currentMenu->menuitems[i].patch,PU_CACHE), graymap);
				y += (tsourdt3rd_currentMenu->menuitems[i].mvar1 ? tsourdt3rd_currentMenu->menuitems[i].mvar1 : SMALLLINEHEIGHT);
				break;
			case IT_TRANSTEXT:
				if (tsourdt3rd_currentMenu->menuitems[i].mvar1)
					y = currentMenu->y+tsourdt3rd_currentMenu->menuitems[i].mvar1;
				/* FALLTHRU */
			case IT_TRANSTEXT2:
				V_DrawString(x, y, V_TRANSLUCENT|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
				y += SMALLLINEHEIGHT;
				break;
			case IT_HEADERTEXT: // draws 16 pixels to the left, in yellow text
				if (tsourdt3rd_currentMenu->menuitems[i].mvar1)
					y = currentMenu->y+tsourdt3rd_currentMenu->menuitems[i].mvar1;

				V_DrawString(x - (collapse ? 0 : 16), y, TSOURDT3RD_M_ALTCOLOR|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
				y += SMALLLINEHEIGHT + 1;
				break;
		}
	}

	if (boxy && menutransition.tics == menutransition.dest)
		M_DrawOptionsBoxTerm(x, boxy, Easing_Linear(boxt, boxy, y));

	// DRAW THE SKULL CURSOR
	if (((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_DISPLAY) == IT_PATCH)
		|| ((currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_DISPLAY) == IT_NOTHING))
	{
		V_DrawScaledPatch(x + SKULLXOFF, cursory - 5, 0,
			W_CachePatchName("M_CURSOR", PU_CACHE));
	}
	else
	{
		M_DrawCursorHand(x, cursory);
	}
}
