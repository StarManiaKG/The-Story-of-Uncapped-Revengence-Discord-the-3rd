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
boolean GrabbingTSoURDt3rdInfo;
boolean NotifyAboutTSoURDt3rdUpdate = true;

char *hms_tsourdt3rd_api;

#ifdef HAVE_THREADS
static I_mutex hms_tsourdt3rd_api_mutex;
#endif
#endif

// COMMANDS //
consvar_t cv_tsourdt3rdupdatemessage = CVAR_INIT ("tsourdt3rdupdatemessage", "On", CV_SAVE, CV_OnOff, NULL);

// SAVEDATA RELATED //

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

// MESSAGE RELATED //

//
// void STAR_Tsourdt3rdEventMessage(INT32 choice)
// Displays a Message on the Screen Asking About Engaging in TSoURDt3rd Events
//
void STAR_Tsourdt3rdEventMessage(INT32 choice)
{
	if (choice == 'y' || choice == KEY_ENTER)
	{
		S_StartSound(NULL, sfx_spdpad);
		COM_BufInsertText("addfile tsourdt3rdextras.pk3\n");
		
		return;
	}

	S_StartSound(NULL, sfx_adderr);
	aprilfoolsmode = false;
	eastermode = false;
	xmasmode = false;

	TSoURDt3rd_LoadExtras = false;
	return;
}

// ONLINE RELATED //

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
void STAR_FindAPI(const char *API)
{
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
// void STAR_GrabFromTsourdt3rdGithub(char *URL)
// Try to Grab Extra Info From the Github of TSoURDt3rd
//
void STAR_GrabFromTsourdt3rdGithub(char *URL)
{
	CURL *curl = curl_easy_init();
	CURLcode res;
	
	FILE *fp;
	char outfilename[256];

	char finalURL[256];
	char finalINFO[256];

	char *tsourdt3rdIdentifyLine;

	GrabbingTSoURDt3rdInfo = true;
	if (curl && hms_tsourdt3rd_api != NULL)
	{
		strcpy(outfilename, va("%s"PATHSEP"%s", srb2home, "tsourdt3rdwebinfo.html"));
		fp = fopen(outfilename, "w+");
		
		snprintf(finalURL, 256, "%s%s", hms_tsourdt3rd_api, URL);
	
		fseek(fp, 0, SEEK_SET);
		curl_easy_setopt(curl, CURLOPT_URL, finalURL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
       	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
       	
       	res = curl_easy_perform(curl);
       	if (res != CURLE_OK)
	   		CONS_Printf("curl_easy_perform(): Failed to Grab TSoURDt3rd Web Info.\n");
	  	else
	  	{
			tsourdt3rdIdentifyLine = va("#define TSOURDT3RDVERSION \"%s\"", TSOURDT3RDVERSION);
			
			fseek(fp, 0, SEEK_SET);
			while (fgets(finalINFO, 256, fp) != NULL)
			{
				fread(finalINFO, 0, 0, fp);
				
				if (NotifyAboutTSoURDt3rdUpdate && fasticmp(finalINFO, tsourdt3rdIdentifyLine))
				{
					NotifyAboutTSoURDt3rdUpdate = false;
					break;
				}
			}
	  	}
	  	fclose(fp);

		remove(outfilename);
		curl_easy_cleanup(curl);
		
		GrabbingTSoURDt3rdInfo = false;
	}
	else if (!curl)
		GrabbingTSoURDt3rdInfo = false;
}
#endif // HAVE_CURL