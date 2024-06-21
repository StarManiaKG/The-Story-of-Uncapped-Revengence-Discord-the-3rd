// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  m_menu.c
/// \brief TSoURDt3rd related menu functions, structures, and data

#include "ss_main.h"
#include "m_menu.h"
#include "s_sound.h"
#include "../doomstat.h"
#include "../d_clisrv.h"
#include "../d_player.h"
#include "../g_game.h"
#include "../p_local.h"
#include "../m_random.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../i_time.h"
#include "../i_system.h"
#include "../i_video.h"
#include "../v_video.h"
#include "../m_misc.h"
#include "../r_draw.h"
#include "../hu_stuff.h"

#include "../d_main.h" // autoloaded/autoloading //
#include "../r_main.h" // shadows //

#ifdef HAVE_DISCORDRPC
#include "../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

//tsourdt3rd_menus_t *TSoURDt3rd_currentMenu = &MainDef;

INT16 MessageMenuDisplay[3][256]; // TO HACK

menuitem_t defaultMenuTitles[256][256];

// ====
// GAME
// ====

static void M_CheckForTSoURDt3rdUpdates(INT32 choice);

// =======
// JUKEBOX
// =======

static void M_DrawTSoURDt3rdJukebox(void);
static void M_HandleTSoURDt3rdJukebox(INT32 choice);

static void M_TSoURDt3rdJukeboxControls(INT32 choice);

// =====
// SNAKE
// =====

static void STAR_InitializeSnakeMenu(INT32 choice);
static void STAR_DrawSnakeMenu(void);
static void STAR_HandleSnakeMenu(INT32 choice);

// =====
// MISC.
// =====

static void STAR_SpawnDispenser(INT32 choice);

// =====
// MENUS
// =====

menuitem_t OP_Tsourdt3rdOptionsMenu[] =
{
	{IT_HEADER, 			NULL, 	"Event Options", 				NULL, 					  	 0},
	{IT_STRING | IT_CVAR, 	NULL,   "EASTER: Allow Egg Hunt",   	&cv_easter_allowegghunt,   	 6},
	{IT_STRING | IT_CVAR, 	NULL,   "EASTER: Egg Hunt Bonuses", 	&cv_easter_egghuntbonuses, 	11},

	{IT_STRING | IT_CVAR,	NULL,	"APRIL FOOLS: Ultimate Mode!",	&cv_aprilfools_ultimatemode,21},

	{IT_HEADER, 			NULL, 	"Game Options", 				NULL, 					    30},
	{IT_STRING | IT_CVAR,	NULL,	"Startup Screen",				&cv_startupscreen,		    36},
	{IT_STRING | IT_CVAR,	NULL,	"Sonic Team Jr Intro",			&cv_stjrintro,		  	    41},

	{IT_STRING | IT_CVAR,	NULL,	"Loading Screen",				&cv_loadingscreen,		    51},
	{IT_STRING | IT_CVAR,	NULL,	"Loading Screen Image",     	&cv_loadingscreenimage,	    56},

	{IT_STRING | IT_CVAR,	NULL,	"Quit Screen",					&cv_quitscreen,	  		   	66},

	{IT_STRING | IT_CVAR,	NULL,	"Is it Called 'Single Player'?",&cv_isitcalledsingleplayer,	76},
	{IT_STRING | IT_CVAR,	NULL,	"Menu Color",					&cv_menucolor,	   		   	81},

	{IT_STRING | IT_CVAR, 	NULL, 	"Show TPS",                 	&cv_tpsrate,         	   	91},
	{IT_STRING | IT_CVAR,	NULL,	"FPS Counter Color",			&cv_fpscountercolor,	   	96},
	{IT_STRING | IT_CVAR,	NULL,	"TPS Counter Color",			&cv_tpscountercolor,	   101},

	{IT_STRING | IT_CVAR,	NULL,	"Shadow Type",					&cv_shadow,	  			   111},
	{IT_STRING | IT_CVAR,	NULL,	"All Objects Have Shadows",		&cv_allobjectshaveshadows, 116},
	{IT_STRING | IT_CVAR,	NULL,	"Shadow Position",				&cv_shadowposition,		   121},

	{IT_STRING | IT_CVAR,	NULL,	"Allow Typical Time Over",		&cv_allowtypicaltimeover,  131},
	{IT_STRING | IT_CVAR,	NULL,	"Pause Graphic Style",			&cv_pausegraphicstyle,	   136},
	{IT_STRING | IT_CVAR,	NULL,	"Automap Outside Devmode",		&cv_automapoutsidedevmode, 141},

	{IT_STRING | IT_CVAR,	NULL,	"Sonic CD Mode",				&cv_soniccd,	   	   	   151},

	{IT_STRING | IT_CVAR,	NULL,	"TSoURDt3rd Update Notice",		&cv_updatenotice,		   161},
	{IT_STRING | IT_CALL,	NULL,	"Check for Updates...",			M_CheckForTSoURDt3rdUpdates,
																							   166},

	{IT_HEADER, 			NULL, 	"Audio Options", 				NULL, 					   175},
	{IT_STRING | IT_CVAR,	NULL,	"Water Muffling",				&cv_watermuffling,	   	   181},

	{IT_STRING | IT_CVAR,	NULL,	"Vape Mode",					&cv_vapemode,	   	   	   191},

	{IT_STRING | IT_CVAR,	NULL,	"Boss Music",					&cv_bossmusic,		  	   201},
	{IT_STRING | IT_CVAR,	NULL,	"Final Boss Music",				&cv_finalbossmusic,		   206},
	{IT_STRING | IT_CVAR,	NULL,	"True Final Boss Music",		&cv_truefinalbossmusic,	   211},

	{IT_STRING | IT_CVAR,	NULL,	"Boss Pinch Music",				&cv_bosspinchmusic,		   221},
	{IT_STRING | IT_CVAR,	NULL,	"Post Boss Music",				&cv_postbossmusic,	  	   226},

	{IT_STRING | IT_CVAR,	NULL,	"Act Clear Music",				&cv_actclearmusic,	  	   236},
	{IT_STRING | IT_CVAR,	NULL,	"Boss Clear Music",				&cv_bossclearmusic,	  	   241},

	{IT_STRING | IT_CVAR,	NULL,	"Game Over Music",				&cv_gameovermusic,		   251},

	{IT_STRING | IT_CVAR,	NULL,	"Default Map Track",			&cv_defaultmaptrack,	   261},

	{IT_HEADER, 			NULL, 	"Player Options", 				NULL, 					   270},
	{IT_STRING | IT_CVAR,	NULL,	"Shield Blocks Transformation",	&cv_shieldblockstransformation,
																						  	   276},
	{IT_STRING | IT_CVAR,	NULL,	"Armageddon Nuke While Super",	&cv_armageddonnukewhilesuper,
																						  	   286},

	{IT_STRING | IT_CVAR,	NULL,	"Always Overlay Invincibility",	&cv_alwaysoverlayinvuln,   291},

	{IT_HEADER, 			NULL, 	"Savedata Options", 			NULL, 					   300},
	{IT_STRING | IT_CVAR, 	NULL, 	"Store Saves in Folders", 		&cv_storesavesinfolders,   306},
	
	{IT_STRING | IT_CVAR, 	NULL, 	"Perfect Save", 				&cv_perfectsave, 		   316},
	{IT_STRING | IT_CVAR, 	NULL, 	"Perfect Save Stripe 1", 		&cv_perfectsavestripe1,	   321},
	{IT_STRING | IT_CVAR, 	NULL, 	"Perfect Save Stripe 2", 		&cv_perfectsavestripe2,    326},
	{IT_STRING | IT_CVAR, 	NULL, 	"Perfect Save Stripe 3", 		&cv_perfectsavestripe3,    331},

	{IT_STRING | IT_CVAR,	NULL,	"Continues",					&cv_continues,		  	   341},

	{IT_HEADER, 			NULL, 	"Server Options", 				NULL,					   350},
	{IT_STRING | IT_CVAR | IT_CV_STRING,	
							NULL,   "Holepunch Server",  			&cv_rendezvousserver,	   356},
	
	{IT_STRING | IT_CVAR,   NULL,   "Show Connecting Players",  	&cv_noticedownload,        370},
	{IT_STRING | IT_CVAR,   NULL,   "Max File Transfer (KB)", 		&cv_maxsend,     	       375},
	{IT_STRING | IT_CVAR,   NULL,   "File Transfer Packet Rate",	&cv_downloadspeed,     	   380},

	{IT_STRING | IT_CVAR,   NULL,   "Player Setup While Moving",	&cv_movingplayersetup,	   390},

	{IT_HEADER, 			NULL, 	"Jukebox Options",     			NULL,					   399},
	{IT_STRING | IT_CALL, 	NULL, 	"Enter Jukebox...",				M_TSoURDt3rdJukebox,   	   405},
	{IT_STRING | IT_CALL,	NULL, 	"Jukebox Controls...",			M_TSoURDt3rdJukeboxControls,
																							   410},

	{IT_STRING | IT_CVAR, 	NULL, 	"Jukebox HUD",					&cv_jukeboxhud,   	       420},

	{IT_STRING | IT_CVAR, 	NULL, 	"Lua Can Stop The Jukebox", 	&cv_luacanstopthejukebox,  430},

	{IT_HEADER, 			NULL, 	"Miscellanious Extras",     	NULL,					   439},
	{IT_STRING | IT_CALL, 	NULL, 	"Play Snake",					STAR_InitializeSnakeMenu,  445},
	{IT_STRING | IT_CALL,	NULL, 	"Dispenser Goin' Up",			STAR_SpawnDispenser,   	   450},

	{IT_STRING | IT_CVAR, 	NULL,   "Window Title Type",    		&cv_windowtitletype,   	   460},
	{IT_STRING | IT_CVAR | IT_CV_STRING,
							NULL,   "Custom Window Title",  		&cv_customwindowtitle,     465},

	{IT_STRING | IT_CVAR, 	NULL,   "Memes on Window Title",    	&cv_memesonwindowtitle,    483},
};

