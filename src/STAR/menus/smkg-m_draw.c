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

// Me when nobody looks at this file while I go ahead and revamp it

#include "smkg-m_sys.h"

#include "../smkg-st_hud.h"
#include "../misc/smkg-m_misc.h"

#include "../../am_map.h"
#include "../../console.h"
#include "../../g_game.h"
#include "../../i_time.h"
#include "../../m_easing.h"
#include "../../r_main.h"
#include "../../r_draw.h"
#include "../../v_video.h"
#include "../../w_wad.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

#define SKULLXOFF -32
#define LINEHEIGHT 13
#define STRINGHEIGHT 9
#define SMALLLINEHEIGHT 9
#define SLIDER_RANGE 10
#define SLIDER_WIDTH (8*SLIDER_RANGE+6)

static fixed_t tsourdt3rd_songcredit_ease;

// ------------------------ //
//        Functions
// ------------------------ //

enum
{
	Stretch_kNone,
	Stretch_kWidth,
	Stretch_kHeight,
	Stretch_kBoth,
};
void TSoURDt3rd_MK_DrawKeyboardString(fixed_t x, fixed_t y, int flags, const char *string, fontdef_t *font, const UINT8 *colormap);

fixed_t TSoURDt3rd_M_TimeFrac(tic_t tics, tic_t duration)
{
	return ((tics < duration) ? ((tics * FRACUNIT + rendertimefrac_unpaused) / duration) : FRACUNIT);
}

fixed_t TSoURDt3rd_M_ReverseTimeFrac(tic_t tics, tic_t duration)
{
	return (FRACUNIT - TSoURDt3rd_M_TimeFrac(duration - tics, duration));
}

fixed_t TSoURDt3rd_M_DueFrac(tic_t start, tic_t duration)
{
	tic_t t = I_GetTime();
	tic_t n = t - start;
	return TSoURDt3rd_M_TimeFrac(min(n, duration), duration);
}

UINT16 TSoURDt3rd_M_GetCvPlayerColor(UINT8 pnum)
{
	UINT8 main_player;
	UINT16 color;
	INT32 skin;

	main_player = (pnum == consoleplayer || pnum == 0);
	if (pnum >= MAXPLAYERS)
		return SKINCOLOR_NONE;

	color = (main_player ? cv_playercolor.value : cv_playercolor2.value);
	if (color != SKINCOLOR_NONE)
		return color;

	skin = R_SkinAvailable((main_player ? cv_skin.string : cv_skin2.string));
	if (skin == -1)
		return SKINCOLOR_NONE;

	return skins[skin]->prefcolor;
}

INT32 TSoURDt3rd_M_DrawCaretString(INT32 x, INT32 y, INT32 flags, fixed_t pscale, fixed_t vscale, const char *string, fontdef_t font)
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
// void TSoURDt3rd_M_ShowMusicCredits(void)
// Sets the easing variable needed in order to display music credits for specific cases.
//
void TSoURDt3rd_M_ShowMusicCredits(void)
{
	tsourdt3rd_songcredit_ease = TSoURDt3rd_M_DueFrac(I_GetTime(), 6);
}

//
// void TSoURDt3rd_M_PreDrawer(boolean *wipe_in_action)
// Draws graphics in the back, before all the main M_Drawer() graphics are rendered.
//
void TSoURDt3rd_M_PreDrawer(boolean *wipe_in_action)
{
#ifndef NO_MENU_WIPE
	// Reset vanilla's wipe!
	(*wipe_in_action) = (tsourdt3rd_currentMenu != NULL ? menuwipe : WipeInAction);
	if (menuwipe)
		F_WipeStartScreen();
#else
	(void)wipe_in_action;
#endif

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

		// Draw [TSoURDt3rd] version down in corner
		// ...but still only in the MAIN MENU.
		// It looks out of place anywhere else, and gets in the way.
		if (currentMenu == &MainDef)
		{
			V_DrawThinString(vid.dup, vid.height - 41*vid.dup, V_NOSCALESTART|V_TRANSLUCENT|V_ALLOWLOWERCASE, TSOURDT3RDVERSIONSTRING);
			V_DrawThinString(vid.dup, vid.height - 33*vid.dup, V_NOSCALESTART|V_TRANSLUCENT|V_ALLOWLOWERCASE, "By StarManiaKG!");
		}

		// Draw typing overlay when needed, above all other menu elements.
		if (menutyping.active)
			TSoURDt3rd_M_DrawMenuTyping();
	}

	// Draw message overlay when needed
	TSoURDt3rd_M_DrawMenuMessage();

