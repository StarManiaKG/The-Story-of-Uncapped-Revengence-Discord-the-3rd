// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
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
#include "../../g_gamedata.h"
#include "../../i_video.h"
#include "../../r_main.h"
#include "../../v_video.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_jukebox_pages_t tsourdt3rd_jukeboxpage_mainpage = { 1, "All", NULL, NULL };
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

struct tsourdt3rd_global_jukebox_s tsourdt3rd_global_jukebox;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_Jukebox_Init(void)
// Initializes TSoURDt3rd's Jukebox.
//
void TSoURDt3rd_Jukebox_Init(void)
{
	memset(&tsourdt3rd_global_jukebox, 0, sizeof(struct tsourdt3rd_global_jukebox_s));
	tsourdt3rd_jukebox_numpages = 0;

	tsourdt3rd_jukebox_available_pages = Z_Malloc(sizeof(tsourdt3rd_jukebox_pages_t *), PU_STATIC, NULL);
	if (tsourdt3rd_jukebox_available_pages == NULL)
	{
		// NOOOOO.... MORRRREEEEEEE!
		STAR_CONS_Printf(STAR_CONS_ERROR, "TSoURDt3rd_Jukebox_Init(): Could not properly allocate memory for jukebox.\n");
		return;
	}
	tsourdt3rd_jukebox_available_pages[0] = &tsourdt3rd_jukeboxpage_mainpage;
	tsourdt3rd_jukebox_numpages = 1;

	tsourdt3rd_global_jukebox.hud_box_w = 320;
	tsourdt3rd_global_jukebox.hud_string_w = 335;
	tsourdt3rd_global_jukebox.hud_track_w = 320;
	tsourdt3rd_global_jukebox.hud_speed_w = 360;

	tsourdt3rd_global_jukebox.initialized = true;
	STAR_CONS_Printf(STAR_CONS_NOTICE, "TSoURDt3rd_Jukebox_Init()...\n");
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
		if (jukedef->linked_musicdef == def)
			break;
		jukedef_prev = jukedef;
		jukedef = jukedef->next;
	}

	if (jukedef == NULL)
	{
		jukedef = Z_Calloc(sizeof(tsourdt3rd_jukeboxdef_t), PU_STATIC, NULL);
		jukedef->linked_musicdef = def;

		jukedef->supported_pages = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);
		jukedef->supported_pages->num = 0;
		strcpy(jukedef->supported_pages[0].page_name, "All");
		jukedef->supported_pages[0].prev = NULL;
		jukedef->supported_pages[0].next = NULL;

		if (jukedef_prev != NULL)
			jukedef_prev->next = jukedef;

		STAR_CONS_Printf(STAR_CONS_DEBUG, "TSoURDt3rd_Jukebox_LoadDefs: Added song '%s'\n", jukedef->linked_musicdef->name[0]);
	}

	(*jukedefp) = jukedef;
}

boolean TSoURDt3rd_Jukebox_PrepareDefs(void)
{
	tsourdt3rd_jukeboxdef_t *jukedef = NULL;
	musicdef_t *def;
	INT32 jukedef_pos = 0;

	S_InitMusicDefs(); // Just in case we're doing this while the game's initializing...
	numsoundtestdefs = 0;

	for (def = musicdefstart; def; def = def->next)
	{
		def->allowed = false;
		numsoundtestdefs++;
	}
	if (numsoundtestdefs <= 0)
	{
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
		TSoURDt3rd_Jukebox_LoadDefs(def, &jukedef);
		tsourdt3rd_jukebox_defs[jukedef_pos++] = jukedef;
		def->allowed = TSoURDt3rd_Jukebox_SongUnlocked(def);
	}
	return true;
}

//
// boolean TSoURDt3rd_Jukebox_Initialized(void)
// Checks if TSoURDt3rd's Jukebox has been initialized.
//
boolean TSoURDt3rd_Jukebox_Initialized(void)
{
	return tsourdt3rd_global_jukebox.initialized;
}

