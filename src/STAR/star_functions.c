// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_functions.c
/// \brief Contains all the Info Portraying to TSoURDt3rd's Variables and STAR Functions

#ifdef HAVE_CURL
#include <curl/curl.h>		// internet variables
#include "../i_threads.h"	// internet variables 2
#endif

#include <time.h>
#include <signal.h>

#include "star_vars.h" 		// star variables

#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../m_misc.h" 		// extra file functions
#include "../m_menu.h" 		// egg and command variables
#include "../d_main.h" 		// event variables
#include "../deh_soc.h"		// savefile variables
#include "../keys.h"		// key variables

#include "../v_video.h"		// video variables
#include "../i_video.h"		// video variables 2

#include "../filesrch.h"	// file variables
#include "../r_skins.h"		// skin variables
#include "../sounds.h"		// sound variables
#include "../deh_tables.h"	// sprite/table variables
#include "../m_random.h"	// m_random functions
#include "../z_zone.h"		// memory variables

#ifdef HAVE_CURL
#include "../fastcmp.h"		// string variables
#endif

#ifdef HAVE_SDL
#include "SDL.h"			// sdl variables
#include "../sdl/sdlmain.h"	// sdl variables 2
#endif

//////////////////////////////////////
//		ABSOLUTELY HILARIOUS	 	//
//			STAR FUNCTIONS		 	//
//				YAY				 	//
//////////////////////////////////////

// STRUCTS //
TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[0];
TSoURDt3rd_t TSoURDt3rdPlayers[MAXPLAYERS];

star_gamekey_t STAR_GameKeyDown[1][NUM_GAMECONTROLS];

static TSoURDt3rdDefaultMusicTracks_t defaultMusicTracks[] =
{
	[1] = {"GFZ1", 		  "gfz1"}, // GFZ1
		  {"D_RUNNIN",  "_runin"}  // DooM Wad Anthem
};

// VARIABLES //
#ifdef HAVE_CURL
char *hms_tsourdt3rd_api;

#ifdef HAVE_THREADS
static I_mutex hms_tsourdt3rd_api_mutex;
#endif
#endif

// COMMANDS //
consvar_t cv_loadingscreen = CVAR_INIT ("loadingscreen", "Off", CV_SAVE|CV_CALL, CV_OnOff, STAR_LoadingScreen_OnChange);

