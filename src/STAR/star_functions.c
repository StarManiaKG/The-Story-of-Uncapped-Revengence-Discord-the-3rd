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
#include "../m_menu.h" 		// egg and command variables
#include "../d_main.h" 		// event variables
#include "../deh_soc.h"		// savefile variables
#include "../keys.h"		// key variables
#include "../v_video.h"		// video variables
#include "../i_video.h"
#include "../filesrch.h"	// file variables
#include "../r_skins.h"		// skin variables
#include "../sounds.h"		// sound variables
#include "../deh_tables.h"	// sprite/table variables
#include "../m_random.h"	// m_random functions
#include "../z_zone.h"		// memory variables

#ifdef HAVE_CURL
#include <curl/curl.h>		// internet variables
#include "../i_threads.h"	// internet variables 2

#include "../fastcmp.h"		// string variables
#endif

#ifdef HAVE_SDL
#include "SDL.h"			// sdl variables
#include "../sdl/sdlmain.h"	// sdl variables 2
#endif

//////////////////////////////////////
//		ABSOLUTELY UNNESSECARY	 	//
//			STAR FUNCTIONS		 	//
//				YAY				 	//
//////////////////////////////////////

// VARIABLES //
#ifdef HAVE_CURL
char *hms_tsourdt3rd_api;

#ifdef HAVE_THREADS
static I_mutex hms_tsourdt3rd_api_mutex;
#endif
#endif

// STRUCTS //
struct TSoURDt3rdInfo_s TSoURDt3rdInfo;

// COMMANDS //
consvar_t cv_loadingscreen = CVAR_INIT ("loadingscreen", "Off", CV_SAVE, CV_OnOff, NULL);

static CV_PossibleValue_t loadingscreenbackground_t[] = {
	{0, "Off"},
	{1, "Dynamic"},

	{2, "Intermission"},
	{3, "Retro"},

	{4, "Greenflower"},
	{5, "Techno Hill"},
	{6, "Deep Sea"},
	{7, "Castle"},
	{8, "Arid Canyon"},
	{9, "Red Volcano"},
	{10, "Egg Rock"},
	{11, "Black Core"},

	{12, "Frozen Hill"},
	{13, "Pipe Tower"},
	{14, "Fortress"},
	{15, "Retro Techno"},

	{16, "Halloween"},
	{17, "Aerial"},
	{18, "Temple"},

	{19, "NiGHTs"},
	{20, "Black Hole"},

	{21, "Random"},
	{0, NULL}};

consvar_t cv_loadingscreenimage = CVAR_INIT ("loadingscreenimage", "Intermission", CV_SAVE, loadingscreenbackground_t, NULL);

consvar_t cv_tsourdt3rdupdatemessage = CVAR_INIT ("tsourdt3rdupdatemessage", "On", CV_SAVE, CV_OnOff, NULL);

// GAME //
//
// void STAR_LoadingScreen(boolean opengl)
// Displays a Loading Screen
//
size_t ls_count = 0;
UINT8 ls_percent = 0;

INT32 STAR_loadingscreentouse = 0;

void STAR_LoadingScreen(boolean opengl)
{
	char s[16];
	INT32 x, y;

	static const char *gstartuplumpnumtype[] = {
		[2] = "SRB2BACK",	// SRB2 titlecard background
		"DEFLTFLR",
		
		"GFZL",
		"THZL",
		"DSZL",
		"CEZL",
		"ACZL",
		"RVZL",
		"ERZL",
		"BCZL",

		"FHZL",
		"PTZL",
		"FFZL",
		"TLZL",

		"HHZL",
		"AGZL",
		"ATZL",

		"ASSL",
		"BHZL",
		NULL
	};

	I_OsPolling();
	//CON_Drawer(); // console shouldn't appear while in a loading screen, honestly

	if (opengl)
		sprintf(s, "%d%%", (++ls_percent)<<1);
	x = BASEVIDWIDTH/2;
	y = BASEVIDHEIGHT/2;
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31); // Black background to match fade in effect
	if (cv_loadingscreenimage.value)
	{
		// Dynamic //
		if (cv_loadingscreenimage.value == 1)
		{
			// In-Game
			if (Playing())
			{
				// Modified-Game Images
				if (autoloaded || savemoddata || modifiedgame)
					STAR_loadingscreentouse = 3;

				// Vanilla/Unmodified-Game Images
				else
				{
					switch (gamemap)
					{
						// GFZ
						case 1:
						case 2:
						case 3:
						{
							STAR_loadingscreentouse = 4;
							break;
						}
						
						// THZ
						case 4:
						case 5:
						case 6:
						{
							STAR_loadingscreentouse = 5;
							break;
						}
			
						// DSZ
						case 7:
						case 8:
						case 9:
						{
							STAR_loadingscreentouse = 6;
							break;
						}

						// CEZ
						case 10:
						case 11:
						case 12:
						{
							STAR_loadingscreentouse = 7;
							break;
						}

						// ACZ
						case 13:
						case 14:
						case 15:
						{
							STAR_loadingscreentouse = 8;
							break;
						}

						// RVZ
						case 16:
						{
							STAR_loadingscreentouse = 9;
							break;
						}

						// ERZ
						case 22:
						case 23:
						{
							STAR_loadingscreentouse = 10;
							break;
						}

						// BCZ
						case 25:
						case 26:
						case 27:
						{
							STAR_loadingscreentouse = 11;
							break;
						}

						// FHZ
						case 30:
						{
							STAR_loadingscreentouse = 12;
							break;
						}

						// PTZ
						case 31:
						{
							STAR_loadingscreentouse = 13;
							break;
						}

						// FFZ
						case 32:
							STAR_loadingscreentouse = 14;
							break;
			
						// TLZ
						case 33:
							STAR_loadingscreentouse = 15;
							break;

						// HHZ
						case 40:
						{
							STAR_loadingscreentouse = 16;
							break;
						}

						// AGZ
						case 41:
						{
							STAR_loadingscreentouse = 17;
							break;
						}

						// ATZ
						case 42:
						{
							STAR_loadingscreentouse = 18;
							break;
						}

						// All Special Stages
						case 50:
						case 51:
						case 52:
						case 53:
						case 54:
						case 55:
						case 56:

						case 60:
						case 61:
						case 62:
						case 63:
						case 64:
						case 65:
						case 66:

						case 70:
						case 71:
						case 72:
						case 73:
						{
							STAR_loadingscreentouse = 19;
							break;
						}

						// BHZ
						case 57:
						{
							STAR_loadingscreentouse = 20;
							break;
						}

						// CTF, Match, and Custom Maps
						case 280:
						case 281:
						case 282:
						case 283:
						case 284:
						case 285:
						case 286:
						case 287:
						case 288:

						case 532:
						case 533:
						case 534:
						case 535:
						case 536:
						case 537:
						case 538:
						case 539:
						case 540:
						case 541:
						case 542:
						case 543:

						default:
						{
							STAR_loadingscreentouse = 3;
							break;
						}
					}
				}
			}

			// Not In-Game
			else
				STAR_loadingscreentouse = 2;
		}

		// Random //
		else if (cv_loadingscreenimage.value == 21 && !STAR_loadingscreentouse)
			STAR_loadingscreentouse = M_RandomRange(2, 20);

		// Apply the Image, and We're Good :) //
		V_DrawPatchFill(W_CachePatchName(
			((cv_loadingscreenimage.value == 1 || cv_loadingscreenimage.value == 21) ? gstartuplumpnumtype[STAR_loadingscreentouse] : gstartuplumpnumtype[cv_loadingscreenimage.value]),
			(PU_CACHE)));
	}

	M_DrawTextBox(x-58, y-8, 13, 1);
	if (opengl)
	{
		V_DrawString(x-50, y, menuColor[cv_menucolor.value], "Loading...");
		V_DrawRightAlignedString(x+50, y, menuColor[cv_menucolor.value], s);
	}
	else
		V_DrawCenteredString(x, y, menuColor[cv_menucolor.value], "Loading...");

	I_UpdateNoVsync();
}