static menuitem_t OP_Tsourdt3rdJukeboxMenu[] =
{
	{IT_KEYHANDLER | IT_STRING, NULL,	"",		M_HandleTSoURDt3rdJukebox,	 0},
};

static menuitem_t OP_Tsourdt3rdJukeboxControlsMenu[] =
{
	{IT_HEADER,				NULL,	"Jukebox Menu",					NULL,						 			0},
	{IT_SPACE,				NULL,	NULL,							NULL,						 			0}, // padding
	{IT_CALL | IT_STRING2,	NULL, 	"Open Jukebox",     			M_ChangeControl,		   JB_OPENJUKEBOX},
	{IT_HEADER,				NULL,	"Jukebox Music",				NULL,						 			0},
	{IT_SPACE,				NULL,	NULL,							NULL,						 			0}, // padding
	{IT_CALL | IT_STRING2,	NULL, 	"Increase Music Speed",     	M_ChangeControl,	JB_INCREASEMUSICSPEED},
	{IT_CALL | IT_STRING2,	NULL, 	"Decrease Music Speed",     	M_ChangeControl,	JB_DECREASEMUSICSPEED},
	{IT_SPACE,				NULL,	NULL,							NULL,						 			0}, // padding
	{IT_CALL | IT_STRING2,	NULL, 	"Play Most Recent Track",   	M_ChangeControl,   JB_PLAYMOSTRECENTTRACK},
	{IT_SPACE,				NULL,	NULL,							NULL,									0}, // padding
	{IT_CALL | IT_STRING2,	NULL, 	"Stop Jukebox",					M_ChangeControl,		   JB_STOPJUKEBOX},
};

static menuitem_t OP_Tsourdt3rdSnakeMenu[] =
{
	{IT_KEYHANDLER | IT_STRING,		NULL,		"",		STAR_HandleSnakeMenu,		0},
};

