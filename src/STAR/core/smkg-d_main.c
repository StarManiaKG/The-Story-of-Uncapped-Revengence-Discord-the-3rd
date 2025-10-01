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
#include "smkg-g_game.h"

#include "../core/smkg-s_jukebox.h"
#include "../core/smkg-s_exmusic.h"
#include "../menus/smkg-m_sys.h" // menumessage //
#include "../misc/smkg-m_misc.h" // TSoURDt3rd_FIL_AccessFile_Build() //

#include "../smkg-i_sys.h" // TSoURDt3rd_I_Pads_InitControllers() //

#ifdef HAVE_CURL
#include "../curl/smkg-curl.h"
#endif

#include "../../d_main.h"
#include "../../console.h"
#include "../../i_time.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

static addfilelist_t autoload_startupwadfiles;

//
// Directories
//

char tsourdt3rd_home_dir[TSOURDT3RD_MAX_DIRPATH];

char tsourdt3rd_gamedata_file[MAX_WADPATH];
char tsourdt3rd_savedata_prefix[MAX_WADPATH];

char tsourdt3rd_save_dir[MAX_WADPATH];
char tsourdt3rd_savefile_dir[TSOURDT3RD_MAX_DIRPATH];

// ------------------------ //
//        Functions
// ------------------------ //

//
// static void D_AddAddonsToAutoLoad(addfilelist_t *list, const char *file)
// Adds files to TSoURDt3rd's autoload list, so the files can actually be loaded.
//
static void D_AddAddonsToAutoLoad(addfilelist_t *list, const char *file)
{
	char *newfile, *newfile_cpy;
	size_t index = 0;

	// REALLOC_FILE_LIST
	if (list->files == NULL)
	{
		list->files = calloc(2, sizeof(list->files));
		list->numfiles = 1;
	}
	else
	{
		index = list->numfiles;
		list->files = realloc(list->files, sizeof(list->files) * ((++list->numfiles) + 1));
		if (list->files == NULL)
			I_Error("D_AddAddonsToAutoLoad(): No more free memory to add file %s", file);
	}

	// Path delimiter + NULL terminator
	newfile = malloc(strlen(file) + 1);
	if (!newfile)
		I_Error("D_AddAddonsToAutoLoad(): No more free memory to autoload %s", file);

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

static inline void D_CleanAutoloadedFiles(addfilelist_t *list)
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
	FILE *autoload_config = TSoURDt3rd_FIL_AccessFile_Build(TSoURDt3rd_FOL_ReturnHomepath_Build(), TSOURDT3RD_AUTOLOAD_CONFIG_FILENAME, "r");
	char *wad_tkn;

	if (autoload_config == NULL)
	{
		// Uh-oh! We couldn't find the actual autoload config!
		return;
	}

	tsourdt3rd_local.autoloaded_mods = false;
	tsourdt3rd_local.autoloading_mods = true;

	wad_tkn = malloc(256);
	if (wad_tkn != NULL)
	{
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
		free(wad_tkn);
	}
	fclose(autoload_config);

	// Load the files!
	if (autoload_startupwadfiles.numfiles)
	{
		// -- Check if the files actually modify the game...
		size_t i, files_important = 0;

		for (i = 0; i < autoload_startupwadfiles.numfiles; i++)
		{
			const char *wad = autoload_startupwadfiles.files[i];
			if (W_VerifyNMUSlumps(wad, false) == 0)
				files_important++; // wad can modify game!
		}

		STAR_CONS_Printf(STAR_CONS_NONE, "TSoURDt3rd_D_AutoLoadAddons()...\n");
		W_InitMultipleFiles(&autoload_startupwadfiles);
		D_CleanAutoloadedFiles(&autoload_startupwadfiles);

		if ((modifiedgame || savemoddata || usedCheats) && files_important)
		{
			STAR_CONS_Printf(STAR_CONS_NONE, "TSoURDt3rd_D_AutoLoadAddons(): Mods have modified the game, new save data will be created.\n");
			tsourdt3rd_local.autoloaded_mods = true;
		}

		modifiedgame = false;
		usedCheats = false;
	}
	tsourdt3rd_local.autoloading_mods = false;
}

// =========================================================================
// TSoURDt3rd program loop
// =========================================================================

void TSoURDt3rd_D_Init(void)
{
	//
	// Start!
	//
	STAR_CONS_Printf(STAR_CONS_NONE, "\nTSoURDt3rd_D_Init(): Initalizing TSoURDt3rd...\n");


	// -- Reset our local structures
	memset(&tsourdt3rd_local, 0, sizeof(struct tsourdt3rd_local_s));

	// -- Set version
	sscanf(TSOURDT3RDVERSION, "%hhd.%hhd.%hhd", &tsourdt3rd_local.major_version, &tsourdt3rd_local.minor_version, &tsourdt3rd_local.sub_version);

	// -- Manage directory and files
	char *app_name = strdup(TSOURDT3RD_APP);
	char *app_name_lowercase = strlwr(strdup(TSOURDT3RD_APP));
	const char *home_path = TSoURDt3rd_FOL_ReturnHomepath_SRB2();
	char new_savedir[TSOURDT3RD_MAX_DIRPATH];
	{
		snprintf(tsourdt3rd_home_dir, sizeof(tsourdt3rd_home_dir), pandf, home_path, app_name);
		snprintf(tsourdt3rd_gamedata_file, sizeof(tsourdt3rd_gamedata_file), "%s.dat", app_name_lowercase);
		snprintf(new_savedir, sizeof(new_savedir), "%s" PATHSEP TSOURDT3RD_DEFAULT_SAVEGAMEFOLDER, home_path);
		TSoURDt3rd_FOL_CreateDirectory(app_name);
		TSoURDt3rd_G_SetSaveDirectory(new_savedir);
	}
	free(app_name);
	free(app_name_lowercase);

	// -- Create the gamedata
	tsourdt3rd_client_gamedata = Z_Malloc(sizeof (*tsourdt3rd_client_gamedata), PU_STATIC, NULL);

	// -- Check our computer's time!
	TSoURDt3rd_CheckTime();

	// -- Initialize our cool controller system!
	TSoURDt3rd_I_Pads_InitControllers();

	// -- Initialize Jukebox data...
	TSoURDt3rd_Jukebox_Init();

	// -- Initialize EXMusic data...
	TSoURDt3rd_EXMusic_Init();

	// -- Reset our player structures
	TSoURDt3rd_InitializePlayer(consoleplayer);

	//
	// Done!
	//
	STAR_CONS_Printf(STAR_CONS_NONE, "\n");
}

//
// Frame building routine for TSoURDt3rd
//
void TSoURDt3rd_D_Loop(void)
{
	static boolean sent_event_message = false;

#ifdef HAVE_CURL
	static boolean check_for_updates = false;

	// Check for any updates to TSoURDt3rd (on startup).
	if (check_for_updates == false && cv_tsourdt3rd_main_checkforupdatesonstartup.value)
	{
		TSoURDt3rd_CurlRoutine_FindUpdates();
	}

	check_for_updates = true;
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
