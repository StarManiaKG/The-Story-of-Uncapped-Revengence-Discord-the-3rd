// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2026 by Star "Guy Who Names Scripts After Him" ManiaKG.
// Copyright (C) 2018-2025 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2025 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_cmds.c
/// \brief Discord integration - net structures

#include "discord.h"

#include "../byteptr.h"
#include "../g_game.h"

#include "../netcode/net_command.h"
#include "../netcode/server_connection.h"

#include "../STAR/star_vars.h" // TSoURDt3rd structure //

static CV_PossibleValue_t discordinvites_cons_t[] = {{0, "Admins Only"}, {1, "Everyone"}, {0, NULL}};
consvar_t cv_discordinvites = CVAR_INIT ("discordinvites", "Everyone", CV_SAVE|CV_CALL, discordinvites_cons_t, DISC_Joinable_OnChange);

/*--------------------------------------------------
	void DISC_Joinable_OnChange(void)

		See header file for description.
--------------------------------------------------*/
void DISC_Joinable_OnChange(void)
{
	UINT8 buf[3];
	UINT8 *p = buf;
	const UINT8 maxplayer = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));

	if (!server)
	{
		return;
	}

	WRITEUINT8(p, maxplayer);
	WRITEUINT8(p, cv_allownewplayer.value);
	WRITEUINT8(p, cv_discordinvites.value);

	SendNetXCmd(XD_DISCORD, &buf, 3);
}

/*--------------------------------------------------
	void DISC_GotNetInfo(void)

		See header file for description.
--------------------------------------------------*/
void DISC_GotNetInfo(UINT8 **cp, INT32 playernum)
{
	if (playernum != serverplayer /*&& !IsPlayerAdmin(playernum)*/)
	{
		// protect against hacked/buggy client
		CONS_Alert(CONS_WARNING, M_GetText("Illegal Discord info command received from %s\n"), player_names[playernum]);
		if (server)
			SendKick(playernum, KICK_MSG_CON_FAIL);
		return;
	}

#ifdef HAVE_DISCORDSUPPORT
	// Implement our data if the server uses TSoURDt3rd.
	// Otherwise, just discard it.
	if (TSoURDt3rdPlayers[serverplayer].server_usingTSoURDt3rd == true)
	{
		discordInfo.net.maxPlayers = READUINT8(*cp);
		discordInfo.net.joinsAllowed = (boolean)READUINT8(*cp);
		discordInfo.net.everyoneCanInvite = (boolean)READUINT8(*cp);
	}
	else
	{
		discordInfo.net.maxPlayers = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));
		discordInfo.net.joinsAllowed = (boolean)cv_allownewplayer.value;
		discordInfo.net.everyoneCanInvite = (boolean)cv_discordinvites.value;
		(*cp) += 3;
	}
	DISC_UpdatePresence();
#else
	// Don't do anything with the information if we don't have Discord RP support
	(*cp) += 3;
#endif
}
