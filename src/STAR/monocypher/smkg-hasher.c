// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-hasher.c
/// \brief TSoURDt3rd's hashing routines, supported by monocrypt

#include "smkg-hasher.h"
#include "monocypher.h"

#include "../../discord/stun.h"

// ------------------------ //
//        Functions
// ------------------------ //

char *TSoURDt3rd_Hash_GenerateFromID(const unsigned char *bin, boolean brief)
{
	size_t i;
	size_t len = PUBKEYLENGTH;
	static char rrid_buf[256];

	if (brief)
		len = 8;

	if (bin == NULL || len == 0)
		return NULL;

	for (i=0; i<len; i++)
	{
		rrid_buf[i*2]   = "0123456789ABCDEF"[bin[i] >> 4];
		rrid_buf[i*2+1] = "0123456789ABCDEF"[bin[i] & 0x0F];
	}

	rrid_buf[len*2] = '\0';

	return rrid_buf;
}
