// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
// Copyright (C) 2020-2023 by Star "I Make My Own Copyrights" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord.h
/// \brief Discord Rich Presence handling

#ifndef __DISCORD__
#define __DISCORD__

#ifdef HAVE_DISCORDRPC

#include "discord_rpc.h"

// Commands //
// Main Things
extern consvar_t cv_discordrp, cv_discordstreamer;
extern consvar_t cv_discordasks;
extern consvar_t cv_discordshowonstatus, cv_discordstatusmemes, cv_discordcharacterimagetype;

// Custom Things
extern consvar_t cv_customdiscorddetails, cv_customdiscordstate;
extern consvar_t cv_customdiscordlargeimagetype, cv_customdiscordsmallimagetype;
extern consvar_t cv_customdiscordlargecharacterimage, cv_customdiscordsmallcharacterimage;
extern consvar_t cv_customdiscordlargesupercharacterimage, cv_customdiscordsmallsupercharacterimage;
extern consvar_t cv_customdiscordlargemapimage, cv_customdiscordsmallmapimage;
extern consvar_t cv_customdiscordlargemiscimage, cv_customdiscordsmallmiscimage;
extern consvar_t cv_customdiscordlargeimagetext, cv_customdiscordsmallimagetext;

extern struct discordInfo_s {
	boolean Initialized;

	char sessionUsername[256];
	char grabbedUsername[256];
	char discriminator[6];
	char userID[20];

	boolean Disconnected;
	
	UINT8 maxPlayers;
	boolean joinsAllowed;
	UINT8 whoCanInvite;

	INT16 serverRoom;
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

/*--------------------------------------------------
	void DRPC_UpdateUsername(void);

		Updates the current Discord Rich Presence
		username.
--------------------------------------------------*/

void DRPC_UpdateUsername(void);


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


/*--------------------------------------------------
	void DRPC_Shutdown(void)

		Clears Everything Related to Discord
		Rich Presence. Only Runs When the
		Game Closes or Crashes.
--------------------------------------------------*/
void DRPC_Shutdown(void);

#endif // HAVE_DISCORDRPC
#endif // __DISCORD__
