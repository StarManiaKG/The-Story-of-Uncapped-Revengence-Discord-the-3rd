// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief Unique TSoURDt3rd gamepad routines for SDL

#ifdef _MSC_VER
#pragma warning(disable : 4214 4244)
#endif

#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include "../smkg-i_sys.h"
#include "../smkg-cvars.h"
#include "../smkg-st_hud.h"

#include "../drrr/kg_input.h"

#include "../../lua_hook.h"
#include "../../i_system.h"
#include "../../screen.h"

#ifdef HAVE_DISCORDSUPPORT
#include "../../discord/discord.h"
#endif

// ------------------------ //
//        Variables
// ------------------------ //

// platform independant window location data
INT32 window_x = -1;
INT32 window_y = -1;

// ------------------------ //
//        Function
// ------------------------ //

#ifdef HAVE_SDL
static void Impl_TSoURDt3rd_HandleWindowEvent(SDL_WindowEvent evt)
{
	switch (evt.event)
	{
		case SDL_WINDOWEVENT_MOVED:
			window_x = evt.data1;
			window_y = evt.data2;
			break;
		default:
			if (window_x == -1 || window_y == -1)
				SDL_GetWindowPosition(window, &window_x, &window_y);

			if (cv_fullscreen.value)
				window_x = window_y = -1;

			break;
	}
}

//
// void TSoURDt3rd_I_GetEvent(SDL_Event *evt)
// Gets SDL events for TSoURDt3rd.
//
void TSoURDt3rd_I_GetEvent(SDL_Event *evt)
{
	switch (evt->type)
	{
		case SDL_WINDOWEVENT:
			Impl_TSoURDt3rd_HandleWindowEvent(evt->window);
			break;
	}
}

#endif // HAVE_SDL

//
// void TSoURDt3rd_I_FinishUpdate(void)
// Updates the contents of the screen.
//
void TSoURDt3rd_I_FinishUpdate(void)
{
#ifdef HAVE_DISCORDSUPPORT
	if (discordRequestList != NULL)
		TSoURDt3rd_ST_AskToJoinEnvelope();
#endif

	if (cv_tsourdt3rd_video_showtps.value)
		TSoURDt3rd_SCR_DisplayTpsRate();
}
