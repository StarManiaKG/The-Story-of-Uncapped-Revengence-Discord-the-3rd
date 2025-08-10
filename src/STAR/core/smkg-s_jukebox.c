// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_jukebox.c
/// \brief TSoURDt3rd's portable jammin' Jukebox

#include "smkg-s_jukebox.h"

#include "smkg-s_audio.h"
#include "../menus/smkg-m_sys.h"

#include "../../g_game.h"
#include "../../i_video.h"
#include "../../v_video.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_jukebox_pages_t tsourdt3rd_jukeboxpage_mainpage = { 0, "All", NULL, NULL };
tsourdt3rd_jukebox_pages_t *tsourdt3rd_jukeboxpages_start = &tsourdt3rd_jukeboxpage_mainpage;
tsourdt3rd_jukebox_pages_t **tsourdt3rd_jukebox_available_pages = NULL;
INT32 tsourdt3rd_jukebox_numpages = 0;

tsourdt3rd_jukeboxdef_t jukebox_def_soundtestsfx = {
	&soundtestsfx,
	&tsourdt3rd_jukeboxpage_mainpage,
	NULL
};
tsourdt3rd_jukeboxdef_t *jukebox_def_start = &jukebox_def_soundtestsfx;
tsourdt3rd_jukeboxdef_t **tsourdt3rd_jukebox_defs = NULL;

tsourdt3rd_jukebox_t *tsourdt3rd_global_jukebox = NULL;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_Jukebox_Init(void)
// Initializes TSoURDt3rd's Jukebox.
//
void TSoURDt3rd_Jukebox_Init(void)
{
	if (dedicated)
	{
		// Dude, you're in a dedicated server! You don't need this!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_Jukebox_Init(): Dedicated mode active, not intializing jukebox.\n");
		return;
	}

	tsourdt3rd_global_jukebox = Z_Calloc(sizeof(tsourdt3rd_jukebox_t), PU_STATIC, NULL);
	if (tsourdt3rd_global_jukebox == NULL)
	{
		// Let's leave now before we cause some crashes...
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_Jukebox_Init(): Could not allocate jukebox memory.\n");
		return;
	}

	tsourdt3rd_jukebox_available_pages = Z_Malloc(TSOURDT3RD_JUKEBOX_MAX_PAGES * sizeof(tsourdt3rd_jukebox_pages_t *), PU_STATIC, NULL);
	if (tsourdt3rd_jukebox_available_pages == NULL)
	{
		// We should also leave here too, in order to prevent future crashes...
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_Jukebox_Init(): Could not allocate memory for jukebox pages.\n");
		return;
	}
	tsourdt3rd_jukebox_available_pages[0] = &tsourdt3rd_jukeboxpage_mainpage;

	tsourdt3rd_global_jukebox->hud_box_w = 320;
	tsourdt3rd_global_jukebox->hud_string_w = 335;
	tsourdt3rd_global_jukebox->hud_track_w = 320;
	tsourdt3rd_global_jukebox->hud_speed_w = 360;
}

//
// boolean TSoURDt3rd_Jukebox_Unlocked(void)
// Checks if TSoURDt3rd's Jukebox has been unlocked.
//
boolean TSoURDt3rd_Jukebox_Unlocked(void)
{
	for (INT32 i = 0; i < MAXUNLOCKABLES; i++)
	{
		if ((unlockables[i].type == SECRET_SOUNDTEST) || (modifiedgame && !savemoddata) || tsourdt3rd_local.autoloaded_mods)
		{
			tsourdt3rd_global_jukebox->unlocked = true;
			break;
		}
		tsourdt3rd_global_jukebox->unlocked = false;
	}
	return tsourdt3rd_global_jukebox->unlocked;
}

//
// boolean TSoURDt3rd_Jukebox_PrepareDefs(void)
// Initalizes, prepares, and defines the data for individual jukebox defs.
//
static void TSoURDt3rd_Jukebox_LoadDefs(musicdef_t *def, tsourdt3rd_jukeboxdef_t **jukedefp)
{
	tsourdt3rd_jukeboxdef_t *jukedef_prev = NULL;
	tsourdt3rd_jukeboxdef_t *jukedef = jukebox_def_start;

	if (!def)
	{
		// ...How?
		return;
	}

	while (jukedef)
	{
		if (jukedef->linked_musicdef == def) break;
		jukedef_prev = jukedef;
		jukedef = jukedef->next;
	}

	if (jukedef == NULL)
	{
		jukedef = Z_Calloc(sizeof(tsourdt3rd_jukeboxdef_t), PU_STATIC, NULL);
		jukedef->linked_musicdef = def;

		jukedef->supported_pages = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);
		jukedef->supported_pages->id = 0;
		strcpy(jukedef->supported_pages[0].page_name, "All");
		jukedef->supported_pages[0].prev = NULL;
		jukedef->supported_pages[0].next = NULL;

		if (jukedef_prev != NULL) jukedef_prev->next = jukedef;
		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_Jukebox_LoadDefs: Added song '%s'\n", jukedef->linked_musicdef->name);
	}

	(*jukedefp) = jukedef;
}