// SAVEDATA //
//
// void STAR_WriteExtraData(void)
// Writes Extra Gamedata to tsourdt3rd.dat
//
void STAR_WriteExtraData(void)
{
    // Initialize Some Variables //
    FILE *tsourdt3rdgamedata;
	const char *path;

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
	// Easter Eggs
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
    FILE *tsourdt3rdgamedata;
	const char *path;

	// Find The File //
	path = va("%s"PATHSEP"%s", srb2home, "tsourdt3rd.dat");
    tsourdt3rdgamedata = fopen(path, "r");

	// Run Some Checks //
	if ((!tsourdt3rdgamedata)
		|| (!eastermode)
		|| (!AllowEasterEggHunt)
		|| (netgame)
		|| (TSoURDt3rd_NoMoreExtras)
		|| (autoloaded))

		return;
	
	// Read Things Within The File //
	// Easter Eggs
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
	INT32 a = 0, b = 0, c = 0;

	// Before we Start, Ensure Some Things //
	if (netgame)
	{
		CONS_Printf("You can't change this while in a netgame.\n");
		CV_StealthSetValue(&cv_storesavesinfolders, (!cv_storesavesinfolders.value ? 1 : 0));
		return;
	}

	// Erase the Strings, Just in Case //
	while (savegamename[a] != '\0') { savegamename[a] = '\0'; a++; }
	while (liveeventbackup[b] != '\0') { liveeventbackup[b] = '\0'; b++; }
	while (savegamefolder[c] != '\0') { savegamefolder[c] = '\0'; c++; }

	// Make the Folder //
	if (cv_storesavesinfolders.value)
	{
		I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER, srb2home), 0755);
		if (TSoURDt3rd_useAsFileName)
		{
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "TSoURDt3rd", srb2home), 0755);
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "TSoURDt3rd" PATHSEP "%s", srb2home, timeattackfolder), 0755);
		}
		else
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "%s", srb2home, timeattackfolder), 0755);
	}

	// Store our Folder Name in a Variable //
	strcpy(savegamefolder, va(SAVEGAMEFOLDER PATHSEP "%s%s",
		(TSoURDt3rd_useAsFileName ? ("TSoURDt3rd"PATHSEP) : ("")), timeattackfolder));
	
	// Store our Savefile Names in a Variable //
	if (!TSoURDt3rd_LoadedGamedataAddon)
	{
		strcpy(savegamename, (TSoURDt3rd_useAsFileName ? ("tsourdt3rd_"SAVEGAMENAME"%u.ssg") : (SAVEGAMENAME"%u.ssg")));
		strcpy(liveeventbackup, va("%slive"SAVEGAMENAME".bkp", (TSoURDt3rd_useAsFileName ? ("tsourdt3rd_") : ("")))); // intentionally not ending with .ssg
	}
	else
	{
		strcpy(savegamename,  va("%s%s", (TSoURDt3rd_useAsFileName ? ("tsourdt3rd_") : ("")), timeattackfolder));
		strlcat(savegamename, "%u.ssg", sizeof(savegamename));

		strcpy(liveeventbackup, va("%slive%s.bkp", (TSoURDt3rd_useAsFileName ? ("tsourdt3rd_") : ("")), timeattackfolder));
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

// FILES //
//
// void STAR_DetectFileType(void)
// Detects the Specific File Type of the File Given
//
// Possible Returns:
//	0 - Unsupported
// 	1 - Folder
//
// 	2 - WAD
// 	3 - PK3
//	4 - KART (if enabled)
//
//	5 - LUA
//	6 - SOC
//
//	7 - CFG
//	8 - TXT
//
INT32 STAR_DetectFileType(const char* filename)
{
	if (pathisdirectory(filename) == 1)
		return 1;
	else
	{
		if (!stricmp(&filename[strlen(filename) - 4], ".wad"))
			return 2;
		else if (!stricmp(&filename[strlen(filename) - 4], ".pk3"))
			return 3;
#ifdef USE_KART
		else if (!stricmp(&filename[strlen(filename) - 5], ".kart"))
			return 4;
#endif

		else if (!stricmp(&filename[strlen(filename) - 4], ".lua"))
			return 5;
		else if (!stricmp(&filename[strlen(filename) - 4], ".soc"))
			return 6;
		
		else if (!stricmp(&filename[strlen(filename) - 4], ".cfg"))
			return 7;
		else if (!stricmp(&filename[strlen(filename) - 4], ".txt"))
			return 8;
	}

	return 0;
}

// EVENTS //
//
// void TSoURDt3rd_CheckTime(void)
// Ported from Final Demo, Date Checking is Back!
// This Helps Check the Current Time on the User's Computer!
//
void TSoURDt3rd_CheckTime(void)
{
	time_t t1;
	struct tm* tptr;

	// Do Special Stuff //
	t1 = time(NULL);
	if (t1 != (time_t)-1)
	{
		tptr = localtime(&t1);

		if (tptr)
		{
			// April Fools
			if (tptr->tm_mon == 3 && (tptr->tm_mday >= 1 && tptr->tm_mday <= 3))
			{
				aprilfoolsmode = true;
				modifiedgame = false;
			}

			// Easter (Changes Every Year Though, so just have it for all of April)
			else if ((tptr->tm_mon == 3)
				&& (!M_CheckParm("-noeaster")))
			{
				eastermode = true;
				modifiedgame = false;
			}

			// Christmas Eve to New Years
			else if (((tptr->tm_mon == 11 && tptr->tm_mday >= 24))
				&& (!M_CheckParm("-noxmas")))
			{
				xmasmode = true;
				xmasoverride = true;
				modifiedgame = false;
			}
		}
	}

	// Do Special Paramater Stuff //
	// April Fools
	if (M_CheckParm("-aprilfools"))
	{
		aprilfoolsmode = true;
		modifiedgame = false;
	}

	// Easter
	else if (M_CheckParm("-easter"))
	{
		eastermode = true;
		modifiedgame = false;
	}

	// Christmas
	else if (!eastermode && M_CheckParm("-xmas"))
	{
		xmasmode = true;
		xmasoverride = true;
		modifiedgame = false;
	}

	// Run Special Stuff Functions //
	// April Fools
	if (aprilfoolsmode)
	{
		CONS_Printf("TSoURDt3rd_CheckTime(): April Fools Mode Enabled!\n");
		TSoURDt3rd_LoadExtras = true;
	}

	// Easter
	else if (eastermode)
	{
		CONS_Printf("TSoURDt3rd_CheckTime(): Easter Mode Enabled!\n");
		TSoURDt3rd_LoadExtras = true;
	}

	// Christmas
	else if (xmasmode)
	{
		CONS_Printf("TSoURDt3rd_CheckTime(): Christmas Mode Enabled!\n");
		TSoURDt3rd_LoadExtras = true;
	}
}

// MESSAGES //
//
// void TSoURDt3rd_EventMessage(INT32 choice)
// Displays a Message on the Screen Asking About Engaging in TSoURDt3rd Events
//
void TSoURDt3rd_EventMessage(INT32 choice)
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
}

