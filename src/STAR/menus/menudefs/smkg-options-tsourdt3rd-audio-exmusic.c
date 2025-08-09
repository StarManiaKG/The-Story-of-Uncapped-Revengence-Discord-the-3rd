// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-audio-exmusic.c
/// \brief TSoURDt3rd's EXMusic menu options

#include "../smkg-m_sys.h"
#include "../../core/smkg-s_exmusic.h"

// ------------------------ //
//        Variables
// ------------------------ //

static tsourdt3rd_exmusic_t **exmusic_data = NULL;
static tsourdt3rd_exmusic_t *exmusic_def = NULL;

static consvar_t *excvar = NULL;
static INT32 exmusic_option = 0;

static boolean exmusic_listening_track = false;

static void M_Sys_EXMusicDrawer(void);
static void M_Sys_EXMusicTicker(void);
static void M_Sys_EXMusicInit(void);
static boolean M_Sys_EXMusicInput(INT32 choice);

menuitem_t TSoURDt3rd_OP_Audio_EXMusicMenu[] =
{
	{IT_HEADER, NULL, "Levels", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Default Map Track",
			&cv_tsourdt3rd_audio_exmusic_defaultmaptrack, 0},
		{IT_STRING | IT_CVAR, NULL, "Game Over Music",
			&cv_tsourdt3rd_audio_exmusic_gameover, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Bosses", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Normal Bosses",
			&cv_tsourdt3rd_audio_exmusic_bosses, 0},
		{IT_STRING | IT_CVAR, NULL, "Pinch Music",
			&cv_tsourdt3rd_audio_exmusic_bosspinch, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Final Bosses",
			&cv_tsourdt3rd_audio_exmusic_finalbosses, 0},
		{IT_STRING | IT_CVAR, NULL, "Pinch Music",
			&cv_tsourdt3rd_audio_exmusic_finalbosspinch, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "True Final Bosses",
			&cv_tsourdt3rd_audio_exmusic_truefinalbosses, 0},
		{IT_STRING | IT_CVAR, NULL, "Pinch Music",
			&cv_tsourdt3rd_audio_exmusic_truefinalbosspinch, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Race Bosses",
			&cv_tsourdt3rd_audio_exmusic_racebosses, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Intermission", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Normal",
			&cv_tsourdt3rd_audio_exmusic_intermission, 0},
		{IT_STRING | IT_CVAR, NULL, "Bosses",
			&cv_tsourdt3rd_audio_exmusic_intermission_bosses, 0},
		{IT_STRING | IT_CVAR, NULL, "Final Bosses",
			&cv_tsourdt3rd_audio_exmusic_intermission_finalbosses, 0},
		{IT_STRING | IT_CVAR, NULL, "True Final Bosses",
			&cv_tsourdt3rd_audio_exmusic_intermission_truefinalbosses, 0}
};

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
	MTREE4(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_Audio_EXMusicMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_AudioDef,
	TSoURDt3rd_OP_Audio_EXMusicMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	24, 64,
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
	NULL,
	M_Sys_EXMusicInput,
	&TSoURDt3rd_TM_OP_AudioDef
};

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_M_EXMusic_LoadMenu(INT32 choice)
{
	const UINT8 pid = 0;

	tsourdt3rd_exmusic_t *exdef = NULL;
	consvar_t *cvar = (consvar_t *)TSoURDt3rd_TM_OP_Audio_EXMusicMenu[TSoURDt3rd_OP_Audio_EXMusicDef.lastOn].itemaction.cvar;

	(void)choice;

	if (cvar == NULL)
	{
		// Uh sorry, can't do that.
		goto no_entry;
	}

	TSoURDt3rd_EXMusic_ReturnType(cvar, exdef);
	if (exdef == NULL)
	{
		// Still can't do that, sorry.
		goto no_entry;
	}

	TSoURDt3rd_TM_OP_Audio_EXMusicDef.music = tsourdt3rd_currentMenu->music;
	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_Audio_EXMusicDef, &TSoURDt3rd_OP_Audio_EXMusicDef, false);
	TSoURDt3rd_M_SetMenuDelay(pid);
	TSoURDt3rd_OP_AudioDef.lastOn = tsourdt3rd_itemOn;

