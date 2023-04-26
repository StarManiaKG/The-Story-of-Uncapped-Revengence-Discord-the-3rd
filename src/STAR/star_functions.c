// SONIC ROBO BLAST 2 - TSOURDT3RD EDITION
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by Star "Guy Who Named Another Script After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_functions.c
/// \brief Contains all the Info Portraying to
///        TSoURDt3rd's Extra STAR Functions

#include <time.h>

#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../m_misc.h" 		// extra file functions
#include "star_vars.h" 		// star variables
#include "../m_menu.h" 		// egg variables
#include "../d_main.h" 		// event variables

#include "../z_zone.h"		// memory variables

#ifdef HAVE_CURL
#include <curl/curl.h>		// internet variables
#include "../i_threads.h"	// internet variables 2
#endif

///////////////////////////////////
//		ABSOLUTELY UNNESSECARY	 //
//			STAR FUNCTIONS		 //
//				YAY				 //
///////////////////////////////////

// VARIABLES //
const char *tsourdt3rdLocation = "https://github.com/StarManiaKG/The-Story-of-Uncapped-Revengence-Discord-the-3rd/";

static char *hms_tsourdt3rd_api;
#ifdef HAVE_THREADS
static I_mutex hms_tsourdt3rd_api_mutex;
#endif

// SAVEDATA RELATED //
void STAR_WriteExtraData(void)
{
    // Initialize Some Variables //
	char *path;
    FILE *tsourdt3rdgamedata;

    // Run Some Checks //
	if ((!eastermode)
		|| (!AllowEasterEggHunt)
		|| (netgame)
		|| (modifiedgame)
		|| (autoloaded))

		return;

    // Open The File //
	path = va("%s"PATHSEP"%s", srb2home, "tsourdt3rd.dat");
	tsourdt3rdgamedata = fopen(path, "w+");

	// Write To The File //
	putw(currenteggs, tsourdt3rdgamedata);
	putw(foundeggs, tsourdt3rdgamedata);

	// Close The File //
    fclose(tsourdt3rdgamedata);
}

void STAR_ReadExtraData(void)
{
    // Initialize Some Variables //
	const char *path;
    FILE *tsourdt3rdgamedata;

	// Run Some Checks //
	if ((!eastermode)
		|| (!AllowEasterEggHunt)
		|| (netgame)
		|| (modifiedgame)
		|| (autoloaded))

		return;
	
	// Find The File //
	path = va("%s"PATHSEP"%s", srb2home, "tsourdt3rd.dat");
    tsourdt3rdgamedata = fopen(path, "r");
    if (!tsourdt3rdgamedata)
		return;
    
	// Read Things Within The File //
	currenteggs = getw(tsourdt3rdgamedata);
	foundeggs = getw(tsourdt3rdgamedata);

    // Close the File //
    fclose(tsourdt3rdgamedata);
}

// ONLINE RELATED //
// Sets the Website API to Use
#ifdef HAVE_CURL
static void STAR_SetAPI(char *api)
{
#ifdef HAVE_THREADS
	I_lock_mutex(&hms_tsourdt3rd_api_mutex);
#endif
	{
		free(hms_tsourdt3rd_api);
		hms_tsourdt3rd_api = api;
	}
#ifdef HAVE_THREADS
	I_unlock_mutex(hms_tsourdt3rd_api_mutex);
#endif
}

// Finds the Specified Website API
static void STAR_FindAPI(const char *api)
{
#ifdef HAVE_THREADS
	I_spawn_thread(
			"grab-tsourdt3rd-stuff",
			(I_thread_fn)STAR_SetAPI,
			strdup(api)
	);
#else
	STAR_SetAPI(strdup(api));
#endif
}

// Try to Grab Something From the Github of TSoURDt3rd
void STAR_GrabFromTsourdt3rdGithub(char *tsourdt3rdURL)
{
	CURL *curl;
	curl = curl_easy_init();

	STAR_FindAPI(tsourdt3rdLocation);
	strlcat(tsourdt3rdURL, va("%s", hms_tsourdt3rd_api), sizeof(tsourdt3rdURL));

	//curl_easy_setopt(curl, CURLOPT_URL, tsourdt3rdURL);
	//CONS_Printf("%p\n", curl);
	curl_easy_setopt(curl, CURLOPT_READDATA, tsourdt3rdURL);
	CONS_Printf("%p\n", curl);
}

// Combine All of the Above Into One Easy Function
void STAR_DoOnlineStuff(void)
{
	char *URL;
	URL = malloc(strlen("tree/main/src/STAR/version") + 1);
	strlcat(URL, "tree/main/src/STAR/star_webinfo.h", sizeof(URL));
	
	STAR_GrabFromTsourdt3rdGithub(URL);
}
#endif // HAVE_CURL