// THE WORLD WIDE WEB //
#ifdef HAVE_CURL
//
// static void STAR_SetAPI(char *API)
// Sets the Website API to Use
//
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
// INT32 STAR_FindStringOnWebsite(const char *API, char *URL, char *INFO, boolean verbose)
// Tries to Find Info From the Given Website, Returns 2 if it Does
//
// 0 - Couldn't Even Access the Website Info in the First Place.
// 1 - Accessed the Website, but Didn't Find the String.
// 2 - Found the String on the Website!
//
INT32 STAR_FindStringOnWebsite(const char *API, char *URL, char *INFO, boolean verbose)
{
	// Make Variables //
	CURL *curl, *sidecurl;
	CURLcode res;

	FILE *webinfo;
	const char *webinfofilelocation;

	char finalURL[256];
	char finalINFO[256];

	// Create the File //
	webinfofilelocation = va("%s"PATHSEP"%s", srb2home, "tsourdt3rd_grabbedwebsiteinfo.html");
	webinfo = fopen(webinfofilelocation, "w+");

	// Find the API //
	while (hms_tsourdt3rd_api == NULL)
		STAR_FindAPI(API);
	curl = curl_easy_init(); sidecurl = curl_easy_init();

	// Print Words //
	if (verbose)
		CONS_Printf("STAR_FindStringOnWebsite(): Attempting to grab string %s from website %s using provided api %s...\n", INFO, URL, API);

	// Do Our Website Stuffs //
	if (curl && sidecurl)
	{
		// Combine the Website Strings
		snprintf(finalURL, 256, "%s%s", API, URL);

		// Check if the Website Actually Exists
		curl_easy_setopt(sidecurl, CURLOPT_URL, finalURL);
		curl_easy_setopt(sidecurl, CURLOPT_NOBODY, 1);

		res = curl_easy_perform(sidecurl);
		if (res != CURLE_OK)
		{
			CONS_Printf("STAR_FindStringOnWebsite() - Failed to check if website was valid.\n");	

			fclose(webinfo);

			remove(webinfofilelocation);
			curl_easy_cleanup(sidecurl);

			return 0;
		}
		curl_easy_cleanup(sidecurl);
		
		// Grab the Actual Info and Write it to a File
		curl_easy_setopt(curl, CURLOPT_URL, finalURL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, webinfo);

		// Use CURL, Check if Everything Went Through, and Then Perform our Actions
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			CONS_Printf("STAR_FindStringOnWebsite() - curl_easy_perform(): Failed to grab website info.\n");	

			fclose(webinfo);

			remove(webinfofilelocation);
			curl_easy_cleanup(curl);

			return 0;
		}

		// Read Strings
		else
		{
			// Check if the File is Empty
			fseek(webinfo, 0, SEEK_END);
			if (ftell(webinfo) == 0)
			{
				CONS_Printf("STAR_FindStringOnWebsite() - Failed to grab website info.\n");	

				fclose(webinfo);

				remove(webinfofilelocation);
				curl_easy_cleanup(curl);

				return 0;
			}

			// File Isn't Empty, so Actually Set the File Position
			fseek(webinfo, 0, SEEK_SET);

			// Grab the Website Info
			while (fgets(finalINFO, sizeof finalINFO, webinfo) != NULL)
			{
				// We've Found the String, so Close the File, Remove it, Clean up, and We're Done :)
				if (fastcmp(finalINFO, INFO))
				{
					if (verbose)
						CONS_Printf("STAR_FindStringOnWebsite(): Found the info!\n");
					fclose(webinfo);

					remove(webinfofilelocation);
					curl_easy_cleanup(curl);

					return 2;
				}
			}
		}
	}

	// We Failed Somewhere, but we Still Have to Close the File and End Curl //
	fclose(webinfo);
		
	remove(webinfofilelocation);
	curl_easy_cleanup(curl);
	return 1;
}