#if 0
	// Draw song credits!
	if (Playing() && ((menuactive && !(menumessage.active && menutyping.active)) || paused))
	{
		fixed_t x = Easing_OutQuad(tsourdt3rd_songcredit_ease, -vid.width, 0);
		fixed_t y = (vid.height-(8*vid.dup)) * FRACUNIT;
		INT32 flags = V_SNAPTOLEFT; // V_SNAPTOBOTTOM
		INT32 string_flags = V_ALLOWLOWERCASE;

		//flags |= V_NOSCALESTART;
		string_flags |= V_NOSCALESTART;

		if (g_realsongcredit && !S_MusicDisabled())
		{
			char *credits = TSoURDt3rd_M_WriteVariedLengthString(g_realsongcredit, (vid.width/15), true);
			INT32 cred_length = V_ThinStringWidth(credits, flags|string_flags);

			INT32 bgt = (NUMTRANSMAPS/2);
			fixed_t bar_destx = (cred_length+7) * FRACUNIT;
			fixed_t bar_offset_width = FRACUNIT;

			if (cred_length >= songcreditbg->width-16)
			{
				// Hey, I heard this string looks pretty long!
				// Why don't we fix that?
				bar_offset_width = ((cred_length - (songcreditbg->width - 16)) * FRACUNIT/4);
			}

			V_DrawStretchyFixedPatch(bar_destx - bar_offset_width, y - (2 * FRACUNIT), bar_offset_width, FRACUNIT, flags|(bgt<<V_ALPHASHIFT), songcreditbg, NULL);
			V_DrawAlignedFontStringAtFixed(x + (2 * FRACUNIT), y, flags|string_flags, FRACUNIT, FRACUNIT, credits, tny_font, alignleft);
		}
	}
#endif

#ifndef NO_MENU_WIPE
	// Wipe the screen!
	if (menuwipe)
	{
		F_WipeEndScreen();
		F_RunWipe(wipedefs[tsourdt3rd_wipe_menu_final], false);
	}
#endif
	menuwipe = false;
}

//
// void TSoURDt3rd_M_DrawMenuTooltips(const tsourdt3rd_menu_t *menu, menutooltip_t menutooltip)
// Draws a banner across the top of the screen, with a description of the current option displayed.
//
// SOURCES:
//	M_DrawMenuTooltips() - from Dr.Robotnik's Ring Racers!
//	M_DrawSplitText() - from SRB2 Classic!
//

static char **M_SplitText(const char *txt, INT16 *lines_p)
{
	INT16 num_lines = 0;
	char **strings = NULL;
	char *icopy = strdup(txt);

	if (icopy == NULL) return NULL;

	char* tok = strtok(icopy, "\n");
	while (tok != NULL)
	{
		char *line = strdup(tok);
		if (line == NULL) return NULL;

		strings = realloc(strings, (num_lines + 1) * sizeof(char*));
		strings[num_lines] = line;
		num_lines++;

		tok = strtok(NULL, "\n");
	}

	free(icopy);
	if (lines_p) (*lines_p) = num_lines;
	return strings;
}

