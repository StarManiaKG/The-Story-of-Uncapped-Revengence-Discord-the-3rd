// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Original Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Original Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/transient/smkg-options-tsourdt3rd.c
/// \brief TSoURDt3rd menu options

#include "../smkg_m_draw.h"
#include "../../smkg-cvars.h"
#include "../../m_menu.h"

#include "../../drrr/k_menu.h"

#include "../../../g_game.h" // playeringame //
#include "../../../r_main.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void G_CheckForTSoURDt3rdUpdates(INT32 choice);

static void OBJ_SpawnTF2Dispenser(INT32 choice);

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
	{IT_STRING | IT_CALL,	NULL,	"Check for Updates...",			G_CheckForTSoURDt3rdUpdates, 166},

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
	{IT_STRING | IT_CALL, 	NULL, 	"Enter Jukebox...",				TSoURDt3rd_Jukebox_InitMenu,405},
	{IT_STRING | IT_CALL,	NULL, 	"Jukebox Controls...",			TSoURDt3rd_Jukebox_InitControlsMenu,
																							   410},

	{IT_STRING | IT_CVAR, 	NULL, 	"Jukebox HUD",					&cv_jukeboxhud,   	       420},

	{IT_STRING | IT_CVAR, 	NULL, 	"Lua Can Stop The Jukebox", 	&cv_luacanstopthejukebox,  430},

	{IT_HEADER, 			NULL, 	"Miscellanious Extras",     	NULL,					   439},
	{IT_STRING | IT_CALL, 	NULL, 	"Play Snake",					TSoURDt3rd_Snake_InitMenu, 445},
	{IT_STRING | IT_CALL,	NULL, 	"Dispenser Goin' Up",			OBJ_SpawnTF2Dispenser,     450},

	{IT_STRING | IT_CVAR, 	NULL,   "Window Title Type",    		&cv_windowtitletype,   	   460},
	{IT_STRING | IT_CVAR | IT_CV_STRING,
							NULL,   "Custom Window Title",  		&cv_customwindowtitle,     465},

	{IT_STRING | IT_CVAR, 	NULL,   "Memes on Window Title",    	&cv_memesonwindowtitle,    483},
};

menu_t OP_TSoURDt3rdOptionsDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD_JUKEBOX),
	NULL,
	sizeof (OP_TSoURDt3rdOptionsDef)/sizeof (menuitem_t),
	&OP_MainDef,
	OP_Tsourdt3rdOptionsMenu,
	M_DrawTSoURDt3rdOptions,
	60, 150,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_OP_TSoURDt3rdOptionsDef = {
	&OP_TSoURDt3rdOptionsDef,
	NULL,
	0,
	0, 0, //1, 5,
	M_DrawTSoURDt3rdOptions,
	//M_DrawOptionsCogs,
	M_OptionsTick,
	NULL,
	NULL,
	M_OptionsInputs,
	NULL,
};

// ------------------------ //
//        Functions
// ------------------------ //

static void G_CheckForTSoURDt3rdUpdates(INT32 choice)
{
	(void)choice;
	TSoURDt3rdPlayers[consoleplayer].checkedVersion = false;
}

static void OBJ_SpawnTF2Dispenser(INT32 choice)
{
	(void)choice;

	if (!Playing() || !playeringame[consoleplayer])
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "You need to be in a game in order to spawn this.\n");
		return;
	}
	else if (netgame)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "Sorry, you can't spawn this while in a netgame.\n");
		return;
	}

	SpawnTheDispenser = true;
}