no_entry:
{
	// Aw man, nothing's valid...
	TSoURDt3rd_M_StartMessage(
		"TSoURDt3rd EXMusic",
		M_GetText("The data needed for EXMusic wasn't initialized.\n"),
		NULL,
		MM_NOTHING,
		NULL,
		NULL
	);
	return;
}

}

static void M_Sys_EXMusicDrawer(void)
{
	fixed_t x = BASEVIDWIDTH/2, y = currentMenu->y;

	if (tsourdt3rd_itemOn >= op_levels_start && tsourdt3rd_itemOn <= op_levels_end)
		y += ((2*op_levels_end)/4);
	else if (tsourdt3rd_itemOn >= op_bosses_start && tsourdt3rd_itemOn <= op_bosses_end)
		y += ((3*op_bosses_end)/3);
	else if (tsourdt3rd_itemOn >= op_intermission_start && tsourdt3rd_itemOn <= op_intermission_end)
		y += ((4*op_intermission_end)/3);

	if (!exmusic_listening_track)
	{
		V_DrawCenteredThinString(x, y, V_ALLOWLOWERCASE|V_YELLOWMAP, "Press 'Jump', 'Enter', or 'A' to take a listen!");
		V_DrawCharacter(x+64, y, V_GREENMAP, '\x1D');
	}
	else
		V_DrawCenteredThinString(x, y, V_ALLOWLOWERCASE|V_YELLOWMAP, "Press 'Spin', 'Escape', or 'B' to stop listening!");
}

static void M_Sys_EXMusicTicker(void)
{
	for (INT32 exmusic_menuitems = 0; exmusic_menuitems < currentMenu->numitems; exmusic_menuitems++)
	{
		tsourdt3rd_exmusic_t *cur_extype = NULL;
		consvar_t *cur_excvar = (consvar_t *)currentMenu->menuitems[exmusic_menuitems].itemaction;
		INT32 menustatus = IT_GRAYEDOUT;

		if (cur_excvar == NULL)
		{
			// We skip these in this household.
			continue;
		}

		TSoURDt3rd_EXMusic_ReturnType(cur_excvar, cur_extype);
		if (cur_extype == NULL || (cur_excvar->value > (INT32)cur_extype->data->num_series))
		{
			COM_BufAddText(va("%s \"0\"\n", excvar->name));
			currentMenu->menuitems[exmusic_menuitems].status = IT_GRAYEDOUT;
			continue;
		}

		//cur_extype = exmusic_cur_data[cur_excvar->value];
		if (TSoURDt3rd_EXMusic_DoesDefHaveValidLump(cur_extype, cur_excvar, true))
			menustatus = (IT_CVAR|IT_STRING);
		currentMenu->menuitems[exmusic_menuitems].status = menustatus;
	}
	TSoURDt3rd_M_OptionsTick();
}

static void M_Sys_EXMusicInit(void)
{
	INT32 starting_pos = TSoURDt3rd_OP_Audio_EXMusicDef.lastOn;
	INT16 iteration_amount = 0;

	exmusic_listening_track = false;

	// Put us right where we left off!
	if (starting_pos >= TSoURDt3rd_OP_Audio_EXMusicDef.numitems)
		starting_pos = 0;
	for (; iteration_amount < TSoURDt3rd_OP_Audio_EXMusicDef.numitems; iteration_amount++, starting_pos++)
	{
		excvar = (consvar_t *)TSoURDt3rd_OP_Audio_EXMusicMenu[starting_pos].itemaction;
		if (excvar == NULL)
		{
			// Invalid.
			continue;
		}

		TSoURDt3rd_EXMusic_ReturnType(excvar, exmusic_def);
#if 0
		exmusic_def = exmusic_data[excvar->value];
		if (exmusic_def != NULL)
		{
			// It's valid! I can finally leave!
			break;
		}
#endif
	}

	TSoURDt3rd_OP_Audio_EXMusicDef.lastOn = starting_pos; // Gets reset later, so just in case...
}

