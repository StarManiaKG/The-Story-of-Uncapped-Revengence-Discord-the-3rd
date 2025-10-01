// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_misc.c
/// \brief Commonly used manipulation routines

#include "smkg-m_misc.h"

#include "../ss_main.h"
#include "../core/smkg-d_main.h"
#include "../core/smkg-g_game.h" // savedata handling //

#include "../../d_main.h"
#include "../../g_game.h"
#include "../../filesrch.h"
#include "../../i_system.h"
#include "../../m_misc.h"
#include "../../z_zone.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// char *TSoURDt3rd_M_RemoveStringChars(char *string, const char *c)
// Removes the given chars, 'c', from the given string.
//
char *TSoURDt3rd_M_RemoveStringChars(char *string, const char *c)
{
	char *tmp, *p;
	INT32 i = 0;

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
	if (term_table == NULL)
		return -1;

	for (INT32 val = 0; term_table[val]; val++)
	{
		const char *term = term_table[val];
		if ((search_types & TSOURDT3RD_TERMTABLESEARCH_MEMCMP) && memcmp(word, term, sizeof(word)-1) == 0)
			return val;
		if ((search_types & TSOURDT3RD_TERMTABLESEARCH_NORM) && !stricmp(word, term))
			return val;
		if ((search_types & TSOURDT3RD_TERMTABLESEARCH_STRSTR) && strstr(word, term))
			return val;
	}
	return -1;
}

//
// char *TSoURDt3rd_M_WriteVariedLengthString(char *string, UINT32 decate_len, boolean decate)
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
// const char *TSoURDt3rd_FOL_ReturnHomepath_SRB2(void)
// Returns the home path of the user's SRB2 folder.
//
const char *TSoURDt3rd_FOL_ReturnHomepath_SRB2(void)
{
#ifdef DEFAULTDIR
	return srb2home;
#else
	return userhome;
#endif
}

//
// const char *TSoURDt3rd_FOL_ReturnHomepath_Build(void)
// Returns the path of the SRB2 home path's TSoURDt3rd folder.
//
const char *TSoURDt3rd_FOL_ReturnHomepath_Build(void)
{
	return tsourdt3rd_home_dir;
}

//
// boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory)
// Returns true if it is a directory, false if not. Simple as that.
//
boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory)
{
	INT32 stat = pathisdirectory(directory);
	return (stat == 1);
}

//
// boolean TSoURDt3rd_FOL_CreateDirectory(const char *directory)
// Creates the directory given. Strings behind a 'PATHSEP' will also be created along the way.
//
boolean TSoURDt3rd_FOL_CreateDirectory(const char *directory)
{
	char cur_path[MAX_WADPATH];
	INT32 word = 0;

	if (directory == NULL || *directory == '\0')
	{
		// No directory was even given!
		return false;
	}
	else if (TSoURDt3rd_FOL_DirectoryExists(directory))
	{
		// The exact directory already exists, no need to create it!
		return true;
	}

	do
	{
		cur_path[word++] = *directory;
		cur_path[word] = '\0'; // -- Null-terminate, to avoid corrupted data

		// We need to make sure the next character isn't a null-terminator.
		// So, you know, we can keep getting data.
		// And so the loop can quit if we need it to.
		if (directory[1] != '\0')
		{
			// If directory hasn't stumbled onto pathsep yet,
			// or the next character is a pathsep,
			// then we keep eating the string data.
			if (*directory != PATHSEP[0] || directory[1] == PATHSEP[0])
			{
				++directory;
				continue;
			}
		}
		++directory;

		if (TSoURDt3rd_FOL_DirectoryExists(cur_path) == false)
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_DEBUG, "Directory '%s' doesn't exist, creating it...\n", cur_path);
			I_mkdir(cur_path, 0755);
		}
	} while (*directory != '\0');
	return TSoURDt3rd_FOL_DirectoryExists(cur_path);
}

//
// FILE *TSoURDt3rd_FIL_AccessFile(const char *directory, const char *filename, const char *mode)
// Access a file in the path specified.
//
// If the directory given doesn't exist, it will be created.
// If the file doesn't exist, it will also be created.
//
FILE *TSoURDt3rd_FIL_AccessFile(const char *directory, const char *filename, const char *mode)
{
	FILE *handle;
	char full_path[MAX_WADPATH];

	if (directory == NULL || filename == NULL)
	{
		return NULL;
	}

	// Get the directory and make sure that it exists first.
	// Afterwards, concat the filename...
	snprintf(full_path, MAX_WADPATH, "%s" PATHSEP, directory);
	if (TSoURDt3rd_FOL_CreateDirectory(full_path) == false)
	{
		return NULL;
	}
	strlcat(full_path, filename, MAX_WADPATH);

	// ...Then, get the file!
	handle = fopenfile(full_path, mode);
	if (handle == NULL)
	{
		handle = fopenfile(full_path, "rw+");
		if (handle != NULL)
		{
			fclose(handle);
			handle = fopenfile(full_path, mode);
		}
	}
	return handle;
}

