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
/// \file  g_gamedata.c
/// \brief Gamedata saving, loading, storing, and handling.

#include "g_gamedata.h"
#include "g_game.h"
#include "d_main.h"
#include "m_misc.h"
#include "m_argv.h"
#include "p_local.h"
#include "z_zone.h"
#include "fastcmp.h"

// TSoURDt3rd
#include "STAR/core/smkg-g_game.h"

gamedata_t *clientGamedata;        // Our gamedata
gamedata_t *serverGamedata;        // Server's gamedata
gamedatalist_t *allClientGamedata; // All client gamedata

#define GAMEDATA_ID 0x86E4A27C // Change every major version, as usual
#define COMPAT_GAMEDATA_ID 0xFCAFE211 // TODO: 2.3: Delete

// G_NewGameDataStruct
// Create a new gamedata_t, for start-up
gamedata_t *G_NewGameDataStruct(void)
{
	gamedata_t *data = Z_Calloc(sizeof (*data), PU_STATIC, NULL);
	M_ClearSecrets(data);
	G_ClearRecords(data);
	return data;
}

// G_NewGameDataList
// Create a new gamedata_t list, for storing gamedata.
gamedatalist_t *G_NewGameDataList(void)
{
	gamedatalist_t *data_list = Z_Calloc(sizeof (*data_list), PU_STATIC, NULL);
	if (data_list == NULL)
	{
		return NULL;
	}
	if (!(data_list->data = Z_Malloc(sizeof (*data_list->data), PU_STATIC, NULL)))
	{
		G_FreeGameDataList(data_list);
		return NULL;
	}
	data_list->num_data = 0;
	return data_list;
}

// G_FreeGameDataList
// Clears the given gamedata_t list.
void G_FreeGameDataList(gamedatalist_t *data_list)
{
	INT32 i;
	if (data_list != NULL)
	{
		if (data_list->num_data > 0)
		{
			for (i = (INT32)data_list->num_data-1; i >= 0; --i)
			{
				if (data_list->data[i] != NULL)
				{
					M_ClearSecrets(data_list->data[i]);
					G_ClearRecords(data_list->data[i]);
					Z_Free(data_list->data[i]->filename);
					Z_Free(data_list->data[i]);
					data_list->data[i] = NULL;
				}
			}
		}
		Z_Free(data_list->data);
		Z_Free(data_list);
	}
	data_list = NULL;
}

// G_AddGameDataToList
// Adds gamedata to the given gamedata list, updates memory accordingly.
boolean G_AddGameDataToList(gamedatalist_t *data_list, gamedata_t *data)
{
	UINT32 gdindex = 0;
	UINT32 i = 0;

	if (data_list == NULL)
	{
		return false;
	}

	if (data == NULL)
	{
		return false;
	}

	while (gdindex < data_list->num_data)
	{
		if (fastncmp(data_list->data[gdindex]->filename, gamedatafilename, sizeof(gamedatafilename)))
		{
			CONS_Debug(DBG_SETUP, M_GetText("WARNING: Gamedata matching '%s' already exists! Updating data there instead.\n"), data_list->data[gdindex]->filename);
			break;
		}
		++gdindex;
	}

	if (gdindex >= data_list->num_data)
	{
		data_list->num_data++;
		data_list->data = Z_Realloc(data_list->data, sizeof(*data_list->data) * data_list->num_data, PU_STATIC, NULL);
		data_list->data[gdindex] = G_NewGameDataStruct();
		CONS_Debug(DBG_SETUP, M_GetText("NOTICE: Gamedata added to storage list. New gamedata storage count is %d.\n"), data_list->num_data);
	}
	G_CopyGameData(data_list->data[gdindex], data);

	if (cv_debug & DBG_SETUP)
	{
		CONS_Debug(DBG_SETUP, M_GetText("\nCurrent gamedata in list:\n"));
		while (i < data_list->num_data)
		{
			CONS_Debug(DBG_SETUP, "%s\n", data_list->data[i++]->filename);
		}
		CONS_Debug(DBG_SETUP, M_GetText("\n"));
	}

	return true;
}

