// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  defs/smkg-ps_def_exmusdef.c
/// \brief STARParser data for EXMUSDEF lumps

#include "../smkg-ps_main.h"
#include "../../core/smkg-s_exmusic.h"

#include "../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum star_exmusic_term_e
{
	star_exmusic_series,
	star_exmusic_lumps
}; static const char *const star_exmusic_term_opt[] = {
	"SERIES",
	"LUMPS",
	NULL
};

enum star_exmusic_lump_option_term_e
{
	star_exmusic_lump_defaultmaptrack,
	star_exmusic_lump_bosses,
	star_exmusic_lump_intermission,
	star_exmusic_lump_gameover
}; static const char *const star_exmusic_lump_option_term_opt[] = {
	"defaultmaptrack",
	"bosses",
	"intermission",
	"gameover",
	NULL
};

enum star_exmusic_lump_type_boss_e
{
	star_exmusic_lump_boss_track,
	star_exmusic_lump_boss_pinchtrack,
	star_exmusic_lump_boss_finalbosstrack,
	star_exmusic_lump_boss_finalbosspinchtrack,
	star_exmusic_lump_boss_truefinalbosstrack,
	star_exmusic_lump_boss_truefinalbosspinchtrack,
	star_exmusic_lump_boss_racetrack
}; static const char *const star_exmusic_lump_type_boss_term_opt[] = {
	"boss_theme",
	"boss_pinch_theme",
	"final_boss_theme",
	"final_boss_pinch_theme",
	"true_final_boss_theme",
	"true_final_boss_pinch_theme",
	"race_theme",
	NULL
};

enum star_exmusic_lump_type_intermission_e
{
	star_exmusic_lump_intermission_track,
	star_exmusic_lump_intermission_bosstrack,
	star_exmusic_lump_intermission_finalbosstrack,
	star_exmusic_lump_intermission_truefinalbosstrack
}; static const char *const star_exmusic_lump_type_intermission_term_opt[] = {
	"act",
	"boss",
	"final_boss",
	"true_final_boss",
	NULL
};

static const char *const exmusic_identifier_name[] = {
	"Default Map Track",
	"Bosses",
	"Intermission",
	"Game Over",
	NULL
};
static const char *const exmusic_identifier_type[TSOURDT3RD_EXMUSIC_NUM_TYPES][TSOURDT3RD_EXMUSIC_MAX_LUMPS] = {
	[0] = {
		"",
		NULL
	},
	[1] = {
		" (Normal)",
		" (Pinch)",
		" (Final Boss)",
		" (Final Boss Pinch)",
		" (True Final Boss)",
		" (True Final Boss Pinch)",
		" (Race) ",
		NULL
	},
	[2] = {
		" (Normal)",
		" (Boss)",
		" (Final Boss)",
		" (True Final Boss)",
		NULL
	},
	[3] = {
		"",
		NULL
	}
};

// ------------------------ //
//        Functions
// ------------------------ //

static void TSoURDt3rd_EXMUSDEF_CreateSeries(char *series_name, INT32 series_option)
{
	tsourdt3rd_exmusic_t **exmusic_def_all = NULL;
	tsourdt3rd_exmusic_t *exmusic_def = NULL;
	tsourdt3rd_exmusic_t *exmusic_def_prev = NULL;
	size_t *exmusic_type_p = NULL;

#define EXMusic_CreateNewSeries(series) \
	exmusic_def_all = tsourdt3rd_global_exmusic_##series; \
	exmusic_type_p = &tsourdt3rd_num_exmusic_##series##_series;

	switch (series_option)
	{
		case TSOURDT3RD_EXMUSIC_DEFAULTMAPTRACK:
			EXMusic_CreateNewSeries(defaultmaptrack)
			break;
		case TSOURDT3RD_EXMUSIC_BOSSES:
			EXMusic_CreateNewSeries(bosses)
			break;
		case TSOURDT3RD_EXMUSIC_INTERMISSION:
			EXMusic_CreateNewSeries(intermission)
			break;
		case TSOURDT3RD_EXMUSIC_GAMEOVER:
			EXMusic_CreateNewSeries(gameover)
			break;
		default:
			goto end_function;
	}
	if (exmusic_def_all == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "EXMUSDEF wasn't properly initialized at startup, so Series \x82\"%s\"\x80 can't be added!\n", series_name);
		goto end_function;
	}
	exmusic_def = exmusic_def_all[0];

	while (exmusic_def)
	{
		if (!strnicmp(exmusic_def->series, series_name, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "EXMUSDEF: Series \x82\"%s\"\x80 already exists in identifer \x84\"%s\"\x80! I'm not creating a new series\nwith the same exact name as another!\n", exmusic_identifier_name[series_option], series_name);
			goto end_function;
		}
		exmusic_def_prev = exmusic_def;
		exmusic_def = exmusic_def->next;
	}

	if (exmusic_def == NULL)
	{
		exmusic_def = Z_Calloc(sizeof(tsourdt3rd_exmusic_t), PU_STATIC, NULL);
		(*exmusic_type_p)++;
		strlcpy(exmusic_def->series, series_name, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);

		if (exmusic_def_prev != NULL)
		{
			exmusic_def->prev = exmusic_def_prev;
			exmusic_def_prev->next = exmusic_def;
		}

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "EXMUSDEF: Created \x84\"%s\"\x80 Series \x82\"%s\"\x80!\n", exmusic_identifier_name[series_option], series_name);
	}

	exmusic_def_all[(*exmusic_type_p)] = exmusic_def;
	goto end_function;

