// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-lu_script.c
/// \brief TSoURDt3rd's Lua scripting basics (no this is not a tutorial)

#include "smkg-lu_main.h"
#include "../star_vars.h" // tsourdt3rd struct //
#include "../core/smkg-g_game.h" // tsourdt3rd_local //
#include "../misc/smkg-m_misc.h" // TSoURDt3rd_M_FindWordInTermTable() //

#include "../../lua_libs.h"

// ------------------------ //
//        Variables
// ------------------------ //

enum tsourdt3rd_blua_global_terms_e
{
	tsourdt3rd_events_eastermode,
	tsourdt3rd_events_egghunt,
	tsourdt3rd_events_egghuntbonuses,
	tsourdt3rd_events_eggs_total,
	tsourdt3rd_events_eggs_found,
	tsourdt3rd_events_eggs_collectedonmap,
	tsourdt3rd_events_eggs_current,
	tsourdt3rd_events_eggs_numonmap,
	tsourdt3rd_main_globalvar,
	tsourdt3rd_main_version,
	tsourdt3rd_main_versionstring,
	tsourdt3rd_files_loadedextras,
	tsourdt3rd_files_nomore_extras,
	tsourdt3rd_mods_autoloaded,
	tsourdt3rd_server_usingbuild_DEPRE, tsourdt3rd_server_usingbuild,
	tsourdt3rd_server_version_DEPRE, tsourdt3rd_server_version,
	tsourdt3rd_sounds_join,
	tsourdt3rd_sounds_leave,
	tsourdt3rd_sounds_synchfailure,
	tsourdt3rd_sounds_discord_requests,
	tsourdt3rd_extras_spawntf2dispenser
}; static const char *const tsourdt3rd_blua_global_terms_opt[] = {
	"eastermode",
	"AllowEasterEggHunt",
	"EnableEasterEggHuntBonuses",
	"TOTALEGGS",
	"foundeggs",
	"collectedmapeggs",
	"currenteggs",
	"numMapEggs",
	"tsourdt3rd",
	"TSOURDT3RDVERSION",
	"TSOURDT3RDVERSIONSTRING",
	"TSoURDt3rd_LoadedExtras",
	"TSoURDt3rd_NoMoreExtras",
	"autoloaded",
	"serverUsesTSoURDt3rd", "server_usingTSoURDt3rd",
	"serverTSoURDt3rdVersion", "server_TSoURDt3rdVersion",
	"STAR_JoinSFX",
	"STAR_LeaveSFX",
	"STAR_SynchFailureSFX",
	"DISCORD_RequestSFX",
	"SpawnTheDispenser",
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

INT32 TSoURDt3rd_LUA_PushGlobalVariables(lua_State *L, const char *word)
{
	TSoURDt3rd_t *tsourdt3rd_user = &TSoURDt3rdPlayers[consoleplayer];
	INT32 word_to_table_val = TSoURDt3rd_M_FindWordInTermTable(tsourdt3rd_blua_global_terms_opt, word, TSOURDT3RD_TERMTABLESEARCH_NORM);

	switch (word_to_table_val)
	{
		case tsourdt3rd_events_eastermode:
			lua_pushboolean(L, (tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER));
			return 1;
		case tsourdt3rd_events_egghunt:
		case tsourdt3rd_events_egghuntbonuses:
		case tsourdt3rd_events_eggs_total:
		case tsourdt3rd_events_eggs_found:
		case tsourdt3rd_events_eggs_collectedonmap:
		case tsourdt3rd_events_eggs_current:
		case tsourdt3rd_events_eggs_numonmap:
		{
			if (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER))
				break; //return luaL_error(L, "global variable %s has no unless easter mode is enabled!", tsourdt3rd_blua_global_terms_opt[word_to_table_val]);

			switch (word_to_table_val) // AGAIN!
			{
				case tsourdt3rd_events_egghunt:
					lua_pushboolean(L, AllowEasterEggHunt);
					break;
				case tsourdt3rd_events_egghuntbonuses:
					lua_pushboolean(L, EnableEasterEggHuntBonuses);
					break;
				case tsourdt3rd_events_eggs_total:
					lua_pushinteger(L, TOTALEGGS);
					break;
				case tsourdt3rd_events_eggs_found:
					lua_pushinteger(L, foundeggs);
					break;
				case tsourdt3rd_events_eggs_collectedonmap:
					lua_pushinteger(L, collectedmapeggs);
					break;
				case tsourdt3rd_events_eggs_current:
					lua_pushinteger(L, currenteggs);
					break;
				case tsourdt3rd_events_eggs_numonmap:
					lua_pushinteger(L, numMapEggs);
					break;
			}
			return 1;
		}

		case tsourdt3rd_main_globalvar:
			lua_pushboolean(L, true);
			return 1;
		case tsourdt3rd_main_version:
			lua_pushstring(L, TSOURDT3RDVERSION);
			return 1;
		case tsourdt3rd_main_versionstring:
			lua_pushstring(L, TSOURDT3RDVERSIONSTRING);
			return 1;

		case tsourdt3rd_files_loadedextras:
			lua_pushboolean(L, tsourdt3rd_currentEvent);
			return 1;
		case tsourdt3rd_files_nomore_extras:
			lua_pushboolean(L, ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata))));
			return 1;

		case tsourdt3rd_mods_autoloaded:
			lua_pushboolean(L, tsourdt3rd_local.autoloaded_mods);
			return 1;

		case tsourdt3rd_server_usingbuild_DEPRE:
		case tsourdt3rd_server_usingbuild:
			lua_pushboolean(L, tsourdt3rd_user->server_usingTSoURDt3rd);
			return 1;
		case tsourdt3rd_server_version_DEPRE:
		case tsourdt3rd_server_version:
			lua_pushinteger(L, tsourdt3rd_user->server_TSoURDt3rdVersion);
			return 1;

		case tsourdt3rd_sounds_join:
			lua_pushinteger(L, STAR_JoinSFX);
			return 1;
		case tsourdt3rd_sounds_leave:
			lua_pushinteger(L, STAR_LeaveSFX);
			return 1;
		case tsourdt3rd_sounds_synchfailure:
			lua_pushinteger(L, STAR_SynchFailureSFX);
			return 1;
		case tsourdt3rd_sounds_discord_requests:
			lua_pushinteger(L, DISCORD_RequestSFX);
			return 1;

		case tsourdt3rd_extras_spawntf2dispenser:
			lua_pushboolean(L, SpawnTheDispenser);
			return 1;

		default:
			break;
	}
	return 0;
}

