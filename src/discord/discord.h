// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord.h
/// \brief Globalizes Discord Rich Presence data

#ifndef __DISCORD__
#define __DISCORD__

#include "../command.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_DISCORDSUPPORT

#include <discord_rpc.h>

// ------------------------ //
//        Variables
// ------------------------ //

// Please feel free to provide your own Discord app if you're making a new custom build :)
#define DISCORD_APPID "1013126566236135516"

#ifdef DEVELOP
#define DISCORD_SECRETIVE
#endif

// length of IP strings
#define IP_SIZE 21

// length of Discord Presence strings
#define DISCORD_PRESENCE_STRING_SIZE 128
#define DISCORD_PRESENCE_IMAGE_STRING_SIZE 64

// Allows for SRB2 to use the discriminators of Discord usernames! (even though they removed them :p)
#define DISCORD_DISCRIMINATORS

// Discord Rich Presence connection status
typedef enum {
	DRPC_NOTCONNECTED = 0,
	DRPC_INITIALIZED,
	DRPC_DISCONNECTED,
	DRPC_CONNECTED
} DRPC_Status_t;

extern struct discordInfo_s {
	UINT8 maxPlayers;
	boolean joinsAllowed;
	boolean everyoneCanInvite;
	DRPC_Status_t ConnectionStatus;
} discordInfo;

typedef struct discordRequest_s {
	char *username; // Discord user name.
	char *discriminator; // Discord discriminator (The little hashtag thing after the username). Separated for a "hide discriminators" cvar.
	char *userID; // The ID of the Discord user, gets used with Discord_Respond()

	// HAHAHA, no.
	// *Maybe* if it was only PNG I would boot up curl just to get AND convert this to Doom GFX,
	// but it can *also* be a JEPG, WebP, or GIF :)
	// Hey, wanna add ImageMagick as a dependency? :dying:
	//patch_t *avatar;

	struct discordRequest_s *next; // Next request in the list.
	struct discordRequest_s *prev; // Previous request in the list. Not used normally, but just in case something funky happens, this should repair the list.
} discordRequest_t;

extern discordRequest_t *discordRequestList;


extern consvar_t cv_discordrp;
extern consvar_t cv_discordstreamer;
extern consvar_t cv_discordasks;
extern consvar_t cv_discordshowonstatus;
extern consvar_t cv_discordstatusmemes;
extern consvar_t cv_discordcharacterimagetype;
extern consvar_t cv_discordcustom_details;
extern consvar_t cv_discordcustom_state;
extern consvar_t cv_discordcustom_imagetype_large;
extern consvar_t cv_discordcustom_imagetype_small;
extern consvar_t cv_discordcustom_characterimage_large;
extern consvar_t cv_discordcustom_characterimage_small;
extern consvar_t cv_discordcustom_supercharacterimage_large;
extern consvar_t cv_discordcustom_supercharacterimage_small;
extern consvar_t cv_discordcustom_mapimage_large;
extern consvar_t cv_discordcustom_mapimage_small;
extern consvar_t cv_discordcustom_miscimage_large;
extern consvar_t cv_discordcustom_miscimage_small;
extern consvar_t cv_discordcustom_imagetext_large;
extern consvar_t cv_discordcustom_imagetext_small;


// ------------------------ //
//        Functions
// ------------------------ //

// =====
// TOOLS
// =====

/*--------------------------------------------------
	void DRPC_StringPrintf(char *main, const char *sep, const char *string, ...);

		Provides easier methods of concatenation when it
		comes to applying Discord Rich Presence statuses to the given string.
--------------------------------------------------*/
void DRPC_StringPrintf(char *main, const char *sep, const char *string, ...);


/*--------------------------------------------------
	void DRPC_ImagePrintf(char *string, const char *sep, const char *image, ...);

		Provides easier methods of applying images to
		Discord Rich Presence statuses.
--------------------------------------------------*/
void DRPC_ImagePrintf(char *string, const char *sep, const char *image, ...);