static CV_PossibleValue_t loadingscreenbackground_t[] = {
	{0, "None"},

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

static CV_PossibleValue_t tsourdt3rdupdatemessage_t[] = {{0, "Off"}, {1, "Screen"}, {2, "Console"}, {0, NULL}};
consvar_t cv_updatenotice = CVAR_INIT ("tsourdt3rdupdatenotice", "Screen", CV_SAVE|CV_CALL, tsourdt3rdupdatemessage_t, STAR_UpdateNotice_OnChange);

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

		STAR_StoreDefaultMenuStrings();
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

//
// boolean TSoURDt3rd_InAprilFoolsMode(void)
// Checks If TSoURDt3rd is in April Fools Mode, and Returns True if so
//
boolean TSoURDt3rd_InAprilFoolsMode(void)
{
	return (aprilfoolsmode && cv_ultimatemode.value);
}

// GAME //
//
// void TSoURDt3rd_InitializeStructures(void)
// Initializes TSoURDt3rd's Structures
//
void TSoURDt3rd_InitializeStructures(void)
{
	// Set the Structures and We're Done :) //
	// Game Stuff
	TSoURDt3rd->usingTSoURDt3rd							= true;
	TSoURDt3rd->checkedVersion							= false;

	TSoURDt3rd->reachedSockSendErrorLimit 				= 0;

	TSoURDt3rd->loadingScreens.loadCount 				= 0;
	TSoURDt3rd->loadingScreens.loadPercentage 			= 0;
	TSoURDt3rd->loadingScreens.bspCount 				= 0;

	TSoURDt3rd->loadingScreens.screenToUse 				= 0;

	TSoURDt3rd->loadingScreens.softwareLoadComplete 	= false;

	// Music Stuff
	TSoURDt3rd->defaultMusicTracks						= defaultMusicTracks;

	// Server Stuff
	TSoURDt3rd->masterServerAddressChanged				= false;

	TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd		= true;
	
	TSoURDt3rd->serverPlayers.majorVersion				= TSoURDt3rd_CurrentMajorVersion();
	TSoURDt3rd->serverPlayers.minorVersion				= TSoURDt3rd_CurrentMinorVersion();
	TSoURDt3rd->serverPlayers.subVersion				= TSoURDt3rd_CurrentSubversion();

	TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion	= TSoURDt3rd_CurrentVersion();

	// Jukebox Stuff
	TSoURDt3rd->jukebox.Unlocked 						= false;
	TSoURDt3rd->jukebox.lastTrackPlayed					= NULL;

	M_ResetJukebox();
}

//
// void TSoURDt3rd_ReinitializeServerStructures(void)
// Reinitializes TSoURDt3rd's Server Structures After Servers Wipe Them
//
void TSoURDt3rd_ReinitializeServerStructures(void)
{
	// Reinitialize the Structures and We're Done :) //
	TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd		= true;
	
	TSoURDt3rd->serverPlayers.majorVersion				= TSoURDt3rd_CurrentMajorVersion();
	TSoURDt3rd->serverPlayers.minorVersion				= TSoURDt3rd_CurrentMinorVersion();
	TSoURDt3rd->serverPlayers.subVersion				= TSoURDt3rd_CurrentSubversion();

	TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion	= TSoURDt3rd_CurrentVersion();
}

//
// void STAR_LoadingScreen(void)
// Displays a Loading Screen
//
void STAR_LoadingScreen(void)
{
	// Make Variables //
	char s[16];
	INT32 x, y;

	static const char *gstartuplumpnumtype[] = {
		[2] = "SRB2BACK",	// SRB2 Titlecard Background
		"DFTL",
		
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

	// Set Parameters and Run Some Functions //
	I_OsPolling();
	//CON_Drawer(); // console shouldn't appear while in a loading screen, honestly

	sprintf(s, "%d%%", (++TSoURDt3rd->loadingScreens.loadPercentage)<<1);
	x = BASEVIDWIDTH/2;
	y = BASEVIDHEIGHT/2;
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31); // Black background to match fade in effect

	// Set Our Loading Screen //
	if (cv_loadingscreenimage.value)
	{
		// Dynamic
		if (cv_loadingscreenimage.value == 1)
		{
			// In-Game
			if (Playing())
			{
				// Modified-Game Images
				if (autoloaded || savemoddata || modifiedgame)
					TSoURDt3rd->loadingScreens.screenToUse = 3;

				// Vanilla/Unmodified-Game Images
				else
				{
					switch (gamemap)
					{
						// GFZ
						case 1:
						case 2:
						case 3: TSoURDt3rd->loadingScreens.screenToUse = 4; break;
						
						// THZ
						case 4:
						case 5:
						case 6: TSoURDt3rd->loadingScreens.screenToUse = 5; break;
			
						// DSZ
						case 7:
						case 8:
						case 9: TSoURDt3rd->loadingScreens.screenToUse = 6; break;

						// CEZ
						case 10:
						case 11:
						case 12: TSoURDt3rd->loadingScreens.screenToUse = 7; break;

						// ACZ
						case 13:
						case 14:
						case 15: TSoURDt3rd->loadingScreens.screenToUse = 8; break;

						// RVZ
						case 16: TSoURDt3rd->loadingScreens.screenToUse = 9; break;

						// ERZ
						case 22:
						case 23: TSoURDt3rd->loadingScreens.screenToUse = 10; break;

						// BCZ
						case 25:
						case 26:
						case 27: TSoURDt3rd->loadingScreens.screenToUse = 11; break;

						// FHZ
						case 30: TSoURDt3rd->loadingScreens.screenToUse = 12; break;

						// PTZ
						case 31: TSoURDt3rd->loadingScreens.screenToUse = 13; break;

						// FFZ
						case 32: TSoURDt3rd->loadingScreens.screenToUse = 14; break;
			
						// TLZ
						case 33: TSoURDt3rd->loadingScreens.screenToUse = 15; break;

						// HHZ
						case 40: TSoURDt3rd->loadingScreens.screenToUse = 16; break;

						// AGZ
						case 41: TSoURDt3rd->loadingScreens.screenToUse = 17; break;

						// ATZ
						case 42: TSoURDt3rd->loadingScreens.screenToUse = 18; break;

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
						case 73: TSoURDt3rd->loadingScreens.screenToUse = 19; break;

						// BHZ
						case 57: TSoURDt3rd->loadingScreens.screenToUse = 20; break;

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

						default: TSoURDt3rd->loadingScreens.screenToUse = 3; break;
					}
				}
			}

			// Not In-Game
			else
				TSoURDt3rd->loadingScreens.screenToUse = 2;
		}

		// Random
		else if (cv_loadingscreenimage.value == 21 && !TSoURDt3rd->loadingScreens.screenToUse)
			TSoURDt3rd->loadingScreens.screenToUse = M_RandomRange(2, 20);

		// Finally, Apply the Image, and We're Good Here :) //
		V_DrawPatchFill(W_CachePatchName(
			(gstartuplumpnumtype[(cv_loadingscreenimage.value == 1 || cv_loadingscreenimage.value == 21) ? TSoURDt3rd->loadingScreens.screenToUse : cv_loadingscreenimage.value]),
			(PU_CACHE)));
	}

	// Run Some Other Necessary Functions Here, and We're Done :) //
	M_DrawTextBox(x-58, y-8, 13, 1);
	V_DrawString(x-50, y, menuColor[cv_menucolor.value], "Loading...");
	V_DrawRightAlignedString(x+50, y, menuColor[cv_menucolor.value], s);

	I_UpdateNoVsync();
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
				// General Game Titles
				if ((autoloaded || savemoddata || modifiedgame)
					|| (gamemap == titlemap)
					|| (players[consoleplayer].powers[pw_super])
					|| (tutorialmode))

					goto generalgametitles;

				// Level Titles
				switch (gamemap)
				{
					// GFZ
					case 1: case 2: dynamictitle = (cv_memesonwindowtitle.value ? "Where ARE the Green Flowers in" : "The Green Beginning -"); break;

					// THZ
					case 4:
					case 5:
					{
						if (cv_memesonwindowtitle.value)
						{
							if (randomTitleTable[1][0] == '\0')
							{
								switch (M_RandomRange(0, 1))
								{
									case 1: strcpy(randomTitleTable[1], "Industrial Society and its Future -"); break;
									default: strcpy(randomTitleTable[1], "Climate Change -"); break;
								}
							}

							dynamictitle = randomTitleTable[1];
						}
						else
							dynamictitle = "So Much Advanced Technology in";

						break;
					}

					// DSZ
					case 7: case 8: dynamictitle = ((fastncmp(skins[players[consoleplayer].skin].name, "sonic", 5)) ? ("Ugh, I Hate Water in") : ("Swimming Around in")); break;

					// CEZ
					case 10: case 11: dynamictitle = (cv_memesonwindowtitle.value ? "How Did Eggman Manage to Build This Castle so Fast in" : "Such a Large Castle in"); break;

					// ACZ
					case 13: case 14: dynamictitle = (cv_memesonwindowtitle.value ? "Playing Through Grand Canyon Zone in" : "Why is There So Much TNT in"); break;

					// Fang
					case 15: dynamictitle = (cv_memesonwindowtitle.value ? "There is a Jerboa With a Popgun in" : "We're on a Train in"); break;

					// RVZ
					case 16: dynamictitle = (cv_memesonwindowtitle.value ? "Where is the Blue Volcano in" : "Too Much Lava in"); break;

					// ERZ
					case 22: case 23: dynamictitle = (cv_memesonwindowtitle.value ? "Robotnik has Too Many Rocks in" : "Be Careful Not to Fall Into Space in"); break;

					// Metal Sonic
					case 25: case 26: dynamictitle = (cv_memesonwindowtitle.value ? "STRRANGE, ISN'T IT? -" : "Beating a Doppelgänger Hedgehog Robot in"); break;

					// Bosses
					case 3: case 6: case 9: case 12: dynamictitle = (cv_memesonwindowtitle.value ? "He is the Eggman, With the Master Plan -" : "Fighting a Giant Talking Egg in"); break;

					// Black Eggman (Yes, Technically Brak's Name is Black Eggman)
					case 27: dynamictitle = (cv_memesonwindowtitle.value ? "No Way Guys, the Cyberdemon is in" : "Fighting the Final Boss in"); break;

					// FHZ
					case 30: dynamictitle = (cv_memesonwindowtitle.value ? "Use the 'Destroy All Enemies' Cheat in" : "Be Careful Not to Slip in"); break;

					// PTZ
					case 31:
					{
						if (cv_memesonwindowtitle.value)
						{
							if (randomTitleTable[2][0] == '\0')
							{
								switch (M_RandomRange(0, 1))
								{
									case 1: strcpy(randomTitleTable[2], "The Princess is in Another Tower in"); break;
									default: strcpy(randomTitleTable[2], "Super Mario in Real Life -"); break;
								}
							}

							dynamictitle = randomTitleTable[2];
						}
						else
							dynamictitle = "We're in Another Dimension in";

						break;
					}

					// FFZ
					case 32: dynamictitle = "There's an In-Construction Castle in"; break;

					// TLZ
					case 33: dynamictitle = "So Much Prehistoric Technology in"; break;

					// HHZ
					case 40: dynamictitle = (cv_memesonwindowtitle.value ? "No Way Guys, Cacodemons Are Also in" : "The Final Boss in"); break;

					// AGZ
					case 41:
					{
						if (cv_memesonwindowtitle.value)
						{
							if (randomTitleTable[3][0] == '\0')
							{
								switch (M_RandomRange(0, 1))
								{
									case 1: strcpy(randomTitleTable[3], "According to all known laws of aviation, there is no way a bee should be able to fly. Its wings are too small to get its fat little body off the ground. The bee, of course, flies anyway, because bees don't care what humans think is impossible. -"); break;
									default: strcpy(randomTitleTable[3], "Welcome to the Bee Zone. -"); break;
								}
							}

							dynamictitle = randomTitleTable[3];
						}
						else
							dynamictitle = "There are So Many Bees in";

						break;
					}

					// ATZ
					case 42: dynamictitle = "The Zone Everyone Hates. -"; break;

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
					case 66: dynamictitle = "Trying to Get All Those Chaos Emeralds in"; break;

					// Special Stage 4
					case 53: case 63: dynamictitle = (cv_memesonwindowtitle.value ? "Trying to Get That DAMN FOURTH Chaos Emerald in" : "Trying to Get All Those Chaos Emeralds in"); break;

					// BHZ
					case 57: dynamictitle = "The True Final Boss in"; break;

					// The Extra Special Stages
					case 70:
					case 71:
					case 72:
					case 73: dynamictitle = (cv_memesonwindowtitle.value ? "Playing the Extra Special Stages For no Reason in" : "Playing the Extra Special Stages in"); break;

					// CTF Maps
					case 280:
					case 281:
					case 282:
					case 283:
					case 284:
					case 285:
					case 286:
					case 287:
					case 288: dynamictitle = (cv_memesonwindowtitle.value ? "Playing Zandronum in" : "Capturing Flags in"); break;

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
							if (randomTitleTable[4][0] == '\0')
							{
								switch (M_RandomRange(0, 1))
								{
									case 1: strcpy(randomTitleTable[4], "Playing Zandronum in"); break;
									default: strcpy(randomTitleTable[4], "Ringslinger Will be Removed in the Next 5 Minutes -"); break;
								}
							}

							dynamictitle = randomTitleTable[4];
						}
						else
							dynamictitle = "Capturing Flags in";

						break;
					}

					// Maps That Aren't Specified Here
					default: goto generalgametitles; break;
				}
			}

			// Gamestate Specific
			switch (gamestate)
			{
				case GS_INTRO: dynamictitle = "Introduction -"; break;
				case GS_CUTSCENE: dynamictitle = "Watching a Cutscene in"; break;
				case GS_CONTINUING: dynamictitle = "Continue? -"; break;
				case GS_INTERMISSION: dynamictitle = (cv_memesonwindowtitle.value ? "End of Chapter! -" : va("%s Got Through %s! -", skins[players[consoleplayer].skin].realname, (!mapheaderinfo[gamemap-1]->actnum ? "the Act" : va("Act %d", mapheaderinfo[gamemap-1]->actnum)))); break;

				case GS_CREDITS:
				case GS_ENDING:
				case GS_EVALUATION:
				case GS_GAMEEND: dynamictitle = (cv_memesonwindowtitle.value ? "Did You Get All Those Chaos Emeralds? -" : "The End. -"); break;

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

generalgametitles:
	// Did we Get Sent Here by a Goto? Let's Check Some Things First Then. //
	// Player is on The Titlemap
	if (gamemap == titlemap)
		dynamictitle = "What is Wrong With You -";

	// Super Character Window Title
	else if (players[consoleplayer].powers[pw_super])
		dynamictitle = (cv_memesonwindowtitle.value ? "Playing as Goku in" : "Got All Those Chaos Emeralds in");

	// Player is Learning How to Play SRB2
	else if (tutorialmode)
		dynamictitle = "Learning How to Play";

	// Player is on a Custom Map
	else
		dynamictitle = va("%s as %s Through %s %s -", (cv_memesonwindowtitle.value ? "D_RUNNIN" : "Running"), skins[players[consoleplayer].skin].realname, mapheaderinfo[gamemap-1]->lvlttl, ((mapheaderinfo[gamemap-1]->levelflags & LF_NOZONE) ? "" : "Zone"));

	// Now That We've Set Our Things, Let's Return our Window Title, and We're Done :)
	windowtitle = va("%s SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING, dynamictitle);

	STAR_RenameWindow(windowtitle);
	return windowtitle;
}
#endif

//
// const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped)
// Generates a Funny Crash Message Everytime TSoURDt3rd Crashes
//
const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped)
{
	// Make Variables //
	const char *jokemsg;
	char underscoremsg[256] = "";

	size_t i;
	size_t tabend;
	size_t current, total;

	// Creates a Random Seed, Generally Needed For Situations Where SRB2 Wasn't Able to Fully Start Up //
	if (!M_RandomSeedFromOS())
		M_RandomSeed((UINT32)time(NULL));
	P_SetRandSeed(M_RandomizedSeed());

	// Come up With a Random Funny Joke //
	switch (M_RandomRange(0, 10))
	{
		// Static //
		default: jokemsg = "Uh..."; break;
		case 1: jokemsg = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAH!"; break;
		case 2: jokemsg = "This will make the TSoURDt3rd Devs sad..."; break;
		case 3: jokemsg = "This will make STJr sad..."; break;
		case 4: jokemsg = "Sonic, what did you do this time?"; break;

		// References //
		// Sonic Adventure
		case 5:
		{
			switch (M_RandomRange(0, 1))
			{
				case 1: jokemsg = "Watch out, you're gonna crash! AAAAH!"; break;
				default: jokemsg = "OH NO!"; break;
			}
			break;
		}

		// Grand Theft Auto
		case 6:
		{
			switch (M_RandomRange(0, 1))
			{
				case 1: jokemsg = "All you had to do, was not crash the game, Sonic!"; break;
				default: jokemsg = "All we had to do, was follow the dang train, CJ!"; break;
			}
			break;
		}

		// Baldi's Basics
		case 7: jokemsg = "OOPS! You messed up!"; break;

		// Sonic Rush
		case 8:
		{
			switch (M_RandomRange(0, 1))
			{
				case 1: jokemsg = "Should we try this again?"; break;
				default: jokemsg = "Never get on my bad side!"; break;
			}
			break;
		}

		// Parappa the Rapper
		case 9:
		{
			switch (M_RandomRange(0, 3))
			{
				case 1:
				{
					switch (M_RandomRange(0, 6))
					{
						case 1: jokemsg = "I gotta, I gotta... No! I can't believe..."; break;
						case 2: jokemsg = "Remember, you gotta believe!"; break;

						case 3: jokemsg = "TSoURDt3rd cut a few corners but we didn't think it would be this bad..."; break;
						case 4: jokemsg = "No cutting corners!"; break;

						case 5: jokemsg = "Oops!"; break;
						case 6: jokemsg = "Uh oh!"; break;

						default: jokemsg = "\t\t\tTRY AGAIN!!\n\n\t\t(X) YES\t\t      (O) NO\t\t\t\t"; break;
					}
					break;
				}

				case 2:
				{
					switch (M_RandomRange(0, 12))
					{
						case 1: jokemsg = "Coding, TSoURDt3rd, it's all in the mind."; break;

						case 2: jokemsg = "Alright, we're here, just playing TSoURDt3rd, I want you to show me if you can get far."; break;
						case 3: jokemsg = "Whoa ho ho ho, stop TSoURDt3rd!\nWe got an emergency, can't you see?"; break;

						case 4: jokemsg = "TSoURDt3rd is all you need!"; break;
						case 5: jokemsg = "All you ever need is to be nice and friendly!"; break;

						case 6: jokemsg = "Every single day, stress comes in every way."; break;
						case 7: jokemsg = "Seafood cake comes just like the riddle."; break;
						case 8: jokemsg = "TSoURDt3rd comes just like the riddle."; break;

						case 9: jokemsg = "Nuh uh, nuh uh, no way!"; break;

						case 10: jokemsg = "Whatcha gonna do, when they come?"; break;
						case 11: jokemsg = "I gotta redeem!\nI gotta relieve!\nI gotta receive!\nI GOTTA BELIEVE!"; break;
						case 12: jokemsg = "Breakin' out was the name of the game for me, you, you, You, and YOU!"; break;
						default: jokemsg = "Somebody say ho! Say ho ho! Say ho ho ho! Now scream!\nEverybody say ho! Say ho ho! Say ho ho ho! Now scream!"; break;
					}
					break;
				}

				case 3:
				{
					switch (M_RandomRange(0, 6))
					{
						case 1: jokemsg = "Again."; break;
						case 2: jokemsg = "You gotta do it again!"; break;
						case 3: jokemsg = "Uh uh, uh uh, come on, let's do this all over again."; break;
						case 4: jokemsg = "Peep! Peep! (TRY AGAIN!!)"; break;
						case 5: jokemsg = "Maybe next time..."; break;
						case 6: jokemsg = "O, oh, I didn't think you'd do THIS bad!"; break;
						default: jokemsg = "U rappin' AWFUL!"; break;
					}
					break;
				}
		
				default:
				{
					switch (M_RandomRange(0, 3))
					{
						case 1: jokemsg = "TSoURDt3rd Devs are codin' AWFUL!"; break;
						case 2: jokemsg = "I'm crashin' AWFUL!"; break;
						case 3: jokemsg = "U playin' COOL!"; break;
						default: jokemsg = "I shouldn't cut corners.\nYou shouldn't cut corners.\nTSoURDt3rd Devs and STJr shouldn't cut corners."; break;
					}
					break;
				}
			}
			break;
		}

		// SRB2
		case 10:
		{
			switch (M_RandomRange(0, 2))
			{
				// STJr Members
				case 1: jokemsg = (crashnum == SIGSEGV ? (coredumped ? "SIGSEGV - seventh sentinel (core dumped)" : "SIGSEGV - seventh sentinel") : "...Huh. This is awkward..."); break;

				// Zone Builder
				case 2:
				{
					switch (M_RandomRange(0, 1))
					{
						case 1: jokemsg = "I'll miss you, you know."; break;
						default: jokemsg = "You were always my favorite user."; break;
					}
					break;
				}

				// Bugs
				default: jokemsg = "The Leader of the Chaotix would be very disappointed in you."; break;
			}
			break;
		}
	}

	// Underscore our Funny Crash Message, Return it, and We're Done :) //
	for (i = current = total = 0; jokemsg[current] != '\0'; i++, current++)
	{
		// Run Special Operations //
		// Tabs
		if (jokemsg[current] == '\t')
			for (tabend = i+8; i < tabend; i++) underscoremsg[i] = '_';
		// New Lines
		else if (jokemsg[current] == '\n')
			i = 0;

		// Uppercase Letters
		else if ((isupper(jokemsg[current]))
			&& ((!isupper(jokemsg[current+1]))
			&& (jokemsg[current+1] != ' ')
			&& (jokemsg[current+1] != '\t')
			&& (jokemsg[current+1] != '\n')))
		{
			underscoremsg[i] = '_';
			i++;
		}

		// Add an Underscore and Continue //
		if (total <= i)
		{
			underscoremsg[i] = '_';
			total = i;
		}
	}

	return va("%s\n%s", jokemsg, underscoremsg);
}

