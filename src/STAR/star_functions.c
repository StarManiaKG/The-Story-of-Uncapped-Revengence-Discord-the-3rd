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
#include "../deh_soc.h"		// savefile variables
#include "../keys.h"		// key variables
#include "../v_video.h"		// video variables

#include "../z_zone.h"		// memory variables

#ifdef HAVE_CURL
#include <curl/curl.h>		// internet variables
#include "../i_threads.h"	// internet variables 2

#include "../fastcmp.h"		// string variables
#endif

///////////////////////////////////
//		ABSOLUTELY UNNESSECARY	 //
//			STAR FUNCTIONS		 //
//				YAY				 //
///////////////////////////////////

// VARIABLES //
#ifdef HAVE_CURL
char *hms_tsourdt3rd_api;

#ifdef HAVE_THREADS
static I_mutex hms_tsourdt3rd_api_mutex;
#endif
#endif

// COMMANDS //
consvar_t cv_tsourdt3rdupdatemessage = CVAR_INIT ("tsourdt3rdupdatemessage", "On", CV_SAVE, CV_OnOff, NULL);

// SAVEDATA //
//
// void STAR_WriteExtraData(void)
// Writes Extra Gamedata to tsourdt3rd.dat
//
void STAR_WriteExtraData(void)
{
    // Initialize Some Variables //
	char *path;
    FILE *tsourdt3rdgamedata;

    // Run Some Checks //
	if ((!eastermode)
		|| (!AllowEasterEggHunt)
		|| (netgame)
		|| (TSoURDt3rd_NoMoreExtras)
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

//
// void STAR_ReadExtraData(void)
// Reads the Info Written to tsourdt3rd.dat
//
void STAR_ReadExtraData(void)
{
    // Initialize Some Variables //
	const char *path;
    FILE *tsourdt3rdgamedata;

	// Run Some Checks //
	if ((!eastermode)
		|| (!AllowEasterEggHunt)
		|| (netgame)
		|| (TSoURDt3rd_NoMoreExtras)
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

//
// void STAR_SetSavefileProperties(void)
// Sets the Current Savefile Name and Position
//
void STAR_SetSavefileProperties(void)
{
	// Make Some Variables //
	INT32 i;

	// Before we Start, Ensure Some Things //
	if (netgame)
	{
		CONS_Printf("You can't change this while in a netgame.\n");
		CV_StealthSetValue(&cv_storesavesinfolders, (!cv_storesavesinfolders.value ? 1 : 0));

		return;
	}

	// Erase the Strings, Just in Case //
	if (savegamename != NULL || liveeventbackup != NULL || savegamefolder != NULL)
	{
		for (i = 0; i < 256; i++)
		{
			if (savegamename[i] != '\0')
				savegamename[i] = '\0';
			if (liveeventbackup[i] != '\0')
				liveeventbackup[i] = '\0';
			if (savegamefolder[i] != '\0')
				savegamefolder[i] = '\0';
		}
	}

	// Make the Folder //
	if (cv_storesavesinfolders.value)
	{
		I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER, srb2home), 0755);
		if (useTSOURDT3RDasFileName)
		{
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "TSoURDt3rd", srb2home), 0755);
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "TSoURDt3rd" PATHSEP "%s", srb2home, timeattackfolder), 0755);
		}
		else
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "%s", srb2home, timeattackfolder), 0755);
	}

	// Store our Folder Name in a Variable //
	strcpy(savegamefolder, va(SAVEGAMEFOLDER PATHSEP "%s%s",
		(useTSOURDT3RDasFileName ? ("TSoURDt3rd"PATHSEP) : ("")), timeattackfolder));
	
	// Store our Savefile Names in a Variable //
	if (!TSoURDt3rd_LoadedGamedataAddon)
	{
		strcpy(savegamename, (useTSOURDT3RDasFileName ? ("tsourdt3rd_"SAVEGAMENAME"%u.ssg") : (SAVEGAMENAME"%u.ssg")));
		strcpy(liveeventbackup, va("%slive"SAVEGAMENAME".bkp", (useTSOURDT3RDasFileName ? ("tsourdt3rd_") : ("")))); // intentionally not ending with .ssg
	}
	else
	{
		strcpy(savegamename,  va("%s%s", (useTSOURDT3RDasFileName ? ("tsourdt3rd_") : ("")), timeattackfolder));
		strlcat(savegamename, "%u.ssg", sizeof(savegamename));

		strcpy(liveeventbackup, va("%slive%s.bkp", (useTSOURDT3RDasFileName ? ("tsourdt3rd_") : ("")), timeattackfolder));
	}

	// Merge our Variables Together //
	// NOTE: can't use sprintf since there is %u in savegamename
