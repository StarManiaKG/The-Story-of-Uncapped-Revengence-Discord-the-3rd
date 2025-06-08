// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_exmusic.c
/// \brief TSoURDt3rd's cool and groovy music setup, definition, and replacement routines

#include "smkg-s_exmusic.h"
#include "../ss_main.h"

#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_exmusic_t tsourdt3rd_exmusic_default_def = {
	"Default",
	{ &soundtestsfx, NULL },
	NULL,
	{ NULL, NULL },
	{ NULL, NULL },
	NULL,
	NULL
};
tsourdt3rd_exmusic_t **tsourdt3rd_global_exmusic_defaultmaptrack = NULL;
tsourdt3rd_exmusic_t **tsourdt3rd_global_exmusic_boss = NULL;
tsourdt3rd_exmusic_t **tsourdt3rd_global_exmusic_intermission = NULL;
tsourdt3rd_exmusic_t **tsourdt3rd_global_exmusic_gameover = NULL;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_S_EXMusic_Init(void)
// Initializes the EXMusic data.
//
void TSoURDt3rd_S_EXMusic_Init(void)
{
	if ((tsourdt3rd_global_exmusic_defaultmaptrack = Z_Malloc(TSOURDT3RD_MAX_EXMUSIC_DEFAULTMAPTRACK_LUMPS * sizeof(tsourdt3rd_exmusic_t *), PU_STATIC, NULL)))
	{
		tsourdt3rd_global_exmusic_defaultmaptrack[0] = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
		memcpy(tsourdt3rd_global_exmusic_defaultmaptrack[0], &tsourdt3rd_exmusic_default_def, sizeof(tsourdt3rd_exmusic_default_def));
	}
	else
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "TSoURDt3rd_Init(): Could not allocate exmusic defaultmaptrack defs.");

	if ((tsourdt3rd_global_exmusic_boss = Z_Malloc(TSOURDT3RD_MAX_EXMUSIC_BOSS_LUMPS * sizeof(tsourdt3rd_exmusic_t *), PU_STATIC, NULL)))
	{
		tsourdt3rd_global_exmusic_boss[0] = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
		memcpy(tsourdt3rd_global_exmusic_boss[0], &tsourdt3rd_exmusic_default_def, sizeof(tsourdt3rd_exmusic_default_def));
	}
	else
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "TSoURDt3rd_Init(): Could not allocate exmusic boss defs.");

	if ((tsourdt3rd_global_exmusic_intermission = Z_Malloc(TSOURDT3RD_MAX_EXMUSIC_INTERMISSION_LUMPS * sizeof(tsourdt3rd_exmusic_t *), PU_STATIC, NULL)))
	{
		tsourdt3rd_global_exmusic_intermission[0] = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
		memcpy(tsourdt3rd_global_exmusic_intermission[0], &tsourdt3rd_exmusic_default_def, sizeof(tsourdt3rd_exmusic_default_def));
	}
	else
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "TSoURDt3rd_Init(): Could not allocate exmusic intermission defs.");

	if ((tsourdt3rd_global_exmusic_gameover = Z_Malloc(TSOURDT3RD_MAX_EXMUSIC_GAMEOVER_LUMPS * sizeof(tsourdt3rd_exmusic_t *), PU_STATIC, NULL)))
	{
		tsourdt3rd_global_exmusic_gameover[0] = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
		memcpy(tsourdt3rd_global_exmusic_gameover[0], &tsourdt3rd_exmusic_default_def, sizeof(tsourdt3rd_exmusic_default_def));
	}
	else
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "TSoURDt3rd_Init(): Could not allocate exmusic gameover defs.");
}

//
// boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr)
// Updates the EXMusic data.
//
boolean TSoURDt3rd_S_EXMusic_CanUpdate(const char *valstr)
{
	tsourdt3rd_exmusic_t **exmusic_data = NULL;
	tsourdt3rd_exmusic_t *exmusic_def = NULL;

	static consvar_t *cvar_table[] = {
		&cv_tsourdt3rd_audio_exmusic_defaultmaptrack,
		&cv_tsourdt3rd_audio_exmusic_bosses,
		&cv_tsourdt3rd_audio_exmusic_intermission,
		&cv_tsourdt3rd_audio_exmusic_gameover,
		NULL
	};

	consvar_t *cvar = CV_FindVar(COM_Argv(0));
	const INT32 valstr_num = atoi(valstr);

	INT32 type = 0;
	size_t exmusic_pos = 0;
	size_t all_exmusic_series = 0;

	if (cvar == NULL)
		return false;

	while (type < NUM_EXMUSIC_TYPES)
	{
		if (!strcmp(cvar->name, cvar_table[type]->name))
			break;
		type++;
	}
	if (type >= NUM_EXMUSIC_TYPES)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "EXMusic - Invalid option \x82\"%s\"\x80.\n", cvar->name);
		return false;
	}

	switch (type)
	{
		case TSOURDT3RD_EXMUSIC_DEFAULTMAPTRACK:
			exmusic_data = tsourdt3rd_global_exmusic_defaultmaptrack;
			break;
		case TSOURDT3RD_EXMUSIC_BOSS:
			exmusic_data = tsourdt3rd_global_exmusic_boss;
			break;
		case TSOURDT3RD_EXMUSIC_INTERMISSION:
			exmusic_data = tsourdt3rd_global_exmusic_intermission;
			break;
		case TSOURDT3RD_EXMUSIC_GAMEOVER:
			exmusic_data = tsourdt3rd_global_exmusic_gameover;
			break;
		default:
			return false;
	}
	if (exmusic_data == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "EXMUSDEF wasn't properly initialized at startup, so this command can't be modified!\n");
		return false;
	}

	exmusic_def = exmusic_data[0];
	while (exmusic_def)
	{
		all_exmusic_series++;
		exmusic_def = exmusic_def->next;
	}
	if (all_exmusic_series <= 0)
		return false;

	if (valstr_num > 0 || !strcmp(valstr, "0"))
	{
		// Set our EXMusic command based on the number.
		exmusic_pos = (size_t)valstr_num;
		exmusic_def = exmusic_data[exmusic_pos];
	}
	else
	{
		// Set our EXMusic command based on the string.
		exmusic_def = exmusic_data[0];
		while (exmusic_def)
		{
			if (!stricmp(exmusic_def->series, valstr))
				break;
			exmusic_def = exmusic_def->next;
			exmusic_pos++;
		}
	}

	// Now just input our values, and we're done!
	if ((exmusic_pos >= all_exmusic_series) || exmusic_def == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "EXMusic - \x82\"%s\"\x80 couldn't find Series \x82\"%s\"\x80, not changing.\n", cvar->name, valstr);
		return false;
	}

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "EXMusic - \x82\"%s\"\x80 has been set to Series \x82\"%s\"\x80\n", cvar->name, exmusic_def->series);
	cvar->string = exmusic_def->series;
	cvar->value = exmusic_pos;
	return true;
}
