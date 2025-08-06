// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-cvars.c
/// \brief TSoURDt3rd's command library

#include "smkg-cvars.h"
#include "ss_main.h"
#include "star_vars.h" // STAR_SetWindowTitle() //
#include "core/smkg-p_pads.h" // TSoURDt3rd_P_Pads_ResetDeviceRumble() //
#include "core/smkg-s_audio.h" // TSoURDt3rd_S_ControlMusicEffects() //
#include "core/smkg-s_exmusic.h"
#include "core/smkg-s_jukebox.h" // TSoURDt3rd_Jukebox_IsPlaying() //
#include "lights/smkg-coronas.h"
#include "menus/smkg-m_sys.h" // menu definitions //
#include "misc/smkg-m_misc.h" // TSoURDt3rd_M_FindWordInTermTable() //

#include "../discord/discord.h" // TSoURDt3rd_D_Got_DiscordInfo() //

#include "../doomstat.h"
#include "../console.h"
#include "../fastcmp.h"
#include "../g_game.h"
#include "../m_menu.h"
#include "../r_main.h"
#include "../v_video.h"

#include "../netcode/net_command.h" // RegisterNetXCmd

#ifdef USE_STUN
#include "stun/stun.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

static CV_PossibleValue_t tsourdt3rd_sdl_windowtitle_cons_t[] = {{0, "Default"}, {1, "Dynamic"}, {2, "Semi-Custom"}, {3, "Fully Custom"}, {0, NULL}};

