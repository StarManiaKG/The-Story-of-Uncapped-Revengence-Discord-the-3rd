// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  defs/smkg-ps_def_exmusdef.c
/// \brief STARParser data for EXMUSDEF lumps

#include "../smkg-ps_main.h"
#include "../../core/smkg-s_exmusic.h"

enum star_exmusic_term_e
{
	star_exmusic_series,
	star_exmusic_lumps
}; static const char *const star_exmusic_term_opt[] = {
	"SERIES",
	"LUMPS",
	NULL
};

static void TSoURDt3rd_EXMUSDEF_AddLumpToSeries(musicdef_t *def, char *series_name, INT32 musicset_type, tsourdt3rd_exmusic_data_identifiers_t *identifier)
{
	tsourdt3rd_exmusic_data_series_t *exm_series;
	tsourdt3rd_exmusic_musicset_t *exm_series_musicset;
	INT32 series_num = 0, music_track = 0, lump_track;

	exm_series = TSoURDt3rd_EXMusic_FindSeries(series_name, &series_num);
	if (exm_series == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "EXMUSDEF: Series \x82\"%s\"\x80 doesn't exist! Did you create it beforehand?\n", series_name);
		return;
	}
	else if (exm_series->hardcoded)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "EXMUSDEF: You can't modify this series!\n");
		return;
	}

	exm_series_musicset = exm_series->track_sets[musicset_type];
	while (music_track < exm_series_musicset->num_music_lumps)
	{
		for (lump_track = 0; lump_track < def->numtracks; lump_track++)
		{
			if (*def->name[lump_track] == '\0')
			{
				continue;
			}
			else if (!strnicmp(exm_series_musicset->music[music_track]->name[lump_track], def->name[lump_track], TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME))
			{
				STAR_CONS_Printf(STAR_CONS_DEBUG, "EXMUSDEF: Duplicate music lump found! \x82\"%s (%d)\"\x80!\n", def->name[lump_track], series_num);
				return;
			}
		}
		music_track++;
	}
	exm_series_musicset->num_music_lumps++;
	exm_series_musicset->music = Z_Realloc(exm_series_musicset->music, sizeof(*exm_series_musicset->music) * exm_series_musicset->num_music_lumps, PU_STATIC, NULL);
	exm_series_musicset->music[music_track] = def;

#if 0
	tsourdt3rd_exmusic_available_series[series_num]->track_sets[musicset_type] = exm_series_musicset;
	tsourdt3rd_exmusic_available_series[series_num]->track_sets[musicset_type]->music = exm_series_musicset->music;
	tsourdt3rd_exmusic_available_series[series_num]->track_sets[musicset_type]->music[music_track] = def;
#endif
#if 1
	tsourdt3rd_exmusic_available_series[series_num]->track_sets[musicset_type] = exm_series_musicset;
#endif
#if 0
	tsourdt3rd_exmusic_available_series[series_num] = exm_series;
#endif

	STAR_CONS_Printf(STAR_CONS_NOTICE,
		"EXMUSDEF: \x84\"%s\"\x80 lump \x82\"%s\"\x80 can now be found within series \x82\"%s\"\x80!\n",
		identifier->type_name, def->name[0], series_name
	);
}

boolean TSoURDt3rd_STARParser_EXMUSDEF(tsourdt3rd_starparser_t *script)
{
	tsourdt3rd_exmusic_data_identifiers_t *musicset = NULL;
	musicdef_t *def;
	char *series_name;
	char *lump_identifier, *lump_type_string;
	INT32 musicset_type;

	switch (TSoURDt3rd_STARParser_ValidTableTerm(script, star_exmusic_term_opt, true))
	{
		case star_exmusic_series:
		{
			while (script->tkn != NULL)
			{
				series_name = malloc(TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);
				TSoURDt3rd_STARParser_STRLCPY(series_name, script->tkn, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);

				TSoURDt3rd_EXMusic_AddNewSeries(series_name, true);
				if (series_name != NULL)
				{
					free(series_name);
					series_name = NULL;
				}

				if (!strcmp(script->val, ","))
				{
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					continue;
				}
				else if (!strcmp(script->val, ";"))
				{
					break;
				}

				TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
				return true;
			}

			return false;
		}
		case star_exmusic_lumps:
		{
			while (script->tkn != NULL)
			{
				musicset = NULL;
				musicset_type = -1;

				if (strcmp(script->val, ":"))
				{
					TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing operator '\x82:\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					break;
				}

				series_name = strdup(script->tkn);
				TSoURDt3rd_STARParser_STRLCPY(series_name, script->tkn, TSOURDT3RD_EXMUSIC_MAX_SERIES_NAME);

				script->tkn = script->tokenizer->get(script->tokenizer, 0);
				script->val = script->tokenizer->get(script->tokenizer, 1);
				if (script->tkn == NULL)
				{
					TSoURDt3rd_STARParser_Error(va("EXMUSDEF: Invalid lump option string \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
					break;
				}

				lump_identifier = strdup(script->tkn);
				lump_type_string = strdup(lump_identifier);

				if (!strcmp(script->val, ":"))
				{
					// Check for lump type...
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					lump_type_string = strdup(script->tkn);
				}
				if (strcmp(script->val, "="))
				{
					TSoURDt3rd_STARParser_Error("EXMUSDEF: Missing operator '\x82=\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					break;
				}
				script->val = script->tokenizer->get(script->tokenizer, 1);
				def = S_FindMusicDef(script->val, NULL, NULL, NULL);

				if (def == NULL)
				{
					// We couldn't even find the lump! That's the core part here!
					STAR_CONS_Printf(STAR_CONS_ERROR, "EXMUSDEF: Lump \x82\"%s\"\x80 wasn't found! Did you type the name of the lump correct, or create it beforehand?\n", script->val);
				}
				else
				{
					// Now add our lump to the series, and we can close this out!
					musicset = TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier(lump_type_string, &musicset_type);
					if (musicset == NULL)
						musicset = TSoURDt3rd_EXMusic_ReturnTypeFromIdentifier(lump_identifier, &musicset_type);

					if (musicset != NULL)
					{
						// ...Identifier found, we can add the lump now.
						TSoURDt3rd_EXMUSDEF_AddLumpToSeries(def, series_name, musicset_type, musicset);
					}
					else
					{
						// ...Identifier not found? We kinda need that...
						STAR_CONS_Printf(STAR_CONS_ERROR, "EXMUSDEF: Identifiers %s,%s weren't found!\n", lump_type_string, lump_identifier);
					}
				}

				if (series_name != NULL)
				{
					free(series_name);
					series_name = NULL;
				}
				if (lump_identifier != NULL)
				{
					free(lump_identifier);
					lump_identifier = NULL;
				}
				if (lump_type_string != NULL)
				{
					free(lump_type_string);
					lump_type_string = NULL;
				}

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
			if (series_name != NULL)
			{
				free(series_name);
				series_name = NULL;
			}
			return false;
		}
		default:
			TSoURDt3rd_STARParser_Error(va("EXMUSDEF: Invalid field \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
			break;
	}

	return true;
}
