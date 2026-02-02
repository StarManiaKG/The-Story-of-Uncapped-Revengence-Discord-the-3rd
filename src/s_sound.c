// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2025 by Sonic Team Junior.
// Copyright (C) 2025-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  s_sound.c
/// \brief System-independent sound and music routines

#include "doomdef.h"
#include "doomstat.h"
#include "command.h"
#include "g_game.h"
#include "m_argv.h"
#include "r_main.h" // R_PointToAngle2() used to calc stereo sep.
#include "r_skins.h" // for skins
#include "i_system.h"
#include "i_sound.h"
#include "s_sound.h"
#include "w_wad.h"
#include "z_zone.h"
#include "d_main.h"
#include "r_sky.h" // skyflatnum
#include "p_local.h" // camera info
#include "fastcmp.h"
#include "m_misc.h" // for tunes command
#include "m_cond.h" // for conditionsets
#include "v_video.h" // V_ThinStringWidth
#include "lua_hook.h" // MusicChange hook

#ifdef HW3SOUND
#include "hardware/hw3sound.h" // 3D Sound Interface
#endif

// TSoURDt3rd
#include "STAR/smkg-cvars.h"
#include "STAR/core/smkg-s_exmusic.h" // EXMusic
#include "STAR/core/smkg-s_jukebox.h" // Jukebox junk
#include "m_random.h" // M_RandomChance

// Music player command
static void Command_Tunes_f(void);

// Music mixer toggles
static void MusicMixer_OnChange(void);

// Sound system toggles
static void GameMIDIMusic_OnChange(void);
static void GameSounds_OnChange(void);
static void GameDigiMusic_OnChange(void);
static void MusicPref_OnChange(void);
#ifdef HAVE_OPENMPT
static void ModFilter_OnChange(void);
#endif

// Sample rate that all game audio will run at.
static CV_PossibleValue_t samplerate_cons_t[] = {
	{8000, "8000"}, {11025, "11025"}, {22050, "22050"}, {32000, "32000"}, //Alam: For easy hacking?
	{44100, "44100"}, {48000, "48000"}, // modern sample rates
	{0, NULL}
};
consvar_t cv_samplerate = CVAR_INIT ("samplerate", "44100", CV_SAVE|CV_CALL|CV_NOINIT, samplerate_cons_t, MusicMixer_OnChange);

// Buffersize for all sounds.
// Higher amounts are faster but cause more delays.
//Alam: 1KB samplecount at 22050hz is 46.439909297052154195011337868481ms of buffer
static CV_PossibleValue_t buffersize_cons_t[] = {
	{256, "256"}, {512, "512"}, {1024, "1024"}, // i mean you *can* use them if you want
	{2048, "2048"}, {4096, "4096"}, // modern buffer sizes
	{0, NULL}
};
consvar_t cv_buffersize = CVAR_INIT ("buffersize", "2048", CV_SAVE|CV_CALL|CV_NOINIT, buffersize_cons_t, MusicMixer_OnChange);

// stereo reverse
consvar_t stereoreverse = CVAR_INIT ("stereoreverse", "Off", CV_SAVE, CV_OnOff, NULL);

// if true, all sounds are loaded at game startup
static consvar_t precachesound = CVAR_INIT ("precachesound", "Off", CV_SAVE, CV_OnOff, NULL);

// actual general (maximum) sound & music volume, saved into the config
CV_PossibleValue_t soundvolume_cons_t[] = {{0, "MIN"}, {31, "MAX"}, {0, NULL}};
consvar_t cv_soundvolume = CVAR_INIT ("soundvolume", "16", CV_SAVE, soundvolume_cons_t, NULL);
consvar_t cv_digmusicvolume = CVAR_INIT ("digmusicvolume", "16", CV_SAVE, soundvolume_cons_t, NULL);
consvar_t cv_midimusicvolume = CVAR_INIT ("midimusicvolume", "16", CV_SAVE, soundvolume_cons_t, NULL);

// accessibility for those who need to visualize sounds and audio
caption_t closedcaptions[NUMCAPTIONS];
static void Captioning_OnChange(void)
{
	S_ResetCaptions();
	if (cv_closedcaptioning.value && gamestate != GS_NULL) // STOP BEEPING ON STARTUP
		S_StartSoundFromEverywhere(sfx_menu1);
}
void S_ResetCaptions(void)
{
	UINT8 i;
	for (i = 0; i < NUMCAPTIONS; i++)
	{
		closedcaptions[i].c = NULL;
		closedcaptions[i].s = NULL;
		closedcaptions[i].t = 0;
		closedcaptions[i].b = 0;
	}
}
consvar_t cv_closedcaptioning = CVAR_INIT ("closedcaptioning", "Off", CV_SAVE|CV_CALL, CV_OnOff, Captioning_OnChange);

// number of channels available
static void SetChannelsNum(void);
consvar_t cv_numChannels = CVAR_INIT ("snd_channels", "32", CV_SAVE|CV_CALL, CV_Unsigned, SetChannelsNum);

// Surround sound
static consvar_t surround = CVAR_INIT ("surround", "Off", CV_SAVE, CV_OnOff, NULL);

// Reset music
consvar_t cv_resetmusic = CVAR_INIT ("resetmusic", "Off", CV_SAVE, CV_OnOff, NULL);
consvar_t cv_resetmusicbyheader = CVAR_INIT ("resetmusicbyheader", "Yes", CV_SAVE, CV_YesNo, NULL);

// 1-up audio preference
static CV_PossibleValue_t cons_1upsound_t[] = {
	{0, "Jingle"},
	{1, "Sound"},
	{0, NULL}
};
consvar_t cv_1upsound = CVAR_INIT ("1upsound", "Jingle", CV_SAVE, cons_1upsound_t, NULL);

// Sound system toggles, saved into the config
consvar_t cv_gamedigimusic = CVAR_INIT ("digimusic", "On", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, GameDigiMusic_OnChange);
consvar_t cv_gamemidimusic = CVAR_INIT ("midimusic", "On", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, GameMIDIMusic_OnChange);
consvar_t cv_gamesounds = CVAR_INIT ("sounds", "On", CV_SAVE|CV_CALL|CV_NOINIT, CV_OnOff, GameSounds_OnChange);

// Music preference
static CV_PossibleValue_t cons_musicpref_t[] = {
	{0, "Digital"},
	{1, "MIDI"},
	{0, NULL}
};
consvar_t cv_musicpref = CVAR_INIT ("musicpref", "Digital", CV_SAVE|CV_CALL|CV_NOINIT, cons_musicpref_t, MusicPref_OnChange);

// Window focus sound sytem toggles
consvar_t cv_playmusicifunfocused = CVAR_INIT ("playmusicifunfocused", "No", CV_SAVE, CV_YesNo, NULL);
consvar_t cv_playsoundsifunfocused = CVAR_INIT ("playsoundsifunfocused", "No", CV_SAVE, CV_YesNo, NULL);

#ifdef HAVE_OPENMPT
// OpenMPT settings
openmpt_module *openmpt_mhandle = NULL;
static CV_PossibleValue_t interpolationfilter_cons_t[] = {{0, "Default"}, {1, "None"}, {2, "Linear"}, {4, "Cubic"}, {8, "Windowed sinc"}, {0, NULL}};
consvar_t cv_modfilter = CVAR_INIT ("modfilter", "0", CV_SAVE|CV_CALL, interpolationfilter_cons_t, ModFilter_OnChange);
#endif

// Display song credits
static CV_PossibleValue_t songcredits_cons_t[] = {{0, "Off"}, {1, "Simple"}, {2, "Dynamic"}, {0, NULL}};
consvar_t cv_songcredits = CVAR_INIT ("songcredits", "Dynamic", CV_SAVE, songcredits_cons_t, NULL);
consvar_t cv_songcredits_debug = CVAR_INIT ("songcredits_debug", "Off", 0, CV_OnOff, NULL);

// when to clip out sounds
// Does not fit the large outdoor areas.
// added 2-2-98 in 8 bit volume control (before (1200*0x10000))
#define S_CLIPPING_DIST (1536*0x10000)

// Distance to origin when sounds should be maxed out.
// This should relate to movement clipping resolution
// (see BLOCKMAP handling).
// Originally: (200*0x10000).
// added 2-2-98 in 8 bit volume control (before (160*0x10000))
#define S_CLOSE_DIST (160*0x10000)

// added 2-2-98 in 8 bit volume control (before remove the +4)
#define S_ATTENUATOR ((S_CLIPPING_DIST-S_CLOSE_DIST)>>(FRACBITS+4))

#define NORM_PITCH 128
#define NORM_PRIORITY 64
#define NORM_SEP 128

#define S_STEREO_SWING (96*0x10000)

#if defined (_WINDOWS) && !defined (SURROUND) //&& defined (_X86_)
#define SURROUND
#endif

#ifdef SURROUND
#define SURROUND_SEP -128
#endif

// the set of channels available
channel_t *channels = NULL;
INT32 numofchannels = 0;

// the internal volumes that all game audio are processed at
INT32 internal_music_volume = 100;
INT32 internal_sfx_volume = 31;

//
// Music internals internals.
//
static lumpnum_t S_GetMusicLumpNum(const char *mname);
static boolean S_CheckQueue(void);

//
// Sound internals.
//
static void S_StopChannel(INT32 cnum);
static INT32 S_AdjustSoundParams(const mobj_t *listener, const mobj_t *source, INT32 *vol, INT32 *sep, INT32 *pitch, sfxinfo_t *sfxinfo);

//
// S_getChannel
//
// If none available, return -1. Otherwise channel #.
//
static INT32 S_getChannel(const void *origin, sfxinfo_t *sfxinfo)
{
	// channel number to use
	INT32 cnum;

	channel_t *c;

	// Find an open channel
	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (!channels[cnum].sfxinfo)
			break;

		// Now checks if same sound is being played, rather
		// than just one sound per mobj
		else if (sfxinfo == channels[cnum].sfxinfo && (sfxinfo->pitch & SF_NOMULTIPLESOUND))
		{
			return -1;
			break;
		}
		else if (sfxinfo == channels[cnum].sfxinfo && sfxinfo->singularity == true)
		{
			S_StopChannel(cnum);
			break;
		}
		else if (origin && channels[cnum].origin == origin && channels[cnum].sfxinfo == sfxinfo)
		{
			if (sfxinfo->pitch & SF_NOINTERRUPT)
				return -1;
			else
				S_StopChannel(cnum);
			break;
		}
		else if (origin && channels[cnum].origin == origin
			&& channels[cnum].sfxinfo->name != sfxinfo->name
			&& channels[cnum].sfxinfo->pitch & SF_TOTALLYSINGLE && sfxinfo->pitch & SF_TOTALLYSINGLE)
		{
			S_StopChannel(cnum);
			break;
		}
	}

	// None available
	if (cnum == numofchannels)
	{
		// Look for lower priority
		for (cnum = 0; cnum < numofchannels; cnum++)
			if (channels[cnum].sfxinfo->priority <= sfxinfo->priority)
				break;

		if (cnum == numofchannels)
		{
			// No lower priority. Sorry, Charlie.
			return -1;
		}
		else
		{
			// Otherwise, kick out lower priority.
			S_StopChannel(cnum);
		}
	}

	c = &channels[cnum];

	// channel is decided to be cnum.
	c->sfxinfo = sfxinfo;
	c->origin = origin;

	return cnum;
}

void S_RegisterSoundStuff(void)
{
	if (dedicated)
	{
		sound_disabled = true;
		return;
	}

	CV_RegisterVar(&stereoreverse);
	CV_RegisterVar(&precachesound);

	CV_RegisterVar(&surround);
	CV_RegisterVar(&cv_samplerate);
	CV_RegisterVar(&cv_buffersize);
	CV_RegisterVar(&cv_resetmusic);
	CV_RegisterVar(&cv_resetmusicbyheader);
	CV_RegisterVar(&cv_1upsound);
	CV_RegisterVar(&cv_playsoundsifunfocused);
	CV_RegisterVar(&cv_playmusicifunfocused);
	CV_RegisterVar(&cv_gamesounds);
	CV_RegisterVar(&cv_gamedigimusic);
	CV_RegisterVar(&cv_gamemidimusic);
	CV_RegisterVar(&cv_musicpref);

#ifdef HAVE_OPENMPT
	CV_RegisterVar(&cv_modfilter);
#endif

#ifdef HAVE_MIXERX
	CV_RegisterVar(&cv_midiplayer);
	CV_RegisterVar(&cv_midisoundfontpath);
	CV_RegisterVar(&cv_miditimiditypath);
#endif

	COM_AddCommand("tunes", Command_Tunes_f, COM_LUA);
	COM_AddCommand("restartaudio", S_RestartAudio, COM_LUA);
}

