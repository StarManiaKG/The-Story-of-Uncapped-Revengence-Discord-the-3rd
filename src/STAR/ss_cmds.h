
#ifndef __SS_CMDS__
#define __SS_CMDS__

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

extern consvar_t cv_stjrintro;
extern consvar_t cv_tpsrate;
extern consvar_t cv_menucolor, cv_fpscountercolor, cv_tpscountercolor;
extern consvar_t cv_storesavesinfolders;

extern consvar_t cv_allowtypicaltimeover;
#define TSOURDT3RD_TIMELIMIT (20999) // one tic off so the timer doesn't display 10:00.00

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

extern consvar_t cv_continues;

// =======
// SERVERS
// =======

extern consvar_t cv_movingplayersetup;

// ------------------------ //
//        Functions
// ------------------------ //

// ====
// GAME
// ====

void STAR_TPSRate_OnChange(void);

#endif // __SS_CMDS__