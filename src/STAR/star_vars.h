// SONIC ROBO BLAST 2 - TSOURDT3RD EDITION
//-----------------------------------------------------------------------------
// Copyright (C) 2023 by Star "Guy Who Named a Script After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  star_vars.h
/// \brief star variables, typically used when allowing lua support

#ifndef __STAR_VARS__
#define __STAR_VARS__

#include "../command.h"

// ============================================================================================	//
// 	 STAR Stuff																					//
//   Technically All Of It Is STAR Stuff,														//
//      But Seeing as How I Just Ported Most Of This From The Other Scripts, like m_cond.h,		//
//          I Don't Actually Care.																//
//                                                                                              //
//   Anyways, Most of This Stuff is Handled in the tsourdt3rd.pk3.                              //
//      If you Want to Look at Some Code, Check that pk3 Out.                                   //
// ============================================================================================	//

// VARIABLES //
extern boolean tsourdt3rd;

extern boolean TSoURDt3rd_TouchyModifiedGame;
extern boolean TSoURDt3rd_LoadExtras;
extern boolean TSoURDt3rd_LoadedExtras;
extern boolean TSoURDt3rd_NoMoreExtras;

extern boolean checkedExtraWads;

#ifdef HAVE_CURL
extern boolean GrabbingTSoURDt3rdInfo;
extern boolean NotifyAboutTSoURDt3rdUpdate;
#endif

// Sound Effects
extern INT32 STAR_JoinSFX;
extern INT32 STAR_LeaveSFX;
extern INT32 STAR_SynchFailureSFX;

extern INT32 DISCORD_RequestSFX;

// Server
extern INT32 reachedSockSendErrorLimit;

// Time Over...
extern const char gameoverMusic[7][7];
extern const INT32 gameoverMusicTics[7];

extern boolean timeover;
extern boolean ForceTimeOver;

// TF2
extern boolean SpawnTheDispenser;

// Easter (STAR NOTE: Most of the Other Egg Stuff is Handled in tsourdt3rd.pk3, Just so You Know :p) //
extern INT32 TOTALEGGS;

extern INT32 foundeggs;
extern INT32 collectedmapeggs;
extern INT32 currenteggs;
extern INT32 numMapEggs;

// COMMANDS //
extern consvar_t cv_soniccd;
extern consvar_t cv_tsourdt3rdupdatemessage;
extern consvar_t cv_socksendlimit;

// FUNCTIONS //
// Savedata
void STAR_WriteExtraData(void);
void STAR_ReadExtraData(void);

void STAR_SetSavefileProperties(void);

// Messages
void STAR_Tsourdt3rdEventMessage(INT32 choice);

// Online
#ifdef HAVE_CURL
void STAR_FindAPI(const char *API);
void STAR_GrabFromTsourdt3rdGithub(char *URL);
#endif

#endif // __STAR_VARS__