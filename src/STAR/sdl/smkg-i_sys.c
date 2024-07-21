// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
//
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Changes by Graue <graue@oceanbase.org> are in the public domain.
//
//-----------------------------------------------------------------------------
/// \file  smkg-i_sys.c
/// \brief TSoURDt3rd main system stuff for SDL

#ifdef _MSC_VER
#pragma warning(disable : 4214 4244)
#endif

#ifdef HAVE_SDL
#include "SDL.h"

#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include "../smkg-i_sys.h"

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_I_CursedWindowMovement(int xd, int yd)
{
	//if (cv_tsourdt3rd_windowquake.value)
	//	return;
	SDL_SetWindowPosition(window, window_x + xd, window_y + yd);
}

#endif // HAVE_SDL