// G_CopyGameData
// Fully copies all gamedata from one struct to another.
void G_CopyGameData(gamedata_t *dest, gamedata_t *src)
{
	INT32 i, j;

	M_ClearSecrets(dest);
	G_ClearRecords(dest);

	dest->loaded = src->loaded;
	dest->filename = src->filename;
	dest->totalplaytime = src->totalplaytime;

	dest->timesBeaten = src->timesBeaten;
	dest->timesBeatenWithEmeralds = src->timesBeatenWithEmeralds;
	dest->timesBeatenUltimate = src->timesBeatenUltimate;

	memcpy(dest->achieved, src->achieved, sizeof(dest->achieved));
	memcpy(dest->collected, src->collected, sizeof(dest->collected));
	memcpy(dest->extraCollected, src->extraCollected, sizeof(dest->extraCollected));
	memcpy(dest->unlocked, src->unlocked, sizeof(dest->unlocked));

	memcpy(dest->mapvisited, src->mapvisited, sizeof(dest->mapvisited));

	// Main records
	for (i = 0; i < NUMMAPS; ++i)
	{
		if (!src->mainrecords[i])
			continue;

		G_AllocMainRecordData((INT16)i, dest);
		dest->mainrecords[i]->score = src->mainrecords[i]->score;
		dest->mainrecords[i]->time = src->mainrecords[i]->time;
		dest->mainrecords[i]->rings = src->mainrecords[i]->rings;
	}

	// Nights records
	for (i = 0; i < NUMMAPS; ++i)
	{
		if (!src->nightsrecords[i] || !src->nightsrecords[i]->nummares)
			continue;

		G_AllocNightsRecordData((INT16)i, dest);

		for (j = 0; j < (src->nightsrecords[i]->nummares + 1); j++)
		{
			dest->nightsrecords[i]->score[j] = src->nightsrecords[i]->score[j];
			dest->nightsrecords[i]->grade[j] = src->nightsrecords[i]->grade[j];
			dest->nightsrecords[i]->time[j] = src->nightsrecords[i]->time[j];
		}

		dest->nightsrecords[i]->nummares = src->nightsrecords[i]->nummares;
	}
}

static boolean store_game_data(gamedatalist_t *data_list, gamedata_t *data, UINT32 gdindex)
{
	if (data_list->num_data == 0)
	{
		CONS_Debug(DBG_SETUP, M_GetText("ERROR: No available slot to store gamedata into!\n"));
		return false;
	}
	else if (gdindex >= data_list->num_data)
	{
		CONS_Debug(DBG_SETUP, M_GetText("ERROR: Gamedata slot '%d' is out of index bounds!\n"), gdindex);
		return false;
	}
	else if (data_list->data[gdindex] == NULL)
	{
		CONS_Debug(DBG_SETUP, M_GetText("ERROR: Gamedata slot '%d' doesn't exist!\n"), gdindex);
		return false;
	}

	G_CopyGameData(data_list->data[gdindex], data);
	CONS_Debug(DBG_SETUP, M_GetText("NOTICE: Stored gamedata updated.\n"));
	return true;
}

// G_StoreGameData
// Saves gamedata into a stored slot in the given array.
boolean G_StoreGameData(gamedatalist_t *data_list, gamedata_t *data)
{
	if (data_list == NULL)
	{
		return false;
	}

	if (data == NULL)
	{
		return false;
	}

	return store_game_data(data_list, data, data_list->num_data-1);
}

// G_StoreGameDataByIndex
// Stores gamedata in the given index of the given gamedata list.
boolean G_StoreGameDataByIndex(gamedatalist_t *data_list, gamedata_t *data, UINT32 gdindex)
{
	if (data_list == NULL)
	{
		return false;
	}

	if (data == NULL)
	{
		return false;
	}

	return store_game_data(data_list, data, gdindex);
}

