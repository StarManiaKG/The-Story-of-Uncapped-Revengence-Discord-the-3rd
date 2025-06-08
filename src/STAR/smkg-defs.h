// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-defs.h
/// \brief BIOS definition data for TSoURDt3rd

#ifndef __SMKG_DEFS__
#define __SMKG_DEFS__

#include "../doomdef.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

//#define TSOURDT3RD_DEBUGGING /* Debugging */

#define AUTOLOADCONFIGFILENAME "autoload.cfg"

#ifdef ALAM_LIGHTING
#define STAR_LIGHTING /* EXTREME BETA: Fix up ALAM_LIGHTING thanks to touch-ups by Star */
#endif

//#define _DEBUG /* Debugging */
//#define DEBUGFILE /* Debugging */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_DEFS__
