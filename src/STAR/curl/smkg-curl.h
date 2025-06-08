// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-curl.h
/// \brief Easy curl implementation for TSoURDt3rd, global header

#ifndef __SMKG_CURL__
#define __SMKG_CURL__

#include "../../doomstat.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum
{
	TSOURDT3RD_CURL_SITENOTFOUND, // Couldn't even access website data.
	TSOURDT3RD_CURL_DATANOTFOUND, // Accessed website, didn't find given website data.
	TSOURDT3RD_CURL_DATAFOUND     // Given website data found.
};

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_Curl_FindStringWithinURL(
	FILE       *SITE_DATA_FILE,
	const char *SITE_STRING,
	char       *SITE_URL,
	char      **SITE_RETURN_STRING,
	int        *SITE_RETURNCODE
);

void TSoURDt3rd_CurlRoutine_FindUpdates(void);

#endif // __SMKG_CURL__
