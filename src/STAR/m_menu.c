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
#include "m_menu.h"
#include "smkg-jukebox.h"

#include "drrr/k_menu.h"

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

menuitem_t defaultMenuTitles[256][256];
gtdesc_t defaultGametypeTitles[NUMGAMETYPES];

// =============
// MENU HANDLERS
// =============

static void M_DrawTSoURDt3rdJukebox(void);
static void M_HandleTSoURDt3rdJukebox(INT32 choice);

// =====
// MENUS
// =====

static menuitem_t OP_Tsourdt3rdJukeboxMenu[] =
{
	{IT_KEYHANDLER | IT_STRING, NULL, "", M_HandleTSoURDt3rdJukebox, 0},
};

menu_t OP_TSoURDt3rdJukeboxDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_JUKEBOX),
	NULL,
	sizeof (OP_Tsourdt3rdJukeboxMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_JukeboxDef,
	OP_Tsourdt3rdJukeboxMenu,
	M_DrawTSoURDt3rdJukebox,
	60, 150,
	0,
	NULL
};

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
	TSoURDt3rdPlayers[consoleplayer].masterServerAddressChanged = true;
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

// =======
// JUKEBOX
// =======

static INT32 st_cc = 0;
static patch_t* st_radio[9];
static patch_t* st_launchpad[4];

static void M_CacheJukebox(void)
{
	UINT8 i;
	char buf[8];

	STRBUFCPY(buf, "M_RADIOn");
	for (i = 0; i < 9; i++)
	{
		buf[7] = (char)('0'+i);
		st_radio[i] = W_CachePatchName(buf, PU_PATCH);
	}

	STRBUFCPY(buf, "M_LPADn");
	for (i = 0; i < 4; i++)
	{
		buf[6] = (char)('0'+i);
		st_launchpad[i] = W_CachePatchName(buf, PU_PATCH);
	}
}

//
// void TSoURDt3rd_Jukebox_InitMenu(INT32 choice)
// Sets up the main Jukebox menu.
//
static void UpdateUnlocks(UINT8 *unlockables_table)
{
	INT32 i, j, ul;
	UINT8 done[MAXUNLOCKABLES];
	UINT16 curheight;

	memset(done, 0, sizeof(done));

	for (i = 1; i <= MAXUNLOCKABLES; ++i)
	{
		curheight = UINT16_MAX;
		ul = -1;

		// Autosort unlockables
		for (j = 0; j < MAXUNLOCKABLES; ++j)
		{
			if (!unlockables[j].height || done[j] || unlockables[j].type < 0)
				continue;

			if (unlockables[j].height < curheight)
			{
				curheight = unlockables[j].height;
				ul = j;
			}
		}
		if (ul < 0)
			break;

		done[ul] = true;
		unlockables_table[i-1] = (UINT8)ul;
	}
}

