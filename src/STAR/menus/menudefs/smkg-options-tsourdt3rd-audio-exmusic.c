// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-audio-exmusic.c
/// \brief TSoURDt3rd's EXMusic menu options

#include "../smkg-m_sys.h"
#include "../../core/smkg-s_exmusic.h"

tsourdt3rd_exmusic_findTrackResult_t exm_found_track_result;
static boolean menu_access_granted = false;
static boolean menu_music_listening = false;

static void M_Sys_EXMusicDrawer(void);
static void M_Sys_EXMusicTicker(void);
static void M_Sys_EXMusicInit(void);
static boolean M_Sys_EXMusicQuit(void);
static boolean M_Sys_EXMusicInput(INT32 choice);

#define EXMUSIC_MENU_CVAR(name, val) {IT_STRING | IT_CVAR, NULL, name, &cv_tsourdt3rd_audio_exmusic[val], 0}

menuitem_t TSoURDt3rd_OP_Audio_EXMusicMenu[] =
{
	{IT_HEADER, NULL, "Levels", NULL, 0},
		EXMUSIC_MENU_CVAR("Default Map Track", tsourdt3rd_exmusic_defaultmaptrack),
		EXMUSIC_MENU_CVAR("Game Over", tsourdt3rd_exmusic_gameover),

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL, NULL, 0},

	{IT_HEADER, NULL, "Bosses", NULL, 0},
		EXMUSIC_MENU_CVAR("Normal Bosses", tsourdt3rd_exmusic_bosses),
		EXMUSIC_MENU_CVAR("Pinch Phase", tsourdt3rd_exmusic_bosses_pinch),

		{IT_SPACE, NULL, NULL, NULL, 0},

		EXMUSIC_MENU_CVAR("Final Bosses", tsourdt3rd_exmusic_bosses_finalboss),
		EXMUSIC_MENU_CVAR("Pinch Phase", tsourdt3rd_exmusic_bosses_finalboss_pinch),

		{IT_SPACE, NULL, NULL, NULL, 0},

		EXMUSIC_MENU_CVAR("True Final Bosses", tsourdt3rd_exmusic_bosses_truefinalboss),
		EXMUSIC_MENU_CVAR("Pinch Phase", tsourdt3rd_exmusic_bosses_truefinalboss_pinch),

		{IT_SPACE, NULL, NULL, NULL, 0},

		EXMUSIC_MENU_CVAR("Race", tsourdt3rd_exmusic_bosses_race),

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL, NULL, 0},

	{IT_HEADER, NULL, "Intermission", NULL, 0},
		EXMUSIC_MENU_CVAR("Normal", tsourdt3rd_exmusic_intermission),
		EXMUSIC_MENU_CVAR("Bosses", tsourdt3rd_exmusic_intermission_boss),
		EXMUSIC_MENU_CVAR("Final Bosses", tsourdt3rd_exmusic_intermission_finalboss),
		EXMUSIC_MENU_CVAR("True Final Bosses", tsourdt3rd_exmusic_intermission_truefinalboss)
};

