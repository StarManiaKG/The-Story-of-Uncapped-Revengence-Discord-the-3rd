// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  defs/smkg-ps_def_jukedef.c
/// \brief STARParser data for JUKEDEF lumps

#include "../smkg-ps_main.h"

#include "../../smkg-jukebox.h"
#include "../../star_vars.h"

#include "../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum star_jukebox_term_e
{
	star_jukebox_pagetitles,
	star_jukebox_lumps,
};

static const char *const star_jukebox_term_opt[] = {
	"PAGETITLES",
	"LUMPS",
	NULL
};

enum star_jukebox_lump_term_e
{
	star_jukebox_lump_jukeboxpage,
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

static void TSoURDt3rd_Jukebox_CreatePages(const char *new_page_name)
{
	tsourdt3rd_jukebox_pages_t *juke_page_prev = NULL;
	tsourdt3rd_jukebox_pages_t *juke_page = tsourdt3rd_jukeboxpages_start;
	char cmp_page_name[64];

	while (juke_page)
	{
		JukeboxDEF_STRLCPY(cmp_page_name, new_page_name, 64);
		if (!stricmp(juke_page->page_name, cmp_page_name))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Page '\x82%s\x80' already exists! I'm not creating a new page\nwith the same exact name as another!\n", new_page_name);
			break;
		}
		juke_page_prev = juke_page;
		juke_page = juke_page->next;
	}

	if (juke_page == NULL)
	{
		juke_page = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);

		juke_page->id = ++tsourdt3rd_jukebox_numpages;
		JukeboxDEF_STRLCPY(juke_page->page_name, new_page_name, 64);

		if (juke_page_prev != NULL)
		{
			juke_page->prev = juke_page_prev;
			juke_page_prev->next = juke_page;
		}

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "JUKEBOXDEF: Created Page '\x82%s\x80'!\n", juke_page->page_name);
	}

	(*tsourdt3rd_jukebox_available_pages) = juke_page;
}

static void TSoURDt3rd_Jukebox_SetSupportedPages(tsourdt3rd_jukebox_pages_t **supported_page_p, char *supported_lump, const char *page_name)
{
	tsourdt3rd_jukebox_pages_t *all_jukebox_pages = tsourdt3rd_jukeboxpages_start;
	tsourdt3rd_jukebox_pages_t *new_page_prev = NULL;
	tsourdt3rd_jukebox_pages_t *new_page = (*supported_page_p);
	INT32 new_page_pos = 0;

	while (all_jukebox_pages)
	{
		if (!stricmp(all_jukebox_pages->page_name, page_name))
			break;
		all_jukebox_pages = all_jukebox_pages->next;
	}
	if (all_jukebox_pages == NULL)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Page '\x82%s\x80' doesn't exist! Did you create it beforehand?\n", page_name);
		return;
	}

	while (new_page)
	{
		if (!stricmp(new_page->page_name, page_name))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "JUKEBOXDEF: Lump '\x82%s\x80' could already be found on Page '\x82%s\x80'!\n", supported_lump, page_name);
			break;
		}
		new_page_prev = new_page;
		new_page = new_page->next;
		new_page_pos++;
	}

	if (new_page == NULL)
	{
		new_page = Z_Calloc(sizeof(tsourdt3rd_jukebox_pages_t), PU_STATIC, NULL);

		new_page->id = new_page_pos;
		strlcpy(new_page->page_name, page_name, 64);

		if (new_page_prev != NULL)
		{
			new_page->prev = new_page_prev;
			new_page_prev->next = new_page;
		}

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "JUKEBOXDEF: Lump '\x82%s\x80' can now be found on Page '\x82%s\x80'!\n", supported_lump, page_name);
	}

	(*supported_page_p) = new_page;
}

boolean TSoURDt3rd_STARParser_JukeDefs(tsourdt3rd_starparser_t *script)
{
	switch (TSoURDt3rd_STARParser_ValidTableTerm(script, star_jukebox_term_opt, true))
	{
		case star_jukebox_pagetitles:
		{
			while (script->tkn != NULL)
			{
				TSoURDt3rd_Jukebox_CreatePages(script->tkn);

				if (!strcmp(script->val, ","))
				{
					script->tkn = script->tokenizer->get(script->tokenizer, 0);
					script->val = script->tokenizer->get(script->tokenizer, 1);
					continue;
				}
				else if (!strcmp(script->val, ";"))
					break;

				TSoURDt3rd_STARParser_Error("JUKEBOXDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
				return true;
			}
			return false;
		}

		case star_jukebox_lumps:
		{
			tsourdt3rd_jukeboxdef_t *jukedef = NULL;
			char *lump = NULL;

			while (script->tkn != NULL)
			{
				jukedef = jukebox_def_start;
				lump = strdup(script->tkn);

				// Check if this new lump has a pre-existing definition...
				while (jukedef)
				{
					if (!stricmp(jukedef->linked_musicdef->name, lump))
					{
						STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "JUKEBOXDEF: Found pre-existing music definition for Lump '\x82%s\x80'!\n", lump);
						break;
					}
					jukedef = jukedef->next;
				}
				if (jukedef == NULL)
				{
					// We couldn't find the lump, so let's just return...
					TSoURDt3rd_STARParser_Error(va("JUKEBOXDEF: Lump '\x82%s\x80' must already have a MUSICDEF!", lump), script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					break;
				}

				if (strcmp(script->val, ":"))
				{
					TSoURDt3rd_STARParser_Error("JUKEBOXDEF: Missing operator '\x82:\x80'!", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
					break;
				}

				switch (TSoURDt3rd_STARParser_ValidTableTerm(script, star_jukebox_lump_term_opt, false))
				{
					case star_jukebox_lump_jukeboxpage:
					{
						if (strcmp(script->val, "="))
						{
							TSoURDt3rd_STARParser_Error("JUKEBOXDEF: Missing operator sign.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
							break;
						}
						script->val = script->tokenizer->get(script->tokenizer, 1);

						while (script->val != NULL)
						{
							tsourdt3rd_jukebox_pages_t *new_supported_page = jukedef->supported_pages;
							char new_page_name[256];

							JukeboxDEF_STRLCPY(new_page_name, script->val, 64);
							TSoURDt3rd_Jukebox_SetSupportedPages(&new_supported_page, lump, new_page_name);

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

							TSoURDt3rd_STARParser_Error("JUKEBOXDEF: Missing required EOL operator.", script, TSOURDT3RD_STARPARSER_ERROR_LINE);
							return true;
						}
						break;
					}

					default:
						break;
				}

				if (lump != NULL)
				{
					free(lump);
					lump = NULL;
				}

				if (TSoURDt3rd_STARParser_CheckForBrackets(script) == TSOURDT3RD_STARPARSER_BRAK_CLOSE)
					break;
			}
			return false;
		}

		default:
			TSoURDt3rd_STARParser_Error(va("JUKEBOXDEF: Invalid field '\x82%s\x80'.", script->tkn), script, TSOURDT3RD_STARPARSER_ERROR_FULL);
			break;
	}

	return true;
}