#ifdef DEFAULTDIR
	if (!cv_storesavesinfolders.value)
	{
		strcatbf(savegamename, srb2home, PATHSEP);
		strcatbf(liveeventbackup, srb2home, PATHSEP);
	}
	else
	{
		strcatbf(savegamename, srb2home, va(PATHSEP"%s"PATHSEP, savegamefolder));
		strcatbf(liveeventbackup, srb2home, va(PATHSEP"%s"PATHSEP, savegamefolder));
	}

#else

	if (!cv_storesavesinfolders.value)
	{
		strcatbf(savegamename, userhome, PATHSEP);
		strcatbf(liveeventbackup, userhome, PATHSEP);
	}
	else
	{
		strcatbf(savegamename, userhome, va(PATHSEP"%s"PATHSEP, savegamefolder));
		strcatbf(liveeventbackup, userhome, va(PATHSEP"%s"PATHSEP, savegamefolder));
	}
#endif
}

// MESSAGES //
//
// void STAR_Tsourdt3rdEventMessage(INT32 choice)
// Displays a Message on the Screen Asking About Engaging in TSoURDt3rd Events
//
void STAR_Tsourdt3rdEventMessage(INT32 choice)
{
	// Yes //
	if (choice == 'y' || choice == KEY_ENTER)
	{
		S_StartSound(NULL, sfx_spdpad);
		COM_BufInsertText("addfile tsourdt3rdextras.pk3\n");
		
		return;
	}

	// No //
	S_StartSound(NULL, sfx_adderr);
	aprilfoolsmode = false;
	eastermode = false;
	xmasmode = false;

	TSoURDt3rd_LoadExtras = false;
	return;
}

// ONLINE INFO //
//
// static void STAR_SetAPI(char *API)
// Sets the Website API to Use
//
#ifdef HAVE_CURL
static void STAR_SetAPI(char *API)
{
#ifdef HAVE_THREADS
	I_lock_mutex(&hms_tsourdt3rd_api_mutex);
#endif
	{
		free(hms_tsourdt3rd_api);
		hms_tsourdt3rd_api = API;
	}
#ifdef HAVE_THREADS
	I_unlock_mutex(hms_tsourdt3rd_api_mutex);
#endif
}

//
// void STAR_FindAPI(const char *API)
// Finds the Specified Website API
//
// STAR NOTE: THIS MUST BE RAN BEFORE STAR_GrabStringFromWebsite()
//
void STAR_FindAPI(const char *API)
{
	// Find Our Website //
#ifdef HAVE_THREADS
	I_spawn_thread(
			"grab-tsourdt3rd-stuff",
			(I_thread_fn)STAR_SetAPI,
			strdup(API)
	);
#else
	STAR_SetAPI(strdup(API));
#endif
}

//
// boolean STAR_GrabStringFromWebsite(const char *API, char *URL, char *INFO, boolean verbose)
// Try to Grab Info From Websites, Returns 'true' if it Does
//
boolean STAR_GrabStringFromWebsite(const char *API, char *URL, char *INFO, boolean verbose)
{
	// Make Variables //
	CURL *curl = curl_easy_init();
	CURLcode res;
	
	FILE *fp;
	char outfilename[256];

	char finalURL[256];
	char finalINFO[256];

	// Create the File //
	strcpy(outfilename, va("%s"PATHSEP"%s", srb2home, "tsourdt3rd_webinfo.html"));
	fp = fopen(outfilename, "w+");

	// Find the API //
	while (hms_tsourdt3rd_api == NULL)
		STAR_FindAPI(API);

	// Print Words //
	if (verbose)
		CONS_Printf("STAR_GrabStringFromWebsite(): Attempting to grab string %s from website %s using provided api %s...\n", INFO, URL, API);

	// Do Our Website Stuffs //
	if (curl)
	{	
		// Combine the Website Strings
		snprintf(finalURL, 256, "%s%s", API, URL);
			
		// Grab the Info
		fseek(fp, 0, SEEK_SET);
		curl_easy_setopt(curl, CURLOPT_URL, finalURL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		
		// Use CURL to Perform Actions
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			CONS_Printf("STAR_GrabStringFromWebsite() - curl_easy_perform(): Failed to grab website info.\n");
		else
		{
			// Read Strings
			fseek(fp, 0, SEEK_SET);
			while (fgets(finalINFO, 256, fp) != NULL)
			{
				fread(finalINFO, 0, 0, fp);
				
				if (fastcmp(finalINFO, INFO))
				{
					// Close the File, End Curl, and We're Done :)
					if (verbose)
						CONS_Printf("STAR_GrabStringFromWebsite(): Found the info!\n");
					fclose(fp);
		
					remove(outfilename);
					curl_easy_cleanup(curl);

					return true;
				}
			}
		}
	}

	// We Failed Somewhere, but we Still Have to Close the File and End Curl //
	fclose(fp);
		
	remove(outfilename);
	curl_easy_cleanup(curl);

	return false;
}

#endif // HAVE_CURL