//
// boolean STAR_Responder(UINT8 player, UINT8 input, boolean preventhold)
// Checks if the Given Key is Being Pressed
// However, it Implements Checks For Good General Input Consistancy
//
boolean STAR_Responder(UINT8 player, UINT8 input, boolean preventhold)
{
	// Make the Variables //
	STAR_GameKeyDown[player][input].pressed = (!player ?
			(gamekeydown[gamecontrol[input][player]] || gamekeydown[gamecontrol[input][player]]) :
			(gamekeydown[gamecontrolbis[input][player]] || gamekeydown[gamecontrolbis[input][player]]));

	// Control Key Stuff //
	// Reset Everything if Not Tapping
	if (!STAR_GameKeyDown[player][input].pressed)
	{
		STAR_GameKeyDown[player][input].keyDown		= 0;
		STAR_GameKeyDown[player][input].tapReady	= false;
	}

	// Set Things While Tapping
	else
	{
		if (STAR_GameKeyDown[player][input].tapReady)
		{
			STAR_GameKeyDown[player][input].pressed	= ((STAR_GameKeyDown[player][input].keyDown > TICRATE/2 && !preventhold) ? true : false);
			STAR_GameKeyDown[player][input].keyDown++;
		}
		else
			STAR_GameKeyDown[player][input].tapReady = STAR_GameKeyDown[player][input].pressed = true;
	}

	// Return, and We're Done :) //
	return STAR_GameKeyDown[player][input].pressed;
}

