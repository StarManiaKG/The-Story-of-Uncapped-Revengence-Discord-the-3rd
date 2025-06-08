// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-misc.h
/// \brief Commonly used manipulation routines, globally initalized

#ifndef __SMKG_MISC__
#define __SMKG_MISC__

#include "../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

typedef enum
{
	TSOURDT3RD_TERMTABLESEARCH_NORM   = 1<<1,
	TSOURDT3RD_TERMTABLESEARCH_MEMCMP = 1<<2,
	TSOURDT3RD_TERMTABLESEARCH_STRSTR = 1<<3
} tsourdt3rd_termtablesearch_t;

extern char tsourdt3rd_savefiles_folder[256];

// ------------------------ //
//        Functions
// ------------------------ //

char *TSoURDt3rd_M_RemoveStringChars(char *string, const char *c);
INT32 TSoURDt3rd_M_FindWordInTermTable(const char *const *term_table, const char *word, INT32 search_types);
char *TSoURDt3rd_M_WriteVariedLengthString(char *string, boolean decatenate);

const char *TSoURDt3rd_FOL_ReturnHomepath(void);
boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory);
boolean TSoURDt3rd_FOL_CreateDirectory(const char *cpath);
void TSoURDt3rd_FOL_UpdateSavefileDirectory(void);

FILE *TSoURDt3rd_FIL_AccessFile(const char *directory, const char *filename, const char *mode);
boolean TSoURDt3rd_FIL_RenameFile(const char *old_name, const char *new_name);
boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename);
void TSoURDt3rd_FIL_CreateSavefileProperly(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_MISC__
