// SONIC ROBO BLAST 2 - TSOURDT3RD EDITION
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by Star "Guy Who Named a Script After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_vars.h
/// \brief star variables, typically used when allowing lua support or just for dumb stuff

#ifndef __STAR_VARS__
#define __STAR_VARS__

#include "star_webinfo.h"
#include "../command.h"

// ============================================================================================	//
// 	 STAR Stuff																					//
//   Technically All Of It Is STAR Stuff,														//
//      But Seeing as How I Just Ported Most Of This From The Other Scripts, like m_cond.h,		//
//          I Don't Actually Care.																//
//                                                                                              //
//   Anyways, Most of This Stuff is Handled in the tsourdt3rd.pk3.                              //
//      If you Want to Look at Some Code, Check that PK3 Out.                                   //
// ============================================================================================	//

//// DEFINITIONS ////
#ifdef DEVELOP
#define TSOURDT3RDBYSTARMANIAKGSTRING "(By StarManiaKG#4884); Dev Mode Edition"
#else
#define TSOURDT3RDBYSTARMANIAKGSTRING "(By StarManiaKG#4884)"
#endif
#define TSOURDT3RDVERSIONSTRING "TSoURDt3rd v"TSOURDT3RDVERSION

//// STRUCTS ////
extern struct TSoURDt3rdInfo_s {
	// General Stuff
	boolean checkedVersion;
	INT32 reachedSockSendErrorLimit;

	// Server Stuff
	boolean alreadyWarnedPlayer;

	boolean serverUsesTSoURDt3rd;
	UINT8 majorVersion, minorVersion, subVersion;

	INT32 serverTSoURDt3rdVersion;
} TSoURDt3rdInfo;

//// VARIABLES ////
// TSoURDt3rd Stuff //
extern boolean tsourdt3rd;

extern boolean TSoURDt3rd_TouchyModifiedGame;
extern boolean TSoURDt3rd_LoadExtras;
extern boolean TSoURDt3rd_LoadedExtras;
extern boolean TSoURDt3rd_NoMoreExtras;

extern boolean TSoURDt3rd_checkedExtraWads;

// Sound Effects //
extern INT32 STAR_JoinSFX;
extern INT32 STAR_LeaveSFX;
extern INT32 STAR_SynchFailureSFX;

extern INT32 DISCORD_RequestSFX;

// Game //
extern size_t ls_count;
extern UINT8 ls_percent;
extern INT32 STAR_loadingscreentouse;

extern const char gameoverMusic[7][7];
extern const INT32 gameoverMusicTics[7];

extern boolean timeover;
extern boolean ForceTimeOver;

// Extras //
// TF2
extern boolean SpawnTheDispenser;

// Easter (STAR NOTE: Most of the Other Egg Stuff is Handled in tsourdt3rd.pk3, Just so You Know :p) //
extern INT32 TOTALEGGS;

extern INT32 foundeggs;
extern INT32 collectedmapeggs;
extern INT32 currenteggs;
extern INT32 numMapEggs;

//// COMMANDS ////
extern consvar_t cv_loadingscreen, cv_loadingscreenimage, cv_soniccd;
extern consvar_t cv_tsourdt3rdupdatemessage;
extern consvar_t cv_socksendlimit;

//// FUNCTIONS ////
// Game
void STAR_LoadingScreen(boolean opengl);

// Savedata
void STAR_WriteExtraData(void);
void STAR_ReadExtraData(void);

void STAR_SetSavefileProperties(void);

// Files
INT32 STAR_DetectFileType(const char* filename);

// Events
void TSoURDt3rd_CheckTime(void);

// Messages
void TSoURDt3rd_EventMessage(INT32 choice);

// The World Wide Web
#ifdef HAVE_CURL
void STAR_FindAPI(const char *API);

INT32 STAR_FindStringOnWebsite(const char *API, char *URL, char *INFO, boolean verbose);
char *STAR_ReturnStringFromWebsite(const char *API, char *URL, char *RETURNINFO, boolean verbose);
#endif

// Servers
boolean STAR_FindServerInfractions(void);

// Miscellanious
UINT32 TSoURDt3rd_CurrentVersion(void);

UINT8 TSoURDt3rd_CurrentMajorVersion(void);
UINT8 TSoURDt3rd_CurrentMinorVersion(void);
UINT8 TSoURDt3rd_CurrentSubversion(void);

INT32 STAR_ConvertStringToCompressedNumber(char *STRING, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionNumber);
char *STAR_ConvertNumberToString(INT32 NUMBER, INT32 startIFrom, INT32 startJFrom, boolean turnIntoVersionString);
INT32 STAR_ConvertNumberToStringAndBack(INT32 NUMBER, INT32 startI1From, INT32 startJ1From, INT32 startI2From, INT32 startJ2From, boolean turnIntoVersionString, boolean turnIntoVersionNumber);

INT32 STAR_CombineNumbers(INT32 ARGS, INT32 FIRSTNUM, ...);

#ifdef HAVE_SDL
void STAR_RenameWindow(const char *title);
const char *STAR_SetWindowTitle(void);
#endif

#endif // __STAR_VARS__