//
// char *STAR_ReturnStringFromWebsite(const char *API, char *URL, char *RETURNINFO, boolean verbose)
// Tries to Find the Given Info From the Given Website, Returns the String if it Does
//
char finalRETURNINFO[256] = " ";

char *STAR_ReturnStringFromWebsite(const char *API, char *URL, char *RETURNINFO, boolean verbose)
{
	// Make Variables //
	CURL *curl, *sidecurl;
	CURLcode res;

	FILE *webinfo;
	const char *webinfofilelocation;

	char finalURL[256];
	INT32 i = 0;

	// Reset the Main Variable //
	while (finalRETURNINFO[i] != '\0') { finalRETURNINFO[i] = '\0'; i++; }
	strcpy(finalRETURNINFO, " ");

	// Create the File //
	webinfofilelocation = va("%s"PATHSEP"%s", srb2home, "tsourdt3rd_grabbedwebsiteinfo.html");
	webinfo = fopen(webinfofilelocation, "w+");

	// Find the API //
	while (hms_tsourdt3rd_api == NULL)
		STAR_FindAPI(API);
	curl = curl_easy_init(); sidecurl = curl_easy_init();

	// Print Words //
	if (verbose)
		CONS_Printf("STAR_ReturnStringFromWebsite(): Attempting to return string %s from website %s using provided api %s...\n", RETURNINFO, URL, API);

	// Do Our Website Stuffs //
	if (curl && sidecurl)
	{
		// Combine the Website Strings
		snprintf(finalURL, 256, "%s%s", API, URL);

		// Check if the Website Actually Exists
		curl_easy_setopt(sidecurl, CURLOPT_URL, finalURL);
		curl_easy_setopt(sidecurl, CURLOPT_NOBODY, 1);

		res = curl_easy_perform(sidecurl);
		if (res != CURLE_OK)
		{
			CONS_Printf("STAR_ReturnStringFromWebsite() - Failed to check if website was valid.\n");	

			fclose(webinfo);

			remove(webinfofilelocation);
			curl_easy_cleanup(sidecurl);

			return finalRETURNINFO;
		}
		curl_easy_cleanup(sidecurl);

		// Grab the Actual Info and Write it to a File
		curl_easy_setopt(curl, CURLOPT_URL, finalURL);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, webinfo);

		// Use CURL, Check if Everything Went Through, and Then Perform our Actions
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			CONS_Printf("STAR_ReturnStringFromWebsite() - curl_easy_perform(): Failed to grab website info.\n");	

			fclose(webinfo);

			remove(webinfofilelocation);
			curl_easy_cleanup(curl);

			return finalRETURNINFO;
		}

		// Read Strings
		else
		{
			// Check if the File is Empty
			fseek(webinfo, 0, SEEK_END);
			if (ftell(webinfo) == 0)
			{
				CONS_Printf("STAR_ReturnStringFromWebsite() - Failed to grab website info.\n");	

				fclose(webinfo);

				remove(webinfofilelocation);
				curl_easy_cleanup(curl);

				return finalRETURNINFO;
			}

			// File Isn't Empty, so Actually Set the File Position
			fseek(webinfo, 0, SEEK_SET);

			// Find the String in the File
			while (fgets(finalRETURNINFO, sizeof finalRETURNINFO, webinfo) != NULL)
			{
				// We've Found the String, so Close the File, Remove it, Clean up, Return the String, and We're Done :)
				if (fastncmp(finalRETURNINFO, RETURNINFO, sizeof(RETURNINFO)))
				{
					if (verbose)
						CONS_Printf("STAR_ReturnStringFromWebsite(): Found the info!\n");
					fclose(webinfo);

					remove(webinfofilelocation);
					curl_easy_cleanup(curl);

					return finalRETURNINFO;
				}
			}
		}
	}

	// We Failed Somewhere, but we Still Have to Close the File and End Curl //
	fclose(webinfo);

	remove(webinfofilelocation);
	curl_easy_cleanup(curl);
	return finalRETURNINFO;
}

