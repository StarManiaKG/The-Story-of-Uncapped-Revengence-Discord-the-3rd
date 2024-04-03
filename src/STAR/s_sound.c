
#include "star_vars.h"
#include "ss_main.h"
#include "s_sound.h"
#include "m_menu.h"
#include "../g_game.h"
#include "../z_zone.h"

// ------------------------ //
//         Jukebox
// ------------------------ //
void M_ResetJukebox(boolean resetmusic)
{
	// Clear the Music Stuff //
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[consoleplayer];

	if (curplaying)
	{
		STAR_CONS_Printf(STAR_CONS_JUKEBOX, "Resetting Jukebox...\n");
		curplaying = NULL;
	}
	TSoURDt3rd->jukebox.musicPlaying = false;
	TSoURDt3rd->jukebox.initHUD = false;

	memset(&TSoURDt3rd->jukebox.musicTrack, 0, sizeof(TSoURDt3rd->jukebox.musicTrack));
	memset(&TSoURDt3rd->jukebox.musicName, 0, sizeof(TSoURDt3rd->jukebox.musicName));

	TSoURDt3rd->jukebox.stoppingTics = 0;

	// The Following Section Prevents Memory Leaks (Thanks SRB2 Discord!) //
	if (soundtestdefs)
	{
		if (currentMenu != &OP_TSoURDt3rdJukeboxDef && currentMenu != &SR_SoundTestDef)
		{
			Z_Free(soundtestdefs);
			soundtestdefs = NULL;
		}
	}

	// Reset Our Music Properties, and We're Done :) //
	if (Playing() && resetmusic)
	{
		if (TSoURDt3rd_InAprilFoolsMode() || !players[consoleplayer].powers[pw_super])
			S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0);
		else
			P_PlayJingle(&players[consoleplayer], JT_SUPER);
	}
	TSoURDt3rd_ControlMusicEffects();
}
