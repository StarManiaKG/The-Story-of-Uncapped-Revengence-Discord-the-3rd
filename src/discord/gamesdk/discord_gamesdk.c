// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_gamesdk.c
/// \brief Discord Game SDK handling and routines

#ifdef HAVE_DISCORDGAMESDK

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <unistd.h>
#include <string.h>

#include <discord_game_sdk.h>
#include "../discord.h"

#include "../../m_menu.h"
#include "../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

// Holds all of the core data for Discord's Game SDK
typedef struct discordGameSDK_s {
	struct IDiscordCore *core;
	struct IDiscordUserManager *users;
	struct IDiscordAchievementManager *achievements;
	struct IDiscordActivityManager *activities;
	struct IDiscordRelationshipManager *relationships;
	struct IDiscordApplicationManager *application;
	struct IDiscordLobbyManager *lobbies;

	DiscordUserId user_id;
	boolean initialized;
} discordGameSDK_t;
static discordGameSDK_t DGSDK;

//#define _Bool boolean

// ------------------------ //
//        Functions
// ------------------------ //

// =========
// CALLBACKS
// =========

static void DISCORD_CALLBACK UpdateActivityCallback(void *data, enum EDiscordResult result)
{
	(void)data;
	(void)result;
}

static _Bool DISCORD_CALLBACK RelationshipPassFilter(void *data, struct DiscordRelationship *relationship)
{
	(void)data;
	if (relationship == NULL)
		return false;
	return (relationship->type == DiscordRelationshipType_Friend);
}

static _Bool DISCORD_CALLBACK ActivityFilter(void *data, struct DiscordActivity *activity)
{
	(void)data;
	if (activity == NULL)
		return false;
	return (activity->type == DiscordActivityType_Playing);
}

static _Bool DISCORD_CALLBACK RelationshipSnowflakeFilter(void *data, struct DiscordRelationship *relationship)
{
	discordGameSDK_t *SDK_AppData = (discordGameSDK_t *)data;
	if (SDK_AppData == NULL || relationship == NULL)
		return false;
	return (relationship->type == DiscordRelationshipType_Friend && (relationship->user.id < SDK_AppData->user_id));
}

static void DISCORD_CALLBACK OnRelationshipsRefresh(void *data)
{
	discordGameSDK_t *SDK_AppData = (discordGameSDK_t *)data;
	struct IDiscordRelationshipManager *module;
	struct DiscordRelationship relationship;
	struct DiscordActivity activity;
	INT32 unfiltered_count = 0, filtered_count = 0;

	memset(&relationship, 0, sizeof(relationship));
	memset(&activity, 0, sizeof(activity));
	if (SDK_AppData == NULL || SDK_AppData->initialized == false)
		return;

	module = SDK_AppData->relationships;
	module->filter(module, SDK_AppData, RelationshipPassFilter);
	module->filter(module, SDK_AppData, RelationshipSnowflakeFilter);

	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "=== Cool Friends ===\n");
	for (INT32 i = 0; i < filtered_count; i += 1)
	{
		module->get_at(module, i, &relationship);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "%lld %s#%s\n", relationship.user.id, relationship.user.username, relationship.user.discriminator);
	}
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "(%d friends less cool than you omitted)\n", unfiltered_count - filtered_count);

	sprintf(activity.details, "Hey Sonic! You have cooler than %d friends", unfiltered_count - filtered_count);
	sprintf(activity.state, "Hey Sonic! %d friends total", unfiltered_count);
	SDK_AppData->activities->update_activity(SDK_AppData->activities, &activity, SDK_AppData, UpdateActivityCallback);
}

static void DISCORD_CALLBACK UpdateActivity(void *callback_data, enum EDiscordResult result)
{
	discordGameSDK_t *SDK_AppData = (discordGameSDK_t *)callback_data;
	struct IDiscordActivityManager *module;
	struct IDiscordRelationshipManager *relation_module;
	struct DiscordActivity activity;
	(void)result;

	memset(&activity, 0, sizeof(activity));
	if (SDK_AppData == NULL || SDK_AppData->initialized == false)
		return;

	module = SDK_AppData->activities;

#if 0
	relation_module = SDK_AppData->relationships;
	relation_module->filter(relation_module, SDK_AppData, RelationshipPassFilter);
	relation_module->filter(relation_module, SDK_AppData, RelationshipSnowflakeFilter);
	relation_module->count(relation_module, &filtered_count);
#else
	(void)relation_module;
#endif
	module->update_activity(module, &activity, SDK_AppData, UpdateActivityCallback);
}

