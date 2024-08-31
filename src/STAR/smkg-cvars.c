// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-cvars.c
/// \brief TSoURDt3rd's command library

#include "smkg-cvars.h"
#include "ss_main.h"
#include "m_menu.h"

#include "drrr/kg_input.h"

#include "../doomstat.h"
#include "../g_game.h"
#include "../m_menu.h"
#include "../v_video.h"

// ------------------------ //
//        Variables
// ------------------------ //

// ====
// MAIN
// ====

consvar_t cv_tsourdt3rd_main_checkforupdatesonstartup = CVAR_INIT ("tsourdt3rd_main_checkforupdatesonstartup", "Yes", CV_SAVE, CV_YesNo, NULL);

// ======
// EVENTS
// ======

consvar_t cv_tsourdt3rd_aprilfools_ultimatemode = CVAR_INIT ("tsourdt3rd_aprilfools_ultimatemode", "Off", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, TSoURD3rd_AprilFools_OnChange);

// ====
// GAME
// ====

static CV_PossibleValue_t tsourdt3rd_startupscreen_t[] = {{0, "Default"}, {1, "Pre-2.2.6"}, {2, "Baby Sonic"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_stjrintro_t[] = {{0, "Default"}, {1, "Pure Fat"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_tpsrate_cons_t[] = {{0, "No"}, {1, "Full"}, {2, "Compact"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_vidcolor_cons_t[] = {
	{V_MAGENTAMAP, "Magenta"},
	{V_YELLOWMAP, "Yellow"},
	{V_GREENMAP, "Green"},
	{V_BLUEMAP, "Blue"},
	{V_REDMAP, "Red"},
	{V_GRAYMAP, "Gray"},
	{V_ORANGEMAP, "Orange"},
	{V_SKYMAP, "Sky"},
	{V_PURPLEMAP, "Purple"},
	{V_AQUAMAP, "Aqua"},
	{V_PERIDOTMAP, "Peridot"},
	{V_AZUREMAP, "Azure"},
	{V_BROWNMAP, "Brown"},
	{V_ROSYMAP, "Rosy"},
	{V_INVERTMAP, "Inverted"},
	{0, NULL}};

static void G_IsItCalledSinglePlayer_OnChange(void);
static void G_TimeOver_OnChange(void);

consvar_t cv_startupscreen = CVAR_INIT ("startupscreen", "Default", CV_SAVE, tsourdt3rd_startupscreen_t, NULL);
consvar_t cv_stjrintro = CVAR_INIT ("stjrintro", "Default", CV_SAVE, tsourdt3rd_stjrintro_t, NULL);

consvar_t cv_tsourdt3rd_game_isitcalledsingleplayer = CVAR_INIT ("tsourdt3rd_game_isitcalledsingleplayer", "Yes", CV_SAVE|CV_CALL, CV_YesNo, G_IsItCalledSinglePlayer_OnChange);

consvar_t cv_allowtypicaltimeover = CVAR_INIT ("allowtypicaltimeover", "No", CV_SAVE|CV_CALL, CV_YesNo, G_TimeOver_OnChange);
consvar_t cv_automapoutsidedevmode = CVAR_INIT ("automapoutsidedevmode", "Off", CV_SAVE, CV_OnOff, NULL);

consvar_t cv_tsourdt3rd_game_soniccd = CVAR_INIT ("soniccd", "Off", CV_SAVE|CV_ALLOWLUA, CV_OnOff, NULL);

// ========
// CONTROLS
// ========

static void C_PadRumble_OnChange(void);
static void C_PadRumble2_OnChange(void);

consvar_t cv_tsourdt3rd_ctrl_drrr_rumble[2] = {
	CVAR_INIT ("tsourdt3rd_drrr_ctrl_rumble", "Off", CV_SAVE|CV_CALL, CV_OnOff, C_PadRumble_OnChange),
	CVAR_INIT ("tsourdt3rd_drrr_ctrl_rumble2", "Off", CV_SAVE|CV_CALL, CV_OnOff, C_PadRumble2_OnChange)
};

// =====
// VIDEO
// =====

// Ported from Uncapped Plus, TPS is back (for some reason)!
consvar_t cv_tsourdt3rd_video_showtps = CVAR_INIT ("tsourdt3rd_video_showtps", "No", CV_SAVE, tsourdt3rd_tpsrate_cons_t, NULL);

consvar_t cv_menucolor = CVAR_INIT ("menucolor", "Yellow", CV_SAVE, tsourdt3rd_vidcolor_cons_t, NULL);
consvar_t cv_fpscountercolor = CVAR_INIT ("fpscountercolor", "Green", CV_SAVE, tsourdt3rd_vidcolor_cons_t, NULL);
consvar_t cv_tpscountercolor = CVAR_INIT ("tpscountercolor", "Green", CV_SAVE, tsourdt3rd_vidcolor_cons_t, NULL);

// =====
// AUDIO
// =====

consvar_t cv_tsourdt3rd_audio_watermuffling = CVAR_INIT ("tsourdt3rd_audio_watermuffling", "Off", CV_SAVE|CV_ALLOWLUA, CV_OnOff, NULL);

// =======
// PLAYERS
// =======

static void P_SuperWithShield_OnChange(void);

consvar_t cv_shieldblockstransformation = CVAR_INIT ("shieldblockstransformation", "Off", CV_SAVE|CV_CALL, CV_OnOff, P_SuperWithShield_OnChange);

consvar_t cv_alwaysoverlayinvuln = CVAR_INIT ("alwaysoverlayinvincibility", "Off", CV_SAVE, CV_OnOff, NULL);

// =========
// SAVEFILES
// =========

static CV_PossibleValue_t tsourdt3rd_perfectsavestripe_t[] = {{0, "MIN"}, {255, "MAX"}, {0, NULL}};

static void SV_UseContinues_OnChange(void);

consvar_t cv_tsourdt3rd_savefiles_storesavesinfolders = CVAR_INIT ("tsourdt3rd_savefiles_storesavesinfolders", "Off", CV_SAVE|CV_CALL, CV_OnOff, STAR_SetSavefileProperties);

consvar_t cv_tsourdt3rd_savefiles_perfectsave = CVAR_INIT ("tsourdt3rd_savefiles_perfectsave", "On", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_savefiles_perfectsavestripe1 = CVAR_INIT ("tsourdt3rd_savefiles_perfectsavestripe1", "134", CV_SAVE, tsourdt3rd_perfectsavestripe_t, NULL);
consvar_t cv_tsourdt3rd_savefiles_perfectsavestripe2 = CVAR_INIT ("tsourdt3rd_savefiles_perfectsavestripe2", "201", CV_SAVE, tsourdt3rd_perfectsavestripe_t, NULL);
consvar_t cv_tsourdt3rd_savefiles_perfectsavestripe3 = CVAR_INIT ("tsourdt3rd_savefiles_perfectsavestripe3", "1", CV_SAVE, tsourdt3rd_perfectsavestripe_t, NULL);

consvar_t cv_continues = CVAR_INIT ("continues", "Off", CV_SAVE|CV_CALL, CV_OnOff, SV_UseContinues_OnChange);

// =======
// SERVERS
// =======

consvar_t cv_rendezvousserver = CVAR_INIT ("holepunchserver", "jart-dev.jameds.org", CV_SAVE, NULL, NULL);

consvar_t cv_movingplayersetup = CVAR_INIT ("movingplayersetup", "Off", CV_SAVE, CV_OnOff, NULL);

// =====
// DEBUG
// =====

consvar_t cv_tsourdt3rd_debug_drrr_virtualkeyboard = CVAR_INIT ("tsourdt3rd_drrr_debug_virtualkeyboard", "Off", CV_SAVE, CV_OnOff, NULL);

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_D_RegisterServerCommands(void)
// Registers TSoURDt3rd's custom commands that should be used by dedicated server hosters.
//
void TSoURDt3rd_D_RegisterServerCommands(void)
{
	CV_RegisterVar(&cv_tsourdt3rd_main_checkforupdatesonstartup);
	CV_RegisterVar(&cv_rendezvousserver);
}

#if 0
static void TSoURDt3rd_InitClientCommands(void)
{
	// Game //
	CV_RegisterVar(&cv_startupscreen);
	CV_RegisterVar(&cv_stjrintro);

	CV_RegisterVar(&cv_loadingscreen);
	CV_RegisterVar(&cv_loadingscreenimage);

	CV_RegisterVar(&cv_quitscreen);

	CV_RegisterVar(&cv_tsourdt3rd_game_isitcalledsingleplayer);

	CV_RegisterVar(&cv_tsourdt3rd_video_showtps);

	CV_RegisterVar(&cv_menucolor);
	CV_RegisterVar(&cv_fpscountercolor);
	CV_RegisterVar(&cv_tpscountercolor);

	CV_RegisterVar(&cv_allowtypicaltimeover);
	CV_RegisterVar(&cv_pausegraphicstyle);
	CV_RegisterVar(&cv_automapoutsidedevmode);

	CV_RegisterVar(&cv_tsourdt3rd_game_soniccd);

	// Savefiles //
	CV_RegisterVar(&cv_continues);

	// Servers //
	CV_RegisterVar(&cv_movingplayersetup);
}
#endif

// ====
// GAME
// ====

static void G_IsItCalledSinglePlayer_OnChange(void)
{
	if (TSoURDt3rd_AprilFools_ModeEnabled())
		return;

	MainMenu[0].text = (cv_tsourdt3rd_game_isitcalledsingleplayer.value ? "Single  Player" : "1  Player");
	STAR_StoreDefaultMenuStrings();	// Stores All the Default Menu Strings Again
}

static void G_TimeOver_OnChange(void)
{
	if (!netgame)
		return;

	if (Playing())
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Sorry, you can't change this while in a netgame.\n");
	CV_StealthSetValue(&cv_allowtypicaltimeover, !cv_allowtypicaltimeover.value);
}

// ========
// CONTROLS
// ========

static void C_PadRumble_OnChange(void)
{
	if (cv_tsourdt3rd_ctrl_drrr_rumble[0].value == 0)
		G_ResetPlayerDeviceRumble(0);
}

static void C_PadRumble2_OnChange(void)
{
	if (cv_tsourdt3rd_ctrl_drrr_rumble[1].value == 0)
		G_ResetPlayerDeviceRumble(1);
}

// =======
// PLAYERS
// =======

static void P_SuperWithShield_OnChange(void)
{
	if (!netgame)
		return;

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Sorry, you can't change this while in a netgame.\n");
	CV_StealthSetValue(&cv_shieldblockstransformation, !cv_shieldblockstransformation.value);
}

// =========
// SAVEFILES
// =========

static void SV_UseContinues_OnChange(void)
{
	if (netgame || multiplayer)
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Please note that continues only work in Singleplayer.\n");
	else if (Playing())
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "You can't set continues while you're playing.\n");
		return;
	}

	useContinues = cv_continues.value;
}
