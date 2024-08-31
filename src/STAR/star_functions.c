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

#include "star_vars.h" 			// star variables
#include "ss_main.h"			// star variables 2
#include "smkg-jukebox.h"		// star variables 3
#include "m_menu.h"				// star variables 4
#include "smkg_g_inputs.h"		// star variables 5
#include "menus/smkg_m_draw.h"	// star variables 6
#include "menus/smkg_m_func.h"	// star variables 7
#include "smkg-p_saveg.h"		// star variables 8
#include "smkg-misc.h"          // star variables 9

#include "drrr/k_menu.h"		// kart krew drrr variables

#include "../i_system.h"
#include "../doomdef.h"
#include "../byteptr.h"
#include "../m_misc.h" 			// extra file functions
#include "../m_menu.h" 			// egg and command variables
#include "../d_main.h" 			// event variables
#include "../deh_soc.h"			// savefile variables
#include "../keys.h"			// key variables

#include "../v_video.h"			// video variables
#include "../i_video.h"			// video variables 2

#include "../filesrch.h"		// file variables
#include "../r_skins.h"			// skin variables
#include "../sounds.h"			// sound variables
#include "../deh_tables.h"		// sprite/table variables
#include "../m_random.h"		// m_random functions
#include "../z_zone.h"			// memory variables

#include "../i_net.h"			// net variables

#ifdef HAVE_SDL
#include "SDL.h"				// sdl variables
#include "../sdl/sdlmain.h"		// sdl variables 2
#endif

#include "../hu_stuff.h"		// hud variables

#include "../console.h"			// console variables

//////////////////////////////////////
//		ABSOLUTELY HILARIOUS	 	//
//			STAR FUNCTIONS		 	//
//				YAY				 	//
//////////////////////////////////////

//// STRUCTS ////
TSoURDt3rd_t TSoURDt3rdPlayers[MAXPLAYERS];

TSoURDt3rdBossMusic_t bossMusic[] = {
	[1] = {"_s1b",	NULL, 		0},	// Sonic 1
	{"_scdb", 		NULL, 		0},	// Sonic CD
	{"_s2b", 		NULL, 		0},	// Sonic 2
	{"_s3b1", 		NULL, 		0},	// Sonic 3
	{"_&kb1", 		NULL, 		0},	// & Knuckles
	{"_s3kb", 		NULL, 		0},	// Sonic 3 & Knuckles

	{"_a2b", 	 "_a2bp", 		0},	// Sonic Advance 2 - R
	{"_a27b", 	"_a27bp", 		0},	// Sonic Advance 2 - Z7
	{"_a3b", 	 "_a3bp", 		0},	// Sonic Advance 3 - R
	{"_a37b", 	"_a37bp", 		0},	// Sonic Advance 3 - Z7
	{"_rms", 	 "_rmsp", 		0},	// Sonic Rush - MS
	{"_rvn", 	 "_rvnp", 		0},	// Sonic Rush - VN
	{"_rab", 	 	NULL,	 5500},	// Rush Adventure - R
	{"_ra7b", 	 	NULL, 		0},	// Rush Adventure - Z7

	{"_smrd", 		NULL, 		0},	// Sonic Mania: RD
	{"_smhp", 		NULL, 		0},	// Sonic Mania: HP
	{"_smhbh", 		NULL, 		0}	// Sonic Mania: HBH
};

TSoURDt3rdFinalBossMusic_t finalBossMusic[] = {
	[1] = {"_s1fb",	NULL,	   	  NULL,		   NULL},	// Sonic 1
	{"_scdfb",	  	NULL,		  NULL,    	   NULL},	// Sonic CD
	{"_s2fb",	  	NULL,	   	  NULL,    	   NULL},	// Sonic 2
	{"_s3kfb",	  	NULL,	   "_&kdz",	 	   NULL},	// Sonic 3&K

	{"_a2fb",	"_a2fbp",	"	_a253",    	   NULL},	// Sonic Advance 2
	{"_a3fb",   "_a3fbp",  	   "_a3eb",    "_a3ebp"},	// Sonic Advance 3
	{"_rbb", 	 "_rbbp",      "_rwib",    "_rwibp"},	// Sonic Rush
	{"_rafb", 	"_rafbp",      "_radc",    "_radcp"},	// Rush Adventure

	{"_smri",	  	NULL,	   "_smer",    "_smerp"}	// Sonic Mania
};

