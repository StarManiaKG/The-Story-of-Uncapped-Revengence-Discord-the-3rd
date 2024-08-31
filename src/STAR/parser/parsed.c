// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  parsed.c
/// \brief Contains STAR script property parsing functions

#include "smkg-script.h"

#include "../smkg-jukebox.h"
#include "../star_vars.h"

#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum star_jukebox_term_e
{
	star_jukebox_pagetitles,
	star_jukebox_lump,
};

enum star_jukebox_lump_term_e
{
	star_jukebox_lump_jukeboxpage,
};

static const char *const star_jukebox_term_opt[] = {
	"PAGETITLES",
	"LUMP",
	NULL
};

static const char *const star_jukebox_lump_term_opt[] = {
	"jukeboxpage",
	NULL,
};

// ------------------------ //
//        Functions
// ------------------------ //

static void JukeboxDEF_STRLCPY(char *p, const char *s, size_t n)
{
	strlcpy(p, s, n);
	while ((p = strchr(p, '_')))
		*p++ = ' '; // turn _ into spaces.
}

boolean TSoURDt3rd_ParseJukeboxDef(tsourdt3rd_starscript_t *script)
{
	tsourdt3rd_jukeboxdef_t *jukedef = NULL;
	CONS_Printf("1 - word + value: %s=%s\n", script->tkn, script->val);

	if (!tsourdt3rd_global_jukebox || !TSoURDt3rd_Jukebox_PrepareDefs())
		return true;
	CONS_Printf("2 - word + value: %s=%s\n", script->tkn, script->val);

	switch (STAR_Script_TokenToTableTerm(script, star_jukebox_term_opt))
	{
		case star_jukebox_pagetitles:
		{
			while (script->tkn != NULL)
			{
				tsourdt3rd_jukebox_pages_t *prev_page = NULL;
				tsourdt3rd_jukebox_pages_t *juke_page = tsourdt3rd_jukeboxpages_start;
				INT32 page_pos = 0, given_page_num = atoi(script->tkn);

				script->val = script->tokenizer->get(script->tokenizer, 1);
				if (strcmp(script->val, "="))
				{
					STAR_Script_Error("JUKEBOXDEF: Missing operator sign.", script, STAR_SCRIPT_ERROR_LINE);
					return true;
				}
				script->val = script->tokenizer->get(script->tokenizer, 1);

				while (page_pos < given_page_num)
				{
					page_pos++;

					if (juke_page == NULL)
						continue;

					if (prev_page != NULL)
						prev_page->next = juke_page;

					prev_page = juke_page;
					juke_page = juke_page->next;
				}

				if (juke_page)
				{
					if (!stricmp(juke_page->page_name, script->val))
						STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Page \x82%s\x80 already exists, with the same name %s!\n", juke_page->page_name, script->val);
					else
					{
						STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Renaming Page \x82%s\x80 to \x82%s\x80!\n", juke_page->page_name, script->val);
						JukeboxDEF_STRLCPY(juke_page->page_name, script->val, 64);
					}
				}
				else
				{
					juke_page = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);
					JukeboxDEF_STRLCPY(juke_page->page_name, script->val, 64);

					if (prev_page != NULL)
						prev_page->next = juke_page;

					STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Created Page \x82%s\x80!\n", juke_page->page_name);
				}	
				CONS_Printf("before val input = %s at pos %d\n", script->val, given_page_num);

#if 0
				(*tsourdt3rd_jukebox_available_pages) = juke_page;
#else
				tsourdt3rd_jukebox_available_pages[given_page_num] = juke_page;
#endif
				script->tkn = script->tokenizer->get(script->tokenizer, 0);

				CONS_Printf("before repeat - word + value = %s : %s\n", script->tkn, script->val);
				if (TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_SEMICOLON))
					break;
				else if (TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_NEWLINE))
				{
					STAR_Script_Error("JUKEBOXDEF: Missing closing semicolon.", script, STAR_SCRIPT_ERROR_LINE);
					return true;
				}
				else if (!TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_COMMA))
				{
					STAR_Script_Error("JUKEBOXDEF: Missing continuing comma.", script, STAR_SCRIPT_ERROR_LINE);
					return true;
				}

				script->tkn = script->tokenizer->get(script->tokenizer, 0);
				CONS_Printf("after repeat - word + value = %s : %s\n", script->tkn, script->val);
			}

			return false;
		}

		case star_jukebox_lump:
		{
			while (script->tkn != NULL)
			{
				char *lump = strdup(script->val);

				if (!lump)
				{
					STAR_Script_Error("JUKEBOXDEF: Lump field missing lump name.", script, STAR_SCRIPT_ERROR_LINE);
					free(lump);
					return true;
				}

				if (!TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_OPENING))
				{
					free(lump);
					return true;
				}

				script->tkn = script->tokenizer->get(script->tokenizer, 0);
				script->val = script->tokenizer->get(script->tokenizer, 1);
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "made it here, %s, %s\n", lump, script->tkn);

				// Check if this new lump has a pre-existing definition...
				STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Searching for pre-existing music definition for lump \x82%s\x80...\n", lump);
				for (jukedef = jukebox_def_start; jukedef; jukedef = jukedef->next)
				{
					CONS_Printf("compare lump strings %s and %s please\n", jukedef->linked_musicdef->name, lump);

					if (!stricmp(jukedef->linked_musicdef->name, lump))
					{
						STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Found pre-existing music definition for lump \x82%s\x80!\n", lump);
						break;
					}
				}

				if (!jukedef || !jukedef->linked_musicdef || *jukedef->linked_musicdef->name == '\0')
				{
					// We couldn't find the lump, so let's just return...
					STAR_Script_Error(va("JUKEBOXDEF: Lump \x82%s\x80 must already have a MUSICDEF!", lump), script, STAR_SCRIPT_ERROR_LINE);
					free(lump);
					return true;
				}

				CONS_Printf("3 - word + value = %s:%s\n", script->tkn, script->val);
				while (script->tkn != NULL)
				{
					CONS_Printf("value before lump junk is = %s:%s\n", script->tkn, script->val);
					switch (STAR_Script_TokenToTableTerm(script, star_jukebox_lump_term_opt))
					{
						case star_jukebox_lump_jukeboxpage:
						{
							if (strcmp(script->val, "="))
							{
								STAR_Script_Error("JUKEBOXDEF: Missing operator sign.", script, STAR_SCRIPT_ERROR_LINE);
								free(lump);
								return true;
							}

							while (script->tkn != NULL)
							{
								INT32 next_available_page = 0, given_page_num = atoi(script->tkn);

								if (TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_NEWLINE))
								{
									STAR_Script_Error("JUKEBOXDEF: Missing closing semicolon.", script, STAR_SCRIPT_ERROR_LINE);
									free(lump);
									return true;
								}

								CONS_Printf("tkn before supported_pages is %d\n", given_page_num);
								if (tsourdt3rd_jukebox_available_pages[given_page_num] == NULL)
								{
									STAR_Script_Error(va("JUKEBOXDEF: Page %s doesn't exist!\n\tDid you create it beforehand?\n", script->tkn), script, STAR_SCRIPT_ERROR_LINE);
									free(lump);
									return true;
								}

								while (jukedef->supported_pages[next_available_page])
									next_available_page++;
								jukedef->supported_pages[next_available_page] = given_page_num;
								STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Lump \x82%s\x80 can now be found on Page \x82%d\x80!\n", lump, given_page_num);

								script->tkn = script->tokenizer->get(script->tokenizer, 0);
								CONS_Printf("supported_pages is %d\n", jukedef->supported_pages[next_available_page]);
								if (TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_COMMA))
									script->tkn = script->tokenizer->get(script->tokenizer, 0);
								if (TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_SEMICOLON))
									break;
							}
							break;
						}

						default:
							break;
					}