static void SetChannelsNum(void)
{
	INT32 i;

	// Allocating the internal channels for mixing
	// (the maximum number of sounds rendered
	// simultaneously) within zone memory.
	if (channels)
	{
		S_StopSounds();
		Z_Free(channels);
	}
	channels = NULL;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_SetSourcesNum();
		return;
	}
#endif

	if (cv_numChannels.value)
		channels = (channel_t *)Z_Malloc(cv_numChannels.value * sizeof (channel_t), PU_STATIC, NULL);
	numofchannels = cv_numChannels.value;

	// Free all channels for use
	for (i = 0; i < numofchannels; i++)
	{
		channels[i].sfxinfo = NULL;
		channels[i].position = 0.0f;
		channels[i].speed = 1.0f;
	}

	S_ResetCaptions();
}

//
// Retrieve the lump number of sfx
//
lumpnum_t S_GetSfxLumpNum(sfxinfo_t *sfx)
{
	char namebuf[9];
	lumpnum_t sfxlump;

	sprintf(namebuf, "ds%s", sfx->name);

	sfxlump = W_CheckNumForName(namebuf);
	if (sfxlump != LUMPERROR)
		return sfxlump;

	strlcpy(namebuf, sfx->name, sizeof namebuf);

	sfxlump = W_CheckNumForName(namebuf);
	if (sfxlump != LUMPERROR)
		return sfxlump;

	return W_GetNumForName("dsthok");
}

//
// Sound Status
//

boolean S_SoundDisabled(void)
{
	return ( sound_disabled || ( window_notinfocus && ! cv_playsoundsifunfocused.value ) );
}

// Stop all sounds, load level info, THEN start sounds.
void S_StopSounds(void)
{
	INT32 cnum;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_StopSounds();
		return;
	}
#endif

	// kill all playing sounds at start of level
	for (cnum = 0; cnum < numofchannels; cnum++)
		if (channels[cnum].sfxinfo)
			S_StopChannel(cnum);

	S_ResetCaptions();
}

void S_StopSoundByID(void *origin, sfxenum_t sfx_id)
{
	INT32 cnum;

	// Sounds without origin can have multiple sources, they shouldn't
	// be stopped by new sounds.
	if (!origin)
		return;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_StopSoundByID(origin, sfx_id);
		return;
	}
#endif

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo == &S_sfx[sfx_id] && channels[cnum].origin == origin)
		{
			S_StopChannel(cnum);
			break;
		}
	}
}

void S_StopSoundByNum(sfxenum_t sfxnum)
{
	INT32 cnum;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_StopSoundByNum(sfxnum);
		return;
	}
#endif

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo == &S_sfx[sfxnum])
		{
			S_StopChannel(cnum);
			break;
		}
	}
}

void S_StartCaption(sfxenum_t sfx_id, INT32 cnum, UINT16 lifespan)
{
	UINT8 i, set, moveup, start;
	boolean same = false;
	sfxinfo_t *sfx;

	if (!cv_closedcaptioning.value) // no captions at all
		return;

	// check for bogus sound #
	// I_Assert(sfx_id >= 0); -- allowing sfx_None; this shouldn't be allowed directly if S_StartCaption is ever exposed to Lua by itself
	I_Assert(sfx_id < NUMSFX);

	sfx = &S_sfx[sfx_id];

	if (sfx->caption[0] == '/') // no caption for this one
		return;

	start = ((closedcaptions[0].s && (closedcaptions[0].s-S_sfx == sfx_None)) ? 1 : 0);

	if (sfx_id)
	{
		for (i = start; i < (set = NUMCAPTIONS-1); i++)
		{
			same = ((sfx == closedcaptions[i].s) || (closedcaptions[i].s && fastcmp(sfx->caption, closedcaptions[i].s->caption)));
			if (same)
			{
				set = i;
				break;
			}
		}
	}
	else
	{
		set = 0;
		same = (closedcaptions[0].s == sfx);
	}

	moveup = 255;

	if (!same)
	{
		for (i = start; i < set; i++)
		{
			if (!(closedcaptions[i].c || closedcaptions[i].s) || (sfx->priority >= closedcaptions[i].s->priority))
			{
				set = i;
				if (closedcaptions[i].s && (sfx->priority >= closedcaptions[i].s->priority))
					moveup = i;
				break;
			}
		}
		for (i = NUMCAPTIONS-1; i > set; i--)
		{
			if (sfx == closedcaptions[i].s)
			{
				closedcaptions[i].c = NULL;
				closedcaptions[i].s = NULL;
				closedcaptions[i].t = 0;
				closedcaptions[i].b = 0;
			}
		}
	}

	if (moveup != 255)
	{
		for (i = moveup; i < NUMCAPTIONS-1; i++)
		{
			if (!(closedcaptions[i].c || closedcaptions[i].s))
				break;
		}
		for (; i > set; i--)
		{
			closedcaptions[i].c = closedcaptions[i-1].c;
			closedcaptions[i].s = closedcaptions[i-1].s;
			closedcaptions[i].t = closedcaptions[i-1].t;
			closedcaptions[i].b = closedcaptions[i-1].b;
		}
	}

	closedcaptions[set].c = ((cnum == -1) ? NULL : &channels[cnum]);
	closedcaptions[set].s = sfx;
	closedcaptions[set].t = lifespan;
	closedcaptions[set].b = 3; // bob
}

void S_StartSoundAtVolume(void *origin_p, sfxenum_t sfx_id, INT32 volume, soundorigin_t soundorigin)
{
	const INT32 initial_volume = volume;
	INT32 sep, pitch, priority, cnum;
	float speed;
	sfxinfo_t *sfx;

	mobj_t *origin = NULL;
	if (soundorigin == SOUNDORIGIN_MOBJ)
		origin = (mobj_t *)origin_p;
	else if (soundorigin == SOUNDORIGIN_SECTOR)
		origin = (mobj_t *)&((sector_t *)origin_p)->soundorg;

	listener_t listener  = {0,0,0,0};
	listener_t listener2 = {0,0,0,0};

	mobj_t *listenmobj = players[displayplayer].mo;
	mobj_t *listenmobj2 = NULL;

	if (S_SoundDisabled() || !sound_started)
		return;

	// Don't want a sound? Okay then...
	if (sfx_id == sfx_None)
		return;

	if (mariomode) // Sounds change in Mario mode!
	{
		switch (sfx_id)
		{
			case sfx_altow1:
			case sfx_altow2:
			case sfx_altow3:
			case sfx_altow4:
				sfx_id = sfx_mario8;
				break;
			case sfx_thok:
			case sfx_wepfir:
				sfx_id = sfx_mario7;
				break;
			case sfx_pop:
				sfx_id = sfx_mario5;
				break;
			case sfx_jump:
				sfx_id = sfx_mario6;
				break;
			case sfx_shield:
			case sfx_wirlsg:
			case sfx_forcsg:
			case sfx_elemsg:
			case sfx_armasg:
			case sfx_attrsg:
			case sfx_s3k3e:
			case sfx_s3k3f:
			case sfx_s3k41:
				sfx_id = sfx_mario3;
				break;
			case sfx_itemup:
				sfx_id = sfx_mario4;
				break;
			case sfx_tink:
				sfx_id = sfx_mario1;
				break;
			case sfx_cgot:
				sfx_id = sfx_mario9;
				break;
			case sfx_lose:
				sfx_id = sfx_mario2;
				break;
			default:
				break;
		}
	}
	if (maptol & TOL_XMAS) // Some sounds change for xmas
	{
		switch (sfx_id)
		{
			case sfx_ideya:
			case sfx_nbmper:
			case sfx_ncitem:
			case sfx_ngdone:
				++sfx_id;
			default:
				break;
		}
	}

	const sfxenum_t actual_id = sfx_id =
		LUA_HookSoundPlay(sfx_id, origin, soundorigin);

	if (players[displayplayer].awayviewtics)
		listenmobj = players[displayplayer].awayviewmobj;

	if (splitscreen)
	{
		listenmobj2 = players[secondarydisplayplayer].mo;
		if (players[secondarydisplayplayer].awayviewtics)
			listenmobj2 = players[secondarydisplayplayer].awayviewmobj;
	}

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_StartSound(origin, sfx_id);
		return;
	}
#endif

	if (camera.chase && !players[displayplayer].awayviewtics)
	{
		listener.x = camera.x;
		listener.y = camera.y;
		listener.z = camera.z;
		listener.angle = camera.angle;
	}
	else if (listenmobj)
	{
		listener.x = listenmobj->x;
		listener.y = listenmobj->y;
		listener.z = listenmobj->z;
		listener.angle = listenmobj->angle;
	}
	else if (origin)
		return;

	if (listenmobj2)
	{
		if (camera2.chase && !players[secondarydisplayplayer].awayviewtics)
		{
			listener2.x = camera2.x;
			listener2.y = camera2.y;
			listener2.z = camera2.z;
			listener2.angle = camera2.angle;
		}
		else
		{
			listener2.x = listenmobj2->x;
			listener2.y = listenmobj2->y;
			listener2.z = listenmobj2->z;
			listener2.angle = listenmobj2->angle;
		}
	}

	// check for bogus sound #
	I_Assert(sfx_id >= 1);
	I_Assert(sfx_id < NUMSFX);

	sfx = &S_sfx[sfx_id];

	if (sfx->skinsound != -1 && origin && origin->skin)
	{
		// redirect player sound to the sound in the skin table
		sfx_id = ((skin_t *)origin->skin)->soundsid[sfx->skinsound];
		sfx = &S_sfx[sfx_id];
	}

	// Initialize sound parameters
	speed = 1.0f;
	pitch = NORM_PITCH;
	priority = NORM_PRIORITY;

	if (splitscreen && listenmobj2) // Copy the sound for the split player
	{
		// Check to see if it is audible, and if not, modify the params
		if (origin && origin != listenmobj2)
		{
			INT32 rc;
			rc = S_AdjustSoundParams(listenmobj2, origin, &volume, &sep, &pitch, sfx);

			if (!rc)
				goto dontplay; // Maybe the other player can hear it...

			if (origin->x == listener2.x && origin->y == listener2.y)
				sep = NORM_SEP;
		}
		else if (!origin)
			// Do not play origin-less sounds for the second player.
			// The first player will be able to hear it just fine,
			// we really don't want it playing twice.
			goto dontplay;
		else
			sep = NORM_SEP;

		// try to find a channel
		cnum = S_getChannel(origin, sfx);

		if (cnum < 0)
			return; // If there's no free channels, it's not gonna be free for player 1, either.

		// This is supposed to handle the loading/caching.
		// For some odd reason, the caching is done nearly
		// each time the sound is needed?

		// cache data if necessary
		// NOTE: set sfx->data NULL sfx->lump -1 to force a reload
		if (!sfx->data)
			sfx->data = I_GetSfx(sfx);

		// increase the usefulness
		if (sfx->usefulness++ < 0)
			sfx->usefulness = -1;

#ifdef SURROUND
		// Avoid channel reverse if surround
		if (stereoreverse.value && sep != SURROUND_SEP)
			sep = (~sep) & 255;
#else
		if (stereoreverse.value)
			sep = (~sep) & 255;
#endif

		// Handle closed caption input.
		S_StartCaption(actual_id, cnum, MAXCAPTIONTICS);

		// Assigns the handle to one of the channels in the
		// mix/output buffer.
		channels[cnum].handle = I_StartSound(sfx_id, volume, sep, speed, pitch, priority, cnum);
	}