static TSoURDt3rdActClearMusic_t actClearMusic[] = {
	[1] = {"_s12ac",NULL,	  	NULL, 	   	   NULL},	// Sonic 1&2
	{"_scdac", 	   	NULL,	  	NULL,		   NULL},	// Sonic CD
	{"_s3kac",	   	NULL,    "_s3fc",		"_&kfc"},	// Sonic 3&K

	{"_a2ac",	 "_a2bc",	"_a2fbc",	   "_a2tfb"},	// Sonic Advance 2
	{"_a3ac",	 "_a3bc",	"_a3fbc",	   "_a3tfb"},	// Sonic Advance 3
	{"_rac",	  "_rbc",    "_rfbc",      "_rtfbc"},	// Sonic Rush
	{"_raac",	 "_rabc",   "_rafbc",      "_ratfb"},	// Rush Adventure

	{"_smac",	   	NULL,	  	NULL,		   NULL},	// Sonic Mania

	{"_btsac",	   	NULL, 	  	NULL,		   NULL}	// Sonic BTS (Before the Sequel)
};

TSoURDt3rdDefaultMusicTracks_t defaultMusicTracks[] = {
	[1] = {"gfz1"},	// GFZ1
	{"_runin"}		// DooM Wad Anthem
};

TSoURDt3rdBossMusic_t *curBossMusic = NULL;
TSoURDt3rdFinalBossMusic_t *curFinaleBossMusic = NULL;

//// COMMANDS ////
consvar_t cv_loadingscreen = CVAR_INIT ("loadingscreen", "Off", CV_SAVE, CV_OnOff, NULL);

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

static CV_PossibleValue_t vapemode_t[] = {{0, "Off"}, {1, "TSoURDt3rd"}, {2, "Sonic Mania Plus"}, {0, NULL}};
consvar_t cv_vapemode = CVAR_INIT ("vapemode", "Off", CV_SAVE|CV_CALL, vapemode_t, TSoURDt3rd_ControlMusicEffects);

