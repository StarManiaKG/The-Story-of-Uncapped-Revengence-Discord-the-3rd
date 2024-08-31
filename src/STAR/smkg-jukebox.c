// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-jukebox.c
/// \brief TSoURDt3rd's cool and groovy sound features

#include "star_vars.h"
#include "ss_main.h"
#include "smkg-jukebox.h"
#include "m_menu.h"

#include "../d_main.h" // autoloaded/autoloading //
#include "../g_game.h"
#include "../i_video.h"
#include "../v_video.h"
#include "../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_jukebox_pages_t tsourdt3rd_jukeboxpage_soundtestpage = {
	"Main",
	NULL
};
tsourdt3rd_jukebox_pages_t *tsourdt3rd_jukeboxpages_start = &tsourdt3rd_jukeboxpage_soundtestpage;
tsourdt3rd_jukebox_pages_t **tsourdt3rd_jukebox_available_pages = NULL;

tsourdt3rd_jukeboxdef_t jukebox_def_soundtestsfx = {
	&soundtestsfx,
	0,
	NULL
};
tsourdt3rd_jukeboxdef_t *jukebox_def_start = &jukebox_def_soundtestsfx;
tsourdt3rd_jukeboxdef_t **tsourdt3rd_jukebox_defs = NULL;

tsourdt3rd_jukebox_t *tsourdt3rd_global_jukebox = NULL;

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_Jukebox_Unlocked(void)
// Checks if TSoURDt3rd's Jukebox has been unlocked.
//
boolean TSoURDt3rd_Jukebox_Unlocked(void)
{
	if (!tsourdt3rd_global_jukebox)
		return false;

	for (INT32 i = 0; i < MAXUNLOCKABLES; i++)
	{
		if ((unlockables[i].type == SECRET_SOUNDTEST) || (modifiedgame && !savemoddata) || autoloaded)
		{
			tsourdt3rd_global_jukebox->Unlocked = true;
			return true;
		}
	}

	tsourdt3rd_global_jukebox->Unlocked = false;
	return false;
}

//
// boolean TSoURDt3rd_Jukebox_PrepareDefs(void)
// Initalizes, prepares, and defines the data for individual jukebox defs.
//
static void TSoURDt3rd_Jukebox_LoadDefs(musicdef_t *def, tsourdt3rd_jukeboxdef_t **jukedefp)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];
	tsourdt3rd_jukeboxdef_t *jukedef_prev = NULL;
	tsourdt3rd_jukeboxdef_t *jukedef = jukebox_def_start;

	if (!def)
		return;

	while (jukedef)
	{
		if (jukedef && jukedef->linked_musicdef && jukedef->linked_musicdef == def)
		{
			(*jukedefp) = jukedef;
			return;
		}
		jukedef_prev = jukedef;
		jukedef = jukedef->next;
	}

	jukedef = Z_Calloc(sizeof(tsourdt3rd_jukeboxdef_t), PU_STATIC, NULL);
	jukedef->linked_musicdef = def;

	jukedef->supported_pages = Z_Calloc(sizeof(UINT8), PU_STATIC, NULL);
	jukedef->supported_pages[0] = 0;

	if (jukedef_prev != NULL)
		jukedef_prev->next = jukedef;

	(*jukedefp) = jukedef;
	CONS_Printf("TSoURDt3rd_Jukebox_LoadDefs: Added song '%s'\n", jukedef->linked_musicdef->name);
}

boolean TSoURDt3rd_Jukebox_PrepareDefs(void)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];
	tsourdt3rd_jukeboxdef_t *jukedef = NULL;
	musicdef_t *def;
	INT32 def_pos = 0;

	if (!numsoundtestdefs)
		S_InitMusicDefs();

	if (!S_PrepareSoundTest())
		return false;

	if (tsourdt3rd_jukebox_defs)
		Z_Free(tsourdt3rd_jukebox_defs);

	if (!(tsourdt3rd_jukebox_defs = Z_Malloc(numsoundtestdefs * sizeof(tsourdt3rd_jukeboxdef_t *), PU_STATIC, NULL)))
		I_Error("TSoURDt3rd_Jukebox_PrepareDefs(): could not allocate jukebox defs.");

	for (def = musicdefstart; def; def = def->next)
	{
		TSoURDt3rd_Jukebox_LoadDefs(def, &jukedef);
		tsourdt3rd_jukebox_defs[def_pos++] = jukedef;
	}
	return true;
}