#endif // HAVE_CURL

// SERVERS //
//
// boolean STAR_FindServerInfractions(void)
// Finds Server Infractions. These Can be Related to Having Too Many Skins for Vanilla Servers to Join, and More
//
// STAR NOTE: the 'accurate' variables are used for the M_StartMessage function at the bottom :p of the hook
//		...Except for accuratenumskinsounds. That one's used to properly check skin sound limits.
//
//		However, the 'numstates' and 'numobjects' variables are already accurate, so there's no need to assign
//			those 'accurate' variables.
//
boolean STAR_FindServerInfractions(void)
{
	// Make Variables //
	UINT32 accuratenumskins = (numskins-6);
	sfxenum_t numsounds; UINT32 accuratenumsounds;
	sfxenum_t numskinsounds; UINT32 accuratenumskinsounds;
	spritenum_t totalnumsprites; UINT32 accuratenumsprites;
	playersprite_t totalnumsprite2s; UINT32 accuratenumsprite2s;
	statenum_t numstates;
	mobjtype_t numobjects;

	// Run Some Initial Things //
	// Count the Sounds
	for (numsounds = sfx_freeslot0; numsounds <= sfx_lastfreeslot; numsounds++)
	{
		if (S_sfx[numsounds].priority > 0)
			continue;
		break;
	}
	for (numskinsounds = sfx_skinsoundslot0; numskinsounds <= sfx_lastskinsoundslot; numskinsounds++)
	{
		if (S_sfx[numskinsounds].priority > 0 && S_sfx[numskinsounds].name)
			continue;
		break;
	}
	accuratenumsounds = (numsounds - sfx_freeslot0);
	accuratenumskinsounds = (numskinsounds - sfx_skinsoundslot0);

	// Count the Sprites
	for (totalnumsprites = SPR_FIRSTFREESLOT; totalnumsprites <= SPR_LASTFREESLOT; totalnumsprites++)
	{
		if (used_spr[(totalnumsprites-SPR_FIRSTFREESLOT)/8] & (1<<(totalnumsprites%8)))
			continue;
		break;
	}
	for (totalnumsprite2s = SPR2_FIRSTFREESLOT; totalnumsprite2s < free_spr2; totalnumsprite2s++)
	{
		if (spr2names[totalnumsprite2s])
			continue;
		break;
	}
	accuratenumsprites = (totalnumsprites - SPR_FIRSTFREESLOT);
	accuratenumsprite2s = (totalnumsprite2s - SPR2_FIRSTFREESLOT);

	// Count the States
	for (numstates = 0; numstates < NUMSTATEFREESLOTS; numstates++)
	{
		if (FREE_STATES[numstates])
			continue;
		break;
	}
	
	// Count the Objects
	for (numobjects = 0; numobjects < NUMMOBJFREESLOTS; numobjects++)
	{
		if (FREE_MOBJS[numobjects])
			continue;
		break;
	}

	// Find Some Infractions //
	if ((numskins > 35)								// Too Many Skins
		|| (numsounds > 2335)						// Too Many Sounds
		|| (accuratenumskinsounds > 3007)			// Too Many Skin Sounds
		
		|| (totalnumsprites > 907)					// Too Many Sprites
		|| ((totalnumsprite2s >= free_spr2)
			&& (totalnumsprite2s >= 128))			// Too Many Sprite2s
		
		|| (numstates >= 4096)						// Too Many States
		|| (numobjects >= 512))						// Too Many Objects
	{
		D_QuitNetGame();
		CL_Reset();
		D_StartTitle();

		M_StartMessage(va("%c%s\x80\nYou've loaded too many add-ons, and now your game isn't vanilla compatible.\n\nYour current values compared to the\nvanilla limits are:\n\n\x82Skins - %d/29\n\x82Sounds - %d/2335\n\x82Skin Sounds - %d/3007\n\x82Sprites - %d/512\n\x82Sprite2s - %d/68\n\x82States - %d/4096\n\x82Objects - %d/512\n\nIf any of these values are above the vanilla limits, you won't be able to\nhost servers until you restart the game and remove some add-ons. \n\n(Press any key to continue)\n", ('\x80' + (menuColor[cv_menucolor.value]|V_CHARCOLORSHIFT)), "Netgame Won't Be Vanilla-Compatible!", accuratenumskins, accuratenumsounds, accuratenumskinsounds, accuratenumsprites, accuratenumsprite2s, numstates, numobjects),NULL,MM_NOTHING);
		return true;
	}

	return false;
}

// MISCELLANIOUS //
//
// UINT32 TSoURDt3rd_CurrentVersion(void)
// Returns the Current Version of TSoURDt3rd, in a Converted, Compressed Number Format
//
UINT32 TSoURDt3rd_CurrentVersion(void)
{
	char versionString[256] = ""; strcpy(versionString, TSOURDT3RDVERSION);
	return STAR_ConvertStringToCompressedNumber(versionString, 0, 0, true);
}

