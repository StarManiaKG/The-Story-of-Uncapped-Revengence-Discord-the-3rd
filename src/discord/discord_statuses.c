// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_statuses.c
/// \brief Discord Rich Presence statuses

#include "discord_cmds.h"
#include "../doomstat.h"
#include "../m_cond.h"
#include "../g_game.h"

#include "../STAR/star_vars.h" // TSoURDt3rd struct //

// ------------------------ //
//        Functions
// ------------------------ //

/*--------------------------------------------------
	void DRPC_ScoreStatus(char **string)

		Applies a Discord Rich Presence status, related to score amounts, to
		the given string.
--------------------------------------------------*/
void DRPC_ScoreStatus(char **string)
{
	if (!(playeringame[consoleplayer] && !demoplayback)
		return;
	strlcat(string, va("Current Score: %d", players[consoleplayer].score), 128);
}

/*--------------------------------------------------
	void DRPC_EmblemStatus(char **string)

		Applies a Discord Rich Presence status, related to emblems, to
		the given string.
--------------------------------------------------*/
void DRPC_EmblemStatus(char **string)
{
	if (!(netgame || splitscreen))
		return;
	snprintf(&string, 128, "%d/%d Emblems", M_CountEmblems(serverGamedata), (numemblems + numextraemblems));
}

/*--------------------------------------------------
	void DRPC_EmeraldStatus(char **string)

		Applies a Discord Rich Presence status, related to how many
		emeralds the user has, to the given string.
--------------------------------------------------*/
void DRPC_EmeraldStatus(char **string)
{
	UINT8 emeraldCount = 0; // Help me find the emouralds!

	if (modeattacking)
		return;

	for (UINT8 i = 0; i < 7; i++)
	{
		// Emerald math, provided by Monster Iestyn and Uncapped Plus' Fafabis :)
		if (gametyperules & GTR_POWERSTONES && (players[consoleplayer].powers[pw_emeralds] & 1<<i))
			emeraldCount += 1;
		else if (!(gametyperules & GTR_POWERSTONES) && emeralds & 1<<i)
			emeraldCount += 1;
	}

	if (!cv_discordshowonstatus.value && !splitscreen)
		strlcat(string, ", ", 128);
	if (emeraldCount == 7 || all7matchemeralds)
		strlcat(string, "All ", 128);

	strlcat(string, va("%d Emerald", emeraldCount), 128);
	if (emeraldCount > 1)
		strlcat(string, "s", 128);

	if (cv_discordstatusmemes.value)
	{
		if (emeraldCount == (3|4))
		{
			/* Fun Fact: The subtitles in Shadow the Hedgehog emphasized "fourth",
				even though Jason Griffith emphasized "damn" in this sentence :p */
			strlcat(string, "s; ", 128); 
			strlcat(string, (emeraldCount == 3 ? "Where's That DAMN FOURTH?" : "Found That DAMN FOURTH!"), 128);
		}
		if (emeraldCount == 7 && players[consoleplayer].powers[pw_super]) // Goku Mode
			strlcat(string, " Currently In Goku Mode", 128);
		if (!emeraldCount) // Punctuation
			strlcat(string, "s?", 128); 

		if (!emeraldCount && (!(gametyperules & GTR_POWERSTONES) || (gametyperules & GTR_POWERSTONES && !all7matchemeralds)))
			snprintf(string, 128, "NO EMERALDS?");
	}
	else
	{
		if (players[consoleplayer].powers[pw_super])
			strlcat(string, " Currently Super", 128);
	}
}

void DRPC_PlaytimeStatus(char **string)
{
	strlcat(string,
		va("Total Playtime: %d Hours, %d Minutes, and %d Seconds",
			G_TicsToHours(serverGamedata->totalplaytime),
			G_TicsToMinutes(serverGamedata->totalplaytime, false),
			G_TicsToSeconds(serverGamedata->totalplaytime)),
	128);
}