dontplay:

	// Check to see if it is audible, and if not, modify the params
	if (origin && origin != listenmobj)
	{
		INT32 rc;
		rc = S_AdjustSoundParams(listenmobj, origin, &volume, &sep, &pitch, sfx);

		if (!rc)
			return;

		if (origin->x == listener.x && origin->y == listener.y)
			sep = NORM_SEP;
	}
	else
		sep = NORM_SEP;

	// try to find a channel
	cnum = S_getChannel(origin, sfx);

	if (cnum < 0)
		return;

	// This is supposed to handle the loading/caching.
	// For some odd reason, the caching is done nearly
	// each time the sound is needed?

	// cache data if necessary
	// NOTE: set sfx->data NULL sfx->lump -1 to force a reload
	if (!sfx->data)
		sfx->data = I_GetSfx(sfx);

	// increase the usefulness
	if (sfx->usefulness++ < 0)
		sfx->usefulness = -1;

#ifdef SURROUND
	// Avoid channel reverse if surround
	if (stereoreverse.value && sep != SURROUND_SEP)
		sep = (~sep) & 255;
#else
	if (stereoreverse.value)
		sep = (~sep) & 255;
#endif

	// Handle closed caption input.
	S_StartCaption(actual_id, cnum, MAXCAPTIONTICS);

	// Assigns the handle to one of the channels in the
	// mix/output buffer.
	channels[cnum].volume = initial_volume;
	channels[cnum].handle = I_StartSound(sfx_id, volume, sep, speed, pitch, priority, cnum);
}

void S_StartSoundFromEverywhereVol(sfxenum_t sfx_id, INT32 volume)
{
	S_StartSoundAtVolume(NULL, sfx_id, volume, SOUNDORIGIN_EVERYWHERE);
}

void S_StartSoundFromMobjVol(mobj_t* origin, sfxenum_t sfx_id, INT32 volume)
{
	S_StartSoundAtVolume(origin, sfx_id, volume, SOUNDORIGIN_MOBJ);
}

void S_StartSoundFromSectorVol(sector_t* origin, sfxenum_t sfx_id, INT32 volume)
{
	S_StartSoundAtVolume(origin, sfx_id, volume, SOUNDORIGIN_SECTOR);
}

void S_StartSound(void *origin, sfxenum_t sfx_id, soundorigin_t soundorigin)
{
	if (S_SoundDisabled())
		return;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_StartSound(origin, sfx_id);
		return;
	}
#endif

	// the volume is handled 8 bits
	S_StartSoundAtVolume(origin, sfx_id, 255, soundorigin);
}

void S_StartSoundFromEverywhere(sfxenum_t sfx_id)
{
	S_StartSound(NULL, sfx_id, SOUNDORIGIN_EVERYWHERE);
}

void S_StartSoundFromMobj(mobj_t* origin, sfxenum_t sfx_id)
{
	S_StartSound(origin, sfx_id, SOUNDORIGIN_MOBJ);
}

void S_StartSoundFromSector(sector_t* origin, sfxenum_t sfx_id)
{
	S_StartSound(origin, sfx_id, SOUNDORIGIN_SECTOR);
}

//
// S_SearchForSoundName
// Finds a sound using the given name.
//
sfxenum_t S_SearchForSoundName(const char *soundname)
{
	sfxenum_t soundnum;

	if (soundname == NULL)
		return sfx_None;

	for (soundnum = sfx_None+1; soundnum < NUMSFX; soundnum++)
	{
		if (S_sfx[soundnum].name != NULL && !stricmp(S_sfx[soundnum].name, soundname))
			return soundnum;
	}
	return sfx_None-1;
}

//
// S_SearchAndPlaySounds
//
// Search for sound name
// Play at max volume
//
static void S_SearchAndPlaySounds(void *origin, const char *soundname, INT32 volume, soundorigin_t soundorigin)
{
	sfxenum_t soundnum;

	if (soundname == NULL)
	{
		S_StartSoundAtVolume(origin, sfx_None, volume, soundorigin);
		return;
	}

	for (soundnum = sfx_None+1; soundnum < NUMSFX; soundnum++)
	{
		if (S_sfx[soundnum].name != NULL && !stricmp(S_sfx[soundnum].name, soundname))
		{
			S_StartSoundAtVolume(origin, soundnum, volume, soundorigin);
			return;
		}
	}
	CONS_Alert(CONS_ERROR, "S_StartSoundName(): Sound '%s' couldn't be found!\n", soundname);
}


void S_StartSoundName(void *origin, const char *soundname, soundorigin_t soundorigin)
{
	S_SearchAndPlaySounds(origin, soundname, 255, soundorigin);
}

void S_StartSoundNameFromEverywhere(const char *soundname)
{
	S_StartSoundName(NULL, soundname, SOUNDORIGIN_EVERYWHERE);
}

void S_StartSoundNameFromMobj(mobj_t* origin, const char *soundname)
{
	S_StartSoundName(origin, soundname, SOUNDORIGIN_MOBJ);
}

void S_StartSoundNameFromSector(sector_t *origin, const char *soundname)
{
	S_StartSoundName(origin, soundname, SOUNDORIGIN_SECTOR);
}

//
// Search for sound name
// Play at specified volume
//
void S_StartSoundNameAtVolume(void *origin, const char *soundname, INT32 volume, soundorigin_t soundorigin)
{
	S_SearchAndPlaySounds(origin, soundname, volume, soundorigin);
}

void S_StartSoundNameFromEverywhereVol(const char *soundname, INT32 volume)
{
	S_StartSoundNameAtVolume(NULL, soundname, volume, SOUNDORIGIN_EVERYWHERE);
}

void S_StartSoundNameFromMobjVol(mobj_t* origin, const char *soundname, INT32 volume)
{
	S_StartSoundNameAtVolume(origin, soundname, volume, SOUNDORIGIN_MOBJ);
}

void S_StartSoundNameFromSectorVol(sector_t* origin, const char *soundname, INT32 volume)
{
	S_StartSoundNameAtVolume(origin, soundname, volume, SOUNDORIGIN_SECTOR);
}

void S_StopSound(void *origin)
{
	INT32 cnum;

	// Sounds without origin can have multiple sources, they shouldn't
	// be stopped by new sounds.
	if (!origin)
		return;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_StopSound(origin);
		return;
	}
#endif

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo && channels[cnum].origin == origin)
		{
			S_StopChannel(cnum);
			break;
		}
	}
}

//
// Updates music & sounds
//
static INT32 actualsfxvolume; // check for change through console
static INT32 actualdigmusicvolume;
static INT32 actualmidimusicvolume;

//
// Restart all program audio.
// Sounds may also be freed during this process, since we restart the entire audio system.
//
void S_RestartAudio(void)
{
	{
		// StarManiaKG (STAR STUFFS):
		// Forcibly stop the jukebox.
		// That way, music can play after the audio starts back up.
		TSoURDt3rd_Jukebox_Stop();
	}

	S_StopMusic();
	S_StopSounds();
	{
		// StarManiaKG: cv_buffersize and cv_samplerate fix!
		// If audio mixing variables these are modified, we gotta free the sound data!
		// Else, the sounds stop sounding like their normal sounding sounds!
		S_ClearSfx();
	}
	I_ShutdownMusic();
	I_ShutdownSound();

	I_StartupSound();
	I_InitMusic();
	S_InitSfxChannels(cv_soundvolume.value);
	S_UpdateSounds();
	I_UpdateSound();

	// These must be called or no sound and music until manually set.
	I_SetSfxVolume(cv_soundvolume.value);
	S_SetMusicVolume(cv_digmusicvolume.value, cv_midimusicvolume.value);

	// Restart the music
	if (Playing())
		P_RestoreMusic(&players[consoleplayer]);
	else
		S_ChangeMusicInternal("_title", false);
}

void S_UpdateSounds(void)
{
	INT32 audible, cnum, volume, sep, pitch;
	float speed;
	channel_t *c;

	listener_t listener;
	listener_t listener2;

	mobj_t *listenmobj = players[displayplayer].mo;
	mobj_t *listenmobj2 = NULL;

	memset(&listener, 0, sizeof(listener_t));
	memset(&listener2, 0, sizeof(listener_t));

	// Update sound/music volumes, if changed manually at console
	if (actualsfxvolume != cv_soundvolume.value)
		S_SetSfxVolume (cv_soundvolume.value);
	if (actualdigmusicvolume != cv_digmusicvolume.value)
		S_SetDigMusicVolume (cv_digmusicvolume.value);
	if (actualmidimusicvolume != cv_midimusicvolume.value)
		S_SetMIDIMusicVolume (cv_midimusicvolume.value);

	// We're done now, if we're not in a level.
	if (gamestate != GS_LEVEL)
	{
#ifndef NOMUMBLE
		// Stop Mumble cutting out. I'm sick of it.
		I_UpdateMumble(NULL, listener);
#endif

		goto notinlevel;
	}

	if (dedicated || sound_disabled)
		return;

	if (players[displayplayer].awayviewtics)
		listenmobj = players[displayplayer].awayviewmobj;

	if (splitscreen)
	{
		listenmobj2 = players[secondarydisplayplayer].mo;
		if (players[secondarydisplayplayer].awayviewtics)
			listenmobj2 = players[secondarydisplayplayer].awayviewmobj;
	}

	if (camera.chase && !players[displayplayer].awayviewtics)
	{
		listener.x = camera.x;
		listener.y = camera.y;
		listener.z = camera.z;
		listener.angle = camera.angle;
	}
	else if (listenmobj)
	{
		listener.x = listenmobj->x;
		listener.y = listenmobj->y;
		listener.z = listenmobj->z;
		listener.angle = listenmobj->angle;
	}

#ifndef NOMUMBLE
	I_UpdateMumble(players[consoleplayer].mo, listener);
#endif

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_UpdateSources();
		goto notinlevel;
	}
#endif

	if (listenmobj2)
	{
		if (camera2.chase && !players[secondarydisplayplayer].awayviewtics)
		{
			listener2.x = camera2.x;
			listener2.y = camera2.y;
			listener2.z = camera2.z;
			listener2.angle = camera2.angle;
		}
		else
		{
			listener2.x = listenmobj2->x;
			listener2.y = listenmobj2->y;
			listener2.z = listenmobj2->z;
			listener2.angle = listenmobj2->angle;
		}
	}

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		c = &channels[cnum];

		if (c->sfxinfo)
		{
			if (I_SoundIsPlaying(c->handle))
			{
				// initialize parameters
				volume = c->volume; // 8 bits internal volume precision
				speed = c->speed;
				pitch = NORM_PITCH;
				sep = NORM_SEP;

				// check non-local sounds for distance clipping
				//  or modify their params
				if (c->origin && ((c->origin != players[consoleplayer].mo) ||
					(splitscreen && c->origin != players[secondarydisplayplayer].mo)))
				{
					// Whomever is closer gets the sound, but only in splitscreen.
					if (listenmobj && listenmobj2 && splitscreen)
					{
						const mobj_t *soundmobj = c->origin;

						fixed_t dist1, dist2;
						dist1 = P_AproxDistance(listener.x-soundmobj->x, listener.y-soundmobj->y);
						dist2 = P_AproxDistance(listener2.x-soundmobj->x, listener2.y-soundmobj->y);

						if (dist1 <= dist2)
						{
							// Player 1 gets the sound
							audible = S_AdjustSoundParams(listenmobj, c->origin, &volume, &sep, &pitch,
								c->sfxinfo);
						}
						else
						{
							// Player 2 gets the sound
							audible = S_AdjustSoundParams(listenmobj2, c->origin, &volume, &sep, &pitch,
								c->sfxinfo);
						}

						if (audible)
							I_UpdateSoundParams(c->handle, volume, sep, speed, pitch);
						else
							S_StopChannel(cnum);
					}
					else if (listenmobj && !splitscreen)
					{
						// In the case of a single player, he or she always should get updated sound.
						audible = S_AdjustSoundParams(listenmobj, c->origin, &volume, &sep, &pitch,
							c->sfxinfo);

						if (audible)
							I_UpdateSoundParams(c->handle, volume, sep, speed, pitch);
						else
							S_StopChannel(cnum);
					}
				}
			}
			else
			{
				// if channel is allocated but sound has stopped, free it
				S_StopChannel(cnum);
			}
		}
	}

notinlevel:
	I_UpdateSound();
}