// ========
// STATUSES
// ========

/*--------------------------------------------------
	void DRPC_GeneralStatus(char *string, char *image, char *imagestr);

		Applies a Discord Rich Presence status, related to general details,
		to the given string.
--------------------------------------------------*/

void DRPC_GeneralStatus(char *string, char *image, char *imagestr);


/*--------------------------------------------------
	void DRPC_ExtendedStatus(char *string);

		Applies a Discord Rich Presence status, related to extended status details,
		to the given string.
--------------------------------------------------*/

void DRPC_ExtendedStatus(char *string);


/*--------------------------------------------------
	void DRPC_ScoreStatus(char *string);

		Applies a Discord Rich Presence status, related to score amounts, to
		the given string.
--------------------------------------------------*/

void DRPC_ScoreStatus(char *string);


/*--------------------------------------------------
	void DRPC_EmblemStatus(char *string);

		Applies a Discord Rich Presence status, related to emblems, to
		the given string.
--------------------------------------------------*/

void DRPC_EmblemStatus(char *string);

/*--------------------------------------------------
	void DRPC_EmeraldStatus(char *string);

		Applies a Discord Rich Presence status, related to how many
		emeralds the user has, to the given string.
--------------------------------------------------*/

void DRPC_EmeraldStatus(char *string);


/*--------------------------------------------------
	void DRPC_GamestateStatus(char *string, char *image, char *imagestr);

		Applies a Discord Rich Presence status, related to gamestates, to
		the given string.
--------------------------------------------------*/

void DRPC_GamestateStatus(char *string, char *image, char *imagestr);


/*--------------------------------------------------
	void DRPC_CharacterStatus(char *charimg, char *charname, char *s_charimg, char *s_charname);

		Applies a Discord Rich Presence status, related to levels, to
		the given string.
--------------------------------------------------*/

void DRPC_CharacterStatus(char *charimg, char *charname, char *s_charimg, char *s_charname);


/*--------------------------------------------------
	void DRPC_PlaytimeStatus(char *string);

		Applies a Discord Rich Presence status, related to SRB2 playtime, to
		the given string.
--------------------------------------------------*/

void DRPC_PlaytimeStatus(char *string);


/*--------------------------------------------------
	void DRPC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr);

		Using the customizable custom discord status commands, this applies
		a Discord Rich Presence status to the given string.
--------------------------------------------------*/

void DRPC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr);


// ====
// MAIN
// ====

/*--------------------------------------------------
	char *DRPC_ReturnUsername(void);

		Returns the Discord username of the user.
		Properly accomdiates for streamer mode.
--------------------------------------------------*/

char *DRPC_ReturnUsername(void);


/*--------------------------------------------------
	void DRPC_RemoveRequest(void);

		Removes an invite from the list.
--------------------------------------------------*/

void DRPC_RemoveRequest(discordRequest_t *removeRequest);


/*--------------------------------------------------
	void DRPC_Init(void);

		Initalizes Discord Rich Presence by linking the Application ID
		and setting the callback functions.
--------------------------------------------------*/

void DRPC_Init(void);


/*--------------------------------------------------
	void DRPC_UpdatePresence(void);

		Updates what is displayed by Rich Presence on the user's profile.
		Should be called whenever something that is displayed is
		changed in-game.
--------------------------------------------------*/

void DRPC_UpdatePresence(void);


#endif // HAVE_DISCORDSUPPORT

/*--------------------------------------------------
	void TSoURDt3rd_D_Joinable_OnChange(void)

		Grabs Discord presence info and packets in netgames.
--------------------------------------------------*/

void TSoURDt3rd_D_Joinable_OnChange(void);


/*--------------------------------------------------
	void TSoURDt3rd_D_Got_DiscordInfo(void)

		Updates Discord presence info based on packets
		received from servers.
--------------------------------------------------*/

void TSoURDt3rd_D_Got_DiscordInfo(UINT8 **cp, INT32 playernum);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // __DISCORD__