void TSoURDt3rd_M_DrawMenuTooltips(const tsourdt3rd_menu_t *menu, menutooltip_t menutooltip)
{
	static INT32 tooltip_alpha_timer = 9;
	INT32 tooltip_alpha_flag = 0;

	if (menu == NULL || menu->menuitems == NULL || (tsourdt3rd_itemOn < 0 || tsourdt3rd_itemOn >= currentMenu->numitems))
	{
		tooltip_alpha_timer = 9;
		return;
	}

	static const tsourdt3rd_menuitem_t *last_item = NULL;
	const tsourdt3rd_menuitem_t *item = &menu->menuitems[tsourdt3rd_itemOn];
	const char *string = NULL;
	char **tooltip_strings = NULL;
	INT16 num_lines = 0;

	if (item && item->tooltip != NULL)
	{
		// Use the menu tooltip!
		string = item->tooltip;
		//string = V_FontWordWrap(0, BASEVIDWIDTH, V_ALLOWLOWERCASE, FRACUNIT, item->tooltip, tny_font);
	}
	else
	{
		/// \todo: STAR NOTE: Do what classic does and add cvar tooltips, they're cool
		return;
	}
	if (string == NULL || !string[0])
	{
		// No valid string found!
		tooltip_alpha_timer = 9;
		return;
	}

	tooltip_strings = M_SplitText(string, &num_lines);
	if (tooltip_strings == NULL || num_lines == 0) // Failed to split the string.
	{
		tooltip_alpha_timer = 9;
		return;
	}

	// Draw the tooltip box and string
	const char *box_lump_name = (menutooltip.box.vflip ? "VMNUHINT" : "MENUHINT");
	patch_t *box_patch = W_CachePatchName(box_lump_name, PU_CACHE);

#if 1
	num_lines = 1;
#endif
	V_DrawStretchyFixedPatch(menutooltip.box.x, menutooltip.box.y,
		//menutooltip.box.pscale, menutooltip.box.vscale + (num_lines * FRACUNIT/6),
		menutooltip.box.pscale, menutooltip.box.vscale + ((num_lines-1) * FRACUNIT/6),
		menutooltip.box.flags,
		box_patch,
		menutooltip.box.colormap
	);

	if (last_item != item)
	{
		tooltip_alpha_timer = 9;
	}
	last_item = item;

#if 0
	INT16 yoffset;
	yoffset = (((5*10 - num_lines*10)));
	//INT16 yoffset = (((5*10 - (num_lines-1)*10)));
	//INT16 yoffset = (((5*10 - ((num_lines-1)*10))));
	//INT16 yoffset = 0;
	//INT16 yoffset = 8;
	for (int i = 0; i < num_lines; i++)
	{
		if (menutooltip.string.align != -1)
		{
			//V_DrawFontStringAtFixed
			//V_DrawAlignedFontString
			V_DrawCenteredThinString(menutooltip.string.x, menutooltip.string.y - yoffset,
				menutooltip.string.flags|V_MENUCOLORMAP,
				//FRACUNIT, FRACUNIT,
				tooltip_strings[i]
				//tny_font,
				//menutooltip.string.align
			);
			V_DrawCenteredThinString(menutooltip.string.x, menutooltip.string.y - yoffset,
				menutooltip.string.flags|tooltip_alpha_flag|((9 - tooltip_alpha_timer) << V_ALPHASHIFT),
				//FRACUNIT, FRACUNIT,
				tooltip_strings[i]
				//tny_font,
				//menutooltip.string.align
			);
		}
		else
		{
			//V_DrawFontString
			V_DrawThinString(menutooltip.string.x, menutooltip.string.y - yoffset,
				menutooltip.string.flags|V_MENUCOLORMAP,
				tooltip_strings[i]
			);
			V_DrawThinString(menutooltip.string.x, menutooltip.string.y - yoffset,
				menutooltip.string.flags|tooltip_alpha_flag|((9 - tooltip_alpha_timer) << V_ALPHASHIFT),
				tooltip_strings[i]
			);
		}
		yoffset += 10;
		free(tooltip_strings[i]); // Remember to free the memory for each line when you're done with it.
	}
	free(tooltip_strings);
#else
	if (menutooltip.string.align != -1)
	{
		V_DrawCenteredThinString(menutooltip.string.x, menutooltip.string.y,
			menutooltip.string.flags|V_MENUCOLORMAP,
			//FRACUNIT, FRACUNIT,
			string
			//tny_font,
			//menutooltip.string.align
		);
	}
	else
	{
		V_DrawThinString(menutooltip.string.x, menutooltip.string.y,
			menutooltip.string.flags|V_MENUCOLORMAP,
			//FRACUNIT, FRACUNIT,
			string
			//tny_font,
			//menutooltip.string.align
		);
	}
	(void)tooltip_strings;
	(void)tooltip_alpha_flag;
#endif

	// very cool alpha timer
	if (tooltip_alpha_timer > 0)
	{
		tooltip_alpha_timer--;
	}
}

INT32 K_DrawGameControl(fixed_t x, fixed_t y, UINT8 player, const char *str, UINT8 alignment, fontdef_t font, UINT32 flags)
{
	INT32 width = V_ThinStringWidth(str, flags);
	(void)alignment;
	if (player && !P_IsLocalPlayer(&players[player]))
	{
		return -1;
	}
	TSoURDt3rd_MK_DrawKeyboardString(x, y, flags, str, &font, NULL);
	return width;
}