void S_UpdateClosedCaptions(void)
{
	UINT8 i;
	boolean gamestopped = (paused || P_AutoPause());
	for (i = 0; i < NUMCAPTIONS; i++) // update captions
	{
		if (!closedcaptions[i].s)
			continue;

		if (i == 0 && (closedcaptions[0].s-S_sfx == sfx_None) && gamestopped)
			continue;

		if (!(--closedcaptions[i].t))
		{
			closedcaptions[i].c = NULL;
			closedcaptions[i].s = NULL;
		}
		else if (closedcaptions[i].c && !I_SoundIsPlaying(closedcaptions[i].c->handle))
		{
			closedcaptions[i].c = NULL;
			if (closedcaptions[i].t > CAPTIONFADETICS)
				closedcaptions[i].t = CAPTIONFADETICS;
		}
	}
}

void S_SetSfxVolume(INT32 volume)
{
	if (volume < 0 || volume > 31)
		CONS_Alert(CONS_WARNING, "sfxvolume should be between 0-31\n");

	CV_SetValue(&cv_soundvolume, volume&0x1F);
	actualsfxvolume = cv_soundvolume.value; // check for change of var

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_SetSfxVolume(volume&0x1F);
		return;
	}
#endif

	// now hardware volume
	I_SetSfxVolume(volume&0x1F);
}

void S_SetInternalSfxVolume(INT32 volume)
{
	if (volume < 0 || volume > 31)
	{
		CONS_Alert(CONS_WARNING, "sfxvolume should be between 0-31\n");
		volume = volume&0x1F;
	}
	internal_sfx_volume = volume;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
	{
		HW3S_SetInternalSfxVolume(internal_music_volume);
		return;
	}
#endif

	I_SetInternalSfxVolume(internal_sfx_volume);
}

INT32 S_GetInternalSfxVolume(void)
{
	return internal_sfx_volume;
}

boolean S_SpeedSound(void *origin, float speed)
{
	INT32 cnum;
	INT32 sped_sounds = 0;

	if (!origin)
		return false;

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo && channels[cnum].origin == origin)
		{
			I_SetSoundSpeed(channels[cnum].handle, speed);
			sped_sounds++;
		}
	}
	return sped_sounds;
}

boolean S_SpeedSoundByID(void *origin, sfxenum_t sfx_id, float speed)
{
	INT32 cnum;
	INT32 sped_sounds = 0;

	if (!origin)
		return false;

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo == &S_sfx[sfx_id] && channels[cnum].origin == origin)
		{
			I_SetSoundSpeed(channels[cnum].handle, speed);
			sped_sounds++;
		}
	}
	return sped_sounds;
}

boolean S_SpeedSoundByNum(sfxenum_t sfx_num, float speed)
{
	INT32 cnum;
	INT32 sped_sounds = 0;

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo == &S_sfx[sfx_num])
		{
			I_SetSoundSpeed(channels[cnum].handle, speed);
			sped_sounds++;
		}
	}
	return sped_sounds;
}

float S_GetSpeedSound(void *origin)
{
	INT32 cnum;

	if (!origin)
		return -1.0;

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo && channels[cnum].origin == origin)
			return channels[cnum].speed;
	}
	return -1.0;
}

float S_GetSpeedSoundByID(void *origin, sfxenum_t sfx_id)
{
	INT32 cnum;

	if (!origin)
		return -1.0;

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo == &S_sfx[sfx_id] && channels[cnum].origin == origin)
			return channels[cnum].speed;
	}
	return -1.0;
}

float S_GetSpeedSoundByNum(sfxenum_t sfx_num)
{
	INT32 cnum;

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].sfxinfo == &S_sfx[sfx_num])
			return channels[cnum].speed;
	}
	return -1.0;
}

void S_ClearSfx(void)
{
	size_t i;
	for (i = 1; i < NUMSFX; i++)
		I_FreeSfx(S_sfx + i);
}

static void S_StopChannel(INT32 cnum)
{
	channel_t *c = &channels[cnum];

	if (c->sfxinfo)
	{
		// stop the sound playing
		if (I_SoundIsPlaying(c->handle))
			I_StopSound(c->handle);

		// degrade usefulness of sound data
		c->sfxinfo->usefulness--;
		c->sfxinfo = NULL;
	}

	c->origin = NULL;
	c->position = 0.0f;
	c->speed = 1.0f;
}

//
// S_CalculateSoundDistance
//
// Calculates the distance between two points for a sound.
// Clips the distance to prevent overflow.
//
fixed_t S_CalculateSoundDistance(fixed_t sx1, fixed_t sy1, fixed_t sz1, fixed_t sx2, fixed_t sy2, fixed_t sz2)
{
	fixed_t approx_dist, adx, ady;

	// calculate the distance to sound origin and clip it if necessary
	adx = abs((sx1>>FRACBITS) - (sx2>>FRACBITS));
	ady = abs((sy1>>FRACBITS) - (sy2>>FRACBITS));

	// From _GG1_ p.428. Approx. euclidian distance fast.
	// Take Z into account
	adx = adx + ady - ((adx < ady ? adx : ady)>>1);
	ady = abs((sz1>>FRACBITS) - (sz2>>FRACBITS));
	approx_dist = adx + ady - ((adx < ady ? adx : ady)>>1);

	if (approx_dist >= FRACUNIT/2)
		approx_dist = FRACUNIT/2-1;

	approx_dist <<= FRACBITS;

	return approx_dist;
}

//
// Changes volume, stereo-separation, and pitch variables
// from the norm of a sound effect to be played.
// If the sound is not audible, returns a 0.
// Otherwise, modifies parameters and returns 1.
//
INT32 S_AdjustSoundParams(const mobj_t *listener, const mobj_t *source, INT32 *vol, INT32 *sep, INT32 *pitch,
	sfxinfo_t *sfxinfo)
{
	fixed_t approx_dist;
	angle_t angle;

	listener_t listensource;

	(void)pitch;
	if (!listener)
		return false;

	if (listener == players[displayplayer].mo && camera.chase)
	{
		listensource.x = camera.x;
		listensource.y = camera.y;
		listensource.z = camera.z;
		listensource.angle = camera.angle;
	}
	else if (splitscreen && listener == players[secondarydisplayplayer].mo && camera2.chase)
	{
		listensource.x = camera2.x;
		listensource.y = camera2.y;
		listensource.z = camera2.z;
		listensource.angle = camera2.angle;
	}
	else
	{
		listensource.x = listener->x;
		listensource.y = listener->y;
		listensource.z = listener->z;
		listensource.angle = listener->angle;
	}

	if (sfxinfo->pitch & SF_OUTSIDESOUND) // Rain special case
	{
		fixed_t x, y, yl, yh, xl, xh, newdist;

		if (R_PointInSubsector(listensource.x, listensource.y)->sector->ceilingpic == skyflatnum)
			approx_dist = 0;
		else
		{
			// Essentially check in a 1024 unit radius of the player for an outdoor area.
			yl = listensource.y - 1024*FRACUNIT;
			yh = listensource.y + 1024*FRACUNIT;
			xl = listensource.x - 1024*FRACUNIT;
			xh = listensource.x + 1024*FRACUNIT;
			approx_dist = 1024*FRACUNIT;
			for (y = yl; y <= yh; y += FRACUNIT*64)
				for (x = xl; x <= xh; x += FRACUNIT*64)
				{
					if (R_PointInSubsector(x, y)->sector->ceilingpic == skyflatnum)
					{
						// Found the outdoors!
						newdist = S_CalculateSoundDistance(listensource.x, listensource.y, 0, x, y, 0);
						if (newdist < approx_dist)
						{
							approx_dist = newdist;
						}
					}
				}
		}
	}
	else
	{
		approx_dist = S_CalculateSoundDistance(listensource.x, listensource.y, listensource.z,
												source->x, source->y, source->z);
	}

	// Ring loss, deaths, etc, should all be heard louder.
	if (sfxinfo->pitch & SF_X8AWAYSOUND)
		approx_dist = FixedDiv(approx_dist,8*FRACUNIT);

	// Combine 8XAWAYSOUND with 4XAWAYSOUND and get.... 32XAWAYSOUND?
	if (sfxinfo->pitch & SF_X4AWAYSOUND)
		approx_dist = FixedDiv(approx_dist,4*FRACUNIT);

	// Combine all the above with X2AWAYSOUND, and you get 64XAWAYSOUND!
	if (sfxinfo->pitch & SF_X2AWAYSOUND)
		approx_dist = FixedDiv(approx_dist,2*FRACUNIT);

	if (approx_dist > S_CLIPPING_DIST)
		return 0;

	// angle of source to listener
	angle = R_PointToAngle2(listensource.x, listensource.y, source->x, source->y);

	if (angle > listensource.angle)
		angle = angle - listensource.angle;
	else
		angle = angle + InvAngle(listensource.angle);

#ifdef SURROUND
	// Produce a surround sound for angle from 105 till 255
	if (surround.value == 1 && (angle > ANG105 && angle < ANG255 ))
		*sep = SURROUND_SEP;
	else
#endif
	{
		angle >>= ANGLETOFINESHIFT;

		// stereo separation
		*sep = 128 - (FixedMul(S_STEREO_SWING, FINESINE(angle))>>FRACBITS);
	}

	// volume calculation
	/* not sure if it should be > (no =), but this matches the old behavior */
	if (approx_dist >= S_CLOSE_DIST)
	{
		// distance effect
		INT32 n = (15 * ((S_CLIPPING_DIST - approx_dist)>>FRACBITS));
		*vol = FixedMul(*vol * FRACUNIT / 255, n) / S_ATTENUATOR;
	}

	return (*vol > 0);
}

// Searches through the channels and checks if a sound is playing
// on the given origin.
INT32 S_OriginPlaying(void *origin)
{
	INT32 cnum;
	if (!origin)
		return false;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
		return HW3S_OriginPlaying(origin);
#endif

	for (cnum = 0; cnum < numofchannels; cnum++)
		if (channels[cnum].origin == origin)
			return 1;
	return 0;
}

// Searches through the channels and checks if a given id
// is playing anywhere.
INT32 S_IdPlaying(sfxenum_t id)
{
	INT32 cnum;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
		return HW3S_IdPlaying(id);
#endif

	for (cnum = 0; cnum < numofchannels; cnum++)
		if ((size_t)(channels[cnum].sfxinfo - S_sfx) == (size_t)id)
			return 1;
	return 0;
}

// Searches through the channels and checks for
// origin x playing sound id y.
INT32 S_SoundPlaying(void *origin, sfxenum_t id)
{
	INT32 cnum;
	if (!origin)
		return 0;

#ifdef HW3SOUND
	if (hws_mode != HWS_DEFAULT_MODE)
		return HW3S_SoundPlaying(origin, id);
#endif

	for (cnum = 0; cnum < numofchannels; cnum++)
	{
		if (channels[cnum].origin == origin
		 && (size_t)(channels[cnum].sfxinfo - S_sfx) == (size_t)id)
			return 1;
	}
	return 0;
}

//
// Initializes sound stuff, including volume
// Sets channels, SFX volume,
//  allocates channel buffer, sets S_sfx lookup.
//
void S_InitSfxChannels(INT32 sfxVolume)
{
	INT32 i;

	if (dedicated)
		return;

	S_SetSfxVolume(sfxVolume);
	S_SetInternalSfxVolume(sfxVolume);

	SetChannelsNum();

	// Note that sounds have not been cached (yet).
	for (i = 1; i < NUMSFX; i++)
	{
		S_sfx[i].usefulness = -1; // for I_GetSfx()
		S_sfx[i].lumpnum = LUMPERROR;
	}

	// precache sounds if requested by cmdline, or precachesound var true
	if (!sound_disabled && (M_CheckParm("-precachesound") || precachesound.value))
	{
		// Initialize external data (all sounds) at start, keep static.
		CONS_Printf(M_GetText("Loading sounds... "));

		for (i = 1; i < NUMSFX; i++)
			if (S_sfx[i].name)
				S_sfx[i].data = I_GetSfx(&S_sfx[i]);

		CONS_Printf(M_GetText(" pre-cached all sound data\n"));
	}
}

/// ------------------------
/// Music
/// ------------------------

static char      music_name[7]; // up to 6-character name
static void      *music_data;
static UINT16    music_flags;
static boolean   music_looping;
static boolean   music_speeding_allowed;
static boolean   music_pitching_allowed;