boolean TSoURDt3rd_Jukebox_PrepareDefs(void)
{
	tsourdt3rd_jukeboxdef_t *jukedef = NULL;
	musicdef_t *def;
	gamedata_t *data = clientGamedata;
	INT32 jukedef_pos = 0;

	if (!numsoundtestdefs)
	{
		// Just in case we're doing this while the game's initializing...
		S_InitMusicDefs();
	}
	numsoundtestdefs = 0;

	for (def = musicdefstart; def; def = def->next)
	{
		if (!(def->soundtestpage & soundtestpage))
			continue;
		def->allowed = false;
		numsoundtestdefs++;
	}
	if (!numsoundtestdefs)
	{
		// STILL not any? Let's just quit then...
		return false;
	}

	if (tsourdt3rd_jukebox_defs)
	{
		Z_Free(tsourdt3rd_jukebox_defs);
		tsourdt3rd_jukebox_defs = NULL;
	}
	if (!(tsourdt3rd_jukebox_defs = Z_Malloc(numsoundtestdefs * sizeof(tsourdt3rd_jukeboxdef_t *), PU_STATIC, NULL)))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "TSoURDt3rd_Jukebox_PrepareDefs(): could not allocate jukebox defs.");
		return false;
	}

	for (def = musicdefstart; def; def = def->next)
	{
		if (!(def->soundtestpage & soundtestpage))
			continue;

		TSoURDt3rd_Jukebox_LoadDefs(def, &jukedef);
		tsourdt3rd_jukebox_defs[jukedef_pos++] = jukedef;

		if (def->soundtestcond > 0 && !(data->mapvisited[def->soundtestcond-1] & MV_BEATEN))
			continue;
		if (def->soundtestcond < 0 && !M_Achieved(-1-def->soundtestcond, data))
			continue;
		def->allowed = true;
	}
	return true;
}

//
// void TSoURDt3rd_Jukebox_Play(musicdef_t *play_def)
// Plays a track in the jukebox, or the last played track if the track given is NULL.
//
void TSoURDt3rd_Jukebox_Play(musicdef_t *play_def)
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
		S_StartSoundFromEverywhere(sfx_lose);
		return;
	}
	else if (!TSoURDt3rd_Jukebox_Unlocked())
	{
		// We haven't even unlocked it yet!
		TSoURDt3rd_M_StartMessage(
			"TSoURDt3rd Jukebox",
			M_GetText("You haven't unlocked this yet!\nGo and unlock the sound test first!\n"),
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		S_StartSoundFromEverywhere(sfx_lose);
		return;
	}
	else if (TSoURDt3rd_Jukebox_IsPlaying())
	{
		// We shouldn't interrupt ourself!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "There's already a track playing!\n");
		S_StartSoundFromEverywhere(sfx_lose);
		return;
	}

	if (!TSoURDt3rd_AprilFools_ModeEnabled())
	{
		if (play_def == NULL)
		{
			if (!tsourdt3rd_global_jukebox->prevtrack)
			{
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "You haven't recently played a track!\n");
				S_StartSoundFromEverywhere(sfx_lose);
				return;
			}
			play_def = tsourdt3rd_global_jukebox->prevtrack;
		}
		else
			tsourdt3rd_global_jukebox->prevtrack = play_def;
	}
	else
		tsourdt3rd_global_jukebox->prevtrack = play_def = &tsourdt3rd_aprilfools_def;
	tsourdt3rd_global_jukebox->curtrack = play_def;

	S_ChangeMusicInternal(
		tsourdt3rd_global_jukebox->curtrack->name,
		!tsourdt3rd_global_jukebox->curtrack->stoppingtics
	);
	tsourdt3rd_global_jukebox->playing = true;

	TSoURDt3rd_S_ControlMusicEffects(NULL, NULL);
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_NOTICE, M_GetText("Loaded track \x82%s\x80.\n"), tsourdt3rd_global_jukebox->curtrack->title);
}

//
// boolean TSoURDt3rd_Jukebox_IsPlaying(void)
// Checks if TSoURDt3rd's Jukebox is currently playing music.
//
boolean TSoURDt3rd_Jukebox_IsPlaying(void)
{
	if (tsourdt3rd_global_jukebox == NULL) return false;
	return (tsourdt3rd_global_jukebox->curtrack && tsourdt3rd_global_jukebox->playing);
}

