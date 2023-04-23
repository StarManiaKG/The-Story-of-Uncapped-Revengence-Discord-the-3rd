// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// DISCORD GAME SDK BY STARMANIAKG(#4884)
// Copyright (C) 2022-2023 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_gamesdk.h
/// \brief Discord Game SDK handling

#ifdef HAVE_DISCORDGAMESDK // HAVE_DISCORDGAMESDK

#include "i_system.h"
#include "d_clisrv.h"
#include "d_netcmd.h"
#include "i_net.h"
#include "g_game.h"
#include "p_tick.h"
#include "m_menu.h" // gametype_cons_t
#include "r_things.h" // skins
#include "mserv.h" // ms_RoomId
#include "m_cond.h" // queries about emblems
#include "z_zone.h"
#include "byteptr.h"
#include "stun.h"
#include "i_tcp.h" // current_port
#include "discord_gamesdk.h" // discord, duh
#include "doomdef.h" // large list of defs, mainly

// Please feel free to provide your own Discord app if you're making a new custom build :)
#define DISCORD_APPID "1013126566236135516"

// length of IP strings
#define IP_SIZE 21

/*--------------------------------------------------
	void DRPC_Init(void)

		See header file for description.
--------------------------------------------------*/
/*
void DRPC_Init(int argc, char** argv)
{
    struct Application app;
    memset(&app, 0, sizeof(app));

    struct IDiscordUserEvents users_events;
    memset(&users_events, 0, sizeof(users_events));
    users_events.on_current_user_update = OnUserUpdated;

    struct IDiscordActivityEvents activities_events;
    memset(&activities_events, 0, sizeof(activities_events));

    struct IDiscordRelationshipEvents relationships_events;
    memset(&relationships_events, 0, sizeof(relationships_events));
    relationships_events.on_refresh = OnRelationshipsRefresh;

    struct DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    params.client_id = 418559331265675294;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = &app;
    params.activity_events = &activities_events;
    params.relationship_events = &relationships_events;
    params.user_events = &users_events;
    DISCORD_REQUIRE(DiscordCreate(DISCORD_VERSION, &params, &app.core));

    app.users = app.core->get_user_manager(app.core);
    app.achievements = app.core->get_achievement_manager(app.core);
    app.activities = app.core->get_activity_manager(app.core);
    app.application = app.core->get_application_manager(app.core);
    app.lobbies = app.core->get_lobby_manager(app.core);

    app.lobbies->connect_lobby_with_activity_secret(
      app.lobbies, "invalid_secret", &app, OnLobbyConnect);

    app.application->get_oauth2_token(app.application, &app, OnOAuth2Token);

    DiscordBranch branch;
    app.application->get_current_branch(app.application, &branch);
    printf("Current branch %s\n", branch);

    app.relationships = app.core->get_relationship_manager(app.core);

    for (;;) {
        DISCORD_REQUIRE(app.core->run_callbacks(app.core));

#ifdef _WIN32
        Sleep(16);
#else
        usleep(16 * 1000);
#endif
    }

    return 0;
}
*/

/*--------------------------------------------------
	void DRPC_UpdatePresence(void)

		See header file for description.
--------------------------------------------------*/
void DRPC_UpdatePresence(void)
{	
	char sonic[64] = "sonic";
	CONS_Printf("%s", sonic);
}

#endif //HAVE_DISCORDGAMESDK
