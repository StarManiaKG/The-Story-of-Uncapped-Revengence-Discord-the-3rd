// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  defs/smkg-ps_def_jukedef.c
/// \brief STARParser data for JUKEDEF lumps

#include "../smkg-ps_main.h"
#include "../../core/smkg-s_jukebox.h"
#include "../../star_vars.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum star_jukebox_term_e
{
	star_jukebox_pagetitles,
	star_jukebox_lumps,
}; static const char *const star_jukebox_term_opt[] = {
	"PAGETITLES",
	"LUMPS",
	NULL
};

enum star_jukebox_lump_term_e
{
	star_jukebox_lump_jukeboxpage,
}; static const char *const star_jukebox_lump_term_opt[] = {
	"jukeboxpage",
	NULL,
};

// ------------------------ //
//        Functions
// ------------------------ //

static void TSoURDt3rd_JUKEDEF_CreatePages(char *page_name)
{
	tsourdt3rd_jukebox_pages_t *juke_page_prev = NULL;
	tsourdt3rd_jukebox_pages_t *juke_page = tsourdt3rd_jukeboxpages_start;

	while (juke_page && (juke_page->num <= tsourdt3rd_jukebox_numpages))
	{
		if (!strnicmp(juke_page->page_name, page_name, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME))
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "JUKEDEF: Page \x82\"%s\"\x80 already exists!\n", page_name);
			return;
		}
		juke_page_prev = juke_page;
		juke_page = juke_page->next;
	}

	if (juke_page == NULL)
	{
		++tsourdt3rd_jukebox_numpages;
		tsourdt3rd_jukebox_available_pages = Z_Realloc(tsourdt3rd_jukebox_available_pages, tsourdt3rd_jukebox_numpages * sizeof(tsourdt3rd_jukebox_pages_t *), PU_STATIC, NULL);

		juke_page = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);
		juke_page->num = tsourdt3rd_jukebox_numpages;

		strlcpy(juke_page->page_name, page_name, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME);

		if (juke_page_prev != NULL)
		{
			juke_page->prev = juke_page_prev;
			juke_page_prev->next = juke_page;
		}

		STAR_CONS_Printf(STAR_CONS_NOTICE, "JUKEDEF: Created page \x82\"%s\"\x80!\n", juke_page->page_name);
	}
	(*tsourdt3rd_jukebox_available_pages) = juke_page;
}

static void TSoURDt3rd_JUKEDEF_SetSupportedPages(tsourdt3rd_jukebox_pages_t **supported_page_p, char *supported_lump, char *page_name)
{
	tsourdt3rd_jukebox_pages_t *all_jukebox_pages = tsourdt3rd_jukeboxpages_start;
	tsourdt3rd_jukebox_pages_t *new_page_prev = NULL;
	tsourdt3rd_jukebox_pages_t *new_page = (*supported_page_p);
	INT32 new_page_pos = 0;

	while (all_jukebox_pages)
	{
		if (!strnicmp(all_jukebox_pages->page_name, page_name, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME)) break;
		all_jukebox_pages = all_jukebox_pages->next;
	}
	if (all_jukebox_pages == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_ERROR, "JUKEDEF: Page \x82\"%s\"\x80 doesn't exist! Did you create it beforehand?\n", page_name);
		return;
	}

	while (new_page)
	{
		if (!strnicmp(new_page->page_name, page_name, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME))
		{
			STAR_CONS_Printf(STAR_CONS_ERROR, "JUKEDEF: Lump \x82\"%s\"\x80 could already be found on page \x82\"%s\"\x80!\n", supported_lump, page_name);
			return;
		}
		new_page_prev = new_page;
		new_page = new_page->next;
		new_page_pos++;
	}

	if (new_page == NULL)
	{
		new_page = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);
		new_page->num = new_page_pos;

		strlcpy(new_page->page_name, page_name, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME);

		if (new_page_prev != NULL)
		{
			new_page->prev = new_page_prev;
			new_page_prev->next = new_page;
		}

		STAR_CONS_Printf(STAR_CONS_NOTICE, "JUKEDEF: Lump \x82\"%s\"\x80 can now be found on page \x82\"%s\"\x80!\n", supported_lump, page_name);
	}
	(*supported_page_p) = new_page;
}

