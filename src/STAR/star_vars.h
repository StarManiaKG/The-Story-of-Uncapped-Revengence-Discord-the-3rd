// SONIC ROBO BLAST 2
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

// Main Stuff //
extern boolean tsourdt3rd;

// Commands //
extern consvar_t cv_soniccd;

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

#endif // __STAR_VARS__