// SONIC ROBO BLAST 2 -- WITH DISCORD RPC BROUGHT TO YOU BY THE KART KREW (And Star lol)
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2020 by Kart Krew.
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

extern consvar_t cv_discordrp;
extern consvar_t cv_discordstreamer;
extern consvar_t cv_discordasks;
extern consvar_t cv_discordshowonstatus;
extern consvar_t cv_discordstatusmemes;
extern consvar_t cv_discordcharacterimagetype;
// Custom Things
extern consvar_t cv_customdiscorddetails;
extern consvar_t cv_customdiscordstate;
extern consvar_t cv_customdiscordlargeimagetype;
extern consvar_t cv_customdiscordsmallimagetype;
extern consvar_t cv_customdiscordlargecharacterimage;
extern consvar_t cv_customdiscordsmallcharacterimage;
extern consvar_t cv_customdiscordlargemapimage;
extern consvar_t cv_customdiscordsmallmapimage;
extern consvar_t cv_customdiscordlargemiscimage;
extern consvar_t cv_customdiscordsmallmiscimage;
extern consvar_t cv_customdiscordlargeimagetext;
extern consvar_t cv_customdiscordsmallimagetext;

extern char discordUserName[64];

extern struct discordInfo_s {
	UINT8 maxPlayers;
	UINT8 joinsAllowed;
	UINT8 whoCanInvite;
} discordInfo;

typedef struct discordRequest_s {
	char *username; // Discord user name.
	char *discriminator; // Discord discriminator (The little hashtag thing after the username). Separated for a "hide discriminators" cvar.
	char *userID; // The ID of the Discord user, gets used with Discord_Respond()
	
	// HAHAHA, no.
	// *Maybe* if it was only PNG I would boot up curl just to get AND convert this to Doom GFX,
	// but it can *also* be a JEPG, WebP, or GIF :)
	// Hey, wanna add ImageMagick as a dependency? :dying:
	//patch *avatar
	char *avatar; // VERY WIP

	struct discordRequest_s *next; // Next request in the list.
	struct discordRequest_s *prev; // Previous request in the list. Not used normally, but just in case something funky happens, this should repair the list.
} discordRequest_t;

extern discordRequest_t *discordRequestList;


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
	void DRPC_ShutDown(void)

		Clears Everything Related to Discord
		Rich Presence. Only Runs On Game Close
		or Game Crash.
--------------------------------------------------*/
void DRPC_ShutDown(void);

#endif // HAVE_DISCORDRPC
#endif // __DISCORD__