#if 0
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					if (!strcmp(script->tkn, "}"))
					{
						script->val = script->tokenizer->get(script->tokenizer, 1);
						if (strcmp(script->val, ";"))
						{
							STAR_Script_Error("JUKEBOXDEF: Missing closing semicolon.", script, STAR_SCRIPT_ERROR_LINE);
							free(lump);
							return true;
						}

						script->tkn = script->tokenizer->get(script->tokenizer, 0);
						script->val = script->tokenizer->get(script->tokenizer, 1);
						CONS_Printf("value after lump junk is = %s:%s\n", script->tkn, script->val);
						break;
					}
#else
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					CONS_Printf("value right before lump junk finale is = %s:%s\n", script->tkn, script->val);
					if (!TSoURDt3rd_STARScript_CheckForCorrectEOL(script, TSOURDT3RD_STARSCRIPT_EOL_BRACKET_CLOSING))
					{
						free(lump);
						return true;
					}

					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					CONS_Printf("value after lump junk is = %s:%s\n", script->tkn, script->val);
#endif
					break;
				}

				if (!stricmp(script->tkn, "LUMP"))
				{
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					continue;
				}

				free(lump);
				break;
			}

			CONS_Printf("finale value after junk is = %s:%s\n", script->tkn, script->val);
			return false;
		}

		default:
			STAR_Script_Error(va("JUKEBOXDEF: Invalid field '%s'.", script->tkn), script, STAR_SCRIPT_ERROR_FULL);
			break;
	}

	return true;
}
