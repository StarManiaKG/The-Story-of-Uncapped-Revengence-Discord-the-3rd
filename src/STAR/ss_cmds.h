
#ifndef __SS_CMDS__
#define __SS_CMDS__

#include "../command.h"

// ------------------------ //
//       Definitions
// ------------------------ //
// ====
// Game
// ====
// one tic off so the timer doesn't display 10:00.00
#define TSOURDT3RD_TIMELIMIT (20999)

// ------------------------ //
//        Commands
// ------------------------ //
// ======
// Events
// ======
// events
extern consvar_t cv_easter_allowegghunt, cv_easter_egghuntbonuses;
extern consvar_t cv_aprilfools_ultimatemode;

// ====
// Game
// ====
extern consvar_t cv_tpsrate;
extern consvar_t cv_menucolor, cv_fpscountercolor, cv_tpscountercolor;
extern consvar_t cv_storesavesinfolders;
extern consvar_t cv_allowtypicaltimeover;
extern consvar_t cv_soniccd;

// =====
// Audio
// =====
extern consvar_t cv_watermuffling;

// =======
// Players
// =======
extern consvar_t cv_shieldblockstransformation;
extern consvar_t cv_alwaysoverlayinvuln;

// =========
// Savefiles
// =========
extern consvar_t cv_continues;

// =======
// Servers
// =======
extern consvar_t cv_movingplayersetup;

// ------------------------ //
//        Functions
// ------------------------ //
void TSoURDt3rd_DiscordCommands_OnChange(void);

// ====
// Game
// ====
void STAR_TPSRate_OnChange(void);

#endif // __SS_CMDS__