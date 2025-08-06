// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_misc.c
/// \brief Commonly used manipulation routines

#include <stdio.h>
#ifdef _WIN32
#define RPC_NO_WINDOWS_H
#include <windows.h>
#endif
#include <sys/stat.h>

#if defined (_WIN32) && defined (_MSC_VER)
#include <io.h>
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif

#include "smkg-m_misc.h"
#include "../smkg-cvars.h"
#include "../ss_main.h"

#include "../../d_main.h"
#include "../../g_game.h"
#include "../../i_system.h"
#include "../../m_misc.h"

// ------------------------ //
//        Variables
// ------------------------ //

#define SAVEGAMEFOLDER "saves"
char tsourdt3rd_savefiles_folder[256];

// ------------------------ //
//        Functions
// ------------------------ //

//
// char *TSoURDt3rd_M_RemoveStringChars(char *string, const char *c)
// Removes the given chars, 'c', from the given string.
//
char *TSoURDt3rd_M_RemoveStringChars(char *string, const char *c)
{
	INT32 i = 0;
	char *tmp, *p;

	if (!string || *string == '\0')
		return NULL;
	tmp = p = string;

	while (*p)
	{
		if (*tmp == c[i] && *tmp)
		{
			i = 0;
			tmp++;
			continue;
		}
		else if (c[i] != '\0')
		{
			i++;
			continue;
		}

		*p = *tmp;
		p++, tmp++;
		i = 0;

		if (!*tmp)
			break;
	}
	*p = '\0'; // Null-terminate at the end

	return string;
}

//
// INT32 TSoURDt3rd_M_FindWordInTermTable(const char *const *term_table, const char *word, INT32 search_types)
// Checks for a word in a given term table, and returns its value within the table if found.
//
INT32 TSoURDt3rd_M_FindWordInTermTable(const char *const *term_table, const char *word, INT32 search_types)
{
	INT32 word_to_table_val = 0;
	size_t size = sizeof(word)-1;

	if (term_table == NULL)
		return -1;

	while (term_table[word_to_table_val])
	{
		if ((search_types & TSOURDT3RD_TERMTABLESEARCH_MEMCMP) && memcmp(word, term_table[word_to_table_val], size) == 0)
			return word_to_table_val;
		if ((search_types & TSOURDT3RD_TERMTABLESEARCH_NORM) && !stricmp(word, term_table[word_to_table_val]))
			return word_to_table_val;
		if ((search_types & TSOURDT3RD_TERMTABLESEARCH_STRSTR) && strstr(word, term_table[word_to_table_val]))
			return word_to_table_val;
		word_to_table_val++;
	}

	word_to_table_val = -1;
	return word_to_table_val;
}

//
// char *TSoURDt3rd_M_WriteVariedLengthString(char *string, boolean decate)
// Returns a string with a varied length.
//
char *TSoURDt3rd_M_WriteVariedLengthString(char *string, UINT32 decate_len, boolean decate)
{
	UINT32 len;
	const UINT32 len_to_decentate = decate_len;
	static char header[1024];

	strlcpy(header, string, 1024);
	if (decate == false)
	{
		// We've done everything we needed to.
		return header;
	}

	len = strlen(header);
	if (len > len_to_decentate)
	{
		len = (len - len_to_decentate);
		header[len] = header[len+1] = header[len+2] = '.';
	}
	else
		len = 0;

	return header+len;
}

//
// const char *TSoURDt3rd_FOL_ReturnHomepath(void)
// Returns the home path of the user's SRB2 folder.
//
const char *TSoURDt3rd_FOL_ReturnHomepath(void)
{
#ifdef DEFAULTDIR
	return srb2home;
#else
	return userhome;
#endif
}

//
// boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory)
// Returns whether the given directory exists or not.
//
boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory)
{
	struct stat given_directory;

#if defined(__linux__) || defined(__FreeBSD__)
	if (lstat(directory, &given_directory) < 0)
#else
	if (stat(directory, &given_directory) < 0)
#endif
		return false;
	return (S_ISDIR(given_directory.st_mode));
}

//
// boolean TSoURDt3rd_FOL_CreateDirectory(const char *cpath)
//
// Creates a directory in the path specified.
// If a 'PATHSEP' is found in the given path, the function will
//	continuously create directories until the string becomes NULL.
//
boolean TSoURDt3rd_FOL_CreateDirectory(const char *cpath)
{
	const char *home_path = TSoURDt3rd_FOL_ReturnHomepath();
	char *cur_path = NULL;
	INT32 i = 0, j;

	if (!cpath || *cpath == '\0')
		return false;

	if (strstr(cpath, home_path))
	{
		// Cuts out the home directory, just in case you manually specified it...
		cpath += strlen(home_path)+1;
	}

	cur_path = malloc(8192);
	if (cur_path == NULL)
		return false;
	strcpy(cur_path, home_path);
	j = strlen(cur_path);

	cur_path[j++] = PATHSEP[0];
	if (PATHSEP[1])
		cur_path[j++] = PATHSEP[1];

	while (cpath[i])
	{
		if (cpath[i] != PATHSEP[0])
		{
			cur_path[j++] = cpath[i];
			if (cpath[i+1] != '\0')
			{
				i++;
				continue;
			}
		}
		cur_path[j] = '\0'; // Null-terminate at the end

		if (!TSoURDt3rd_FOL_DirectoryExists(cur_path))
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, "Directory '%s' doesn't exist, creating it...\n", cur_path);
			I_mkdir(cur_path, 0755);
		}

		if (cpath[++i] == '\0')
			break;

		cur_path[j++] = PATHSEP[0];
		if (PATHSEP[1])
			cur_path[j++] = PATHSEP[1];
	}

	if (cur_path) free(cur_path);
	return true;
}

