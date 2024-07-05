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

extern consvar_t cv_discordcustom_details, cv_discordcustom_state;
extern consvar_t cv_discordcustom_imagetype_large, cv_discordcustom_imagetype_small;
extern consvar_t cv_discordcustom_characterimage_large, cv_discordcustom_characterimage_small;
extern consvar_t cv_discordcustom_supercharacterimage_large, cv_discordcustom_supercharacterimage_small;
extern consvar_t cv_discordcustom_mapimage_large, cv_discordcustom_mapimage_small;
extern consvar_t cv_discordcustom_miscimage_large, cv_discordcustom_miscimage_small;
extern consvar_t cv_discordcustom_imagetext_large, cv_discordcustom_imagetext_small;
#endif

// ------------------------ //
//        Functions
// ------------------------ //

void Joinable_OnChange(void);
void Got_DiscordInfo(UINT8 **cp, INT32 playernum);

#endif // __DISCORD_CMDS__