void TSoURDt3rd_Jukebox_InitMenu(INT32 choice)
{
	INT32 ul;
	UINT8 tsourdt3rd_skyRoomMenuTranslations[MAXUNLOCKABLES];

	if (currentMenu == &OP_TSoURDt3rdJukeboxDef)
		return;
	if (!tsourdt3rd_global_jukebox)
		return;

	memset(tsourdt3rd_skyRoomMenuTranslations, 0, sizeof(tsourdt3rd_skyRoomMenuTranslations));
	UpdateUnlocks(tsourdt3rd_skyRoomMenuTranslations);
	ul = tsourdt3rd_skyRoomMenuTranslations[choice-1];

	soundtestpage = (UINT8)(unlockables[ul].variable);
	if (!soundtestpage)
		soundtestpage = 1;

	if (!TSoURDt3rd_Jukebox_Unlocked())
	{
		DRRR_M_StartMessage(
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
		DRRR_M_StartMessage(
			"TSoURDt3rd Jukebox",
			M_GetText("No selectable tracks found.\n"),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		return;
	}
	M_CacheJukebox();

	st_cc = cv_closedcaptioning.value; // hack;
	cv_closedcaptioning.value = 1; // hack

	OP_TSoURDt3rdJukeboxDef.prevMenu = currentMenu; // Prevent the game from crashing when using the jukebox keybind :)
	M_SetupNextMenu(&OP_TSoURDt3rdJukeboxDef);
}

//
// static void M_DrawTSoURDt3rdJukebox(void)
// Draws the main Jukebox menu.
//
static void M_DrawTSoURDt3rdJukebox(void)
{
	INT32 x, y, i;

	// the ticker for the jukebox is ran within our menu systems' pre-drawer, so let's move on.

	x = 90<<FRACBITS;
	y = (BASEVIDHEIGHT-32)<<FRACBITS;

	V_DrawStretchyFixedPatch(x, y,
		tsourdt3rd_global_jukebox->jukebox_hscale, tsourdt3rd_global_jukebox->jukebox_vscale,
		0, st_radio[tsourdt3rd_global_jukebox->jukebox_frames[0]], NULL);

	V_DrawFixedPatch(x, y, FRACUNIT/2, 0, st_launchpad[0], NULL);

	for (i = 0; i < 9; i++)
	{
		if (i == tsourdt3rd_global_jukebox->jukebox_frames[2])
		{
			UINT8 *colmap = R_GetTranslationColormap(TC_RAINBOW, tsourdt3rd_global_jukebox->jukebox_frames[3], GTC_CACHE);
			V_DrawFixedPatch(x, y + (tsourdt3rd_global_jukebox->jukebox_frames[1]<<FRACBITS), FRACUNIT/2, 0, st_launchpad[tsourdt3rd_global_jukebox->jukebox_frames[1]+1], colmap);
		}
		else
			V_DrawFixedPatch(x, y, FRACUNIT/2, 0, st_launchpad[1], NULL);

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

		if (tsourdt3rd_global_jukebox->curtrack)
		{
			if (tsourdt3rd_global_jukebox->curtrack->alttitle[0])
				titl = va("%s - %s - ", tsourdt3rd_global_jukebox->curtrack->title, tsourdt3rd_global_jukebox->curtrack->alttitle);
			else
				titl = va("%s - ", tsourdt3rd_global_jukebox->curtrack->title);
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

		if (tsourdt3rd_global_jukebox->curtrack)
			V_DrawRightAlignedThinString(BASEVIDWIDTH-16, 46, V_ALLOWLOWERCASE, tsourdt3rd_global_jukebox->curtrack->authors);
	}

	V_DrawFill(165, 60, 140+15, 112, 159);
	{
		INT32 t, b, q, m = 112;

		if (numsoundtestdefs <= 7)
		{
			t = 0;
			b = numsoundtestdefs - 1;
			i = 0;
		}
		else
		{
			q = m;
			m = (5*m)/numsoundtestdefs;
			if (tsourdt3rd_global_jukebox->jukebox_selection < 3)
			{
				t = 0;
				b = 6;
				i = 0;
			}
			else if (tsourdt3rd_global_jukebox->jukebox_selection >= numsoundtestdefs-4)
			{
				t = numsoundtestdefs - 7;
				b = numsoundtestdefs - 1;
				i = q-m;
			}
			else
			{
				t = tsourdt3rd_global_jukebox->jukebox_selection - 3;
				b = tsourdt3rd_global_jukebox->jukebox_selection + 3;
				i = (t * (q-m))/(numsoundtestdefs - 7);
			}
		}

		V_DrawFill(165+140-1+15, 60 + i, 1, m, 0); // Draw a little scroll bar

		if (t != 0)
			V_DrawString(165+140+4, 60+4 - (tsourdt3rd_skullAnimCounter/5), V_MENUCOLORMAP, "\x1A");

		if (b != numsoundtestdefs - 1)
			V_DrawString(165+140+4, 60+112-12 + (tsourdt3rd_skullAnimCounter/5), V_MENUCOLORMAP, "\x1B");

		x = 169;
		y = 64;

		while (t <= b)
		{
			if (t == tsourdt3rd_global_jukebox->jukebox_selection)
				V_DrawFill(165, y-4, 140-1+16, 16, 155);

			if (!tsourdt3rd_jukebox_defs[t]->linked_musicdef->allowed)
			{
				V_DrawString(x, y, (t == tsourdt3rd_global_jukebox->jukebox_selection ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, "???");
			}
			else if (tsourdt3rd_jukebox_defs[t] == &jukebox_def_soundtestsfx)
			{
				const char *sfxstr = va("SFX %s", cv_soundtest.string);

				V_DrawString(x, y, (t == tsourdt3rd_global_jukebox->jukebox_selection ? V_MENUCOLORMAP : 0), sfxstr);
				if (t == tsourdt3rd_global_jukebox->jukebox_selection)
				{
					V_DrawCharacter(x - 10 - (tsourdt3rd_skullAnimCounter/5), y,
						'\x1C' | V_MENUCOLORMAP, false);
					V_DrawCharacter(x + 2 + V_StringWidth(sfxstr, 0) + (tsourdt3rd_skullAnimCounter/5), y,
						'\x1D' | V_MENUCOLORMAP, false);
				}

				if (tsourdt3rd_global_jukebox->curtrack == tsourdt3rd_jukebox_defs[t]->linked_musicdef)
				{
					sfxstr = (cv_soundtest.value ? S_sfx[cv_soundtest.value].name : "N/A");
					i = V_StringWidth(sfxstr, 0);

					V_DrawFill(165+140-9-i+16, y-4, i+8, 16, 150);
					V_DrawRightAlignedString(165+140-5+16, y, V_MENUCOLORMAP, sfxstr);
				}
			}
			else
			{
				if (strlen(tsourdt3rd_jukebox_defs[t]->linked_musicdef->title) < 18)
					V_DrawString(x, y, (t == tsourdt3rd_global_jukebox->jukebox_selection ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, tsourdt3rd_jukebox_defs[t]->linked_musicdef->title);
				else
					V_DrawThinString(x, y, (t == tsourdt3rd_global_jukebox->jukebox_selection ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, tsourdt3rd_jukebox_defs[t]->linked_musicdef->title);

				if (tsourdt3rd_global_jukebox->curtrack == tsourdt3rd_jukebox_defs[t]->linked_musicdef)
				{
					V_DrawFill(165-8, y-4, 8, 16, 150);
					//V_DrawCharacter(165-8, y, '\x19' | V_MENUCOLORMAP, false);
					V_DrawFixedPatch((165-8)<<FRACBITS, (y<<FRACBITS)-(tsourdt3rd_global_jukebox->jukebox_bouncing*4), FRACUNIT, 0, hu_font.chars['\x19'-FONTSTART], V_GetStringColormap(V_MENUCOLORMAP));
				}
			}

			t++;
			y += 16;
		}

		INT32 extra_option_flags = 0;
		{
			// Draw some of our extra options now

			if (tsourdt3rd_jukebox_defs[t] == &jukebox_def_soundtestsfx)
				extra_option_flags = V_TRANSLUCENT;
			else if (atof(cv_jukeboxspeed.string) > 20.0f || atof(cv_jukeboxspeed.string) < 0.1f)
				extra_option_flags = V_TRANSLUCENT;
			else
				extra_option_flags = V_MENUCOLORMAP;

			V_DrawString(
				10+BASEVIDWIDTH/2, 15+BASEVIDWIDTH/2,
				V_SNAPTORIGHT|extra_option_flags,
				"Music Speed");

			V_DrawString(
				114+BASEVIDWIDTH/2, 15+BASEVIDWIDTH/2,
				V_SNAPTORIGHT|extra_option_flags,
				va("%.4s", cv_jukeboxspeed.string));

			V_DrawCharacter( // Left Arrow
				100+BASEVIDWIDTH/2, 15+BASEVIDWIDTH/2,
				'\x1C' | V_SNAPTORIGHT | extra_option_flags,
				false
			);
			V_DrawCharacter( // Right Arrow
				152+BASEVIDWIDTH/2, 15+BASEVIDWIDTH/2,
				'\x1D' | V_SNAPTORIGHT | extra_option_flags,
				false
			);
		}
	}
}

//
// static void M_HandleTSoURDt3rdJukebox(INT32 choice)
// Controller system for the main Jukebox menu.
//
static void M_HandleTSoURDt3rdJukebox(INT32 choice)
{
	boolean exitmenu = true;

	switch (choice)
	{
		case KEY_UPARROW:
		case KEY_DOWNARROW:
			switch (choice)
			{
				case KEY_UPARROW:
					if (!tsourdt3rd_global_jukebox->jukebox_selection--)
						tsourdt3rd_global_jukebox->jukebox_selection = numsoundtestdefs-1;
					break;
				default:
					if (tsourdt3rd_global_jukebox->jukebox_selection++ >= numsoundtestdefs-1)
						tsourdt3rd_global_jukebox->jukebox_selection = 0;
					break;
			}

			cv_closedcaptioning.value = st_cc; // hack
			S_StartSound(NULL, sfx_menu1);
			cv_closedcaptioning.value = 1; // hack
			break;

		case KEY_PGUP:
		case KEY_PGDN:
			switch (choice)
			{
				case KEY_PGUP:
					if (!tsourdt3rd_global_jukebox->jukebox_selection)
						break;

					tsourdt3rd_global_jukebox->jukebox_selection -= 3;
					if (tsourdt3rd_global_jukebox->jukebox_selection < 0)
						tsourdt3rd_global_jukebox->jukebox_selection = 0;
					break;
				default:
					if (tsourdt3rd_global_jukebox->jukebox_selection >= numsoundtestdefs-1)
						break;

					tsourdt3rd_global_jukebox->jukebox_selection += 3;
					if (tsourdt3rd_global_jukebox->jukebox_selection >= numsoundtestdefs-1)
						tsourdt3rd_global_jukebox->jukebox_selection = numsoundtestdefs-1;
					break;
			}

			cv_closedcaptioning.value = st_cc; // hack
			S_StartSound(NULL, sfx_menu1);
			cv_closedcaptioning.value = 1; // hack
			break;

		case KEY_LEFTARROW:
		case KEY_RIGHTARROW:
			if (!tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection]->linked_musicdef->allowed)
				break;

			if (tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection] == &jukebox_def_soundtestsfx)
			{
				tsourdt3rd_global_jukebox->curtrack = tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection]->linked_musicdef;
				S_StopSounds();
				S_StopMusic();
				CV_AddValue(&cv_soundtest, (choice == KEY_RIGHTARROW ? 1 : -1));
				break;
			}

			switch (choice)
			{
				case KEY_LEFTARROW:
					if (atof(cv_jukeboxspeed.string) < 0.01f)
						break;
					CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)-(0.1f)));
					break;
				default:
					if (atof(cv_jukeboxspeed.string) > 20.0f)
						break;
					CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)+(0.1f)));
					break;
			}

			S_StartSound(NULL, sfx_menu1);
			break;

		case KEY_BACKSPACE:
			if (tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection] == &jukebox_def_soundtestsfx)
				tsourdt3rd_global_jukebox->curtrack = NULL;

			if (!tsourdt3rd_global_jukebox->curtrack)
			{
				S_StartSound(NULL, sfx_lose);
				break;
			}

			S_StopSounds();
			S_StopMusic();

			S_StartSound(NULL, sfx_skid);

			cv_closedcaptioning.value = st_cc; // hack
			cv_closedcaptioning.value = 1; // hack

			TSoURDt3rd_Jukebox_RefreshLevelMusic();
			break;

		case KEY_ENTER:
			S_StopSounds();
			S_StopMusic();

			if (!tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection]->linked_musicdef->allowed)
			{
				S_StartSound(NULL, sfx_lose);
				TSoURDt3rd_Jukebox_RefreshLevelMusic();
				break;
			}
			tsourdt3rd_global_jukebox->prevtrack = tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection]->linked_musicdef;
			tsourdt3rd_global_jukebox->curtrack = tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection]->linked_musicdef;

			if (tsourdt3rd_jukebox_defs[tsourdt3rd_global_jukebox->jukebox_selection] == &jukebox_def_soundtestsfx)
			{
				// S_StopMusic() -- is this necessary?
				if (cv_soundtest.value)
					S_StartSound(NULL, cv_soundtest.value);
				break;
			}

			if (TSoURDt3rd_AprilFools_ModeEnabled())
				tsourdt3rd_global_jukebox->curtrack = &tsourdt3rd_aprilfools_def;

			S_ChangeMusicInternal(tsourdt3rd_global_jukebox->curtrack->name, !tsourdt3rd_global_jukebox->curtrack->stoppingtics);
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, M_GetText("Loaded track \x82%s\x80.\n"), tsourdt3rd_global_jukebox->curtrack->title);

			tsourdt3rd_global_jukebox->playing = true;
			tsourdt3rd_global_jukebox->initHUD = true;

			TSoURDt3rd_ControlMusicEffects();
			break;

		case KEY_ESCAPE:
			exitmenu = false;
			break;

		default:
			break;
	}

	if (!exitmenu)
	{
		Z_Free(tsourdt3rd_jukebox_defs);
		tsourdt3rd_jukebox_defs = NULL;

		cv_closedcaptioning.value = st_cc; // undo hack

		if (currentMenu->prevMenu)
			M_SetupNextMenu(currentMenu->prevMenu);
		else
			M_ClearMenus(true);
	}
}
