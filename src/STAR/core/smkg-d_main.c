// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-d_main.c
/// \brief TSoURDt3rd's main program data

#include "smkg-d_main.h"
#include "../smkg-misc.h" // TSoURDt3rd_FIL_AccessFile() //
#include "../star_vars.h"

#include "../../console.h"
#include "../../fastcmp.h"
#include "../../w_wad.h"

// ------------------------ //
//        Variables
// ------------------------ //

static addfilelist_t autoload_startupwadfiles;

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

// ------------------------ //
//        Functions
// ------------------------ //

static void D_AddAddonsToAutoLoad(addfilelist_t *list, const char *file, INT32 filetype)
{
	char *newfile;
	boolean addon_is_folder = (filetype == 1);
	const char *addon_type = (addon_is_folder ? "folder" : "file");
	size_t xtra_delimiter = (addon_is_folder ? 2 : 1);
	size_t index = 0;

	REALLOC_FILE_LIST

	newfile = malloc(strlen(file) + xtra_delimiter); // Path delimiter + NULL terminator
	if (!newfile)
		I_Error("TSoURDt3rd - D_AddAddonsToAutoLoad: No more free memory to autoload %s %s", addon_type, file);

	strcpy(newfile, file);
	if (addon_is_folder)
		strcat(newfile, PATHSEP);

	if (filetype <= 6)
		list->files[index] = newfile;
	else
		COM_BufAddText(va("exec %s\n", newfile));
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

void TSoURDt3rd_D_AutoLoadAddons(void)
{
	FILE *autoload_config = NULL;
	char *wad_tkn = NULL;

	autoload_config = TSoURDt3rd_FIL_AccessFile(NULL, AUTOLOADCONFIGFILENAME, "r");
	if (autoload_config != NULL)
	{
		// Let's move this old file to a new directory!
		fclose(autoload_config);
		if (TSoURDt3rd_FIL_RenameFile(AUTOLOADCONFIGFILENAME, "TSoURDt3rd" PATHSEP AUTOLOADCONFIGFILENAME))
			autoload_config = TSoURDt3rd_FIL_AccessFile("TSoURDt3rd", AUTOLOADCONFIGFILENAME, "r");
	}

	autoload_config = TSoURDt3rd_FIL_AccessFile("TSoURDt3rd", AUTOLOADCONFIGFILENAME, "r");
	if (autoload_config == NULL)
	{
		// Uh-oh! We couldn't find the actual autoload config!
		return;
	}

	tsourdt3rd_local.autoloading_mods = true;
	CON_StopRefresh(); // Temporarily stop refreshing the screen for wad autoloading
	wad_tkn = malloc(256);

	while (fgets(wad_tkn, 256, autoload_config) != NULL)
	{
		if (wad_tkn[0] == '#' || wad_tkn[0] == '\0' || wad_tkn[0] == '\n')
		{
			// This is a comment or a non-valid line, so skip it please.
			continue;
		}

		if (fastncmp(wad_tkn, "addfile ", 8) || fastncmp(wad_tkn, "addfolder ", 10))
		{
			// This is an outdated version of an autoloadfile line, so let's update it!
			wad_tkn += (fastncmp(wad_tkn, "addfile ", 8) ? 8 : 10);
		}

		// Remove that straying newline.
		if (wad_tkn[strlen(wad_tkn)-1] == '\n')
			wad_tkn[strlen(wad_tkn)-1] = '\0';

		// Check the wad's filetype. If it's valid, load it, otherwise, do nothing.
		if (!STAR_DetectFileType(wad_tkn))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, M_GetText("TSoURDt3rd_D_AutoLoadAddons: File %s is unknown or invalid.\n"), wad_tkn);
			continue;
		}
		D_AddAddonsToAutoLoad(&autoload_startupwadfiles, wad_tkn, STAR_DetectFileType(wad_tkn));
	}

	if (autoload_startupwadfiles.numfiles)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "TSoURDt3rd_D_AutoLoadAddons(): Autoloading Addons...\n");
		W_InitMultipleFiles(&autoload_startupwadfiles);
		D_CleanFile(&autoload_startupwadfiles);
		if (modifiedgame || savemoddata || usedCheats)
			tsourdt3rd_local.autoloaded_mods = true;
		modifiedgame = false;
		tsourdt3rd_local.autoloading_mods = false;
		usedCheats = false;
	}

	// We're done! :)
	free(wad_tkn);
	fclose(autoload_config);
	CON_StartRefresh(); // Restart the refresh!
}