//
// void TSoURDt3rd_FOL_UpdateSavefileDirectory(void)
//
// Ensures that TSoURDt3rd's savefile directory still exists.
// Re-creates the directory if it doesn't, does nothing otherwise.
//
void TSoURDt3rd_FOL_UpdateSavefileDirectory(void)
{
	char savefile_directory[256] = "";

	if (!cv_tsourdt3rd_savefiles_storesavesinfolders.value)
		return;

	if (tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods)
		sprintf(savefile_directory, SAVEGAMEFOLDER PATHSEP "%s" PATHSEP "%s", "TSoURDt3rd", timeattackfolder);
	else
		sprintf(savefile_directory, SAVEGAMEFOLDER PATHSEP "%s", timeattackfolder);
	TSoURDt3rd_FOL_CreateDirectory(savefile_directory);
}

//
// FILE *TSoURDt3rd_FIL_AccessFile(const char *directory, const char *filename, const char *mode)
//
// Access a file in the path specified.
// If the file doesn't exist, it's created beforehand.
//
// However, if the directory given isn't found, the function will create those directories first,
//	then the file.
//
FILE *TSoURDt3rd_FIL_AccessFile(const char *directory, const char *filename, const char *mode)
{
	FILE *handle;
	char full_path[256];

	if (directory != NULL && *directory != '\0')
	{
		sprintf(full_path, "%s" PATHSEP "%s" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), directory, filename);
		TSoURDt3rd_FOL_CreateDirectory(directory);
	}
	else
		sprintf(full_path, "%s" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), filename);

	handle = fopen(full_path, mode);
	if (handle == NULL)
	{
		handle = fopen(full_path, "rw+");
		if (handle != NULL)
		{
			fclose(handle);
			handle = fopen(full_path, mode);
		}
	}
	return handle;
}

//
// boolean TSoURDt3rd_FIL_RenameFile(const char *old_name, const char *new_name)
//
// Renames the file specified. Can also be used to move files to new directories.
// Returns true if it renamed the file, false otherwise.
//
boolean TSoURDt3rd_FIL_RenameFile(const char *old_name, const char *new_name)
{
	char old_path[256], new_path[256];

	sprintf(old_path, "%s" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), old_name);
	sprintf(new_path, "%s" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), new_name);
	return (!rename(old_name, new_name));
}

//
// boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename)
//
// Removes a file in the path specified.
// Returns true if it removed the file, false otherwise.
//
boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename)
{
	char full_path[256];

	sprintf(full_path, "%s" PATHSEP "%s" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), directory, filename);
	return (!remove(full_path));
}

//
// void TSoURDt3rd_FIL_CreateSavefileProperly(void)
// Creates the current savefile in the proper directory, depending on the user's options.
//
void TSoURDt3rd_FIL_CreateSavefileProperly(void)
{
	if (netgame)
		return;

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "CREATING SAVEFILE\n");
#if 0
	memset(savegamename, 0, sizeof(savegamename));
	memset(liveeventbackup, 0, sizeof(liveeventbackup));
#endif
	if (*tsourdt3rd_savefiles_folder == '\0')
		memset(tsourdt3rd_savefiles_folder, 0, sizeof(tsourdt3rd_savefiles_folder));

	TSoURDt3rd_FOL_UpdateSavefileDirectory();

	strcpy(tsourdt3rd_savefiles_folder, va(SAVEGAMEFOLDER PATHSEP "%s%s",
		((tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods) ? ("TSoURDt3rd"PATHSEP) : ("")), timeattackfolder));

	if (!cv_tsourdt3rd_savefiles_storesavesinfolders.value)
	{
		strcpy(liveeventbackup, va("live%s.bkp", timeattackfolder));

		// can't use sprintf since there is %u in savegamename
		strcatbf(savegamename, srb2home, PATHSEP);
		strcatbf(liveeventbackup, srb2home, PATHSEP);
	}

	if (!savemoddata)
	{
		strcpy(savegamename, ((tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods) ? ("tsourdt3rd_"SAVEGAMENAME"%u.ssg") : (SAVEGAMENAME"%u.ssg")));
		strcpy(liveeventbackup, va("%slive"SAVEGAMENAME".bkp", ((tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods) ? ("tsourdt3rd_") : ("")))); // intentionally not ending with .ssg
	}
	else
	{
		strcpy(savegamename,  va("%s%s", ((tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods) ? ("tsourdt3rd_") : ("")), timeattackfolder));
		strlcat(savegamename, "%u.ssg", sizeof(savegamename));
		strcpy(liveeventbackup, va("%slive%s.bkp", ((tsourdt3rd_local.autoloading_mods || tsourdt3rd_local.autoloaded_mods) ? ("tsourdt3rd_") : ("")), timeattackfolder));
	}

	// NOTE: can't use sprintf since there is %u in savegamename
	if (!cv_tsourdt3rd_savefiles_storesavesinfolders.value)
	{
		strcatbf(savegamename, TSoURDt3rd_FOL_ReturnHomepath(), PATHSEP);
		strcatbf(liveeventbackup, TSoURDt3rd_FOL_ReturnHomepath(), PATHSEP);
	}
	else
	{
		strcatbf(savegamename, TSoURDt3rd_FOL_ReturnHomepath(), va(PATHSEP"%s"PATHSEP, tsourdt3rd_savefiles_folder));
		strcatbf(liveeventbackup, TSoURDt3rd_FOL_ReturnHomepath(), va(PATHSEP"%s"PATHSEP, tsourdt3rd_savefiles_folder));
	}
}