//// FUNCTIONS ////
// GAME //
//
// void STAR_LoadingScreen(void)
// Displays a Loading Screen
//
void STAR_LoadingScreen(void)
{
	// Make Variables //
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];

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
			(loadingscreenlumpnumtype[(cv_loadingscreenimage.value == 1 || cv_loadingscreenimage.value == 21) ? TSoURDt3rd->loadingScreens.screenToUse : cv_loadingscreenimage.value]),
			(PU_CACHE)));
	}

	// Run Some Other Necessary Functions Here, and We're Done :) //
	M_DrawTextBox(x-58, y-8, 13, 1);
	V_DrawString(x-50, y, V_MENUCOLORMAP, "Loading...");
	V_DrawRightAlignedString(x+50, y, V_MENUCOLORMAP, s);

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
const char *STAR_SetWindowTitle(void)
{
	// Make Variables //
	static char randomTitleTable[5][256];

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
				case GS_INTERMISSION: dynamictitle = (cv_memesonwindowtitle.value ? "End of Chapter! -" : (!mapheaderinfo[gamemap-1]->actnum ? (va("%s Got Through the Act! -", skins[players[consoleplayer].skin].realname)) : (va("%s Got Through Act %d! -", skins[players[consoleplayer].skin].realname, mapheaderinfo[gamemap-1]->actnum)))); break;

				case GS_CREDITS:
				case GS_ENDING:
				case GS_EVALUATION:
				case GS_GAMEEND: dynamictitle = (cv_memesonwindowtitle.value ? "Did You Get All Those Chaos Emeralds? -" : "The End of"); break;

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
// void TSoURDt3rd_BuildTicCMD(UINT8 player)
// Builds TSoURDt3rd's Custom Keybinds and Runs Their Functions
//
void TSoURDt3rd_BuildTicCMD(UINT8 player)
{
	// Check for keys //
	boolean jukebox_open = STAR_G_KeyPressed(player, JB_OPENJUKEBOX);
	boolean jukebox_increasespeed = STAR_G_KeyHeld(player, JB_INCREASEMUSICSPEED);
	boolean jukebox_decreasespeed = STAR_G_KeyHeld(player, JB_DECREASEMUSICSPEED);
	boolean jukebox_recent_track = STAR_G_KeyPressed(player, JB_PLAYMOSTRECENTTRACK);
	boolean jukebox_stop = STAR_G_KeyPressed(player, JB_STOPJUKEBOX);

	if (!tsourdt3rd_global_jukebox)
		return;

	if (demoplayback && titledemo)
		return;

	if (CON_Ready() || chat_on || menutyping.active)
		return; // With our menu revamps, might be best to add this here...

	if (player != consoleplayer)
		return; // don't run for bots please

	if (STAR_M_DoesMenuHaveKeyHandler())
		return; // we may need to type or manually move, so calm down right quick

	if (optionsmenu.bindtimer)
		return; // we're binding inputs right now, so please don't

	// Manage keys //
	if (jukebox_open)
	{
		// Shortcut to open the jukebox menu
#if 1
#if 0
#if 1
		if (Playing())
			M_StartControlPanel();
#endif
		TSoURDt3rd_Jukebox_InitMenu(0);
#if 1
		// Prevent the game from crashing when using the jukebox keybind :)
		currentMenu->prevMenu = NULL;
#endif
#else
		if (currentMenu == &OP_TSoURDt3rdJukeboxDef)
			return;

		// Prevent the game from crashing when using the jukebox keybind :)
		if (!menuactive)
		{
			OP_TSoURDt3rdJukeboxDef.prevMenu = NULL;
			M_StartControlPanel();
		}
		else
			OP_TSoURDt3rdJukeboxDef.prevMenu = currentMenu;
		TSoURDt3rd_Jukebox_InitMenu(0);
#endif
#else
		M_StartControlPanel();
		currentMenu = &OP_TSoURDt3rdJukeboxDef;
		currentMenu->lastOn = 0;
#endif
	}

	if (jukebox_increasespeed)
	{
		// Increase the speed of the jukebox track
		if (atof(cv_jukeboxspeed.string) > 20.0f)
		{
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Can't increase the speed any further!\n");
			S_StartSound(NULL, sfx_skid);
		}
		else
			CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)+(0.1f)));
	}

	if (jukebox_decreasespeed)
	{
		// Decrease the speed of the jukebox track
		if (atof(cv_jukeboxspeed.string) < 0.1f)
		{
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Can't decrease the speed any further!\n");
			S_StartSound(NULL, sfx_skid);
		}
		else
			CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)-(0.1f)));
	}

	if (jukebox_recent_track)
	{
		// Replay the most recent jukebox track
		if (!tsourdt3rd_global_jukebox->prevtrack || (tsourdt3rd_global_jukebox->prevtrack && *tsourdt3rd_global_jukebox->prevtrack->name == '\0'))
		{
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "You haven't recently played a track!\n");
			S_StartSound(NULL, sfx_lose);
		}
		else if (tsourdt3rd_global_jukebox->curtrack)
		{
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "There's already a track playing!\n");
			S_StartSound(NULL, sfx_lose);
		}
		else if (TSoURDt3rd_Jukebox_Unlocked())
		{
			S_ChangeMusicInternal(tsourdt3rd_global_jukebox->prevtrack->name, !tsourdt3rd_global_jukebox->prevtrack->stoppingtics);
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, M_GetText("Loaded track \x82%s\x80.\n"), tsourdt3rd_global_jukebox->prevtrack->title);

			tsourdt3rd_global_jukebox->curtrack = tsourdt3rd_global_jukebox->prevtrack;
			tsourdt3rd_global_jukebox->initHUD = false;
			tsourdt3rd_global_jukebox->playing = true;

			TSoURDt3rd_ControlMusicEffects();
		}
		else
		{
			DRRR_M_StartMessage(
				"TSoURDt3rd Jukebox",
				M_GetText("You haven't unlocked this yet!\nGo and unlock the sound test first!\n"),
				NULL,
				MM_NOTHING,
				NULL,
				NULL
			);
			S_StartSound(NULL, sfx_lose);
		}
	}

	if (jukebox_stop)
	{
		// Stop and reset the jukebox
		if (!tsourdt3rd_global_jukebox->curtrack)
		{
			STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Nothing is currently playing in the jukebox!\n");
			S_StartSound(NULL, sfx_lose);
		}
		else
		{
			S_StopSounds();
			S_StopMusic();

			S_StartSound(NULL, sfx_skid);
			TSoURDt3rd_Jukebox_RefreshLevelMusic();
		}
	}
}

