// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_gamesdk.c
/// \brief Discord Game SDK handling

#ifdef HAVE_DISCORDGAMESDK // HAVE_DISCORDGAMESDK

#include "discord_game_sdk.h"

#include "../i_system.h"
#include "discord_gamesdk.h"
#include "discord.h"
#include "stun.h"
#include "../doomdef.h" // large list of defs, mainly
#include "../d_clisrv.h"
#include "../d_netcmd.h"
#include "../i_net.h"
#include "../g_game.h"
#include "../p_tick.h"
#include "../m_menu.h" // gametype_cons_t
#include "../r_things.h" // skins
#include "../mserv.h" // ms_RoomId
#include "../m_cond.h" // queries about emblems
#include "../z_zone.h"
#include "../byteptr.h"
#include "../i_tcp.h" // current_port

// ------------------------ //
//        Functions
// ------------------------ //

// =========
// CALLBACKS
// =========

void DISCORD_CALLBACK UpdateActivityCallback(void *data, enum EDiscordResult result)
{
	DISCORD_REQUIRE(result);
}

boolean DISCORD_CALLBACK RelationshipPassFilter(void *data, struct DiscordRelationship *relationship)
{
	return (relationship->type == DiscordRelationshipType_Friend);
}

boolean DISCORD_CALLBACK RelationshipSnowflakeFilter(void *data, struct DiscordRelationship *relationship)
{
	discordGameSDK_t *app = (discordGameSDK_t *)data;
	return (relationship->type == DiscordRelationshipType_Friend && relationship->user.id < app->user_id);
}

void DISCORD_CALLBACK OnRelationshipsRefresh(void *data)
{
	discordGameSDK_t *app = (discordGameSDK_t *)data;
	struct IDiscordRelationshipManager *module = app->relationships;

	struct DiscordActivity activity;

	INT32 unfiltered_count = 0;
	INT32 filtered_count = 0;

	module->filter(module, app, RelationshipPassFilter);
	DISCORD_REQUIRE(module->count(module, &unfiltered_count));

	module->filter(module, app, RelationshipSnowflakeFilter);
	DISCORD_REQUIRE(module->count(module, &filtered_count));

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "=== Cool Friends ===\n");
	for (INT32 i = 0; i < filtered_count; i += 1)
	{
		struct DiscordRelationship relationship;
		DISCORD_REQUIRE(module->get_at(module, i, &relationship));

		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "%lld %s#%s\n", relationship.user.id, relationship.user.username, relationship.user.discriminator);
	}
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "(%d friends less cool than you omitted)\n", unfiltered_count - filtered_count);
 
	memset(&activity, 0, sizeof(activity));

	sprintf(activity.details, "Hey Sonic! You have cooler than %d friends", unfiltered_count - filtered_count);
	sprintf(activity.state, "Hey Sonic! %d friends total", unfiltered_count);

	app->activities->update_activity(app->activities, &activity, app, UpdateActivityCallback);
}

void DISCORD_CALLBACK OnUserUpdated(void *data)
{
	discordGameSDK_t *app = (discordGameSDK_t *)data;
	struct DiscordUser user;

	app->users->get_current_user(app->users, &user);
	app->user_id = user.id;
}

void DISCORD_CALLBACK OnOAuth2Token(void *data, enum EDiscordResult result, struct DiscordOAuth2Token *token)
{
	if (result == DiscordResult_Ok)
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "OAuth2 token: %s\n", token->access_token);
	else
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "GetOAuth2Token failed with %d\n", (int)result);
}

void DISCORD_CALLBACK OnLobbyConnect(void *data, enum EDiscordResult result, struct DiscordLobby *lobby)
{
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "LobbyConnect returned %d\n", (int)result);
}

// ====
// MAIN
// ====

/*--------------------------------------------------
	void DRPC_UpdatePresence(void)

		See header file for description.
--------------------------------------------------*/

void DRPC_RunCallbacks(void)
{
	app.core->run_callbacks(app.core, 0);
}

void DRPC_UpdatePresence(void)
{
	discordGameSDK_t app;

	struct IDiscordUserEvents users_events;
	struct IDiscordActivityEvents activities_events;
	struct IDiscordRelationshipEvents relationships_events;
	struct DiscordCreateParams params;

	DiscordBranch branch;

	memset(&app, 0, sizeof(app));

	memset(&users_events, 0, sizeof(users_events));
	users_events.on_current_user_update = OnUserUpdated;

	memset(&activities_events, 0, sizeof(activities_events));

	memset(&relationships_events, 0, sizeof(relationships_events));
	relationships_events.on_refresh = OnRelationshipsRefresh;

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

	app.lobbies->connect_lobby_with_activity_secret(app.lobbies, "invalid_secret", &app, OnLobbyConnect);

	app.application->get_oauth2_token(app.application, &app, OnOAuth2Token);

	app.application->get_current_branch(app.application, &branch);
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_DEBUG, "Current branch %s\n", branch);

	app.relationships = app.core->get_relationship_manager(app.core);

	for (;;)
	{
		DISCORD_REQUIRE(app.core->run_callbacks(app.core));
		I_Sleep(cv_sleep.value);
	}
}

#endif //HAVE_DISCORDGAMESDK
