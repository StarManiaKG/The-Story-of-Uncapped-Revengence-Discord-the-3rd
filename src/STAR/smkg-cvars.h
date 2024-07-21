
#ifndef __SMKG_CVARS__
#define __SMKG_CVARS__

#include "../command.h"

// ------------------------ //
//        Variables
// ------------------------ //

// ======
// EVENTS
// ======

extern consvar_t cv_easter_allowegghunt, cv_easter_egghuntbonuses;
extern consvar_t cv_aprilfools_ultimatemode;

// ====
// GAME
// ====

extern consvar_t cv_startupscreen, cv_stjrintro;
extern consvar_t cv_tpsrate;
extern consvar_t cv_menucolor, cv_fpscountercolor, cv_tpscountercolor;
extern consvar_t cv_allowtypicaltimeover;
extern consvar_t cv_automapoutsidedevmode;
extern consvar_t cv_soniccd;

// =====
// AUDIO
// =====

extern consvar_t cv_watermuffling;

// =======
// PLAYERS
// =======

extern consvar_t cv_shieldblockstransformation;
extern consvar_t cv_alwaysoverlayinvuln;

// =========
// SAVEFILES
// =========

extern consvar_t cv_storesavesinfolders;
extern consvar_t cv_continues;

// =======
// SERVERS
// =======

extern consvar_t cv_movingplayersetup;

// =====
// DEBUG
// =====

extern consvar_t cv_tsourdt3rd_drrr_debug_virtualkeyboard;

// ------------------------ //
//        Functions
// ------------------------ //

// ====
// GAME
// ====

void STAR_TPSRate_OnChange(void);

#endif // __SMKG_CVARS__