static CV_PossibleValue_t tsourdt3rd_game_startup_image_cons_t[] = {{0, "Default"}, {1, "Pre-2.2.6"}, {2, "Baby Sonic"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_game_startup_intro_cons_t[] = {{0, "Default"}, {1, "Pure Fat"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_game_loadingscreen_image_cons_t[] = {{0, "None"}, {1, "Dynamic"},
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
static CV_PossibleValue_t tsourdt3rd_game_pausescreen_cons_t[] = {{0, "Default"}, {1, "Old-School"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_game_quitscreen_cons_t[] = {
	{0, "Default"}, {1, "Color"},
	{2, "Smug"},
	{3, "Kel"},
	{4, "SATRB"},
	{0, NULL}};
static CV_PossibleValue_t tsourdt3rd_game_shadowposition_cons_t[] ={
	{0, "Object's Front"},
	{1, "Camera"},
	{2, "Static"},
	{0, NULL}};

static CV_PossibleValue_t tsourdt3rd_video_tpsrate_cons_t[] = {{0, "No"}, {1, "Full"}, {2, "Compact"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_video_coloring_cons_t[] = {
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
/// \todo add and make it work
static CV_PossibleValue_t tsourdt3rd_video_font_cons_t[] = {{0, "Normal"}, {1, "Thin"}, {0, NULL}};
#ifdef STAR_LIGHTING
static CV_PossibleValue_t tsourdt3rd_video_lighting_coronas_cons_t[] = {
	{0, "Off"},
	{1, "Special"},
	{2, "Most"},
	{3, "Dim"},
	{4, "All"},
	{5, "Bright"},
	{6, "Old"},
	{0, NULL}};
static CV_PossibleValue_t tsourdt3rd_video_lighting_coronas_lightingtype_cons_t[] = {
	{0, "Static"},
	{1, "Dynamic"},
	{0, NULL}};
static CV_PossibleValue_t tsourdt3rd_video_lighting_coronas_drawingmode_cons_t[] = {
	{0, "Blend"},
	{1, "Blend_BG"},
	{2, "Additive"},
	{3, "Additive_BG"},
	{4, "Add_Limit"},
	{0, NULL}};
#endif

static CV_PossibleValue_t tsourdt3rd_audio_vapemode_cons_t[] = {{0, "Off"}, {1, "TSoURDt3rd"}, {2, "Sonic Mania Plus"}, {0, NULL}};

static CV_PossibleValue_t tsourdt3rd_savefiles_perfectsavestripe_cons_t[] = {{0, "MIN"}, {255, "MAX"}, {0, NULL}};

static CV_PossibleValue_t tsourdt3rd_jukebox_hud_cons_t[] = {{0, "Off"}, {1, "Minimalized"}, {2, "On"}, {0, NULL}};
static CV_PossibleValue_t tsourdt3rd_jukebox_modifier_cons_t[] = {{0.1*FRACUNIT, "MIN"}, {20*FRACUNIT, "MAX"}, {0, NULL}};

static boolean G_TimeOver_OnChange(const char *valstr);
static boolean G_RealisticShadows_OnChange(const char *valstr);
static void G_IsItCalledSinglePlayer_OnChange(void);

static void C_PadRumble_OnChange(void);

#ifdef STAR_LIGHTING
static void V_Coronas_OnChange(void);
#endif

static void A_VapeMode_OnChange(void);

static boolean P_SuperWithShield_OnChange(const char *valstr);

static void SV_UseContinues_OnChange(void);

static void JB_Speed_OnChange(void);
static void JB_Pitch_OnChange(void);

consvar_t cv_tsourdt3rd_aprilfools_ultimatemode = CVAR_INIT ("tsourdt3rd_aprilfools_ultimatemode", "Off", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, TSoURD3rd_AprilFools_OnChange);

consvar_t cv_tsourdt3rd_main_checkforupdatesonstartup = CVAR_INIT ("tsourdt3rd_main_checkforupdatesonstartup", "Yes", CV_SAVE, CV_YesNo, NULL);

consvar_t cv_tsourdt3rd_game_sdl_windowtitle_type = CVAR_INIT ("tsourdt3rd_game_sdl_windowtitle_type", "Default", CV_SAVE|CV_CALL, tsourdt3rd_sdl_windowtitle_cons_t, (void *)STAR_SetWindowTitle);
consvar_t cv_tsourdt3rd_game_sdl_windowtitle_custom = CVAR_INIT ("tsourdt3rd_game_sdl_windowtitle_custom", "Currently Robo Blasting in", CV_SAVE|CV_CALL, NULL, (void *)STAR_SetWindowTitle);
consvar_t cv_tsourdt3rd_game_sdl_windowtitle_memes = CVAR_INIT ("tsourdt3rd_game_sdl_windowtitle_memes", "Yes", CV_SAVE|CV_CALL, CV_YesNo, (void *)STAR_SetWindowTitle);
consvar_t cv_tsourdt3rd_game_startup_image = CVAR_INIT ("tsourdt3rd_game_startup_image", "Default", CV_SAVE, tsourdt3rd_game_startup_image_cons_t, NULL);
consvar_t cv_tsourdt3rd_game_startup_intro = CVAR_INIT ("tsourdt3rd_game_startup_intro", "Default", CV_SAVE, tsourdt3rd_game_startup_intro_cons_t, NULL);
consvar_t cv_tsourdt3rd_game_loadingscreen = CVAR_INIT ("tsourdt3rd_game_loadingscreen", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_game_loadingscreen_image = CVAR_INIT ("tsourdt3rd_game_loadingscreen_image", "Intermission", CV_SAVE, tsourdt3rd_game_loadingscreen_image_cons_t, NULL);
consvar_t cv_tsourdt3rd_game_pausescreen = CVAR_INIT ("tsourdt3rd_game_pausescreen", "Default", CV_SAVE, tsourdt3rd_game_pausescreen_cons_t, NULL);
consvar_t cv_tsourdt3rd_game_quitscreen = CVAR_INIT ("tsourdt3rd_game_quitscreen", "Default", CV_SAVE, tsourdt3rd_game_quitscreen_cons_t, NULL);
consvar_t cv_tsourdt3rd_game_allowtimeover = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_game_allowtimeover", "No", CV_SAVE|CV_CALL, CV_YesNo, NULL, G_TimeOver_OnChange);
consvar_t cv_tsourdt3rd_game_shadows_realistic = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_game_shadows_realistic", "Off", CV_SAVE|CV_CALL, CV_OnOff, NULL, G_RealisticShadows_OnChange);
consvar_t cv_tsourdt3rd_game_shadows_forallobjects = CVAR_INIT ("tsourdt3rd_game_shadows_forallobjects", "No", CV_SAVE, CV_YesNo, NULL);
consvar_t cv_tsourdt3rd_game_shadows_positioning = CVAR_INIT ("tsourdt3rd_game_shadows_positioning", "Object's Front", CV_SAVE, tsourdt3rd_game_shadowposition_cons_t, NULL);
consvar_t cv_tsourdt3rd_game_soniccd = CVAR_INIT ("soniccd", "Off", CV_SAVE|CV_ALLOWLUA, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_game_isitcalledsingleplayer = CVAR_INIT ("tsourdt3rd_game_isitcalledsingleplayer", "Yes", CV_SAVE|CV_CALL, CV_YesNo, G_IsItCalledSinglePlayer_OnChange);

consvar_t cv_tsourdt3rd_drrr_controls_rumble[2] = {
	CVAR_INIT ("tsourdt3rd_drrr_controls_rumble", "Off", CV_SAVE|CV_CALL, CV_OnOff, C_PadRumble_OnChange),
	CVAR_INIT ("tsourdt3rd_drrr_controls_rumble2", "Off", CV_SAVE|CV_CALL, CV_OnOff, C_PadRumble_OnChange)
};

consvar_t cv_tsourdt3rd_video_sdl_window_shaking = CVAR_INIT ("tsourdt3rd_video_sdl_window_shaking", "No", CV_SAVE, CV_YesNo, NULL);
consvar_t cv_tsourdt3rd_video_showtps = CVAR_INIT ("tsourdt3rd_video_showtps", "No", CV_SAVE, tsourdt3rd_video_tpsrate_cons_t, NULL); // Ported from Uncapped Plus, TPS is back (for some reason)!
consvar_t cv_tsourdt3rd_video_coloring_menus = CVAR_INIT ("tsourdt3rd_video_coloring_menus", "Yellow", CV_SAVE, tsourdt3rd_video_coloring_cons_t, NULL);
consvar_t cv_tsourdt3rd_video_coloring_fpsrate = CVAR_INIT ("tsourdt3rd_video_coloring_fpsrate", "Green", CV_SAVE, tsourdt3rd_video_coloring_cons_t, NULL);
/// \todo add and make it work
consvar_t cv_tsourdt3rd_video_font_fps = CVAR_INIT ("tsourdt3rd_video_font_fps", "Normal", CV_SAVE, tsourdt3rd_video_font_cons_t, NULL); // Credit to SRB2Classic, Jisk, LuigiBuddd, etc. for the idea
consvar_t cv_tsourdt3rd_video_coloring_tpsrate = CVAR_INIT ("tsourdt3rd_video_coloring_tpsrate", "Green", CV_SAVE, tsourdt3rd_video_coloring_cons_t, NULL);
/// \todo add and make it work
consvar_t cv_tsourdt3rd_video_font_tps = CVAR_INIT ("tsourdt3rd_video_font_tps", "Normal", CV_SAVE, tsourdt3rd_video_font_cons_t, NULL); // Credit to SRB2Classic, Jisk, LuigiBuddd, etc. for the idea
#ifdef STAR_LIGHTING
consvar_t cv_tsourdt3rd_video_lighting_coronas = CVAR_INIT ("tsourdt3rd_video_lighting_coronas", "Off", CV_SAVE|CV_CALL, tsourdt3rd_video_lighting_coronas_cons_t, V_Coronas_OnChange);
consvar_t cv_tsourdt3rd_video_lighting_coronas_size = CVAR_INIT ("tsourdt3rd_video_lighting_coronas_size", "1", CV_SAVE|CV_FLOAT, 0, NULL);
consvar_t cv_tsourdt3rd_video_lighting_coronas_lightingtype = CVAR_INIT ("tsourdt3rd_video_lighting_coronas_lightingtype", "Dynamic", CV_SAVE, tsourdt3rd_video_lighting_coronas_lightingtype_cons_t, NULL);
consvar_t cv_tsourdt3rd_video_lighting_coronas_drawingmode = CVAR_INIT ("tsourdt3rd_video_lighting_coronas_drawingmode", "Additive", CV_SAVE, tsourdt3rd_video_lighting_coronas_drawingmode_cons_t, NULL);
#endif

consvar_t cv_tsourdt3rd_audio_watermuffling = CVAR_INIT ("tsourdt3rd_audio_watermuffling", "Off", CV_SAVE|CV_ALLOWLUA, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_audio_vapemode = CVAR_INIT ("tsourdt3rd_audio_vapemode", "Off", CV_SAVE|CV_CALL, tsourdt3rd_audio_vapemode_cons_t, A_VapeMode_OnChange);
consvar_t cv_tsourdt3rd_audio_bosses_postboss = CVAR_INIT ("tsourdt3rd_audio_bosses_postboss", "On", CV_SAVE, CV_OnOff, NULL);
#if 1
// MARKED FOR REMOVAL
/// \todo so remove it
static CV_PossibleValue_t tsourdt3rd_audio_gameover_cons_t[] = {{0, "Game's Default"},
	{1, "Sonic 1&2"},
	{2, "Sonic CD"},
	{3, "Sonic 3&K"},
	{4, "Sonic Rush"},
	{5, "Sonic Mania"},
	{6, "Sammy"},
	{7, "Child"},
	{8, "Yeah!"},
	{0, NULL}};
consvar_t cv_tsourdt3rd_audio_gameover = CVAR_INIT ("tsourdt3rd_audio_gameover", "Game's Default", CV_SAVE, tsourdt3rd_audio_gameover_cons_t, NULL);
#endif
consvar_t cv_tsourdt3rd_audio_exmusic_defaultmaptrack = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_defaultmaptrack", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_bosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_bosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_bosspinch = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_bosspinch", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_finalbosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_finalbosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_finalbosspinch = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_finalbosspinch", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_truefinalbosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_truefinalbosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_truefinalbosspinch = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_truefinalbosspinch", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_racebosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_racebosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_intermission = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_intermission", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_intermission_bosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_intermission_bosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_intermission_finalbosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_intermission_finalbosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_intermission_truefinalbosses = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_intermission_truefinalbosses", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);
consvar_t cv_tsourdt3rd_audio_exmusic_gameover = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_audio_exmusic_gameover", "Default", CV_SAVE|CV_NOINIT|CV_CALL, NULL, TSoURDt3rd_S_EXMusic_Update, TSoURDt3rd_S_EXMusic_CanUpdate);

consvar_t cv_tsourdt3rd_players_shieldblockstransformation = CVAR_INIT_WITH_CALLBACKS ("tsourdt3rd_players_shieldblockstransformation", "Off", CV_SAVE|CV_CALL, CV_OnOff, NULL, P_SuperWithShield_OnChange);
consvar_t cv_tsourdt3rd_players_nukewhilesuper = CVAR_INIT ("tsourdt3rd_players_nukewhilesuper", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_players_setupwhilemoving = CVAR_INIT ("tsourdt3rd_players_setupwhilemoving", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_players_alwaysoverlayinvulnsparks = CVAR_INIT ("tsourdt3rd_players_tsourdt3rd_players_alwaysoverlayinvulnsparkssparks", "Off", CV_SAVE, CV_OnOff, NULL);

consvar_t cv_tsourdt3rd_savefiles_limitedcontinues = CVAR_INIT ("tsourdt3rd_savefiles_limitedcontinues", "Off", CV_SAVE|CV_CALL, CV_OnOff, SV_UseContinues_OnChange);
consvar_t cv_tsourdt3rd_savefiles_storesavesinfolders = CVAR_INIT ("tsourdt3rd_savefiles_storesavesinfolders", "Off", CV_SAVE|CV_CALL, CV_OnOff, TSoURDt3rd_FIL_CreateSavefileProperly);
consvar_t cv_tsourdt3rd_savefiles_perfectsave = CVAR_INIT ("tsourdt3rd_savefiles_perfectsave", "On", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_savefiles_perfectsave_stripe1 = CVAR_INIT ("tsourdt3rd_savefiles_perfectsave_stripe1", "134", CV_SAVE, tsourdt3rd_savefiles_perfectsavestripe_cons_t, NULL);
consvar_t cv_tsourdt3rd_savefiles_perfectsave_stripe2 = CVAR_INIT ("tsourdt3rd_savefiles_perfectsave_stripe2", "201", CV_SAVE, tsourdt3rd_savefiles_perfectsavestripe_cons_t, NULL);
consvar_t cv_tsourdt3rd_savefiles_perfectsave_stripe3 = CVAR_INIT ("tsourdt3rd_savefiles_perfectsave_stripe3", "1", CV_SAVE, tsourdt3rd_savefiles_perfectsavestripe_cons_t, NULL);

consvar_t cv_tsourdt3rd_servers_holepunchrendezvous = CVAR_INIT ("tsourdt3rd_servers_holepunchrendezvous", "jart-dev.jameds.org", CV_SAVE, NULL, NULL);

consvar_t cv_tsourdt3rd_jukebox_hud = CVAR_INIT ("tsourdt3rd_jukebox_hud", "On", CV_SAVE, tsourdt3rd_jukebox_hud_cons_t, NULL);
consvar_t cv_tsourdt3rd_jukebox_speed = CVAR_INIT ("tsourdt3rd_jukebox_speed", "1", CV_FLOAT|CV_SAVE|CV_CALL, tsourdt3rd_jukebox_modifier_cons_t, JB_Speed_OnChange);
consvar_t cv_tsourdt3rd_jukebox_pitch = CVAR_INIT ("tsourdt3rd_jukebox_pitch", "1", CV_FLOAT|CV_SAVE|CV_CALL, tsourdt3rd_jukebox_modifier_cons_t, JB_Pitch_OnChange);

consvar_t cv_tsourdt3rd_debug_drrr_virtualkeyboard = CVAR_INIT ("tsourdt3rd_drrr_debug_virtualkeyboard", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_tsourdt3rd_debug_automapanywhere = CVAR_INIT ("tsourdt3rd_debug_automapanywhere", "Off", CV_SAVE, CV_OnOff, NULL);

enum tsourdt3rd_old_cvars_e
{
#ifdef HAVE_DISCORDSUPPORT
	// Discord
	discordcustom_details,
	discordcustom_state,
	discordcustom_imagetype_large,
	discordcustom_imagetype_small,
	discordcustom_characterimage_large,
	discordcustom_characterimage_small,
	discordcustom_supercharacterimage_large,
	discordcustom_supercharacterimage_small,
	discordcustom_mapimage_large,
	discordcustom_mapimage_small,
	discordcustom_miscimage_large,
	discordcustom_miscimage_small,
	discordcustom_imagetext_large,
	discordcustom_imagetext_small,
#endif
	// Events
	tsourdt3rd_easter_egghunt_allowed,
	tsourdt3rd_easter_egghunt_bonuses,
	// Game
	tsourdt3rd_game_sdl_windowtitle_type,
	tsourdt3rd_game_sdl_windowtitle_custom,
	tsourdt3rd_game_sdl_windowtitle_memes,
	tsourdt3rd_game_startup_image,
	tsourdt3rd_game_startup_intro,
	tsourdt3rd_game_loadingscreen,
	tsourdt3rd_game_loadingscreen_image,
	tsourdt3rd_game_pausescreen,
	tsourdt3rd_game_quitscreen,
	tsourdt3rd_game_allowtimeover,
	tsourdt3rd_game_shadows_positioning,
	tsourdt3rd_game_isitcalledsingleplayer,
	// Video
	tsourdt3rd_video_showtps,
	tsourdt3rd_video_coloring_menus,
	tsourdt3rd_video_coloring_fpsrate,
	tsourdt3rd_video_coloring_tpsrate,
	// Audio
	tsourdt3rd_audio_gameover,
	// Players
	tsourdt3rd_players_shieldblockstransformation,
	tsourdt3rd_players_nukewhilesuper,
	tsourdt3rd_players_setupwhilemoving,
	tsourdt3rd_players_alwaysoverlayinvulnsparks,
	// Savefiles
	tsourdt3rd_savefiles_limitedcontinues,
	tsourdt3rd_savefiles_storesavesinfolders,
	tsourdt3rd_savefiles_perfectsave,
	tsourdt3rd_savefiles_perfectsave_stripe1,
	tsourdt3rd_savefiles_perfectsave_stripe2,
	tsourdt3rd_savefiles_perfectsave_stripe3,
	// Servers
	tsourdt3rd_servers_holepunchrendezvous,
	// Jukebox
	tsourdt3rd_jukebox_hud,
	tsourdt3rd_jukebox_speed,
	// Debug
	tsourdt3rd_debug_automapanywhere,
}; static const char *const tsourdt3rd_old_cvars_opt[] = {
#ifdef HAVE_DISCORDSUPPORT
	// Discord
	"customdiscorddetails",
	"customdiscordstate",
	"customdiscordlargeimagetype",
	"customdiscordsmallimagetype",
	"customdiscordlargecharacterimage",
	"customdiscordsmallcharacterimage",
	"customdiscordlargesupercharacterimage",
	"customdiscordsmallsupercharacterimage",
	"customdiscordlargemapimage",
	"customdiscordsmallmapimage",
	"customdiscordlargemiscimage",
	"customdiscordsmallmiscimage",
	"customdiscordlargeimagetext",
	"customdiscordsmallimagetext",
#endif
	// Events
	"alloweasteregghunt",
	"easteregghuntbonuses",
	// Game
	"windowtitletype",
	"customwindowtitle",
	"memesonwindowtitle",
	"startupscreen",
	"stjrintro",
	"loadingscreen",
	"loadingscreenimage",
	"pausegraphicstyle",
	"quitscreen",
	"allowtypicaltimeover",
	"realisticshadowscanrotate",
	"isitcalledsingleplayer",
	// Video
	"showtps",
	"menucolor",
	"fpscountercolor",
	"tpscountercolor",
	// Audio
	"gameovermusic",
	// Players
	"shieldblockstransformation",
	"armageddonnukesuper",
	"movingplayersetup",
	"alwaysoverlayinvincibility",
	// Savefiles
	"continues",
	"storesavesinfolders",
	"perfectsave",
	"perfectsavestripe1",
	"perfectsavestripe2",
	"perfectsavestripe3",
	// Servers
	"holepunchserver",
	// Jukebox
	"jukeboxhud",
	"jukeboxspeed",
	// Debug
	"automapoutsidedevmode",
	NULL
}; static consvar_t *old_to_new_vars[] = {
#ifdef HAVE_DISCORDSUPPORT
	&cv_discordcustom_details,
	&cv_discordcustom_state,
	&cv_discordcustom_imagetype_large,
	&cv_discordcustom_imagetype_small,
	&cv_discordcustom_characterimage_large,
	&cv_discordcustom_characterimage_small,
	&cv_discordcustom_supercharacterimage_large,
	&cv_discordcustom_supercharacterimage_small,
	&cv_discordcustom_mapimage_large,
	&cv_discordcustom_mapimage_small,
	&cv_discordcustom_miscimage_large,
	&cv_discordcustom_miscimage_small,
	&cv_discordcustom_imagetext_large,
	&cv_discordcustom_imagetext_small,
#endif
	&cv_tsourdt3rd_easter_egghunt_allowed,
	&cv_tsourdt3rd_easter_egghunt_bonuses,
	&cv_tsourdt3rd_game_sdl_windowtitle_type,
	&cv_tsourdt3rd_game_sdl_windowtitle_custom,
	&cv_tsourdt3rd_game_sdl_windowtitle_memes,
	&cv_tsourdt3rd_game_startup_image,
	&cv_tsourdt3rd_game_startup_intro,
	&cv_tsourdt3rd_game_loadingscreen,
	&cv_tsourdt3rd_game_loadingscreen_image,
	&cv_tsourdt3rd_game_pausescreen,
	&cv_tsourdt3rd_game_quitscreen,
	&cv_tsourdt3rd_game_allowtimeover,
	&cv_tsourdt3rd_game_shadows_positioning,
	&cv_tsourdt3rd_game_isitcalledsingleplayer,
	&cv_tsourdt3rd_video_showtps,
	&cv_tsourdt3rd_video_coloring_menus,
	&cv_tsourdt3rd_video_coloring_fpsrate,
	&cv_tsourdt3rd_video_coloring_tpsrate,
	&cv_tsourdt3rd_audio_gameover,
	&cv_tsourdt3rd_players_shieldblockstransformation,
	&cv_tsourdt3rd_players_nukewhilesuper,
	&cv_tsourdt3rd_players_setupwhilemoving,
	&cv_tsourdt3rd_players_alwaysoverlayinvulnsparks,
	&cv_tsourdt3rd_savefiles_limitedcontinues,
	&cv_tsourdt3rd_savefiles_storesavesinfolders,
	&cv_tsourdt3rd_savefiles_perfectsave,
	&cv_tsourdt3rd_savefiles_perfectsave_stripe1,
	&cv_tsourdt3rd_savefiles_perfectsave_stripe2,
	&cv_tsourdt3rd_savefiles_perfectsave_stripe3,
	&cv_tsourdt3rd_servers_holepunchrendezvous,
	&cv_tsourdt3rd_jukebox_hud,
	&cv_tsourdt3rd_jukebox_speed,
	&cv_tsourdt3rd_debug_automapanywhere,
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_D_RegisterServerCommands(void)
// Registers TSoURDt3rd's custom commands that are necessary when used by dedicated server hosters.
//
void TSoURDt3rd_D_RegisterServerCommands(void)
{
	// Discord //
#ifdef USE_STUN
	CV_RegisterVar(&cv_stunserver);
#endif
	CV_RegisterVar(&cv_discordinvites);
	RegisterNetXCmd(XD_DISCORD, TSoURDt3rd_D_Got_DiscordInfo);

	// Main //
	CV_RegisterVar(&cv_tsourdt3rd_main_checkforupdatesonstartup);

	// Server //
	CV_RegisterVar(&cv_tsourdt3rd_servers_holepunchrendezvous);
}

//
// void TSoURDt3rd_D_RegisterClientCommands(void)
// Registers TSoURDt3rd's custom commands that can be used anytime, for everyone.
//
void TSoURDt3rd_D_RegisterClientCommands(void)
{
	INT32 i;

#ifdef HAVE_DISCORDSUPPORT
	// Discord //
	CV_RegisterVar(&cv_discordrp);
	CV_RegisterVar(&cv_discordstreamer);
	CV_RegisterVar(&cv_discordasks);
	CV_RegisterVar(&cv_discordshowonstatus);
	CV_RegisterVar(&cv_discordstatusmemes);
	CV_RegisterVar(&cv_discordcharacterimagetype);
	CV_RegisterVar(&cv_discordcustom_details);
	CV_RegisterVar(&cv_discordcustom_state);
	CV_RegisterVar(&cv_discordcustom_imagetype_large);
	CV_RegisterVar(&cv_discordcustom_imagetype_small);
	CV_RegisterVar(&cv_discordcustom_characterimage_large);
	CV_RegisterVar(&cv_discordcustom_characterimage_small);
	CV_RegisterVar(&cv_discordcustom_supercharacterimage_large);
	CV_RegisterVar(&cv_discordcustom_supercharacterimage_small);
	CV_RegisterVar(&cv_discordcustom_mapimage_large);
	CV_RegisterVar(&cv_discordcustom_mapimage_small);
	CV_RegisterVar(&cv_discordcustom_miscimage_large);
	CV_RegisterVar(&cv_discordcustom_miscimage_small);
	CV_RegisterVar(&cv_discordcustom_imagetext_large);
	CV_RegisterVar(&cv_discordcustom_imagetext_small);
#endif

	// Game //
	CV_RegisterVar(&cv_tsourdt3rd_game_sdl_windowtitle_type);
	CV_RegisterVar(&cv_tsourdt3rd_game_sdl_windowtitle_custom);
	CV_RegisterVar(&cv_tsourdt3rd_game_sdl_windowtitle_memes);
	CV_RegisterVar(&cv_tsourdt3rd_game_startup_image);
	CV_RegisterVar(&cv_tsourdt3rd_game_startup_intro);
	CV_RegisterVar(&cv_tsourdt3rd_game_loadingscreen);
	CV_RegisterVar(&cv_tsourdt3rd_game_loadingscreen_image);
	CV_RegisterVar(&cv_tsourdt3rd_game_pausescreen);
	CV_RegisterVar(&cv_tsourdt3rd_game_quitscreen);
	CV_RegisterVar(&cv_tsourdt3rd_game_allowtimeover);
	CV_RegisterVar(&cv_tsourdt3rd_game_shadows_realistic);
	CV_RegisterVar(&cv_tsourdt3rd_game_shadows_forallobjects);
	CV_RegisterVar(&cv_tsourdt3rd_game_shadows_positioning);
	CV_RegisterVar(&cv_tsourdt3rd_game_soniccd);
	CV_RegisterVar(&cv_tsourdt3rd_game_isitcalledsingleplayer);

	// Controls //
	for (i = 0; i < MAXSPLITSCREENPLAYERS; i++)
		CV_RegisterVar(&cv_tsourdt3rd_drrr_controls_rumble[i]);

	// Video //
	CV_RegisterVar(&cv_tsourdt3rd_video_sdl_window_shaking);
	CV_RegisterVar(&cv_tsourdt3rd_video_showtps);
	CV_RegisterVar(&cv_tsourdt3rd_video_coloring_menus);
	CV_RegisterVar(&cv_tsourdt3rd_video_coloring_fpsrate);
	CV_RegisterVar(&cv_tsourdt3rd_video_font_fps);
	CV_RegisterVar(&cv_tsourdt3rd_video_coloring_tpsrate);
	CV_RegisterVar(&cv_tsourdt3rd_video_font_tps);
#ifdef STAR_LIGHTING
	// -- Lighting
	CV_RegisterVar(&cv_tsourdt3rd_video_lighting_coronas);
	CV_RegisterVar(&cv_tsourdt3rd_video_lighting_coronas_size);
	CV_RegisterVar(&cv_tsourdt3rd_video_lighting_coronas_lightingtype);
	CV_RegisterVar(&cv_tsourdt3rd_video_lighting_coronas_drawingmode);
#endif

	// Audio //
	CV_RegisterVar(&cv_tsourdt3rd_audio_watermuffling);
	CV_RegisterVar(&cv_tsourdt3rd_audio_vapemode);
	CV_RegisterVar(&cv_tsourdt3rd_audio_bosses_postboss);
	// -- EXMusic
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_defaultmaptrack);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_bosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_bosspinch);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_finalbosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_finalbosspinch);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_truefinalbosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_truefinalbosspinch);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_racebosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_intermission);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_intermission_bosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_intermission_finalbosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_intermission_truefinalbosses);
	CV_RegisterVar(&cv_tsourdt3rd_audio_exmusic_gameover);

	// Players //
	CV_RegisterVar(&cv_tsourdt3rd_players_shieldblockstransformation);
	CV_RegisterVar(&cv_tsourdt3rd_players_nukewhilesuper);
	CV_RegisterVar(&cv_tsourdt3rd_players_setupwhilemoving);
	CV_RegisterVar(&cv_tsourdt3rd_players_alwaysoverlayinvulnsparks);

	// Savefiles //
	CV_RegisterVar(&cv_tsourdt3rd_savefiles_limitedcontinues);
	CV_RegisterVar(&cv_tsourdt3rd_savefiles_storesavesinfolders);
	CV_RegisterVar(&cv_tsourdt3rd_savefiles_perfectsave);
	CV_RegisterVar(&cv_tsourdt3rd_savefiles_perfectsave_stripe1);
	CV_RegisterVar(&cv_tsourdt3rd_savefiles_perfectsave_stripe2);
	CV_RegisterVar(&cv_tsourdt3rd_savefiles_perfectsave_stripe3);

	// Jukebox //
	CV_RegisterVar(&cv_tsourdt3rd_jukebox_hud);
	CV_RegisterVar(&cv_tsourdt3rd_jukebox_speed);
	CV_RegisterVar(&cv_tsourdt3rd_jukebox_pitch);

	// Debugging //
	CV_RegisterVar(&cv_tsourdt3rd_debug_drrr_virtualkeyboard);
	CV_RegisterVar(&cv_tsourdt3rd_debug_automapanywhere);
}

boolean TSoURDt3rd_CV_CheckForOldCommands(void)
{
	INT32 word_to_table_val;
	const char *command;
	const char *cv_value;
	consvar_t *cvar;

	// Check if the command doesn't already exist...
	command = COM_Argv(0);
	if (command == NULL || *command == '\0' || CV_FindVar(command))
		return false;

	// Check if the command matches a version of an old TSoURDt3rd command...
	if (con_destlines || dedicated)
		STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("Checking if command '%s' matches old TSoURDt3rd commands...\n"), command);

	word_to_table_val = TSoURDt3rd_M_FindWordInTermTable(tsourdt3rd_old_cvars_opt, command, TSOURDT3RD_TERMTABLESEARCH_NORM);
	if (word_to_table_val < 0)
	{
		if (con_destlines || dedicated)
			STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("Command '%s' doesn't match old TSoURDt3rd command! Skipping...\n"), command);
		return false;
	}

	// Port the command values over, and we're golden!
	cvar = old_to_new_vars[word_to_table_val];
	cv_value = COM_Argv(1);

	if (con_destlines || dedicated)
		STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("Command '%s' matches old TSoURDt3rd command! Proceeding to work our magic...\n"), command);
	if (cv_value == NULL || *cv_value == '\0')
	{
		// perform a variable print or set
		CONS_Printf(M_GetText("\"%s\" is \"%s\" default is \"%s\"\n"), cvar->name, cvar->string, cvar->defaultvalue);
		return true;
	}

	switch (word_to_table_val)
	{
#ifdef HAVE_DISCORDSUPPORT
		case discordcustom_imagetype_large:
		case discordcustom_imagetype_small:
			if (fastcmp(cv_value, "CS Portrait") || fastcmp(cv_value, "CS Portraits"))
				cv_value = "C.S.S Portraits";
			else if (!stricmp(cv_value, "S. CS Portraits"))
				cv_value = "Super C.S.S";
			else if (!stricmp(cv_value, "S. Continue Sprites") || !stricmp(cv_value, "Super C.S"))
				cv_value = "Super Continue Sprites";
			else if (!stricmp(cv_value, "S. Life Icon Sprites") || !stricmp(cv_value, "Super L.I.S"))
				cv_value = "Super Life Icon Sprites";
			break;
		case discordcustom_characterimage_large:
		case discordcustom_characterimage_small:
			if (!stricmp(cv_value, "Default")) cv_value = "Sonic";
			break;
		case discordcustom_mapimage_large:
		case discordcustom_mapimage_small:
		{
			static const char *const map_image_table_old[] = {
				"BS - ",
				"CS - ",
				"SSS - ",
				"NBS - ",
				"MATCH - ",
				NULL
			};
			static const char *const map_image_table_new[] = {
				"EX - ",
				"ADV - ",
				"NiGHTS - ",
				"NiGHTS - ",
				"RINGSLINGER - ",
				NULL
			};
			INT32 found_matching_map = TSoURDt3rd_M_FindWordInTermTable(map_image_table_old, cv_value, TSOURDT3RD_TERMTABLESEARCH_STRSTR);
			char *cpy_cv_value = NULL;

			if (!stricmp(cv_value, "Tutorial - TZ"))
			{
				cv_value = "Tutorial Zone";
				break;
			}
			else if (!stricmp(cv_value, "Custom"))
			{
				cv_value = "Custom Zone";
				break;
			}
			else if (found_matching_map < 0)
				break;

			cpy_cv_value = strchr(cv_value, '-'); // finds the initial seperator
			cpy_cv_value += 2; // skips that initial seperator
			cv_value = va("%s%s", map_image_table_new[found_matching_map], cpy_cv_value);
			break;
		}
		case discordcustom_miscimage_large:
		case discordcustom_miscimage_small:
		{
			static const char *const misc_image_table_old[] = {
				"Default",
				"Intro 1",
				"Intro 2",
				"Intro 3",
				"Intro 4",
				"Intro 5",
				"Intro 6",
				"Intro 7",
				"Intro 8",
				"Alt. Sonic Image 1",
				"Alt. Sonic Image 2",
				"Alt. Sonic Image 3",
				"Alt. Sonic Image 4",
				"Alt. Sonic Image 5",
				"Alt. Tails Image 1",
				"Alt. Tails Image 2",
				"Alt. Knuckles Image 1",
				"Alt. Knuckles Image 2",
				"Alt. Amy Image 1",
				"Alt. Fang Image 1",
				"Alt. Metal Sonic Image 1",
				"Alt. Metal Sonic Image 2",
				"Alt. Eggman Image 1",
				NULL
			};
			INT32 found_matching_misc = TSoURDt3rd_M_FindWordInTermTable(misc_image_table_old, cv_value, TSOURDT3RD_TERMTABLESEARCH_NORM);

			if (found_matching_misc > -1)
				cv_value = cvar->PossibleValue[found_matching_misc].strvalue;
			break;
		}
#endif

		case tsourdt3rd_video_coloring_menus:
		case tsourdt3rd_video_coloring_fpsrate:
		case tsourdt3rd_video_coloring_tpsrate:
			if (!stricmp(cv_value, "Default")) cv_value = "Yellow";
			break;

		case tsourdt3rd_game_shadows_positioning:
			if (!stricmp(cv_value, "Yes"))
				cv_value = "Object's Position";
			else if (!stricmp(cv_value, "No"))
				cv_value = "Static";
			break;
		case tsourdt3rd_game_isitcalledsingleplayer:
			if (!stricmp(cv_value, "Yes!"))
				cv_value = "Yes";
			else if (!stricmp(cv_value, "No!"))
				cv_value = "No";
			break;

		case tsourdt3rd_audio_gameover:
			if (!stricmp(cv_value, "Default")) cv_value = "Game's Default";
			break;

		case tsourdt3rd_jukebox_hud:
			if (!stricmp(cv_value, "No")) cv_value = "On";
			break;

		default:
			break;
	}

	CV_Set(cvar, cv_value);
	return true;
}

static boolean G_TimeOver_OnChange(const char *valstr)
{
	(void)valstr;
	if (netgame)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "You can't change command 'cv_tsourdt3rd_game_allowtimeover' while in a netgame.\n");
		//CV_StealthSetValue(&cv_tsourdt3rd_game_allowtimeover, !cv_tsourdt3rd_game_allowtimeover.value);
		return false;
	}
	return true;
}

static boolean G_RealisticShadows_OnChange(const char *valstr)
{
	(void)valstr;
	if (!cv_shadow.value)
	{
		//STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "You can't change command 'cv_tsourdt3rd_game_shadows_realistic' without enabling command 'cv_shadow'.\n");
		//CV_StealthSetValue(&cv_tsourdt3rd_game_shadows_realistic, !cv_tsourdt3rd_game_shadows_realistic.value);
		return false;
	}
	return true;
}

static void G_IsItCalledSinglePlayer_OnChange(void)
{
	if (TSoURDt3rd_AprilFools_ModeEnabled()) return;
	MainMenu[0].text = (cv_tsourdt3rd_game_isitcalledsingleplayer.value ? "Single  Player" : "1  Player");
	TSoURDt3rd_AprilFools_StoreDefaultMenuStrings(); // Stores All the Default Menu Strings Again
}

static void C_PadRumble_OnChange(void)
{
	for (INT32 i = 0; i < MAXSPLITSCREENPLAYERS; i++)
		if (cv_tsourdt3rd_drrr_controls_rumble[i].value == 0)
			TSoURDt3rd_P_Pads_ResetDeviceRumble(i);
}

#ifdef STAR_LIGHTING
static void V_Coronas_OnChange(void)
{
	// Force light setup, without another test.
	for (int i = 0; i < NUMLIGHTS; i++)
		t_lspr[i]->impl_flags |= SLI_changed;
}
#endif

static void A_VapeMode_OnChange(void) { TSoURDt3rd_S_ControlMusicEffects(NULL, NULL); }

static boolean P_SuperWithShield_OnChange(const char *valstr)
{
	(void)valstr;
	if (netgame)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "You can't change command 'cv_tsourdt3rd_players_shieldblockstransformation' while in a netgame.\n");
		//CV_StealthSetValue(&cv_tsourdt3rd_players_shieldblockstransformation, !cv_tsourdt3rd_players_shieldblockstransformation.value);
		return false;
	}
	return true;
}

static void SV_UseContinues_OnChange(void) { useContinues = cv_tsourdt3rd_savefiles_limitedcontinues.value; }

static void JB_Speed_OnChange(void)
{
	if (!TSoURDt3rd_Jukebox_IsPlaying()) return;
	S_SpeedMusic(atof(cv_tsourdt3rd_jukebox_speed.string));
}

static void JB_Pitch_OnChange(void)
{
	if (!TSoURDt3rd_Jukebox_IsPlaying()) return;
	S_PitchMusic(atof(cv_tsourdt3rd_jukebox_pitch.string));
}