menu_t OP_TSoURDt3rdOptionsDef = DEFAULTSCROLLMENUSTYLE(
	MTREE2(MN_OP_MAIN, MN_OP_TSOURDT3RD),
	"M_TSOURD", OP_Tsourdt3rdOptionsMenu, &OP_MainDef, 30, 30);

menu_t OP_TSoURDt3rdJukeboxDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_JUKEBOX),
	NULL,
	sizeof (OP_Tsourdt3rdJukeboxMenu)/sizeof (menuitem_t),
	&OP_TSoURDt3rdOptionsDef,
	OP_Tsourdt3rdJukeboxMenu,
	M_DrawTSoURDt3rdJukebox,
	60, 150,
	0,
	NULL
};

menu_t OP_TSoURDt3rdJukeboxControlsDef = CONTROLMENUSTYLE(
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_JUKEBOXCONTROLS),
	OP_Tsourdt3rdJukeboxControlsMenu, &OP_TSoURDt3rdOptionsDef);

menu_t OP_TSoURDt3rdSnakeDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_SNAKE),
	NULL,
	sizeof (OP_Tsourdt3rdSnakeMenu)/sizeof (menuitem_t),
	&OP_TSoURDt3rdOptionsDef,
	OP_Tsourdt3rdSnakeMenu,
	STAR_DrawSnakeMenu,
	60, 150,
	0,
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

// =======
// DRAWING
// =======

//
// void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed)
// Draws a button graphic on the screen. Changes upon being pressed.
//
// Ported from Dr. Robotnik's Ring Racers!
//
void K_drawButton(fixed_t x, fixed_t y, INT32 flags, patch_t *button[2], boolean pressed)
{
	V_DrawFixedPatch(x, y, FRACUNIT, flags, button[(pressed == true) ? 1 : 0], NULL);
}

//
// void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic)
// Draws a button graphic on the screen, and animates it. Changes upon being pressed.
//
// Ported from Dr. Robotnik's Ring Racers!
//
void K_drawButtonAnim(INT32 x, INT32 y, INT32 flags, patch_t *button[2], tic_t animtic)
{
	const UINT8 anim_duration = 16;
	const boolean anim = ((animtic % (anim_duration * 2)) < anim_duration);
	K_drawButton(x << FRACBITS, y << FRACBITS, flags, button, anim);
}

//
// void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall)
// Draws a sticker graphic on the HUD.
//
// Ported from Dr. Robotnik's Ring Racers!
//
void K_DrawSticker(INT32 x, INT32 y, INT32 width, INT32 flags, boolean isSmall)
{
	patch_t *stickerEnd;
	INT32 height;

	if (isSmall == true)
	{
		stickerEnd = (patch_t*)(W_CachePatchName("K_STIKE2", PU_CACHE));
		height = 6;
	}
	else
	{
		stickerEnd = (patch_t*)(W_CachePatchName("K_STIKEN", PU_CACHE));
		height = 11;
	}

	V_DrawFixedPatch(x*FRACUNIT, y*FRACUNIT, FRACUNIT, flags, stickerEnd, NULL);
	V_DrawFill(x, y, width, height, 24|flags);
	V_DrawFixedPatch((x + width)*FRACUNIT, y*FRACUNIT, FRACUNIT, flags|V_FLIP, stickerEnd, NULL);
}

// ====
// GAME
// ====

static void M_CheckForTSoURDt3rdUpdates(INT32 choice)
{
	(void)choice;
	TSoURDt3rdPlayers[consoleplayer].checkedVersion = false;
}

// =======
// SERVERS
// =======

//
// void M_HandleMasterServerResetChoice(INT32 choice)
// Handles resetting the master server address.
//
// Ported from Kart!
//
void M_HandleMasterServerResetChoice(INT32 choice)
{
	if (choice == 'y' || choice == KEY_ENTER)
	{
		CV_Set(&cv_masterserver, cv_masterserver.defaultvalue);
		S_StartSound(NULL, sfx_s221);
	}

	TSoURDt3rdPlayers[consoleplayer].masterServerAddressChanged = true;
}

//
// void M_PreStartServerMenuChoice(INT32 choice)
// Alerts the server menu to reset the master server address.
//
// Ported from Kart!
//
void M_PreStartServerMenuChoice(INT32 choice)
{
	M_HandleMasterServerResetChoice(choice);
	M_StartServerMenu(-1);
}

//
// void M_PreConnectMenuChoice(INT32 choice)
// Alerts the connect menu to reset the master server address.
//
// Ported from Kart!
//
void M_PreConnectMenuChoice(INT32 choice)
{
	M_HandleMasterServerResetChoice(choice);
	M_ConnectMenuModChecks(-1);
}

//
// void M_ShiftMessageQueueDown(void)
// Shifts the message queue down a notch.
//
void M_ShiftMessageQueueDown(void)
{
	// Is the message table after the one on screen empty? //
	if (MessageDef.menuitems[1].text == NULL) // If so, clear the message table, and we're done early :)
	{
		memset(MessageMenu, 0, sizeof(MessageMenu));
		memset(MessageMenuDisplay, 0, sizeof(MessageMenuDisplay));

		return;
	}

	// Shift our message table down //
	for (size_t i = 0, j = 0; i < 256; i++)
	{
		memmove(&MessageMenu[i], &MessageMenu[i+1], sizeof(MessageMenu[i+1]));
		for (j = 0; j < 3; j++)
			memmove(&MessageMenuDisplay[j][i], &MessageMenuDisplay[j][i+1], sizeof(MessageMenuDisplay[j][i+1]));
	}

	// Update to the new message's position, play an alerting sound, and we're done :) //
	MessageDef.x						= MessageMenuDisplay[0][0];
	MessageDef.y						= MessageMenuDisplay[1][0];

	MessageDef.lastOn					= MessageMenuDisplay[2][0];

	S_StartSound(NULL, sfx_zoom);
}

// ======
// I QUIT
// ======

