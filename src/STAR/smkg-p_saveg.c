// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_saveg.c
/// \brief TSoURDt3rd's custom savegame code

#include "smkg-p_saveg.h"
#include "core/smkg-g_game.h" // tsourdt3rd_local //
#include "misc/smkg-m_misc.h"

#include "../byteptr.h"
#include "../doomstat.h"
#include "../d_main.h"
#include "../g_game.h"
#include "../p_local.h"
#include "../z_zone.h"

#include "../netcode/i_net.h"

// ------------------------ //
//        Functions
// ------------------------ //

//#ifdef _TSOURDT3RD_DEBUGGING
	#define WRITETOFILE
//#endif

//#define SAVE_BLOCK
#define TSOURDT3RD_ARCHIVEBLOCK_USERS 0x7FA5C508

// ------------------------ //
//        Functions
// ------------------------ //

// -------------------------------
// Data Reading Routines
// -------------------------------

#ifdef WRITETOFILE
static void Write(INT32 playernum, boolean archive)
{
	FILE *f;
	const char *path;
	const char *filename = (archive ? "STAR_bye.txt" : "STAR_hi.txt");
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	if (!playeringame[playernum])
		return;

	path = va(pandf, TSoURDt3rd_FOL_ReturnHomepath_Build(), filename);
	f = fopen(path, (playernum == 0 ? "w+" : "a+"));

	fputs(va("CURRENT TIC: %d\n", gametic), f);
	if (archive)
		fputs("Type: SENDING!\n", f);
	else
		fputs("Type: RECEIVING!\n", f);

	fputs(va("\nName: %s\n", player_names[playernum]), f);
	fputs(va("Player: %d\n", playernum), f);

	fputs(TSoURDt3rd->user_hash, f);
	fputs(va("%d\n", TSoURDt3rd->usingTSoURDt3rd), f);
	fputs(va("%d\n", TSoURDt3rd->server_usingTSoURDt3rd), f);
	fclose(f);
}
#endif

#define CHECK_FOR_TSOURDT3RD \
	if (!tsourdt3rd_user || !tsourdt3rd_user->usingTSoURDt3rd) \
		return fallback;

UINT8 TSoURDt3rd_P_ReadUINT8(save_t *save_p, TSoURDt3rd_t *tsourdt3rd_user, UINT8 fallback)
{
	CHECK_FOR_TSOURDT3RD
	return P_ReadUINT8(save_p);
}

UINT32 TSoURDt3rd_P_ReadUINT32(save_t *save_p, TSoURDt3rd_t *tsourdt3rd_user, UINT32 fallback)
{
	CHECK_FOR_TSOURDT3RD
	return P_ReadUINT32(save_p);
}

const char *TSoURDt3rd_P_ReadString(save_t *save_p, TSoURDt3rd_t *tsourdt3rd_user, char *string, const char *fallback)
{
	CHECK_FOR_TSOURDT3RD
	P_ReadString(save_p, string);
	return string;
}

// -------------------------------
// Archival Routines
// -------------------------------

void TSoURDt3rd_P_NetArchiveUsers(save_t *save_p)
{
	UINT32 i;

#ifdef SAVE_BLOCK
	P_WriteUINT32(save_p, TSOURDT3RD_ARCHIVEBLOCK_USERS);
#endif
	for (i = 0; i < MAXPLAYERS; i++)
	{
		if (!playeringame[i])
			continue;

		P_WriteString(save_p, TSoURDt3rdPlayers[i].user_hash);
		P_WriteUINT8(save_p, TSoURDt3rdPlayers[i].usingTSoURDt3rd);
		P_WriteUINT8(save_p, TSoURDt3rdPlayers[i].server_usingTSoURDt3rd);
		P_WriteUINT8(save_p, TSoURDt3rdPlayers[i].server_majorVersion);
		P_WriteUINT8(save_p, TSoURDt3rdPlayers[i].server_minorVersion);
		P_WriteUINT8(save_p, TSoURDt3rdPlayers[i].server_subVersion);
		P_WriteUINT8(save_p, TSoURDt3rdPlayers[i].server_TSoURDt3rdVersion);

#ifdef WRITETOFILE
		Write(i, true);
#endif
	}
}

void TSoURDt3rd_P_NetUnArchiveUsers(save_t *save_p)
{
	UINT32 i;

#ifdef SAVE_BLOCK
	if (P_ReadUINT32(save_p) != TSOURDT3RD_ARCHIVEBLOCK_USERS)
	{
		// No TSoURDt3rd data exists...
		I_Error("Bad $$$.sav at archive block TSoURDt3rd");
	}
#endif

	for (i = 0; i < MAXPLAYERS; i++)
	{
		TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[i];

		if (!playeringame[i])
			continue;

		TSoURDt3rd_P_ReadString(save_p, TSoURDt3rd, TSoURDt3rdPlayers[i].user_hash, "\0");

		TSoURDt3rdPlayers[i].usingTSoURDt3rd = TSoURDt3rd_P_ReadUINT8(save_p, TSoURDt3rd, false);
		TSoURDt3rdPlayers[i].server_usingTSoURDt3rd = TSoURDt3rd_P_ReadUINT8(save_p, TSoURDt3rd, false);
		TSoURDt3rdPlayers[i].server_majorVersion = TSoURDt3rd_P_ReadUINT8(save_p, TSoURDt3rd, tsourdt3rd_local.major_version);
		TSoURDt3rdPlayers[i].server_minorVersion = TSoURDt3rd_P_ReadUINT8(save_p, TSoURDt3rd, tsourdt3rd_local.minor_version);
		TSoURDt3rdPlayers[i].server_subVersion = TSoURDt3rd_P_ReadUINT8(save_p, TSoURDt3rd, tsourdt3rd_local.sub_version);
		TSoURDt3rdPlayers[i].server_TSoURDt3rdVersion = TSoURDt3rd_P_ReadUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentVersion());

#ifdef WRITETOFILE
		Write(i, false);
#endif
	}
}