static char      queue_name[7];
static UINT16    queue_flags;
static boolean   queue_looping;
static UINT32    queue_position;
static float     queue_speed;
static float     queue_pitch;
static UINT32    queue_fadeinms;

static tic_t     pause_starttic;

/// ------------------------
/// Music Definitions
/// ------------------------

enum
{
	MUSICDEF_220,
	MUSICDEF_221,
};

basicmusicdef_t soundtestsfx_basicdef = {
	"Sound Effects",
	"",
	"SEGA, VAdaPEGA, other sources",
	""
};

musicdef_t soundtestsfx = {
	{ "_STSFX" }, // prevents exactly one valid track name from being used on the sound test
	NULL,
	NULL,
	NULL,
	NULL,

	{ 0 },
	{ false },
	1,
	DEFAULT_MUSICDEF_VOLUME,
	DEFAULT_MUSICDEF_VOLUME,
	true,
	false,

	1, // show on soundtest page 1
	0, // with no conditions
	0,
	0,
	0,
	false,

	&soundtestsfx_basicdef,

	NULL
};

musicdef_t *musicdefstart = &soundtestsfx;

static void MusicDefStrcpy(char **p, const char *s, int version)
{
	char *buf = Z_StrDup(s);
	char *space_buf = buf;
	if (version == MUSICDEF_220)
	{
		while ((space_buf = strchr(space_buf, '_') ))
			*space_buf++ = ' '; // turn _ into spaces.
	}
	(*p) = buf;
}

static boolean
MusicDefError
(
		alerttype_t  level,
		const char * description,
		const char * field,
		lumpnum_t    lumpnum,
		int          line
){
	const wadfile_t  * wad  =    wadfiles[WADFILENUM (lumpnum)];
	const lumpinfo_t * lump = &wad->lumpinfo[LUMPNUM (lumpnum)];

	CONS_Alert(level,
			va("%%s|%%s: %s (line %%d)\n", description),
			wad->filename,
			lump->fullname,
			field,
			line
	);

	return false;
}

static boolean
ReadMusicDefFields
(
		lumpnum_t    lumpnum,
		int          line,
		boolean      fields,
		char        *stoken,
		musicdef_t **defp,
		int         *versionp
){
	musicdef_t *def;
	int version;

	char *value;
	char *textline;
	int value_as_number;

	if (!stricmp(stoken, "lump"))
	{
		value = strtok(NULL, " ,");
		if (!value)
		{
			return MusicDefError(CONS_WARNING,
					"Field '%'s is missing name.",
					stoken, lumpnum, line);
		}
		else
		{
			musicdef_t *prev = NULL;
			UINT8 track = 0;
			def = S_FindMusicDef(value, NULL, &track, &prev);

			// Nothing found, add to the end.
			if (!def)
			{
				def = Z_Calloc(sizeof (musicdef_t), PU_STATIC, NULL);

				do
				{
					if (track >= MAXDEFTRACKS)
						break;

					if (value[0] == '\\')
					{
						def->basenoloop[track] = true;
						value++;
					}

					STRBUFCPY(def->name[track], value);
					strlwr(def->name[track]);
					CONS_Debug(DBG_SETUP, "S_LoadMusicDefs: Added song '%s'\n", def->name[track]);

					def->hash[track] = quickncasehash (def->name[track], 6);
					track++;
				} while ((value = strtok(NULL," ,")) != NULL);

				if (value != NULL)
				{
					return MusicDefError(CONS_ERROR,
							"Extra tracks for field '%s' beyond 3 discarded.", // MAXDEFTRACKS
							stoken, lumpnum, line);
				}

				def->bpm = TICRATE<<(FRACBITS-1); // FixedDiv((60*TICRATE)<<FRACBITS, 120<<FRACBITS)

				def->numtracks = track;
				def->volume = DEFAULT_MUSICDEF_VOLUME;

				def->basicdef = NULL;

				if (prev != NULL)
					prev->next = def;
			}

			(*defp) = def;
		}
	}
	else if (!stricmp(stoken, "version"))
	{
		if (fields)/* is this not the first field? */
		{
			return MusicDefError(CONS_WARNING,
					"MUSICDEF: Field '%s' must come first.\n",
					stoken, lumpnum, line);
		}
		else
		{
			value = strtok(NULL, " ");
			if (!value)
			{
				return MusicDefError(CONS_WARNING,
						"MUSICDEF: Field '%s' is missing version.\n",
						stoken, lumpnum, line);
			}
			else
			{
				if (strcasecmp(value, "2.2.0"))
					(*versionp) = MUSICDEF_221;
			}
		}
	}
	else
	{
		version = (*versionp);

		if (version == MUSICDEF_220)
			value = strtok(NULL, " =");
		else
		{
			value = strtok(NULL, "");

			if (value)
			{
				// Find the equals sign.
				value = strchr(value, '=');
			}
		}

		if (!value)
		{
			return MusicDefError(CONS_WARNING,
					"Field '%s' is missing value.",
					stoken, lumpnum, line);
		}
		else
		{
			def = (*defp);

			if (!def)
			{
				return MusicDefError(CONS_ERROR,
						"No music definition before field '%s'.",
						stoken, lumpnum, line);
			}

			if (version != MUSICDEF_220)
			{
				// Skip the equals sign.
				value++;

				// Now skip funny whitespace.
				value += strspn(value, "\t ");
			}

			textline = value;
			value_as_number = atoi(value);

			if (!stricmp(stoken, "title"))
			{
				Z_Free(def->title);
				MusicDefStrcpy(&def->title, textline, version);
			}
			else if (!stricmp(stoken, "alttitle"))
			{
				Z_Free(def->alttitle);
				MusicDefStrcpy(&def->alttitle, textline, version);
			}
			else if (!stricmp(stoken, "authors"))
			{
				Z_Free(def->authors);
				MusicDefStrcpy(&def->authors, textline, version);
			}
			else if (!stricmp(stoken, "source"))
			{
				Z_Free(def->source);
				MusicDefStrcpy(&def->source, textline, version);
			}
			else if (!stricmp(stoken, "volume"))
			{
				def->volume = atoi(textline);
			}
			else if (!stricmp(stoken, "important"))
			{
				textline[0] = toupper(textline[0]);
				def->important = (textline[0] == 'Y' || textline[0] == 'T' || textline[0] == '1');
			}
			else if (!stricmp(stoken, "contentidunsafe"))
			{
				textline[0] = toupper(textline[0]);
				def->contentidunsafe = (textline[0] == 'Y' || textline[0] == 'T' || textline[0] == '1');
			}
			else if (!stricmp(stoken, "soundtestpage"))
			{
				def->soundtestpage = (UINT8)value_as_number;
			}
			else if (!stricmp(stoken, "soundtestcond"))
			{
				// Convert to map number
				if (textline[0] >= 'A' && textline[0] <= 'Z' && textline[2] == '\0')
					value_as_number = M_MapNumber(textline[0], textline[1]);
				def->soundtestcond = (INT16)value_as_number;
			}
			else if (!stricmp(stoken, "stoppingtime"))
			{
				double stoppingtime = atof(textline)*TICRATE;
				def->stoppingtics = (tic_t)stoppingtime;
			}
			else if (!stricmp(stoken, "bpm"))
			{
				double bpm = atof(textline);
				fixed_t bpmf = FLOAT_TO_FIXED(bpm);
				if (bpmf > 0)
					def->bpm = FixedDiv((60*TICRATE)<<FRACBITS, bpmf);
			}
			else if (!stricmp(stoken, "loopms"))
			{
				def->loop_ms = atoi(textline);
			}
			else if (!stricmp(stoken, "usage"))
			{
				// Does nothing as always but just in case some dude defined this...
				;
			}
			else
			{
				MusicDefError(CONS_WARNING,
						"Unknown field '%s'.",
						stoken, lumpnum, line);
			}
		}
	}

	return true;
}

static void S_LoadMusicDefLump(lumpnum_t lumpnum)
{
	char *lump;
	char *musdeftext;
	size_t size;

	char *lf;
	char *stoken;

	size_t nlf = 0xFFFFFFFF;
	size_t ncr;

	musicdef_t *def = NULL;
	int version = MUSICDEF_220;
	int line = 1; // for better error msgs
	boolean fields = false;

	lump = W_CacheLumpNum(lumpnum, PU_CACHE);
	size = W_LumpLength(lumpnum);

	// Null-terminated MUSICDEF lump.
	musdeftext = malloc(size+1);
	if (!musdeftext)
		I_Error("S_LoadMusicDefs: No more free memory for the parser\n");
	M_Memcpy(musdeftext, lump, size);
	musdeftext[size] = '\0';

	// Find music def
	stoken = musdeftext;
	for (;;)
	{
		lf = strpbrk(stoken, "\r\n");
		if (lf)
		{
			if (*lf == '\n')
				nlf = 1;
			else
				nlf = 0;
			*lf++ = '\0';/* now we can delimit to here */
		}

		stoken = strtok(stoken, " ");
		if (stoken)
		{
			if (!ReadMusicDefFields(lumpnum, line, fields, stoken, &def, &version))
				break;
			fields = true;
		}

		if (lf)
		{
			do
			{
				line += nlf;
				ncr = strspn(lf, "\r");/* skip CR */
				lf += ncr;
				nlf = strspn(lf, "\n");
				lf += nlf;
			}
			while (nlf || ncr) ;

			stoken = lf;/* now the next nonempty line */
		}
		else
			break;/* EOF */
	}

	free(musdeftext);
}

//
// S_LoadMusicDefs
//
// Search for music definitions in a wad and load them.
//
void S_LoadMusicDefs(UINT16 wad)
{
	const lumpnum_t wadnum = wad << 16;
	UINT16 lump = 0;

	while (( lump = W_CheckNumForNamePwad("MUSICDEF", wad, lump) ) != INT16_MAX)
	{
		S_LoadMusicDefLump(wadnum | lump);
		lump++;
	}
}

//
// S_InitMusicDefs
//
// Simply load music defs in all wads.
//
void S_InitMusicDefs(void)
{
	UINT16 i;
	for (i = 0; i < numwadfiles; i++)
		S_LoadMusicDefs(i);
}

//
// S_FindMusicDef
//
// Find music def by 6 char name
//
musicdef_t *S_FindMusicDef(const char *name, UINT8 *pos_p, UINT8 *track_p, musicdef_t **prev_p)
{
	musicdef_t *def, *prev;
	UINT32 hash;
	UINT32 pos = 0;
	UINT32 track;

	if (!name || !name[0])
		return NULL;
	hash = quickncasehash (name, 6);

	for (def = musicdefstart; def; def = def->next, pos++)
	{
		for (track = 0; track < def->numtracks; track++)
		{
			if (hash != def->hash[track] || strcasecmp(def->name[track], name))
				continue;
			if (pos_p) (*pos_p) = pos;
			if (track_p) (*track_p) = track;
			return def;
		}
		prev = def;
		if (prev_p) (*prev_p) = prev;
	}

	if (pos_p) (*pos_p) = 0;
	if (track_p) (*track_p) = 0;
	return NULL;
}

static inline void PrintMusicDefField(const char *label, const char *field)
{
	if (field)
	{
		CONS_Printf("%s%s\n", label, field);
	}
}

static void PrintSongAuthors(const musicdef_t *def, UINT8 track)
{
	if (def->numtracks > 1)
	{
		PrintMusicDefField("Title:      ", va("%s (%c)", def->title, track+'A'));
	}
	else
	{
		PrintMusicDefField("Title:      ", def->title);
	}
	PrintMusicDefField("Alt Title:  ", def->alttitle);

	CONS_Printf("\n");

	PrintMusicDefField("Original Source:    ", def->source);
	PrintMusicDefField("Original Authors:   ", def->authors);
}

static void PrintMusicDef(const char *song)
{
	UINT8 track = 0;
	const musicdef_t *def = S_FindMusicDef(song, NULL, &track, NULL);

	if (def != NULL)
	{
		PrintSongAuthors(def, track);
	}
}

musicdef_t **soundtestdefs = NULL;
INT32 numsoundtestdefs = 0;
UINT8 soundtestpage = 1;

