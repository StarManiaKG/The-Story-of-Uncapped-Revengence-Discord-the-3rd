// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_statuses.c
/// \brief Discord Rich Presence statuses

#include "../doomstat.h"
#include "../m_cond.h"

// ------------------------ //
//        Functions
// ------------------------ //

void DRPC_EmblemStatus(char **string)
{
    if (netgame || splitscreen)
	    snprintf(&string, 128, "%d/%d Emblems", M_CountEmblems(serverGamedata), (numemblems + numextraemblems));
}
