// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2022-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-lu_base.c
/// \brief TSoURDt3rd's base Lua library

#include "smkg-lu_main.h"
#include "../core/smkg-p_pads.h"

#include "../../lua_libs.h"
#include "../../d_player.h"
#include "../../p_local.h"
#include "../../s_sound.h"

// ------------------------ //
//        Functions
// ------------------------ //

static int lib_pLEDController(lua_State *L)
{
	INT32 player = (UINT8)luaL_checkinteger(L, 1);
	UINT8 red = (UINT8)luaL_checkinteger(L, 2);
	UINT8 green = (UINT8)luaL_checkinteger(L, 3);
	UINT8 blue = (UINT8)luaL_checkinteger(L, 4);

	//NOHUD

	TSoURDt3rd_P_Pads_SetIndicatorColor(player, red, green, blue);

	return 0;
}

static int lib_pRumbleController(lua_State *L)
{
	player_t *player = NULL;
	fixed_t low = luaL_checkfixed(L, 2);
	fixed_t high = luaL_checkfixed(L, 3);
	tic_t duration_tics = luaL_checkinteger(L, 4);

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
	TSoURDt3rd_P_Pads_PlayerDeviceRumble(player, low, high, duration_tics);

	return 0;
}

static int lib_pRumbleControllerTriggers(lua_State *L)
{
	player_t *player = NULL;
	fixed_t left_strength = luaL_checkfixed(L, 2);
	fixed_t right_strength = luaL_checkfixed(L, 3);
	tic_t duration_tics = luaL_checkinteger(L, 4);

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
	TSoURDt3rd_P_Pads_PlayerDeviceRumbleTriggers(player, left_strength, right_strength, duration_tics);

	return 0;
}

static int lib_pPauseControllerRumble(lua_State *L)
{
	player_t *player = NULL;
	boolean rumbling_paused = luaL_checkboolean(L, 2);
	boolean trigger_rumbling_paused = luaL_checkboolean(L, 3);

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
	TSoURDt3rd_P_Pads_PauseDeviceRumble(player, rumbling_paused, trigger_rumbling_paused);

	return 0;
}

static int lib_pResetControllerRumble(lua_State *L)
{
	INT32 player = luaL_checkinteger(L, 1);

	//NOHUD

	TSoURDt3rd_P_Pads_ResetDeviceRumble(player);

	return 0;
}

static int lib_sGetSpeedMusic(lua_State *L)
{
	player_t *player = NULL;

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
	{
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
		if (!player)
			return LUA_ErrInvalid(L, "player_t");
	}
	if (!player || P_IsLocalPlayer(player))
		lua_pushinteger(L, S_GetSpeedMusic());
	else
		lua_pushnil(L);

	return 1;
}

static int lib_sPitchMusic(lua_State *L)
{
	fixed_t fixedpitch = luaL_checkfixed(L, 1);
	float pitch = FIXED_TO_FLOAT(fixedpitch);
	player_t *player = NULL;

	//NOHUD

	if (!lua_isnone(L, 2) && lua_isuserdata(L, 2))
	{
		player = *((player_t **)luaL_checkudata(L, 2, META_PLAYER));
		if (!player)
			return LUA_ErrInvalid(L, "player_t");
	}
	if (!player || P_IsLocalPlayer(player))
		S_PitchMusic(pitch);

	return 0;
}

static int lib_sGetPitchMusic(lua_State *L)
{
	player_t *player = NULL;

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
	{
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
		if (!player)
			return LUA_ErrInvalid(L, "player_t");
	}
	if (!player || P_IsLocalPlayer(player))
		lua_pushinteger(L, S_GetPitchMusic());
	else
		lua_pushnil(L);

	return 1;
}

static int lib_sGetInternalMusicVolume(lua_State *L)
{
	player_t *player = NULL;

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
	{
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
		if (!player)
			return LUA_ErrInvalid(L, "player_t");
	}
	if (!player || P_IsLocalPlayer(player))
		lua_pushinteger(L, (UINT32)S_GetInternalMusicVolume());
	else
		lua_pushnil(L);

	return 1;
}

static int lib_sSetInternalSfxVolume(lua_State *L)
{
	UINT32 sfxvolume = (UINT32)luaL_checkinteger(L, 1);
	player_t *player = NULL;

	//NOHUD

	if (!lua_isnone(L, 2) && lua_isuserdata(L, 2))
	{
		player = *((player_t **)luaL_checkudata(L, 2, META_PLAYER));
		if (!player)
			return LUA_ErrInvalid(L, "player_t");
	}
	if (!player || P_IsLocalPlayer(player))
	{
		S_SetInternalSfxVolume(sfxvolume);
		lua_pushboolean(L, true);
	}
	else
		lua_pushnil(L);

	return 1;
}

static int lib_sGetInternalSfxVolume(lua_State *L)
{
	player_t *player = NULL;

	//NOHUD

	if (!lua_isnone(L, 1) && lua_isuserdata(L, 1))
	{
		player = *((player_t **)luaL_checkudata(L, 1, META_PLAYER));
		if (!player)
			return LUA_ErrInvalid(L, "player_t");
	}
	if (!player || P_IsLocalPlayer(player))
		lua_pushinteger(L, (UINT32)S_GetInternalSfxVolume());
	else
		lua_pushnil(L);

	return 1;
}

static luaL_Reg tsourdt3d_base_lib[] = {
	{"P_LEDController",lib_pLEDController},
	{"P_RumbleController",lib_pRumbleController},
	{"P_RumbleControllerTriggers",lib_pRumbleControllerTriggers},
	{"P_PauseControllerRumble",lib_pPauseControllerRumble},
	{"P_ResetControllerRumble",lib_pResetControllerRumble},

	{"S_GetSpeedMusic",lib_sGetSpeedMusic},
	{"S_PitchMusic",lib_sPitchMusic},
	{"S_GetPitchMusic",lib_sGetPitchMusic},
	{"S_GetInternalMusicVolume", lib_sGetInternalMusicVolume},
	{"S_SetInternalSfxVolume", lib_sSetInternalSfxVolume},
	{"S_GetInternalSfxVolume", lib_sGetInternalSfxVolume},

	{NULL, NULL} // FINISHED!
};

int TSoURDt3rd_LUA_BaseLib(lua_State *L)
{
	// Set global functions
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	luaL_register(L, "tsourdt3rd", tsourdt3d_base_lib);
	return 0;
}