//
// S_PrepareSoundTest
//
// Prepare sound test. What am I, your butler?
//
boolean S_PrepareSoundTest(void)
{
	gamedata_t *data = clientGamedata;
	musicdef_t *def;
	INT32 pos = numsoundtestdefs = 0;

	for (def = musicdefstart; def; def = def->next)
	{
		if (!(def->soundtestpage & soundtestpage))
			continue;
		def->allowed = false;
		numsoundtestdefs++;
	}

	if (!numsoundtestdefs)
		return false;

	if (soundtestdefs)
		Z_Free(soundtestdefs);

	if (!(soundtestdefs = Z_Malloc(numsoundtestdefs*sizeof(musicdef_t *), PU_STATIC, NULL)))
		I_Error("S_PrepareSoundTest(): could not allocate soundtestdefs.");

	for (def = musicdefstart; def /*&& i < numsoundtestdefs*/; def = def->next)
	{
		if (!(def->soundtestpage & soundtestpage))
			continue;
		soundtestdefs[pos++] = def;
		if (def->soundtestcond > 0 && !(data->mapvisited[def->soundtestcond-1] & MV_BEATEN))
			continue;
		if (def->soundtestcond < 0 && !M_Achieved(-1-def->soundtestcond, data))
			continue;
		def->allowed = true;
	}

	return true;
}


//
// Music credits
//

struct cursongcredit cursongcredit; // Currently displayed song credit info
char *g_realsongcredit;

//
// S_LoadMusicCredit
//
// Load the current song's credit into memory
//
void S_LoadMusicCredit(void)
{
	UINT8 track = 0;
	musicdef_t *def = S_FindMusicDef(S_MusicName(), NULL, &track, NULL);

	char credittext[128] = "";
	char *work = NULL;
	size_t len = 128, worklen;
	INT32 widthused = (3*BASEVIDWIDTH/4) - 7, workwidth;

	S_UnloadMusicCredit();

	if (!def)
	{
		// No definitions
		if (cv_songcredits_debug.value)
			CONS_Printf("S_LoadMusicCredit() - '%s' has no music definition!", S_MusicName());
		return;
	}
	else if (TSoURDt3rd_Jukebox_SongPlaying() && !cv_songcredits_debug.value)
	{
		// Jukebox credits display overrides music credits
		return;
	}

	work = ((def->title && def->title[0]) ? def->title : def->name[track]);
	worklen = strlen(work);
	if (worklen <= len)
	{
		strncat(credittext, work, len-1);
		len -= worklen;

		if (def->numtracks > 1)
		{
			work = va(" (%c)", track+'A');
			worklen = strlen(work);
			if (worklen <= len)
			{
				strncat(credittext, work, len);
				len -= worklen;
			}
		}

		widthused -= V_ThinStringWidth(credittext, V_ALLOWLOWERCASE);

#define MUSICCREDITAPPEND(field, force)\
		if (field)\
		{\
			work = va(" - %s", field);\
			worklen = strlen(work);\
			if (worklen <= len)\
			{\
				workwidth = V_ThinStringWidth(work, V_ALLOWLOWERCASE);\
				if (force || widthused >= workwidth)\
				{\
					strncat(credittext, work, len-1);\
					len -= worklen;\
					widthused -= workwidth;\
				}\
			}\
		}

		MUSICCREDITAPPEND(def->authors, true);
		MUSICCREDITAPPEND(def->source, false);

#undef MUSICCREDITAPPEND
	}

	if (credittext[0] == '\0')
		return;

	g_realsongcredit = Z_StrDup(credittext);
}

void S_UnloadMusicCredit(void)
{
	if (cv_songcredits_debug.value)
		return;

	Z_Free(g_realsongcredit);
	g_realsongcredit = NULL;
}

//
// S_ShowMusicCredit
//
// Display current song's credit on screen
//
void S_ShowMusicCredit(void)
{
	cursongcredit.x = cursongcredit.old_x = 0;
	cursongcredit.trans = NUMTRANSMAPS;

	if (!cv_songcredits.value)
		return;

	if (!g_realsongcredit)
	{
		// Like showing a blank credit.
		S_StopMusicCredit();
		return;
	}

	if (TSoURDt3rd_Jukebox_SongPlaying() && !cv_songcredits_debug.value)
	{
		// Jukebox credits display overrides music credits
		return;
	}

	Z_Free(cursongcredit.text);
	cursongcredit.text = Z_StrDup(g_realsongcredit);
	cursongcredit.anim = SONGCREDIT_ANIM_DURATION;
	cursongcredit.musicnote_normal_chance = M_RandomChance((9*FRACUNIT) / 10); // 90%
}

void S_StopMusicCredit(void)
{
	if (cv_songcredits_debug.value)
		return;

	Z_Free(cursongcredit.text);
	memset(&cursongcredit, 0, sizeof(struct cursongcredit));
}

/// ------------------------
/// Music Status
/// ------------------------

boolean S_DigMusicDisabled(void)
{
	return digital_disabled;
}

boolean S_MIDIMusicDisabled(void)
{
	return midi_disabled;
}

boolean S_MusicDisabled(void)
{
	return (midi_disabled && digital_disabled);
}

boolean S_MusicLoaded(void)
{
	return I_SongLoaded();
}

boolean S_MusicPlaying(void)
{
	return I_SongPlaying();
}

boolean S_MusicPaused(void)
{
	return I_SongPaused();
}

boolean S_MusicNotInFocus(void)
{
	return (( window_notinfocus && ! cv_playmusicifunfocused.value ));
}

musictype_t S_MusicType(void)
{
	return I_SongType();
}

const char *S_MusicName(void)
{
	return music_name;
}

boolean S_MusicExists(const char *mname, boolean checkMIDI, boolean checkDigi)
{
	if (checkDigi && W_CheckNumForName(va("O_%s", mname)) != LUMPERROR)
	{
		return true;
	}
	if (checkMIDI && W_CheckNumForName(va("D_%s", mname)) != LUMPERROR)
	{
		return true;
	}
	return false;
}

/// ------------------------
/// Music Effects
/// ------------------------

boolean S_SpeedMusicAllowed(void)
{
	return (I_SongLoaded() && music_speeding_allowed);
}

boolean S_SpeedMusic(float speed)
{
	return I_SetSongSpeed(speed);
}

float S_GetSpeedMusic(void)
{
	return I_GetSongSpeed();
}

boolean S_PitchMusicAllowed(void)
{
	return (I_SongLoaded() && music_pitching_allowed);
}

boolean S_PitchMusic(float pitch)
{
	return I_SetSongPitch(pitch);
}

float S_GetPitchMusic(void)
{
	return I_GetSongPitch();
}

//
// void S_ControlMusicEffects(void)
// Controls the effects of the currently playing music, based on factors like the Jukebox and Vape Mode.
//
void S_ControlMusicEffects(void)
{
	float new_music_speed = 0.0f, new_music_pitch = 0.0f;
	fixed_t cur_music_speed = FloatToFixed(S_GetSpeedMusic());
	fixed_t cur_music_pitch = FloatToFixed(S_GetPitchMusic());

	if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		new_music_speed = atof(cv_tsourdt3rd_jukebox_speed.string);
		new_music_pitch = atof(cv_tsourdt3rd_jukebox_pitch.string);
	}
	else
	{
		switch (cv_tsourdt3rd_audio_vapemode.value)
		{
			case 1:
				new_music_speed = new_music_pitch = 0.9f;
				break;
			case 2:
				new_music_speed = 0.75;
				new_music_pitch = 0.5f;
				break;
			default:
				new_music_speed = new_music_pitch = 1.0f;
				break;
		}
	}

	if (cur_music_speed != FloatToFixed(new_music_speed))
		S_SpeedMusic(new_music_speed);
	if (cur_music_pitch != FloatToFixed(new_music_pitch))
		S_SpeedMusic(new_music_pitch);
}

/// ------------------------
/// Music Seeking
/// ------------------------

UINT32 S_GetMusicLength(void)
{
	return I_GetSongLength();
}

//
// S_MusicInfo
//
// Returns metadata about supplied music
// If name is NULL, returns the currently playing music (if any)
//
musicdef_t *S_MusicInfo(const char *name)
{
	if (!name) name = S_MusicName();
	return S_FindMusicDef(name, NULL, NULL, NULL);
}

boolean S_SetMusicLoopPoint(UINT32 looppoint)
{
	return I_SetSongLoopPoint(looppoint);
}

UINT32 S_GetMusicLoopPoint(void)
{
	return I_GetSongLoopPoint();
}

boolean S_SetMusicPosition(UINT32 position)
{
	return I_SetSongPosition(position);
}

UINT32 S_GetMusicPosition(void)
{
	return I_GetSongPosition();
}

/// ------------------------
/// Music Stacking (Jingles)
/// In this section: mazmazz doesn't know how to do dynamic arrays or struct pointers!
/// ------------------------

char music_stack_nextmusname[7];
boolean music_stack_noposition = false;
UINT32 music_stack_fadeout = 0;
UINT32 music_stack_fadein = 0;
static musicstack_t *music_stacks = NULL;
static musicstack_t *last_music_stack = NULL;

void S_SetStackAdjustmentStart(void)
{
	if (!pause_starttic)
		pause_starttic = gametic;
}

void S_AdjustMusicStackTics(void)
{
	if (pause_starttic)
	{
		musicstack_t *mst;
		for (mst = music_stacks; mst; mst = mst->next)
			mst->tic += gametic - pause_starttic;
		pause_starttic = 0;
	}
}

static void S_ResetMusicStack(void)
{
	musicstack_t *mst, *mst_next;
	for (mst = music_stacks; mst; mst = mst_next)
	{
		mst_next = mst->next;
		Z_Free(mst);
	}
	music_stacks = last_music_stack = NULL;
}

static void S_RemoveMusicStackEntry(musicstack_t *entry)
{
	musicstack_t *mst;
	for (mst = music_stacks; mst; mst = mst->next)
	{
		if (mst == entry)
		{
			// Remove ourselves from the chain and link
			// prev and next together

			if (mst->prev)
				mst->prev->next = mst->next;
			else
				music_stacks = mst->next;

			if (mst->next)
				mst->next->prev = mst->prev;
			else
				last_music_stack = mst->prev;

			break;
		}
	}
	Z_Free(entry);
}

static void S_RemoveMusicStackEntryByStatus(UINT16 status)
{
	musicstack_t *mst, *mst_next;

	if (!status)
		return;

	for (mst = music_stacks; mst; mst = mst_next)
	{
		mst_next = mst->next;
		if (mst->status == status)
			S_RemoveMusicStackEntry(mst);
	}
}

static void S_AddMusicStackEntry(const char *mname, UINT16 mflags, boolean looping, UINT32 position, float speed, float pitch, UINT16 status)
{
	musicstack_t *mst, *new_mst;

	// if the first entry is empty, force master onto it
	if (!music_stacks)
	{
		music_stacks = Z_Calloc(sizeof (*mst), PU_MUSIC, NULL);
		strncpy(music_stacks->musname, (status == JT_MASTER ? mname : (S_CheckQueue() ? queue_name : mapmusname)), sizeof(music_stacks->musname)-1);
		music_stacks->musflags = (status == JT_MASTER ? mflags : (S_CheckQueue() ? queue_flags : mapmusflags));
		music_stacks->looping = (status == JT_MASTER ? looping : (S_CheckQueue() ? queue_looping : true));
		music_stacks->position = (status == JT_MASTER ? position : (S_CheckQueue() ? queue_position : S_GetMusicPosition()));
		music_stacks->speed = (status == JT_MASTER ? position : (S_CheckQueue() ? queue_speed : S_GetSpeedMusic()));
		music_stacks->pitch = (status == JT_MASTER ? position : (S_CheckQueue() ? queue_pitch : S_GetPitchMusic()));
		music_stacks->tic = gametic;
		music_stacks->status = JT_MASTER;
		music_stacks->mlumpnum = S_GetMusicLumpNum(music_stacks->musname);
		music_stacks->noposition = S_CheckQueue();

		if (status == JT_MASTER)
			return; // we just added the user's entry here
	}

	// look for an empty slot to park ourselves
	for (mst = music_stacks; mst->next; mst = mst->next);

	// create our new entry
	new_mst = Z_Calloc(sizeof (*new_mst), PU_MUSIC, NULL);
	strncpy(new_mst->musname, mname, 7);
	new_mst->musname[6] = 0;
	new_mst->musflags = mflags;
	new_mst->looping = looping;
	new_mst->position = position;
	new_mst->speed = speed;
	new_mst->pitch = pitch;
	new_mst->tic = gametic;
	new_mst->status = status;
	new_mst->mlumpnum = S_GetMusicLumpNum(new_mst->musname);
	new_mst->noposition = false;

	mst->next = new_mst;
	new_mst->prev = mst;
	new_mst->next = NULL;
	last_music_stack = new_mst;
}