//
// void TSoURDt3rd_BuildTicCMD(UINT8 player)
// Builds TSoURDt3rd's Custom Keybinds and Runs Their Functions
//
void TSoURDt3rd_BuildTicCMD(UINT8 player)
{
	// Make the Variables //
	// Jukebox
	boolean openjukeboxkey;
	boolean increasemusicspeedkey, decreasemusicspeedkey;
	boolean playmostrecenttrackkey;
	boolean stopjukeboxkey;

	// Give Variables Their Values //
	// Jukebox
	openjukeboxkey			= STAR_Responder(player, JB_OPENJUKEBOX, true);

	increasemusicspeedkey	= STAR_Responder(player, JB_INCREASEMUSICSPEED, false);
	decreasemusicspeedkey	= STAR_Responder(player, JB_DECREASEMUSICSPEED, false);

	playmostrecenttrackkey	= STAR_Responder(player, JB_PLAYMOSTRECENTTRACK, true);

	stopjukeboxkey			= STAR_Responder(player, JB_STOPJUKEBOX, true);

	// Run the Main Inputs //
	// Jukebox Inputs
	// Open the Jukebox Menu if the Key was Pressed
	if (openjukeboxkey)
	{
		M_StartControlPanel();
		M_TSoURDt3rdJukebox(0);

		currentMenu->prevMenu = NULL;
	}

	// Increase the Music Speed if the Key was Pressed
	if (increasemusicspeedkey)
		(atof(cv_jukeboxspeed.string) >= 20.0f ?
			(STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Can't increase the speed any further!\n")) :
			(CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)+(0.1f)))));

	// Decrease the Music Speed if the Key was Pressed
	if (decreasemusicspeedkey)
		(atof(cv_jukeboxspeed.string) < 0.1f ?
			(STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Can't decrease the speed any further!\n")) :
			(CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)-(0.1f)))));

	// Replay the Most Recent Jukebox Track if the Key was Pressed
	if (playmostrecenttrackkey)
	{
		// Haven't Recently Played a Track
		if (!TSoURDt3rd->jukebox.lastTrackPlayed)
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "You haven't recently played a track!\n");

		// Already Have the Track Playing
		else if (TSoURDt3rd->jukebox.musicPlaying)
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "There's already a track playing!\n");

		// Run Everything Normally, and We're Done :)
		else
		{
			M_TSoURDt3rdJukebox(0);

			strcpy(TSoURDt3rd->jukebox.musicName, (TSoURDt3rd_InAprilFoolsMode() ? "Get rickrolled lol" : TSoURDt3rd->jukebox.lastTrackPlayed->title));
			strcpy(TSoURDt3rd->jukebox.musicTrack, (TSoURDt3rd_InAprilFoolsMode() ? "_hehe" : TSoURDt3rd->jukebox.lastTrackPlayed->name));

			S_ChangeMusicInternal(TSoURDt3rd->jukebox.musicTrack, !TSoURDt3rd->jukebox.lastTrackPlayed->stoppingtics);
			S_SpeedMusic(atof(cv_jukeboxspeed.string));

			STAR_CONS_Printf(STAR_CONS_JUKEBOX, M_GetText("Loaded track \x82%s\x80.\n"), TSoURDt3rd->jukebox.musicName);

			TSoURDt3rd->jukebox.musicPlaying			= true;
			TSoURDt3rd->jukebox.initHUD					= true;
		}
	}

	// Stop the Jukebox if the Key was Pressed
	if (stopjukeboxkey)
	{
		if (!TSoURDt3rd->jukebox.musicPlaying)
		{
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Nothing is currently playing in the jukebox!\n");
			S_StartSound(NULL, sfx_lose);
		}
		else
		{
			M_ResetJukebox();

			S_StopSounds();
			S_StopMusic();

			S_StartSound(NULL, sfx_skid);

			if (Playing())
			{
				player_t *rplayer = &players[consoleplayer];
				(TSoURDt3rd_InAprilFoolsMode() ?
					// Do Funniness for April Fools
					(S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0)) :

					// Regularly Execute Everything
					(P_RestoreMusic(rplayer)));
			}
		}
	}
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
	INT32 i;

	// Before we Start, Ensure Some Things //
	if (netgame)
	{
		CONS_Printf("You can't change this while in a netgame.\n");
		CV_StealthSetValue(&cv_storesavesinfolders, (!cv_storesavesinfolders.value ? 1 : 0));
		return;
	}

	// Erase the Strings, Just in Case //
	for (i = 0; savegamename[i] != '\0'; i++) savegamename[i] = '\0';
	for (i = 0; liveeventbackup[i] != '\0'; i++) liveeventbackup[i] = '\0';
	for (i = 0; savegamefolder[i] != '\0'; i++) savegamefolder[i] = '\0';

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

	// Merge the Variables Together, and We're Done :) //
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
// void TSoURDt3rd_TryToLoadTheExtras(void)
// Tries to Load Extras, Usually Stuff From tsourdt3rdextras.pk3
//
void TSoURDt3rd_TryToLoadTheExtras(void)
{
	// Run Some Checks First //
	// Are we Being Requested to Load tsourdt3rdextras.pk3? If not, Don't Run This.
	if (!TSoURDt3rd_LoadExtras)
		return;

	// Run the Main Code Now //
	if (eastermode || aprilfoolsmode || xmasmode)
	{
		// Easter Specific Stuff
		if (eastermode && (!netgame && !TSoURDt3rd_TouchyModifiedGame))
		{
			CV_StealthSetValue(&cv_alloweasteregghunt, 1);
			AllowEasterEggHunt = true;

			M_UpdateEasterStuff();
		}

		// General Stuff
		STAR_ReadExtraData();
	}

	// Set Our Variables, and We're Done :) //
	TSoURDt3rd_LoadedExtras = true;
	TSoURDt3rd_LoadExtras = false;
}

