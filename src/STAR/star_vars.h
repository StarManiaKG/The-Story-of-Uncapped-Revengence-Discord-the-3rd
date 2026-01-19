// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_vars.h
/// \brief star variables, typically used when allowing lua support or just for making and doing dumb stuff

/// \todo remove this entire file eventually, spread everything out :)

#ifndef __STAR_VARS__
#define __STAR_VARS__

#include "ss_main.h" // main star varaibles
#include "star_webinfo.h" // star variables
#include "monocypher/smkg-m_hash.h" // Crypto/RRID primitives //

// ============================================================================	//																//
//   Just so you know, some of this stuff is also handled in tsourdt3rd.pk3.	//
//      If you want to look at some other code, check that PK3 out.             //
// ============================================================================ //

//// DEFINITIONS ////
#define TSOURDT3RDVERSIONSTRING "TSoURDt3rd v"TSOURDT3RDVERSION

//// STRUCTS ////
typedef struct TSoURDt3rd_s {
	// TSoURDt3rd authentication
	uint8_t public_key[PUBKEYLENGTH];
	uint8_t secret_key[PRIVKEYLENGTH];
	char *user_hash;
	// Everything else
	boolean usingTSoURDt3rd;
	boolean server_usingTSoURDt3rd;
	SINT8   server_majorVersion;
	SINT8   server_minorVersion;
	SINT8   server_subVersion;
	UINT8   server_TSoURDt3rdVersion;
} TSoURDt3rd_t;
extern TSoURDt3rd_t TSoURDt3rdPlayers[MAXPLAYERS];

//// VARIABLES ////
// Audio //
#if 1
// MARKED FOR REMOVAL
// Game Over Music
extern const char gameoverMusic[9][7];
extern INT32 gameoverMusicTics[9];
#endif
// Star SFX
extern INT32 STAR_JoinSFX;
extern INT32 STAR_LeaveSFX;
extern INT32 STAR_SynchFailureSFX;

// Servers //
extern INT32 STAR_ServerToExtend;

// Extras //
// TF2
extern boolean SpawnTheDispenser;

//// FUNCTIONS ////
// Events //
void TSoURDt3rd_CheckTime(void);

// Game //
void TSoURDt3rd_InitializePlayer(INT32 playernum);
void TSoURDt3rd_ClearPlayer(INT32 playernum);
void STAR_LoadingScreen(void);

// SDL //
#ifdef HAVE_SDL
void STAR_RenameWindow(const char *title);
#endif
const char *STAR_SetWindowTitle(void);

#if 1
// MARKED FOR REMOVAL
// Audio //
const char *TSoURDt3rd_DetermineLevelMusic(void);
#endif

// Miscellanious //
// -- Version
UINT32 TSoURDt3rd_CurrentVersion(void);

#endif // __STAR_VARS__
