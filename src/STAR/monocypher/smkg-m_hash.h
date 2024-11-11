// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-m_hash.h
/// \brief TSoURDt3rd's hashing routines, global header

#ifndef __SMKG_M_HASH__
#define __SMKG_M_HASH__

#include "monocypher.h"
#include "../../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

// Crypto/RRID primitives
#define PUBKEYLENGTH 32 // Enforced by Monocypher EdDSA

// ------------------------ //
//        Functions
// ------------------------ //

char *TSoURDt3rd_Hash_GenerateFromID(const unsigned char *bin, boolean brief);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_M_HASH__
