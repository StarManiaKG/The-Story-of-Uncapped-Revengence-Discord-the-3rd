// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/transient/smkg-options-tsourdt3rd-extras-jukebox.c
/// \brief TSoURDt3rd's full interactive Jukebox menu

#include "../../smkg-m_sys.h"
#include "../../../core/smkg-s_jukebox.h"
#include "../../../core/smkg-s_audio.h"

#include "../../../../g_demo.h"
#include "../../../../r_draw.h"
#include "../../../../r_main.h"
#include "../../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

static INT32 skyRoomMenu_ul = 0;
static UINT8 tsourdt3rd_skyRoomMenuTranslations[MAXUNLOCKABLES];

static boolean jb_draw_options = false;
static boolean jb_handle_sfx = false;

static boolean jb_draw_controls_tip = false;
static tic_t jb_controls_ticker = 0;
static INT32 jb_controls_trans = 0;
static INT32 jb_controls_trans_table[] = {
	[30*TICRATE] = V_10TRANS,
	[31*TICRATE] = V_20TRANS,
	[32*TICRATE] = V_30TRANS,
	[33*TICRATE] = V_40TRANS,
	[34*TICRATE] = V_TRANSLUCENT,
	[35*TICRATE] = V_60TRANS,
	[36*TICRATE] = V_70TRANS,
	[37*TICRATE] = V_80TRANS,
	[38*TICRATE] = V_90TRANS,
	[39*TICRATE] = -1,
};

static tsourdt3rd_jukebox_pages_t *jb_page = NULL;
static tsourdt3rd_jukeboxdef_t **tsourdt3rd_cur_page_jukebox_defs = NULL;
static INT32 jb_page_playable_tracks = 0;
static INT32 jb_sel = 0;
static INT32 jb_cc = 0;
static patch_t *jb_radio[9];
static patch_t *jb_launchpad[4];
static UINT8 jb_frame[4] = {0, 0, -1, SKINCOLOR_RUBY};

static fixed_t jb_work, jb_bpm;
static angle_t jb_ang;
static fixed_t jb_hscale = FRACUNIT/2;
static fixed_t jb_vscale = FRACUNIT/2;
static fixed_t jb_bounce = 0;

static void M_Sys_DrawJukebox(void);
static void M_Sys_InitJukebox(void);
static boolean M_Sys_HandleJukebox(INT32 choice);

