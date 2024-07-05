// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  s_sound.c
/// \brief TSoURDt3rd's unique sound library

#include "star_vars.h"
#include "ss_main.h"
#include "s_sound.h"
#include "m_menu.h"
#include "../g_game.h"
#include "../z_zone.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void M_ResetJukebox(boolean resetmusic)
// Resets TSoURDt3rd's jukebox. Can reset level music too, if specified.
//
void M_ResetJukebox(boolean resetmusic)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];

	if (TSoURDt3rd->jukebox.curtrack)
	{
		STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Jukebox reset.\n");
		TSoURDt3rd->jukebox.prevtrack = TSoURDt3rd->jukebox.curtrack;
		curplaying = NULL;
	}
	TSoURDt3rd->jukebox.curtrack = NULL;
	TSoURDt3rd->jukebox.initHUD = false;

	TSoURDt3rd->jukebox.tics = 0;

	if (currentMenu == &OP_TSoURDt3rdJukeboxDef && currentMenu == &SR_SoundTestDef)
	{
		Z_Free(soundtestdefs);
		soundtestdefs = NULL;
	}

	if (resetmusic)
	{
		if (playeringame[consoleplayer] && players[consoleplayer].powers[pw_super])
			P_PlayJingle(&players[consoleplayer], JT_SUPER);
		else
		{
			S_ChangeMusicEx(TSoURDt3rd_DetermineLevelMusic(), mapmusflags, true, mapmusposition, 0, 0);
			if (!S_MusicPlaying() && playeringame[consoleplayer])
				P_RestoreMusic(&players[consoleplayer]);
		}
		TSoURDt3rd_ControlMusicEffects();
	}
}