void TSoURDt3rd_M_DrawMediocreKeyboardKey(const char *text, INT32 *workx, INT32 worky, boolean push, boolean rightaligned)
{
	INT32 buttonwidth = V_StringWidth(text, V_ALLOWLOWERCASE) + 2;

#if 1
	if (rightaligned)
	{
		(*workx) -= buttonwidth;
	}

#if 0
	if (push)
	{
		worky += 2;
	}
#else
	(void)push;
#endif
#if 0
	else
	{
		V_DrawFill((*workx)-1, worky+10, buttonwidth, 2, 24);
	}

	V_DrawFill((*workx)-1, worky, buttonwidth, 10, 16);
	V_DrawString((*workx), worky + 1, V_ALLOWLOWERCASE, text);
	return;
#endif
#endif

	K_DrawGameControl((*workx)<<FRACBITS, worky<<FRACBITS, 0, va("\xED %s \xED", text), 0, tny_font, V_ALLOWLOWERCASE);
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
			break;
	}
}

// ==========================================================================
// GENERIC GRAPHICS
// ==========================================================================

//
//
//
//

static void chain_patch(float x, float y, float width, float height, int stretch_flags, int flags, patch_t *patch, const UINT8 *colormap)
{
	width = (width <= 0.f ? 0.f : width);
	height = (height <= 0.f ? 0.f : height);
	stretch_flags = (stretch_flags == -1 ? Stretch_kNone : stretch_flags);

	float scale_ = 1.f;

	const bool stretchH = (stretch_flags == Stretch_kWidth || stretch_flags == Stretch_kBoth);
	const bool stretchV = (stretch_flags == Stretch_kHeight || stretch_flags == Stretch_kBoth);

	const fixed_t h = (stretchH ? FloatToFixed(width / patch->width) : FRACUNIT);
	const fixed_t v = (stretchV ? FloatToFixed(height / patch->height) : FRACUNIT);

	V_DrawStretchyFixedPatch(FloatToFixed(x), FloatToFixed(y), h * scale_, v * scale_, flags, patch, colormap);
}

static inline fixed_t CenteredCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	INT32 cxoff;
	/*
	For example, center a 4 wide patch to 8 width:
	4/2   = 2
	8/2   = 4
	4 - 2 = 2 (our offset)
	2 + 4 = 6 = 8 - 2 (equal space on either side)
	*/
	cxoff  = hchw -((*cwp) >> 1 );
	(*cwp) = chw;
	return FixedMul (( cxoff * dupx )<< FRACBITS, scale);
}

static inline fixed_t BunchedCharacterDim(
		fixed_t  scale,
		fixed_t   chw,
		INT32    hchw,
		INT32    dupx,
		fixed_t *  cwp)
{
	(void)chw;
	(void)hchw;
	(void)dupx;
	(*cwp) = FixedMul(max(1, (*cwp) - 1) << FRACBITS, scale);
	return 0;
}

static boolean V_CharacterValid(fontdef_t *font, int c)
{
	return (c >= 0 && c < FONTSIZE && font->chars[c] != NULL);
}