static void DISCORD_CALLBACK OnUserUpdated(void *data)
{
	discordGameSDK_t *SDK_AppData = (discordGameSDK_t *)data;
	struct DiscordUser user;

	memset(&user, 0, sizeof(user));
	if (SDK_AppData == NULL || SDK_AppData->initialized == false)
		return;

	SDK_AppData->users->get_current_user(SDK_AppData->users, &user);
	SDK_AppData->user_id = user.id;

	CONS_Printf("New user is %s\n", user.username);
	CONS_Printf("Discriminator is %s\n", user.discriminator);
	CONS_Printf("Avatar is %s\n", user.avatar);
}

static void DISCORD_CALLBACK OnOAuth2Token(void *data, enum EDiscordResult result, struct DiscordOAuth2Token *auth_token)
{
	(void)data;
	if (result != DiscordResult_Ok)
		STAR_CONS_Printf(STAR_CONS_DISCORD|STAR_CONS_ERROR, "GetOAuth2Token failed with %d\n", (int)result);
	else
		STAR_CONS_Printf(STAR_CONS_DISCORD, "OAuth2 token: %s\n", auth_token->access_token);
}

static void DISCORD_CALLBACK OnLobbyConnect(void *data, enum EDiscordResult result, struct DiscordLobby *lobby)
{
	(void)data;
	(void)lobby;
}

#if 0
/*--------------------------------------------------
	static void DISCORD_CALLBACK DGSDK_HandleReady(void *data)

		Callback function, ran when the game connects to Discord.

	Input Arguments:-
		user - Struct containing Discord user info.

	Return:-
		None
--------------------------------------------------*/
static void DGSDK_HandleReady(void *data)
{
	OnUserUpdated(data);
	//DISC_HandleConnected(user->username, user->discriminator, user->userId);
	CONS_Printf("DGSDK_HandleReady\n");
}
#endif

// ====
// MAIN
// ====

/*--------------------------------------------------
	boolean DISC_HandleInit(void)

		See header file for description.
--------------------------------------------------*/
boolean DISC_HandleInit(void)
{
	struct IDiscordUserEvents users_events;
	{
		memset(&users_events, 0, sizeof(users_events));
		users_events.on_current_user_update = OnUserUpdated;
	}
	struct IDiscordActivityEvents activities_events;
	{
		memset(&activities_events, 0, sizeof(activities_events));
	}
	struct IDiscordRelationshipEvents relationships_events;
	{
		memset(&relationships_events, 0, sizeof(relationships_events));
		relationships_events.on_refresh = OnRelationshipsRefresh;
	}
	struct DiscordCreateParams params;
	{
		memset(&params, 0, sizeof(params));
	}
	{
		memset(&DGSDK, 0, sizeof(DGSDK));
	}

	DiscordCreateParamsSetDefault(&params);
	//params.client_id = DISCORD_APPID;
	//params.flags = DiscordCreateFlags_Default;
	params.flags = DiscordCreateFlags_NoRequireDiscord;
	//params.event_data = &DGSDK;
	params.user_events = &users_events;
	params.activity_events = &activities_events;
	params.relationship_events = &relationships_events;

	enum EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &DGSDK.core);
	if (result != DiscordResult_Ok || DGSDK.core == NULL)
	{
		memset(&DGSDK, 0, sizeof(DGSDK));
		return;
	}
	DGSDK.users = DGSDK.core->get_user_manager(DGSDK.core);
	DGSDK.achievements = DGSDK.core->get_achievement_manager(DGSDK.core);
	DGSDK.activities = DGSDK.core->get_activity_manager(DGSDK.core);
	DGSDK.application = DGSDK.core->get_application_manager(DGSDK.core);
#if 0
		DiscordBranch branch;
		DGSDK.application->get_oauth2_token(DGSDK.application, &DGSDK, OnOAuth2Token);
		DGSDK.application->get_current_branch(DGSDK.application, &branch);
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR, "DISCORD: Current branch %s\n", branch);
#endif
	DGSDK.lobbies = DGSDK.core->get_lobby_manager(DGSDK.core);
		DGSDK.lobbies->connect_lobby_with_activity_secret(DGSDK.lobbies, DISC_XORIPString("invalid_secret"), &DGSDK, OnLobbyConnect);
	DGSDK.relationships = DGSDK.core->get_relationship_manager(DGSDK.core);
	DGSDK.user_id = -1;
	DGSDK.initialized = true;

	DISC_HandleConnected("deez", "nuts", "inyourmouth");
}