//
// UINT8 TSoURDt3rd_CurrentMajorVersion(void)
// Only Returns the Current Major Version of TSoURDt3rd, in a Converted, Compressed Number Format
//
UINT8 TSoURDt3rd_CurrentMajorVersion(void)
{
	INT32 i = 0;
	char majorVersionString[256] = ""; strcpy(majorVersionString, TSOURDT3RDVERSION);
	char iterateString[256] = "";

	// Iterate Through Our Two Strings //
	while (majorVersionString[i] != '\0')
	{
		iterateString[i] = majorVersionString[i];

		// Found a Dot? Return the Major Version, and We're Done!
		if (majorVersionString[i+1] == '.')
			return atoi(iterateString);

		i++;
	}

	// How Did We Not Find Anything? //
	return 0;
}

//
// UINT8 TSoURDt3rd_CurrentMinorVersion(void)
// Only Returns the Current Minor Version of TSoURDt3rd, in a Converted, Compressed Number Format
//
UINT8 TSoURDt3rd_CurrentMinorVersion(void)
{
	INT32 i = 0, j;
	char minorVersionString[256] = ""; strcpy(minorVersionString, TSOURDT3RDVERSION);
	char iterateString[256] = "";

	// Iterate Through Our Two Strings //
	while (minorVersionString[i] != '\0')
	{
		// We Found One Dot, So Let's Iterate Through it, Stop at Another Dot, and We're Done :)
		if (minorVersionString[i] == '.')
		{
			i++;
			for (j = 0; (minorVersionString[i] != '.' && minorVersionString[i] != '\0'); j++, i++)
				iterateString[j] = minorVersionString[i];
			
			return atoi(iterateString);
		}

		i++;
	}

	// How Did We Still Not Find Anything? //
	return 0;
}

//
// UINT8 TSoURDt3rd_CurrentSubversion(void)
// Only Returns the Current Subversion of TSoURDt3rd, in a Converted, Compressed Number Format
//
UINT8 TSoURDt3rd_CurrentSubversion(void)
{
	INT32 i = 0, j;
	boolean oneDotFound = false;
	char subVersionString[256] = ""; strcpy(subVersionString, TSOURDT3RDVERSION);
	char iterateString[256] = "";

	// Iterate Through Our Two Strings //
	while (subVersionString[i] != '\0')
	{
		// We Found A Dot!
		if (subVersionString[i] == '.')
		{
			// One Dot Found, So Let's Skip It.
			if (!oneDotFound)
			{
				oneDotFound = true;
				i++;
				continue;
			}

			// A Different Dot Has Been Found, so Iterate Through it and We're Done :)
			else
			{
				i++;
				for (j = 0; subVersionString[i] != '\0'; j++, i++)
					iterateString[j] = subVersionString[i];
			
				return atoi(iterateString);
			}
		}

		i++;
	}

	// This Outcome is More Reasonable, Honestly //
	return 0;
}

//
// INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean twoToThreeDigit)
// Converts Strings to Compressed Numbers
//
// Example of a Possible Return:
//	STRING == '2.8', twoToThreeDigit = true		=	Returned Number = 280
//	STRING == '2.7.1', twoToThreeDigit = false	=	Returned Number = 271
//
INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean twoToThreeDigit)
{
	// Make Variables //
	INT32 i = startIFrom, j = startJFrom;
	char convertedVersionString[256] = "";

	// Initialize the Main String, and Iterate Through Our Two Strings //
	while (STRING[j] != '\0')
	{
		if (STRING[j] == '.' || STRING[j] == '"')
			j++;
		
		convertedVersionString[i] = STRING[j];
		i++, j++;
	}

	// Add an Extra Digit if Our Version Has Less Than 2 Digits, Return Our Compressed Number, and We're Done! //
	if (twoToThreeDigit && strlen(convertedVersionString) <= 2)
		convertedVersionString[2] = '0';
	return atoi(convertedVersionString);
}

//
// char *STAR_ConvertNumberToString(INT32 NUMBER, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionString)
// Converts Strings to Compressed Numbers
//
// Example of a Possible Return:
//	NUMBER == 280, turnIntoVersionString = true		=	Returned String = '2.8.0'
//	NUMBER == 271, turnIntoVersionString = false	=	Returned String = '271'
//
char finalNumberString[256] = "";

char *STAR_ConvertNumberToString(INT32 NUMBER, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionString)
{
	// Make Variables //
	INT32 i = startIFrom, j = startJFrom;
	char convertedNumberString[256] = ""; sprintf(convertedNumberString, "%d", NUMBER);

	// Initialize the Main String, and Iterate Through Our Two Strings //
	if (turnIntoVersionString)
	{
		while (convertedNumberString[j] != '\0')
		{
			finalNumberString[i] = convertedNumberString[j];
			i++;
			j++;

			if (convertedNumberString[j] != '\0') // Prevents an Extra Dot From Being Added at the End
			{
				finalNumberString[i] = '.';
				i++;
			}
		}
	}
	else
		strcpy(finalNumberString, convertedNumberString);

	// Return Our Converted String and We're Done! //
	return finalNumberString;
}

//
// INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean twoToThreeDigit)
// Converts Numbers to Strings, and Then Converts Them Back to Numbers
//
INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean twoToThreeDigit)
{
	// Return The Number, and We're Done :) //
	char numberString[256] = ""; strcpy(numberString, STAR_ConvertNumberToString(NUMBER, startI1From, startJ1From, turnIntoVersionString));
	return STAR_ConvertStringToCompressedNumber(numberString, startI2From, startJ2From, twoToThreeDigit);
}