// G_LoadGameData
// Loads the main data file, which stores information such as emblems found, etc.
void G_LoadGameData(gamedata_t *data)
{
	save_t savebuffer;
	INT32 i, j;

	UINT32 versionID;
	UINT8 rtemp;

	//For records
	UINT32 recscore;
	tic_t  rectime;
	UINT16 recrings;

	UINT8 recmares;
	INT32 curmare;

	TSoURDt3rd_G_LoadGameData(); // STAR STUFF: STEAL SAVEFILE DATA //

	// Stop saving, until we successfully load it again.
	data->loaded = false;
	data->filename = Z_StrDup(gamedatafilename);

	// Backwards compat stuff
	INT32 max_emblems = MAXEMBLEMS;
	INT32 max_extraemblems = MAXEXTRAEMBLEMS;
	INT32 max_unlockables = MAXUNLOCKABLES;
	INT32 max_conditionsets = MAXCONDITIONSETS;

	// Clear things so previously read gamedata doesn't transfer
	// to new gamedata
	G_ClearRecords(data); // main and nights records
	M_ClearSecrets(data); // emblems, unlocks, maps visited, etc
	data->totalplaytime = 0; // total play time (separate from all)

	if (M_CheckParm("-nodata"))
	{
		// Don't load at all.
		return;
	}

	if (M_CheckParm("-resetdata"))
	{
		// Don't load, but do save. (essentially, reset)
		data->loaded = true;
		return;
	}

	savebuffer.size = FIL_ReadFile(va(pandf, srb2home, gamedatafilename), &savebuffer.buf);
	if (!savebuffer.size)
	{
		// No gamedata. We can save a new one.
		data->loaded = true;
		return;
	}

	savebuffer.pos = 0;

	// Version check
	versionID = P_ReadUINT32(&savebuffer);
	if (versionID != GAMEDATA_ID
#ifdef COMPAT_GAMEDATA_ID // backwards compat behavior
		&& versionID != COMPAT_GAMEDATA_ID
#endif
		)
	{
		const char *gdfolder = "the SRB2 folder";
		if (strcmp(srb2home,"."))
			gdfolder = srb2home;

		Z_Free(savebuffer.buf);
		I_Error("Game data is from another version of SRB2.\nDelete %s(maybe in %s) and try again.", gamedatafilename, gdfolder);
	}

#ifdef COMPAT_GAMEDATA_ID // Account for lower MAXUNLOCKABLES and MAXEXTRAEMBLEMS from older versions
	if (versionID == COMPAT_GAMEDATA_ID)
	{
		max_extraemblems = 16;
		max_unlockables = 32;
	}
#endif

	data->totalplaytime = P_ReadUINT32(&savebuffer);

#ifdef COMPAT_GAMEDATA_ID
	if (versionID == COMPAT_GAMEDATA_ID)
	{
		// We'll temporarily use the old condition when loading an older file.
		// The proper mod-specific hash will get saved in afterwards.
		boolean modded = P_ReadUINT8(&savebuffer);

		if (modded && !savemoddata)
		{
			goto datacorrupt;
		}
		else if (modded != true && modded != false)
		{
			goto datacorrupt;
		}

		// make a backup of the old data
		char currentfilename[64];
		char backupfilename[69];
		char bak[5];

		strcpy(bak, ".bak");
		strcpy(currentfilename, gamedatafilename);
		STRBUFCPY(backupfilename, strcat(currentfilename, bak));

		FIL_WriteFile(va(pandf, srb2home, backupfilename), &savebuffer.buf, savebuffer.size);
	}
	else
#endif
	{
		// Quick & dirty hash for what mod this save file is for.
		UINT32 modID = P_ReadUINT32(&savebuffer);
		UINT32 expectedID = quickncasehash(timeattackfolder, sizeof timeattackfolder);

		if (modID != expectedID)
		{
			// Aha! Someone's been screwing with the save file!
			goto datacorrupt;
		}
	}

	// TODO put another cipher on these things? meh, I don't care...
	for (i = 0; i < NUMMAPS; i++)
		if ((data->mapvisited[i] = P_ReadUINT8(&savebuffer)) > MV_MAX)
			goto datacorrupt;

	// To save space, use one bit per collected/achieved/unlocked flag
	for (i = 0; i < max_emblems;)
	{
		rtemp = P_ReadUINT8(&savebuffer);
		for (j = 0; j < 8 && j+i < max_emblems; ++j)
			data->collected[j+i] = ((rtemp >> j) & 1);
		i += j;
	}
	for (i = 0; i < max_extraemblems;)
	{
		rtemp = P_ReadUINT8(&savebuffer);
		for (j = 0; j < 8 && j+i < max_extraemblems; ++j)
			data->extraCollected[j+i] = ((rtemp >> j) & 1);
		i += j;
	}
	for (i = 0; i < max_unlockables;)
	{
		rtemp = P_ReadUINT8(&savebuffer);
		for (j = 0; j < 8 && j+i < max_unlockables; ++j)
			data->unlocked[j+i] = ((rtemp >> j) & 1);
		i += j;
	}
	for (i = 0; i < max_conditionsets;)
	{
		rtemp = P_ReadUINT8(&savebuffer);
		for (j = 0; j < 8 && j+i < max_conditionsets; ++j)
			data->achieved[j+i] = ((rtemp >> j) & 1);
		i += j;
	}

	data->timesBeaten = P_ReadUINT32(&savebuffer);
	data->timesBeatenWithEmeralds = P_ReadUINT32(&savebuffer);
	data->timesBeatenUltimate = P_ReadUINT32(&savebuffer);

	// Main records
	for (i = 0; i < NUMMAPS; ++i)
	{
		recscore = P_ReadUINT32(&savebuffer);
		rectime  = (tic_t)P_ReadUINT32(&savebuffer);
		recrings = P_ReadUINT16(&savebuffer);
		P_ReadUINT8(&savebuffer); // compat

		if (recrings > 10000 || recscore > MAXSCORE)
			goto datacorrupt;

		if (recscore || rectime || recrings)
		{
			G_AllocMainRecordData((INT16)i, data);
			data->mainrecords[i]->score = recscore;
			data->mainrecords[i]->time = rectime;
			data->mainrecords[i]->rings = recrings;
		}
	}

	// Nights records
	for (i = 0; i < NUMMAPS; ++i)
	{
		if ((recmares = P_ReadUINT8(&savebuffer)) == 0)
			continue;

		G_AllocNightsRecordData((INT16)i, data);

		for (curmare = 0; curmare < (recmares+1); ++curmare)
		{
			data->nightsrecords[i]->score[curmare] = P_ReadUINT32(&savebuffer);
			data->nightsrecords[i]->grade[curmare] = P_ReadUINT8(&savebuffer);
			data->nightsrecords[i]->time[curmare] = (tic_t)P_ReadUINT32(&savebuffer);

			if (data->nightsrecords[i]->grade[curmare] > GRADE_S)
			{
				goto datacorrupt;
			}
		}

		data->nightsrecords[i]->nummares = recmares;
	}

	// done
	Z_Free(savebuffer.buf);

	// Don't consider loaded until it's a success!
	// It used to do this much earlier, but this would cause the gamedata to
	// save over itself when it I_Errors from the corruption landing point below,
	// which can accidentally delete players' legitimate data if the code ever has any tiny mistakes!
	data->loaded = true;

	// Silent update unlockables in case they're out of sync with conditions
	M_SilentUpdateUnlockablesAndEmblems(data);
	M_SilentUpdateSkinAvailabilites();

	return;

	// Landing point for corrupt gamedata
	datacorrupt:
	{
		const char *gdfolder = "the SRB2 folder";
		if (strcmp(srb2home,"."))
			gdfolder = srb2home;

		Z_Free(savebuffer.buf);

		I_Error("Corrupt game data file.\nDelete %s(maybe in %s) and try again.", gamedatafilename, gdfolder);
	}
}

