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
/// \file  discord.h
/// \brief Discord integration structures, routines, and data

#ifndef __DISCORD__
#define __DISCORD__

#include <time.h>

#include "../command.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_DISCORDSUPPORT


// ------------------------ //
//        Variables
// ------------------------ //


#ifdef DEVELOP
#define DISCORD_SECRETIVE
#endif

// Please feel free to provide your own Discord app if you're making a new custom build :)
#define DISCORD_APPID			1013126566236135516
#define DISCORD_APPID_STRING	"1013126566236135516" // direct copy of the above in a string format

// Allows for SRB2 to use the discriminators of Discord usernames! (even though they removed them :p)
#define DISCORD_DISCRIMINATORS

// length of IP strings
#define DISCORD_IP_SIZE 21

// length of Discord Presence strings
#define DISC_STATUS_MAX_STRING_SIZE 128
#define DISC_STATUS_MAX_IMAGE_STRING_SIZE 64
#define DISC_STATUS_MIN_STRING_SIZE 2


// Discord client info
typedef enum {
	DISC_NOTCONNECTED	= 0,
	DISC_DISCONNECTED	= 1>>0,
	DISC_CONNECTED		= 1>>1,
} DISC_ConnectionStatus_t;

extern struct discordInfo_s {
	DISC_ConnectionStatus_t connectionStatus;
	struct {
		UINT8 maxPlayers;
		boolean joinsAllowed;
		boolean everyoneCanInvite;
	} net;
} discordInfo;


// Discord request info
typedef enum {
	DISC_REQUEST_REPLY_NO = 0,
	DISC_REQUEST_REPLY_YES,
	DISC_REQUEST_REPLY_IGNORE
} DISC_RequestReply_t;

typedef struct discordRequest_s {
	char *username; // Discord user name.
	char *discriminator; // Discord discriminator (The little hashtag thing after the username). Separated for a "hide discriminators" cvar.
	char *userID; // The ID of the Discord user, gets used with DISC_Respond()

	// HAHAHA, no.
	// *Maybe* if it was only PNG I would boot up curl just to get AND convert this to Doom GFX,
	// but it can *also* be a JEPG, WebP, or GIF :)
	// Hey, wanna add ImageMagick as a dependency? :dying:
	//patch_t *avatar;

	struct discordRequest_s *next; // Next request in the list.
	struct discordRequest_s *prev; // Previous request in the list. Not used normally, but just in case something funky happens, this should repair the list.
} DISC_Request_t;

extern DISC_Request_t *discordRequestList;


// COMMANDS
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


// MISC
extern char discord_integration_type[DISC_STATUS_MAX_STRING_SIZE];
extern char discord_fullusername[DISC_STATUS_MAX_STRING_SIZE];


// ------------------------ //
//        Functions
// ------------------------ //

// =====
// TOOLS
// =====


/*--------------------------------------------------
	void DISC_StatusPrintf(boolean is_image, char *main, const char *sep, const char *string, ...)

		Provides easier methods of concatenation when it
		comes to applying game info to our user's
		Discord status.
--------------------------------------------------*/

void DISC_StatusPrintf(boolean is_image, char *main, const char *sep, const char *string, ...);


// ========
// STATUSES
// ========


/*--------------------------------------------------
	void DISC_BasicStatus(char *string, char *image, char *imagestr)

		Applies basic Discord status info to the given strings.
--------------------------------------------------*/

void DISC_BasicStatus(char *string, char *image, char *imagestr);


/*--------------------------------------------------
	void DISC_PlayerStatus(char *string)

		Applies more advanced Discord status info to the given string.
--------------------------------------------------*/

void DISC_PlayerStatus(char *string);


/*--------------------------------------------------
	void DISC_ScoreStatus(char *string)

		Applies the user's current score to the given string.
--------------------------------------------------*/

void DISC_ScoreStatus(char *string);


/*--------------------------------------------------
	void DISC_EmblemStatus(char *string)

		Applies the user's current emblem info to the given string.
--------------------------------------------------*/