void TSoURDt3rd_MK_DrawKeyboardString(fixed_t x, fixed_t y, int flags, const char *string, fontdef_t *font, const UINT8 *colormap)
{
#define THING
	int c = 0;
	const char *ssave;

	INT32 boxedflags = ((flags) & (~V_HUDTRANS)) | (V_40TRANS);
	UINT8 boxed = 0;
	INT32 lowercase;

	INT32 hchw; /* half-width for centering */
	INT32 dup;

#ifndef THING
	fixed_t left;
	fixed_t right;
#else
	INT32 left = 0;
	INT32 right = 0;
#endif
	fixed_t bot;

	fixed_t cx, cy;
	fixed_t cxoff, cyoff;
	fixed_t cxsave;

	fixed_t scale = FRACUNIT;

	fixed_t cw;
	fixed_t f_chw = 0;

#ifdef THING
	INT32 w, new_dupx, dupy, scrwidth, center = 0;
	fixed_t pscale = FRACUNIT, vscale = FRACUNIT;

	INT32 charflags = (flags & V_CHARCOLORMASK);
	INT32 spacewidth = font->spacewidth, charwidth = 0;

	if (flags & V_NOSCALESTART)
	{
		new_dupx = vid.dup<<FRACBITS;
		dupy = vid.dup<<FRACBITS;
		scrwidth = vid.width;
	}
	else
	{
		new_dupx = pscale;
		dupy = vscale;
		scrwidth = FixedDiv(vid.width<<FRACBITS, vid.dup);
		left = (scrwidth - (BASEVIDWIDTH << FRACBITS))/2;
		scrwidth -= left;
	}
#endif

	lowercase = (flags & V_ALLOWLOWERCASE);
	flags &= ~(V_FLIP); /* These two (V_ALLOWLOWERCASE) share a bit. */

	/* Some of these flags get overloaded in this function so
	   don't pass them on. */
	flags &= ~(V_PARAMMASK);

#if 0
	hchw = fontspec.chw >> 1;
	fontspec.lfh    <<= FRACBITS;
#endif
	hchw = f_chw >> 1;

	hchw    <<= FRACBITS;
	fixed_t lfh = 12;
	lfh    <<= FRACBITS;

#define Mul( id, scale ) ( id = FixedMul (scale, id) )
	Mul (hchw, scale);
	Mul (lfh, scale);
#undef  Mul

	cx = cxsave = x;
	cy = y;
	cyoff = 0;
	ssave = string;

	if (( flags & V_NOSCALESTART ))
	{
		dup   = vid.dup;
		hchw *= dup;

		right = vid.width;
		lfh  *= dupy;
	}
	else
	{
		dup       = 1;

		right     = ( vid.width / vid.dup );
		if (!( flags & V_SNAPTOLEFT ))
		{
			left   = ( right - BASEVIDWIDTH )/ 2; /* left edge of drawable area */
			right -= left;
		}
	}

	right <<= FRACBITS;
	bot     = vid.height << FRACBITS;

	if (colormap == NULL)
	{
		colormap = V_GetStringColormap(( flags & V_CHARCOLORMASK ));
	}

	for (; ( c = *string ); ++string)
	{
		switch (c) // unused character parsing whateverness
		{
			case '\n':
			{
				if (boxed)
					continue;
				cy += lfh; // fontspec.lfh
				if (cy >= bot)
					return;
				cx  =   x;
				break;
			}
			case '\xEE': // Not lifted
			case '\xED': // animated
			case '\xEC': // pressed
			{
				UINT8 anim_duration = 16;
				UINT8 anim = 0;

				if (c == '\xEC')
					anim = 1;
				else if (c != '\xEE')
					anim = (((I_GetTime() % (anim_duration * 2)) < anim_duration) ? 1 : 0);

				// For bullshit text outlining reasons, we cannot draw this background character-by-character.
				// Thinking about doing string manipulation and calling out to V_StringWidth made me drink water.
				// So instead, we just draw this section of the string twice—invisibly the first time, to measure the width.

				if (boxed == 0) // Save our position and start no-op drawing
				{
					cy -= 2*FRACUNIT;

					//Draw(FixedToFloat(cx), FixedToFloat(cy)-3).flags(flags).patch(gen_button_keyleft[anim]);
					//V_DrawFixedPatch(cx, cy - 3*FRACUNIT, FRACUNIT, flags, gen_button_keyleft[anim], NULL);
					chain_patch(FixedToFloat(cx), FixedToFloat(cy)-3, -1.f, -1.f, -1, flags, gen_button_keyleft[anim], NULL);

					cx += 3*FRACUNIT;
					ssave = string;
					cxsave = cx;

					boxed = 1;
				}
				else if (boxed == 1) // Draw box from saved pos to current pos and roll back
				{
					//cx += (fontspec.right_outline)*FRACUNIT;

					cx += (2)*FRACUNIT;
					//cx += (1)*FRACUNIT;
					//cx += 0;

					fixed_t working = cxsave - 1*FRACUNIT;

#if 0
					Draw(FixedToFloat(working)+1, FixedToFloat(cy)-3)
						.width(FixedToFloat(cx - working)-1)
						.flags(flags)
						.stretch(Draw::Stretch::kWidth).patch(gen_button_keycenter[anim]);
					Draw(FixedToFloat(cx), FixedToFloat(cy)-3).flags(flags).patch(gen_button_keyright[anim]);
#endif
#if 0
					V_DrawStretchyFixedPatch(working + 1*FRACUNIT, cy - 3*FRACUNIT, (cx - working) - 1*FRACUNIT, FRACUNIT, flags, gen_button_keycenter[anim], NULL);
					V_DrawFixedPatch(cx, cy - 3*FRACUNIT, FRACUNIT, flags, gen_button_keyright[anim], NULL);
#endif
#if 1
					chain_patch(FixedToFloat(working)+1, FixedToFloat(cy)-3, FixedToFloat(cx - working)-1, -1, Stretch_kWidth, flags, gen_button_keycenter[anim], NULL);
					chain_patch(FixedToFloat(cx), FixedToFloat(cy)-3, -1.f, -1.f, -1, flags, gen_button_keyright[anim], NULL);
#endif

					string = ssave;
					cx = cxsave;

					// This is a little gross, but this is our way of smuggling text offset to
					// the standard character drawing case. boxed=3 means we're drawing a pressed button.
					boxed = 2 + anim;
				}
				else // Meeting the ending tag the second time, space away and resume standard parsing
				{
					boxed = 0;

					cx += (3)*FRACUNIT;
					cy += 2*FRACUNIT;
				}

				break;
			}
			default:
			{
#ifndef THING
				if (cx < right)
				{
					if (!lowercase)
					{
						c = toupper(c);
					}
					else if (V_CharacterValid(font, c - FONTSTART) == false)
					{
						// Try the other case if it doesn't exist
						if (c >= 'A' && c <= 'Z')
						{
							c = tolower(c);
						}
						else if (c >= 'a' && c <= 'z')
						{
							c = toupper(c);
						}
					}

					c -= FONTSTART;
					if (V_CharacterValid(font, c) == true)
					{
						// Remove offsets from patch
						fixed_t patchxofs = SHORT (font->chars[c]->leftoffset) * dup * scale;
						cw = SHORT (font->chars[c]->width) * dup;

						//cxoff = (*font.dim_fn)(scale, fontspec.chw, hchw, dup, &cw);
						//cxoff = CenteredCharacterDim(scale, f_chw, hchw, dup, &cw);
						cxoff = BunchedCharacterDim(scale, f_chw, hchw, dup, &cw);

						if (boxed != 1)
						{
							V_DrawFixedPatch(cx + cxoff + patchxofs, cy + cyoff + (boxed == 3 ? 2*FRACUNIT : 0), scale,
								boxed ? boxedflags : flags, font->chars[c], boxed ? 0 : colormap);
						}

						cx += cw;
					}
					else
						cx += font->spacewidth;
				}
#else
				if (cx < right)
				{
					//if (boxed != 1)
					{
						//if (V_CharacterValid(font, c) == false)
						(void)V_CharacterValid;
						if (!c)
							break;
						if (c & 0x80) // color parsing -x 2.16.09
						{
							// manually set flags override color codes
							if (!(flags & V_CHARCOLORMASK))
								charflags = ((c & 0x7f) << V_CHARCOLORSHIFT) & V_CHARCOLORMASK;
							continue;
						}
#if 0
						if (c == '\n')
						{
							cx = x;
							cy += FixedMul(((flags & V_RETURN8) ? 8 : font->linespacing)<<FRACBITS, dupy);
							continue;
						}#
#endif

						c = (lowercase ? c : toupper(c)) - FONTSTART;
						if (c < 0 || c >= FONTSIZE || !font->chars[c])
						{
							cx += FixedMul((spacewidth<<FRACBITS), dup);
							continue;
						}

						if (charwidth)
						{
							w = FixedMul((charwidth<<FRACBITS), new_dupx);
							center = w/2 - FixedMul(font->chars[c]->width<<FRACBITS, (new_dupx/2));
						}
						else
							w = FixedMul(font->chars[c]->width<<FRACBITS, new_dupx);

						if ((cx>>FRACBITS) > scrwidth)
							continue;
						if (cx+left + w < 0) // left boundary check
						{
							cx += w;
							continue;
						}

						if (boxed != 1)
						{
							//INT32 new_flags = boxed ? boxedflags : flags;
							INT32 new_flags = flags;

							//UINT8 *new_colormap = boxed ? 0 : V_GetStringColormap(charflags);
							UINT8 *new_colormap = V_GetStringColormap(charflags);

							fixed_t boxed_offset = (boxed == 3 ? 2*FRACUNIT : 0);

							boxed_offset += FRACUNIT;

							chain_patch(FixedToFloat(cx + center), FixedToFloat(cy + cyoff + boxed_offset), FixedToFloat(pscale), FixedToFloat(vscale), -1.f, new_flags, font->chars[c], new_colormap);
							//V_DrawStretchyFixedPatch(cx + center, cy + cyoff + boxed_offset, pscale, vscale, new_flags, font->chars[c], new_colormap);

#if 0
							cw = SHORT (font->chars[c]->width) * new_dupx;
							cxoff = BunchedCharacterDim(scale, f_chw, hchw, new_dupx, &cw);
							//cxoff = CenteredCharacterDim(scale, f_chw, hchw, new_dupx, &cw);
							fixed_t patchxofs = SHORT (font->chars[c]->leftoffset) * new_dupx * scale;
							fixed_t new_x = cx + cxoff + patchxofs;
							fixed_t new_y =  cy + cyoff + boxed_offset;
							V_DrawFixedPatch(new_x, new_y, scale, new_flags, font->chars[c], new_colormap);
#endif

#if 0
							//Draw bt = Draw(
								//	FixedToFloat(cx + cxoff) - (bt_inst->x * dupx),
								//	FixedToFloat(cy + cyoff) - ((bt_inst->y + fontspec.button_yofs) * dupy))
								//	.flags(flags);
							chain_patch(FixedToFloat(cx + cxoff) - (2 * new_dupx), // bt_inst->x replaces 2 eventually
								FixedToFloat(cy + cyoff) - ((4 + 2) * dupy),
								-1.f, -1.f,
							Stretch_kNone,
							flags,
							) // bt_inst->y replaces 4 eventually; fontspec.button_yofs replaces 2
#endif
						}

						cx += w + (font->kerning<<FRACBITS);
					}
				}
#endif
				break;
			}
		}
	}

	(void)boxedflags;
	(void)cw;
	(void)cxoff;
	(void)cyoff;
}