end_function:
{
	if (series_name != NULL)
	{
		free(series_name);
		series_name = NULL;
	}
	return;
}
}
#undef EXMusic_CreateNewSeries

static void TSoURDt3rd_EXMUSDEF_AddLumpToSeries(char *series_name, const char *lump_name, INT32 lump_option, INT32 lump_type)
{
	tsourdt3rd_exmusic_t **exmusic_def_all = NULL;
	tsourdt3rd_exmusic_t *exmusic_def = NULL;
	musicdef_t *def = musicdefstart;

	switch (lump_option)
	{
		case TSOURDT3RD_EXMUSIC_DEFAULTMAPTRACK:
			exmusic_def_all = tsourdt3rd_global_exmusic_defaultmaptrack;
			break;
		case TSOURDT3RD_EXMUSIC_BOSSES:
			exmusic_def_all = tsourdt3rd_global_exmusic_bosses;
			break;
		case TSOURDT3RD_EXMUSIC_INTERMISSION:
			exmusic_def_all = tsourdt3rd_global_exmusic_intermission;
			break;
		case TSOURDT3RD_EXMUSIC_GAMEOVER:
			exmusic_def_all = tsourdt3rd_global_exmusic_gameover;
			break;
		default:
			goto end_function;
	}
	if (exmusic_def_all == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "EXMUSDEF wasn't properly initialized at startup, so Lump \x82\"%s\"\x80 can't be added!\n", lump_name);
		goto end_function;
	}
	exmusic_def = exmusic_def_all[0];

	while (exmusic_def)
	{
		if (!strnicmp(exmusic_def->series, series_name, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "EXMUSDEF: Found Series \x82\"%s\"\x80!\n", series_name);
			break;
		}
		exmusic_def = exmusic_def->next;
	}
	if (exmusic_def == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "EXMUSDEF: Series \x82\"%s\"\x80 doesn't exist! Did you create it beforehand?\n", series_name);
		goto end_function;
	}

	while (def)
	{
		if (!strnicmp(def->name, lump_name, 7))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "EXMUSDEF: Found pre-existing music definition for Lump \x82\"%s\"\x80!\n", lump_name);
			break;
		}
		def = def->next;
	}
	if (def == NULL)
	{
		// We couldn't find the series, so let's just return...
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "EXMUSDEF: Lump \x82\"%s\"\x80 wasn't found! Did you type the name of the lump correct, or create it beforehand?\n", lump_name);
		goto end_function;
	}

	switch (lump_option)
	{
		case TSOURDT3RD_EXMUSIC_DEFAULTMAPTRACK:
			exmusic_def->lump_slot_1[0] = def;
			break;
		case TSOURDT3RD_EXMUSIC_BOSSES:
			switch (lump_type)
			{
				case star_exmusic_lump_boss_track:
					exmusic_def->lump_slot_1[0] = def;
					break;
				case star_exmusic_lump_boss_pinchtrack:
					exmusic_def->lump_slot_1[1] = def;
					break;
				case star_exmusic_lump_boss_finalbosstrack:
					exmusic_def->lump_slot_2[0] = def;
					break;
				case star_exmusic_lump_boss_finalbosspinchtrack:
					exmusic_def->lump_slot_2[1] = def;
					break;
				case star_exmusic_lump_boss_truefinalbosstrack:
					exmusic_def->lump_slot_3[0] = def;
					break;
				case star_exmusic_lump_boss_truefinalbosspinchtrack:
					exmusic_def->lump_slot_3[1] = def;
					break;
				case star_exmusic_lump_boss_racetrack:
					exmusic_def->lump_slot_4[0] = def;
					break;
				default:
					goto end_function;
			}
			break;
		case TSOURDT3RD_EXMUSIC_INTERMISSION:
			switch (lump_type)
			{
				case star_exmusic_lump_intermission_track:
					exmusic_def->lump_slot_1[0] = def;
					break;
				case star_exmusic_lump_intermission_bosstrack:
					exmusic_def->lump_slot_1[1] = def;
					break;
				case star_exmusic_lump_intermission_finalbosstrack:
					exmusic_def->lump_slot_2[0] = def;
					break;
				case star_exmusic_lump_intermission_truefinalbosstrack:
					exmusic_def->lump_slot_3[0] = def;
					break;
				default:
					goto end_function;
			}
			break;
		case TSOURDT3RD_EXMUSIC_GAMEOVER:
			exmusic_def->lump_slot_1[0] = def;
			break;
		default:
			goto end_function;
	}

	STAR_CONS_Printf(
		STAR_CONS_TSOURDT3RD_NOTICE,
		"EXMUSDEF: \x84\"%s%s\"\x80 lump \x82\"%s\"\x80 can now be found within series \x82\"%s\"\x80!\n",
		exmusic_identifier_name[lump_option], exmusic_identifier_type[lump_option][lump_type],
		lump_name, series_name
	);
	goto end_function;

