// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief Unique TSoURDt3rd routines for SDL

#ifdef _MSC_VER
#pragma warning(disable : 4214 4244)
#endif

#ifdef HAVE_SDL
#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include "../smkg_pad_i_sys.h"

#include "../../../i_system.h"
#include "../../../screen.h"

// ------------------------ //
//        Variables
// ------------------------ //

// platform independant window location data
INT32 window_x = -1;
INT32 window_y = -1;

// ------------------------ //
//        Function
// ------------------------ //

//
// static void Impl_HandleWindowEvent(SDL_WindowEvent evt)
// Routine for handling TSoURDt3rd window events.
//
static void Impl_HandleWindowEvent(SDL_WindowEvent evt)
{
	switch (evt.event)
	{
		case SDL_WINDOWEVENT_MOVED:
			window_x = evt.data1;
			window_y = evt.data2;
			break;
		default:
			if (cv_fullscreen.value)
			{
				window_x = window_y = -1;
				break;
			}

			if (window_x == -1 || window_y == -1)
				SDL_GetWindowPosition(window, &window_x, &window_y);
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
			Impl_HandleWindowEvent(evt->window);
			break;
	}
}

#endif
