// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-g_game.c
/// \brief TSoURDt3rd's game data

#include "smkg-g_game.h"
#include "smkg-d_main.h"

#include "../ss_main.h"
#include "../smkg-cvars.h"
#include "../misc/smkg-m_misc.h"

#include "../../p_saveg.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

tsourdt3rd_t tsourdt3rd[MAXPLAYERS];
struct tsourdt3rd_local_s tsourdt3rd_local;

tsourdt3rd_gamedata_t *tsourdt3rd_client_gamedata;

// ------------------------ //
//        Functions
// ------------------------ //

// -------------------------------
// Savedata Routines
// -------------------------------

//
// void TSoURDt3rd_G_SetSaveDirectory(const char *new_directory)
// Sets the directory for TSoURDt3rd's save directory. Includes fallbacks too.
//
void TSoURDt3rd_G_SetSaveDirectory(const char *new_directory)
{
	if (TSoURDt3rd_FOL_CreateDirectory(new_directory) == false)
	{
		// Since the new directory we wanted can't be created, we'll keep the old save directory.
		// If that can't be used either, then the save directory will just be set to SRB2's home path.
		if (TSoURDt3rd_FOL_CreateDirectory(tsourdt3rd_save_dir))
			new_directory = tsourdt3rd_save_dir;
		else
			new_directory = TSoURDt3rd_FOL_ReturnHomepath_SRB2();
	}
	strlcpy(tsourdt3rd_save_dir, new_directory, MAX_WADPATH);
	TSoURDt3rd_G_UpdateSaveDirectory();
}

//
// void TSoURDt3rd_G_UpdateSaveDirectory(void)
// Updates TSoURDt3rd's savefile directory. Directory is created if it doesn't exist.
//
void TSoURDt3rd_G_UpdateSaveDirectory(void)
{
	boolean has_autoloaded = (tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods);
	const char *home_path = (cv_tsourdt3rd_savefiles_storesavesinfolders.value ? tsourdt3rd_save_dir : TSoURDt3rd_FOL_ReturnHomepath_SRB2());

	// Get the save prefix, for autoloading and all that.
	memset(tsourdt3rd_savedata_prefix, 0, sizeof(tsourdt3rd_savedata_prefix));
	if (has_autoloaded)
	{
		char *save_prefix = strdup(TSOURDT3RD_APP "_");
		strlwr(save_prefix);
		strlcpy(tsourdt3rd_savedata_prefix, save_prefix, MAX_WADPATH);
		free(save_prefix);
	}

	// Get the default save path.
	strlcpy(tsourdt3rd_savefile_dir, home_path, TSOURDT3RD_MAX_DIRPATH);
	if (cv_tsourdt3rd_savefiles_storesavesinfolders.value)
	{
		strcat(tsourdt3rd_savefile_dir, PATHSEP);
		if (has_autoloaded)
		{
			// Autoloading gives us a TSoURDt3rd-specific save directory.
			// This helps users not accidentally screw up their own saves.
			strcat(tsourdt3rd_savefile_dir, TSOURDT3RD_APP PATHSEP);
		}
		strcat(tsourdt3rd_savefile_dir, timeattackfolder);
	}
	strlcat(tsourdt3rd_savefile_dir, PATHSEP, TSOURDT3RD_MAX_DIRPATH);

	// Create the directory if it doesn't already exist.
	// Our homepath should always exist though.
	TSoURDt3rd_FOL_CreateDirectory(tsourdt3rd_savefile_dir);
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_DEBUG, M_GetText("Save directory updated!\n"));
}

