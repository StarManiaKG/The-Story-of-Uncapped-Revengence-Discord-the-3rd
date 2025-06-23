// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-audio.c
/// \brief TSoURDt3rd's EXMusic menu options

#include "../smkg-m_sys.h"
#include "../../core/smkg-s_audio.h"
#include "../../core/smkg-s_exmusic.h"

// ------------------------ //
//        Variables
// ------------------------ //

static tsourdt3rd_exmusic_t **exmusic_data = NULL;
static tsourdt3rd_exmusic_t *exmusic_def = NULL;

static consvar_t *exmusic_cvar = NULL;
static size_t exmusic_series_size = 0;
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
		{NULL, "Maps that don't have tracks selected will play this instead.", {NULL}, TSOURDT3RD_EXMUSIC_DEFAULTMAPTRACK, 0},
		{NULL, "Music that plays when you get a game over.", {NULL}, TSOURDT3RD_EXMUSIC_GAMEOVER, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Music that plays when a boss is present within a level.", {NULL}, TSOURDT3RD_EXMUSIC_BOSSES, 0},
		{NULL, "The above option, but the music changes if the boss is on its last legs.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_BOSSPINCH, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when you fight the final boss.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_FINALBOSSES, 0},
		{NULL, "The above option, but the music changes if the boss is on its last legs.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_FINALBOSSPINCH, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when you fight the true final boss.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_TRUEFINALBOSSES, 0},
		{NULL, "The above option, but the music changes if the boss is on its last legs.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_TRUEFINALBOSSPINCH, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Music that plays when a race is present within a level.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_RACEBOSSES, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Music that plays once you've completed an act.", {NULL}, TSOURDT3RD_EXMUSIC_INTERMISSION, 0},
		{NULL, "Music that plays once you've beaten a boss.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_INTERMISSION_BOSSES, 0},
		{NULL, "Music that plays once you've beaten the final boss.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_INTERMISSION_FINALBOSSES, 0},
		{NULL, "Music that plays once you've beaten the true final boss.", {NULL}, TSOURDT3RD_EXMUSIC_CVAR_INTERMISSION_TRUEFINALBOSSES, 0},
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
	tsourdt3rd_exmusic_t **exmusic_def_p = NULL;
	boolean can_enter_exmusic_menu = false;

	(void)choice;
	TSoURDt3rd_TM_OP_Audio_EXMusicDef.music = tsourdt3rd_currentMenu->music;

	for (INT32 exmusic_type = 0; exmusic_type < TSOURDT3RD_EXMUSIC_MAIN_TYPES; exmusic_type++)
	{
		exmusic_def_p = TSoURDt3rd_S_EXMusic_ReturnDefFromType(exmusic_type, NULL);
		if (exmusic_def_p != NULL)
		{
			can_enter_exmusic_menu = true;
			break;
		}
	}

	if (!can_enter_exmusic_menu)
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

	TSoURDt3rd_M_SetupNextMenu(&TSoURDt3rd_TM_OP_Audio_EXMusicDef, &TSoURDt3rd_OP_Audio_EXMusicDef, false);
	TSoURDt3rd_M_SetMenuDelay(pid);
	TSoURDt3rd_OP_AudioDef.lastOn = tsourdt3rd_itemOn;
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
	tsourdt3rd_exmusic_t **exmusic_cur_data = NULL;
	tsourdt3rd_exmusic_t *exmusic_cur_def = NULL;

	consvar_t *exmusic_cur_cvar = NULL;
	size_t exmusic_curdef_all_series = 0;
	INT32 exmusic_cur_option = 0;

	TSoURDt3rd_M_OptionsTick();

	for (INT32 exmusic_menuitems = 0; exmusic_menuitems < currentMenu->numitems; exmusic_menuitems++)
	{
		exmusic_cur_option = tsourdt3rd_currentMenu->menuitems[exmusic_menuitems].mvar1;
		exmusic_cur_cvar = (consvar_t *)currentMenu->menuitems[exmusic_menuitems].itemaction;

		if (exmusic_cur_cvar == NULL)
		{
			// We skip these in this household.
			continue;
		}

		exmusic_cur_data = TSoURDt3rd_S_EXMusic_ReturnDefFromType(exmusic_cur_option, &exmusic_curdef_all_series);

		if (exmusic_cur_data == NULL || (exmusic_cur_cvar->value > (INT32)exmusic_curdef_all_series))
		{
			COM_BufAddText(va("%s \"0\"\n", exmusic_cvar->name));
			currentMenu->menuitems[exmusic_menuitems].status = IT_GRAYEDOUT;
			continue;
		}

		exmusic_cur_def = exmusic_cur_data[exmusic_cur_cvar->value];
		currentMenu->menuitems[exmusic_menuitems].status = (TSoURDt3rd_S_EXMusic_DoesDefHaveValidLump(exmusic_cur_def, exmusic_cur_option) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	}
}

static void M_Sys_EXMusicInit(void)
{
	INT32 starting_pos = TSoURDt3rd_OP_Audio_EXMusicDef.lastOn;
	INT16 iteration_amount = 0;

	// Put us right where we left off!
	for (; iteration_amount < TSoURDt3rd_OP_Audio_EXMusicDef.numitems; iteration_amount++, starting_pos++)
	{
		if (starting_pos >= TSoURDt3rd_OP_Audio_EXMusicDef.numitems)
			starting_pos = 0;

		exmusic_cvar = (consvar_t *)TSoURDt3rd_OP_Audio_EXMusicMenu[starting_pos].itemaction;
		exmusic_option = TSoURDt3rd_TM_OP_Audio_EXMusicMenu[starting_pos].mvar1;

		exmusic_data = TSoURDt3rd_S_EXMusic_ReturnDefFromType(exmusic_option, NULL);
		if (exmusic_data == NULL || exmusic_cvar == NULL)
		{
			// Invalid.
			continue;
		}

		exmusic_def = exmusic_data[exmusic_cvar->value];
		if (exmusic_def != NULL)
		{
			// It's valid! I can finally leave!
			break;
		}
	}

	TSoURDt3rd_OP_Audio_EXMusicDef.lastOn = starting_pos; // Gets reset later, so just in case...
	exmusic_listening_track = false;
}

static void M_Sys_FindNewEXMusicTrack(boolean decrease)
{
	tsourdt3rd_exmusic_t *exmusic_cur_def = exmusic_def;

	while (exmusic_cur_def)
	{
		if (decrease)
		{
			if (exmusic_cur_def->prev == NULL)
				exmusic_cur_def = exmusic_data[exmusic_series_size];
			else
				exmusic_cur_def = exmusic_cur_def->prev;
		}
		else
		{
			if (exmusic_cur_def->next == NULL)
				exmusic_cur_def = exmusic_data[0];
			else
				exmusic_cur_def = exmusic_cur_def->next;
		}

		if (exmusic_cur_def == NULL || exmusic_cur_def == exmusic_def)
		{
			// Oh no..... What just happened? What did we do?
			break;
		}

		if (TSoURDt3rd_S_EXMusic_DoesDefHaveValidLump(exmusic_cur_def, exmusic_option))
		{
			// This is valid, so we can leave now!
			exmusic_def = exmusic_cur_def;
			COM_BufAddText(va("%s \"%s\"\n", exmusic_cvar->name, exmusic_cur_def->series));
			S_StartSound(NULL, sfx_s3k5b);
			return;
		}
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

		exmusic_cvar = (consvar_t *)currentMenu->menuitems[tsourdt3rd_itemOn].itemaction;
		exmusic_option = tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1;

		exmusic_data = TSoURDt3rd_S_EXMusic_ReturnDefFromType(exmusic_option, &exmusic_series_size);
		if (exmusic_data != NULL)
		{
			// -- We can set this definition!
			exmusic_def = exmusic_data[exmusic_cvar->value];
		}

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

		// If we made it here, then we can't play this track.
		S_StartSound(NULL, sfx_lose);
		return true;
	}

	return false;
}
#undef SET_EXMUSIC_TO_PLAY
