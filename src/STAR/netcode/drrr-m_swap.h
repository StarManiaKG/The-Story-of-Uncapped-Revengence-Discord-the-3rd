// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  drrr-m_swap.h
/// \brief Extended endianess handling, swapping 16bit and 32bit

#ifndef __DRRR_M_SWAP__
#define __DRRR_M_SWAP__

#include "../../endian.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP_SHORT(x) ((INT16)(\
(((UINT16)(x) & (UINT16)0x00ffU) << 8) \
| \
(((UINT16)(x) & (UINT16)0xff00U) >> 8))) \

#define SWAP_LONG(x) ((INT32)(\
(((UINT32)(x) & (UINT32)0x000000ffUL) << 24) \
| \
(((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) \
| \
(((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) \
| \
(((UINT32)(x) & (UINT32)0xff000000UL) >> 24)))

// Endianess handling.
// WAD files are stored little endian.
#ifdef SRB2_BIG_ENDIAN
#define MSBF_SHORT(x) ((INT16)(x))
#define MSBF_LONG(x) ((INT32)(x))
#else
#define MSBF_SHORT SWAP_SHORT
#define MSBF_LONG SWAP_LONG
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __DRRR_M_SWAP__