//
// void STAR_M_InitQuitMessages(void)
// Initializes our quit messages.
//
void STAR_M_InitQuitMessages(void)
{
	quitmsg[TSOURDT3RD_QUITSMSG1] = M_GetText("Every time you press 'Y',\nthe TSoURDt3rd Devs cry...\n\n(Press 'Y' to quit)");
	quitmsg[TSOURDT3RD_QUITSMSG2] = M_GetText("Who do you think you are?\nItaly?\n\n(Press 'Y' to quit)");

	quitmsg[TSOURDT3RD_QUITSMSG3] = M_GetText("Hehe, you couldn't even make\nit past the Title Screen,\ncould you, silly?\n\n(Press 'Y' to quit)"); // tmp string
	quitmsg[TSOURDT3RD_QUITSMSG4] = M_GetText("Wait, <insert player name here>!\nCome back! I need you!\n\n(Press 'Y' to quit)"); // tmp string
	quitmsg[TSOURDT3RD_QUITSMSG5] = M_GetText("Come back!\nYou have more jukebox music to play!\n\n(Press 'Y' to quit)"); // tmp string

	quitmsg[TSOURDT3RD_QUITSMSG6] = M_GetText("You know, I have to say\nsomething cool here in order to\nclose the game...\n\n(Press 'Y' to quit)");

	quitmsg[TSOURDT3RD_AF_QUITAMSG1] = M_GetText("Aww, was April Fools\ntoo much for you to handle?\n\n(Press 'Y' to quit)");
	quitmsg[TSOURDT3RD_AF_QUITAMSG2] = M_GetText("Happy April Fools!\n\n(Press 'Y' to quit)");
	quitmsg[TSOURDT3RD_AF_QUITAMSG3] = M_GetText("Wait!\nActivate Ultimate Mode!\n\n(Press 'Y' to quit)");
	quitmsg[TSOURDT3RD_AF_QUITAMSG4] = M_GetText("Could you not deal with\nall the pranks?\n\n(Press 'Y' to quit)");
}

//
// void STAR_M_InitDynamicQuitMessages(void)
// Initializes our dynamic quit messages.
//
void STAR_M_InitDynamicQuitMessages(void)
{
	char *maptitle = G_BuildMapTitle(gamemap);

	if (Playing() && gamestate == GS_LEVEL)
		quitmsg[TSOURDT3RD_QUITSMSG3] = va(M_GetText("Hehe, was \n%s\ntoo hard for you?\n\n(Press 'Y' to quit)"), maptitle);
	else
		quitmsg[TSOURDT3RD_QUITSMSG3] = M_GetText("Hehe, you couldn't even make\nit past the Title Screen,\ncould you, silly?\n\n(Press 'Y' to quit)");

	quitmsg[TSOURDT3RD_QUITSMSG4] = va(M_GetText("Wait, %s!\nCome back! I need you!\n\n(Press 'Y' to quit)"), TSoURDt3rd_ReturnUsername());

	if (TSoURDt3rdPlayers[consoleplayer].jukebox.curtrack)
		quitmsg[TSOURDT3RD_QUITSMSG5] = va(M_GetText("Come back!\nFinish listening to\n%s!\n\n(Press 'Y' to quit)"), TSoURDt3rdPlayers[consoleplayer].jukebox.curtrack->title);
	else
		quitmsg[TSOURDT3RD_QUITSMSG5] = M_GetText("Come back!\nYou have more jukebox music to play!\n\n(Press 'Y' to quit)");

	Z_Free(maptitle);
}

//
// INT32 STAR_M_SelectQuitMessage(void)
// Selects a quit message for us.
//
INT32 STAR_M_SelectQuitMessage(void)
{
	INT32 randomMessage = M_RandomKey(NUM_QUITMESSAGES); // Assign a quit message //
	STAR_M_InitDynamicQuitMessages();

	// No April Fools messages when it's not April Fools! //
	if (!TSoURDt3rd_InAprilFoolsMode())
	{
		while (randomMessage >= TSOURDT3RD_AF_QUITAMSG1 && randomMessage <= TSOURDT3RD_AF_QUITAMSG4)
			randomMessage = M_RandomKey(NUM_QUITMESSAGES);
	}

	// Choose a quit sound //
	switch (randomMessage)
	{
		case QUITMSG4: S_StartSound(NULL, sfx_adderr); break;
		case QUITMSG5: S_StartSound(NULL, sfx_cgot); break;

		case QUIT2MSG1:
		case QUIT2MSG2: S_StartSound(NULL, sfx_pop); break;

		case QUIT2MSG3:
		{
			switch(M_RandomRange(0, 1))
			{
				case 1: S_StartSound(NULL, sfx_supert); break;
				default: S_StartSound(NULL, sfx_cgot); break;
			}
			break;
		}

		case QUIT2MSG4: S_StartSound(NULL, sfx_spin); break;

		case QUIT2MSG5: S_StartSound(NULL, sfx_cdpcm0+M_RandomKey(9)); break;

		case QUIT3MSG2: S_StartSound(NULL, sfx_supert); break;
		case QUIT3MSG3: S_StartSound(NULL, sfx_s3k95); break;
	}

	return randomMessage; // Return our random message and we're done :) //
}

//
// void STAR_M_DrawQuitGraphic(void)
// Draws a quit graphic for us.
//
void STAR_M_DrawQuitGraphic(void)
{
	const char *quitgfx;

	switch (cv_quitscreen.value)
	{
		case 1: quitgfx = "SS_QCOLR"; break; // aseprite moment
		case 2: quitgfx = "SS_QSMUG"; break; // funny aseprite moment
		case 3: quitgfx = "SS_QKEL"; break; // kel world aseprite moment
		case 4: quitgfx = "SS_QATRB"; break; // secret aseprite moment

		default: // Demo 3 Quit Screen Tails 06-16-2001
			V_DrawScaledPatch(0, 0, 0, W_CachePatchName("GAMEQUIT", PU_PATCH));
			return;
	}

	V_DrawScaledPatch(0, 0, 0, W_CachePatchName(quitgfx, PU_PATCH));
	V_DrawScaledPatch(0, 0, 0, W_CachePatchName("SS_QDISC", PU_PATCH)); // psst, disclaimer; this game should not be sold :p
}

