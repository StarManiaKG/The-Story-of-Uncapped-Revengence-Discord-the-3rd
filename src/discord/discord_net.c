// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2025 by Kart Krew.
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_cmds.c
/// \brief Discord status net structures

#include "discord.h"

#include "../STAR/star_vars.h" // TSoURDt3rd structure //

#include "../d_netcmd.h"
#include "../g_game.h"
#include "../byteptr.h"

// ------------------------ //
//        Functions
// ------------------------ //

/*--------------------------------------------------
	void TSoURDt3rd_D_Joinable_OnChange(void)

		See header file for description.
--------------------------------------------------*/
void TSoURDt3rd_D_Joinable_OnChange(void)
{
	UINT8 buf[3];
	UINT8 *p = buf;
	UINT8 maxplayer;

	if (!server)
		return;

	maxplayer = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));

	WRITEUINT8(p, maxplayer);
	WRITEUINT8(p, cv_allownewplayer.value);
	WRITEUINT8(p, cv_discordinvites.value);

	SendNetXCmd(XD_DISCORD, &buf, 3);
}

/*--------------------------------------------------
	void TSoURDt3rd_D_Got_DiscordInfo(void)

		See header file for description.
--------------------------------------------------*/
void TSoURDt3rd_D_Got_DiscordInfo(UINT8 **cp, INT32 playernum)
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
	// Implement our data as we see fit
	if (TSoURDt3rdPlayers[serverplayer].server_usingTSoURDt3rd)
	{
		discordInfo.maxPlayers = READUINT8(*cp);
		discordInfo.joinsAllowed = (boolean)READUINT8(*cp);
		discordInfo.everyoneCanInvite = (boolean)READUINT8(*cp);
	}
	else
	{
		discordInfo.maxPlayers = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));
		discordInfo.joinsAllowed = cv_allownewplayer.value;
		discordInfo.everyoneCanInvite = (boolean)cv_discordinvites.value;
		(*cp) += 3;
	}
	DISC_UpdatePresence();
#else
	// Don't do anything with the information if we don't have Discord RP support
	(*cp) += 3;
#endif
}
