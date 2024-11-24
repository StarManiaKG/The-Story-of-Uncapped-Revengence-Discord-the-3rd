// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/transient/smkg-options-tsourdt3rd-extras-jukebox.c
/// \brief TSoURDt3rd's full interactive Jukebox menu

#include "../../smkg-m_sys.h"

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

static fixed_t jb_work, jb_bpm;
static angle_t jb_ang;

static menu_t *last_menu_reference = NULL;
static tsourdt3rd_menu_t *last_tsourdt3rd_menu_reference = NULL;
boolean tsourdt3rd_jukebox_inmenu = false;

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
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Extras_JukeboxMenu[] =
{
	{NULL, "Should a HUD appear whenever you're using the Jukebox?", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, "Increase or decrease the speed of the current Jukebox track.", {NULL}, 0, 0},
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
	NULL,
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
		if (!stricmp(cur_juke_page->page_name, jb_page->page_name))
			return true;
		cur_juke_page = cur_juke_page->next;
	}
	return false;
}

static boolean M_Sys_FindNewJukeboxTrack(INT32 choice)
{
	if (jb_page_playable_tracks <= 1)
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

static void M_Sys_SetupNewJukeboxPage(void)
{
	tsourdt3rd_jukeboxdef_t *jukedef = NULL;
	INT32 cur_juke_def_pos;

	jb_page_playable_tracks = 0;

	Z_Free(tsourdt3rd_cur_page_jukebox_defs);
	tsourdt3rd_cur_page_jukebox_defs = NULL;
	if (!(tsourdt3rd_cur_page_jukebox_defs = Z_Malloc(numsoundtestdefs * sizeof(tsourdt3rd_jukeboxdef_t *), PU_STATIC, NULL)))
		I_Error("TSoURDt3rd_M_Jukebox_Init(): could not allocate jukebox defs.");

	for (cur_juke_def_pos = 0; cur_juke_def_pos < numsoundtestdefs; cur_juke_def_pos++)
	{
		jukedef = tsourdt3rd_jukebox_defs[cur_juke_def_pos];
		if (!M_Sys_TrackIsOnPage(jukedef))
			continue;
		tsourdt3rd_cur_page_jukebox_defs[jb_page_playable_tracks++] = jukedef;
	}
}

static boolean M_Sys_FindNewJukeboxPage(boolean decrease)
{
	size_t all_juke_pages = sizeof(tsourdt3rd_jukebox_available_pages)/sizeof(tsourdt3rd_jukebox_pages_t);

	if (!tsourdt3rd_jukebox_numpages)
		return false;

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
	M_Sys_SetupNewJukeboxPage();
	return true;
}

static void M_Sys_DrawJukebox(void)
{
	INT32 x, y, i;
	fixed_t hscale = FRACUNIT/2, vscale = FRACUNIT/2, bounce = 0;
	UINT8 frame[4] = {0, 0, -1, SKINCOLOR_RUBY};

	if (jb_draw_options)
	{
		V_DrawThinString(120, 164, V_YELLOWMAP, "Menu Controls");
		V_DrawThinString(12, 176, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Up/Up: Scroll Up");
		V_DrawThinString(12, 184, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Down/Down: Scroll Down");
		V_DrawThinString(180, 176, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Left/Right: Change Page");
		V_DrawThinString(180, 184, V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Tab: Modify Music Options");
		TSoURDt3rd_M_DrawGenericOptions();
		return;
	}

	if (tsourdt3rd_global_jukebox->playing)
	{
		if (tsourdt3rd_global_jukebox->curtrack == &soundtestsfx && cv_soundtest.value)
		{
			frame[1] = (2 - (tsourdt3rd_global_jukebox->track_tics >> FRACBITS));
			frame[2] = ((cv_soundtest.value - 1) % 9);
			frame[3] += (((cv_soundtest.value - 1) / 9) % (FIRSTSUPERCOLOR - frame[3]));
		}
		else
		{
			frame[0] = 8-(jb_work/(20<<FRACBITS));
			if (frame[0] > 8) // VERY small likelihood for the above calculation to wrap, but it turns out it IS possible lmao
				frame[0] = 0;

			jb_ang = (FixedAngle(jb_work)>>ANGLETOFINESHIFT) & FINEMASK;
			bounce = (FINESINE(jb_ang) - FRACUNIT/2);
			hscale -= bounce/16;
			vscale += bounce/16;
		}
	}

	x = 90<<FRACBITS;
	y = (BASEVIDHEIGHT-32)<<FRACBITS;

	V_DrawStretchyFixedPatch(x, y,
		hscale, vscale,
		0, jb_radio[frame[0]], NULL);

	V_DrawFixedPatch(x, y, FRACUNIT/2, 0, jb_launchpad[0], NULL);

	for (i = 0; i < 9; i++)
	{
		if (i == frame[2])
		{
			UINT8 *colmap = R_GetTranslationColormap(TC_RAINBOW, frame[3], GTC_CACHE);
			V_DrawFixedPatch(x, y + (frame[1]<<FRACBITS), FRACUNIT/2, 0, jb_launchpad[frame[1]+1], colmap);
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

	y = (BASEVIDWIDTH-(vid.width/vid.dupx))/2;

	V_DrawFill(y, 20, (vid.width/vid.dupx)+12, 24, 159);
	{
		static fixed_t st_scroll = -FRACUNIT;
		const char* titl;

		x = 16;
		V_DrawString(x, 10, 0, "NOW PLAYING:");

		if (tsourdt3rd_global_jukebox->playing)
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

		st_scroll += renderdeltatics;

		while (st_scroll >= (i << FRACBITS))
			st_scroll -= i << FRACBITS;

		x -= st_scroll >> FRACBITS;

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
			tsourdt3rd_jukeboxdef_t *cur_juke_def = NULL;

			if (jb_page_playable_tracks <= 1)
				break;
			cur_juke_def = tsourdt3rd_cur_page_jukebox_defs[t];

			if (t == jb_sel)
				V_DrawFill(165, y-4, 140-1+16, 16, 155);

			if (!cur_juke_def->linked_musicdef->allowed)
			{
				V_DrawString(x, y, (t == jb_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, "???");
			}
			else if (cur_juke_def == &jukebox_def_soundtestsfx)
			{
				const char *sfxstr = va("SFX %s", cv_soundtest.string);

				V_DrawString(x, y, (t == jb_sel ? V_MENUCOLORMAP : 0), sfxstr);
				if (t == jb_sel)
				{
					V_DrawCharacter(x - 10 - (tsourdt3rd_skullAnimCounter/5), y,
						'\x1C' | V_MENUCOLORMAP, false);
					V_DrawCharacter(x + 2 + V_StringWidth(sfxstr, 0) + (tsourdt3rd_skullAnimCounter/5), y,
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
					V_DrawFixedPatch((165-8)<<FRACBITS, (y<<FRACBITS)-(bounce*4), FRACUNIT, 0, hu_font.chars['\x19'-FONTSTART], V_GetStringColormap(V_MENUCOLORMAP));
				}
			}
			t++;
			y += 16;
		}

		V_DrawFill(165+140-1+15, 60 + saved_scrollbar_position, 1, m, 0); // little scroll bar

		// Pages
		if (!tsourdt3rd_jukebox_numpages)
			V_DrawRightAlignedThinString(BASEVIDWIDTH-50, 3, V_MENUCOLORMAP|V_ALLOWLOWERCASE, va("Page: \x82'%s'\x80", jb_page->page_name));
		else
			V_DrawRightAlignedThinString(BASEVIDWIDTH-50, 3, V_MENUCOLORMAP|V_ALLOWLOWERCASE, va("Page %d/%d: \x82'%s'\x80", jb_page->id, tsourdt3rd_jukebox_numpages, jb_page->page_name));

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
			if (jb_controls_trans_table[jb_controls_ticker])
				jb_controls_trans = jb_controls_trans_table[jb_controls_ticker];
			V_DrawFill(165-142, 60+28, 139, 50, 0|jb_controls_trans);
			V_DrawThinString(165-96, 60+33, jb_controls_trans|V_YELLOWMAP, "CONTROLS");
			V_DrawThinString(165-134, 60+41, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Up/Up: Scroll Up");
			V_DrawThinString(165-134, 60+49, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Page Down/Down: Scroll Down");
			V_DrawThinString(165-134, 60+57, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Left/Right: Change Page");
			V_DrawThinString(165-134, 60+65, jb_controls_trans|V_MENUCOLORMAP|V_ALLOWLOWERCASE, "Tab: Modify Music Options");
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

void TSoURDt3rd_M_Jukebox_Ticker(void)
{
	fixed_t stoppingtics;

	if (!tsourdt3rd_global_jukebox->playing)
	{
		if (!menuactive || currentMenu == NULL || currentMenu != &TSoURDt3rd_OP_Extras_JukeboxDef)
			jb_draw_controls_tip = true;
		return;
	}

	TSoURDt3rd_M_OptionsTick();
	stoppingtics = (fixed_t)(tsourdt3rd_global_jukebox->curtrack->stoppingtics) << FRACBITS;

	if (tsourdt3rd_global_jukebox->curtrack == &soundtestsfx)
	{
		if (cv_soundtest.value)
		{
			if (tsourdt3rd_global_jukebox->track_tics < (2 << FRACBITS))
				tsourdt3rd_global_jukebox->track_tics += renderdeltatics;
			if (tsourdt3rd_global_jukebox->track_tics >= (2 << FRACBITS))
				tsourdt3rd_global_jukebox->track_tics = 2 << FRACBITS;
		}
		return;
	}
	else if (stoppingtics && tsourdt3rd_global_jukebox->track_tics >= stoppingtics)
	{
		// Whoa, whoa, we've run out of time!
		TSoURDt3rd_Jukebox_Reset();
		TSoURDt3rd_Jukebox_RefreshLevelMusic();
		return;
	}

	jb_work = jb_bpm = tsourdt3rd_global_jukebox->curtrack->bpm / S_GetSpeedMusic();
	//jb_bpm = FixedDiv((60*TICRATE)<<FRACBITS, jb_bpm); -- bake this in on load

	jb_work = tsourdt3rd_global_jukebox->track_tics;
	jb_work %= jb_bpm;

	if (tsourdt3rd_global_jukebox->track_tics >= (FRACUNIT << (FRACBITS - 2))) // prevent overflow jump - takes about 15 minutes of loop on the same song to reach
		tsourdt3rd_global_jukebox->track_tics = jb_work;

	jb_work = FixedDiv(jb_work*180, jb_bpm);

	if (!S_MusicPaused()) // prevents time from being added up while the game is paused
		tsourdt3rd_global_jukebox->track_tics += renderdeltatics * S_GetSpeedMusic();
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

	M_Sys_CacheJukebox();
	M_Sys_SetupNewJukeboxPage();

	jb_cc = cv_closedcaptioning.value; // hack;
	cv_closedcaptioning.value = 1; // hack

	TSoURDt3rd_M_ResetOptions();
}

static boolean M_Sys_HandleJukebox(INT32 choice)
{
	tsourdt3rd_jukeboxdef_t *cur_juke_def = tsourdt3rd_cur_page_jukebox_defs[jb_sel];
	const UINT8 pid = 0;

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

	switch (choice)
	{
		case KEY_PGUP:
		case KEY_PGDN:
		{
			M_Sys_FindNewJukeboxTrack(choice);
			cv_closedcaptioning.value = jb_cc; // hack
			S_StartSound(NULL, sfx_menu1);
			cv_closedcaptioning.value = 1; // hack
			return true;
		}
		case KEY_TAB:
			jb_draw_options = true;
			return true;
		default:
			break;
	}

	if (menucmd[pid].dpad_ud > 0 || menucmd[pid].dpad_ud < 0) // down & up
	{
		cv_closedcaptioning.value = jb_cc; // hack
		if (M_Sys_FindNewJukeboxTrack(menucmd[pid].dpad_ud)) // up = -1; +1 otherwise
			S_StartSound(NULL, sfx_menu1);
		else
			S_StartSound(NULL, sfx_adderr);
		cv_closedcaptioning.value = 1; // hack
		return true;
	}
	else if (menucmd[pid].dpad_lr < 0 || menucmd[pid].dpad_lr > 0) // left & right
	{
		if ((cur_juke_def != &jukebox_def_soundtestsfx && cur_juke_def->linked_musicdef->allowed)
			|| jb_page_playable_tracks <= 1)
		{
			cv_closedcaptioning.value = jb_cc; // hack
			if (M_Sys_FindNewJukeboxPage(menucmd[pid].dpad_lr < 0)) // left = -1; +1 otherwise
				S_StartSound(NULL, sfx_menu1);
			else
				S_StartSound(NULL, sfx_adderr);
			cv_closedcaptioning.value = 1; // hack

			TSoURDt3rd_M_SetMenuDelay(pid);
			return true;
		}

		S_StopSounds();
		S_StopMusic();
		CV_AddValue(&cv_soundtest, ((menucmd[pid].dpad_lr > 0) ? 1 : -1)); // right, or left otherwise
		return true;
	}
	else if (TSoURDt3rd_M_MenuExtraPressed(pid))
	{
		if (!tsourdt3rd_global_jukebox->playing)
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

		if (!cur_juke_def->linked_musicdef->allowed)
		{
			S_StartSound(NULL, sfx_lose);
			TSoURDt3rd_Jukebox_RefreshLevelMusic();
			return true;
		}

		TSoURDt3rd_M_SetMenuDelay(pid);

		if (cur_juke_def == &jukebox_def_soundtestsfx)
		{
			// S_StopMusic() -- is this necessary?
			if (cv_soundtest.value)
				S_StartSound(NULL, cv_soundtest.value);
			return true;
		}

		if (!TSoURDt3rd_AprilFools_ModeEnabled())
		{
			tsourdt3rd_global_jukebox->prevtrack = cur_juke_def->linked_musicdef;
			tsourdt3rd_global_jukebox->curtrack = cur_juke_def->linked_musicdef;
		}
		else
		{
			tsourdt3rd_global_jukebox->curtrack = &tsourdt3rd_aprilfools_def;
			tsourdt3rd_global_jukebox->prevtrack = tsourdt3rd_global_jukebox->curtrack;
		}

		S_ChangeMusicInternal(
			tsourdt3rd_global_jukebox->curtrack->name,
			!tsourdt3rd_global_jukebox->curtrack->stoppingtics
		);
		tsourdt3rd_global_jukebox->playing = true;

		STAR_CONS_Printf(STAR_CONS_JUKEBOX, M_GetText("Loaded track \x82%s\x80.\n"), tsourdt3rd_global_jukebox->curtrack->title);
		return true;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		Z_Free(tsourdt3rd_jukebox_defs);
		tsourdt3rd_jukebox_defs = NULL;

		Z_Free(tsourdt3rd_cur_page_jukebox_defs);
		tsourdt3rd_cur_page_jukebox_defs = NULL;

		cv_closedcaptioning.value = jb_cc; // undo hack

		jb_page_playable_tracks = 0;
		TSoURDt3rd_M_SetupNextMenu(last_tsourdt3rd_menu_reference, last_menu_reference, false);
		TSoURDt3rd_M_SetMenuDelay(pid);

		jb_draw_controls_tip = false;
		jb_controls_trans = 0;
		jb_controls_ticker = 0;
		tsourdt3rd_jukebox_inmenu = false;
		return true;
	}

	return false;
}

void TSoURDt3rd_M_Jukebox_Init(INT32 choice)
{
	boolean in_menu = menuactive;

	if (currentMenu == &TSoURDt3rd_OP_Extras_JukeboxDef && in_menu)
	{
		// Please don't go to the same menu twice.
		return;
	}
	else if (TSoURDt3rd_M_DoesMenuHaveKeyHandler() > 0)
	{
		// Let's not be doing or reading something potentially important before we access the Jukebox, mmk?
		return;
	}

	if (in_menu == false)
	{
		// We should probably make sure that the menu is opened first...
		M_StartControlPanel();
		tsourdt3rd_itemOn = currentMenu->lastOn;
	}

	memset(tsourdt3rd_skyRoomMenuTranslations, 0, sizeof(tsourdt3rd_skyRoomMenuTranslations));
	M_Sys_UpdateUnlocks(tsourdt3rd_skyRoomMenuTranslations);
	skyRoomMenu_ul = tsourdt3rd_skyRoomMenuTranslations[choice-1];

	// When using the jukebox keybind, this prevents the game from crashing!
	if (in_menu)
	{
		last_tsourdt3rd_menu_reference = tsourdt3rd_currentMenu;
		last_menu_reference = currentMenu;
	}
	else
	{
		last_tsourdt3rd_menu_reference = NULL;
		last_menu_reference = NULL;
	}

	if (last_tsourdt3rd_menu_reference != NULL)
		TSoURDt3rd_TM_OP_Extras_JukeboxDef.music = last_tsourdt3rd_menu_reference->music;

	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_Extras_JukeboxDef, &TSoURDt3rd_OP_Extras_JukeboxDef, false);
	if (last_menu_reference == &TSoURDt3rd_OP_Extras_JukeboxDef)
		TSoURDt3rd_OP_Extras_JukeboxDef.lastOn = 0;

	optionsmenu.ticker = 0;
	TSoURDt3rd_M_OptionsTick();

	tsourdt3rd_jukebox_inmenu = true;
}