//
// void TSoURDt3rd_Jukebox_Reset(void)
// Resets TSoURDt3rd's jukebox. Can reset level music too, if specified.
//
/// /// \todo remake eventually
//
void TSoURDt3rd_Jukebox_Reset(void)
{
	if (!TSoURDt3rd_Jukebox_IsPlaying())
		return;

	tsourdt3rd_global_jukebox->playing = false;

	tsourdt3rd_global_jukebox->hud_initialized = false;
	tsourdt3rd_global_jukebox->hud_box_w = 320;
	tsourdt3rd_global_jukebox->hud_string_w = 335;
	tsourdt3rd_global_jukebox->hud_track_w = 320;
	tsourdt3rd_global_jukebox->hud_speed_w = 360;

	tsourdt3rd_global_jukebox->track_tics = 0;
	tsourdt3rd_global_jukebox->curtrack = NULL;

	TSoURDt3rd_S_ControlMusicEffects(NULL, NULL);
	TSoURDt3rd_S_RefreshMusic();

	if (!tsourdt3rd_global_jukebox->in_menu)
	{
		// Ok, let's try NOT to overload the console, please.
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_NOTICE, "Jukebox reset.\n");
	}
}

//
// void TSoURDt3rd_Jukebox_ST_drawJukebox(void)
// Draws the jukebox's HUD.
//
/// \todo remake eventually
//
void TSoURDt3rd_Jukebox_ST_drawJukebox(void)
{
	if (!TSoURDt3rd_Jukebox_IsPlaying() || !cv_tsourdt3rd_jukebox_hud.value)
	{
		// Because we don't meet the conditions, just hide the HUD
		if (tsourdt3rd_global_jukebox != NULL)
		{
			tsourdt3rd_global_jukebox->hud_initialized = false;
			tsourdt3rd_global_jukebox->hud_box_w = 320;
			tsourdt3rd_global_jukebox->hud_string_w = 335;
			tsourdt3rd_global_jukebox->hud_track_w = 320;
			tsourdt3rd_global_jukebox->hud_speed_w = 360;
		}
		return;
	}

	// Initialize the Jukebox HUD //
	if (tsourdt3rd_global_jukebox->hud_box_w > 21)
		tsourdt3rd_global_jukebox->hud_box_w -= 5;

	if (tsourdt3rd_global_jukebox->hud_string_w > 36)
		tsourdt3rd_global_jukebox->hud_string_w -= 5;

	if (tsourdt3rd_global_jukebox->hud_track_w > 11)
	{
		// I like centered text.
		tsourdt3rd_global_jukebox->hud_track_w -= 5;
	}

	if (tsourdt3rd_global_jukebox->hud_speed_w > -56)
		tsourdt3rd_global_jukebox->hud_speed_w -= 5;

	const INT32 jukebox_w = V_ThinStringWidth(va("PLAYING: %s", tsourdt3rd_global_jukebox->curtrack->title), V_SNAPTORIGHT|V_ALLOWLOWERCASE);

	// Apply Variables and Render Things //
	// The Box
	V_DrawFillConsoleMap(
		(BASEVIDWIDTH - (tsourdt3rd_global_jukebox->hud_box_w + jukebox_w)), 45,
		130+jukebox_w, (cv_tsourdt3rd_jukebox_hud.value == 1 ? 25 : 65),
		V_SNAPTORIGHT|V_HUDTRANSHALF
	);

	// Header Text
	V_DrawString(
		(BASEVIDWIDTH - (tsourdt3rd_global_jukebox->hud_string_w + (jukebox_w/2))), 45,
		V_SNAPTORIGHT|V_MENUCOLORMAP,
		"JUKEBOX"
	);

	// Track Title
	V_DrawThinString(
		(BASEVIDWIDTH - (tsourdt3rd_global_jukebox->hud_track_w + jukebox_w)),
		60,
		V_SNAPTORIGHT|V_ALLOWLOWERCASE|V_YELLOWMAP,
		va("PLAYING: %s", tsourdt3rd_global_jukebox->curtrack->title)
	);

	// Render Some Extra Things, and We're Done :) //
	if (cv_tsourdt3rd_jukebox_hud.value != 2)
		return;

	// Track
	V_DrawThinString(
		BASEVIDWIDTH + tsourdt3rd_global_jukebox->hud_speed_w, 80,
		V_SNAPTORIGHT|V_ALLOWLOWERCASE|V_YELLOWMAP,
		va("TRACK: %s", tsourdt3rd_global_jukebox->curtrack->name)
	);

	// Track Speed
	V_DrawThinString(
		BASEVIDWIDTH + tsourdt3rd_global_jukebox->hud_speed_w, 90,
		V_SNAPTORIGHT|V_YELLOWMAP,
		va("SPEED: %.4s", cv_tsourdt3rd_jukebox_speed.string)
	);

	// Track Pitch
	V_DrawThinString(
		BASEVIDWIDTH + tsourdt3rd_global_jukebox->hud_speed_w, 100,
		V_SNAPTORIGHT|V_YELLOWMAP,
		va("Pitch: %.4s", cv_tsourdt3rd_jukebox_pitch.string)
	);
}
