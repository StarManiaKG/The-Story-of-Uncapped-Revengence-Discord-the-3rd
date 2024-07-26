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
#include "../smkg_m_func.h"
#include "../../smkg-cvars.h"
#include "../../m_menu.h"

#include "../../drrr/k_menu.h"
#include "../../drrr/kv_video.h"

#include "../../../g_game.h" // playeringame //
#include "../../../i_time.h"
#include "../../../m_easing.h"
#include "../../../r_draw.h"
#include "../../../r_main.h"
#include "../../../v_video.h"
#include "../../../w_wad.h"
#include "../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

struct optionsmenu_s optionsmenu;

static void TSoURDt3rd_M_DrawOptions(void);
static void TSoURDt3rd_M_Tick(void);
static boolean TSoURDt3rd_M_OptionsInputs(INT32 ch);
static boolean TSoURDt3rd_M_OptionsQuit(void);

static void G_CheckForTSoURDt3rdUpdates(INT32 choice);

static void OBJ_SpawnTF2Dispenser(INT32 choice);

menuitem_t OP_Tsourdt3rdOptionsMenu[] =
{
	{IT_HEADER, 			NULL, 	"Event Options", 				NULL, 					  	 0},
	{IT_STRING | IT_CVAR, 	NULL,   "EASTER: Allow Egg Hunt",   	&cv_easter_allowegghunt,   	 6},
	{IT_STRING | IT_CVAR, 	NULL,   "EASTER: Egg Hunt Bonuses", 	&cv_easter_egghuntbonuses, 	11},

	{IT_STRING | IT_CVAR,	NULL,	"APRIL FOOLS: Ultimate Mode!",	&cv_tsourdt3rd_aprilfools_ultimatemode,21},

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
	MTREE2(MN_OP_MAIN, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (OP_Tsourdt3rdOptionsMenu)/sizeof (menuitem_t),
	&OP_MainDef,
	OP_Tsourdt3rdOptionsMenu,
	NULL,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_OP_TSoURDt3rdOptionsDef = {
	&OP_TSoURDt3rdOptionsDef,
	NULL,
	SKINCOLOR_SLATE, 0,
	0,
	2, 5,
	TSoURDt3rd_M_DrawOptions,
	TSoURDt3rd_M_Tick,
	NULL,
	NULL,
	TSoURDt3rd_M_OptionsInputs,
	NULL,
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// void M_TSoURDt3rdOptions(INT32 choice)
// Initializes main TSoURDt3rd menu options.
//
static void M_ResetOptions(void)
{
	optionsmenu.ticker = 0;
	optionsmenu.offset.start = 0;

	optionsmenu.optx = 0;
	optionsmenu.opty = 0;
	optionsmenu.toptx = 0;
	optionsmenu.topty = 0;

	// BG setup:
	optionsmenu.currcolour = TSoURDt3rd_OP_TSoURDt3rdOptionsDef.extra1;
	optionsmenu.lastcolour = 0;
	optionsmenu.fade = 0;

#if 0
	// For profiles:
	memset(setup_player, 0, sizeof(setup_player));
	optionsmenu.profile = NULL;
#endif
}

void M_TSoURDt3rdOptions(INT32 choice)
{
	(void)choice;

	M_ResetOptions();

	// So that pause doesn't go to the main menu...
	OP_TSoURDt3rdOptionsDef.prevMenu = currentMenu;

	M_SetupNextMenu(&OP_TSoURDt3rdOptionsDef);
}

//
// static void TSoURDt3rd_M_DrawOptions(INT32 choice)
// Drawing routine for the main TSoURDt3rd options menu.
//
static void M_DrawOptionsMovingButton(void)
{
	patch_t *butt = W_CachePatchName("OPT_BUTT", PU_CACHE);
	UINT8 *c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
	fixed_t t = M_DueFrac(optionsmenu.topt_start, M_OPTIONS_OFSTIME);
	fixed_t z = Easing_OutSine(M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t tx = Easing_OutQuad(t, optionsmenu.optx * FRACUNIT, optionsmenu.toptx * FRACUNIT) + z;
	fixed_t ty = Easing_OutQuad(t, optionsmenu.opty * FRACUNIT, optionsmenu.topty * FRACUNIT) + z;

	V_DrawFixedPatch(tx, ty, FRACUNIT, 0, butt, c);

	const char *s = OP_TSoURDt3rdOptionsDef.menuitems[OP_TSoURDt3rdOptionsDef.lastOn].text;
	fixed_t w = DRRR_V_StringScaledWidth(
		FRACUNIT,
		FRACUNIT,
		FRACUNIT,
		V_ALLOWLOWERCASE,
		LT_FONT,
		s
	);
	DRRR_V_DrawStringScaled(
		tx - 3*FRACUNIT - (w/2),
		ty - 16*FRACUNIT,
		FRACUNIT,
		FRACUNIT,
		FRACUNIT,
		V_ALLOWLOWERCASE,
		c,
		LT_FONT,
		s
	);
}

static void TSoURDt3rd_M_DrawOptions(void)
{
	UINT8 i;
	fixed_t t = Easing_OutSine(M_DueFrac(optionsmenu.offset.start, M_OPTIONS_OFSTIME), optionsmenu.offset.dist * FRACUNIT, 0);
	fixed_t x = (140 - (48*tsourdt3rd_itemOn))*FRACUNIT + t;
	fixed_t y = 70*FRACUNIT + t;
	fixed_t tx = M_EaseWithTransition(Easing_InQuart, 5 * 64 * FRACUNIT);
	patch_t *buttback = W_CachePatchName("OPT_BUTT", PU_CACHE);

	UINT8 *c = NULL;

	for (i=0; i < currentMenu->numitems; i++)
	{
		fixed_t py = y - (tsourdt3rd_itemOn*48)*FRACUNIT;
		fixed_t px = x - tx;
		INT32 tflag = 0;

		if (i == tsourdt3rd_itemOn)
			c = R_GetTranslationColormap(TC_RAINBOW, SKINCOLOR_EMERALD, GTC_CACHE);
		else
			c = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_BLACK, GTC_CACHE);

		if (currentMenu->menuitems[i].status & IT_TRANSTEXT)
			tflag |= V_TRANSLUCENT;

		if (!(menutransition.tics != menutransition.dest && i == tsourdt3rd_itemOn))
		{
			V_DrawFixedPatch(px, py, FRACUNIT, 0, buttback, c);

			const char *s = currentMenu->menuitems[i].text;
			fixed_t w = DRRR_V_StringScaledWidth(
				FRACUNIT,
				FRACUNIT,
				FRACUNIT,
				V_ALLOWLOWERCASE,
				LT_FONT,
				s
			);
			TSoURDt3rd_V_DrawLevelTitleAtFixed(
				px - 3*FRACUNIT - (w/2),
				py - 16*FRACUNIT,
				tflag,
				s);
		}

		y += 48*FRACUNIT;
		x += 48*FRACUNIT;
	}

	STAR_M_DrawMenuTooltips();

	if (menutransition.tics != menutransition.dest)
		M_DrawOptionsMovingButton();
}

//
// static void TSoURDt3rd_M_Tick(INT32 choice)
// Ticking routine for the main TSoURDt3rd options menu.
//
static void TSoURDt3rd_M_Tick(void)
{
	boolean instanttransmission = optionsmenu.ticker == 0 && menuwipe;

	optionsmenu.ticker++;

	if (tsourdt3rd_currentMenu == NULL)
		return;

	// Move the button for cool animations
	if (currentMenu == &OP_TSoURDt3rdOptionsDef)
	{
		TSoURDt3rd_M_OptionsQuit();	// ...So now this is used here.
	}
#if 0
	else if (optionsmenu.profile == NULL)	// Not currently editing a profile (otherwise we're using these variables for other purposes....)
#else
	else
#endif
	{
		// I don't like this, it looks like shit but it needs to be done..........
		if (optionsmenu.profilemenu)
		{
			optionsmenu.toptx = 440;
			optionsmenu.topty = 70+1;
		}
		else
		{
			optionsmenu.toptx = 160;
			optionsmenu.topty = 50;
		}
	}

	// Handle the background stuff:
	if (optionsmenu.fade)
		optionsmenu.fade--;

	// change the colour if we aren't matching the current menu colour
	if (instanttransmission)
	{
		optionsmenu.currcolour = tsourdt3rd_currentMenu->extra1;
		optionsmenu.offset.start = optionsmenu.fade = 0;

		optionsmenu.optx = optionsmenu.toptx;
		optionsmenu.opty = optionsmenu.topty;
	}
	else
	{
		if (optionsmenu.fade)
			optionsmenu.fade--;
		if (optionsmenu.currcolour != tsourdt3rd_currentMenu->extra1)
		{
			optionsmenu.fade = 10;
			optionsmenu.lastcolour = optionsmenu.currcolour;
			optionsmenu.currcolour = tsourdt3rd_currentMenu->extra1;
		}

		if (optionsmenu.optx != optionsmenu.toptx || optionsmenu.opty != optionsmenu.topty)
		{
			tic_t t = I_GetTime();
			tic_t n = t - optionsmenu.topt_start;
			if (n == M_OPTIONS_OFSTIME)
			{
				optionsmenu.optx = optionsmenu.toptx;
				optionsmenu.opty = optionsmenu.topty;
			}
			else if (n > M_OPTIONS_OFSTIME)
			{
				optionsmenu.topt_start = I_GetTime();
			}
		}
	}

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

	for (INT32 i = op_perfectsave; i < op_continues; i++)
		OP_Tsourdt3rdOptionsMenu[i].status =
			((!(Playing() && playeringame[consoleplayer]) && cv_perfectsave.value) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	OP_Tsourdt3rdOptionsMenu[op_continues].status =
		(!(Playing() && playeringame[consoleplayer]) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

	// Extra Options //
	// TF2
	OP_Tsourdt3rdOptionsMenu[op_dispensergoingup].status =
		(!netgame ? IT_CALL|IT_STRING : IT_GRAYEDOUT);

	// Window Titles
#ifdef HAVE_SDL
	OP_Tsourdt3rdOptionsMenu[op_customwindowtitle].status =
		(cv_windowtitletype.value >= 2 ? IT_STRING|IT_CVAR|IT_CV_STRING : IT_GRAYEDOUT);

	OP_Tsourdt3rdOptionsMenu[op_memesonwindowtitle].status =
		(cv_windowtitletype.value == 1 ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);

#else
	for (INT32 i = op_windowtitletype; i < op_memesonwindowtitle; i++)
		OP_Tsourdt3rdOptionsMenu[i].status = IT_GRAYEDOUT;	
#endif
}

//
// static void TSoURDt3rd_M_OptionsInputs(INT32 choice)
// Input routine for the main TSoURDt3rd options menu.
//
static boolean TSoURDt3rd_M_OptionsInputs(INT32 ch)
{
	const UINT8 pid = 0;
	(void)ch;

	if (menucmd[pid].dpad_ud > 0)
	{
		M_SetMenuDelay(pid);
		optionsmenu.offset.dist = 48;
		STAR_M_NextOpt();
		S_StartSound(NULL, sfx_s3k5b);

		if (tsourdt3rd_itemOn == 0)
			optionsmenu.offset.dist -= currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();

		return true;
	}
	else if (menucmd[pid].dpad_ud < 0)
	{
		M_SetMenuDelay(pid);
		optionsmenu.offset.dist = -48;
		STAR_M_PrevOpt();
		S_StartSound(NULL, sfx_s3k5b);

		if (tsourdt3rd_itemOn == currentMenu->numitems-1)
			optionsmenu.offset.dist += currentMenu->numitems*48;

		optionsmenu.offset.start = I_GetTime();

		return true;
	}
	else if (M_MenuConfirmPressed(pid))
	{
		if (currentMenu->menuitems[tsourdt3rd_itemOn].status & IT_TRANSTEXT)
			return true;	// No.

		optionsmenu.optx = 140;
		optionsmenu.opty = 70;	// Default position for the currently selected option.
		return false;	// Don't eat.
	}
	return false;
}

//
// static void TSoURDt3rd_M_OptionsInputs(INT32 choice)
// Quitting routine for the main TSoURDt3rd options menu.
//
static boolean TSoURDt3rd_M_OptionsQuit(void)
{
	optionsmenu.toptx = 140;
	optionsmenu.topty = 70;

#if 0
	// Reset button behaviour because profile menu is different, since of course it is.
	if (optionsmenu.resetprofilemenu)
	{
		optionsmenu.profilemenu = false;
		optionsmenu.profile = NULL;
		optionsmenu.resetprofilemenu = false;
	}
#endif

	return true;	// Always allow quitting, duh.
}

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