#undef EXMUSIC_MENU_CVAR

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_Audio_EXMusicMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Maps that don't have tracks selected will play this instead.", {NULL}, 0, 0},
		{NULL, "Music that plays when you get a game over.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Music that plays when a boss is present within a level.", {NULL}, 0, 0},
		{NULL, "The above option, but the music changes if the boss is on its last legs.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when you fight the final boss.", {NULL}, 0, 0},
		{NULL, "The above option, but the music changes if the boss is on its last legs.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when you fight the true final boss.", {NULL}, 0, 0},
		{NULL, "The above option, but the music changes if the boss is on its last legs.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when a race is present within a level.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Music that plays once you've completed an act.", {NULL}, 0, 0},
		{NULL, "Music that plays once you've beaten a boss.", {NULL}, 0, 0},
		{NULL, "Music that plays once you've beaten the final boss.", {NULL}, 0, 0},
		{NULL, "Music that plays once you've beaten the true final boss.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_Audio_EXMusicDef =
{
	MTREE4(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_AUDIO, MN_OP_TSOURDT3RD_AUDIO_EXMUSIC),
	NULL,
	sizeof (TSoURDt3rd_OP_Audio_EXMusicMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_AudioDef,
	TSoURDt3rd_OP_Audio_EXMusicMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	24, 76,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_Audio_EXMusicDef = {
	TSoURDt3rd_TM_OP_Audio_EXMusicMenu,
	SKINCOLOR_SLATE, 18,
	0,
	NULL,
	2, 5,
	M_Sys_EXMusicDrawer,
	M_Sys_EXMusicTicker,
	M_Sys_EXMusicInit,
	M_Sys_EXMusicQuit,
	M_Sys_EXMusicInput,
	NULL,
	&TSoURDt3rd_TM_OP_AudioDef
};

static boolean EXMusic_FindNextTrack(tsourdt3rd_exmusic_findTrackResult_t *result, INT32 musicset_type, boolean decrease)
{
	tsourdt3rd_exmusic_data_series_t *series = result->series;
	tsourdt3rd_exmusic_musicset_t *set = result->track_set;
	musicdef_t *music_track = result->lump;
	INT32 series_index = result->series_pos, track_index = result->track_pos;

	do
	{
		track_index = (decrease ? track_index-1 : track_index+1);
		if (set != NULL && set->num_music_lumps && (track_index >= 0 && track_index < set->num_music_lumps))
		{
			music_track = set->music[track_index];
			break;
		}
		else
		{
			if (decrease)
			{
				series_index = (series->prev != NULL ? series_index-1 : tsourdt3rd_exmusic_num_series-1);
			}
			else
			{
				series_index = (series->next != NULL ? series_index+1 : 0);
			}
			series = result->all_series[series_index];
			set = series->track_sets[musicset_type];

			if (set->num_music_lumps > 0)
			{
				track_index = (decrease ? (set->num_music_lumps-1) : 0);
				music_track = set->music[track_index];
				break;
			}

			track_index = 0;
		}
	} while (series != NULL && !(series == result->series && track_index == result->track_pos));
	if (series == NULL || (series == result->series && track_index == result->track_pos))
	{
		return false;
	}

	result->series_pos	    = series_index;
	result->track_pos	    = track_index;
	result->series		    = series;
	result->track_set	    = set;
	result->lump		    = music_track;
	result->all_music_lumps	= set->music;
	return (result->lump != NULL);
}

static void M_Sys_FindNewEXMusicTrack(boolean decrease)
{
	tsourdt3rd_exmusic_findTrackResult_t cur_exm_found_track_result = exm_found_track_result;
	consvar_t *exm_cvar = exm_found_track_result.cvar;
	INT32 exm_identifier_pos = exm_found_track_result.identifier_pos;
	char string[256];

	if (EXMusic_FindNextTrack(&cur_exm_found_track_result, exm_identifier_pos, decrease) == false)
	{
		S_StartSoundFromEverywhere(sfx_lose); // Play the accurately named sound effect for this scenario :p
		return;
	}

	snprintf(string, 256, "%s:%d", cur_exm_found_track_result.series->series_name, cur_exm_found_track_result.track_pos);
	exm_found_track_result = cur_exm_found_track_result;

	/// \todo: STAR NOTE: Should be CV_Set or something immediate
	COM_BufInsertText(va("%s \"%s\"\n", exm_cvar->name, string));

	menu_music_listening = false;
	TSoURDt3rd_S_RefreshMusic();
	S_StartSoundFromEverywhere(sfx_s3k5b);
}

void TSoURDt3rd_M_EXMusic_LoadMenu(INT32 choice)
{
	const UINT8 pid = 0;
	static boolean already_checked = false;
	INT32 menuopt;

	(void)choice;
	memset(&exm_found_track_result, 0, sizeof(exm_found_track_result));

	if (already_checked == false && tsourdt3rd_exmusic_initialized)
	{
		for (menuopt = 0; menuopt < TSoURDt3rd_OP_Audio_EXMusicDef.numitems; menuopt++)
		{
			consvar_t *cvar = (consvar_t *)TSoURDt3rd_OP_Audio_EXMusicMenu[menuopt].itemaction;
			if (TSoURDt3rd_EXMusic_ReturnTypeFromCVar(cvar, NULL) != NULL)
			{
				menu_access_granted = true;
				break;
			}
		}
	}
	already_checked = true;

	if (menu_access_granted == false)
	{
		// Can't let you do that, Star Fox.
		TSoURDt3rd_M_StartPlainMessage("TSoURDt3rd EXMusic", M_GetText("The data needed for EXMusic wasn't initialized.\n"));
		return;
	}

	TSoURDt3rd_TM_OP_Audio_EXMusicDef.music = tsourdt3rd_currentMenu->music;
	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_Audio_EXMusicDef, &TSoURDt3rd_OP_Audio_EXMusicDef, false);
	TSoURDt3rd_M_SetMenuDelay(pid);
	TSoURDt3rd_OP_AudioDef.lastOn = tsourdt3rd_itemOn;
}

static void M_Sys_EXMusicDrawer(void)
{
	fixed_t x = BASEVIDWIDTH/2, y = currentMenu->y;

	if (exm_found_track_result.lump == NULL)
		return;

	if (tsourdt3rd_itemOn >= op_levels_start && tsourdt3rd_itemOn <= op_levels_end)
		y += ((2*op_levels_end)/4);
	else if (tsourdt3rd_itemOn >= op_bosses_start && tsourdt3rd_itemOn <= op_bosses_end)
		y += ((3*op_bosses_end)/3);
	else if (tsourdt3rd_itemOn >= op_intermission_start && tsourdt3rd_itemOn <= op_intermission_end)
		y += ((4*op_intermission_end)/3);

	if (!menu_music_listening)
	{
		V_DrawCenteredThinString(x, y, V_ALLOWLOWERCASE|V_YELLOWMAP, "Press 'Jump', 'Enter', or 'A' to take a listen!");
		V_DrawCharacter(x+64, y, V_GREENMAP, '\x1D');
	}
	else
	{
		V_DrawCenteredThinString(x, y, V_ALLOWLOWERCASE|V_YELLOWMAP, "Press 'Spin', 'Escape', or 'B' to stop listening!");
	}
}

static void M_Sys_EXMusicTicker(void)
{
	for (INT32 exmusic_menuitems = 0; exmusic_menuitems < currentMenu->numitems; exmusic_menuitems++)
	{
		INT32 identifier_pos = -1;
		consvar_t *cvar = (consvar_t *)currentMenu->menuitems[exmusic_menuitems].itemaction;

		if (cvar == NULL)
			continue;

		if (TSoURDt3rd_EXMusic_ReturnTypeFromCVar(cvar, &identifier_pos) == NULL)
		{
			currentMenu->menuitems[exmusic_menuitems].status = IT_GRAYEDOUT;
			continue;
		}

		if (TSoURDt3rd_EXMusic_FindTrack(cvar->string, identifier_pos, NULL) == false)
		{
			/// \todo: STAR NOTE: Should be CV_Set or something immediate
			COM_BufInsertText(va("%s \"Default:0\"\n", cvar->name));
		}
		currentMenu->menuitems[exmusic_menuitems].status = IT_CVAR|IT_STRING;
	}
	TSoURDt3rd_M_OptionsTick();
}

static void M_Sys_EXMusicInit(void)
{
	INT32 starting_pos = TSoURDt3rd_OP_Audio_EXMusicDef.lastOn;
	INT16 iteration_amount = 0;

	for (; iteration_amount < TSoURDt3rd_OP_Audio_EXMusicDef.numitems; iteration_amount++, starting_pos++)
	{
		INT32 identifier_pos = -1;
		consvar_t *exm_cvar = (consvar_t *)TSoURDt3rd_OP_Audio_EXMusicMenu[iteration_amount].itemaction;

		if (starting_pos >= TSoURDt3rd_OP_Audio_EXMusicDef.numitems)
			starting_pos = 0;

		if (TSoURDt3rd_EXMusic_ReturnTypeFromCVar(exm_cvar, &identifier_pos) == NULL)
		{
			continue;
		}

		if (TSoURDt3rd_EXMusic_FindTrack(exm_cvar->string, identifier_pos, &exm_found_track_result))
		{
			// It's valid! I can finally leave!
			break;
		}
	}

	menu_music_listening = false;
	TSoURDt3rd_OP_Audio_EXMusicDef.lastOn = starting_pos; // Put us right where we left off!
}

static boolean M_Sys_EXMusicQuit(void)
{
	memset(&exm_found_track_result, 0, sizeof(exm_found_track_result));
	tsourdt3rd_noFurtherInput = menu_music_listening = false;
	return true;
}

static boolean M_Sys_EXMusicInput(INT32 choice)
{
	const UINT8 pid = 0;

	(void)choice;

	if (menu_music_listening)
	{
		if (TSoURDt3rd_M_MenuBackPressed(pid))
		{
			// We're no longer listening to our track!
			tsourdt3rd_noFurtherInput = menu_music_listening = false;
			S_StartSoundFromEverywhere(sfx_kc52);
			S_StopMusic();
			TSoURDt3rd_S_RefreshMusic();
			TSoURDt3rd_M_SetMenuDelay(pid);
		}
		return true;
	}

	if (menucmd[pid].dpad_ud < 0 || menucmd[pid].dpad_ud > 0) // up & down
	{
		boolean found_menu_opt = false;
		INT32 identifier_pos = -1;
		consvar_t *exm_cvar;

		if (menucmd[pid].dpad_ud < 0)
			found_menu_opt = TSoURDt3rd_M_PrevOpt();
		else
			found_menu_opt = TSoURDt3rd_M_NextOpt();
		S_StartSoundFromEverywhere(found_menu_opt ? sfx_s3k5b : sfx_lose);

		exm_cvar = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
		if (exm_cvar && TSoURDt3rd_EXMusic_ReturnTypeFromCVar(exm_cvar, &identifier_pos))
			TSoURDt3rd_EXMusic_FindTrack(exm_cvar->string, identifier_pos, &exm_found_track_result);

		menu_music_listening = false;
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (menucmd[pid].dpad_lr < 0 || menucmd[pid].dpad_lr > 0) // left & right
	{
		M_Sys_FindNewEXMusicTrack(menucmd[pid].dpad_lr < 0); // left = -1; +1 otherwise
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (TSoURDt3rd_M_MenuConfirmPressed(pid)) // Try to play our track!
	{
		INT32 lump_track_num = exm_found_track_result.lump_track;
		musicdef_t *def = exm_found_track_result.lump;

		TSoURDt3rd_S_MusicExists(def, lump_track_num, {
			tsourdt3rd_noFurtherInput = menu_music_listening = true;
			S_StartSoundFromEverywhere(sfx_s3k5b);
			S_ChangeMusicInternal(def->name[lump_track_num], !def->stoppingtics);
			S_ControlMusicEffects();
			return true;
		})

		S_StartSoundFromEverywhere(sfx_lose);
		return true;
	}
	else if (TSoURDt3rd_M_MenuExtraPressed(pid))
	{
		INT32 identifier_pos = -1;
		consvar_t *exm_cvar = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
		if (exm_cvar)
		{
			menu_music_listening = false;
			/// \todo: STAR NOTE: Should be CV_Set or something immediate
			COM_BufInsertText(va("%s \"Default:0\"\n", exm_cvar->name));
			TSoURDt3rd_EXMusic_ReturnTypeFromCVar(exm_cvar, &identifier_pos);
			TSoURDt3rd_EXMusic_FindTrack(exm_cvar->string, identifier_pos, &exm_found_track_result);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}

	return false;
}