boolean TSoURDt3rd_STARParser_JUKEDEF(tsourdt3rd_starparser_t *script)
{
	switch (TSoURDt3rd_STARParser_ValidTableTerm(script, star_jukebox_term_opt, true))
	{
		case star_jukebox_pagetitles:
		{
			while (script->tkn != NULL)
			{
				char *page_name = malloc(TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME);

				TSoURDt3rd_STARParser_STRLCPY(page_name, script->tkn, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME);
				TSoURDt3rd_JUKEDEF_CreatePages(page_name);
				free(page_name);
				page_name = NULL;

				if (!strcmp(script->val, ","))
				{
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					continue;
				}
				else if (!strcmp(script->val, ";"))
					break;

				TSoURDt3rd_STARParser_Error("JUKEDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
				return true;
			}
			return false;
		}

		case star_jukebox_lumps:
		{
			while (script->tkn != NULL)
			{
				tsourdt3rd_jukeboxdef_t *jukedef = jukebox_def_start;
				char *lump_name = strdup(script->tkn);

				// Check if this new lump has a pre-existing definition...
				while (jukedef)
				{
					musicdef_t *def = jukedef->linked_musicdef;
					INT32 track = 0;

					while (track < def->numtracks)
					{
						if (!strnicmp(def->name[track], lump_name, 7))
						{
							STAR_CONS_Printf(STAR_CONS_DEBUG, "JUKEDEF: Found pre-existing music definition for Lump \x82\"%s\"\x80!\n", lump_name);
							break;
						}
						track++;
					}
					if (track < def->numtracks)
					{
						break;
					}

					jukedef = jukedef->next;
				}
				if (jukedef == NULL)
				{
					// We couldn't find the lump, so let's just return...
					TSoURDt3rd_STARParser_Error(va("JUKEDEF: Lump \x82\"%s\"\x80 must already have a MUSICDEF!", lump_name), script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					break;
				}

				if (strcmp(script->val, ":"))
				{
					TSoURDt3rd_STARParser_Error("JUKEDEF: Missing operator '\x82:\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					break;
				}

				switch (TSoURDt3rd_STARParser_ValidTableTerm(script, star_jukebox_lump_term_opt, false))
				{
					case star_jukebox_lump_jukeboxpage:
					{
						if (strcmp(script->val, "="))
						{
							TSoURDt3rd_STARParser_Error("JUKEDEF: Missing operator sign.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
							break;
						}
						script->val = script->tokenizer->get(script->tokenizer, 1);

						while (script->val != NULL)
						{
							tsourdt3rd_jukebox_pages_t *new_supported_page = jukedef->supported_pages;
							char *page_name = malloc(TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME);

							TSoURDt3rd_STARParser_STRLCPY(page_name, script->val, TSOURDT3RD_JUKEBOX_MAX_PAGE_NAME);
							TSoURDt3rd_JUKEDEF_SetSupportedPages(&new_supported_page, lump_name, page_name);
							free(page_name);
							page_name = NULL;

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
								break;
							}

							TSoURDt3rd_STARParser_Error("JUKEDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
							return true;
						}
						break;
					}
					default:
						TSoURDt3rd_STARParser_Error(va("JUKEDEF: Invalid lump term \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
						break;
				}

				if (lump_name != NULL)
				{
					free(lump_name);
					lump_name = NULL;
				}

				if (TSoURDt3rd_STARParser_CheckForBrackets(script) == TSOURDT3RD_STARPARSER_BRAK_CLOSE)
					break;
			}
			return false;
		}

		default:
			TSoURDt3rd_STARParser_Error(va("JUKEDEF: Invalid field \x82\"%s\"\x80.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
			break;
	}

	return true;
}