//
// INT32 STAR_CombineNumbers(INT32 ARGS, INT32 FIRSTNUM, ...)
// Combines Numbers Together, Like You Would Do a String, But Doesn't Perform Math on the Numbers
//
INT32 STAR_CombineNumbers(INT32 ARGS, INT32 FIRSTNUM, ...)
{
	// Make Variables //
	va_list argptr;
	INT32 i;
	
	char numberString[256] = ""; sprintf(numberString, "%d", FIRSTNUM);
	
	// Initialize and Iterate Through the Variable List of Arguments, Combine our Number Strings Together, and Then End it //
	va_start(argptr, FIRSTNUM);
	for (i = 0; i < ARGS-1; i++)
		strlcat(numberString, va("%d", va_arg(argptr, int)), sizeof(numberString));
	va_end(argptr);

	// Convert the String Made Earlier Into a Number, Return The Number, and We're Done :) //
	return STAR_ConvertStringToCompressedNumber(numberString, 0, 0, false);
}

#ifdef HAVE_SDL
//
// void STAR_RenameWindow(const char *title)
// Renames SRB2's Window Title
//
void STAR_RenameWindow(const char *title)
{
	if (window == NULL)
		return;
	SDL_SetWindowTitle(window, title);
}

//
// const char *STAR_SetWindowTitle(void)
// Sets SRB2's Window Title
//
char randomTitleTable[5][256];