// G_SaveGameData
// Saves the main data file, which stores information such as emblems found, etc.
void G_SaveGameData(gamedata_t *data)
{
	save_t savebuffer;

	INT32 i, j;
	UINT8 btemp;

	INT32 curmare;

	TSoURDt3rd_G_SaveGamedata(); // STAR STUFF: VIVA LA AUTOLOADING //

	if (!data)
		return; // data struct not valid

	if (!data->loaded)
		return; // If never loaded (-nodata), don't save

	savebuffer.size = GAMEDATASIZE;
	savebuffer.buf = (UINT8 *)malloc(savebuffer.size);
	if (!savebuffer.buf)
	{
		CONS_Alert(CONS_ERROR, M_GetText("No more free memory for saving game data\n"));
		return;
	}
	savebuffer.pos = 0;

	if (usedCheats)
	{
		free(savebuffer.buf);
		return;
	}

	// Version test
	P_WriteUINT32(&savebuffer, GAMEDATA_ID);

	P_WriteUINT32(&savebuffer, data->totalplaytime);

	P_WriteUINT32(&savebuffer, quickncasehash(timeattackfolder, sizeof timeattackfolder));

	// TODO put another cipher on these things? meh, I don't care...
	for (i = 0; i < NUMMAPS; i++)
		P_WriteUINT8(&savebuffer, (data->mapvisited[i] & MV_MAX));

	// To save space, use one bit per collected/achieved/unlocked flag
	for (i = 0; i < MAXEMBLEMS;)
	{
		btemp = 0;
		for (j = 0; j < 8 && j+i < MAXEMBLEMS; ++j)
			btemp |= (data->collected[j+i] << j);
		P_WriteUINT8(&savebuffer, btemp);
		i += j;
	}
	for (i = 0; i < MAXEXTRAEMBLEMS;)
	{
		btemp = 0;
		for (j = 0; j < 8 && j+i < MAXEXTRAEMBLEMS; ++j)
			btemp |= (data->extraCollected[j+i] << j);
		P_WriteUINT8(&savebuffer, btemp);
		i += j;
	}
	for (i = 0; i < MAXUNLOCKABLES;)
	{
		btemp = 0;
		for (j = 0; j < 8 && j+i < MAXUNLOCKABLES; ++j)
			btemp |= (data->unlocked[j+i] << j);
		P_WriteUINT8(&savebuffer, btemp);
		i += j;
	}
	for (i = 0; i < MAXCONDITIONSETS;)
	{
		btemp = 0;
		for (j = 0; j < 8 && j+i < MAXCONDITIONSETS; ++j)
			btemp |= (data->achieved[j+i] << j);
		P_WriteUINT8(&savebuffer, btemp);
		i += j;
	}

	P_WriteUINT32(&savebuffer, data->timesBeaten);
	P_WriteUINT32(&savebuffer, data->timesBeatenWithEmeralds);
	P_WriteUINT32(&savebuffer, data->timesBeatenUltimate);

	// Main records
	for (i = 0; i < NUMMAPS; i++)
	{
		if (data->mainrecords[i])
		{
			P_WriteUINT32(&savebuffer, data->mainrecords[i]->score);
			P_WriteUINT32(&savebuffer, data->mainrecords[i]->time);
			P_WriteUINT16(&savebuffer, data->mainrecords[i]->rings);
		}
		else
		{
			P_WriteUINT32(&savebuffer, 0);
			P_WriteUINT32(&savebuffer, 0);
			P_WriteUINT16(&savebuffer, 0);
		}
		P_WriteUINT8(&savebuffer, 0); // compat
	}

	// NiGHTS records
	for (i = 0; i < NUMMAPS; i++)
	{
		if (!data->nightsrecords[i] || !data->nightsrecords[i]->nummares)
		{
			P_WriteUINT8(&savebuffer, 0);
			continue;
		}

		P_WriteUINT8(&savebuffer, data->nightsrecords[i]->nummares);

		for (curmare = 0; curmare < (data->nightsrecords[i]->nummares + 1); ++curmare)
		{
			P_WriteUINT32(&savebuffer, data->nightsrecords[i]->score[curmare]);
			P_WriteUINT8(&savebuffer, data->nightsrecords[i]->grade[curmare]);
			P_WriteUINT32(&savebuffer, data->nightsrecords[i]->time[curmare]);
		}
	}

	FIL_WriteFile(va(pandf, srb2home, gamedatafilename), savebuffer.buf, savebuffer.pos);
	free(savebuffer.buf);
}