//
// boolean TSoURDt3rd_Jukebox_Unlocked(void)
// Checks if TSoURDt3rd's Jukebox has been unlocked.
//
boolean TSoURDt3rd_Jukebox_Unlocked(void)
{
	if ((modifiedgame && !savemoddata) || tsourdt3rd_local.autoloaded_mods)
	{
		return true;
	}
	else
	{
		for (INT32 i = 0; i < MAXUNLOCKABLES; i++)
		{
			if (unlockables[i].type == SECRET_SOUNDTEST)
			{
				return true;
			}
		}
	}
	return false;
}

//
// boolean TSoURDt3rd_Jukebox_SongUnlocked(void)
// Checks if the given song has been unlocked in the jukebox.
//
boolean TSoURDt3rd_Jukebox_SongUnlocked(musicdef_t *def)
{
	gamedatalist_t *data_list = allClientGamedata;
	gamedata_t *data = NULL;
	UINT32 data_num;
	boolean already_achieved = false;
	boolean unlocked = false;

	if (def == NULL)
	{
		return false;
	}

#if 0
	if (!(def->soundtestpage & soundtestpage))
	{
		return false;
	}
#endif

	for (data_num = 0; data_num < data_list->num_data; data_num++)
	{
		data = data_list->data[data_num];
		if (def->soundtestcond > 0 && !(data->mapvisited[def->soundtestcond-1] & MV_BEATEN))
		{
			unlocked = false;
			continue;
		}
		if (!already_achieved && (def->soundtestcond < 0 && !M_Achieved(-1-def->soundtestcond, data)))
		{
			unlocked = false;
			continue;
		}
		unlocked = already_achieved = true;
		break;
	}
	return unlocked;
}

//
// boolean TSoURDt3rd_Jukebox_SongPlaying(void)
// Checks if TSoURDt3rd's Jukebox is currently playing music.
//
boolean TSoURDt3rd_Jukebox_SongPlaying(void)
{
	return (tsourdt3rd_global_jukebox.curtrack && tsourdt3rd_global_jukebox.playing);
}

//
// void TSoURDt3rd_Jukebox_Play(musicdef_t *def, INT32 track)
// Plays a track in the jukebox, or the last played track if the track given is NULL.
//
void TSoURDt3rd_Jukebox_Play(musicdef_t *def, INT32 track)
{
	char *name = (def->title ? def->title : def->name[track]);

	if (!TSoURDt3rd_Jukebox_Initialized())
	{
		TSoURDt3rd_M_StartPlainMessage("TSoURDt3rd Jukebox", M_GetText("The data needed for the jukebox wasn't initialized.\n"));
		S_StartSoundFromEverywhere(sfx_lose);
		return;
	}

	// Check unlocked status for song
	{
		tsourdt3rd_global_jukebox.unlocked = TSoURDt3rd_Jukebox_Unlocked();

		if (tsourdt3rd_global_jukebox.unlocked == false)
		{
			TSoURDt3rd_M_StartPlainMessage("TSoURDt3rd Jukebox", M_GetText("You haven't unlocked this yet!\nGo and unlock the sound test first!\n"));
			S_StartSoundFromEverywhere(sfx_lose);
			return;
		}
		else if (!TSoURDt3rd_Jukebox_SongUnlocked(def))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "This song hasn't been unlocked yet!\n");
			S_StartSoundFromEverywhere(sfx_lose);
			return;
		}
	}

	if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "There's already a track playing!\n");
		S_StartSoundFromEverywhere(sfx_lose);
		return;
	}

	if (!TSoURDt3rd_AprilFools_ModeEnabled())
	{
		if (def == NULL)
		{
			if (tsourdt3rd_global_jukebox.prevtrack == NULL)
			{
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "You haven't recently played a track!\n");
				S_StartSoundFromEverywhere(sfx_lose);
				return;
			}
			def = tsourdt3rd_global_jukebox.prevtrack;
		}
		else
		{
			tsourdt3rd_global_jukebox.prevtrack = def;
		}
	}
	else
	{
		// Always play April Fools music for April Fools!
		tsourdt3rd_global_jukebox.prevtrack = def = &tsourdt3rd_aprilfools_def;
	}
	tsourdt3rd_global_jukebox.prev_track_num = track;
	tsourdt3rd_global_jukebox.curtrack = def;

	S_ChangeMusicInternal(def->name[track], !def->stoppingtics);
	tsourdt3rd_global_jukebox.playing = true;

	S_StopMusicCredit();
	S_UnloadMusicCredit();

	S_ControlMusicEffects();

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_NOTICE, M_GetText("Loaded track \"\x82%s\x80\".\n"), name);
}

