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

#ifdef HAVE_DISCORDSUPPORT

// ------------------------ //
//        Variables
// ------------------------ //

//#define 

// ------------------------ //
//        Functions
// ------------------------ //

/*--------------------------------------------------
	void DRPC_ScoreStatus(char *string)

		Applies a Discord Rich Presence status, related to score amounts, to
		the given string.
--------------------------------------------------*/
void DRPC_ScoreStatus(char *string)
{
	if (!(playeringame[consoleplayer] && !demoplayback))
		return;
	snprintf(string, 128, va("Current Score: %d", players[consoleplayer].score));
}

/*--------------------------------------------------
	void DRPC_EmblemStatus(char *string)

		Applies a Discord Rich Presence status, related to emblems, to
		the given string.
--------------------------------------------------*/
void DRPC_EmblemStatus(char *string)
{
	if (!(netgame || splitscreen))
		return;

	if (string[0] == '\0')
		snprintf(string, 128, "%d/%d Emblems", M_CountEmblems(serverGamedata), (numemblems + numextraemblems));
	else
		strlcat(string, va("%d/%d Emblems", M_CountEmblems(serverGamedata), (numemblems + numextraemblems)), 128);
}

/*--------------------------------------------------
	void DRPC_EmeraldStatus(char *string)

		Applies a Discord Rich Presence status, related to how many
		emeralds the user has, to the given string.
--------------------------------------------------*/
void DRPC_EmeraldStatus(char *string)
{
	UINT16 emerald_type = (gametyperules & GTR_POWERSTONES ? players[consoleplayer].powers[pw_emeralds] : emeralds);
	UINT8 emerald_count = 0; // Help me find the emouralds!

	if (modeattacking)
		return;

	// Emerald math, provided by Monster Iestyn and Uncapped Plus' Fafabis :)
	for (UINT8 i = 0; i < 7; i++)
	{
		if (emerald_type & 1<<i)
			emerald_count += 1;
	}

	if (!cv_discordshowonstatus.value && !splitscreen)
		strlcat(string, ", ", 128);
	if (emerald_count == 7 || all7matchemeralds)
		strlcat(string, "All ", 128);

	strlcat(string, va("%d Emerald", emerald_count), 128);
	if (emerald_count > 1)
		//strlcat(string, "s", 128);
		strlcat(string, "s; ", 128);

	if (cv_discordstatusmemes.value)
	{
		if (emerald_count == (3|4))
		{
			/* Fun Fact: The subtitles in Shadow the Hedgehog emphasized "fourth",
				even though Jason Griffith emphasized "damn" in this sentence :p */
			//strlcat(string, "s; ", 128); 
			strlcat(string, (emerald_count == 3 ? "Where's That DAMN FOURTH?" : "Found That DAMN FOURTH!"), 128);
		}
		if (emerald_count == 7 && players[consoleplayer].powers[pw_super]) // Goku Mode
			strlcat(string, " Currently In Goku Mode", 128);
		if (!emerald_count) // Punctuation
		{
			if ((!(gametyperules & GTR_POWERSTONES) || (gametyperules & GTR_POWERSTONES && !all7matchemeralds)))
			{
				snprintf(string, 128, "NO EMERALDS?");
				return;
			}
			else
				strlcat(string, "s?", 128);
		}
	}
	else
	{
		if (players[consoleplayer].powers[pw_super])
			strlcat(string, " Currently Super", 128);
	}
}

/*--------------------------------------------------
	void DRPC_PlaytimeStatus(char *string)

		Applies a Discord Rich Presence status, related to SRB2 playtime, to
		the given string.
--------------------------------------------------*/
void DRPC_PlaytimeStatus(char *string)
{
	snprintf(string, 128, "Total Playtime: %d Hours, %d Minutes, and %d Seconds",
		G_TicsToHours(serverGamedata->totalplaytime),
		G_TicsToMinutes(serverGamedata->totalplaytime, false),
		G_TicsToSeconds(serverGamedata->totalplaytime));
}

