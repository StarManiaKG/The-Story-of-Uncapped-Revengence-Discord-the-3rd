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

#include "smkg-misc.h"
#include "../doomstat.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// char *STAR_M_RemoveStringChars(char *string, const char *c)
// Removes the given chars, 'c', from the given string.
//
char *STAR_M_RemoveStringChars(char *string, const char *c)
{
	INT32 i = 0, j = 0, k = 0;
	char buf[128];

	if (!string || *string == '\0')
		return NULL;

	while (string[i])
	{
		if (!c[k])
		{
			buf[j++] = string[i++];
			k = 0;
		}

		if (string[i] == c[k++])
		{
			i++;
			k = 0;
		}
	}
	buf[j] = '\0'; // Null-terminate at the end

	sprintf(string, "%s", buf);
	return string;
}