// AUDIO //
//
// void TSoURDt3rd_ControlMusicEffects(void)
// Controls the Effects of the Currently Playing Music, Based on Factors like Vape Mode
//
void TSoURDt3rd_ControlMusicEffects(void)
{
	if (tsourdt3rd_global_jukebox->playing)
	{
		S_SpeedMusic(atof(cv_jukeboxspeed.string));
		S_PitchMusic(1.0f);
		return;
	}

	switch (cv_vapemode.value)
	{
		case 1:
			S_SpeedMusic(0.9f);
			S_PitchMusic(0.9f);
			break;
		case 2:
			S_SpeedMusic(0.75f);
			S_PitchMusic(0.5f);
			break;
		default:
			S_SpeedMusic(1.0f);
			S_PitchMusic(1.0f);
			break;
	}
}

//
// const char *TSoURDt3rd_DetermineLevelMusic(void)
// Determines and Returns What Music Should be Played on the Current Stage
//
#define MUSICEXISTS(music) (music && S_MusicExists(music, !midi_disabled, !digital_disabled))

const char *TSoURDt3rd_DetermineLevelMusic(void)
{
	mobj_t *mobj = TSoURDt3rd_BossInMap();

	boolean pinchPhase = ((mobj && mobj->health <= (mobj->info->damage ? mobj->info->damage : 3)) && cv_bosspinchmusic.value);
	boolean allEmeralds = (emeralds == 127);

	boolean bossMap = (mobj && (mapheaderinfo[gamemap-1]->bonustype == 1 || (mapheaderinfo[gamemap-1]->levelflags & LF_WARNINGTITLE))); // Boss BonusType or Warning Title
	boolean finalBossMap = (mobj && ((mapheaderinfo[gamemap-1]->bonustype == 2 || mapheaderinfo[gamemap-1]->typeoflevel & TOL_ERZ3 || (mapheaderinfo[gamemap-1]->levelflags & LF_WARNINGTITLE)) // ERZ3 BonusType, ERZ3 TypeOfLevel, or Warning Title
		&& ((mapheaderinfo[gamemap-1]->nextlevel == 1101) // Evaluation
			|| (mapheaderinfo[gamemap-1]->nextlevel == 1102) // Credits
			|| (mapheaderinfo[gamemap-1]->nextlevel == 1103)))); // Ending
	boolean trueFinalBossMap = (finalBossMap && allEmeralds);

	// Conflicting music //
	// Event music //
	if (TSoURDt3rd_AprilFools_ModeEnabled())
		return "_hehe";

	// Gamestate-based music //
	switch (gamestate)
	{
		case GS_INTERMISSION:
		{
			if (!cv_actclearmusic.value || (cv_actclearmusic.value && !actClearMusic[cv_actclearmusic.value].actClear))
				return (MUSICEXISTS(mapheaderinfo[gamemap-1]->musintername) ? mapheaderinfo[gamemap-1]->musintername : "_clear");

			if (!cv_bossclearmusic.value)
				return actClearMusic[cv_actclearmusic.value].actClear;

			if (trueFinalBossMap && actClearMusic[cv_actclearmusic.value].trueFinalBossClear)
				return actClearMusic[cv_actclearmusic.value].trueFinalBossClear;
			else if (finalBossMap && actClearMusic[cv_actclearmusic.value].finalBossClear)
				return actClearMusic[cv_actclearmusic.value].finalBossClear;
			else if (bossMap && actClearMusic[cv_actclearmusic.value].bossClear)
				return actClearMusic[cv_actclearmusic.value].bossClear;

			return actClearMusic[cv_actclearmusic.value].actClear;
		}

		case GS_EVALUATION:
		case GS_GAMEEND:
		{
			if (!cv_actclearmusic.value)
				break;

			if (allEmeralds && actClearMusic[cv_actclearmusic.value].trueFinalBossClear)
				return actClearMusic[cv_actclearmusic.value].trueFinalBossClear;
			else if (actClearMusic[cv_actclearmusic.value].finalBossClear)
				return actClearMusic[cv_actclearmusic.value].finalBossClear;

			break;
		}

		case GS_LEVEL:
		default:
		{
			if ((mobj && mobj->health <= 0)
				&& (cv_postbossmusic.value && MUSICEXISTS(mapheaderinfo[gamemap-1]->muspostbossname)))
			{
				return mapheaderinfo[gamemap-1]->muspostbossname;
			}
			else if (finalBossMap)
			{
				if (!cv_finalbossmusic.value)
					break;

				if (pinchPhase)
				{
					curFinaleBossMusic = &finalBossMusic[cv_finalbossmusic.value];

					if ((trueFinalBossMap && cv_truefinalbossmusic.value)
						&& finalBossMusic[cv_finalbossmusic.value].trueFinalBossPinchMusic)

						return finalBossMusic[cv_finalbossmusic.value].trueFinalBossPinchMusic;
					else if (finalBossMusic[cv_finalbossmusic.value].finalBossPinchMusic)
						return finalBossMusic[cv_finalbossmusic.value].finalBossPinchMusic;
				}

				if ((trueFinalBossMap && cv_truefinalbossmusic.value)
					&& finalBossMusic[cv_finalbossmusic.value].trueFinalBossMusic)

					return finalBossMusic[cv_finalbossmusic.value].trueFinalBossMusic;
				else if (finalBossMusic[cv_finalbossmusic.value].finalBossMusic)
					return finalBossMusic[cv_finalbossmusic.value].finalBossMusic;
			}
			else if (bossMap)
			{
				if (!cv_bossmusic.value)
					break;

				if (pinchPhase)
				{
					curBossMusic = &bossMusic[cv_bossmusic.value];

					if (bossMusic[cv_bossmusic.value].bossPinchMusic)
						return bossMusic[cv_bossmusic.value].bossPinchMusic;
				}

				if (bossMusic[cv_bossmusic.value].bossMusic)
					return bossMusic[cv_bossmusic.value].bossMusic;
			}

			break;
		}
	}	

	// Made it here? Play the map's default track, and we're done :) //
	if (gamestate == GS_TITLESCREEN || titlemapinaction)
	{
		if (MUSICEXISTS(mapheaderinfo[gamemap-1]->musname))
			return mapheaderinfo[gamemap-1]->musname;
		if (MUSICEXISTS("_title"))
			return "_title";
		return mapmusname;
	}

	if (RESETMUSIC || strnicmp(S_MusicName(),
		((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname), 7))
		return ((mapmusflags & MUSIC_RELOADRESET) ? mapheaderinfo[gamemap-1]->musname : mapmusname);
	else
		return mapheaderinfo[gamemap-1]->musname;

#if 0
	if (strnicmp(TSoURDt3rd_DetermineLevelMusic(), S_MusicName(), 7))
		return mapmusname;
#endif
	return ((!mapmusname[0] || !strnicmp(mapmusname, S_MusicName(), 7)) ? mapheaderinfo[gamemap-1]->musname : mapmusname);
}

UINT32 TSoURDt3rd_PinchMusicPosition(void)
{
	if (!curBossMusic || !curBossMusic->pinchMusicPos)
		return mapmusposition;
	return curBossMusic->pinchMusicPos;
}

boolean TSoURDt3rd_SetPinchMusicSpeed(void)
{
	if (curBossMusic && !curBossMusic->bossPinchMusic)
		return true;
	if (curFinaleBossMusic && !curFinaleBossMusic->finalBossPinchMusic && !curFinaleBossMusic->trueFinalBossPinchMusic)
		return true;
	return false;
}


#undef MUSICEXISTS

// SAVEDATA //
//
// void STAR_SetSavefileProperties(void)
// Sets the Current Savefile Name and Position
//
void STAR_SetSavefileProperties(void)
{
#if 0
	// Before we Start, Ensure Some Things //
	if (netgame)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "You can't change this while in a netgame.\n");
		CV_StealthSetValue(&cv_tsourdt3rd_savefiles_storesavesinfolders, !cv_tsourdt3rd_savefiles_storesavesinfolders.value);

		return;
	}
