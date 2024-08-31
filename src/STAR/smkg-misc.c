// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-misc.c
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

#include "smkg-misc.h"
#include "ss_main.h"

#include "../d_main.h"
#include "../i_system.h"
#include "../m_misc.h"

// ------------------------ //
//        Functions
// ------------------------ //

// =======
// STRINGS
// =======

//
// char *STAR_M_RemoveStringChars(char *string, const char *c)
// Removes the given chars, 'c', from the given string.
//
char *STAR_M_RemoveStringChars(char *string, const char *c)
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

// =======
// FOLDERS
// =======

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
	if (lstat(directory, &given_directory) == 0 && S_ISDIR(given_directory.st_mode))
#else
	if (stat(directory, &given_directory) == 0 && S_ISDIR(given_directory.st_mode))
#endif
		return true;
	return false;
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
	INT32 i, j;
	const char *latest_directory = TSoURDt3rd_FOL_ReturnHomepath();
	char cur_path[512] = "";

	if (!cpath || *cpath == '\0')
		return false;

	if (!strchr(cpath, PATHSEP[0]))
	{
		I_mkdir(va("%s" PATHSEP "%s", latest_directory, cpath), 0755);
		return true;
	}

	for (i = 0, j = 0; cpath[i]; i++, j++)
	{
		if (cpath[i] != PATHSEP[0])
		{
			cur_path[j] = cpath[i];

			if (cpath[i+1] == '\0')
			{
				I_mkdir(va("%s" PATHSEP "%s", latest_directory, cur_path), 0755);
				break;
			}

			continue;
		}
		cur_path[j] = '\0'; // Null-terminate at the end

		latest_directory = va("%s" PATHSEP "%s", latest_directory, cur_path);
		if (!TSoURDt3rd_FOL_DirectoryExists(latest_directory))
		{
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Directory %s doesn't exist, creating it...\n", latest_directory);
			I_mkdir(latest_directory, 0755);
		}

		memset(cur_path, 0, sizeof(cur_path));
		j = -1;
	}
	return true;
}

// =====
// FILES
// =====

//
// FILE *TSoURDt3rd_FIL_CreateFile(const char *directory, const char *filename, const char *mode)
//
// Creates a file in the path specified.
// If the directory given isn't found, the function will create those directories first,
//	then the file.
//
FILE *TSoURDt3rd_FIL_CreateFile(const char *directory, const char *filename, const char *mode)
{
	char full_path[256];
	FILE *handle;

	sprintf(full_path, "%s" PATHSEP "%s" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), directory, filename);
	TSoURDt3rd_FOL_CreateDirectory(directory);

	handle = fopen(full_path, mode);
	return handle;
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
	if (!remove(full_path))
		return true;

	return false;
}