void DISC_EmblemStatus(char *string);


/*--------------------------------------------------
	void DISC_EmeraldStatus(char *string)

		Applies the user's current emerald info to the given string.
--------------------------------------------------*/

void DISC_EmeraldStatus(char *string);


/*--------------------------------------------------
	void DISC_GamestateStatus(char *string, char *image, char *imagestr)

		Applies the game's gamestate info to the given strings.
--------------------------------------------------*/

void DISC_GamestateStatus(char *string, char *image, char *imagestr);


/*--------------------------------------------------
	void DISC_CharacterStatus(char *image, char *imagestr, char *s_image, char *s_imagestr)

		Applies the user's skin info to the given strings.
--------------------------------------------------*/

void DISC_CharacterStatus(char *image, char *imagestr, char *s_image, char *s_imagestr);


/*--------------------------------------------------
	void DISC_PlaytimeStatus(char *string)

		Applies the user's playtime info to the given string.
--------------------------------------------------*/

void DISC_PlaytimeStatus(char *string);


/*--------------------------------------------------
	void DISC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr)

		Using the customizable custom Discord status commands, this applies
		a user-defined status to the currently connected user.
--------------------------------------------------*/

void DISC_CustomStatus(char *detailstr, char *statestr, char *image, char *imagestr, char *s_image, char *s_imagestr);


// ========
// HANDLERS
// ========


/*--------------------------------------------------
	void DISC_SetConnectionStatus(DISC_ConnectionStatus_t status)

		Sets the connection status for our currently connected user.
--------------------------------------------------*/

void DISC_SetConnectionStatus(DISC_ConnectionStatus_t status);


/*--------------------------------------------------
	char *DISC_HideUsername(char *input)

		Handle usernames while cv_discordstreamer is activated.
		(The loss of discriminators is still a dumbass regression
		that I will never forgive the Discord developers for.)
--------------------------------------------------*/

char *DISC_HideUsername(char *input);


/*--------------------------------------------------
	char *DISC_ReturnUsername(void)

		Returns the Discord username of the user.
		Properly accomdiates for streamer mode.

	Input Arguments:-
		None

	Return:-
		Discord Username String
--------------------------------------------------*/

char *DISC_ReturnUsername(void);


/*--------------------------------------------------
	char *DISC_XORIPString(const char *input)

		Simple XOR encryption/decryption. Not complex or
		very secretive because we aren't sending anything
		that isn't easily accessible via our Master Server anyway.
--------------------------------------------------*/

char *DISC_XORIPString(const char *input);


/*--------------------------------------------------
	const char *DRPC_GetServerIP(void)

		Retrieves the IP address of the server that you're
		connected to. Will attempt to use stun for getting your
		own IP address, if it's not yours.
--------------------------------------------------*/

const char *DRPC_GetServerIP(void);


/*--------------------------------------------------
	void DRPC_GotServerIP(UINT32 address)

		Callback triggered by successful STUN response.

	Input Arguments:-
		address - IPv4 address of this machine, in network byte order.

	Return:-
		None
--------------------------------------------------*/

void DRPC_GotServerIP(UINT32 address);


/*--------------------------------------------------
	void DISC_HandleInitializing(const char *integration_type)

		Handles various Discord data intended for when our current integration
		is initializing.
--------------------------------------------------*/

void DISC_HandleInitializing(const char *integration_type);


/*--------------------------------------------------
	void DISC_HandleConnected(const char *username, const char *discriminator, const char *userId)

		Handles various Discord data intended for when a user
		connects to our current integration.
--------------------------------------------------*/

void DISC_HandleConnected(const char *username, const char *discriminator, const char *userId);


/*--------------------------------------------------
	void DISC_HandleDisconnected(INT32 err, const char *msg)

		Handles various Discord data intended for when a user
		disconnects to our current integration.
--------------------------------------------------*/

void DISC_HandleDisconnected(INT32 err, const char *msg);