const char *STAR_SetWindowTitle(void)
{
	// Make Variables //
	const char *windowtitle = "";
	const char *dynamictitle = "";

	// Configure the Window Title //
	// Default Title
	if (!cv_windowtitletype.value)
		windowtitle = ("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING);

	// Others
	else
	{
		// Dynamic Titles
		if (cv_windowtitletype.value == 1)
		{
			// Main Window Titles
			// Map Specific
			if (Playing())
			{
				// Modified-Game Titles
				if (autoloaded || savemoddata || modifiedgame)
				{
					// Player is on The Titlemap
					if (gamemap == titlemap)
						dynamictitle = "What is Wrong With You. -";
		
					// Super Character Window Title
					else if (players[consoleplayer].powers[pw_super])
					{
						if (cv_memesonwindowtitle.value)
							dynamictitle = "Playing as Goku in";
						else
							dynamictitle = "Going Super in";
					}

					// Player is Learning How to Play SRB2
					else if (tutorialmode)
						dynamictitle = "Learning How to Play";

					// Player is on a Custom Map
					else
						dynamictitle = va("%s Through MAP%d -", (cv_memesonwindowtitle.value ? "D_RUNNIN" : "Running"), gamemap);
				}

				// Vanilla/Unmodified-Game Titles
				else
				{
					switch (gamemap)
					{
						// GFZ
						case 1:
						case 2:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Where ARE the Green Flowers in" : "The Green Beginning -");
							break;
						}
						
						// THZ
						case 4:
						case 5:
						{
							if (cv_memesonwindowtitle.value)
							{
								if (randomTitleTable[1] == NULL || randomTitleTable[1][0] == '\0')
								{
									switch (M_RandomRange(0, 1))
									{
										case 1:
											strcpy(randomTitleTable[1], "Industrial Society and its Future -");
											break;
										default:
											strcpy(randomTitleTable[1], "Climate Change -");
											break;
									}
								}
				
								dynamictitle = randomTitleTable[1];
							}
							else
								dynamictitle = "So Much Advanced Technology in";
							break;
						}
			
						// DSZ
						case 7:
						case 8:
						{
							if (fastncmp(skins[players[consoleplayer].skin].name, "sonic", 5))
								dynamictitle = "Ugh, I Hate Water in";
							else
								dynamictitle = "Swimming Around in";
							break;
						}

						// CEZ
						case 10:
						case 11:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "How Did Eggman Manage to Build This Castle so Fast in" : "Such a Large Castle in");
							break;
						}

						// ACZ
						case 13:
						case 14:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Playing Through Grand Canyon Zone in" : "Why is There So Much TNT in");
							break;
						}

						// Fang
						case 15:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "There is a Jerboa With a Popgun in" : "We're on a Train in");
							break;
						}

						// RVZ
						case 16:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Where is the Blue Volcano in" : "Too Much Lava in");
							break;
						}

						// ERZ
						case 22:
						case 23:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Robotnik has Too Many Rocks in" : "Be Careful Not to Fall Into Space in");
							break;
						}

						// Metal Sonic
						case 25:
						case 26:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "STRRANGE, ISN'T IT? -" : "Beating a Doppelgänger Hedgehog Robot in");
							break;
						}

						// Bosses
						case 3:
						case 6:
						case 9:
						case 12:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "He is the EggMan, With the Master Plan -" : "Fighting a Giant Talking Egg in");
							break;
						}

						// Black Eggman (Yes, Technically Brak's Name is Black Eggman)
						case 27:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "No Way Guys, the Cyberdemon is in" : "Fighting the Final Boss in");
							break;
						}

						// FHZ
						case 30:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Use the 'Destroy All Enemies' Cheat in" : "Be Careful Not to Slip in");
							break;
						}

						// PTZ
						case 31:
						{
							if (cv_memesonwindowtitle.value)
							{
								if (randomTitleTable[2] == NULL || randomTitleTable[2][0] == '\0')
								{
									switch (M_RandomRange(0, 1))
									{
										case 1:
											strcpy(randomTitleTable[2], "The Princess is in Another Tower in");
											break;
										default:
											strcpy(randomTitleTable[2], "Super Mario in Real Life -");
											break;
									}
								}

								dynamictitle = randomTitleTable[2];
							}
							else
								dynamictitle = "We're in Another Dimension in";
							break;
						}

						// FFZ
						case 32:
						{
							dynamictitle = "There's an In-Construction Castle in";
							break;
						}
			
						// TLZ
						case 33:
						{
							dynamictitle = "So Much Prehistoric Technology in";
							break;
						}

						// HHZ
						case 40:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "No Way Guys, Cacodemons Are Also in" : "The Final Boss in");
							break;
						}

						// AGZ
						case 41:
						{
							if (cv_memesonwindowtitle.value)
							{
								if (randomTitleTable[3] == NULL || randomTitleTable[3][0] == '\0')
								{
									switch (M_RandomRange(0, 1))
									{
										case 1:
											strcpy(randomTitleTable[3], "According to all known laws of aviation, there is no way a bee should be able to fly. Its wings are too small to get its fat little body off the ground. The bee, of course, flies anyway, because bees don't care what humans think is impossible. -");
											break;
										default:
											strcpy(randomTitleTable[3], "Welcome to the Bee Zone. -");
											break;
									}
								}

								dynamictitle = randomTitleTable[3];
							}
							else
								dynamictitle = "There are So Many Bees in";
							break;
						}

						// ATZ
						case 42:
							dynamictitle = "The Zone Everyone Hates. -";
							break;

						// Special Stages
						case 50:
						case 51:
						case 52:
						case 54:
						case 55:
						case 56:
			
						case 60:
						case 61:
						case 62:
						case 64:
						case 65:
						case 66:
						{
							dynamictitle = "Trying to Get All Those Chaos Emeralds in";
							break;
						}

						// Special Stage 4
						case 53:
						case 63:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Trying to Get That DAMN FOURTH Chaos Emerald in" : "Trying to Get All Those Chaos Emeralds in");
							break;
						}

						// BHZ
						case 57:
							dynamictitle = "The True Final Boss in";
							break;
		
						// The Extra Special Stages
						case 70:
						case 71:
						case 72:
						case 73:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Playing the Extra Special Stages For no Reason in" : "Playing the Extra Special Stages in");
							break;
						}

						// CTF Maps
						case 280:
						case 281:
						case 282:
						case 283:
						case 284:
						case 285:
						case 286:
						case 287:
						case 288:
						{
							dynamictitle = (cv_memesonwindowtitle.value ? "Playing Zandronum in" : "Capturing Flags in");
							break;
						}

						// Match Maps
						case 532:
						case 533:
						case 534:
						case 535:
						case 536:
						case 537:
						case 538:
						case 539:
						case 540:
						case 541:
						case 542:
						case 543:
						{
							if (cv_memesonwindowtitle.value)
							{
								if (randomTitleTable[4] == NULL || randomTitleTable[4][0] == '\0')
								{
									switch (M_RandomRange(0, 1))
									{
										case 1:
											strcpy(randomTitleTable[4], "Playing Zandronum in");
											break;
										default:
											strcpy(randomTitleTable[4], "Ringslinger Will be Removed in the Next 5 Minutes -");
											break;
									}
								}

								dynamictitle = randomTitleTable[4];
							}
							else
								dynamictitle = "Capturing Flags in";
							break;
						}

						default:
						{
							// Player is on The Titlemap
							if (gamemap == titlemap)
							{
								dynamictitle = "What is Wrong With You -";
								break;
							}

							// Super Character Window Title
							else if (players[consoleplayer].powers[pw_super])
							{
								dynamictitle = (cv_memesonwindowtitle.value ? "Playing as Goku in" : "Got All Those Chaos Emeralds in");
								break;
							}

							// Player is Learning How to Play SRB2
							else if (tutorialmode)
							{
								dynamictitle = "Learning How to Play";
								break;
							}

							// Player is on a Custom Map
							else
							{
								dynamictitle = va("%s Through MAP%d -", (cv_memesonwindowtitle.value ? "D_RUNNIN" : "Running"), gamemap);
								break;
							}
						}
					}
				}
			}

			// Gamestate Specific
			switch (gamestate)
			{
				case GS_INTRO:
					dynamictitle = "Introduction -";
					break;
	
				case GS_CUTSCENE:
					dynamictitle = "Watching a Cutscene in";
					break;

				case GS_CONTINUING:
					dynamictitle = "Continue? -";
					break;

				case GS_INTERMISSION:
				{
					if (cv_memesonwindowtitle.value)
						dynamictitle = "End of Chapter! -";
					else
						dynamictitle = "You Got Pass the Act! -";
					break;
				}

				case GS_CREDITS:
				case GS_ENDING:
				case GS_EVALUATION:
				case GS_GAMEEND:
				{
					if (cv_memesonwindowtitle.value)
						dynamictitle = "Did You Get All Those Chaos Emeralds? -";
					else
						dynamictitle = "The End. -";
					break;
				}

				default:
				{
					if (gamestate == GS_NULL || (gamestate == GS_TITLESCREEN || titlemapinaction))
					{
						STAR_RenameWindow("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING);
						return ("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING);
					}
		
					break;
				}
			}

			windowtitle = va("%s SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING, dynamictitle);
		}

		// Semi-Custom Titles
		else if (cv_windowtitletype.value == 2)
			windowtitle = va("%s SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING, cv_customwindowtitle.string);

		// Fully Custom Titles
		else
			windowtitle = cv_customwindowtitle.string;
	}

	// Set the Window Title, Return it, and We're Done :) //
	STAR_RenameWindow(windowtitle);
	return windowtitle;
}
#endif