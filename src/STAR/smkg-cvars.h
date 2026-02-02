// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-cvars.h
/// \brief TSoURDt3rd's command library

#ifndef __SMKG_CVARS__
#define __SMKG_CVARS__

#include "smkg-defs.h"

#include "../command.h"
#include "../netcode/d_net.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

// Events //
extern consvar_t cv_tsourdt3rd_easter_egghunt_allowed;
extern consvar_t cv_tsourdt3rd_easter_egghunt_bonuses;
extern consvar_t cv_tsourdt3rd_aprilfools_ultimatemode;

// Main //
extern consvar_t cv_tsourdt3rd_main_checkforupdatesonstartup;

// Game //
extern consvar_t cv_tsourdt3rd_game_sdl_windowtitle_type;
extern consvar_t cv_tsourdt3rd_game_sdl_windowtitle_custom;
extern consvar_t cv_tsourdt3rd_game_sdl_windowtitle_memes;
extern consvar_t cv_tsourdt3rd_game_startup_image;
extern consvar_t cv_tsourdt3rd_game_startup_intro;
extern consvar_t cv_tsourdt3rd_game_loadingscreen;
extern consvar_t cv_tsourdt3rd_game_loadingscreen_image;
extern consvar_t cv_tsourdt3rd_game_pausescreen;
extern consvar_t cv_tsourdt3rd_game_quitscreen;
extern consvar_t cv_tsourdt3rd_game_allowtimeover;
extern consvar_t cv_tsourdt3rd_game_shadows_realistic;
extern consvar_t cv_tsourdt3rd_game_shadows_forallobjects;
extern consvar_t cv_tsourdt3rd_game_shadows_positioning;
extern consvar_t cv_tsourdt3rd_game_shadows_offset;
extern consvar_t cv_tsourdt3rd_game_soniccd;
extern consvar_t cv_tsourdt3rd_game_isitcalledsingleplayer;

// Controls //
extern consvar_t cv_tsourdt3rd_drrr_controls_rumble[MAXSPLITSCREENPLAYERS];

// Video //
extern consvar_t cv_tsourdt3rd_video_sdl_window_shaking;
extern consvar_t cv_tsourdt3rd_video_showtps; // Ported from Uncapped Plus, TPS is back (for some reason)!
extern consvar_t cv_tsourdt3rd_video_font_fps, cv_tsourdt3rd_video_font_tps; // Credit to SRB2Classic, Jisk, LuigiBuddd, etc. for the idea
#ifdef STAR_LIGHTING
extern consvar_t cv_tsourdt3rd_video_lighting_coronas;
extern consvar_t cv_tsourdt3rd_video_lighting_coronas_size;
extern consvar_t cv_tsourdt3rd_video_lighting_coronas_lightingtype;
extern consvar_t cv_tsourdt3rd_video_lighting_coronas_drawingmode;
extern consvar_t cv_tsourdt3rd_video_lighting_coronas_walllighting;
extern consvar_t cv_tsourdt3rd_video_lighting_coronas_maxlights;
#endif

// Audio //
extern consvar_t cv_tsourdt3rd_audio_watermuffling;
extern consvar_t cv_tsourdt3rd_audio_vapemode;
extern consvar_t cv_tsourdt3rd_audio_bosses_postboss;

// Players //
extern consvar_t cv_tsourdt3rd_players_shieldblockstransformation;
extern consvar_t cv_tsourdt3rd_players_nukewhilesuper;
extern consvar_t cv_tsourdt3rd_players_setupwhilemoving;
extern consvar_t cv_tsourdt3rd_players_alwaysoverlayinvulnsparks;

// Savefiles //
extern consvar_t cv_tsourdt3rd_savefiles_limitedcontinues;
extern consvar_t cv_tsourdt3rd_savefiles_storesavesinfolders;
extern consvar_t cv_tsourdt3rd_savefiles_perfectsave;
extern consvar_t cv_tsourdt3rd_savefiles_perfectsave_stripe1;
extern consvar_t cv_tsourdt3rd_savefiles_perfectsave_stripe2;
extern consvar_t cv_tsourdt3rd_savefiles_perfectsave_stripe3;

// Servers //
extern consvar_t cv_tsourdt3rd_servers_holepunchrendezvous;

// Jukebox //
extern consvar_t cv_tsourdt3rd_jukebox_hud;
extern consvar_t cv_tsourdt3rd_jukebox_speed;
extern consvar_t cv_tsourdt3rd_jukebox_pitch;

// Debug //
extern consvar_t cv_tsourdt3rd_debug_drrr_virtualkeyboard;
extern consvar_t cv_tsourdt3rd_debug_automapanywhere;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_D_RegisterServerCommands(void);
void TSoURDt3rd_D_RegisterClientCommands(void);

boolean TSoURDt3rd_CV_CheckForOldCommands(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_CVARS__
