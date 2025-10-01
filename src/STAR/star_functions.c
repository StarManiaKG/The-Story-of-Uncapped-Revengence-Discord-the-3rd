// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_functions.c
/// \brief Contains all the Info Portraying to TSoURDt3rd's Variables and STAR Functions

/// \todo burn

#include "star_vars.h" 				// star variables
#include "ss_main.h"				// star variables 2
#include "core/smkg-s_jukebox.h"	// star variables 3
#include "smkg-p_saveg.h"			// star variables 6
#include "smkg-defs.h"				// star variables 8
#include "menus/smkg-m_sys.h"		// star variables 5
#include "misc/smkg-m_misc.h"    	// star variables 7

#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../m_misc.h" 				// extra file functions
#include "../m_menu.h" 				// egg and command variables
#include "../d_main.h" 				// event variables
#include "../deh_soc.h"				// savefile variables
#include "../keys.h"				// key variables

#include "../v_video.h"				// video variables
#include "../i_video.h"				// video variables 2

#include "../filesrch.h"			// file variables
#include "../r_skins.h"				// skin variables
#include "../deh_tables.h"			// sprite/table variables
#include "../m_random.h"			// m_random functions
#include "../z_zone.h"				// memory variables

#include "../netcode/i_net.h"		// net variables

#ifdef HAVE_SDL
#include "SDL.h"					// sdl variables
#include "../sdl/sdlmain.h"			// sdl variables 2
#endif

#include "../hu_stuff.h"			// hud variables

#include "../console.h"				// console variables

//////////////////////////////////////
//		ABSOLUTELY HILARIOUS	 	//
//			STAR FUNCTIONS		 	//
//				YAY				 	//
//////////////////////////////////////

//// STRUCTS ////
TSoURDt3rd_t TSoURDt3rdPlayers[MAXPLAYERS];