// ================
// TSOURDT3RD MENUS
// ================

#if 0
static void M_TSoURDt3rdOptions(INT32 choice)
{
	(void)choice;

	// Event Options //
	// Main Option Header
	OP_Tsourdt3rdOptionsMenu[op_eventoptionsheader].status =
		((eastermode || aprilfoolsmode || xmasmode) ? IT_HEADER : IT_HEADER|IT_GRAYEDOUT);

	// Easter
	M_UpdateEasterStuff();

	// April Fools
	OP_Tsourdt3rdOptionsMenu[op_aprilfools].status =
		(aprilfoolsmode ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	// Game Options //
	STAR_LoadingScreen_OnChange();

	OP_Tsourdt3rdOptionsMenu[op_fpscountercolor].status =
		(cv_ticrate.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	STAR_TPSRate_OnChange();

	STAR_Shadow_OnChange();

	OP_Tsourdt3rdOptionsMenu[op_allowtypicaltimeover].status =
		(!netgame ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	STAR_UpdateNotice_OnChange();

	// Audio Options //
	OP_Tsourdt3rdOptionsMenu[op_defaultmaptrack].status =
		(!(Playing() && playeringame[consoleplayer]) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	// Player Options //
	OP_Tsourdt3rdOptionsMenu[op_shieldblockstransformation].status =
		(!netgame ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
	
	OP_Tsourdt3rdOptionsMenu[op_alwaysoverlayinvuln].status =
		((players[consoleplayer].powers[pw_invulnerability] && (players[consoleplayer].powers[pw_shield] & SH_NOSTACK) != SH_NONE) ? IT_GRAYEDOUT : IT_CVAR|IT_STRING);

	// Savegame Options //
	OP_Tsourdt3rdOptionsMenu[op_storesavesinfolders].status =
		(!netgame ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	STAR_PerfectSave_OnChange();

	OP_Tsourdt3rdOptionsMenu[op_continues].status =
		(!(Playing() && playeringame[consoleplayer]) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	// Extra Options //
	// Snake
	OP_Tsourdt3rdOptionsMenu[op_snake].status =
		(!Playing() ? IT_CALL|IT_STRING : IT_GRAYEDOUT);

	// TF2
	OP_Tsourdt3rdOptionsMenu[op_dispensergoingup].status =
		(!netgame ? IT_CALL|IT_STRING : IT_GRAYEDOUT);

#ifdef HAVE_SDL
	STAR_WindowTitleVars_OnChange();
#else
	for (INT32 i = op_windowtitletype; i < op_memesonwindowtitle; i++)
		OP_Tsourdt3rdOptionsMenu[i].status = IT_GRAYEDOUT;	
#endif

	M_SetupNextMenu(&OP_TSoURDt3rdOptionsDef);
}
#endif

// =============
// JUKEBOX MENUS
// =============

//
// boolean TSoURDt3rd_M_IsJukeboxUnlocked(TSoURDt3rdJukebox_t *TSoURDt3rdJukebox)
// Checks if TSoURDt3rd's Jukebox has been unlocked.
//
boolean TSoURDt3rd_M_IsJukeboxUnlocked(TSoURDt3rdJukebox_t *TSoURDt3rdJukebox)
{
	if (TSoURDt3rdJukebox->Unlocked)
		return true;

	for (INT32 i = 0; i < MAXUNLOCKABLES; i++)
	{
		if ((unlockables[i].type == SECRET_SOUNDTEST) || (modifiedgame && !savemoddata) || autoloaded)
		{
			TSoURDt3rdJukebox->Unlocked = true;
			return true;
		}
	}

	M_StartMessage(M_GetText("You haven't unlocked the jukebox yet!\nGo and unlock the sound test first!\n"),NULL,MM_NOTHING);
	return false;
}

//
// void M_TSoURDt3rdJukebox(INT32 choice)
// Sets up the main Jukebox menu.
//
void M_TSoURDt3rdJukebox(INT32 choice)
{
	INT32 ul = skyRoomMenuTranslations[choice-1];
	TSoURDt3rdJukebox_t *TSoURDt3rdJukebox = &TSoURDt3rdPlayers[consoleplayer].jukebox;

	soundtestpage = (UINT8)(unlockables[ul].variable);
	if (!soundtestpage)
		soundtestpage = 1;

	if (TSoURDt3rdJukebox->curtrack)
		curplaying = TSoURDt3rdJukebox->curtrack;
	else
		M_ResetJukebox(true);

	if (!TSoURDt3rd_M_IsJukeboxUnlocked(TSoURDt3rdJukebox))
		return;
	else if (!S_PrepareSoundTest())
	{
		M_StartMessage(M_GetText("No accessible tracks found in the jukebox.\n"),NULL,MM_NOTHING);
		return;
	}

	M_CacheSoundTest();

	st_time = 0;

	st_sel = 0;

	st_cc = cv_closedcaptioning.value; // hack;
	cv_closedcaptioning.value = 1; // hack

	M_SetupNextMenu(&OP_TSoURDt3rdJukeboxDef);
}

//
// static void M_DrawTSoURDt3rdJukebox(void)
// Draws the main Jukebox menu.
//
static void M_DrawTSoURDt3rdJukebox(void)
{
	INT32 x, y, i;
	fixed_t hscale = FRACUNIT/2, vscale = FRACUNIT/2, bounce = 0;
	UINT8 frame[4] = {0, 0, -1, SKINCOLOR_RUBY};

	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];

	// let's handle the ticker first.
	// STAR NOTE: there's a duplicate of the latter, non-sfx part of this ticker in d_main.c, where the D_SRB2Loop function is, just so you know :p
	if (TSoURDt3rd->jukebox.curtrack)
	{
		if (curplaying == &soundtestsfx)
		{
			if (cv_soundtest.value)
			{
				frame[1] = (2 - (st_time >> FRACBITS));
				frame[2] = ((cv_soundtest.value - 1) % 9);
				frame[3] += (((cv_soundtest.value - 1) / 9) % (FIRSTSUPERCOLOR - frame[3]));
				if (st_time < (2 << FRACBITS))
					st_time += renderdeltatics;
				if (st_time >= (2 << FRACBITS))
					st_time = 2 << FRACBITS;
			}
		}
		else
		{
			fixed_t stoppingtics = (fixed_t)(curplaying->stoppingtics) << FRACBITS;
			if (stoppingtics && st_time >= stoppingtics)
			{
				M_ResetJukebox(true); // Whoa, Whoa, We Ran Out of Time
				st_time = 0;
			}
			else
			{
				fixed_t work, bpm;
				work = bpm = curplaying->bpm/S_GetSpeedMusic();

				angle_t ang;
				//bpm = FixedDiv((60*TICRATE)<<FRACBITS, bpm); -- bake this in on load

				work = st_time;
				work %= bpm;

				if (st_time >= (FRACUNIT << (FRACBITS - 2))) // prevent overflow jump - takes about 15 minutes of loop on the same song to reach
					st_time = work;

				work = FixedDiv(work*180, bpm);
				frame[0] = 8-(work/(20<<FRACBITS));
				if (frame[0] > 8) // VERY small likelihood for the above calculation to wrap, but it turns out it IS possible lmao
					frame[0] = 0;
				ang = (FixedAngle(work)>>ANGLETOFINESHIFT) & FINEMASK;
				bounce = (FINESINE(ang) - FRACUNIT/2);
				hscale -= bounce/16;
				vscale += bounce/16;

				if (!(paused || P_AutoPause())) // prevents time from being added up while the game is paused
					st_time += renderdeltatics*S_GetSpeedMusic();
			}
		}
	}

	x = 90<<FRACBITS;
	y = (BASEVIDHEIGHT-32)<<FRACBITS;

	V_DrawStretchyFixedPatch(x, y,
		hscale, vscale,
		0, st_radio[frame[0]], NULL);

	V_DrawFixedPatch(x, y, FRACUNIT/2, 0, st_launchpad[0], NULL);

	for (i = 0; i < 9; i++)
	{
		if (i == frame[2])
		{
			UINT8 *colmap = R_GetTranslationColormap(TC_RAINBOW, frame[3], GTC_CACHE);
			V_DrawFixedPatch(x, y + (frame[1]<<FRACBITS), FRACUNIT/2, 0, st_launchpad[frame[1]+1], colmap);
		}
		else
			V_DrawFixedPatch(x, y, FRACUNIT/2, 0, st_launchpad[1], NULL);

		if ((i % 3) == 2)
		{
			x -= ((2*28) + 25)<<(FRACBITS-1);
			y -= ((2*7) - 11)<<(FRACBITS-1);
		}
		else
		{
			x += 28<<(FRACBITS-1);
			y += 7<<(FRACBITS-1);
		}
	}

	y = (BASEVIDWIDTH-(vid.width/vid.dupx))/2;

	V_DrawFill(y, 20, (vid.width/vid.dupx)+12, 24, 159);
	{
		static fixed_t st_scroll = -FRACUNIT;
		const char* titl;
		x = 16;
		V_DrawString(x, 10, 0, "NOW PLAYING:");

		if (TSoURDt3rd->jukebox.curtrack)
		{
			if (curplaying->alttitle[0])
				titl = va("%s - %s - ", TSoURDt3rd->jukebox.curtrack->title, curplaying->alttitle);
			else
				titl = va("%s - ", TSoURDt3rd->jukebox.curtrack->title);
		}
		else
			titl = "None - ";

		i = V_LevelNameWidth(titl);

		st_scroll += renderdeltatics;

		while (st_scroll >= (i << FRACBITS))
			st_scroll -= i << FRACBITS;

		x -= st_scroll >> FRACBITS;

		while (x < BASEVIDWIDTH-y)
			x += i;
		while (x > y)
		{
			x -= i;
			V_DrawLevelTitle(x, 22, 0, titl);
		}

		if (TSoURDt3rd->jukebox.curtrack)
			V_DrawRightAlignedThinString(BASEVIDWIDTH-16, 46, V_ALLOWLOWERCASE, curplaying->authors);
	}

	V_DrawFill(165, 60, 140+15, 112, 159);

	{
		INT32 t, b, q, m = 112;

		if (numsoundtestdefs <= 7)
		{
			t = 0;
			b = numsoundtestdefs - 1;
			i = 0;
		}
		else
		{
			q = m;
			m = (5*m)/numsoundtestdefs;
			if (st_sel < 3)
			{
				t = 0;
				b = 6;
				i = 0;
			}
			else if (st_sel >= numsoundtestdefs-4)
			{
				t = numsoundtestdefs - 7;
				b = numsoundtestdefs - 1;
				i = q-m;
			}
			else
			{
				t = st_sel - 3;
				b = st_sel + 3;
				i = (t * (q-m))/(numsoundtestdefs - 7);
			}
		}

		if (t != 0)
			V_DrawString(165+140+4, 60+4 - (skullAnimCounter/5), V_MENUCOLORMAP, "\x1A");

		if (b != numsoundtestdefs - 1)
			V_DrawString(165+140+4, 60+112-12 + (skullAnimCounter/5), V_MENUCOLORMAP, "\x1B");

		x = 169;
		y = 64;

		while (t <= b)
		{
			if (t == st_sel)
				V_DrawFill(165, y-4, 140-1+16, 16, 155);
			if (!soundtestdefs[t]->allowed)
			{
				V_DrawString(x, y, (t == st_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, "???");
			}
			else if (soundtestdefs[t] == &soundtestsfx)
			{
				const char *sfxstr = va("SFX %s", cv_soundtest.string);
				V_DrawString(x, y, (t == st_sel ? V_MENUCOLORMAP : 0), sfxstr);
				if (t == st_sel)
				{
					V_DrawCharacter(x - 10 - (skullAnimCounter/5), y,
						'\x1C' | V_MENUCOLORMAP, false);
					V_DrawCharacter(x + 2 + V_StringWidth(sfxstr, 0) + (skullAnimCounter/5), y,
						'\x1D' | V_MENUCOLORMAP, false);
				}

				if (curplaying == soundtestdefs[t])
				{
					sfxstr = (cv_soundtest.value) ? S_sfx[cv_soundtest.value].name : "N/A";
					i = V_StringWidth(sfxstr, 0);
					V_DrawFill(165+140-9-i+16, y-4, i+8, 16, 150);
					V_DrawRightAlignedString(165+140-5+16, y, V_MENUCOLORMAP, sfxstr);
				}
			}
			else
			{
				if (strlen(soundtestdefs[t]->title) < 18)
					V_DrawString(x, y, (t == st_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, soundtestdefs[t]->title);
				else
					V_DrawThinString(x, y, (t == st_sel ? V_MENUCOLORMAP : 0)|V_ALLOWLOWERCASE, soundtestdefs[t]->title);

				if (curplaying == soundtestdefs[t])
				{
					V_DrawFill(165+140-9+24, y-4, 8, 16, 150);
					
					//V_DrawCharacter(165+140-8+24, y, '\x19' | V_MENUCOLORMAP, false);
					V_DrawFixedPatch((165+140-9+24)<<FRACBITS, (y<<FRACBITS)-(bounce*4), FRACUNIT, 0, hu_font['\x19'-HU_FONTSTART], V_GetStringColormap(V_MENUCOLORMAP));
				}
			}
			t++;
			y += 16;
		}

		V_DrawFill(165+140-1+15, 60 + i, 1, m, 0); // White Scroll Bar

		V_DrawString(((BASEVIDWIDTH/2)+15), ((BASEVIDWIDTH/2)+15),
			(V_SNAPTORIGHT|((soundtestdefs[st_sel] == &soundtestsfx) ? V_TRANSLUCENT : V_MENUCOLORMAP)),
			(atof(cv_jukeboxspeed.string) < 10.0f ?
				(va("Music Speed     %.3s", cv_jukeboxspeed.string)) :
				(va("Music Speed     %.4s", cv_jukeboxspeed.string))));

		V_DrawCharacter(((BASEVIDWIDTH/2)+107), ((BASEVIDWIDTH/2)+15),
			'\x1C' | V_SNAPTORIGHT | (((soundtestdefs[st_sel] == &soundtestsfx) || atof(cv_jukeboxspeed.string) < 0.1f) ? V_TRANSLUCENT : V_MENUCOLORMAP),
			false); // Left Arrow
		V_DrawCharacter(((BASEVIDWIDTH/2)+(atof(cv_jukeboxspeed.string) < 10.0f ? 145 : 152)),
			((BASEVIDWIDTH/2)+15), '\x1D' | V_SNAPTORIGHT | (((soundtestdefs[st_sel] == &soundtestsfx) || atof(cv_jukeboxspeed.string) >= 20.0f) ? V_TRANSLUCENT : V_MENUCOLORMAP),
			false);	// Right Arrow
	}
}

//
// static void M_HandleTSoURDt3rdJukebox(INT32 choice)
// Controller system for the main Jukebox menu.
//
static void M_HandleTSoURDt3rdJukebox(INT32 choice)
{
	boolean exitmenu = true;
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];

	switch (choice)
	{
		case KEY_DOWNARROW:
			if (st_sel++ >= numsoundtestdefs-1)
				st_sel = 0;
			{
				cv_closedcaptioning.value = st_cc; // hack
				S_StartSound(NULL, sfx_menu1);
				cv_closedcaptioning.value = 1; // hack
			}
			break;
		case KEY_UPARROW:
			if (!st_sel--)
				st_sel = numsoundtestdefs-1;
			{
				cv_closedcaptioning.value = st_cc; // hack
				S_StartSound(NULL, sfx_menu1);
				cv_closedcaptioning.value = 1; // hack
			}
			break;
		case KEY_PGDN:
			if (st_sel < numsoundtestdefs-1)
			{
				st_sel += 3;
				if (st_sel >= numsoundtestdefs-1)
					st_sel = numsoundtestdefs-1;
				cv_closedcaptioning.value = st_cc; // hack
				S_StartSound(NULL, sfx_menu1);
				cv_closedcaptioning.value = 1; // hack
			}
			break;
		case KEY_PGUP:
			if (st_sel)
			{
				st_sel -= 3;
				if (st_sel < 0)
					st_sel = 0;
				cv_closedcaptioning.value = st_cc; // hack
				S_StartSound(NULL, sfx_menu1);
				cv_closedcaptioning.value = 1; // hack
			}
			break;
		case KEY_BACKSPACE:
			if (!TSoURDt3rd->jukebox.curtrack)
			{
				S_StartSound(NULL, sfx_lose);
				break;
			}

			S_StopSounds();
			S_StopMusic();
			M_ResetJukebox(true);
			st_time = 0;

			S_StartSound(NULL, sfx_skid);

			cv_closedcaptioning.value = st_cc; // hack
			cv_closedcaptioning.value = 1; // hack
			break;
		case KEY_ESCAPE:
			exitmenu = false;
			break;

		case KEY_RIGHTARROW:
			if (soundtestdefs[st_sel]->allowed && soundtestdefs[st_sel] == &soundtestsfx)
			{
				S_StopSounds();
				S_StopMusic();
				curplaying = soundtestdefs[st_sel];
				st_time = 0;
				CV_AddValue(&cv_soundtest, 1);
			}
			else
			{
				if (atof(cv_jukeboxspeed.string) >= 20.0f)
					break;
				S_StartSound(NULL, sfx_menu1);
				CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)+(0.1f)));
			}
			break;
		case KEY_LEFTARROW:
			if (soundtestdefs[st_sel]->allowed && soundtestdefs[st_sel] == &soundtestsfx)
			{
				S_StopSounds();
				S_StopMusic();
				curplaying = soundtestdefs[st_sel];
				st_time = 0;
				CV_AddValue(&cv_soundtest, -1);
			}
			else
			{
				if (atof(cv_jukeboxspeed.string) < 0.1f)
					break;
				S_StartSound(NULL, sfx_menu1);
				CV_Set(&cv_jukeboxspeed, va("%f", atof(cv_jukeboxspeed.string)-(0.1f)));
			}
			break;
		case KEY_ENTER:
			S_StopSounds();
			S_StopMusic();
			st_time = 0;
			if (soundtestdefs[st_sel]->allowed)
			{
				if (TSoURDt3rd->jukebox.curtrack)
					TSoURDt3rd->jukebox.prevtrack = TSoURDt3rd->jukebox.curtrack;
				curplaying = soundtestdefs[st_sel];

				if (curplaying == &soundtestsfx)
				{
					// S_StopMusic() -- is this necessary?
					if (cv_soundtest.value)
						S_StartSound(NULL, cv_soundtest.value);
				}
				else
				{
					if (TSoURDt3rd_InAprilFoolsMode())
					{
						strcpy(curplaying->title, "Get rickrolled lol");
						strcpy(curplaying->name, "_hehe");
					}

					S_ChangeMusicInternal(curplaying->name, !curplaying->stoppingtics);
					STAR_CONS_Printf(STAR_CONS_JUKEBOX, M_GetText("Loaded track \x82%s\x80.\n"), curplaying->title);

					TSoURDt3rd->jukebox.curtrack = curplaying;
					TSoURDt3rd->jukebox.initHUD	= true;
					TSoURDt3rd_ControlMusicEffects();
				}
			}
			else
			{
				curplaying = NULL;
				S_StartSound(NULL, sfx_lose);
			}
			break;

		default:
			break;
	}

	if (!exitmenu)
	{
		Z_Free(soundtestdefs);
		soundtestdefs = NULL;

		if (!TSoURDt3rd->jukebox.curtrack)
			M_ResetJukebox(Playing());

		cv_closedcaptioning.value = st_cc; // undo hack

		if (currentMenu->prevMenu)
			M_SetupNextMenu(currentMenu->prevMenu);
		else
			M_ClearMenus(true);
	}
}

//
// static void M_TSoURDt3rdJukeboxControls(INT32 choice)
// Initializes the Jukebox control menu.
//
static void M_TSoURDt3rdJukeboxControls(INT32 choice)
{
	(void)choice;
	setupcontrols = gamecontrol; // necessary in order to set controls, crashes otherwise

	M_SetupNextMenu(&OP_TSoURDt3rdJukeboxControlsDef);
}

// =====
// SNAKE
// =====

static void STAR_InitializeSnakeMenu(INT32 choice)
{
	(void)choice;

	if (snake)
	{
		free(snake);
		snake = NULL;
	}
	Snake_Initialise();
	M_SetupNextMenu(&OP_TSoURDt3rdSnakeDef);
}

static void STAR_DrawSnakeMenu(void)
{
	if (snake)
		Snake_Draw();
	V_DrawRightAlignedString(BASEVIDWIDTH-4, BASEVIDHEIGHT-12, V_ALLOWLOWERCASE, "\x86""Press ""\x82""ESC""\x86"" to quit.");
}

static void STAR_HandleSnakeMenu(INT32 choice)
{
	(void)choice;
	tic_t nowtime, quittime, lasttime;

	if (snake)
	{
		// Remove Uncapped While They're Here //
		nowtime = lasttime = I_GetTime();
		quittime = nowtime + NEWTICRATE*2;

		while (quittime > nowtime)
		{
			// Delay the Screen
			while (!((nowtime = I_GetTime()) - lasttime))
			{
				I_Sleep(cv_sleep.value);
				I_UpdateTime(cv_timescale.value);
			}
			lasttime = nowtime;

			I_OsPolling();
			I_UpdateNoBlit();

			// do hacks
			for (; eventtail != eventhead; eventtail = (eventtail+1) & (MAXEVENTS-1))
			{
				if (!Snake_Joy_Grabber(&events[eventtail]))
					G_MapEventsToControls(&events[eventtail]);
			}

			// Check Controls While Removing Uncapped
			if (gamekeydown[KEY_ESCAPE] || gamekeydown[KEY_JOY1+1])
			{
				// Free the Memory
				if (snake)
				{
					free(snake);
					snake = NULL;
				}

				// Close the Menu
				D_StartTitle();
				memset(gamekeydown, 0, NUMKEYS);
				return;
			}	

			if (snake)
				Snake_Handle();

			// Delay the Screen Even More
#ifdef HAVE_THREADS
			I_lock_mutex(&m_menu_mutex);
#endif
			M_Drawer(); // menu is drawn even on top of wipes
#ifdef HAVE_THREADS
			I_unlock_mutex(m_menu_mutex);
#endif
			// Update the Screen Again
			I_UpdateNoVsync(); // page flip or blit buffer
			I_FinishUpdate(); // Update the screen with the image Tails 06-19-2001

			// Fix Some Small Things
			if (moviemode) // make sure we save frames for the white hold too
				M_SaveFrame();
			S_UpdateSounds();
			S_UpdateClosedCaptions();
		}
	}
	
	// Update the Screen AGAIN, and We're Done :)
	I_UpdateNoVsync();
}

// =====
// MISC.
// =====

//
// static void STAR_SpawnDispenser(INT32 choice)
// Spawns a TF2 Dispenser.
//
// Part of TSoURDt3rd's custom objects!
//
static void STAR_SpawnDispenser(INT32 choice)
{
	(void)choice;

	if (!(Playing() && playeringame[consoleplayer]) || netgame)
	{
		(!(Playing() && playeringame[consoleplayer]) ?
			(CONS_Printf("You need to be in a game in order to spawn this.\n")) :

			((Playing() && netgame) ? CONS_Printf("Sorry, you can't spawn this while in a netgame.\n") : 0));
	}
	else
		SpawnTheDispenser = true;
}