menuitem_t TSoURDt3rd_OP_Extras_JukeboxMenu[] =
{
	{IT_STRING | IT_CVAR, NULL, "Jukebox HUD",
		&cv_tsourdt3rd_jukebox_hud, 0},

	{IT_SPACE, NULL, NULL,
		NULL, 0},

	{IT_STRING | IT_CVAR, NULL, "Current Music Speed",
		&cv_tsourdt3rd_jukebox_speed, 0},
	{IT_STRING | IT_CVAR, NULL, "Current Music Pitch",
		&cv_tsourdt3rd_jukebox_pitch, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_JukeboxMenu[] =
{
	{NULL, "Should a HUD appear whenever you're using the Jukebox?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, "Increase or decrease the speed of the current Jukebox track.", {NULL}, 0, 0},
	{NULL, "Increase or decrease the pitch of the current Jukebox track.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_Extras_JukeboxDef =
{
	MTREE4(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_JUKEBOX, MN_OP_TSOURDT3RD_JUKEBOX),
	NULL,
	sizeof (TSoURDt3rd_OP_Extras_JukeboxMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_ExtrasDef,
	TSoURDt3rd_OP_Extras_JukeboxMenu,
	M_Sys_DrawJukebox,
	48, 72,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Extras_JukeboxDef = {
	TSoURDt3rd_TM_OP_Extras_JukeboxMenu,
	0, 0,
	0,
	NULL,
	0, 0,
	NULL,
	NULL,
	M_Sys_InitJukebox,
	TSoURDt3rd_M_Jukebox_Quit,
	M_Sys_HandleJukebox,
	&TSoURDt3rd_TM_OP_ExtrasDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_UpdateUnlocks(UINT8 *unlockables_table)
{
	INT32 i, j, cur_ul;
	UINT8 done[MAXUNLOCKABLES];
	UINT16 curheight;

	memset(done, 0, sizeof(done));

	for (i = 1; i <= MAXUNLOCKABLES; ++i)
	{
		curheight = UINT16_MAX;
		cur_ul = -1;

		// Autosort unlockables
		for (j = 0; j < MAXUNLOCKABLES; ++j)
		{
			if (!unlockables[j].height || done[j] || unlockables[j].type < 0)
				continue;

			if (unlockables[j].height < curheight)
			{
				curheight = unlockables[j].height;
				cur_ul = j;
			}
		}
		if (cur_ul < 0)
			break;

		done[cur_ul] = true;
		unlockables_table[i-1] = (UINT8)cur_ul;
	}
}

static void M_Sys_CacheJukebox(void)
{
	UINT8 i;
	char buf[8];

	STRBUFCPY(buf, "M_RADIOn");
	for (i = 0; i < 9; i++)
	{
		buf[7] = (char)('0'+i);
		jb_radio[i] = W_CachePatchName(buf, PU_PATCH);
	}

	STRBUFCPY(buf, "M_LPADn");
	for (i = 0; i < 4; i++)
	{
		buf[6] = (char)('0'+i);
		jb_launchpad[i] = W_CachePatchName(buf, PU_PATCH);
	}
}

static boolean M_Sys_TrackIsOnPage(tsourdt3rd_jukeboxdef_t *jukedef)
{
	tsourdt3rd_jukebox_pages_t *cur_juke_page = NULL;

	if (jukedef == NULL || jukedef->supported_pages == NULL)
		return false;
	if (jb_page == NULL)
		return false;
	cur_juke_page = jukedef->supported_pages;

	while (cur_juke_page)
	{
		if (!stricmp(cur_juke_page->page_name, jb_page->page_name)) return true;
		cur_juke_page = cur_juke_page->next;
	}
	return false;
}

static boolean M_Sys_FindNewJukeboxTrack(INT32 choice)
{
	if (jb_page_playable_tracks <= 1) // sound test counts as one track
		return false;

	switch (choice)
	{
		case KEY_PGDN: // page down
		{
			if (jb_sel >= jb_page_playable_tracks-1)
				break;

			jb_sel += 3;
			if (jb_sel >= jb_page_playable_tracks-1)
				jb_sel = jb_page_playable_tracks-1;

			break;
		}
		case KEY_PGUP: // page up
		{
			// page up
			if (!jb_sel)
				break;

			jb_sel -= 3;
			if (jb_sel < 0)
				jb_sel = 0;

			break;
		}
		default:
		{
			if (choice > 0)
			{
				// down
				if (jb_sel++ >= jb_page_playable_tracks-1)
					jb_sel = 0;
			}
			else
			{
				// up
				if (!jb_sel--)
					jb_sel = jb_page_playable_tracks-1;
			}
			break;
		}
	}
	return true;
}

static boolean M_Sys_SetupNewJukeboxPage(void)
{
	tsourdt3rd_jukeboxdef_t *jukedef = NULL;
	INT32 cur_juke_def_pos;

	jb_page_playable_tracks = 0;

	if (tsourdt3rd_cur_page_jukebox_defs)
	{
		Z_Free(tsourdt3rd_cur_page_jukebox_defs);
		tsourdt3rd_cur_page_jukebox_defs = NULL;
	}

	if (!(tsourdt3rd_cur_page_jukebox_defs = Z_Malloc(numsoundtestdefs * sizeof(tsourdt3rd_jukeboxdef_t *), PU_STATIC, NULL)))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "M_Sys_SetupNewJukeboxPage(): could not allocate jukebox defs.\n");
		return false;
	}

	for (cur_juke_def_pos = 0; cur_juke_def_pos < numsoundtestdefs; cur_juke_def_pos++)
	{
		jukedef = tsourdt3rd_jukebox_defs[cur_juke_def_pos];
		if (!M_Sys_TrackIsOnPage(jukedef))
			continue;
		tsourdt3rd_cur_page_jukebox_defs[jb_page_playable_tracks++] = jukedef;
	}
	return (jb_page_playable_tracks > 1);
}

static boolean M_Sys_FindNewJukeboxPage(boolean decrease)
{
	size_t all_juke_pages;

	if (tsourdt3rd_jukebox_available_pages == NULL)
	{
		// Luckily, a fallback exists!
		jb_page = &tsourdt3rd_jukeboxpage_mainpage;
		return false;
	}
	else if (tsourdt3rd_jukebox_numpages <= 0)
	{
		// We don't really *need* to find another page.
		return false;
	}

	all_juke_pages = sizeof(tsourdt3rd_jukebox_available_pages)/sizeof(tsourdt3rd_jukebox_pages_t);
	if (decrease)
	{
		if (jb_page->prev == NULL)
			jb_page = tsourdt3rd_jukebox_available_pages[all_juke_pages];
		else
			jb_page = jb_page->prev;
	}
	else
	{
		if (jb_page->next == NULL)
			jb_page = tsourdt3rd_jukeboxpages_start;
		else
			jb_page = jb_page->next;
	}

	jb_sel = 0;
	return (M_Sys_SetupNewJukeboxPage());
}

static void M_Sys_DrawJukebox(void)
{
	INT32 x, y, i;

	if (jb_draw_options)
	{
		V_DrawThinString(120, 164, V_YELLOWMAP, "Menu Controls");
		V_DrawThinString(4, 176, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Up/Up: Scroll Up");
		V_DrawThinString(4, 184, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Down/Down: Scroll Down");
		V_DrawThinString(164, 176, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Left/Right/Sideways: Change Page");
		V_DrawThinString(164, 184, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Tab: Modify Music Options");
		TSoURDt3rd_M_DrawGenericOptions();
		return;
	}

	x = 90<<FRACBITS;
	y = (BASEVIDHEIGHT-32)<<FRACBITS;

	V_DrawStretchyFixedPatch(x, y,
		jb_hscale, jb_vscale,
		0, jb_radio[jb_frame[0]], NULL);

	V_DrawFixedPatch(x, y, FRACUNIT/2, 0, jb_launchpad[0], NULL);

	for (i = 0; i < 9; i++)
	{
		if (i == jb_frame[2])
		{
			UINT8 *colmap = R_GetTranslationColormap(TC_RAINBOW, jb_frame[3], GTC_CACHE);
			V_DrawFixedPatch(x, y + (jb_frame[1]<<FRACBITS), FRACUNIT/2, 0, jb_launchpad[jb_frame[1]+1], colmap);
		}
		else
			V_DrawFixedPatch(x, y, FRACUNIT/2, 0, jb_launchpad[1], NULL);

		if ((i % 3) == 2)
		{
			x -= ((2*28) + 25)<<(FRACBITS-1);
			y -= ((2*7) - 11)<<(FRACBITS-1);
		}
		else
		{
			x += 28<<(FRACBITS-1);
			y += 7<<(FRACBITS-1);
		}
	}

	y = (BASEVIDWIDTH-(vid.width/vid.dup))/2;

	V_DrawFill(y, 20, (vid.width/vid.dup)+12, 24, 159);
	{
		static fixed_t jb_scroll = -FRACUNIT;
		const char* titl;

		x = 16;
		V_DrawString(x, 10, 0, "NOW PLAYING:");

		if (jb_handle_sfx)
			titl = va("%s - ", soundtestsfx.title);
		else if (TSoURDt3rd_Jukebox_IsPlaying())
		{
			if (tsourdt3rd_global_jukebox->curtrack->alttitle[0])
				titl = va("%s - %s - ", tsourdt3rd_global_jukebox->curtrack->title, tsourdt3rd_global_jukebox->curtrack->alttitle);
			else
				titl = va("%s - ", tsourdt3rd_global_jukebox->curtrack->title);
			V_DrawRightAlignedThinString(BASEVIDWIDTH-16, 46, V_ALLOWLOWERCASE, tsourdt3rd_global_jukebox->curtrack->authors);
		}
		else
			titl = "None - ";

		i = V_LevelNameWidth(titl);

		jb_scroll += renderdeltatics;

		while (jb_scroll >= (i << FRACBITS))
			jb_scroll -= i << FRACBITS;

		x -= jb_scroll >> FRACBITS;

		while (x < BASEVIDWIDTH-y)
			x += i;
		while (x > y)
		{
			x -= i;
			V_DrawLevelTitle(x, 22, 0, titl);
		}
	}

	V_DrawFill(165, 60, 140+15, 112, 159);

	{
		INT32 t, b, q, m = 112;

		INT32 saved_scrollbar_position;

		if (jb_page_playable_tracks <= 7)
		{
			t = 0;
			b = jb_page_playable_tracks - 1;
			i = 0;
		}
		else
		{
			q = m;
			m = (5*m)/jb_page_playable_tracks;
			if (jb_sel < 3)
			{
				t = 0;
				b = 6;
				i = 0;
			}
			else if (jb_sel >= jb_page_playable_tracks-4)
			{
				t = jb_page_playable_tracks - 7;
				b = jb_page_playable_tracks - 1;
				i = q-m;
			}
			else
			{
				t = jb_sel - 3;
				b = jb_sel + 3;
				i = (t * (q-m))/(jb_page_playable_tracks - 7);
			}
		}

		saved_scrollbar_position = i;

		if (t != 0)
			V_DrawString(165+140+4, 60+4 - (tsourdt3rd_skullAnimCounter/5), V_MENUCOLORMAP, "\x1A");

		if (b != jb_page_playable_tracks - 1)
			V_DrawString(165+140+4, 60+112-12 + (tsourdt3rd_skullAnimCounter/5), V_MENUCOLORMAP, "\x1B");

		x = 169;
		y = 64;

		while (t <= b)
		{
			tsourdt3rd_jukeboxdef_t *cur_juke_def = tsourdt3rd_cur_page_jukebox_defs[t];

			if (t == jb_sel)
				V_DrawFill(165, y-4, 140-1+16, 16, 155);

			if (!cur_juke_def->linked_musicdef->allowed)
			{
				V_DrawString(x, y, (t == jb_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, "???");
			}
			else if (jb_handle_sfx && cur_juke_def == &jukebox_def_soundtestsfx)
			{
				const char *sfxstr = va("SFX %s", cv_soundtest.string);

				V_DrawString(x+10, y, (t == jb_sel ? V_MENUCOLORMAP : 0), sfxstr);
				if (t == jb_sel)
				{
					V_DrawCharacter(x - 10 - (tsourdt3rd_skullAnimCounter/5) + 10, y,
						'\x1C' | V_MENUCOLORMAP, false);
					V_DrawCharacter(x + 2 + V_StringWidth(sfxstr, 0) + (tsourdt3rd_skullAnimCounter/5) + 10, y,
						'\x1D' | V_MENUCOLORMAP, false);
				}

				if (tsourdt3rd_global_jukebox->curtrack == cur_juke_def->linked_musicdef)
				{
					sfxstr = (cv_soundtest.value ? S_sfx[cv_soundtest.value].name : "N/A");
					i = V_StringWidth(sfxstr, 0);

					V_DrawFill(165+140-9-i+16, y-4, i+8, 16, 150);
					V_DrawRightAlignedString(165+140-5+16, y, V_MENUCOLORMAP, sfxstr);
				}
			}
			else
			{
				if (strlen(cur_juke_def->linked_musicdef->title) < 18)
					V_DrawString(x, y, (t == jb_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, cur_juke_def->linked_musicdef->title);
				else
					V_DrawThinString(x, y, (t == jb_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, cur_juke_def->linked_musicdef->title);

				if (tsourdt3rd_global_jukebox->curtrack == cur_juke_def->linked_musicdef)
				{
					V_DrawFill(165-8, y-4, 8, 16, 150);
					//V_DrawCharacter(165+140-8, y, '\x19' | V_MENUCOLORMAP, false);
					V_DrawFixedPatch((165-8)<<FRACBITS, (y<<FRACBITS)-(jb_bounce*4), FRACUNIT, 0, hu_font.chars['\x19'-FONTSTART], V_GetStringColormap(V_MENUCOLORMAP));
				}
			}
			t++;
			y += 16;
		}

		V_DrawFill(165+140-1+15, 60 + saved_scrollbar_position, 1, m, 0); // little scroll bar

		// Pages
		if (tsourdt3rd_jukebox_numpages)
		{
			const char *page_str = va("%cPage %d/%d:\x80 %c\"%s\"\x80", ('\x80' + (V_MENUCOLORMAP>>V_CHARCOLORSHIFT)), jb_page->id, tsourdt3rd_jukebox_numpages, ('\x80' + (tsourdt3rd_highlightflags>>V_CHARCOLORSHIFT)), jb_page->page_name);
			V_DrawRightAlignedThinString(BASEVIDWIDTH - 50, 3, V_ALLOWLOWERCASE, page_str);
			V_DrawCharacter(BASEVIDWIDTH - 50 - V_ThinStringWidth(page_str, V_ALLOWLOWERCASE) - 10 + (tsourdt3rd_skullAnimCounter/5), 3,
				'\x1C' | V_MENUCOLORMAP, false);
			V_DrawCharacter(BASEVIDWIDTH - 50 + 4 - (tsourdt3rd_skullAnimCounter/5), 3,
				'\x1D' | V_MENUCOLORMAP, false);
		}
		else
			V_DrawRightAlignedThinString(BASEVIDWIDTH-50, 3, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Main Page");

		if (jb_page_playable_tracks <= 1)
		{
			V_DrawRightAlignedThinString(BASEVIDWIDTH-50, 3+8, V_REDMAP|V_ALLOWLOWERCASE, "No playable tracks found on page!");
			V_DrawThinString(165+20, 105, V_REDMAP, "No playable tracks found!");
		}
		else
			V_DrawRightAlignedThinString(BASEVIDWIDTH-50, 3+8, V_YELLOWMAP|V_ALLOWLOWERCASE, va("%d track(s) found!", jb_page_playable_tracks-1));

		// Controls
		if (jb_draw_controls_tip & (jb_controls_trans_table[jb_controls_ticker] > -1))
		{
			INT32 box_height = (jb_handle_sfx ? 35 : 55);

			if (jb_controls_trans_table[jb_controls_ticker])
				jb_controls_trans = jb_controls_trans_table[jb_controls_ticker];

			V_DrawFill(165-162, 60+28, 159, box_height, 0|jb_controls_trans);
			V_DrawThinString(165-106, 60+33, jb_controls_trans|V_YELLOWMAP, "CONTROLS");

			if (jb_handle_sfx == false)
			{
				V_DrawThinString(165-159, 60+45, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Up/Up: Scroll Up");
				V_DrawThinString(165-159, 60+53, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Down/Down: Scroll Down");
				V_DrawThinString(165-159, 60+61, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Left/Right/Sideways: Change Page");
				V_DrawThinString(165-159, 60+69, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Tab: Modify Music Options");
			}
			else
			{
				V_DrawThinString(165-159, 60+45, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Left/Right: Change SFX");
				V_DrawThinString(165-159, 60+53, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Esc/Spin: Exit Sound Test");
			}

			jb_controls_ticker++;
		}
		else
		{
			jb_draw_controls_tip = false;
			jb_controls_trans = 0;
			jb_controls_ticker = 0;
		}
	}
}

static void M_Sys_InitJukebox(void)
{
	soundtestpage = (UINT8)(unlockables[skyRoomMenu_ul].variable);
	if (!soundtestpage)
		soundtestpage = 1;

	if (jb_page == NULL)
		jb_page = tsourdt3rd_jukeboxpages_start;

	if (!TSoURDt3rd_Jukebox_Unlocked())
	{
		TSoURDt3rd_M_StartMessage(
			"TSoURDt3rd Jukebox",
			M_GetText("You haven't unlocked this yet!\nGo and unlock the sound test first!\n"),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		return;
	}
	else if (!TSoURDt3rd_Jukebox_PrepareDefs())
	{
		TSoURDt3rd_M_StartMessage(
			"TSoURDt3rd Jukebox",
			M_GetText("No selectable tracks found.\n"),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		return;
	}
	else if (!M_Sys_SetupNewJukeboxPage())
	{
		TSoURDt3rd_M_StartMessage(
			"TSoURDt3rd Jukebox",
			M_GetText("No jukebox pages found.\n"),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		return;
	}

	M_Sys_CacheJukebox();

	jb_cc = cv_closedcaptioning.value; // hack;
	cv_closedcaptioning.value = 1; // hack

	TSoURDt3rd_M_ResetOptions();
}

static boolean M_Sys_HandleJukebox(INT32 choice)
{
	const UINT8 pid = 0;
	tsourdt3rd_jukeboxdef_t *cur_juke_def = tsourdt3rd_cur_page_jukebox_defs[jb_sel];

	if (jb_draw_options)
	{
		if (TSoURDt3rd_M_MenuBackPressed(pid))
		{
			jb_draw_options = false;
			TSoURDt3rd_M_SetMenuDelay(pid);
			return true;
		}
		return false;
	}

	if (jb_handle_sfx)
	{
		if (menucmd[pid].dpad_lr < 0 || menucmd[pid].dpad_lr > 0) // left & right
		{
			tsourdt3rd_global_jukebox->track_tics = 0;
			S_StopSounds();
			S_StopMusic();
			CV_AddValue(&cv_soundtest, ((menucmd[pid].dpad_lr > 0) ? 1 : -1)); // right, or left otherwise
			TSoURDt3rd_M_SetMenuDelay(pid);
		}
		else if (TSoURDt3rd_M_MenuConfirmPressed(pid))
		{
			// S_StopMusic() -- is this necessary?
			if (cv_soundtest.value)
				S_StartSound(NULL, cv_soundtest.value);
			tsourdt3rd_global_jukebox->track_tics = 0;
		}
		else if (TSoURDt3rd_M_MenuBackPressed(pid))
		{
			jb_handle_sfx = false;
			jb_draw_controls_tip = false;
			S_StopSounds();
			S_StopMusic();
			TSoURDt3rd_M_SetMenuDelay(pid);
		}
		return true;
	}

	switch (choice)
	{
		case KEY_PGUP:
		case KEY_PGDN:
			M_Sys_FindNewJukeboxTrack(choice);
			cv_closedcaptioning.value = jb_cc; // hack
			S_StartSound(NULL, sfx_menu1);
			cv_closedcaptioning.value = 1; // hack
			return true;
		case KEY_TAB:
			jb_draw_options = true;
			return true;
		default:
			break;
	}

	if (menucmd[pid].dpad_ud > 0 || menucmd[pid].dpad_ud < 0) // down & up
	{
		{
			cv_closedcaptioning.value = jb_cc; // hack
			if (M_Sys_FindNewJukeboxTrack(menucmd[pid].dpad_ud)) // up = -1; +1 otherwise
				S_StartSound(NULL, sfx_menu1);
			else
				S_StartSound(NULL, sfx_adderr);
			cv_closedcaptioning.value = 1; // hack
		}
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (menucmd[pid].dpad_lr < 0 || menucmd[pid].dpad_lr > 0) // left & right
	{
		if (cur_juke_def->linked_musicdef->allowed)
		{
			cv_closedcaptioning.value = jb_cc; // hack
			if (M_Sys_FindNewJukeboxPage(menucmd[pid].dpad_lr < 0)) // left = -1; +1 otherwise
				S_StartSound(NULL, sfx_menu1);
			else
				S_StartSound(NULL, sfx_adderr);
			cv_closedcaptioning.value = 1; // hack
		}
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (TSoURDt3rd_M_MenuExtraPressed(pid))
	{
		if (!TSoURDt3rd_Jukebox_IsPlaying())
		{
			S_StartSound(NULL, sfx_lose);
			return true;
		}

		S_StopSounds();
		S_StopMusic();

		cv_closedcaptioning.value = jb_cc; // hack
		S_StartSound(NULL, sfx_skid);
		cv_closedcaptioning.value = 1; // hack

		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (TSoURDt3rd_M_MenuConfirmPressed(pid))
	{
		S_StopSounds();
		S_StopMusic();

		TSoURDt3rd_M_SetMenuDelay(pid);

		if (!cur_juke_def->linked_musicdef->allowed)
		{
			S_StartSound(NULL, sfx_lose);
			TSoURDt3rd_S_RefreshMusic();
			return true;
		}

		if (cur_juke_def == &jukebox_def_soundtestsfx)
		{
			jb_controls_trans = 0;
			jb_controls_ticker = 0;
			jb_draw_controls_tip = true;
			jb_handle_sfx = true;
			return true;
		}

		TSoURDt3rd_Jukebox_Play(cur_juke_def->linked_musicdef);
		return true;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_prevMenu, vanilla_prevMenu, false);
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}

	return false;
}

void TSoURDt3rd_M_Jukebox_Init(INT32 choice)
{
	if (tsourdt3rd_global_jukebox == NULL)
	{
		// Jukebox definition thing is NULL, so don't go any further.
		TSoURDt3rd_M_StartMessage(
			"TSoURDt3rd Jukebox",
			M_GetText("The data needed for the jukebox wasn't initialized.\n"),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		S_StartSound(NULL, sfx_lose);
		return;
	}
	else if (currentMenu == &TSoURDt3rd_OP_Extras_JukeboxDef)
	{
		// Please don't go to the same menu twice.
		return;
	}
	else if (TSoURDt3rd_M_DoesMenuHaveKeyHandler() > 0)
	{
		// Let's not be doing or reading something potentially important before we access the Jukebox, mmk?
		return;
	}

	memset(tsourdt3rd_skyRoomMenuTranslations, 0, sizeof(tsourdt3rd_skyRoomMenuTranslations));
	M_Sys_UpdateUnlocks(tsourdt3rd_skyRoomMenuTranslations);
	skyRoomMenu_ul = tsourdt3rd_skyRoomMenuTranslations[choice-1];

	// When using the jukebox keybind, this prevents the game from crashing!
	if (tsourdt3rd_prevMenu != NULL)
		TSoURDt3rd_TM_OP_Extras_JukeboxDef.music = tsourdt3rd_prevMenu->music;

	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_Extras_JukeboxDef, &TSoURDt3rd_OP_Extras_JukeboxDef, false);
	if (vanilla_prevMenu == &TSoURDt3rd_OP_Extras_JukeboxDef)
		TSoURDt3rd_OP_Extras_JukeboxDef.lastOn = 0;

	optionsmenu.ticker = 0;
	TSoURDt3rd_M_OptionsTick();

	tsourdt3rd_global_jukebox->in_menu = true;
}

void TSoURDt3rd_M_Jukebox_Ticker(void)
{
	const UINT8 frame[4] = {0, 0, -1, SKINCOLOR_RUBY};
	fixed_t stoppingtics;

	memcpy(jb_frame, frame, sizeof(frame));
	jb_hscale = FRACUNIT/2;
	jb_vscale = FRACUNIT/2;
	jb_bounce = 0;

	TSoURDt3rd_OP_Extras_JukeboxMenu[op_music_speed].status = ((S_GetSpeedMusic() > 0.0f) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	TSoURDt3rd_OP_Extras_JukeboxMenu[op_music_pitch].status = ((S_GetPitchMusic() > 0.0f) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	TSoURDt3rd_M_OptionsTick();

	if (jb_handle_sfx)
	{
		if (cv_soundtest.value)
		{
			jb_frame[1] = (2 - (tsourdt3rd_global_jukebox->track_tics >> FRACBITS));
			jb_frame[2] = ((cv_soundtest.value - 1) % 9);
			jb_frame[3] += (((cv_soundtest.value - 1) / 9) % (FIRSTSUPERCOLOR - jb_frame[3]));
			if (tsourdt3rd_global_jukebox->track_tics < (2 << FRACBITS))
				tsourdt3rd_global_jukebox->track_tics += renderdeltatics;
			if (tsourdt3rd_global_jukebox->track_tics >= (2 << FRACBITS))
				tsourdt3rd_global_jukebox->track_tics = 2 << FRACBITS;
		}
		return;
	}
	else if (!TSoURDt3rd_Jukebox_IsPlaying())
	{
		// NOOOOO.... MORRRREEEEEEE!
		if (!menuactive || currentMenu == NULL || currentMenu != &TSoURDt3rd_OP_Extras_JukeboxDef)
			jb_draw_controls_tip = true;
		return;
	}

	stoppingtics = (fixed_t)(tsourdt3rd_global_jukebox->curtrack->stoppingtics) << FRACBITS;
	jb_frame[0] = 8-(jb_work/(20<<FRACBITS));
	if (jb_frame[0] > 8) // VERY small likelihood for the above calculation to wrap, but it turns out it IS possible lmao
		jb_frame[0] = 0;

	jb_ang = (FixedAngle(jb_work)>>ANGLETOFINESHIFT) & FINEMASK;
	jb_bounce = (FINESINE(jb_ang) - FRACUNIT/2);
	jb_hscale -= jb_bounce/16;
	jb_vscale += jb_bounce/16;

	if (stoppingtics && (tsourdt3rd_global_jukebox->track_tics >= stoppingtics))
	{
		// Whoa, whoa, we've run out of time!
		TSoURDt3rd_Jukebox_Reset();
		TSoURDt3rd_S_RefreshMusic();
		return;
	}

	jb_work = jb_bpm = tsourdt3rd_global_jukebox->curtrack->bpm / ((S_GetSpeedMusic() > 0.0f) ? S_GetSpeedMusic() : 1.0f);
	//jb_bpm = FixedDiv((60*TICRATE)<<FRACBITS, jb_bpm); -- bake this in on load

	jb_work = tsourdt3rd_global_jukebox->track_tics;
	jb_work %= jb_bpm;

	if (tsourdt3rd_global_jukebox->track_tics >= (FRACUNIT << (FRACBITS - 2))) // prevent overflow jump - takes about 15 minutes of loop on the same song to reach
		tsourdt3rd_global_jukebox->track_tics = jb_work;

	jb_work = FixedDiv(jb_work*180, jb_bpm);

	if (!S_MusicPaused()) // prevents time from being added up while the game is paused
		tsourdt3rd_global_jukebox->track_tics += renderdeltatics * ((S_GetSpeedMusic() > 0.0f) ? S_GetSpeedMusic() : 1.0f);
}

boolean TSoURDt3rd_M_Jukebox_Quit(void)
{
	Z_Free(tsourdt3rd_jukebox_defs);
	tsourdt3rd_jukebox_defs = NULL;

	Z_Free(tsourdt3rd_cur_page_jukebox_defs);
	tsourdt3rd_cur_page_jukebox_defs = NULL;

	cv_closedcaptioning.value = jb_cc; // undo hack

	jb_page_playable_tracks = 0;
	jb_draw_controls_tip = false;
	jb_controls_trans = 0;
	jb_controls_ticker = 0;

	tsourdt3rd_global_jukebox->in_menu = false;
	return true;
}