//// FUNCTIONS ////
// GAME //
//
// void STAR_LoadingScreen(void)
// Displays a Loading Screen
//
void STAR_LoadingScreen(void)
{
	// Make Variables //
	char s[16];
	INT32 x, y;

	static const char *loadingscreenlumpnumtype[] = {
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
	sprintf(s, "%d%%", (++tsourdt3rd_loadingscreen.loadPercentage)<<1);
	x = BASEVIDWIDTH/2;
	y = BASEVIDHEIGHT/2;
	V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 31); // Black background to match fade in effect

	// Set Our Loading Screen //
	if (cv_tsourdt3rd_game_loadingscreen_image.value)
	{
		// Dynamic
		if (cv_tsourdt3rd_game_loadingscreen_image.value == 1)
		{
			// In-Game
			if (Playing())
			{
				// Modified-Game Images
				if (tsourdt3rd_local.autoloaded_mods || savemoddata || modifiedgame)
					tsourdt3rd_loadingscreen.screenToUse = 3;

				// Vanilla/Unmodified-Game Images
				else
				{
					switch (gamemap)
					{
						// GFZ
						case 1:
						case 2:
						case 3: tsourdt3rd_loadingscreen.screenToUse = 4; break;

						// THZ
						case 4:
						case 5:
						case 6: tsourdt3rd_loadingscreen.screenToUse = 5; break;

						// DSZ
						case 7:
						case 8:
						case 9: tsourdt3rd_loadingscreen.screenToUse = 6; break;

						// CEZ
						case 10:
						case 11:
						case 12: tsourdt3rd_loadingscreen.screenToUse = 7; break;

						// ACZ
						case 13:
						case 14:
						case 15: tsourdt3rd_loadingscreen.screenToUse = 8; break;

						// RVZ
						case 16: tsourdt3rd_loadingscreen.screenToUse = 9; break;

						// ERZ
						case 22:
						case 23: tsourdt3rd_loadingscreen.screenToUse = 10; break;

						// BCZ
						case 25:
						case 26:
						case 27: tsourdt3rd_loadingscreen.screenToUse = 11; break;

						// FHZ
						case 30: tsourdt3rd_loadingscreen.screenToUse = 12; break;

						// PTZ
						case 31: tsourdt3rd_loadingscreen.screenToUse = 13; break;

						// FFZ
						case 32: tsourdt3rd_loadingscreen.screenToUse = 14; break;

						// TLZ
						case 33: tsourdt3rd_loadingscreen.screenToUse = 15; break;

						// HHZ
						case 40: tsourdt3rd_loadingscreen.screenToUse = 16; break;

						// AGZ
						case 41: tsourdt3rd_loadingscreen.screenToUse = 17; break;

						// ATZ
						case 42: tsourdt3rd_loadingscreen.screenToUse = 18; break;

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
						case 73: tsourdt3rd_loadingscreen.screenToUse = 19; break;

						// BHZ
						case 57: tsourdt3rd_loadingscreen.screenToUse = 20; break;

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

						default: tsourdt3rd_loadingscreen.screenToUse = 3; break;
					}
				}
			}

			// Not In-Game
			else
				tsourdt3rd_loadingscreen.screenToUse = 2;
		}

		// Random
		else if (cv_tsourdt3rd_game_loadingscreen_image.value == 21 && !tsourdt3rd_loadingscreen.screenToUse)
			tsourdt3rd_loadingscreen.screenToUse = M_RandomRange(2, 20);

		// Finally, Apply the Image, and We're Good Here :) //
		V_DrawPatchFill(W_CachePatchName(
			(loadingscreenlumpnumtype[(cv_tsourdt3rd_game_loadingscreen_image.value == 1 || cv_tsourdt3rd_game_loadingscreen_image.value == 21) ? tsourdt3rd_loadingscreen.screenToUse : cv_tsourdt3rd_game_loadingscreen_image.value]),
			(PU_CACHE)));
	}

	// Run Some Other Necessary Functions Here, and We're Done :) //
	M_DrawTextBox(x-58, y-8, 13, 1);
	V_DrawString(x-50, y, V_MENUCOLORMAP, "Loading...");
	V_DrawRightAlignedString(x+50, y, V_MENUCOLORMAP, s);

	I_OsPolling();

	//CON_Drawer(); // let the user know what we are doing
	I_FinishUpdate(); // page flip or blit buffer
	I_UpdateNoVsync();

	if (moviemode)
		M_SaveFrame();

	NetKeepAlive(); // Update the network so we don't cause timeouts
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
#endif

//
// const char *STAR_SetWindowTitle(void)
// Sets SRB2's Window Title
//
const char *STAR_SetWindowTitle(void)
{
	// Make Variables //
	static char randomTitleTable[5][256];

	const char *windowtitle = "";
	const char *dynamictitle = "";

	// Configure the Window Title //
	// Default Title
	if (!cv_tsourdt3rd_game_sdl_windowtitle_type.value)
		windowtitle = ("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING);

	// Others
	else
	{
		// Dynamic Titles
		if (cv_tsourdt3rd_game_sdl_windowtitle_type.value == 1)
		{
			// Main Window Titles
			// Map Specific
			if (Playing())
			{
				// General Game Titles
				if ((tsourdt3rd_local.autoloaded_mods || savemoddata || modifiedgame)
					|| (gamemap == titlemap)
					|| (players[consoleplayer].powers[pw_super])
					|| (tutorialmode))

					goto generalgametitles;

				// Level Titles
				switch (gamemap)
				{
					// GFZ
					case 1: case 2: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Where ARE the Green Flowers in" : "The Green Beginning -"); break;

					// THZ
					case 4:
					case 5:
					{
						if (cv_tsourdt3rd_game_sdl_windowtitle_memes.value)
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
					case 7: case 8: dynamictitle = ((fastncmp(skins[players[consoleplayer].skin]->name, "sonic", 5)) ? ("Ugh, I Hate Water in") : ("Swimming Around in")); break;

					// CEZ
					case 10: case 11: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "How Did Eggman Manage to Build This Castle so Fast in" : "Such a Large Castle in"); break;

					// ACZ
					case 13: case 14: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Playing Through Grand Canyon Zone in" : "Why is There So Much TNT in"); break;

					// Fang
					case 15: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "There is a Jerboa With a Popgun in" : "We're on a Train in"); break;

					// RVZ
					case 16: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Where is the Blue Volcano in" : "Too Much Lava in"); break;

					// ERZ
					case 22: case 23: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Robotnik has Too Many Rocks in" : "Be Careful Not to Fall Into Space in"); break;

					// Metal Sonic
					case 25: case 26: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "STRRANGE, ISN'T IT? -" : "Beating a Doppelgänger Hedgehog Robot in"); break;

					// Bosses
					case 3: case 6: case 9: case 12: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "He is the Eggman, With the Master Plan -" : "Fighting a Giant Talking Egg in"); break;

					// Black Eggman (Yes, Technically Brak's Name is Black Eggman)
					case 27: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "No Way Guys, the Cyberdemon is in" : "Fighting the Final Boss in"); break;

					// FHZ
					case 30: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Use the 'Destroy All Enemies' Cheat in" : "Be Careful Not to Slip in"); break;

					// PTZ
					case 31:
					{
						if (cv_tsourdt3rd_game_sdl_windowtitle_memes.value)
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
					case 40: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "No Way Guys, Cacodemons Are Also in" : "The Final Boss in"); break;

					// AGZ
					case 41:
					{
						if (cv_tsourdt3rd_game_sdl_windowtitle_memes.value)
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
					case 53: case 63: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Trying to Get That DAMN FOURTH Chaos Emerald in" : "Trying to Get All Those Chaos Emeralds in"); break;

					// BHZ
					case 57: dynamictitle = "The True Final Boss in"; break;

					// The Extra Special Stages
					case 70:
					case 71:
					case 72:
					case 73: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Playing the Extra Special Stages For no Reason in" : "Playing the Extra Special Stages in"); break;

					// CTF Maps
					case 280:
					case 281:
					case 282:
					case 283:
					case 284:
					case 285:
					case 286:
					case 287:
					case 288: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Playing Zandronum in" : "Capturing Flags in"); break;

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
						if (cv_tsourdt3rd_game_sdl_windowtitle_memes.value)
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
							dynamictitle = "Having a Friendly Deathmatch in";

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
				case GS_INTERMISSION: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "End of Chapter! -" : (!mapheaderinfo[gamemap-1]->actnum ? (va("%s Got Through the Act! -", skins[players[consoleplayer].skin]->realname)) : (va("%s Got Through Act %d! -", skins[players[consoleplayer].skin]->realname, mapheaderinfo[gamemap-1]->actnum)))); break;

				case GS_CREDITS:
				case GS_ENDING:
				case GS_EVALUATION:
				case GS_GAMEEND: dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Did You Get All Those Chaos Emeralds? -" : "The End of"); break;

				default:
				{
					if (gamestate == GS_NULL || (gamestate == GS_TITLESCREEN || titlemapinaction))
					{
#ifdef HAVE_SDL
						STAR_RenameWindow("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING);
#endif
						return ("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING);
					}

					break;
				}
			}

			windowtitle = va("%s SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING, dynamictitle);
		}

		// Semi-Custom Titles
		else if (cv_tsourdt3rd_game_sdl_windowtitle_type.value == 2)
			windowtitle = va("%s SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING, cv_tsourdt3rd_game_sdl_windowtitle_custom.string);

		// Fully Custom Titles
		else
			windowtitle = cv_tsourdt3rd_game_sdl_windowtitle_custom.string;
	}

	// Set the Window Title, Return it, and We're Done :) //
#ifdef HAVE_SDL
	STAR_RenameWindow(windowtitle);
#endif
	return windowtitle;

generalgametitles:
	// Did we Get Sent Here by a Goto? Let's Check Some Things First Then. //
	// Player is on The Titlemap
	if (gamemap == titlemap)
		dynamictitle = "What is Wrong With You -";

	// Super Character Window Title
	else if (players[consoleplayer].powers[pw_super])
		dynamictitle = (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "Playing as Goku in" : "Got All Those Chaos Emeralds in");

	// Player is Learning How to Play SRB2
	else if (tutorialmode)
		dynamictitle = "Learning How to Play";

	// Player is on a Custom Map
	else
		dynamictitle = va("%s as %s Through %s %s -", (cv_tsourdt3rd_game_sdl_windowtitle_memes.value ? "D_RUNNIN" : "Running"), skins[players[consoleplayer].skin]->realname, mapheaderinfo[gamemap-1]->lvlttl, ((mapheaderinfo[gamemap-1]->levelflags & LF_NOZONE) ? "" : "Zone"));

	// Now That We've Set Our Things, Let's Return our Window Title, and We're Done :)
	windowtitle = va("%s SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING, dynamictitle);

#ifdef HAVE_SDL
	STAR_RenameWindow(windowtitle);
#endif
	return windowtitle;
}

// MISCELLANIOUS //
//
// UINT32 TSoURDt3rd_CurrentVersion(void)
// Returns the Current Version of TSoURDt3rd, in a Converted, Compressed Number Format
//
UINT32 TSoURDt3rd_CurrentVersion(void)
{
	char *version_string = strdup(TSOURDT3RDVERSION);
	char *compressed_version_string = TSoURDt3rd_M_RemoveStringChars(version_string, ".");
	const UINT32 current_version = (UINT32)atoi(compressed_version_string);
	free(version_string);
	return current_version;
}
