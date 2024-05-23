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
/// \file  discord_cmds.h
/// \brief Globalizes commands needed for Discord Rich Presence

#ifndef __DISCORD_CMDS__
#define __DISCORD_CMDS__

#include "discord.h"
#include "../command.h"

#ifdef HAVE_DISCORDSUPPORT

// ------------------------ //
//        Variables
// ------------------------ //

extern consvar_t cv_discordrp, cv_discordstreamer;
extern consvar_t cv_discordasks;
extern consvar_t cv_discordshowonstatus, cv_discordstatusmemes, cv_discordcharacterimagetype;

extern consvar_t cv_customdiscorddetails, cv_customdiscordstate;
extern consvar_t cv_customdiscordlargeimagetype, cv_customdiscordsmallimagetype;
extern consvar_t cv_customdiscordlargecharacterimage, cv_customdiscordsmallcharacterimage;
extern consvar_t cv_customdiscordlargesupercharacterimage, cv_customdiscordsmallsupercharacterimage;
extern consvar_t cv_customdiscordlargemapimage, cv_customdiscordsmallmapimage;
extern consvar_t cv_customdiscordlargemiscimage, cv_customdiscordsmallmiscimage;
extern consvar_t cv_customdiscordlargeimagetext, cv_customdiscordsmallimagetext;

#endif // HAVE_DISCORDSUPPORT
#endif // __DISCORD_CMDS__