/*--------------------------------------------------
	void DISC_UpdatePresence(void)

		See header file for description.
--------------------------------------------------*/
void DISC_UpdatePresence(void)
{
	struct DiscordActivity activity;
	memset(&activity, 0, sizeof(activity));

	char *client_joinSecret = NULL;
	char *server_partyID = NULL;

	activity.instance = true;
	activity.type = DiscordActivityType_Playing;
	activity.application_id = DISCORD_APPID;

	if (DGSDK.initialized == false)
	{
		// Integration isn't enabled!
		// Technically, an alternate way of disabling game activity. <comment below gives context to this>
		return;
	}

	if (discordInfo.connected == false || cv_discordintegration.value == 1)
	{
		// TehRealSalt:
		// User either doesn't want to show their game information, or can't show presence.
		// So, update with empty presence.
		// This just shows that they're playing. (If that's too much, then they should disable game activity :V)
		DISC_EmptyRequests();
	}
	else
	{
		DISC_SetActivityStatus(
			activity.details, activity.state,
			activity.assets.large_image, activity.assets.large_text,
			activity.assets.small_image, activity.assets.small_text,
			(time_t *)&activity.timestamps.start, (time_t *)&activity.timestamps.end,
			&client_joinSecret,
			&server_partyID, &activity.party.size.current_size, &activity.party.size.max_size
		);
		snprintf(activity.secrets.join, DISC_STATUS_MAX_STRING_SIZE, "%s", client_joinSecret);
		snprintf(activity.party.id, DISC_STATUS_MAX_STRING_SIZE, "%s", server_partyID);
	}

	//DGSDK.activities->update_activity(DGSDK.activities, &activity, &DGSDK, UpdateActivity);
	DGSDK.activities->update_activity(DGSDK.activities, &activity, &DGSDK, UpdateActivityCallback);
}

#if 0
struct DiscordState
{
   discord::User currentUser;
   std::unique_ptr<discord::Core> core;
};

DiscordState state{};
discord::Core* core;
auto result = discord::Core::Create(..., DiscordCreateFlags_Default, &core);

state.core.reset(core);
if (state.core)
{
   state.core->SetLogHook(discord::LogLevel::Debug, [](discord::LogLevel level, const char* message) {
	   qDebug() << "Log(" << static_cast<uint32_t>(level) << "): " << message << "\n";
   });

   core->UserManager().OnCurrentUserUpdate.Connect([&state]()
   {
	   state.core->UserManager().GetCurrentUser(&state.currentUser);
	   qDebug() << "Current user updated: " << state.currentUser.GetUsername() << state.currentUser.GetDiscriminator() << "\n";
   });
}
else
   qDebug() << "Failed to instantiate discord core! (err " << static_cast<int>(result) << ")\n";
#endif

/*--------------------------------------------------
	void DISC_RunCallbacks(void)

		See header file for description.
--------------------------------------------------*/
void DISC_RunCallbacks(void)
{
	if (DGSDK.initialized == false)
	{
		return;
	}
	DGSDK.core->run_callbacks(DGSDK.core);
}

/*--------------------------------------------------
	void DISC_Respond(const char *userID, DISC_RequestReply_t response)

		See header file for description.

	Input Arguments:-
		None

	Return:-
		None
--------------------------------------------------*/
void DISC_Respond(const char *userID, DISC_RequestReply_t response)
{
	(void)userID;
	(void)response;
}

/*--------------------------------------------------
	void DISC_RemoveRequest(DISC_Request_t *removeRequest)

		See header file for description.

	Input Arguments:-
		None

	Return:-
		None
--------------------------------------------------*/
void DISC_RemoveRequest(DISC_Request_t *removeRequest)
{
	(void)removeRequest;
}

/*--------------------------------------------------
	void DISC_HandleQuit(void)

		See header file for description.
--------------------------------------------------*/
void DISC_HandleQuit(void)
{
	//DGSDK.core->destroy(DGSDK.core);
	memset(&DGSDK, 0, sizeof(DGSDK));
}

#endif //HAVE_DISCORDGAMESDK