boolean TSoURDt3rd_LUA_SetGlobalVariables(lua_State *L, const char *word)
{
	INT32 word_to_table_val = TSoURDt3rd_M_FindWordInTermTable(tsourdt3rd_blua_global_terms_opt, word, TSOURDT3RD_TERMTABLESEARCH_NORM);

	switch (word_to_table_val)
	{
		case tsourdt3rd_sounds_join:
		{
			sfxenum_t sound_id = luaL_checkinteger(L, 2);
			if (sound_id >= NUMSFX)
				return luaL_error(L, "sfx %d out of range (0 - %d)", sound_id, NUMSFX-1);
			STAR_JoinSFX = (INT32)luaL_checkinteger(L, 2);
			break;
		}
		case tsourdt3rd_sounds_leave:
		{
			sfxenum_t sound_id = luaL_checkinteger(L, 2);
			if (sound_id >= NUMSFX)
				return luaL_error(L, "sfx %d out of range (0 - %d)", sound_id, NUMSFX-1);
			STAR_LeaveSFX = (INT32)luaL_checkinteger(L, 2);
			break;
		}
		case tsourdt3rd_sounds_synchfailure:
		{
			sfxenum_t sound_id = luaL_checkinteger(L, 2);
			if (sound_id >= NUMSFX)
				return luaL_error(L, "sfx %d out of range (0 - %d)", sound_id, NUMSFX-1);
			STAR_SynchFailureSFX = (INT32)luaL_checkinteger(L, 2);
			break;
		}
		case tsourdt3rd_sounds_discord_requests:
		{
			sfxenum_t sound_id = luaL_checkinteger(L, 2);
			if (sound_id >= NUMSFX)
				return luaL_error(L, "sfx %d out of range (0 - %d)", sound_id, NUMSFX-1);
			DISCORD_RequestSFX = (INT32)luaL_checkinteger(L, 2);
			break;
		}

		case tsourdt3rd_extras_spawntf2dispenser:
		{
			if ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata)))
				return luaL_error(L, "global variable SpawnTheDispenser can't be used while the game is modified!");
			if (netgame)
				return luaL_error(L, "global variable SpawnTheDispenser can't be changed in netgames!");
			if (tsourdt3rd_local.autoloaded_mods)
				return luaL_error(L, "global variable SpawnTheDispenser can't be changed after autoloading add-ons!");
			SpawnTheDispenser = luaL_checkboolean(L, 2);
			break;
		}

		case tsourdt3rd_events_egghunt:
		case tsourdt3rd_events_egghuntbonuses:
		case tsourdt3rd_events_eggs_total:
		case tsourdt3rd_events_eggs_found:
		case tsourdt3rd_events_eggs_collectedonmap:
		case tsourdt3rd_events_eggs_current:
		case tsourdt3rd_events_eggs_numonmap:
		{
			if ((tsourdt3rd_local.autoloaded_mods || ((modifiedgame || usedCheats) && savemoddata)))
				return luaL_error(L, "global variable %s can't be used while the game is modified!", tsourdt3rd_blua_global_terms_opt[word_to_table_val]);
			if (!(tsourdt3rd_currentEvent & TSOURDT3RD_EVENT_EASTER))
				return luaL_error(L, "global variable %s is unless easter mode is enabled!", tsourdt3rd_blua_global_terms_opt[word_to_table_val]);
			if (netgame)
				return luaL_error(L, "global variable %s can't be changed in netgames!", tsourdt3rd_blua_global_terms_opt[word_to_table_val]);
			if (tsourdt3rd_local.autoloaded_mods)
				return luaL_error(L, "global variable %s can't be changed after autoloading add-ons!", tsourdt3rd_blua_global_terms_opt[word_to_table_val]);

			switch (word_to_table_val) // AGAIN!
			{
				case tsourdt3rd_events_egghunt:
					AllowEasterEggHunt = luaL_checkboolean(L, 2);
					break;
				case tsourdt3rd_events_egghuntbonuses:
					EnableEasterEggHuntBonuses = luaL_checkboolean(L, 2);
					break;
				case tsourdt3rd_events_eggs_total:
					TOTALEGGS = (INT32)luaL_checkinteger(L, 2);
					break;
				case tsourdt3rd_events_eggs_found:
					foundeggs = (INT32)luaL_checkinteger(L, 2);
					break;
				case tsourdt3rd_events_eggs_collectedonmap:
					collectedmapeggs = (INT32)luaL_checkinteger(L, 2);
					break;
				case tsourdt3rd_events_eggs_current:
					currenteggs = (INT32)luaL_checkinteger(L, 2);
					break;
				case tsourdt3rd_events_eggs_numonmap:
					numMapEggs = (INT32)luaL_checkinteger(L, 2);
					break;
			}
			break;
		}

		default:
			return false;
	}
	return true;
}