end_function:
{
	if (series_name != NULL)
	{
		free(series_name);
		series_name = NULL;
	}
	return;
}
}

boolean TSoURDt3rd_STARParser_EXMUSDEF(tsourdt3rd_starparser_t *script)
{
	char *series_name = NULL;
	INT32 series_option = 0;
	INT32 lump_option = 0, lump_type = 0;

	switch (TSoURDt3rd_STARParser_ValidTableTerm(script, star_exmusic_term_opt, true))
	{
		case star_exmusic_series:
		{
			while (script->tkn != NULL)
			{
				series_name = malloc(TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);
				TSoURDt3rd_STARParser_STRLCPY(series_name, script->tkn, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);

				if (strcmp(script->val, ":"))
				{
					TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing operator '\x82:\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					if (series_name != NULL)
					{
						free(series_name);
						series_name = NULL;
					}
					break;
				}

				series_option = TSoURDt3rd_STARParser_ValidTableTerm(script, star_exmusic_lump_option_term_opt, false);
				TSoURDt3rd_EXMUSDEF_CreateSeries(series_name, series_option);

				if (!strcmp(script->val, ","))
				{
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					continue;
				}
				else if (!strcmp(script->val, ";"))
					break;

				TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
				return true;
			}
			return false;
		}
		case star_exmusic_lumps:
		{
			while (script->tkn != NULL)
			{
				lump_type = 0;
				series_name = strdup(script->tkn);
				TSoURDt3rd_STARParser_STRLCPY(series_name, script->tkn, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);

				if (strcmp(script->val, ":"))
				{
					TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing operator '\x82:\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					if (series_name != NULL)
					{
						free(series_name);
						series_name = NULL;
					}
					break;
				}

				// Check for lump options and type...
				lump_option = TSoURDt3rd_STARParser_ValidTableTerm(script, star_exmusic_lump_option_term_opt, false);
				if (lump_option < 0)
				{
					TSoURDt3rd_STARParser_Error(va("EXMUSDEF: Invalid lump option \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
					if (series_name != NULL)
					{
						free(series_name);
						series_name = NULL;
					}
					break;
				}

				if (!strcmp(script->val, ":"))
				{
					boolean checked_lump_type = false;
					switch (lump_option)
					{
						case TSOURDT3RD_EXMUSIC_BOSSES:
							lump_type = TSoURDt3rd_STARParser_ValidTableTerm(script, star_exmusic_lump_type_boss_term_opt, false);
							checked_lump_type = true;
							break;
						case TSOURDT3RD_EXMUSIC_INTERMISSION:
							lump_type = TSoURDt3rd_STARParser_ValidTableTerm(script, star_exmusic_lump_type_intermission_term_opt, false);
							checked_lump_type = true;
							break;
						default:
							script->tkn = script->tokenizer->get(script->tokenizer, 0);
							script->val = script->tokenizer->get(script->tokenizer, 1);
							break;
					}
					if (checked_lump_type && lump_type < 0)
					{
						TSoURDt3rd_STARParser_Error(va("EXMUSDEF: Invalid type option \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
						break;
					}
				}
				if (strcmp(script->val, "="))
				{
					TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing operator '\x82=\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					if (series_name != NULL)
					{
						free(series_name);
						series_name = NULL;
					}
					break;
				}
				script->val = script->tokenizer->get(script->tokenizer, 1);

				// Now add our lump to the series, and we can close this out!
				TSoURDt3rd_EXMUSDEF_AddLumpToSeries(series_name, script->val, lump_option, lump_type);

				script->val = script->tokenizer->get(script->tokenizer, 1);
				if (!strcmp(script->val, ":"))
				{
					script->val = script->tokenizer->get(script->tokenizer, 1);
					continue;
				}
				else if (!strcmp(script->val, ",") || !strcmp(script->val, ";"))
				{
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					if (TSoURDt3rd_STARParser_CheckForBrackets(script) == TSOURDT3RD_STARPARSER_BRAK_CLOSE)
						break;
				}
				else
				{
					TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					return true;
				}
			}
			return false;
		}
		default:
			TSoURDt3rd_STARParser_Error(va("EXMUSDEF: Invalid field \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
			break;
	}

	return true;
}