//
// void TSoURDt3rd_G_SaveGamedata(void)
// Writes data to TSoURDt3rd's gamedata.
//
void TSoURDt3rd_G_SaveGamedata(void)
{
	const char *build_homepath = TSoURDt3rd_FOL_ReturnHomepath_Build();
	FILE *gamedata, *gamedata_bak;
	save_t savebuffer;

	if (tsourdt3rd_local.autoloaded_mods)
	{
		// Game is modified beyond a manageable point, so don't save this stuff!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Mods have been autoloaded, not saving build gamedata!\n"));
		return;
	}
	else if (tsourdt3rd_client_gamedata == NULL)
	{
		// Gamedata is NULL!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Can't save build gamedata!\n"));
		return;
	}

	gamedata = TSoURDt3rd_FIL_AccessFile_Build(build_homepath, tsourdt3rd_gamedata_file, "w+b");
	if (gamedata == NULL)
	{
		// Uh-oh! We couldn't find the actual gamedata file!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Can't save build gamedata!\n"));
		return;
	}

	savebuffer.size = GAMEDATASIZE;
	savebuffer.buf = (UINT8 *)malloc(savebuffer.size);
	if (!savebuffer.buf)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Can't save build gamedata!\n"));
		fclose(gamedata);
		return;
	}
	savebuffer.pos = 0;


	//
	// Write our data to the file.
	//

	// -- Easter Event Data
	P_WriteUINT32(&savebuffer, currenteggs);
	P_WriteUINT32(&savebuffer, foundeggs);


	//
	// Write to the file, then close it.
	// After that, we're done!
	//

	{
		// -- Write a gamedata back up too, just in case.
		char currentfile_name[MAX_WADPATH];
		char backupfile_name[MAX_WADPATH+5];
		char backupfile_extension[5];

		strcpy(backupfile_extension, ".bak");
		strcpy(currentfile_name, tsourdt3rd_gamedata_file);
		STRBUFCPY(backupfile_name, strcat(currentfile_name, backupfile_extension));

		gamedata_bak = TSoURDt3rd_FIL_AccessFile(build_homepath, backupfile_name, "w+b");
		if (gamedata_bak)
		{
			fwrite(savebuffer.buf, 1, savebuffer.pos, gamedata_bak);
			fclose(gamedata_bak);
		}
	}
	fwrite(savebuffer.buf, 1, savebuffer.pos, gamedata);
	fclose(gamedata);
	free(savebuffer.buf);

	STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("TSoURDt3rd Build gamedata saved!\n"));
}

//
// void TSoURDt3rd_G_LoadGameData(void)
// Reads info in TSoURDt3rd's gamedata.
//
void TSoURDt3rd_G_LoadGameData(void)
{
	const char *build_homepath = TSoURDt3rd_FOL_ReturnHomepath_Build();
	FILE *gamedata;
	save_t savebuffer;

	if (tsourdt3rd_local.autoloaded_mods)
	{
		// Game is modified beyond a manageable point, so don't save this stuff!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Mods have been autoloaded, not loading build gamedata!\n"));
		return;
	}
	else if (tsourdt3rd_client_gamedata == NULL)
	{
		// Gamedata is NULL!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Can't load build gamedata!\n"));
		return;
	}

	gamedata = TSoURDt3rd_FIL_AccessFile_Build(build_homepath, tsourdt3rd_gamedata_file, "rb");
	if (gamedata == NULL)
	{
		// Uh-oh! We couldn't find the actual gamedata file!
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Build gamedata doesn't exist!\n"));
		return;
	}

	// Create saving buffer (but this time for loading!)
	savebuffer.size = TSoURDt3rd_FIL_ReadFileContents(gamedata, &savebuffer.buf, PU_STATIC);
	if (!savebuffer.size)
	{
		// No/invalid/null gamedata.
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, M_GetText("Can't load build gamedata!\n"));
		fclose(gamedata);
		return;
	}
	savebuffer.pos = 0;


	//
	// Read the file's data.
	//

	// -- Easter Eggs
	currenteggs	= P_ReadUINT32(&savebuffer);
	foundeggs	= P_ReadUINT32(&savebuffer);


	//
	// Close the file.
	//

	fclose(gamedata);
	Z_Free(savebuffer.buf);

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_DEBUG, M_GetText("Build gamedata loaded!\n"));
}
