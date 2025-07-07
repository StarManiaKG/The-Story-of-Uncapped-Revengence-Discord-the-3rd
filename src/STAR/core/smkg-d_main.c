// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-d_main.c
/// \brief TSoURDt3rd's main program data

#include "smkg-d_main.h"

#include "smkg-r_main.h"
#include "../star_vars.h"
#include "../menus/smkg-m_sys.h" // menumessage //
#include "../misc/smkg-m_misc.h" // TSoURDt3rd_FIL_AccessFile() //

#include "../../console.h"
#include "../../filesrch.h"
#include "../../i_time.h"
#include "../../w_wad.h"

#ifdef HAVE_CURL
#include "../curl/smkg-curl.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

static addfilelist_t autoload_startupwadfiles;

// ------------------------ //
//        Functions
// ------------------------ //

#define REALLOC_FILE_LIST \
	if (list->files == NULL) \
	{ \
		list->files = calloc(2, sizeof(list->files)); \
		list->numfiles = 1; \
	} \
	else \
	{ \
		index = list->numfiles; \
		list->files = realloc(list->files, sizeof(list->files) * ((++list->numfiles) + 1)); \
		if (list->files == NULL) \
			I_Error("%s: No more free memory to add file %s", __FUNCTION__, file); \
	}

//
// static void D_AddAddonsToAutoLoad(addfilelist_t *list, const char *file)
// Adds files to TSoURDt3rd's autoload list, so the files can actually be loaded.
//
static void D_AddAddonsToAutoLoad(addfilelist_t *list, const char *file)
{
	char *newfile, *newfile_cpy;
	size_t index = 0;

	REALLOC_FILE_LIST

	// Path delimiter + NULL terminator
	newfile = malloc(strlen(file) + 1);
	if (!newfile)
		I_Error("TSoURDt3rd - D_AddAddonsToAutoLoad: No more free memory to autoload %s", file);

	strcpy(newfile, file);
	newfile_cpy = (newfile + strlen(newfile) - 4);

	if (!stricmp(newfile_cpy, ".cfg") || !stricmp(newfile_cpy, ".txt"))
	{
		COM_BufAddText(va("exec \"%s\"\n", newfile));
		list->files[index] = NULL;
		list->numfiles--;
	}
	else
		list->files[index] = newfile;
}

#undef REALLOC_FILE_LIST

static inline void D_CleanFile(addfilelist_t *list)
{
	if (list->files)
	{
		size_t pnumwadfiles = 0;

		for (; pnumwadfiles < list->numfiles; pnumwadfiles++)
			free(list->files[pnumwadfiles]);

		free(list->files);
		list->files = NULL;
	}

	list->numfiles = 0;
}

//
// void TSoURDt3rd_D_AutoLoadAddons(void)
// Autoloading system for TSoURDt3rd.
//
void TSoURDt3rd_D_AutoLoadAddons(void)
{
	FILE *autoload_config = NULL;
	char *wad_tkn = NULL;
	size_t wad_string_size = 256;

	autoload_config = TSoURDt3rd_FIL_AccessFile(NULL, AUTOLOADCONFIGFILENAME, "r");
	if (autoload_config != NULL)
	{
		// Let's move this old file to a new directory!
		fclose(autoload_config);
		TSoURDt3rd_FIL_RenameFile(AUTOLOADCONFIGFILENAME, "TSoURDt3rd" PATHSEP AUTOLOADCONFIGFILENAME);
	}
	autoload_config = TSoURDt3rd_FIL_AccessFile("TSoURDt3rd", AUTOLOADCONFIGFILENAME, "r");

	if (autoload_config == NULL)
	{
		// Uh-oh! We couldn't find the actual autoload config!
		return;
	}

	tsourdt3rd_local.autoloading_mods = true;
	wad_tkn = malloc(wad_string_size);

	while (fgets(wad_tkn, 256, autoload_config) != NULL)
	{
		if (wad_tkn == NULL || *wad_tkn == '#' || *wad_tkn == '\0' || *wad_tkn == '\n')
		{
			// This is a comment or a non-valid line, so skip it please.
			continue;
		}

		// This string could be an outdated version of an autoloadfile line, so let's update it!
		if (!strcmp(wad_tkn, "addfile"))
			wad_tkn += 8;
		else if (!strcmp(wad_tkn, "addfolder"))
			wad_tkn += 10;

		TSoURDt3rd_M_RemoveStringChars(wad_tkn, "\n"); // remove newlines please
		D_AddAddonsToAutoLoad(&autoload_startupwadfiles, wad_tkn);
	}

	if (autoload_startupwadfiles.numfiles)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "TSoURDt3rd_D_AutoLoadAddons(): Autoloading Addons...\n");
		W_InitMultipleFiles(&autoload_startupwadfiles);
		D_CleanFile(&autoload_startupwadfiles);
		if (modifiedgame || savemoddata || usedCheats)
			tsourdt3rd_local.autoloaded_mods = true;
		modifiedgame = false;
		usedCheats = false;
	}

	// We're done! :)
	free(wad_tkn);
	fclose(autoload_config);
	tsourdt3rd_local.autoloading_mods = false;
}

// =========================================================================
// TSoURDt3rd_D_Loop
// =========================================================================

//
// Frame building routines and data for TSoURDt3rd
//
void TSoURDt3rd_D_BuildFrame(void)
{
	boolean interp = (R_UsingFrameInterpolation() && !dedicated);

	if (interp)
	{
		rendertimefrac_unpaused = g_time.timefrac;
	}
	else
	{
		rendertimefrac_unpaused = FRACUNIT;
	}
}

void TSoURDt3rd_D_Loop(void)
{
	static boolean check_for_updates = false;
	static boolean sent_event_message = false;

#ifndef HAVE_CURL
	(void)check_for_updates;
#else
	// Check for any updates to TSoURDt3rd (on startup).
	if (check_for_updates == false && cv_tsourdt3rd_main_checkforupdatesonstartup.value)
		TSoURDt3rd_CurlRoutine_FindUpdates();
	check_for_updates = true;
#endif

#ifdef HAVE_DISCORDSUPPORT
	if (!dedicated)
	{
		// Run any Discord activity callbacks for us, please.
		DISC_RunCallbacks();
	}
#endif

	switch (gamestate)
	{
		case GS_ENDING:
		case GS_CREDITS:
		case GS_EVALUATION:
			if (TSoURDt3rd_AprilFools_ModeEnabled())
			{
				// -- Close the game if we're in April Fools' Ultimate Mode but just beat the game :p
				I_Error("Definitely caused by a SIGSEGV - seventh sentinel (core dumped)");
			}
			/* FALLTHRU */
		default:
			break;
	}

	if (!menumessage.active && !sent_event_message && tsourdt3rd_currentEvent)
	{
		TSoURDt3rd_M_StartMessage(
			"A TSoURDt3rd Event is Occuring",
			"We're having a seasonal event! Have fun!",
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
		sent_event_message = true;
	}
}
