// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-p_saveg.c
/// \brief TSoURDt3rd's custom savegame code

#include "smkg-p_saveg.h"
#include "smkg-misc.h"

#include "../byteptr.h"
#include "../doomstat.h"
#include "../i_net.h"
#include "../d_main.h"
#include "../p_local.h"
#include "../z_zone.h"

//#define WRITETOFILE

// ------------------------ //
//        Functions
// ------------------------ //

#ifdef WRITETOFILE
#include "../g_game.h" // player_names //

static void Write(INT32 playernum, boolean archive)
{
	FILE *f;
	const char *path;
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	if (!TSoURDt3rd)
		return;

	if (archive)
		path = va("%s"PATHSEP"%s", srb2home, "STAR_bye.txt");
	else
		path = va("%s"PATHSEP"%s", srb2home, "STAR_hi.txt");
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

UINT8 TSOURDT3RD_READUINT8(UINT8 *save_p, TSoURDt3rd_t *tsourdt3rd_user, UINT8 fallback)
{
	if (!tsourdt3rd_user || !tsourdt3rd_user->usingTSoURDt3rd || !netbuffer->u.servercfg.tsourdt3rd)
		return fallback;
	return READUINT8(save_p);
}

UINT32 TSOURDT3RD_READUINT32(UINT8 *save_p, TSoURDt3rd_t *tsourdt3rd_user, UINT32 fallback)
{
	if (!tsourdt3rd_user || !tsourdt3rd_user->usingTSoURDt3rd || !netbuffer->u.servercfg.tsourdt3rd)
		return fallback;
	return READUINT32(save_p);
}

void TSoURDt3rd_NetArchiveUsers(UINT8 *save_p, INT32 playernum)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	if (!TSoURDt3rd)
		return;

	WRITESTRING(save_p, TSoURDt3rd->user_hash);
	WRITEUINT8(save_p, TSoURDt3rd->usingTSoURDt3rd);
	WRITEUINT8(save_p, TSoURDt3rd->server_usingTSoURDt3rd);
	WRITEUINT8(save_p, TSoURDt3rd->server_majorVersion);
	WRITEUINT8(save_p, TSoURDt3rd->server_minorVersion);
	WRITEUINT8(save_p, TSoURDt3rd->server_subVersion);
	WRITEUINT8(save_p, TSoURDt3rd->server_TSoURDt3rdVersion);

#ifdef WRITETOFILE
	Write(playernum, true);
#endif
}

void TSoURDt3rd_NetUnArchiveUsers(UINT8 *save_p, INT32 playernum)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	if (!TSoURDt3rd)
		return;

	if (!TSoURDt3rd->usingTSoURDt3rd || !netbuffer->u.servercfg.tsourdt3rd)
		TSoURDt3rd->user_hash[0] = '\0';
	else
		READSTRING(save_p, TSoURDt3rd->user_hash);
	TSoURDt3rd->usingTSoURDt3rd = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, false);
	TSoURDt3rd->server_usingTSoURDt3rd = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, false);
	TSoURDt3rd->server_majorVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentMajorVersion());
	TSoURDt3rd->server_minorVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentMinorVersion());
	TSoURDt3rd->server_subVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentSubversion());
	TSoURDt3rd->server_TSoURDt3rdVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentVersion());

#ifdef WRITETOFILE
	Write(playernum, false);
#endif
}

//
// void TSoURDt3rd_PSav_WriteExtraData(void)
// Writes unique TSoURDt3rd data to the custom savefile.
//
void TSoURDt3rd_PSav_WriteExtraData(void)
{
	FILE *tsourdt3rd_gamedata = NULL;

	tsourdt3rd_gamedata = TSoURDt3rd_FIL_AccessFile(NULL, "tsourdt3rd.dat", "r");
	if (tsourdt3rd_gamedata != NULL)
	{
		// Let's move this old file to a new directory!
		fclose(tsourdt3rd_gamedata);
		TSoURDt3rd_FIL_RenameFile("tsourdt3rd.dat", "TSoURDt3rd" PATHSEP "tsourdt3rd.dat");
	}
	tsourdt3rd_gamedata = TSoURDt3rd_FIL_AccessFile("TSoURDt3rd", "tsourdt3rd.dat", "w+");

	TSoURDt3rd_FOL_UpdateSavefileDirectory();

	if (tsourdt3rd_gamedata == NULL)
	{
		// Uh-oh! We couldn't find the actual gamedata file!
		return;
	}

	if ((!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER)|| !AllowEasterEggHunt)
		|| (netgame || tsourdt3rd_local.autoloaded_mods))
		return;

	// Write To The File //
	// Easter Eggs
	putw(currenteggs, tsourdt3rd_gamedata);
	putw(foundeggs, tsourdt3rd_gamedata);

	// Close The File //
	fclose(tsourdt3rd_gamedata);
}

//
// void TSoURDt3rd_PSav_ReadExtraData(void)
// Reads the info in TSoURDt3rd's custom savefile.
//
void TSoURDt3rd_PSav_ReadExtraData(void)
{
	FILE *tsourdt3rd_gamedata = NULL;

	tsourdt3rd_gamedata = TSoURDt3rd_FIL_AccessFile(NULL, "tsourdt3rd.dat", "r");
	if (tsourdt3rd_gamedata != NULL)
	{
		// Let's move this old file to a new directory!
		fclose(tsourdt3rd_gamedata);
		TSoURDt3rd_FIL_RenameFile("tsourdt3rd.dat", "TSoURDt3rd" PATHSEP "tsourdt3rd.dat");
	}
	tsourdt3rd_gamedata = TSoURDt3rd_FIL_AccessFile("TSoURDt3rd", "tsourdt3rd.dat", "w+");

	TSoURDt3rd_FOL_UpdateSavefileDirectory();

	if (tsourdt3rd_gamedata == NULL)
	{
		// Uh-oh! We couldn't find the actual gamedata file!
		return;
	}

	if ((!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER)|| !AllowEasterEggHunt)
		|| (netgame || tsourdt3rd_local.autoloaded_mods))
		return;

	// Read Things Within The File //
	// Easter Eggs
	currenteggs = getw(tsourdt3rd_gamedata);
	foundeggs = getw(tsourdt3rd_gamedata);

	// Close the File //
	fclose(tsourdt3rd_gamedata);
}