static void M_Sys_FindNewEXMusicTrack(boolean decrease)
{
	tsourdt3rd_exmusic_t *exmusic_cur_def = exmusic_def;

	while (exmusic_cur_def)
	{
		if (decrease)
			exmusic_cur_def = ((exmusic_cur_def->prev == NULL) ? exmusic_data[exmusic_def->data->num_series] : exmusic_cur_def->prev);
		else
			exmusic_cur_def = ((exmusic_cur_def->next == NULL) ? exmusic_data[0] : exmusic_cur_def->next);

		if (exmusic_cur_def == NULL || exmusic_cur_def == exmusic_def)
		{
			// Oh no..... What just happened? What did we do?
			break;
		}

#if 0
		if (TSoURDt3rd_EXMusic_DoesDefHaveValidLump(exmusic_cur_def, exmusic_option))
		{
			// This is valid, so we can leave now!
			exmusic_def = exmusic_cur_def;
			//COM_BufAddText(va("%s \"%s\"\n", excvar->name, exmusic_cur_def->series));
			S_StartSound(NULL, sfx_s3k5b);
			return;
		}
#endif
	}
	S_StartSound(NULL, sfx_lose);
}

static boolean M_Sys_EXMusicInput(INT32 choice)
{
	const UINT8 pid = 0;

	(void)choice;

#define SET_EXMUSIC_TO_PLAY(def) \
	TSoURDt3rd_S_MusicExists(def, { \
		S_ChangeMusicInternal(def->name, !def->stoppingtics); \
		tsourdt3rd_noFurtherInput = exmusic_listening_track = true; \
		return exmusic_listening_track; \
	})

	if (exmusic_listening_track)
	{
		if (TSoURDt3rd_M_MenuBackPressed(pid))
		{
			// We're no longer listening to our track!
			S_StopMusic();
			TSoURDt3rd_S_RefreshMusic();
			TSoURDt3rd_M_SetMenuDelay(pid);
			tsourdt3rd_noFurtherInput = exmusic_listening_track = false;
		}
		return true;
	}

	if (menucmd[pid].dpad_ud < 0 || menucmd[pid].dpad_ud > 0) // up & down
	{
		if (menucmd[pid].dpad_ud < 0)
			TSoURDt3rd_M_PrevOpt();
		else
			TSoURDt3rd_M_NextOpt();
		S_StartSound(NULL, sfx_s3k5b);

		excvar = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
		TSoURDt3rd_EXMusic_ReturnType(excvar, exmusic_def);
#if 0
		if (exmusic_data != NULL)
		{
			// -- We can set this definition!
			exmusic_def = exmusic_data[excvar->value];
		}
#endif

		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (menucmd[pid].dpad_lr < 0 || menucmd[pid].dpad_lr > 0) // left & right
	{
		// Let's set ourselves to a different new track then!
		M_Sys_FindNewEXMusicTrack(menucmd[pid].dpad_lr < 0); // left = -1; +1 otherwise
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (TSoURDt3rd_M_MenuConfirmPressed(pid))
	{
		// Try to play our track!
		if (exmusic_def->play_routine == NULL) return false;
		exmusic_def->play_routine(NULL);
#if 0
		switch (exmusic_option)
		{
			case TSOURDT3RD_EXMUSIC_CVAR_BOSSPINCH:
			case TSOURDT3RD_EXMUSIC_CVAR_INTERMISSION_BOSSES:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_1[1]);
				break;
			case TSOURDT3RD_EXMUSIC_CVAR_FINALBOSSES:
			case TSOURDT3RD_EXMUSIC_CVAR_INTERMISSION_FINALBOSSES:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_2[0]);
				break;
			case TSOURDT3RD_EXMUSIC_CVAR_FINALBOSSPINCH:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_2[1]);
				break;
			case TSOURDT3RD_EXMUSIC_CVAR_TRUEFINALBOSSES:
			case TSOURDT3RD_EXMUSIC_CVAR_INTERMISSION_TRUEFINALBOSSES:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_3[0]);
				break;
			case TSOURDT3RD_EXMUSIC_CVAR_TRUEFINALBOSSPINCH:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_3[1]);
				break;
			case TSOURDT3RD_EXMUSIC_CVAR_RACEBOSSES:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_4[0]);
				break;
			default:
				SET_EXMUSIC_TO_PLAY(exmusic_def->lump_slot_1[0]);
				break;
		}
#endif

		// If we made it here, then we can't play this track.
		S_StartSound(NULL, sfx_lose);
		return true;
	}

	return false;
}
#undef SET_EXMUSIC_TO_PLAY
