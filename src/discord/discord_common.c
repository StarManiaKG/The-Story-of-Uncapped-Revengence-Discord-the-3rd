// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_common.c
/// \brief Shared structures and routines between Discord's RPC and Game SDK

#ifdef HAVE_DISCORDSUPPORT

#include "discord.h"

// ------------------------ //
//        Variables
// ------------------------ //

struct discordInfo_s discordInfo;

discordRequest_t *discordRequestList = NULL;

char discord_username[DISC_STATUS_IMAGE_STRING_SIZE];

// ------------------------ //
//        Functions
// ------------------------ //

/*--------------------------------------------------
	char *DISC_HideUsername(char *input);

		Handle usernames while cv_discordstreamer is activated.
		(The loss of discriminators is still a dumbass regression
		that I will never forgive the Discord developers for.)
--------------------------------------------------*/
char *DISC_HideUsername(char *input)
{
	static char buffer[5];
	int i;

	buffer[0] = input[0];

	for (i = 1; i < 4; ++i)
	{
		buffer[i] = '.';
	}

	buffer[4] = '\0';
	return buffer;
}

/*--------------------------------------------------
	char *DISC_ReturnUsername(void);

		Returns the Discord username of the user.
		Properly accomdiates for streamer mode.

	Input Arguments:-
		None

	Return:-
		Discord Username String
--------------------------------------------------*/
char *DISC_ReturnUsername(void)
{
	if (cv_discordstreamer.value)
		return DISC_HideUsername(discord_username);
	return discord_username;
}

/*--------------------------------------------------
	char *DISC_XORIPString(const char *input)

		Simple XOR encryption/decryption. Not complex or
		very secretive because we aren't sending anything
		that isn't easily accessible via our Master Server anyway.
--------------------------------------------------*/
char *DISC_XORIPString(const char *input)
{
	const UINT8 xor[IP_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
	char *output = malloc(sizeof(char) * (IP_SIZE+1));
	UINT8 i;

	for (i = 0; i < IP_SIZE; i++)
	{
		char xorinput;

		if (!input[i])
			break;

		xorinput = input[i] ^ xor[i];

		if (xorinput < 32 || xorinput > 126)
		{
			xorinput = input[i];
		}

		output[i] = xorinput;
	}

	output[i] = '\0';

	return output;
}

#endif // HAVE_DISCORDSUPPORT
