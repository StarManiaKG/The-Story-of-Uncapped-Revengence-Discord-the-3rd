// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_misc.h
/// \brief Commonly used manipulation routines, globalized

#ifndef __SMKG_M_MISC__
#define __SMKG_M_MISC__

#include "../smkg-defs.h"

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

// ------------------------ //
//        Functions
// ------------------------ //

char *TSoURDt3rd_M_RemoveStringChars(char *string, const char *c);
INT32 TSoURDt3rd_M_FindWordInTermTable(const char *const *term_table, const char *word, INT32 search_types);
char *TSoURDt3rd_M_WriteVariedLengthString(char *string, UINT32 decate_len, boolean decate);

//
// Folders
//

const char *TSoURDt3rd_FOL_ReturnHomepath_SRB2(void);
const char *TSoURDt3rd_FOL_ReturnHomepath_Build(void);

boolean TSoURDt3rd_FOL_DirectoryExists(const char *directory);
boolean TSoURDt3rd_FOL_CreateDirectory(const char *directory);

//
// Files
//

FILE *TSoURDt3rd_FIL_AccessFile(const char *directory, const char *filename, const char *mode);
FILE *TSoURDt3rd_FIL_AccessFile_Build(const char *directory, const char *filename, const char *mode);

size_t TSoURDt3rd_FIL_ReadFileContents(FILE *file_handle, UINT8 **buffer, INT32 tag);

boolean TSoURDt3rd_FIL_RenameFile(const char *old_name, const char *new_name);
boolean TSoURDt3rd_FIL_RemoveFile(const char *directory, const char *filename);

void TSoURDt3rd_FIL_CreateSavefileProperly(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_M_MISC__