/*--------------------------------------------------
	void DISC_HandleError(INT32 err, const char *msg)

		Handles various Discord data intended for when a user
		disconnects to our current integration.
--------------------------------------------------*/

void DISC_HandleError(INT32 err, const char *msg);


/*--------------------------------------------------
	void DISC_HandleJoining(const char *join_secret)

		Handles various Discord data intended for when the user
		joins a server using the 'Ask to Join' feature.
--------------------------------------------------*/

void DISC_HandleJoining(const char *join_secret);


/*--------------------------------------------------
	void DISC_HandleQuitting(void)

		Handles various Discord data intended for when the game
		is closed.
--------------------------------------------------*/

void DISC_HandleQuitting(void);


/*--------------------------------------------------
	void DISC_EmptyRequests(void)

		Empties the request list. Any existing requests
		will get an ignore reply.
--------------------------------------------------*/

void DISC_EmptyRequests(void);


// ===============
// ACTIVITY STATUS
// ===============


/*--------------------------------------------------
	boolean DISC_InvitesAllowed(void)

		Determines whenever or not invites or
		ask to join requests are allowed.

	Input Arguments:-
		None

	Return:-
		true if invites are allowed, false otherwise.
--------------------------------------------------*/

boolean DISC_InvitesAllowed(void);


/*--------------------------------------------------
	void DISC_SetActivityStatus(
		char *details, char *state,
		char *image, char *imagetxt,
		char *s_image, char *s_imagetxt,
		time_t *timestamp_start, time_t *timestamp_end,
		char **clientJoinSecret,
		char **partyID, int *partySize, int *partyMax
	)

		Sets the activity status info for our currently connected user.
--------------------------------------------------*/

void DISC_SetActivityStatus(
	char *details, char *state,
	char *image, char *imagetxt,
	char *s_image, char *s_imagetxt,
	time_t *timestamp_start, time_t *timestamp_end,
	char **clientJoinSecret,
	char **partyID, int *partySize, int *partyMax
);


// =========
// CALLBACKS
// =========


/*--------------------------------------------------
	void DISC_Init(void)

		Initalizes Discord support by linking the Application ID
		and setting the callback functions.
--------------------------------------------------*/

void DISC_Init(void);


/*--------------------------------------------------
	void DISC_UpdatePresence(void)

		Updates what is displayed by Discord on the user's profile.
		Should be called whenever something that is displayed is
		changed in-game.
--------------------------------------------------*/

void DISC_UpdatePresence(void);


/*--------------------------------------------------
	void DISC_RunCallbacks(void)

		Alerts Discord to check for errors, run some functions,
		and begin updating the user's presence.
--------------------------------------------------*/

void DISC_RunCallbacks(void);


/*--------------------------------------------------
	void DISC_Respond(const char *userID, DISC_RequestReply_t response)

		Response handler for Discord requests.
--------------------------------------------------*/

void DISC_Respond(const char *userID, DISC_RequestReply_t response);


/*--------------------------------------------------
	void DISC_RemoveRequest(void)

		Removes an invite from the list.
--------------------------------------------------*/

void DISC_RemoveRequest(DISC_Request_t *removeRequest);


/*--------------------------------------------------
	void DISC_Quit(void)

		Handles freeing Discord whenever the game exits.
--------------------------------------------------*/

void DISC_Quit(void);


#endif // HAVE_DISCORDSUPPORT


extern consvar_t cv_discordinvites;


/*--------------------------------------------------
	void DISC_D_Joinable_OnChange(void)

		Grabs Discord presence info and packets in netgames.
--------------------------------------------------*/

void DISC_D_Joinable_OnChange(void);


/*--------------------------------------------------
	void DISC_D_Got_NetInfo(void)

		Updates Discord presence info based on packets
		received from servers.
--------------------------------------------------*/

void DISC_D_Got_NetInfo(UINT8 **cp, INT32 playernum);


#ifdef __cplusplus
} // extern "C"
#endif


#endif // __DISCORD__