/*--------------------------------------------------
	void DRPC_CustomStatus(char *detailstr, char *statestr)

		Using the customizable custom discord status commands, this applies
		a Discord Rich Presence status to the given string.
--------------------------------------------------*/
#include "../m_menu.h"
void DRPC_CustomStatus(char *detailstr, char *statestr)
{
#if 1
	(void)detailstr;
	(void)statestr;
#else
	// Error Out if the String is Less Than Two Letters Long //
	// MAJOR STAR NOTE: please come back to this and flesh it out more lol //
	if (strlen(cv_customdiscorddetails.string) <= 2 || strlen(cv_customdiscordstate.string) <= 2 || strlen(cv_customdiscordsmallimagetext.string) <= 2 || strlen(cv_customdiscordlargeimagetext.string) <= 2)
	{
		STAR_M_StartMessage("Custom Discord RPC String Too Short", "Sorry, Discord RPC requires Strings to be longer than two characters.\n\nResetting strings with less than two letters back to defaults.\n\n(Press a key)\n",NULL,MM_NOTHING);
		S_StartSound(NULL, sfx_skid);

		if (strlen(cv_customdiscorddetails.string) <= 2)
			CV_Set(&cv_customdiscorddetails, cv_customdiscorddetails.defaultvalue);
		if (strlen(cv_customdiscordstate.string) <= 2)
			CV_Set(&cv_customdiscordstate, cv_customdiscordstate.defaultvalue);

		if (strlen(cv_customdiscordsmallimagetext.string) <= 2)
			CV_Set(&cv_customdiscordsmallimagetext, cv_customdiscordsmallimagetext.defaultvalue);
		if (strlen(cv_customdiscordlargeimagetext.string) <= 2)
			CV_Set(&cv_customdiscordlargeimagetext, cv_customdiscordlargeimagetext.defaultvalue);
	}

	// Write the Heading Strings to Discord
	if (strlen(cv_customdiscorddetails.string) > 2)
		strcpy(detailstr, cv_customdiscorddetails.string);
	if (strlen(cv_customdiscordstate.string) > 2)
		strcpy(statestr, cv_customdiscordstate.string);

	// Write The Images and Their Text to Discord //
	// Small Images
	if (cv_customdiscordsmallimagetype.value != 8)
	{
		strcpy(customSImage, va("%s%s", customStringType[cv_customdiscordsmallimagetype.value],
			(cv_customdiscordsmallimagetype.value <= 2 ? supportedSkins[cv_customdiscordsmallcharacterimage.value] :
			((cv_customdiscordsmallimagetype.value >= 3 && cv_customdiscordsmallimagetype.value <= 5) ? supportedSuperSkins[cv_customdiscordsmallsupercharacterimage.value] :
			(cv_customdiscordsmallimagetype.value == 6 ? supportedMaps[cv_customdiscordsmallmapimage.value] :
		supportedMiscs[cv_customdiscordsmallmiscimage.value])))));
	
		strcpy(simagestr, customSImage);
		(strlen(cv_customdiscordsmallimagetext.string) > 2 ? strcpy(simagetxtstr, cv_customdiscordsmallimagetext.string) : 0);
	}
	
	// Large Images
	if (cv_customdiscordlargeimagetype.value != 8)
	{
		strcpy(customLImage, va("%s%s", customStringType[cv_customdiscordlargeimagetype.value],
			(cv_customdiscordlargeimagetype.value <= 2 ? supportedSkins[cv_customdiscordlargecharacterimage.value] :
			((cv_customdiscordlargeimagetype.value >= 3 && cv_customdiscordlargeimagetype.value <= 5) ? supportedSuperSkins[cv_customdiscordlargesupercharacterimage.value] :
			(cv_customdiscordlargeimagetype.value == 6 ? supportedMaps[cv_customdiscordlargemapimage.value] :
		supportedMiscs[cv_customdiscordlargemiscimage.value])))));

		strcpy(imagestr, customLImage);
		(strlen(cv_customdiscordlargeimagetext.string) > 2 ? strcpy(imagetxtstr, cv_customdiscordlargeimagetext.string) : 0);
	}
#endif
}

#endif // HAVE_DISCORDSUPPORT