#endif

	// Erase the Strings, Just in Case //
	memset(savegamename, 0, sizeof(savegamename));
	memset(liveeventbackup, 0, sizeof(liveeventbackup));
	memset(savegamefolder, 0, sizeof(savegamefolder));

	// Make the Folder //
	if (cv_tsourdt3rd_savefiles_storesavesinfolders.value)
	{
		I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER, TSoURDt3rd_FOL_ReturnHomepath()), 0755);
		if (TSoURDt3rd_useAsFileName)
		{
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "TSoURDt3rd", TSoURDt3rd_FOL_ReturnHomepath()), 0755);
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "TSoURDt3rd" PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), timeattackfolder), 0755);
		}
		else
			I_mkdir(va("%s" PATHSEP SAVEGAMEFOLDER PATHSEP "%s", TSoURDt3rd_FOL_ReturnHomepath(), timeattackfolder), 0755);
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
	if (!cv_tsourdt3rd_savefiles_storesavesinfolders.value)
	{
		strcatbf(savegamename, TSoURDt3rd_FOL_ReturnHomepath(), PATHSEP);
		strcatbf(liveeventbackup, TSoURDt3rd_FOL_ReturnHomepath(), PATHSEP);
	}
	else
	{
		strcatbf(savegamename, TSoURDt3rd_FOL_ReturnHomepath(), va(PATHSEP"%s"PATHSEP, savegamefolder));
		strcatbf(liveeventbackup, TSoURDt3rd_FOL_ReturnHomepath(), va(PATHSEP"%s"PATHSEP, savegamefolder));
	}
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
		if (eastermode && !netgame)
		{
			CV_StealthSetValue(&cv_easter_allowegghunt, 1);
			AllowEasterEggHunt = true;
		}

		// General Stuff
		TSoURDt3rd_PSav_ReadExtraData();
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
//	0 - Unsupported/Unknown
//
// 	1 - Folder
//
// 	2 - WAD
// 	3 - PK3
//	4 - KART (if USE_KART enabled)
//
//	5 - LUA
//	6 - SOC
//
//	7 - STAR
//
//	8 - CFG
//	9 - TXT
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

		else if (!stricmp(&filename[strlen(filename) - 5], ".star"))
			return 7;

		else if (!stricmp(&filename[strlen(filename) - 4], ".cfg"))
			return 8;
		else if (!stricmp(&filename[strlen(filename) - 4], ".txt"))
			return 9;
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