static musicstack_t *S_GetMusicStackEntry(UINT16 status, boolean fromfirst, INT16 startindex)
{
	musicstack_t *mst, *start_mst = NULL, *mst_next;

	// if the first entry is empty, force master onto it
	// fixes a memory corruption bug
	if (!music_stacks && status != JT_MASTER)
		S_AddMusicStackEntry(mapmusname, mapmusflags, true, S_GetMusicPosition(), S_GetSpeedMusic(), S_GetPitchMusic(), JT_MASTER);

	if (startindex >= 0)
	{
		INT16 i = 0;
		for (mst = music_stacks; mst && i <= startindex; mst = mst->next, i++)
			start_mst = mst;
	}
	else
		start_mst = (fromfirst ? music_stacks : last_music_stack);

	for (mst = start_mst; mst; mst = mst_next)
	{
		mst_next = (fromfirst ? mst->next : mst->prev);

		if (!status || mst->status == status)
		{
			if (P_EvaluateMusicStatus(mst->status, mst->musname))
			{
				if (!S_MusicExists(mst->musname, !midi_disabled, !digital_disabled)) // paranoia
					S_RemoveMusicStackEntry(mst); // then continue
				else
					return mst;
			}
			else
				S_RemoveMusicStackEntry(mst); // then continue
		}
	}

	return NULL;
}

void S_RetainMusic(const char *mname, UINT16 mflags, boolean looping, UINT32 position, UINT16 status)
{
	musicstack_t *mst;

	if (!status) // we use this as a null indicator, don't push
	{
		CONS_Alert(CONS_ERROR, "Music stack entry must have a nonzero status.\n");
		return;
	}
	else if (status == JT_MASTER) // enforce only one JT_MASTER
	{
		for (mst = music_stacks; mst; mst = mst->next)
		{
			if (mst->status == JT_MASTER)
			{
				CONS_Alert(CONS_ERROR, "Music stack can only have one JT_MASTER entry.\n");
				return;
			}
		}
	}
	else // remove any existing status
		S_RemoveMusicStackEntryByStatus(status);

	S_AddMusicStackEntry(mname, mflags, looping, position, 1.0f, 1.0f, status);
}

boolean S_RecallMusic(UINT16 status, boolean fromfirst)
{
	UINT32 newpos = 0;
	boolean mapmuschanged = false;
	musicstack_t *result;
	musicstack_t *entry = Z_Calloc(sizeof (*result), PU_MUSIC, NULL);
	boolean currentmidi = (I_SongType() == MU_MID || I_SongType() == MU_MID_EX);
	boolean midipref = cv_musicpref.value;

	if (status)
		result = S_GetMusicStackEntry(status, fromfirst, -1);
	else
		result = S_GetMusicStackEntry(JT_NONE, false, -1);

	if (result && !S_MusicExists(result->musname, !midi_disabled, !digital_disabled))
	{
		Z_Free(entry);
		return false; // music doesn't exist, so don't do anything
	}

	// make a copy of result, since we make modifications to our copy
	if (result)
	{
		*entry = *result;
		memcpy(entry->musname, result->musname, sizeof(entry->musname));
	}

	// no result, just grab mapmusname
	if (!result || !entry->musname[0] || ((status == JT_MASTER || (music_stacks ? !music_stacks->status : false)) && !entry->status))
	{
		strncpy(entry->musname, mapmusname, 7);
		entry->musflags = mapmusflags;
		entry->looping = true;
		entry->position = mapmusposition;
		entry->speed = S_GetSpeedMusic();
		entry->pitch = S_GetPitchMusic();
		entry->tic = gametic;
		entry->status = JT_MASTER;
		entry->mlumpnum = S_GetMusicLumpNum(entry->musname);
		entry->noposition = false; // don't set this until we do the mapmuschanged check, below. Else, this breaks some resumes.
	}

	if (entry->status == JT_MASTER)
	{
		mapmuschanged = strnicmp(entry->musname, mapmusname, 7);
		if (mapmuschanged)
		{
			strncpy(entry->musname, mapmusname, 7);
			entry->musflags = mapmusflags;
			entry->looping = true;
			entry->position = mapmusposition;
			entry->speed = S_GetSpeedMusic();
			entry->pitch = S_GetPitchMusic();
			entry->tic = gametic;
			entry->status = JT_MASTER;
			entry->mlumpnum = S_GetMusicLumpNum(entry->musname);
			entry->noposition = true;
		}
		S_ResetMusicStack();
	}
	else if (!entry->status)
	{
		Z_Free(entry);
		return false;
	}

	if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		// STAR STUFF: currently jukeboxing, so just clear memory and move on :p //
		Z_Free(entry);
		return false;
	}

	if (strncmp(entry->musname, S_MusicName(), 7) || // don't restart music if we're already playing it
		(midipref != currentmidi && S_PrefAvailable(midipref, entry->musname))) // but do if the user's preference has changed
	{
		if (music_stack_fadeout)
			S_ChangeMusicEx(entry->musname, entry->musflags, entry->looping, 0, music_stack_fadeout, 0);
		else
		{
			S_ChangeMusicEx(entry->musname, entry->musflags, entry->looping, 0, 0, music_stack_fadein);

			if (!entry->noposition && !music_stack_noposition) // HACK: Global boolean to toggle position resuming, e.g., de-superize
			{
				UINT32 poslapse = 0;

				// To prevent the game from jumping past the end of the music, we need
				// to check if we can get the song's length. Otherwise, if the lapsed resume time goes
				// over a LOOPPOINT, mixer_sound.c will be unable to calculate the new resume position.
				if (S_GetMusicLength())
					poslapse = (UINT32)((float)(gametic - entry->tic)/(float)TICRATE*(float)MUSICRATE);

				newpos = entry->position + poslapse;
			}

			// If the newly recalled music lumpnum does not match the lumpnum that we stored in stack,
			// then discard the new position. That way, we will not recall an invalid position
			// when the music is replaced or digital/MIDI is toggled.
			if (newpos > 0 && S_MusicPlaying() && S_GetMusicLumpNum(entry->musname) == entry->mlumpnum)
				S_SetMusicPosition(newpos);
			else
			{
				S_StopFadingMusic();
				S_SetInternalMusicVolume(100);
			}
		}
		S_SpeedMusic(entry->speed);
		S_PitchMusic(entry->pitch);
		music_stack_noposition = false;
		music_stack_fadeout = 0;
		music_stack_fadein = JINGLEPOSTFADE;
	}

	Z_Free(entry);
	return true;
}

/// ------------------------
/// Music Playback
/// ------------------------

static lumpnum_t S_GetMusicLumpNum(const char *mname)
{
	boolean midipref = cv_musicpref.value;

	if (S_PrefAvailable(midipref, mname))
		return W_GetNumForName(va(midipref ? "d_%s":"o_%s", mname));
	else if (S_PrefAvailable(!midipref, mname))
		return W_GetNumForName(va(midipref ? "o_%s":"d_%s", mname));
	else
		return TSoURDt3rd_EXMusic_DefaultMapTrack_Play(&mname); // STAR STUFF: play some fallback music... //
}

static inline void S_UnloadMusic(void)
{
	I_UnloadSong();

#ifndef HAVE_SDL //SDL uses RWOPS
	if (music_data)
		Z_ChangeTag(music_data, PU_CACHE);
#endif
	music_data = NULL;

	music_name[0] = 0;
	music_flags = 0;
	music_looping = false;
	music_speeding_allowed = music_pitching_allowed = false;
}

static inline boolean S_PlayMusic(const char *mname, boolean looping, UINT32 fadeinms)
{
	lumpnum_t mlumpnum;
	musicdef_t *def;

	if (S_MusicDisabled())
		return false;
	S_StopMusic();

	mlumpnum = S_GetMusicLumpNum(mname);
	if (mlumpnum == LUMPERROR)
	{
		CONS_Alert(CONS_ERROR, "Music %.6s could not be loaded: lump not found!\n", mname);
		return false;
	}

	// load & register it
	music_data = W_CacheLumpNum(mlumpnum, PU_MUSIC);

	if (I_LoadSong(music_data, W_LumpLength(mlumpnum)))
	{
		strncpy(music_name, mname, 7);
		music_name[6] = 0;
	}
	else
	{
		CONS_Alert(CONS_ERROR, "Music %.6s could not be loaded: engine failure!\n", mname);
		S_UnloadMusic();
		return false;
	}

	if ((!fadeinms && !I_PlaySong(looping)) ||
		(fadeinms && !I_FadeInPlaySong(fadeinms, looping)))
	{
		CONS_Alert(CONS_ERROR, "Music %.6s could not be played: engine failure!\n", music_name);
		S_UnloadMusic();
		return false;
	}

	/* set loop point from MUSICDEF */
	def = S_FindMusicDef(music_name, NULL, NULL, NULL);
	if (def && def->loop_ms)
		S_SetMusicLoopPoint(def->loop_ms);

	S_InitMusicVolume(); // switch between digi and sequence volume

	if (S_MusicNotInFocus())
		S_PauseMusic();

	CONS_Debug(DBG_DETAILED, "Now playing song %s\n", music_name);
	return true;
}

static void S_QueueMusic(const char *mmusic, UINT16 mflags, boolean looping, UINT32 position, UINT32 fadeinms)
{
	strncpy(queue_name, mmusic, 7);
	queue_flags = mflags;
	queue_looping = looping;
	queue_position = position;
	queue_speed = queue_pitch = 1.0f;
	queue_fadeinms = fadeinms;
}

static boolean S_CheckQueue(void)
{
	return queue_name[0];
}

static void S_ClearQueue(void)
{
	queue_name[0] = queue_flags = queue_position = queue_fadeinms = 0;
	queue_speed = queue_pitch = 1.0f;
	queue_looping = false;
}

static void S_ChangeMusicToQueue(void)
{
	S_ChangeMusicEx(queue_name, queue_flags, queue_looping, queue_position, 0, queue_fadeinms);
	S_SpeedMusic(queue_speed);
	S_PitchMusic(queue_pitch);
	S_ClearQueue();
}

void S_ChangeMusicEx(const char *mmusic, UINT16 mflags, boolean looping, UINT32 position, UINT32 prefadems, UINT32 fadeinms)
{
	char newmusic[7];

	struct MusicChange hook_param = {
		newmusic,
		&mflags,
		&looping,
		&position,
		&prefadems,
		&fadeinms
	};

	boolean currentmidi = (I_SongType() == MU_MID || I_SongType() == MU_MID_EX);
	boolean midipref = cv_musicpref.value;

	if (S_MusicDisabled())
		return;

	if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		// STAR STUFF: jukebox has priority //
		return;
	}

	strncpy(newmusic, mmusic, sizeof(newmusic)-1);
	newmusic[6] = 0;
	if (LUA_HookMusicChange(music_name, &hook_param))
		return;

	// No Music (empty string)
	if (newmusic[0] == 0)
	{
		if (prefadems)
			I_FadeSong(0, prefadems, &S_StopMusic);
		else
			S_StopMusic();
		return;
	}

	if (prefadems) // queue music change for after fade, allow even if no music is playing or music is the same
	{
		CONS_Debug(DBG_DETAILED, "Now fading out song %s\n", music_name);
		S_QueueMusic(newmusic, mflags, looping, position, fadeinms);
		I_FadeSong(0, prefadems, S_ChangeMusicToQueue);
		return;
	}
	else if (strnicmp(music_name, newmusic, 6) || (mflags & MUSIC_FORCERESET) ||
		(midipref != currentmidi && S_PrefAvailable(midipref, newmusic)))
	{
		if (!S_PlayMusic(newmusic, looping, fadeinms))
			return;

		music_flags = mflags;
		music_looping = looping;
		music_speeding_allowed = S_SpeedMusic(1.0f);
		music_pitching_allowed = S_PitchMusic(1.0f);

		if (position)
			I_SetSongPosition(position);

		I_SetSongTrack(mflags & MUSIC_TRACKMASK);

		S_LoadMusicCredit();
		S_ShowMusicCredit();
	}
	else if (fadeinms) // let fades happen with same music
	{
		I_SetSongPosition(position);
		I_FadeSong(100, fadeinms, NULL);
	}
	else // reset volume to 100 with same music
	{
		I_StopFadingSong();
		I_FadeSong(100, 500, NULL);
	}

	S_ControlMusicEffects(); // STAR STUFF: Set the effects again, just to be sure.... //
}

