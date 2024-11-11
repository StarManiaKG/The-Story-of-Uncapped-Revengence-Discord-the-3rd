// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-s_audio.c
/// \brief TSoURDt3rd's cool and groovy jukebox features

#include "smkg-s_audio.h"
#include "../smkg-jukebox.h"
#include "../menus/smkg_m_func.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean TSoURDt3rd_S_CanModifyMusic(char *menu_mus_origin)
// Prevents TSoURDt3rd's music stuff, like Jukebox music, from being forcibly reset or modified. (YAY!)
//
boolean TSoURDt3rd_S_CanModifyMusic(char *menu_mus_origin)
{
	if (menuactive && (menu_mus_origin != NULL && *menu_mus_origin != '\0') && tsourdt3rd_currentMenu != NULL)
		return false;

	if (!tsourdt3rd_global_jukebox->playing)
		return true;

	if (paused)
		S_ResumeAudio();
	return false;
}