//
// void TSoURDt3rd_Jukebox_Reset(void)
// Resets TSoURDt3rd's jukebox. Can reset level music too, if specified.
//
void TSoURDt3rd_Jukebox_Reset(void)
{
	if (!tsourdt3rd_global_jukebox || !tsourdt3rd_global_jukebox->playing)
		return;

	tsourdt3rd_global_jukebox->playing = false;
	tsourdt3rd_global_jukebox->initHUD = false;

	tsourdt3rd_global_jukebox->jukebox_tics = 0;

	tsourdt3rd_global_jukebox->curtrack = NULL;

	TSoURDt3rd_ControlMusicEffects();
	STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Jukebox reset.\n");
}

//
// void TSoURDt3rd_Jukebox_RefreshLevelMusic(void)
// Refreshes the level music after jukebox reset.
//
void TSoURDt3rd_Jukebox_RefreshLevelMusic(void)
{
	player_t *player = &players[consoleplayer];

	if (!player)
	{
		S_ChangeMusicEx(TSoURDt3rd_DetermineLevelMusic(), mapmusflags, true, mapmusposition, 0, 0);
		return;
	}

	if (!S_MusicPlaying())
	{
		P_RestoreMusic(player);
		if (netgame || multiplayer)
			P_RestoreMultiMusic(player);
	}

	if (player->powers[pw_super])
		P_PlayJingle(player, JT_SUPER);
}

//
// boolean TSoURDt3rd_Jukebox_CanModifyMusic(void)
// Prevents your fun jukebox music from being forcibly reset or modified (YAY!)
//
boolean TSoURDt3rd_Jukebox_CanModifyMusic(void)
{
	if (!tsourdt3rd_global_jukebox || !tsourdt3rd_global_jukebox->playing)
		return true;

	if (paused)
		S_ResumeAudio();
	return false;
}

//
// void TSoURDt3rd_Jukebox_ST_drawJukebox(void)
// Draws the jukebox's HUD.
//
void TSoURDt3rd_Jukebox_ST_drawJukebox(void)
{
	static INT32 boxw		= 320;	// Slides our Filed Box

	static INT32 strw		= 335; 	// Slides our Header Text
	static INT32 tstrw		= 320; 	// Slides our Track Text

	static INT32 sstrw		= 360;	// Slides our Side Jukebox HUD Text
	static INT32 jukeboxw	= 0;	// Stores the String Width of the Current Jukebox Track

	// Hide the Jukebox HUD if Circumstances Have Been Met //
	if (!tsourdt3rd_global_jukebox)
		return;
	else if (!cv_jukeboxhud.value || !tsourdt3rd_global_jukebox->playing)
	{
		boxw = 320; strw = 335; tstrw = 320; sstrw = 360;
		jukeboxw = 0;

		tsourdt3rd_global_jukebox->initHUD = false;
		return;
	}

	// Initialize the Jukebox HUD //
	if (boxw > 21) boxw -= 5;
	if (strw > 36) strw -= 5;
	if (tstrw > 21) tstrw -= 5;
	if (sstrw > 61) sstrw -= 5;

	jukeboxw = V_ThinStringWidth(va("PLAYING: %s", tsourdt3rd_global_jukebox->curtrack->title), V_SNAPTORIGHT|V_ALLOWLOWERCASE);

	// Apply Variables and Render Things //
	// The Box
	V_DrawFillConsoleMap(BASEVIDWIDTH-(boxw+jukeboxw), 45,
		(130+jukeboxw),
		(cv_jukeboxhud.value == 1 ? 25 : 55),
		(V_SNAPTORIGHT|V_HUDTRANSHALF));

	// Header Text
	V_DrawString(BASEVIDWIDTH-(strw+(jukeboxw/2)), 45,
		(V_SNAPTORIGHT|V_MENUCOLORMAP),
		("JUKEBOX"));

	// Track Title
	V_DrawThinString(BASEVIDWIDTH-(tstrw+jukeboxw-(cv_jukeboxhud.value == 1 ? 10 : 0)), 60,
		(V_SNAPTORIGHT|V_ALLOWLOWERCASE|V_YELLOWMAP),
		(va("PLAYING: %s", tsourdt3rd_global_jukebox->curtrack->title)));

	// Render Some Extra Things, and We're Done :) //
	if (cv_jukeboxhud.value != 2)
		return;

	// Track
	V_DrawThinString(BASEVIDWIDTH-sstrw, 80,
		(V_SNAPTORIGHT|V_ALLOWLOWERCASE|V_YELLOWMAP),
		(va("TRACK: %s", tsourdt3rd_global_jukebox->curtrack->name)));

	// Track Speed
	V_DrawThinString(BASEVIDWIDTH-sstrw, 90,
		(V_SNAPTORIGHT|V_YELLOWMAP),
		(atof(cv_jukeboxspeed.string) < 10.0f ?
			(va("SPEED: %.3s", cv_jukeboxspeed.string)) :
			(va("SPEED: %.4s", cv_jukeboxspeed.string))));
}
