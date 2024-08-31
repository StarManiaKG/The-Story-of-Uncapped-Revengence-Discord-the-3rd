// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_saveg.h
/// \brief TSoURDt3rd's custom savegame code, global header

#ifndef __SMKG_P_SAVEG__
#define __SMKG_P_SAVEG__

#include "star_vars.h"

// ------------------------ //
//        Functions
// ------------------------ //

UINT8 TSOURDT3RD_READUINT8(UINT8 *save_p, TSoURDt3rd_t *TSoURDt3rd, UINT8 fallback);
UINT32 TSOURDT3RD_READUINT32(UINT8 *save_p, TSoURDt3rd_t *TSoURDt3rd, UINT32 fallback);

void TSoURDt3rd_NetArchiveUsers(UINT8 *save_p, INT32 playernum);
void TSoURDt3rd_NetUnArchiveUsers(UINT8 *save_p, INT32 playernum);

void TSoURDt3rd_PSav_WriteExtraData(void);
void TSoURDt3rd_PSav_ReadExtraData(void);

#endif // __SMKG_P_SAVEG__