void S_StopMusic(void)
{
	if (!I_SongLoaded())
		return;

	TSoURDt3rd_Jukebox_Stop(); // STAR STUFF: reset music stuffs //

	I_StopSong();
	S_UnloadMusic(); // for now, stopping also means you unload the song

	S_StopMusicCredit();
	S_UnloadMusicCredit();

	if (cv_closedcaptioning.value)
	{
		if (closedcaptions[0].s-S_sfx == sfx_None)
		{
			if (gamestate != wipegamestate)
			{
				closedcaptions[0].c = NULL;
				closedcaptions[0].s = NULL;
				closedcaptions[0].t = 0;
				closedcaptions[0].b = 0;
			}
			else
				closedcaptions[0].t = CAPTIONFADETICS;
		}
	}
}

//
// Stop and resume music, during game PAUSE.
//
void S_PauseMusic(void)
{
	if (I_SongLoaded() && !I_SongPaused())
		I_PauseSong();

	S_SetStackAdjustmentStart();
}

void S_ResumeMusic(void)
{
	if (S_MusicNotInFocus())
		return;

	if (I_SongLoaded() && I_SongPaused())
		I_ResumeSong();

	S_AdjustMusicStackTics();
}

void S_SetMusicVolume(INT32 digvolume, INT32 seqvolume)
{
	if (digvolume < 0)
		digvolume = cv_digmusicvolume.value;
	if (seqvolume < 0)
		seqvolume = cv_midimusicvolume.value;

	if (digvolume < 0 || digvolume > 31)
		CONS_Alert(CONS_WARNING, "digmusicvolume should be between 0-31\n");
	CV_SetValue(&cv_digmusicvolume, digvolume&31);
	actualdigmusicvolume = cv_digmusicvolume.value;   //check for change of var

	if (seqvolume < 0 || seqvolume > 31)
		CONS_Alert(CONS_WARNING, "midimusicvolume should be between 0-31\n");
	CV_SetValue(&cv_midimusicvolume, seqvolume&31);
	actualmidimusicvolume = cv_midimusicvolume.value;   //check for change of var

	switch(I_SongType())
	{
		case MU_MID:
		case MU_MID_EX:
		//case MU_MOD:
		//case MU_GME:
			I_SetMusicVolume(seqvolume&31);
			break;
		default:
			I_SetMusicVolume(digvolume&31);
			break;
	}
}

/// ------------------------
/// Music Fading
/// ------------------------

void S_SetInternalMusicVolume(INT32 volume)
{
	internal_music_volume = min(max(volume, 0), 100);
	I_SetInternalMusicVolume((UINT8)internal_music_volume);
}

INT32 S_GetInternalMusicVolume(void)
{
	return internal_music_volume;
}

void S_StopFadingMusic(void)
{
	I_StopFadingSong();
}

boolean S_FadeMusicFromVolume(UINT8 target_volume, INT16 source_volume, UINT32 ms)
{
	if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		// STAR STUFF: don't fade if jukeboxing please //
		return false;
	}

	if (source_volume < 0)
		return I_FadeSong(target_volume, ms, NULL);
	else
		return I_FadeSongFromVolume(target_volume, source_volume, ms, NULL);
}

boolean S_FadeOutStopMusic(UINT32 ms)
{
	if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		// STAR STUFF: don't fade if jukeboxing please //
		return false;
	}

	return I_FadeSong(0, ms, &S_StopMusic);
}

/// ------------------------
/// Init & Others
/// ------------------------

//
// Per level startup code.
// Determines music if any, changes music.
//
void S_StartEx(boolean reset)
{
	if (mapmusflags & MUSIC_RELOADRESET)
	{
		strncpy(mapmusname, TSoURDt3rd_EXMusic_DetermineLevelMusic(), 7);
		mapmusname[6] = 0;
		mapmusflags = (mapheaderinfo[gamemap-1]->mustrack & MUSIC_TRACKMASK);
		mapmusposition = mapheaderinfo[gamemap-1]->muspos;
	}

	if (!TSoURDt3rd_Jukebox_SongPlaying()) // STAR NOTE: only if we're not jukebox do we do this //
	{
		if (RESETMUSIC || reset)
			S_StopMusic();
		S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0);
	}

	S_ResetMusicStack();
	music_stack_noposition = false;
	music_stack_fadeout = 0;
	music_stack_fadein = JINGLEPOSTFADE;
}

static void Command_Tunes_f(void)
{
	const char *tunearg;
	UINT16 track = 0;
	UINT32 position = 0;
	const size_t argc = COM_Argc();

	if (argc < 2) //tunes slot ...
	{
		CONS_Printf("tunes <name/num> [track] [speed] [pitch] [position] / <-show> / <-showmapmus> / <-showdefault> / <-default> / <-none>:\n");
		CONS_Printf(M_GetText("Play an arbitrary music lump. If a map number is used, 'MAP##M' is played.\n"));
		CONS_Printf(M_GetText("If the format supports multiple songs, you can specify which one to play.\n\n"));
		CONS_Printf(M_GetText("* With \"-show\", shows the currently playing tune and track.\n"));
		CONS_Printf(M_GetText("* With \"-showmapmus\", shows the current mapmusname tune for the level.\n"));
		CONS_Printf(M_GetText("* With \"-showdefault\", shows the default music for the level.\n"));
		CONS_Printf(M_GetText("* With \"-default\", returns to the default music for the map.\n"));
		CONS_Printf(M_GetText("* With \"-none\", any music playing will be stopped.\n"));
		return;
	}

	tunearg = COM_Argv(1);
	track = 0;

	if (!strcasecmp(tunearg, "-show"))
	{
		CONS_Printf(M_GetText("The current tune is: %s [track %d]\n"), S_MusicName(), (mapmusflags & MUSIC_TRACKMASK));
		PrintMusicDef(S_MusicName());
		return;
	}
	else if (!strcasecmp(tunearg, "-showmapmus"))
	{
		CONS_Printf(M_GetText("The current mapmusname tune is: %s [track %d]\n"), mapmusname, (mapmusflags & MUSIC_TRACKMASK));
		PrintMusicDef(mapmusname);
		return;
	}
	else if (!strcasecmp(tunearg, "-showdefault"))
	{
		CONS_Printf(M_GetText("The default level tune is: %s [track %d]\n"), mapheaderinfo[gamemap-1]->musname, (mapmusflags & MUSIC_TRACKMASK));
		PrintMusicDef(mapheaderinfo[gamemap-1]->musname);
		return;
	}
	else if (!strcasecmp(tunearg, "-none"))
	{
		S_StopMusic();
		return;
	}
	else if (!strcasecmp(tunearg, "-default"))
	{
		tunearg = mapheaderinfo[gamemap-1]->musname;
		track = mapheaderinfo[gamemap-1]->mustrack;
	}

	// STAR STUFF: minor tunes propaganda //
	if (TSoURDt3rd_AprilFools_ModeEnabled())
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_APRILFOOLS|STAR_CONS_WARNING, "Nice try. Perhaps there's a command you need to turn off first?\n");
		return;
	}
	else if (TSoURDt3rd_Jukebox_SongPlaying())
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD|STAR_CONS_JUKEBOX|STAR_CONS_WARNING, "Sorry, you can't use this command while playing music.\n");
		return;
	}

	if (strlen(tunearg) > 6) // This is automatic -- just show the error just in case
		CONS_Alert(CONS_NOTICE, M_GetText("Music name too long - truncated to six characters.\n"));

	if (argc > 2)
		track = (UINT16)atoi(COM_Argv(2))-1;

	strncpy(mapmusname, tunearg, 7);
	mapmusname[6] = 0;

	if (argc > 5)
		position = (UINT32)atoi(COM_Argv(5));

	mapmusflags = (track & MUSIC_TRACKMASK);
	mapmusposition = position;

	S_ChangeMusicEx(mapmusname, mapmusflags, true, mapmusposition, 0, 0);
	if (argc > 3)
		S_SpeedMusic((float)atof(COM_Argv(3)));
	if (argc > 4)
		S_PitchMusic((float)atof(COM_Argv(4)));
}

static void MusicMixer_OnChange(void)
{
	if (!sound_started)
		return;
	S_RestartAudio();
}

static void GameSounds_OnChange(void)
{
	if (M_CheckParm("-nosound") || M_CheckParm("-noaudio"))
		return;

	if (sound_disabled)
	{
		sound_disabled = false;
		I_StartupSound(); // will return early if initialised
		S_InitSfxChannels(cv_soundvolume.value);
		S_StartSoundFromEverywhere(sfx_strpst);
	}
	else
	{
		sound_disabled = true;
		S_StopSounds();
	}
}

static void GameDigiMusic_OnChange(void)
{
	if (M_CheckParm("-nomusic") || M_CheckParm("-noaudio"))
		return;
	else if (M_CheckParm("-nodigmusic"))
		return;

	if (digital_disabled)
	{
		digital_disabled = false;
		I_StartupSound(); // will return early if initialised
		I_InitMusic();

		if (Playing())
			P_RestoreMusic(&players[consoleplayer]);
		else if ((!cv_musicpref.value || midi_disabled) && S_DigExists("_clear"))
			S_ChangeMusicInternal("_clear", false);
	}
	else
	{
		digital_disabled = true;
		if (S_MusicType() != MU_MID && S_MusicType() != MU_MID_EX)
		{
			S_StopMusic();
			if (!midi_disabled && sound_started)
			{
				if (Playing())
					P_RestoreMusic(&players[consoleplayer]);
				else
					S_ChangeMusicInternal("_clear", false);
			}
		}
	}
}

static void GameMIDIMusic_OnChange(void)
{
	if (M_CheckParm("-nomusic") || M_CheckParm("-noaudio"))
		return;
	else if (M_CheckParm("-nomidimusic"))
		return;

	if (midi_disabled)
	{
		midi_disabled = false;
		I_StartupSound(); // will return early if initialised
		I_InitMusic();

		if (Playing())
			P_RestoreMusic(&players[consoleplayer]);
		else if ((cv_musicpref.value || digital_disabled) && S_MIDIExists("_clear"))
			S_ChangeMusicInternal("_clear", false);
	}
	else
	{
		midi_disabled = true;
		if (S_MusicType() == MU_MID || S_MusicType() == MU_MID_EX)
		{
			S_StopMusic();
			if (!digital_disabled && sound_started)
			{
				if (Playing())
					P_RestoreMusic(&players[consoleplayer]);
				else
					S_ChangeMusicInternal("_clear", false);
			}
		}
	}
}

static void MusicPref_OnChange(void)
{
	if (M_CheckParm("-nomusic") || M_CheckParm("-noaudio"))
		return;
	else if (M_CheckParm("-nomidimusic") || M_CheckParm("-nodigmusic"))
		return;
	else if (!sound_started) // StarManiaKG: i'd rather not see thousands of sound errors on startup, thanks //
		return;

	if (Playing())
		P_RestoreMusic(&players[consoleplayer]);
	else if (S_PrefAvailable(cv_musicpref.value, "_clear"))
		S_ChangeMusicInternal("_clear", false);
}

#ifdef HAVE_OPENMPT
static void ModFilter_OnChange(void)
{
	if (openmpt_mhandle)
		openmpt_module_set_render_param(openmpt_mhandle, OPENMPT_MODULE_RENDER_INTERPOLATIONFILTER_LENGTH, cv_modfilter.value);
}
#endif