//
// INT32 STAR_DetectFileType(const char* filename)
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

//
// boolean STAR_DoesStringMatchHarcodedFileName(const char *string)
// Detects if the String Given Matches the Name of a Hardcoded File, and Returns True if it Does
//
boolean STAR_DoesStringMatchHarcodedFileName(const char *string)
{
	// Does the String Given Match the Name of a Hardcoded File? If so, Return True. //
	if ((strcmp(string, CONFIGFILENAME) == 0)
		|| (strcmp(string, AUTOLOADCONFIGFILENAME) == 0)

		|| (strcmp(string, "adedserv.cfg") == 0)
		|| (strcmp(string, "autoexec.cfg") == 0)

		|| (strcmp(string, "srb2.pk3") == 0)
		|| (strcmp(string, "zones.pk3") == 0)
		|| (strcmp(string, "player.dta") == 0)
		|| (strcmp(string, "music.dta") == 0)
#ifdef USE_PATCH_DTA
		|| (strcmp(string, "patch.pk3") == 0)
#endif

		|| (strcmp(string, "tsourdt3rd.pk3") == 0)
		|| (strcmp(string, "jukebox.pk3") == 0))

		return true;

	// The String Given Doesn't Match the Name of a Hardcoded File? Return False Then. //
	return false;
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
	while (hms_tsourdt3rd_api == NULL) STAR_FindAPI(API);
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
	INT32 i;

	// Reset the Main Variable //
	for (i = 0; finalRETURNINFO[i] != '\0'; i++) finalRETURNINFO[i] = '\0';
	strcpy(finalRETURNINFO, " ");

	// Create the File //
	webinfofilelocation = va("%s"PATHSEP"%s", srb2home, "tsourdt3rd_grabbedwebsiteinfo.html");
	webinfo = fopen(webinfofilelocation, "w+");

	// Find the API //
	while (hms_tsourdt3rd_api == NULL) STAR_FindAPI(API);
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
				// We've Found the String, so Close the File, Remove it, Clean up Curl, Clean up the String and Return it, and We're Done :)
				if (fastncmp(finalRETURNINFO, RETURNINFO, sizeof(RETURNINFO)))
				{
					if (verbose)
						CONS_Printf("STAR_ReturnStringFromWebsite(): Found the info!\n");
					fclose(webinfo);

					remove(webinfofilelocation);
					curl_easy_cleanup(curl);

					for (i = 0; finalRETURNINFO[i] != '\0'; i++)
					{
						if (finalRETURNINFO[i] == '\n')
							finalRETURNINFO[i] = '\0';
					}

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


//
// void TSoURDt3rd_FindCurrentVersion(void)
// Finds the Current Version of TSoURDt3rd From the Github Repository
//
void TSoURDt3rd_FindCurrentVersion(void)
{
	// Make Some Variables //
	// STAR NOTE: If You're Planning on Using the Internet Functions, Use This Block as an Example :)
	const char *API = "https://raw.githubusercontent.com/StarManiaKG/The-Story-of-Uncapped-Revengence-Discord-the-3rd/";
	char URL[256];	strcpy(URL,	 va("%s/src/STAR/star_webinfo.h", compbranch));
	char INFO[256]; strcpy(INFO, va("#define TSOURDT3RDVERSION \"%s\"", TSOURDT3RDVERSION));

	char RETURNINFO[256] = "#define TSOURDT3RDVERSION";
	char RETURNEDSTRING[256] = "";

	UINT32 internalVersionNumber;

	UINT32 displayVersionNumber;
	const char *displayVersionString;

	// Run Some Checks //
	if ((gamestate == GS_NULL)			// Have we Even Initialized the Game? If not, Don't Run This.
		|| (TSoURDt3rd->checkedVersion)	// Have we Already Checked the Version? If so, Don't Run This Again.
		|| (!cv_updatenotice.value))	// Do we Allow Screen or Console Messages? If not, Don't Run This.

		return;

	// Run the Main Code //
	// Check the Version, And If They Don't Match the Branch's Version, Run the Block Below
	CONS_Printf("STAR_FindStringOnWebsite() & STAR_ReturnStringFromWebsite(): Grabbing latest TSoURDt3rd version...\n");

	if (STAR_FindStringOnWebsite(API, URL, INFO, false) == 1)
	{
		strcpy(RETURNEDSTRING, STAR_ReturnStringFromWebsite(API, URL, RETURNINFO, false));

		internalVersionNumber = STAR_ConvertStringToCompressedNumber(RETURNEDSTRING, 0, 26, true);

		displayVersionNumber = STAR_ConvertStringToCompressedNumber(RETURNEDSTRING, 0, 26, false);
		displayVersionString = STAR_ConvertNumberToString(displayVersionNumber, 0, 0, true);

		if (TSoURDt3rd_CurrentVersion() < internalVersionNumber)
			(cv_updatenotice.value == 1 ?
				(M_StartMessage(va("%c%s\x80\nYou're using an outdated version of TSoURDt3rd.\n\nThe newest version is: %s\nYou're using version: %s\n\nCheck the SRB2 Message Board for the latest version!\n\n(Press any key to continue)\n", ('\x80' + (menuColor[cv_menucolor.value]|V_CHARCOLORSHIFT)), "Update TSoURDt3rd, Please", displayVersionString, TSOURDT3RDVERSION),NULL,MM_NOTHING)) :
				(STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "You're using an outdated version of TSoURDt3rd.\n\nThe newest version is: %s\nYou're using version: %s\n\nCheck the SRB2 Message Board for the latest version!\n", displayVersionString, TSOURDT3RDVERSION)));
		else if (TSoURDt3rd_CurrentVersion() > internalVersionNumber)
			(cv_updatenotice.value == 1 ?
				(M_StartMessage(va("%c%s\x80\nYou're using a version of TSoURDt3rd that hasn't even released yet.\n\nYou're probably a tester or coder,\nand in that case, hello!\n\nEnjoy messing around with the build!\n\n(Press any key to continue)\n", ('\x80' + (menuColor[cv_menucolor.value]|V_CHARCOLORSHIFT)), "Hello, Tester/Coder!"),NULL,MM_NOTHING)) :
				(STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "You're using a version of TSoURDt3rd that hasn't even released yet.\nYou're probably a tester or coder, and in that case, hello!\nEnjoy messing around with the build!\n")));
	}

	TSoURDt3rd->checkedVersion = true;
}
#endif // HAVE_CURL