//
// FILE *TSoURDt3rd_FIL_AccessFile_Build(const char *directory, const char *filename, const char *mode)
//
// Accesses a file, but is meant for TSoURDt3rd's older files.
// If they're not found in the TSoURDt3rd (TSOURDT3RD_APP) folder, it moves them to that folder.
//
FILE *TSoURDt3rd_FIL_AccessFile_Build(const char *directory, const char *filename, const char *mode)
{
	if (FIL_FileExists(filename))
	{
		// -- Move the file from where-ever it was before to the TSoURDt3rd folder.
		// -- Modern and cleaner locations for the win!
		char new_filename[MAX_WADPATH];
		snprintf(new_filename, MAX_WADPATH, pandf, TSoURDt3rd_FOL_ReturnHomepath_Build(), filename);
		TSoURDt3rd_FIL_RenameFile(filename, new_filename);
	}
	return TSoURDt3rd_FIL_AccessFile(directory, filename, mode);
}

//
// size_t TSoURDt3rd_FIL_ReadFileContents(FILE *file_handle, UINT8 **buffer, INT32 tag)
//
// Reads content within a file and inputs it into a buffer.
// Returns the bytes read, or an error if one occured.
//
// SOURCES:
//	- FIL_ReadFile() in 'm_misc.c'
//
size_t TSoURDt3rd_FIL_ReadFileContents(FILE *file_handle, UINT8 **buffer, INT32 tag)
{
	size_t count, length;
	UINT8 *buf;

	if (file_handle == NULL)
	{
		return 0;
	}

	fseek(file_handle, 0, SEEK_END);
	length = ftell(file_handle);
	fseek(file_handle, 0, SEEK_SET);

	buf = Z_Malloc(length + 1, tag, NULL);
	count = fread(buf, 1, length, file_handle);

	if (count < length)
	{
		Z_Free(buf);
		return 0;
	}

	// append 0 byte for script text files
	buf[length] = 0;

	*buffer = buf;
	return length;
}

//
// boolean TSoURDt3rd_FIL_RenameFile(const char *old_name, const char *new_name)
//
// Renames the file specified.
// This can also be used to move files to new directories.
//
// Returns true if it renamed/moved the file, false otherwise.
//
boolean TSoURDt3rd_FIL_RenameFile(const char *old_name, const char *new_name)
{
	int result = rename(old_name, new_name);
	return (result == 0);
}

//
// boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename)
//
// Removes a file in the path specified.
// Returns true if it removed the file, false otherwise.
//
boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename)
{
	const char *full_path = va("%s" PATHSEP "%s", directory, filename);
	int result = remove(full_path);
	return (result == 0);
}

//
// void TSoURDt3rd_FIL_CreateSavefileProperly(void)
// Creates the current savefile in the proper directory, depending on the user's options.
//
void TSoURDt3rd_FIL_CreateSavefileProperly(void)
{
	// get the appropriate directory
	TSoURDt3rd_G_UpdateSaveDirectory();

	// create the savefiles
	if (!savemoddata)
	{
		strlcpy(savegamename, SAVEGAMENAME "%u.ssg", sizeof(savegamename));
		sprintf(liveeventbackup, "live" SAVEGAMENAME ".bkp"); // intentionally not ending with .ssg
	}
	else
	{
		strcpy(savegamename, timeattackfolder);
		strlcat(savegamename, "%u.ssg", sizeof(savegamename));
		sprintf(liveeventbackup, "live%s.bkp", timeattackfolder); // intentionally not ending with .ssg
	}

	// NOTE: can't use sprintf since there is %u in savegamename
	strcatbf(savegamename, tsourdt3rd_savefile_dir, tsourdt3rd_savedata_prefix);
	strcatbf(liveeventbackup, tsourdt3rd_savefile_dir, tsourdt3rd_savedata_prefix);

#ifdef TSOURDT3RD_DEBUGGING
	// print debugging stuff
	STAR_CONS_Printf(STAR_CONS_DEBUG, "\nTSOURDT3RD SAVEFILENAME & SAVEFOLDER\n");
	STAR_CONS_Printf(STAR_CONS_DEBUG, "\t - savegamename: %s\n",            savegamename);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "\t - liveeventbackup: %s\n",         liveeventbackup);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "\t - tsourdt3rd_savefile_dir: %s\n", tsourdt3rd_savefile_dir);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "\t - tsourdt3rd_save_dir: %s\n",     tsourdt3rd_save_dir);
	STAR_CONS_Printf(STAR_CONS_NONE,  "\n");
#endif
}
