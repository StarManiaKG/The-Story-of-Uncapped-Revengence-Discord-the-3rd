// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2025-2026 by Ronald "Eidolon" Kinard.
// Copyright (C) 2025-2026 by Kart Krew.
// Copyright (C) 1999-2026 by Sonic Team Junior.
// Copyright (C) 2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  g_gamedata.h
/// \brief Gamedata writing and handling.

#ifndef __G_GAMEDATA__
#define __G_GAMEDATA__

#include "m_cond.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// GAMEDATA STRUCTURE
// Can be found in m_cond.h
/// \todo Put here eventually
//

typedef struct
{
	gamedata_t **data;
	UINT32 num_data;
} gamedatalist_t;

extern gamedata_t *clientGamedata;
extern gamedata_t *serverGamedata;
extern gamedatalist_t *allClientGamedata;

gamedata_t *G_NewGameDataStruct(void);
gamedatalist_t *G_NewGameDataList(void);
void G_FreeGameDataList(gamedatalist_t *data_list);

void G_CopyGameData(gamedata_t *dest, gamedata_t *src);

boolean G_AddGameDataToList(gamedatalist_t *data_list, gamedata_t *data);
boolean G_StoreGameData(gamedatalist_t *data_list, gamedata_t *data);
boolean G_StoreGameDataByIndex(gamedatalist_t *data_list, gamedata_t *data, UINT32 dindex);

void G_SaveGameData(gamedata_t *data);
void G_LoadGameData(gamedata_t *data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __G_GAMEDATA__