// SERVERS //
//
// boolean STAR_FindServerInfractions(void)
// Finds Server Infractions, and Returns True if Any Were Found.
// These Can be Related to Having Too Many Skins for Vanilla Servers to Join, and More
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
		if (S_sfx[numsounds].priority > 0) continue;
		break;
	}
	for (numskinsounds = sfx_skinsoundslot0; numskinsounds <= sfx_lastskinsoundslot; numskinsounds++)
	{
		if (S_sfx[numskinsounds].priority > 0 && S_sfx[numskinsounds].name) continue;
		break;
	}
	accuratenumsounds = (numsounds - sfx_freeslot0);
	accuratenumskinsounds = (numskinsounds - sfx_skinsoundslot0);

	// Count the Sprites
	for (totalnumsprites = SPR_FIRSTFREESLOT; totalnumsprites <= SPR_LASTFREESLOT; totalnumsprites++)
	{
		if (used_spr[(totalnumsprites-SPR_FIRSTFREESLOT)/8] & (1<<(totalnumsprites%8))) continue;
		break;
	}
	for (totalnumsprite2s = SPR2_FIRSTFREESLOT; totalnumsprite2s < free_spr2; totalnumsprite2s++)
	{
		if (spr2names[totalnumsprite2s][0] != '\0') continue;
		break;
	}
	accuratenumsprites = (totalnumsprites - SPR_FIRSTFREESLOT);
	accuratenumsprite2s = (totalnumsprite2s - SPR2_FIRSTFREESLOT);

	// Count the States
	for (numstates = 0; numstates < NUMSTATEFREESLOTS; numstates++)
	{
		if (FREE_STATES[numstates]) continue;
		break;
	}

	// Count the Objects
	for (numobjects = 0; numobjects < NUMMOBJFREESLOTS; numobjects++)
	{
		if (FREE_MOBJS[numobjects]) continue;
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
		if (dedicated)
		{
			I_Error("You've loaded too many add-ons, and now your game isn't vanilla compatible.\n\nYour current values compared to the\nvanilla limits are:\n\nSkins - %d/29\nSounds - %d/2335\nSkin Sounds - %d/3007\nSprites - %d/512\nSprite2s - %d/68\nStates - %d/4096\nObjects - %d/512\n\nIf any of these values are above the vanilla limits, you won't be able to\nhost servers until you restart the game and remove some add-ons.\n", accuratenumskins, accuratenumsounds, accuratenumskinsounds, accuratenumsprites, accuratenumsprite2s, numstates, numobjects);
			return true;
		}

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
	INT32 i;

	char majorVersionString[256] = ""; strcpy(majorVersionString, TSOURDT3RDVERSION);
	char iterateString[256] = "";

	// Iterate Through Our Two Strings //
	for (i = 0; majorVersionString[i] != '\0'; i++)
	{
		iterateString[i] = majorVersionString[i];

		// Found a Dot? Return the Major Version, and We're Done!
		if (majorVersionString[i+1] == '.')
			return atoi(iterateString);
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
	INT32 i, j;

	char minorVersionString[256] = ""; strcpy(minorVersionString, TSOURDT3RDVERSION);
	char iterateString[256] = "";

	// Iterate Through Our Two Strings //
	for (i = 0; minorVersionString[i] != '\0'; i++)
	{
		// We Found One Dot, So Let's Iterate Through it, Stop at Another Dot, and We're Done :)
		if (minorVersionString[i] == '.')
		{
			i++;
			for (j = 0; (minorVersionString[i] != '.' && minorVersionString[i] != '\0'); j++, i++)
				iterateString[j] = minorVersionString[i];

			return atoi(iterateString);
		}
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
	INT32 i, j;
	boolean oneDotFound = false;

	char subVersionString[256] = ""; strcpy(subVersionString, TSOURDT3RDVERSION);
	char iterateString[256] = "";

	// Iterate Through Our Two Strings //
	for (i = 0; subVersionString[i] != '\0'; i++)
	{
		// We Found A Dot!
		if (subVersionString[i] == '.')
		{
			// One Dot Found, So Let's Skip It.
			if (!oneDotFound)
			{
				oneDotFound = true;
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
	}

	// This Outcome is More Reasonable, Honestly //
	return 0;
}

//
// INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionNumber)
// Converts Strings to Compressed Numbers
//
// Example of a Possible Return:
//	STRING == '2.8', turnIntoVersionNumber = true		=	Returned Number = 280
//	STRING == '2.7.1', turnIntoVersionNumber = false	=	Returned Number = 271
//
INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionNumber)
{
	// Make Variables //
	INT32 i = startIFrom, j = startJFrom;
	INT32 finalNumber;

	char convertedString[256] = "";

	// Initialize the Main String, and Iterate Through Our Two Strings //
	while (STRING[j] != '\0')
	{
		if (STRING[j] == '.' || STRING[j] == '"' || STRING[j] == ' ')
		{
			j++;
			continue;
		}

		convertedString[i] = STRING[j];
		i++, j++;
	}

	// Add an Extra Digit or Two if Our String Has Less Than 2 Digits, Else Return Our Compressed Number, and We're Done! //
	finalNumber = ((turnIntoVersionNumber && strlen(convertedString) <= 2) ?
					(strlen(convertedString) == 2 ? 
						(STAR_CombineNumbers(2, atoi(convertedString), 0)) :
						(STAR_CombineNumbers(3, atoi(convertedString), 0, 0))) :
					(atoi(convertedString)));
	return finalNumber;
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
			i++; j++;

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
// INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean turnIntoVersionNumber)
// Converts Numbers to Strings, and Then Converts Them Back to Numbers
//
INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean turnIntoVersionNumber)
{
	// Return The Number, and We're Done :) //
	char numberString[256] = ""; strcpy(numberString, STAR_ConvertNumberToString(NUMBER, startI1From, startJ1From, turnIntoVersionString));
	return STAR_ConvertStringToCompressedNumber(numberString, startI2From, startJ2From, turnIntoVersionNumber);
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