//
// void TSoURDt3rd_Jukebox_Stop(void)
// Resets TSoURDt3rd's jukebox. Can reset level music too, if specified.
//
void TSoURDt3rd_Jukebox_Stop(void)
{
	if (!TSoURDt3rd_Jukebox_SongPlaying())
	{
		return;
	}

	tsourdt3rd_global_jukebox.playing = false;
	tsourdt3rd_global_jukebox.music_speed = tsourdt3rd_global_jukebox.music_pitch = 1.0f;

	tsourdt3rd_global_jukebox.hud_initialized = false;
	tsourdt3rd_global_jukebox.hud_box_w = 320;
	tsourdt3rd_global_jukebox.hud_string_w = 335;
	tsourdt3rd_global_jukebox.hud_track_w = 320;
	tsourdt3rd_global_jukebox.hud_speed_w = 360;

	tsourdt3rd_global_jukebox.work = tsourdt3rd_global_jukebox.bpm = 0;
	tsourdt3rd_global_jukebox.track_tics = 0;
	tsourdt3rd_global_jukebox.curtrack = NULL;

	TSoURDt3rd_S_RefreshMusic();
	S_ControlMusicEffects();

	if (tsourdt3rd_global_jukebox.in_menu == false)
	{
		// Ok, let's try NOT to overload the console, please.
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_NOTICE, "Jukebox reset.\n");
	}
}

//
// void TSoURDt3rd_Jukebox_Tick(void)
// Updates the Jukebox every frame.
//
void TSoURDt3rd_Jukebox_Tick(void)
{
	if (!TSoURDt3rd_Jukebox_SongPlaying())
	{
		return;
	}

	tsourdt3rd_global_jukebox.music_speed = FloatToFixed(S_SpeedMusicAllowed() ? S_GetSpeedMusic() : 1.0f);
	tsourdt3rd_global_jukebox.music_pitch = FloatToFixed(S_PitchMusicAllowed() ? S_GetPitchMusic() : 1.0f);

	if (cv_soundtest.value)
	{
		if (tsourdt3rd_global_jukebox.track_tics < (2 << FRACBITS))
			tsourdt3rd_global_jukebox.track_tics += renderdeltatics;
		if (tsourdt3rd_global_jukebox.track_tics >= (2 << FRACBITS))
			tsourdt3rd_global_jukebox.track_tics = 2 << FRACBITS;
	}
	else
	{
		const fixed_t stoppingtics = (fixed_t)(tsourdt3rd_global_jukebox.curtrack->stoppingtics) << FRACBITS;

		if (stoppingtics && (tsourdt3rd_global_jukebox.track_tics >= stoppingtics)) // Whoa, whoa, we've run out of time!
		{
			S_StopMusic();
			if (tsourdt3rd_global_jukebox.in_menu == false)
				TSoURDt3rd_S_RefreshMusic();
			return;
		}
	}

	tsourdt3rd_global_jukebox.work = tsourdt3rd_global_jukebox.bpm = FixedDiv(tsourdt3rd_global_jukebox.curtrack->bpm, tsourdt3rd_global_jukebox.music_speed);
	//tsourdt3rd_global_jukebox.bpm = FixedDiv((60*TICRATE)<<FRACBITS, tsourdt3rd_global_jukebox.bpm); -- bake this in on load

	tsourdt3rd_global_jukebox.work = tsourdt3rd_global_jukebox.track_tics;
	tsourdt3rd_global_jukebox.work %= tsourdt3rd_global_jukebox.bpm;

	if (tsourdt3rd_global_jukebox.track_tics >= (FRACUNIT << (FRACBITS - 2))) // prevent overflow jump - takes about 15 minutes of loop on the same song to reach
		tsourdt3rd_global_jukebox.track_tics = tsourdt3rd_global_jukebox.work;

	tsourdt3rd_global_jukebox.work = FixedDiv(tsourdt3rd_global_jukebox.work*180, tsourdt3rd_global_jukebox.bpm);

	if (!S_MusicPaused()) // prevents time from being added up while the game is paused
		tsourdt3rd_global_jukebox.track_tics += FixedMul(renderdeltatics, tsourdt3rd_global_jukebox.music_speed);
}

