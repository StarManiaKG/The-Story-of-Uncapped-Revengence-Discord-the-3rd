
#ifndef __SMKG_CVARS__
#define __SMKG_CVARS__

#include "../command.h"

// ------------------------ //
//        Variables
// ------------------------ //

// ====
// MAIN
// ====

extern consvar_t cv_tsourdt3rd_main_checkforupdatesonstartup;

// ======
// EVENTS
// ======

extern consvar_t cv_easter_allowegghunt, cv_easter_egghuntbonuses;
extern consvar_t cv_tsourdt3rd_aprilfools_ultimatemode;

// ====
// GAME
// ====

extern consvar_t cv_startupscreen, cv_stjrintro;
extern consvar_t cv_tsourdt3rd_game_isitcalledsingleplayer;
extern consvar_t cv_allowtypicaltimeover;
extern consvar_t cv_automapoutsidedevmode;
extern consvar_t cv_tsourdt3rd_game_soniccd;

// =====
// VIDEO
// =====

extern consvar_t cv_tsourdt3rd_video_showtps;
extern consvar_t cv_menucolor, cv_fpscountercolor, cv_tpscountercolor;

// =====
// AUDIO
// =====

extern consvar_t cv_tsourdt3rd_audio_watermuffling;

// =======
// PLAYERS
// =======

extern consvar_t cv_shieldblockstransformation;
extern consvar_t cv_alwaysoverlayinvuln;

// =========
// SAVEFILES
// =========

extern consvar_t cv_tsourdt3rd_savefiles_storesavesinfolders;
extern consvar_t cv_tsourdt3rd_savefiles_perfectsave;
extern consvar_t cv_tsourdt3rd_savefiles_perfectsavestripe1, cv_tsourdt3rd_savefiles_perfectsavestripe2, cv_tsourdt3rd_savefiles_perfectsavestripe3;
extern consvar_t cv_continues;

// =======
// SERVERS
// =======

extern consvar_t cv_rendezvousserver;
extern consvar_t cv_movingplayersetup;

// =====
// DEBUG
// =====

extern consvar_t cv_tsourdt3rd_debug_drrr_virtualkeyboard;

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_D_RegisterServerCommands(void);

#endif // __SMKG_CVARS__