//
// void TSoURDt3rd_MK_DrawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed)
// Draws a button graphic on the screen. Changes upon being pressed.
//
// SOURCES:
// 	- Dr. Robotnik's Ring Racers!
//
void TSoURDt3rd_MK_DrawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed)
{
	V_DrawFixedPatch(x, y, FRACUNIT, flags, button[(pressed == true) ? 1 : 0], NULL);
}

//
// void TSoURDt3rd_MK_DrawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic)
// Draws a button graphic on the screen, and animates it. Changes upon being pressed.
//
// SOURCES:
// 	- Dr. Robotnik's Ring Racers!
//
void TSoURDt3rd_MK_DrawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic)
{
	const UINT8 anim_duration = 16;
	const boolean anim = ((animtic % (anim_duration * 2)) < anim_duration);
	TSoURDt3rd_MK_DrawButton(x << FRACBITS, y << FRACBITS, flags, button, anim);
}

//
// void TSoURDt3rd_MK_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall)
// Draws a sticker graphic on the HUD.
//
// SOURCES:
// 	- Dr. Robotnik's Ring Racers!
//
void TSoURDt3rd_MK_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall)
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

// ==========================================================================
// GENERIC MENUS
// ==========================================================================

//
// void TSoURDt3rd_M_DrawGenericOptions(void)
//
// Text-based list menu, seperating various options by using boxes.
// Used for TSoURDt3rd's various unique options.
//
// SOURCES:
// 	- Dr. Robotnik's Ring Racers!
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

	if (tsourdt3rd_currentMenu == &TSoURDt3rd_TM_OP_MainMenuDef || tsourdt3rd_currentMenu->prev_menu == &TSoURDt3rd_TM_OP_MainMenuDef)
		s = TSoURDt3rd_OP_MainMenuDef.menuitems[TSoURDt3rd_OP_MainMenuDef.lastOn].text;
	else if (currentMenu->prevMenu)
		s = currentMenu->prevMenu->menuitems[currentMenu->prevMenu->lastOn].text;

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

	// Tooltip
	// Draw it at the top of the screen
	{
		menutooltip_t menutooltip = {
			{
				0, 0,
				vid.width*FRACUNIT, FRACUNIT,
				V_SNAPTOLEFT|V_SNAPTOTOP,
				NULL,
				false
			},
			{
				BASEVIDWIDTH/2, 13,
				V_SNAPTOTOP|V_ALLOWLOWERCASE,
				aligncenter
			}
		};
		TSoURDt3rd_M_DrawMenuTooltips(tsourdt3rd_currentMenu, menutooltip);
	}
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