//
// void TSoURDt3rd_Jukebox_ST_drawJukebox(void)
// Draws the jukebox's HUD.
//
/// \todo remake eventually
//
void TSoURDt3rd_Jukebox_ST_drawJukebox(void)
{
	musicdef_t *def = tsourdt3rd_global_jukebox.curtrack;
	INT32 track = tsourdt3rd_global_jukebox.prev_track_num;

	char *name;
	const char *playing_string;
	INT32 jukebox_w;
	INT32 ease_x;

	if (!TSoURDt3rd_Jukebox_Initialized() || !TSoURDt3rd_Jukebox_SongPlaying() || !cv_tsourdt3rd_jukebox_hud.value)
	{
		// Because we don't meet the conditions, just hide the HUD
		tsourdt3rd_global_jukebox.hud_initialized = false;
		tsourdt3rd_global_jukebox.hud_box_w = 320;
		tsourdt3rd_global_jukebox.hud_string_w = 335;
		tsourdt3rd_global_jukebox.hud_track_w = 320;
		tsourdt3rd_global_jukebox.hud_speed_w = 360;
		return;
	}

	if (tsourdt3rd_global_jukebox.hud_box_w > 21)
		tsourdt3rd_global_jukebox.hud_box_w -= 5;

	if (tsourdt3rd_global_jukebox.hud_string_w > 36)
		tsourdt3rd_global_jukebox.hud_string_w -= 5;

	if (tsourdt3rd_global_jukebox.hud_track_w > 11)
	{
		// I like centered text.
		tsourdt3rd_global_jukebox.hud_track_w -= 5;
	}

	if (tsourdt3rd_global_jukebox.hud_speed_w > -56)
		tsourdt3rd_global_jukebox.hud_speed_w -= 5;

	name = (def->title ? def->title : def->name[track]);
	playing_string = va("Playing: %s", name);
	jukebox_w = V_ThinStringWidth(playing_string, V_SNAPTORIGHT|V_ALLOWLOWERCASE);
	ease_x = tsourdt3rd_global_jukebox.hud_speed_w;

	// Apply Variables and Render Things //
	// The Box
	V_DrawFillConsoleMap((BASEVIDWIDTH - (tsourdt3rd_global_jukebox.hud_box_w + jukebox_w)), 45,
		130+jukebox_w, (cv_tsourdt3rd_jukebox_hud.value == 1 ? 25 : 65),
		V_SNAPTORIGHT|V_HUDTRANSDOUBLE
	);

	// Header Text
	V_DrawString((BASEVIDWIDTH - (tsourdt3rd_global_jukebox.hud_string_w + (jukebox_w/2))), 45,
		V_SNAPTORIGHT|V_MENUCOLORMAP,
		"JUKEBOX"
	);

	// Track Title
	V_DrawThinString((BASEVIDWIDTH - (tsourdt3rd_global_jukebox.hud_track_w + jukebox_w)), 60,
		V_SNAPTORIGHT|V_ALLOWLOWERCASE|V_YELLOWMAP,
		playing_string
	);

	// Render Some Extra Things, and We're Done :) //
	if (cv_tsourdt3rd_jukebox_hud.value != 2)
		return;

	// Track
	V_DrawThinString(BASEVIDWIDTH + ease_x, 80,
		V_SNAPTORIGHT|V_ALLOWLOWERCASE|V_YELLOWMAP,
		va("Lump: %s", def->name[track])
	);

	// Track Speed
	V_DrawThinString(BASEVIDWIDTH + ease_x, 90,
		V_SNAPTORIGHT|(S_SpeedMusicAllowed() ? V_YELLOWMAP : V_GRAYMAP|V_TRANSLUCENT),
		va("Speed: %.4s", cv_tsourdt3rd_jukebox_speed.string)
	);

	// Track Pitch
	V_DrawThinString(BASEVIDWIDTH + ease_x, 100,
		V_SNAPTORIGHT|(S_PitchMusicAllowed() ? V_YELLOWMAP : V_GRAYMAP|V_TRANSLUCENT),
		va("Pitch: %.4s", cv_tsourdt3rd_jukebox_pitch.string)
	);
}
