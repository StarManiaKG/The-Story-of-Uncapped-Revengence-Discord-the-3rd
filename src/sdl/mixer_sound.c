// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2014-2025 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  mixer_sound.c
/// \brief SDL_Mixer/SDL_Mixer-X interface for sound

#ifdef HAVE_GME
#ifdef HAVE_ZLIB
#ifndef _MSC_VER
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#endif

#ifndef _LFS64_LARGEFILE
#define _LFS64_LARGEFILE
#endif

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 0
#endif

#include <zlib.h>
#endif // HAVE_ZLIB
#endif // HAVE_GME

#include "../doomdef.h"
#include "../doomstat.h"

#if defined(HAVE_SDL) && defined(HAVE_MIXER) && SOUND==SOUND_MIXER

#include "../sounds.h"
#include "../s_sound.h"
#include "../i_sound.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../byteptr.h"
#include "../g_game.h" // P_RestoreMusic
#include "../p_local.h" // players[]
#include "../filesrch.h" // pathisdirectory

#ifdef _MSC_VER
#pragma warning(disable : 4214 4244)
#endif
#include "SDL.h"
#ifdef _MSC_VER
#pragma warning(default : 4214 4244)
#endif

#ifdef HAVE_MIXERX
	#ifdef _WIN32
		#include <SDL_mixer_ext.h>
	#else
		#include <SDL2/SDL_mixer_ext.h>
	#endif
	#define SDL_MIXER_HANDLER "SDL_Mixer_X"
#else
	#include <SDL_mixer.h>
	#define SDL_MIXER_HANDLER "SDL_Mixer"
#endif

/* This is the version number macro for the current SDL_mixer version: */
#ifndef SDL_MIXER_COMPILEDVERSION
#define SDL_MIXER_COMPILEDVERSION \
	SDL_VERSIONNUM(MIX_MAJOR_VERSION, MIX_MINOR_VERSION, MIX_PATCHLEVEL)
#endif

/* This macro will evaluate to true if compiled with SDL_mixer at least X.Y.Z */
#ifndef SDL_MIXER_VERSION_ATLEAST
#define SDL_MIXER_VERSION_ATLEAST(X, Y, Z) \
	(SDL_MIXER_COMPILEDVERSION >= SDL_VERSIONNUM(X, Y, Z))
#endif

// thanks alam for making the buildbots happy!
#if SDL_MIXER_VERSION_ATLEAST(2,0,2)
#define MUS_MP3_MAD MUS_MP3_MAD_UNUSED
#define MUS_MODPLUG MUS_MODPLUG_UNUSED
#endif

#ifdef HAVE_GME
#include <gme/gme.h>
#define GME_TREBLE 5.0f
#define GME_BASS 1.0f
#endif // HAVE_GME

#ifdef HAVE_OPENMPT
#include "libopenmpt/libopenmpt.h"
#endif

/// ------------------------
/// Audio Declarations
/// ------------------------

boolean sound_started = false;

static const int AUDIO_CHANNELS = 2;
static const SDL_AudioFormat AUDIO_FORMAT = AUDIO_S16SYS;
static const int AUDIO_MAX_NUM_CHANNELS = 256;
static int AUDIO_NUM_CHANNELS = 0;
static long double AUDIO_SAMPLERATE;
static long double AUDIO_INTERNAL_SAMPLERATE;

static Mix_Music *music;
static char *music_memory;
static UINT8 music_volume, sfx_volume, internal_music_volume, internal_sfx_volume;
static float music_speed, music_pitch;
static float loop_point;
static float song_length; // length in seconds
static boolean songpaused;
static UINT32 music_bytes;
static boolean is_looping;

static boolean codec_allows_position;
static boolean codec_allows_looppoints;
static boolean codec_allows_songvolume;
static boolean codec_allows_speeding;
static boolean codec_allows_pitching;
static float codec_max_speed;
static float codec_max_pitch;

static I_mutex i_mixer_fade = NULL;

// fading
static boolean is_fading;
static UINT8 fading_source;
static UINT8 fading_target;
static UINT32 fading_timer;
static UINT32 fading_duration;
static INT32 fading_id;
static void (*fading_callback)(void);
static boolean fading_do_callback;
static boolean fading_nocleanup;

#ifdef HAVE_GME
static Music_Emu *gme;
static UINT16 current_track;
#endif

#ifdef HAVE_OPENMPT
static int mod_err = OPENMPT_ERROR_OK;
static const char *mod_err_str;
static UINT16 current_subsong;
static size_t probesize;
static int result;
#endif

#ifdef HAVE_MIXERX

static I_mutex i_music_midiplayerload = NULL;

// MIDI_EDMIDI didn't always exist, so we need to compensate for that!
#if !SDL_MIXER_VERSION_ATLEAST(2,5,1)
#define MIDI_EDMIDI MIDI_ANY
#endif

#ifdef _WIN32
#define TIMIDITY_CFG_DIR "sf2\\timidity"
#else
#define TIMIDITY_CFG_DIR "/etc/timidity"
#endif

static INT32 fluidsynth_loadsoundfonts(const char *soundfont)
{
	SDL_RWops *rw = NULL;
	const char *current_soundfont = Mix_GetSoundFonts();
	char *source;
	char *miditoken;

	if (soundfont == NULL)
	{
		CONS_Alert(CONS_ERROR, "No Fluidsynth soundfont given to load!\n");
		return 0;
	}
	else if (current_soundfont != NULL && !stricmp(current_soundfont, soundfont))
	{
		// We're already using these soundfonts!
		return 2;
	}

	source = strdup(soundfont);
	miditoken = strtok(source, ";"); // get first token

	while (miditoken != NULL) // find soundfont files
	{
		rw = SDL_RWFromFile(miditoken, "r");
		if (rw == NULL)
			break;
		miditoken = strtok(NULL, ";");
		SDL_RWclose(rw);
	}
	free(source);

	if (miditoken == NULL || Mix_SetSoundFonts(soundfont) == 0)
	{
		CONS_Alert(CONS_ERROR, "One or more fluidsynth soundfont paths are invalid!\n");
		return 0;
	}
	return 1;
}

static boolean timidity_setcfg(const char *cfg)
{
	const char *CFGDirectories[] = { cfg, TIMIDITY_CFG_DIR, "." PATHSEP, NULL };
	const char *timiditycfg = NULL;
	INT32 i;

	if (Mix_GetMidiPlayer() != MIDI_Timidity || (I_SongType() != MU_NONE && I_SongType() != MU_MID_EX))
		return false;

	for (i = 0; CFGDirectories[i] != NULL; i++)
	{
		const char *file = va("%s" PATHSEP "timidity.cfg", CFGDirectories[i]);
		SDL_RWops *rw = SDL_RWFromFile(file, "r");
		if (rw != NULL)
		{
			SDL_RWclose(rw);
			timiditycfg = file;
			break;
		}
		else if (pathisdirectory(CFGDirectories[i]))
		{
			timiditycfg = CFGDirectories[i];
			break;
		}
	}
	if (timiditycfg == NULL)
	{
		CONS_Alert(CONS_ERROR, "Couldn't get Timidity CFG!\n");
		return false;
	}

#if SDL_MIXER_VERSION_ATLEAST(2,0,4)
	Mix_SetTimidityCfg(timiditycfg);
#else
	Mix_Timidity_addToPathList(timiditycfg);
#endif

	return true;
}

static boolean midi_loadplayer(void)
{
	switch (Mix_GetMidiPlayer())
	{
		case MIDI_Fluidsynth:
			return fluidsynth_loadsoundfonts(cv_midisoundfontpath.string);
		case MIDI_Timidity:
			return timidity_setcfg(cv_miditimiditypath.string);
		default:
			return true;
	}
}

static void Midiplayer_Onchange(void)
{
	const INT32 prev_midi_player = Mix_GetMidiPlayer();
	const INT32 new_midi_player = cv_midiplayer.value;

	if (!sound_started || (I_SongType() != MU_NONE && I_SongType() != MU_MID_EX && I_SongType() != MU_MID))
		return;

	S_StopMusic();

	// Threading is required for Fluidsynth to load properly,
	// at least on later MixerX versions.
	I_lock_mutex(&i_music_midiplayerload);
	{
		if (prev_midi_player != new_midi_player && Mix_SetMidiPlayer(new_midi_player) != 0)
		{
			CONS_Alert(CONS_ERROR, "Midi player error (for MIDI type '%s'): %s\n", cv_midiplayer.string, Mix_GetError());
		}
		if (prev_midi_player != Mix_GetMidiPlayer() && midi_loadplayer())
		{
			S_StopMusic();
			if (Playing())
				P_RestoreMusic(&players[consoleplayer]);
			else
				S_ChangeMusicInternal("_title", false);
		}
	}
	I_unlock_mutex(i_music_midiplayerload);
}

// check if soundfont file or path exists; menu calls this method at every keystroke
static void MidiSoundfontPath_Onchange(void)
{
	if (Mix_GetMidiPlayer() != MIDI_Fluidsynth || (I_SongType() != MU_NONE && I_SongType() != MU_MID_EX))
		return;

	if (fluidsynth_loadsoundfonts(cv_midisoundfontpath.string) == 1)
	{
		S_StopMusic();
		if (Playing())
			P_RestoreMusic(&players[consoleplayer]);
		else
			S_ChangeMusicInternal("_title", false);
	}
}

// check if timidity cfg or path exists; menu calls this method at every keystroke
static void MidiTimidityPath_OnChange(void)
{
	if (Mix_GetMidiPlayer() != MIDI_Timidity || (I_SongType() != MU_NONE && I_SongType() != MU_MID_EX))
		return;

	if (timidity_setcfg(cv_miditimiditypath.string))
	{
		S_StopMusic();
		if (Playing())
			P_RestoreMusic(&players[consoleplayer]);
		else
			S_ChangeMusicInternal("_title", false);
	}
}

// make sure that s_sound.c does not already verify these
// which happens when: defined(HAVE_MIXERX) && !defined(HAVE_MIXER)
static CV_PossibleValue_t midiplayer_cons_t[] = {
	{MIDI_ADLMIDI,    "ADLMIDI"},
	{MIDI_Native,     "Native"},
	{MIDI_Timidity,   "Timidity"},
	{MIDI_OPNMIDI,    "OPNMIDI"},
	{MIDI_Fluidsynth, "Fluidsynth"},
	{MIDI_EDMIDI,     "EDMIDI"},
	{MIDI_ANY,        "Any"},
	{0,               NULL}
};
consvar_t cv_midiplayer = CVAR_INIT ("midiplayer", "OPNMIDI", CV_SAVE|CV_CALL|CV_NOINIT, midiplayer_cons_t, Midiplayer_Onchange);
consvar_t cv_midisoundfontpath = CVAR_INIT ("midisoundfont", "sf2" PATHSEP "8bitsf.SF2", CV_SAVE|CV_CALL|CV_NOINIT, NULL, MidiSoundfontPath_Onchange);
consvar_t cv_miditimiditypath = CVAR_INIT ("midisoundbank", TIMIDITY_CFG_DIR, CV_SAVE|CV_CALL|CV_NOINIT, NULL, MidiTimidityPath_OnChange);

#endif // HAVE_MIXERX

static inline void var_cleanup(void)
{
	// dependant on 'cv_samplerate.value'
	//AUDIO_SAMPLERATE = (long double)cv_samplerate.value;

	// individual files can set it; by default, I_LoadSong sets this to 44100hz.
	// helps with looping.
	//AUDIO_INTERNAL_SAMPLERATE = 0.0L;
	AUDIO_INTERNAL_SAMPLERATE = 44100.0L;

	song_length = loop_point = 0.0f;
	songpaused = is_looping = false;

	music_bytes = 0;
	music_speed = music_pitch = 1.0f;

	codec_allows_position = codec_allows_looppoints = codec_allows_songvolume
	= codec_allows_speeding = codec_allows_pitching = false;
	codec_max_speed = codec_max_pitch = 20.0f;

	is_fading = false;
	fading_source = fading_target = fading_timer = fading_duration = 0;
	if (!fading_nocleanup)
	{
		// HACK: See music_loop, where we want the fade timing to proceed after a non-looping
		// song has stopped playing
		fading_callback = NULL;
		fading_do_callback = false;
	}
	else
	{
		// use it once, set it back immediately
		fading_nocleanup = false;
	}

	internal_music_volume = 100;
	internal_sfx_volume = 31;
}

static inline void music_cleanup(void)
{
#ifdef HAVE_GME
	if (gme)
	{
		gme_delete(gme);
		gme = NULL;
	}
#endif
#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		openmpt_module_destroy(openmpt_mhandle);
		openmpt_mhandle = NULL;
	}
#endif
	if (music)
	{
		Mix_FreeMusic(music);
		music = NULL;
	}
	if (music_memory != NULL)
	{
		Z_Free(music_memory);
		music_memory = NULL;
	}
}

static boolean audio_init(INT32 SAMPLERATE)
{
	if (sound_started)
	{
		Mix_CloseAudio();
#if SDL_MIXER_VERSION_ATLEAST(1,2,11)
		Mix_Quit();
#endif
	}

	sound_started = false;
	songpaused = false;

#if SDL_MIXER_VERSION_ATLEAST(1,2,11)
	if (sound_started)
	{
		Mix_Init(MIX_INIT_FLAC|MIX_INIT_MOD|MIX_INIT_MP3|MIX_INIT_OGG|MIX_INIT_MID|MIX_INIT_OPUS);
	}
#endif

	if (Mix_OpenAudio(SAMPLERATE, AUDIO_FORMAT, AUDIO_CHANNELS, cv_buffersize.value) < 0)
	{
		CONS_Alert(CONS_ERROR, "Error starting " SDL_MIXER_HANDLER ": %s\n", Mix_GetError());
		// call to start mixer failed -- we do not have it
		return false;
	}

	sound_started = true;
	songpaused = false;

#ifdef HAVE_MIXERX
	Mix_SetMidiPlayer(cv_midiplayer.value);
	Mix_SetSoundFonts(cv_midisoundfontpath.string);
	midi_loadplayer();
#endif

	AUDIO_SAMPLERATE = SAMPLERATE;
	AUDIO_NUM_CHANNELS = Mix_AllocateChannels(AUDIO_MAX_NUM_CHANNELS);
	return true;
}

#if defined (HAVE_GME) && defined (HAVE_ZLIB)
static const char* get_zlib_error(int zErr)
{
	switch (zErr)
	{
		case Z_ERRNO:
			return "Z_ERRNO";
		case Z_STREAM_ERROR:
			return "Z_STREAM_ERROR";
		case Z_DATA_ERROR:
			return "Z_DATA_ERROR";
		case Z_MEM_ERROR:
			return "Z_MEM_ERROR";
		case Z_BUF_ERROR:
			return "Z_BUF_ERROR";
		case Z_VERSION_ERROR:
			return "Z_VERSION_ERROR";
		default:
			return "unknown error";
	}
}
#endif

#if defined (HAVE_OPENMPT)
static void print_openmpt_string(const char *string, const char *openmpt_string)
{
	const char *openmpt_gotten_string = openmpt_get_string(openmpt_string);

	if (openmpt_gotten_string == NULL)
		openmpt_gotten_string = "unknown";

	CONS_Printf("libopenmpt %s: %s\n", string, openmpt_gotten_string);
	openmpt_free_string(openmpt_gotten_string);
}
#endif

/// ------------------------
/// Audio System
/// ------------------------

void I_StartupSound(void)
{
	if (sound_started)
		return;

#ifdef _WIN32
	// Force DirectSound instead of WASAPI
	// SDL 2.0.6+ defaults to the latter and it screws up our sound effects
	SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);
#endif

	// EE inits audio first so we're following along.
	if (SDL_WasInit(SDL_INIT_AUDIO) == SDL_INIT_AUDIO)
	{
		CONS_Debug(DBG_DETAILED, "SDL Audio already started\n");
		return;
	}
	else if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		CONS_Alert(CONS_ERROR, "Error initializing SDL Audio: %s\n", SDL_GetError());
		// call to start audio failed -- we do not have it
		return;
	}

	fading_nocleanup = false;
	var_cleanup();

	music_cleanup();
	music_volume = sfx_volume = 0;

	if (audio_init(cv_samplerate.value) == false)
	{
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}

	{
		SDL_version SDLmixcompiled;
		const SDL_version *SDLmixlinked = Mix_Linked_Version();
		SDL_MIXER_VERSION(&SDLmixcompiled)

		CONS_Printf("Compiled for " SDL_MIXER_HANDLER " version: %d.%d.%d\n", SDLmixcompiled.major, SDLmixcompiled.minor, SDLmixcompiled.patch);
		CONS_Printf("Linked with " SDL_MIXER_HANDLER " version: %d.%d.%d\n", SDLmixlinked->major, SDLmixlinked->minor, SDLmixlinked->patch);
	}
#ifdef HAVE_OPENMPT
	{
		print_openmpt_string("build date", "build");
		print_openmpt_string("version", "library_version");
	}
#endif
}

void I_ShutdownSound(void)
{
	if (!sound_started)
		return; // not an error condition

	I_StopSong();
	music_cleanup();
	var_cleanup();

	Mix_CloseAudio();
#if SDL_MIXER_VERSION_ATLEAST(1,2,11)
	Mix_Quit();
#endif

	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	sound_started = false;
	AUDIO_NUM_CHANNELS = 0;
}

void I_UpdateSound(void)
{
	if (fading_do_callback)
	{
		if (fading_callback)
			(*fading_callback)();
		fading_callback = NULL;
		fading_do_callback = false;
	}
}

/// ------------------------
/// SFX
/// ------------------------

static UINT8 get_real_sfxvolume(UINT8 volume)
{
	// (256 * 31) / 62 == 127
	return (((UINT16)volume + 1) * (UINT16)sfx_volume) / 62;
}

// SFX callback for speeding sounds.
// This could also maybe work on music too if you want that, but MixerX can already do that so...
static void mix_speed_chunk(INT32 channel, void *stream, INT32 len, void *udata)
{
	channel_t *c = &channels[channel];
	Mix_Chunk *chunk = (Mix_Chunk*)udata;

	INT16 *sound_data = (INT16*)chunk->abuf;
	INT16 *sound_samples = (INT16*)stream;

	INT32 nsamples = (len / sizeof(INT16));
	INT32 sound_frames = ((chunk->alen / sizeof(INT16)) / AUDIO_CHANNELS); // DEFAULT

	if (c == NULL || c->position > sound_frames)
		return;

	for (INT32 i = 0; i < nsamples / AUDIO_CHANNELS; i++) // iterate frames
	{
		INT32 ipos = (INT32)c->position;   // integer frame position
		float frac = (c->position - ipos); // fractional part

		if (ipos >= sound_frames - 1)
		{
			// loop around
			ipos = 0;
		}

		// Linear interpolation
		for (INT32 ch = 0; ch < AUDIO_CHANNELS; ch++)
		{
			INT16 left = sound_data[ipos * AUDIO_CHANNELS + ch];
			INT16 right = sound_data[((ipos + 1) % sound_frames) * AUDIO_CHANNELS + ch];
			sound_samples[i * AUDIO_CHANNELS + ch] = (INT16)(left + frac * (right - left));
		}

		// Advance the sound by *frames*, not samples.
		// Then, wrap the sound position, just in case.
		c->position += c->speed;
	}
}

// this is as fast as I can possibly make it.
// sorry. more asm needed.
static Mix_Chunk *ds2chunk(void *stream)
{
	UINT16 ver,freq;
	UINT32 samples, i, newsamples;
	UINT8 *sound;

	SINT8 *s;
	INT16 *d;
	INT16 o;
	fixed_t step, frac;

	// lump header
	ver = READUINT16(stream); // sound version format?
	if (ver != 3) // It should be 3 if it's a doomsound...
		return NULL; // onos! it's not a doomsound!
	freq = READUINT16(stream);
	samples = READUINT32(stream);

	// convert from signed 8bit ???hz to signed 16bit 44100hz.
	switch(freq)
	{
	case 44100:
		if (samples >= UINT32_MAX>>2)
			return NULL; // would wrap, can't store.
		newsamples = samples;
		break;
	case 22050:
		if (samples >= UINT32_MAX>>3)
			return NULL; // would wrap, can't store.
		newsamples = samples<<1;
		break;
	case 11025:
		if (samples >= UINT32_MAX>>4)
			return NULL; // would wrap, can't store.
		newsamples = samples<<2;
		break;
	default:
		frac = (44100 << FRACBITS) / (UINT32)freq;
		if (!(frac & 0xFFFF)) // other solid multiples (change if FRACBITS != 16)
			newsamples = samples * (frac >> FRACBITS);
		else // strange and unusual fractional frequency steps, plus anything higher than 44100hz.
			newsamples = FixedMul(FixedDiv(samples, freq), 44100) + 1; // add 1 to counter truncation.
		if (newsamples >= UINT32_MAX>>2)
			return NULL; // would and/or did wrap, can't store.
		break;
	}
	sound = Z_Malloc(newsamples<<AUDIO_CHANNELS, PU_SOUND, NULL); // samples * frequency shift * bytes per sample * channels

	s = (SINT8 *)stream;
	d = (INT16 *)sound;

	i = 0;
	switch(freq)
	{
	case 44100: // already at the same rate? well that makes it simple.
		while(i++ < samples)
		{
			o = ((INT16)(*s++)+0x80)<<8; // changed signedness and shift up to 16 bits
			*d++ = o; // left channel
			*d++ = o; // right channel
		}
		break;
	case 22050: // unwrap 2x
		while(i++ < samples)
		{
			o = ((INT16)(*s++)+0x80)<<8; // changed signedness and shift up to 16 bits
			*d++ = o; // left channel
			*d++ = o; // right channel
			*d++ = o; // left channel
			*d++ = o; // right channel
		}
		break;
	case 11025: // unwrap 4x
		while(i++ < samples)
		{
			o = ((INT16)(*s++)+0x80)<<8; // changed signedness and shift up to 16 bits
			*d++ = o; // left channel
			*d++ = o; // right channel
			*d++ = o; // left channel
			*d++ = o; // right channel
			*d++ = o; // left channel
			*d++ = o; // right channel
			*d++ = o; // left channel
			*d++ = o; // right channel
		}
		break;
	default: // convert arbitrary hz to 44100.
		step = 0;
		frac = ((UINT32)freq << FRACBITS) / 44100 + 1; //Add 1 to counter truncation.
		while (i < samples)
		{
			o = (INT16)(*s+0x80)<<8; // changed signedness and shift up to 16 bits
			while (step < FRACUNIT) // this is as fast as I can make it.
			{
				*d++ = o; // left channel
				*d++ = o; // right channel
				step += frac;
			}
			do {
				i++; s++;
				step -= FRACUNIT;
			} while (step >= FRACUNIT);
		}
		break;
	}

	// return Mixer Chunk.
	return Mix_QuickLoad_RAW(sound, (Uint32)((UINT8*)d-sound));
}

void *I_GetSfx(sfxinfo_t *sfx)
{
	void *lump;
	Mix_Chunk *chunk;
	SDL_RWops *rw;
#ifdef HAVE_GME
	Music_Emu *emu;
	gme_info_t *info;
#endif

	if (sfx->lumpnum == LUMPERROR)
		sfx->lumpnum = S_GetSfxLumpNum(sfx);
	sfx->length = W_LumpLength(sfx->lumpnum);

	lump = W_CacheLumpNum(sfx->lumpnum, PU_SOUND);

	// convert from standard DoomSound format.
	chunk = ds2chunk(lump);
	if (chunk)
	{
		Z_Free(lump);
		return chunk;
	}

	// Not a doom sound? Try something else.
#ifdef HAVE_GME
	// VGZ format
	if (((UINT8 *)lump)[0] == 0x1F
		&& ((UINT8 *)lump)[1] == 0x8B)
	{
#ifdef HAVE_ZLIB
		UINT8 *inflatedData;
		size_t inflatedLen;
		z_stream stream;
		int zErr; // Somewhere to handle any error messages zlib tosses out

		memset(&stream, 0x00, sizeof (z_stream)); // Init zlib stream
		// Begin the inflation process
		inflatedLen = *(UINT32 *)lump + (sfx->length-4); // Last 4 bytes are the decompressed size, typically
		inflatedData = (UINT8 *)Z_Malloc(inflatedLen, PU_SOUND, NULL); // Make room for the decompressed data
		stream.total_in = stream.avail_in = sfx->length;
		stream.total_out = stream.avail_out = inflatedLen;
		stream.next_in = (UINT8 *)lump;
		stream.next_out = inflatedData;

		zErr = inflateInit2(&stream, 32 + MAX_WBITS);
		if (zErr == Z_OK) // We're good to go
		{
			zErr = inflate(&stream, Z_FINISH);
			if (zErr == Z_STREAM_END) {
				// Run GME on new data
				if (!gme_open_data(inflatedData, inflatedLen, &emu, cv_samplerate.value))
				{
					short *mem;
					UINT32 len;
					gme_equalizer_t eq = {GME_TREBLE, GME_BASS, 0,0,0,0,0,0,0,0};

					Z_Free(inflatedData); // GME supposedly makes a copy for itself, so we don't need this lying around
					Z_Free(lump); // We're done with the uninflated lump now, too.

					gme_start_track(emu, 0);
					gme_set_equalizer(emu, &eq);
					gme_track_info(emu, &info, 0);

					len = (info->play_length * 441 / 10) << 2;
					mem = Z_Malloc(len, PU_SOUND, NULL);
					gme_play(emu, len >> 1, mem);
					gme_free_info(info);
					gme_delete(emu);

					return Mix_QuickLoad_RAW((Uint8 *)mem, len);
				}
			}
			else
				CONS_Alert(CONS_ERROR,"Encountered %s when running inflate: %s\n", get_zlib_error(zErr), stream.msg);
			(void)inflateEnd(&stream);
		}
		else // Hold up, zlib's got a problem
			CONS_Alert(CONS_ERROR,"Encountered %s when running inflateInit: %s\n", get_zlib_error(zErr), stream.msg);
		Z_Free(inflatedData); // GME didn't open jack, but don't let that stop us from freeing this up
#else
		return NULL; // No zlib support
#endif
	}
	// Try to read it as a GME sound
	else if (!gme_open_data(lump, sfx->length, &emu, cv_samplerate.value))
	{
		short *mem;
		UINT32 len;
		gme_equalizer_t eq = {GME_TREBLE, GME_BASS, 0,0,0,0,0,0,0,0};

		Z_Free(lump);

		gme_start_track(emu, 0);
		gme_set_equalizer(emu, &eq);
		gme_track_info(emu, &info, 0);

		len = (info->play_length * 441 / 10) << 2;
		mem = Z_Malloc(len, PU_SOUND, NULL);
		gme_play(emu, len >> 1, mem);
		gme_free_info(info);
		gme_delete(emu);

		return Mix_QuickLoad_RAW((Uint8 *)mem, len);
	}
#endif

	// Try to load it as a WAVE or OGG using Mixer.
	rw = SDL_RWFromMem(lump, sfx->length);
	if (rw != NULL)
	{
		chunk = Mix_LoadWAV_RW(rw, 1);
		return chunk;
	}

	return NULL; // haven't been able to get anything
}

void I_FreeSfx(sfxinfo_t *sfx)
{
	if (sfx->data)
	{
		Mix_Chunk *chunk = (Mix_Chunk*)sfx->data;
		UINT8 *abufdata = NULL;

		if (chunk->allocated == 0)
		{
			// We allocated the data in this chunk, so get the abuf from mixer, then let it free the chunk, THEN we free the data
			// I believe this should ensure the sound is not playing when we free it
			abufdata = chunk->abuf;
		}

		Mix_FreeChunk(sfx->data);

		if (abufdata)
		{
			// I'm going to assume we used Z_Malloc to allocate this data.
			Z_Free(abufdata);
		}
	}
	sfx->data = NULL;
	sfx->lumpnum = LUMPERROR;
}

INT32 I_StartSound(sfxenum_t id, UINT8 vol, UINT8 sep, float speed, UINT8 pitch, UINT8 priority, INT32 channel)
{
	INT32 handle = Mix_PlayChannel(channel, S_sfx[id].data, 0);
	if (handle != -1)
	{
		Mix_Volume(handle, get_real_sfxvolume(vol));
		Mix_SetPanning(handle, min((UINT16)(0xff-sep)<<1, 0xff), min((UINT16)(sep)<<1, 0xff));
		I_SetSoundSpeed(handle, speed);
		Mix_RegisterEffect(handle, mix_speed_chunk, NULL, S_sfx[id].data);
		(void)pitch; // Mixer can't handle pitch
		(void)priority; // priority and channel management is handled by SRB2...
	}
	return handle;
}

void I_StopSound(INT32 handle)
{
	Mix_UnregisterEffect(handle, mix_speed_chunk);
	Mix_HaltChannel(handle);
}

boolean I_SoundIsPlaying(INT32 handle)
{
	if (!sound_started || handle < 0 || handle > AUDIO_NUM_CHANNELS)
	{
		// If you run this function while music is disabled,
		// or if you check for the wrong handle,
		// a crash could happen.
		return false;
	}

	return Mix_Playing(handle);
}

void I_UpdateSoundParams(INT32 handle, UINT8 vol, UINT8 sep, float speed, UINT8 pitch)
{
	if (!sound_started || handle < 0 || handle > AUDIO_NUM_CHANNELS)
	{
		// If you run this function while music is disabled,
		// or if you check for the wrong handle,
		// a crash could happen.
		return;
	}

	Mix_Volume(handle, get_real_sfxvolume(vol));
	Mix_SetPanning(handle, min((UINT16)(0xff-sep)<<1, 0xff), min((UINT16)(sep)<<1, 0xff));
	I_SetSoundSpeed(handle, speed);
	(void)pitch;
}

void I_SetSfxVolume(UINT8 volume)
{
	sfx_volume = volume;
}

void I_SetInternalSfxVolume(UINT8 volume)
{
	internal_sfx_volume = sfx_volume = volume;
}

boolean I_SetSoundSpeed(INT32 handle, float speed)
{
	INT32 channel;

	for (channel = 0; channel < cv_numChannels.value; channel++)
	{
		if (channels[channel].handle == handle)
		{
			channels[channel].speed = speed;
			return true;
		}
	}
	return false;
}

/// ------------------------
/// Music Utilities
/// ------------------------

static UINT32 get_real_volume(UINT8 volume)
{
	// convert volume to mixer's 128 scale
	// then apply 'internal_music_volume' as a percentage
	return ((UINT32)((codec_allows_songvolume ? volume : 31)*128/31)) * (UINT32)internal_music_volume / 100;
}

static UINT32 get_adjusted_position(UINT32 position)
{
	// all in milliseconds
	UINT32 length = I_GetSongLength();
	UINT32 looppoint = I_GetSongLoopPoint();
	if (length && position >= length)
		return (position % (length-looppoint));
	else
		return position;
}

static void do_fading_callback(void)
{
	I_lock_mutex(&i_mixer_fade);
	{
		fading_do_callback = true;
	}
	I_unlock_mutex(i_mixer_fade);
}

// In order for the best music support, these checks are necessary.
// Not all of the codecs support doing these things.
// Doing these checks allow us to save on code, and also future-proofs it.
//
// For instance, witih SDL-Mixer, only Modplug, OGGs, FLAC, MP3s, and MPGs support music position.
// Plus, with SDL-Mixer-X, WAV formats also support it.
static boolean music_validate_codec(boolean looping)
{
	boolean validated = false;
	is_looping = looping;

#ifdef HAVE_GME
	if (gme)
	{
		codec_allows_position = codec_allows_looppoints = codec_allows_songvolume = codec_allows_speeding = codec_allows_pitching = true;
		codec_max_speed = codec_max_pitch = 20.0f;
		validated = true;
	}
	else
#endif
#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		codec_allows_position = true;
		codec_allows_looppoints = false;
		codec_allows_songvolume = codec_allows_speeding = codec_allows_pitching = true;
		codec_max_speed = codec_max_pitch = 4.0f; // Limit this to 4x to prevent crashing, stupid fix but... ~SteelT 27/9/19
		validated = true;
	}
	else
#endif
	if (music)
	{
		Mix_VolumeMusic(get_real_volume(music_volume));
#if SDL_MIXER_VERSION_ATLEAST(2,6,0) || defined (HAVE_MIXERX)
		codec_allows_position = codec_allows_looppoints = (Mix_GetMusicPosition(music) > -1.0f);
#else
		codec_allows_position = (Mix_SetMusicPosition(0.0) > -1.0f);
		codec_allows_looppoints = false;
#endif
		codec_allows_songvolume = ((UINT32)Mix_VolumeMusic(-1) == get_real_volume(music_volume));
#ifdef HAVE_MIXERX
		codec_allows_speeding = (Mix_SetMusicTempo(music, 1.0) > -1.0
#if SDL_MIXER_VERSION_ATLEAST(2,6,0)
			|| Mix_SetMusicSpeed(music, 1.0) > -1.0
#endif
		);
		codec_allows_pitching = (Mix_SetMusicPitch(music, 1.0) > -1.0);
#else
		codec_allows_speeding = codec_allows_pitching = false;
#endif
		codec_max_speed = codec_max_pitch = 20.0f;
		validated = true;
	}

	if (validated)
	{
		int song_type = I_SongType();
		const char *music_type;
		switch (song_type)
		{
			case MU_WAV:
				music_type = "Wav";
				break;
			case MU_MOD:
				music_type = "Modplug";
				break;
			case MU_MID:
			case MU_MID_EX:
				music_type = "MIDI";
				break;
			case MU_OGG:
				music_type = "OGG";
				break;
			case MU_MP3:
				music_type = "MP3";
				break;
			case MU_FLAC:
				music_type = "FLAC";
				break;
			case MU_OPUS:
				music_type = "OPUS";
				break;
			case MU_WAVPACK:
				music_type = "Wavpack";
				break;
			case MU_GME:
				music_type = "GME";
				break;
			case MU_MOD_EX:
				music_type = "libOpenMPT";
				break;
			default:
				music_type = "Unknown";
				break;
		}
		CONS_Debug(DBG_DETAILED, "\n"
			"Music Validated!\n"
			"Samplerate - %lf\n"
			"Interal Samplerate - %lf\n"
			"Song type - %s (%d)\n"
			"\n"
			"Allows song positon - %d\n"
			"Allows song looppoints - %d\n"
			"Allows song volume - %d\n"
			"Allows song speeding - %d\n"
			"Allows song pitching - %d\n"
			"\n",
			(double)AUDIO_SAMPLERATE,
			(double)AUDIO_INTERNAL_SAMPLERATE,
			music_type, I_SongType(),
			codec_allows_position, codec_allows_looppoints, codec_allows_songvolume, codec_allows_speeding, codec_allows_pitching
		);
		return true;
	}

	CONS_Alert(CONS_ERROR, "I_PlaySong() - Failed to validate song!\n");
	I_UnloadSong();
	return false;
}

/// ------------------------
/// Music Hooks
/// ------------------------

static void count_music_bytes(int chan, void *stream, int len, void *udata)
{
	(void)chan;
	(void)stream;
	(void)udata;

	if (!music || songpaused || !codec_allows_position)
		return;
	if (I_SongType() == MU_GME || I_SongType() == MU_MOD) // don't bother counting if GME or MOD
		return;

	music_bytes += len;
}

static void music_loop(void)
{
	if (is_looping)
	{
		Mix_PlayMusic(music, 0);
		if (codec_allows_looppoints)
		{
			Mix_SetMusicPosition(loop_point);
#if SDL_MIXER_VERSION_ATLEAST(2,6,0) || defined (HAVE_MIXERX)
			if (Mix_GetMusicPosition(music) >= loop_point)
#endif
			{
				music_bytes = (UINT32)(loop_point*AUDIO_INTERNAL_SAMPLERATE*4); //assume internal audio samplerate, 4-byte length (see I_GetSongPosition)
				return;
			}
		}
		music_bytes = 0;
	}
	else
	{
		// HACK: Let fade timing proceed beyond the end of a
		// non-looping song. This is a specific case where the timing
		// should persist after stopping a song, so I don't believe
		// this should apply every time the user stops a song.
		// This is auto-unset in var_cleanup, called by I_StopSong
		fading_nocleanup = true;
		I_StopSong();
	}
}

static UINT32 music_fade(UINT32 interval, void *param)
{
	(void)param;

	if (!is_fading ||
		internal_music_volume == fading_target ||
		fading_duration == 0)
	{
		I_StopFadingSong();
		do_fading_callback();
		return 0;
	}
	else if (songpaused) // don't decrement timer
		return interval;
	else if ((fading_timer -= 10) <= 0)
	{
		internal_music_volume = fading_target;
		Mix_VolumeMusic(get_real_volume(music_volume));
		I_StopFadingSong();
		do_fading_callback();
		return 0;
	}
	else
	{
		UINT8 delta = abs(fading_target - fading_source);
		fixed_t factor = FixedDiv(fading_duration - fading_timer, fading_duration);
		if (fading_target < fading_source)
			internal_music_volume = max(min(internal_music_volume, fading_source - FixedMul(delta, factor)), fading_target);
		else if (fading_target > fading_source)
			internal_music_volume = min(max(internal_music_volume, fading_source + FixedMul(delta, factor)), fading_target);
		Mix_VolumeMusic(get_real_volume(music_volume));
		return interval;
	}
}

#ifdef HAVE_GME
static void mix_gme(void *udata, Uint8 *stream, int len)
{
	int i;
	short *p;

	(void)udata;

	// no gme? no music.
	if (!gme || gme_track_ended(gme) || songpaused)
		return;

	// play gme into stream
	gme_play(gme, len/2, (short *)stream);

	// Limiter to prevent music from being disorted with some formats
	if (music_volume >= 18)
		music_volume = 18;

	// apply volume to stream
	for (i = 0, p = (short *)stream; i < len / 2; i++, p++)
		*p = ((INT32)*p) * music_volume * internal_music_volume / 100 / 20;
}
#endif

#ifdef HAVE_OPENMPT
static void mix_openmpt(void *udata, Uint8 *stream, int len)
{
	int i;
	short *p;

	(void)udata;

	if (!openmpt_mhandle || songpaused)
		return;

	// Play module into stream
	openmpt_module_read_interleaved_stereo(openmpt_mhandle, (INT32)AUDIO_SAMPLERATE, (size_t)cv_buffersize.value, (short *)stream);

	// Limiter to prevent music from being disorted with some formats
	if (music_volume >= 18)
		music_volume = 18;

	// apply volume to stream
	for (i = 0, p = (short *)stream; i < len / 2; i++, p++)
		*p = ((INT32)*p) * music_volume * internal_music_volume / 100 / 20;
}
#endif

/// ------------------------
/// Music System
/// ------------------------

void I_InitMusic(void)
{
}

void I_ShutdownMusic(void)
{
	I_UnloadSong();
}

/// ------------------------
/// Music Properties
/// ------------------------

musictype_t I_SongType(void)
{
#ifdef HAVE_GME
	if (gme)
		return MU_GME;
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
		return MU_MOD_EX;
#endif

	if (!music)
		return MU_NONE;

	switch (Mix_GetMusicType(music))
	{
		case MUS_WAV:
			return MU_WAV;
		case MUS_MOD:
		case MUS_MODPLUG:
			return MU_MOD;
		case MUS_MID:
#ifdef HAVE_MIXERX
			if (Mix_GetMidiPlayer() == MIDI_Native)
				return MU_MID_EX;
#endif
			return MU_MID;
		case MUS_OGG:
			return MU_OGG;
		case MUS_MP3:
		case MUS_MP3_MAD:
			return MU_MP3;
		case MUS_FLAC:
			return MU_FLAC;
		case MUS_OPUS:
			return MU_OPUS;
#ifdef HAVE_MIXERX
		case MUS_WAVPACK:
			return MU_WAVPACK;
		case MUS_GME:
			return MU_GME;
#endif
		default:
			return MU_UNKNOWN;
	}
}

boolean I_SongLoaded(void)
{
#ifdef HAVE_GME
	if (I_SongType() == MU_GME && gme)
		return !gme_track_ended(gme);
#endif
#ifdef HAVE_OPENMPT
	if (I_SongType() == MU_MOD_EX && openmpt_mhandle)
		return true;
#endif
	return (music != NULL);
}

boolean I_SongPlaying(void)
{
	return (I_SongLoaded() && !songpaused);
}

boolean I_SongPaused(void)
{
	return (I_SongLoaded() && songpaused);
}

/// ------------------------
/// Music Effects
/// ------------------------

boolean I_SetSongSpeed(float speed)
{
	if (!codec_allows_speeding)
	{
		music_speed = 1.0f;
		return false;
	}

	if (speed > codec_max_speed)
	{
		CONS_Alert(CONS_WARNING, "I_SetSongSpeed(): Music speed for this format cannot be set above %1fx!\n", codec_max_speed);
		speed = codec_max_speed; // limit speed up to max codec amount
	}
	else if (speed < 0.1f)
	{
		CONS_Alert(CONS_WARNING, "I_SetSongSpeed(): Music speed cannot be set below 0.1x!\n");
		speed = 0.1f; // can't use a negative speed
	}

	if (FloatToFixed(speed) == FloatToFixed(music_speed))
	{
		// StarManiaKG: This prevents the music from noticibly speeding itself up/down
		// past the specified amount if this function is spammed.
		return true;
	}
	music_speed = speed;

#ifdef HAVE_GME
	if (gme)
	{
		SDL_LockAudio();
		gme_set_tempo(gme, music_speed);
		SDL_UnlockAudio();
		return true;
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
#if OPENMPT_API_VERSION_MAJOR < 1 && OPENMPT_API_VERSION_MINOR < 5
		{
			// deprecated in 0.5.0
			char modspd[13];
			sprintf(modspd, "%g", music_speed);
			openmpt_module_ctl_set(openmpt_mhandle, "play.tempo_factor", modspd);
		}
#else
		openmpt_module_ctl_set_floatingpoint(openmpt_mhandle, "play.tempo_factor", (double)music_speed);
#endif
		return true;
	}
#endif

#ifdef HAVE_MIXERX
	// StarManiaKG: a new speed system! (tons of modding options here!)
	if (music)
	{
		if (Mix_SetMusicTempo(music, (double)music_speed) > -1.0
#if SDL_MIXER_VERSION_ATLEAST(2,6,0)
			|| Mix_SetMusicSpeed(music, (double)music_speed) > -1.0
#endif
		)
		{
			return true;
		}
	}
#endif

	music_speed = 1.0f;
	return false;
}

float I_GetSongSpeed(void)
{
	return music_speed;
}

// StarManiaKG: a new pitching system too! (even more modding opportunities!) //
boolean I_SetSongPitch(float pitch)
{
	if (!codec_allows_pitching)
	{
		music_pitch = 1.0f;
		return false;
	}

	if (pitch > codec_max_pitch)
	{
		CONS_Alert(CONS_WARNING, "I_SetSongPitch(): Music pitch cannot be set above %1fx!\n", codec_max_pitch);
		pitch = codec_max_pitch; // limit pitch up to max codec amount
	}
	else if (pitch < 0.1f)
	{
		CONS_Alert(CONS_WARNING, "I_SetSongPitch(): Music pitch cannot be set below 0.1x!\n");
		pitch = 0.1f; // can't use a negative pitch
	}

	if (FloatToFixed(pitch) == FloatToFixed(music_pitch))
	{
		// StarManiaKG: This prevents the music from noticibly pitching itself up/down
		// past the specified amount if this function is spammed.
		return true;
	}
	music_pitch = pitch;

#ifdef HAVE_GME
	if (gme)
	{
		SDL_LockAudio();
		gme_set_stereo_depth(gme, music_pitch);
		SDL_UnlockAudio();
		return true;
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
#if OPENMPT_API_VERSION_MAJOR < 1 && OPENMPT_API_VERSION_MINOR < 5
		{
			// deprecated in 0.5.0
			char modspd[13];
			sprintf(modspd, "%g", music_pitch);
			openmpt_module_ctl_set(openmpt_mhandle, "play.pitch_factor", modspd);
		}
#else
		openmpt_module_ctl_set_floatingpoint(openmpt_mhandle, "play.pitch_factor", (double)music_pitch);
#endif
		return true;
	}
#endif

#ifdef HAVE_MIXERX
	if (music)
	{
		if (Mix_SetMusicPitch(music, (double)music_pitch) > -1.0)
		{
			return true;
		}
	}
#endif

	music_pitch = 1.0f;
	return false;
}

float I_GetSongPitch(void)
{
	return music_pitch;
}

/// ------------------------
///  MUSIC SEEKING
/// ------------------------

UINT32 I_GetSongLength(void)
{
	INT32 length;
	double xlength = -1.0;

#ifdef HAVE_GME
	if (gme)
	{
		gme_info_t *info;
		gme_err_t gme_e = gme_track_info(gme, &info, current_track);

		if (gme_e != NULL)
		{
			CONS_Alert(CONS_ERROR, "GME error: %s\n", gme_e);
			length = 0;
		}
		else
		{
			// reconstruct info->play_length, from GME source
			// we only want intro + 1 loop, not 2
			length = info->length;
			if (length <= 0)
			{
				length = info->intro_length + info->loop_length; // intro + 1 loop
				if (length <= 0)
					length = 150 * 1000; // 2.5 minutes
			}
		}

		gme_free_info(info);
		return max(length, 0);
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
		return (UINT32)(openmpt_module_get_duration_seconds(openmpt_mhandle) * 1000.0);
#endif

	if (!music)
		return 0;

#if SDL_MIXER_VERSION_ATLEAST(2,6,0)
	// As of SDL_Mixer 2.6.0, we can now just get duration using Mix_MusicDuration!
	xlength = Mix_MusicDuration(music);
#elif defined (HAVE_MIXERX)
	// This is the older version of getting music duration for SDL_Mixer_X.
	xlength = Mix_GetMusicTotalTime(music);
#endif

	if (xlength >= 0.0)
		return (UINT32)(xlength*1000);

	if (fpclassify(song_length) != FP_ZERO)
	{
		// Since the above methods failed, we need to move to the fallback-fallback method.
		// VERY IMPORTANT to set your LENGTHMS= in your song files, folks!
		// Could help you in the worst case scenarios.
		length = (UINT32)(song_length*1000);
		return length;
	}

	CONS_Debug(DBG_DETAILED, "Getting music length: This song is missing a LENGTHMS= tag! Required to make seeking work properly.\n");
	return 0;
}

boolean I_SetSongLoopPoint(UINT32 looppoint)
{
	if (!music || !codec_allows_looppoints || !is_looping)
	{
		return false;
	}
	else
	{
		const UINT32 length = I_GetSongLength();

		if (length > 0)
			looppoint %= length;

		loop_point = max((float)(looppoint / 1000.0L), 0);
		return true;
	}
}

UINT32 I_GetSongLoopPoint(void)
{
	if (!codec_allows_looppoints)
		return 0;

#ifdef HAVE_GME
	if (gme)
	{
		INT32 looppoint;
		gme_info_t *info;
		gme_err_t gme_e = gme_track_info(gme, &info, current_track);

		if (gme_e != NULL)
		{
			CONS_Alert(CONS_ERROR, "GME error: %s\n", gme_e);
			looppoint = 0;
		}
		else
			looppoint = info->intro_length > 0 ? info->intro_length : 0;

		gme_free_info(info);
		return max(looppoint, 0);
	}
#endif

	if (!music)
		return 0;
	return (UINT32)(loop_point * 1000);
}

boolean I_SetSongPosition(UINT32 position)
{
	UINT32 length;

	if (!codec_allows_position)
	{
		music_bytes = 0;
		return false;
	}

#ifdef HAVE_GME
	if (gme)
	{
		// this is unstable, so fail silently
		return true;

		// this isn't required technically, but GME thread-locks for a second
		// if you seek too high from the counter
		/*
		length = I_GetSongLength();
		if (length)
			position = get_adjusted_position(position);

		SDL_LockAudio();
		gme_err_t gme_e = gme_seek(gme, position);
		SDL_UnlockAudio();

		if (gme_e != NULL)
		{
			CONS_Alert(CONS_ERROR, "GME error: %s\n", gme_e);
			return false;
		}
		else
			return true;
		*/
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		// This isn't 100% correct because we don't account for loop points because we can't get them.
		// But if you seek past end of song, OpenMPT seeks to 0. So adjust the position anyway.
		openmpt_module_set_position_seconds(openmpt_mhandle, (double)(get_adjusted_position(position)/1000.0L)); // returns new position
		return true;
	}
#endif

	if (!music)
	{
		music_bytes = 0;
		return false;
	}

	if (I_SongType() == MU_MOD)
	{
		// Goes by channels
		return Mix_SetMusicPosition(position);
	}

	// In the offchance SDL mixer can't identify song length, if you have
	// a position input greater than the real length, then
	// music_bytes becomes inaccurate.

	length = I_GetSongLength(); // get it in MS
	if (length)
		position = get_adjusted_position(position);

	Mix_RewindMusic(); // needed for mp3
	Mix_SetMusicPosition((float)(position/1000.0L));
	if ((position/1000.0L) <= length)
	{
		// use internal audio samplerate, 4-byte length (see I_GetSongPosition)
		music_bytes = (UINT32)(position/1000.0L*AUDIO_INTERNAL_SAMPLERATE*4);
		return true;
	}

	// NOTE: This block fires on incorrect song format,
	// OR if position input is greater than song length.
	music_bytes = 0;
	return true;
}

UINT32 I_GetSongPosition(void)
{
	if (!codec_allows_position)
	{
		return false;
	}

#ifdef HAVE_GME
	if (gme)
	{
		INT32 position = gme_tell(gme);

		gme_info_t *info;
		gme_err_t gme_e = gme_track_info(gme, &info, current_track);

		if (gme_e != NULL)
		{
			CONS_Alert(CONS_ERROR, "GME error: %s\n", gme_e);
			return position;
		}
		else
		{
			// adjust position, since GME's counter keeps going past loop
			if (info->length > 0)
				position %= info->length;
			else if (info->intro_length + info->loop_length > 0)
				position = position >= (info->intro_length + info->loop_length) ? (position % info->loop_length) : position;
			else
				position %= 150 * 1000; // 2.5 minutes
		}

		gme_free_info(info);
		return max(position, 0);
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		// This will be incorrect if we adjust for length because we can't get loop points.
		// So return unadjusted. See note in SetMusicPosition: we adjust for that.
		return (UINT32)(openmpt_module_get_position_seconds(openmpt_mhandle)*1000.0);
		//return get_adjusted_position((UINT32)(openmpt_module_get_position_seconds(openmpt_mhandle)*1000.0));
	}
#endif

	if (!music)
		return 0;

#if SDL_MIXER_VERSION_ATLEAST(2,6,0) || defined (HAVE_MIXERX)
	double xposition = Mix_GetMusicPosition(music);
	if (xposition >= 0.0)
		return (UINT32)(xposition*1000);
#endif

	return (UINT32)(music_bytes/AUDIO_INTERNAL_SAMPLERATE*1000.0L/4); // use internal audio samplerate
	// 4 = byte length for 16-bit samples (AUDIO_S16SYS), stereo (2-channel)
	// This is hardcoded in I_StartupSound. Other formats for factor:
	// 8M: 1 | 8S: 2 | 16M: 2 | 16S: 4
}

/// ------------------------
/// Music Playback
/// ------------------------

boolean I_LoadSong(char *data, size_t len)
{
	static const char *key1 = "LOOP";
	static const char *key2 = "POINT=";
	static const char *key3 = "MS=";
	static const char *key4 = "LENGTHMS=";
	static const char *key5 = "SAMPLERATE=";
	const size_t key1len = strlen(key1);
	const size_t key2len = strlen(key2);
	const size_t key3len = strlen(key3);
	const size_t key4len = strlen(key4);
	const size_t key5len = strlen(key5);
	boolean looppoint_tiedto_samplerate;
	char *p = data;
	SDL_RWops *rw;

	// always run these whether or not music is playing
	I_UnloadSong();
	var_cleanup();

#ifdef HAVE_GME
	if ((UINT8)data[0] == 0x1F
		&& (UINT8)data[1] == 0x8B)
	{
#ifdef HAVE_ZLIB
		UINT8 *inflatedData;
		size_t inflatedLen;
		z_stream stream;
		int zErr; // Somewhere to handle any error messages zlib tosses out

		memset(&stream, 0x00, sizeof (z_stream)); // Init zlib stream
		// Begin the inflation process
		inflatedLen = *(UINT32 *)(data + (len-4)); // Last 4 bytes are the decompressed size, typically
		inflatedData = (UINT8 *)Z_Calloc(inflatedLen, PU_MUSIC, NULL); // Make room for the decompressed data
		stream.total_in = stream.avail_in = len;
		stream.total_out = stream.avail_out = inflatedLen;
		stream.next_in = (UINT8 *)data;
		stream.next_out = inflatedData;

		zErr = inflateInit2(&stream, 32 + MAX_WBITS);
		if (zErr == Z_OK) // We're good to go
		{
			zErr = inflate(&stream, Z_FINISH);
			if (zErr == Z_STREAM_END)
			{
				// Run GME on new data
				if (!gme_open_data(inflatedData, inflatedLen, &gme, cv_samplerate.value))
				{
					Z_Free(inflatedData); // GME supposedly makes a copy for itself, so we don't need this lying around
					return true;
				}
			}
			else
				CONS_Alert(CONS_ERROR, "Encountered %s when running inflate: %s\n", get_zlib_error(zErr), stream.msg);
			(void)inflateEnd(&stream);
		}
		else // Hold up, zlib's got a problem
			CONS_Alert(CONS_ERROR, "Encountered %s when running inflateInit: %s\n", get_zlib_error(zErr), stream.msg);
		Z_Free(inflatedData); // GME didn't open jack, but don't let that stop us from freeing this up
		return false;
#else
		CONS_Alert(CONS_ERROR, "Cannot decompress VGZ; no zlib support\n");
		return false;
#endif
	}
	else if (!gme_open_data(data, len, &gme, cv_samplerate.value))
		return true;
#endif

#ifdef HAVE_OPENMPT
	/*
		If the size of the data to be checked is bigger than the recommended size (> 2048 bytes)
		Let's just set the probe size to the recommended size
		Otherwise let's give it the full data size
	*/

	if (len > openmpt_probe_file_header_get_recommended_size())
		probesize = openmpt_probe_file_header_get_recommended_size();
	else
		probesize = len;

	result = openmpt_probe_file_header(OPENMPT_PROBE_FILE_HEADER_FLAGS_DEFAULT, data, probesize, len, NULL, NULL, NULL, NULL, NULL, NULL);

	if (result == OPENMPT_PROBE_FILE_HEADER_RESULT_SUCCESS) // We only cared if it succeeded, continue on if not.
	{
		openmpt_mhandle = openmpt_module_create_from_memory2(data, len, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if (!openmpt_mhandle) // Failed to create module handle? Show error and return!
		{
			mod_err = openmpt_module_error_get_last(openmpt_mhandle);
			mod_err_str = openmpt_error_string(mod_err);
			CONS_Alert(CONS_ERROR, "openmpt_module_create_from_memory2: %s\n", mod_err_str);
			return false;
		}
		else
			return true; // All good and we're ready for music playback!
	}
#endif

#ifdef HAVE_MIXERX
	if (Mix_GetMidiPlayer() != cv_midiplayer.value)
		Mix_SetMidiPlayer(cv_midiplayer.value);
	if (!Mix_GetSoundFonts() || stricmp(Mix_GetSoundFonts(), cv_midisoundfontpath.string))
		Mix_SetSoundFonts(cv_midisoundfontpath.string);
	if (cv_midiplayer.value == MIDI_Timidity && !timidity_setcfg(cv_miditimiditypath.string))
	{
		CONS_Alert(CONS_WARNING, "Couldn't use specified Timidity cfg, trying default.\n");
		if (!timidity_setcfg(cv_miditimiditypath.defaultvalue))
		{
			// StarManiaKG:
			// If we can't find ANY cfg to use, then we just need to quit.
			// This prevents a Timidity-related SDL_Mixer crash from occuring when loading music.
			return false;
		}
	}
#endif

	// StarManiaKG: Just so you know, the block of code below now features a fix.
	// See here: https://stackoverflow.com/questions/73097254/sdl-rwread-and-sdl2-mixer-loading-entire-file-into-memory.
	//
	// Essentially, we now get the music data, read all of it, and verify it again.
	// We then keep the data in memory until the song has been stopped and unloaded.
	// This fixes a bug where some audio files just straight up crash the game.
	// From my experience, it can be seen mostly with OGG files.
	// And before you ask, no, threading this did not work.
	//
	// This is all a hack, but it's better than nothing.
	// We really need Ring Racers' audio subsystem... (or at least, I need to finish working on my own...)

	// Let's see if Mixer is able to load this.
	rw = SDL_RWFromConstMem(data, len);
	if (rw != NULL)
	{
		size_t nb_read_total = 0, nb_read = 1;
		char *buf = NULL;

		music_memory = Z_Malloc(len + 1, PU_MUSIC, NULL);
		if (music_memory != NULL)
		{
			buf = music_memory;

			while (nb_read_total < len && nb_read != 0)
			{
				nb_read = SDL_RWread(rw, buf, 1, (len - nb_read_total));
				nb_read_total += nb_read;
				buf += nb_read;
			}

			if (nb_read_total >= len)
			{
				// NOW we can actually load the music!
				music_memory[nb_read_total] = '\0';
				rw = SDL_RWFromConstMem(music_memory, len);
				music = Mix_LoadMUS_RW(rw, 1);
			}
		}
	}
	if (!music)
	{
		CONS_Alert(CONS_ERROR, "Mix_LoadMUS_RW: %s\n", Mix_GetError());
		music_cleanup();
		SDL_RWclose(rw);
		return false;
	}

	// Find the music loop point and sample rate.
	looppoint_tiedto_samplerate = false;
	loop_point = 0.0f;
	song_length = 0.0f;
	AUDIO_INTERNAL_SAMPLERATE = 0.0L;

	while ((UINT32)(p - data) < len)
	{
		if (fpclassify(loop_point) == FP_ZERO && !strncmp(p, key1, key1len))
		{
			p += key1len; // skip LOOP
			if (!strncmp(p, key2, key2len) || *p == '=') // is it LOOPPOINT=? (or even just LOOP=?)
			{
				p += (!strncmp(p, key2, key2len) ? key2len : 1); // skip POINT= , or just =
				loop_point = atoi(p);
				looppoint_tiedto_samplerate = true;
				// LOOPPOINT (or LOOP) work by sample count.
			}
			else if (!strncmp(p, key3, key3len)) // is it LOOPMS=?
			{
				p += key3len; // skip MS=
				loop_point = (float)(atoi(p) / 1000.0L); // LOOPMS works in real time, as miliseconds.
				// Everything that uses LOOPMS will work perfectly with SDL_Mixer.
			}
			p++;
			continue;
		}

		if (fpclassify(song_length) == FP_ZERO && !strncmp(p, key4, key4len))
		{
			p += key4len; // skip LENGTHMS=
			song_length = (float)(atoi(p) / 1000.0L); // LENGTHMS works in real time, as miliseconds.
			p++;
			continue;
		}

		if (fpclassify(AUDIO_INTERNAL_SAMPLERATE) == FP_ZERO && !strncmp(p, key5, key5len))
		{
			p += key5len; // skip SAMPLERATE=
			AUDIO_INTERNAL_SAMPLERATE = (long double)atof(p);
			p++;
			continue;
			// SDL_Mixer also doesn't have a way to get music frequency.
			// And I don't feel like making a whole audio subsystem, like in Ring Racers, just to do so.
			// So, this is why we assume all music equals whatever our initializing sample rate value is.
			// This *should* save our looping system!
		}

		if (fpclassify(loop_point) != FP_ZERO && fpclassify(song_length) != FP_ZERO && fpclassify(AUDIO_INTERNAL_SAMPLERATE) != FP_ZERO)
			break; // Got what we needed
		else
			p++; // continue searching
	}

	if (fpclassify(AUDIO_INTERNAL_SAMPLERATE) == FP_ZERO)
	{
		// SDL_Mixer also doesn't have a way to get music frequency.
		// Most of our audio is meant to run at 44100Hz, so we set that by default.
		// And we don't have a custom audio codec implementation that could handle this like Ring Racers does. (Thankfully I'm working on one!)
		// This is why we assume all music equals whatever our initializing sample rate value is.
		// This *should* save our looping system!
		AUDIO_INTERNAL_SAMPLERATE = 44100.L;
	}

	if (looppoint_tiedto_samplerate)
	{
		loop_point = (float)(((AUDIO_INTERNAL_SAMPLERATE / 1000.0L) + loop_point) / AUDIO_INTERNAL_SAMPLERATE);
		// Because SDL_Mixer (and SDL_Mixer_X by Xtension) is USELESS and can't even tell us
		// something simple like the frequency of the streaming music,
		// we are unfortunately forced to assume that ALL MUSIC is whatever our internal samplerate happens to be. (usually 44100hz)
		// This means a lot of tracks that are only 22050hz for a reasonable downloadable file size will loop VERY badly.
		// We also set this down here because the sample rate value can be modified by the music comments.
	}

	return true;
}

void I_UnloadSong(void)
{
	I_StopSong();
	music_cleanup();
}

boolean I_PlaySong(boolean looping)
{
#ifdef HAVE_GME
	if (gme)
	{
		gme_equalizer_t eq = {GME_TREBLE, GME_BASS, 0,0,0,0,0,0,0,0};
#if defined (GME_VERSION) && GME_VERSION >= 0x000603
		if (looping)
		{
			gme_set_autoload_playback_limit(gme, 0);
		}
#endif
		gme_set_equalizer(gme, &eq);
		gme_start_track(gme, 0);
		current_track = 0;
		Mix_HookMusic(mix_gme, gme);
		return music_validate_codec(looping);
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		openmpt_module_select_subsong(openmpt_mhandle, 0);
		openmpt_module_set_render_param(openmpt_mhandle, OPENMPT_MODULE_RENDER_INTERPOLATIONFILTER_LENGTH, cv_modfilter.value);
		if (looping)
		{
			openmpt_module_set_repeat_count(openmpt_mhandle, -1); // Always repeat
		}
		current_subsong = 0;
		Mix_HookMusic(mix_openmpt, openmpt_mhandle);
		return music_validate_codec(looping);
	}
#endif

	if (!music)
		return false;

	if (fpclassify(song_length) == FP_ZERO)
		CONS_Debug(DBG_DETAILED, "This song is missing a LENGTHMS= tag! Optional, but allows seeking to work properly on earlier SDL_Mixer versions.\n");

#if SDL_MIXER_VERSION_ATLEAST(2,6,0) || defined (HAVE_MIXERX)
	if (Mix_PlayMusic(music, ((looping && Mix_GetMusicPosition(music) <= -1.0f) ? -1 : 0)) == -1)
	{
		CONS_Alert(CONS_ERROR, "Mix_PlayMusic: %s\n", Mix_GetError());
		return false;
	}
#else
	if (I_SongType() != MU_MOD && I_SongType() != MU_MID && Mix_PlayMusic(music, 0) == -1)
	{
		CONS_Alert(CONS_ERROR, "Mix_PlayMusic: %s\n", Mix_GetError());
		return false;
	}
	else if ((I_SongType() == MU_MOD || I_SongType() == MU_MID || I_SongType() == MU_MID_EX) && Mix_PlayMusic(music, looping ? -1 : 0) == -1) // if MOD, loop forever
	{
		CONS_Alert(CONS_ERROR, "Mix_PlayMusic: %s\n", Mix_GetError());
		return false;
	}
#endif

	Mix_HookMusicFinished(music_loop);
	if (I_SongType() != MU_MOD && I_SongType() != MU_MID && I_SongType() != MU_MID_EX)
	{
		if (!Mix_RegisterEffect(MIX_CHANNEL_POST, count_music_bytes, NULL, NULL))
			CONS_Alert(CONS_WARNING, "Error registering SDL music position counter: %s\n", Mix_GetError());
	}
	return music_validate_codec(looping);
}

void I_StopSong(void)
{
	// HACK: See music_loop on why we want fade timing to proceed
	// after end of song
	if (!fading_nocleanup)
		I_StopFadingSong();

#ifdef HAVE_GME
	if (gme)
	{
		Mix_HookMusic(NULL, NULL);
		current_track = -1;
	}
#endif
#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		Mix_HookMusic(NULL, NULL);
		current_subsong = -1;
	}
#endif
	if (music)
	{
		Mix_UnregisterEffect(MIX_CHANNEL_POST, count_music_bytes);
		Mix_HookMusicFinished(NULL);
		Mix_HaltMusic();
	}

	var_cleanup();
}

void I_PauseSong(void)
{
	if (!I_SongPlaying() || Mix_PausedMusic())
		return;

	Mix_PauseMusic();
	songpaused = Mix_PausedMusic();
}

void I_ResumeSong(void)
{
	if (!I_SongLoaded() || !Mix_PausedMusic() || !songpaused)
		return;

	Mix_ResumeMusic();
	songpaused = Mix_PausedMusic();
}

void I_SetMusicVolume(UINT8 volume)
{
	if (!I_SongLoaded())
		return;

	music_volume = volume;
	Mix_VolumeMusic(get_real_volume(codec_allows_songvolume ? music_volume : 31));
}

// Checks if the specified track is within the number of tracks playing, changes it if so.
boolean I_SetSongTrack(INT32 track)
{
#ifdef HAVE_GME
	if (gme)
	{
		if (current_track == track)
			return false;

		SDL_LockAudio();
		if (track >= 0 && track < gme_track_count(gme)-1)
		{
			gme_err_t gme_e = gme_start_track(gme, track);
			if (gme_e != NULL)
			{
				CONS_Alert(CONS_ERROR, "GME error: %s\n", gme_e);
			}
			else
			{
				current_track = track;
			}
		}
		SDL_UnlockAudio();

		return (current_track == track);
	}
#endif

#ifdef HAVE_OPENMPT
	if (openmpt_mhandle)
	{
		if (current_subsong == track)
			return false;

		SDL_LockAudio();
		if (track >= 0 && track < openmpt_module_get_num_subsongs(openmpt_mhandle))
		{
			openmpt_module_select_subsong(openmpt_mhandle, track);
			current_subsong = track;
		}
		SDL_UnlockAudio();

		return (current_subsong == track);
	}
#endif

	if (I_SongType() == MU_MOD)
		return !Mix_SetMusicPosition(track);
	return false;
}

/// ------------------------
/// MUSIC FADING
/// ------------------------

void I_SetInternalMusicVolume(UINT8 volume)
{
	internal_music_volume = volume;
	if (I_SongLoaded())
		Mix_VolumeMusic(get_real_volume(music_volume));
}

void I_StopFadingSong(void)
{
	if (fading_id)
		SDL_RemoveTimer(fading_id);
	is_fading = false;
	fading_source = fading_target = fading_timer = fading_duration = fading_id = 0;
	// don't unset fading_nocleanup here just yet; fading_callback is cleaned up
	// in var_cleanup()
}

boolean I_FadeSongFromVolume(UINT8 target_volume, UINT8 source_volume, UINT32 ms, void (*callback)(void))
{
	INT16 volume_delta;

	source_volume = min(source_volume, 100);
	volume_delta = (INT16)(target_volume - source_volume);

	I_StopFadingSong();

	if (!ms && volume_delta)
	{
		I_SetInternalMusicVolume(target_volume);
		if (callback)
			(*callback)();
		return true;

	}
	else if (!volume_delta)
	{
		if (callback)
			(*callback)();
		return true;
	}

	// Round MS to nearest 10
	// If n - lower > higher - n, then round up
	ms = (ms - ((ms / 10) * 10) > (((ms / 10) * 10) + 10) - ms) ?
		(((ms / 10) * 10) + 10) // higher
		: ((ms / 10) * 10); // lower

	if (!ms)
		I_SetInternalMusicVolume(target_volume);
	else if (source_volume != target_volume)
	{
		fading_id = SDL_AddTimer(10, music_fade, NULL);
		if (fading_id)
		{
			is_fading = true;
			fading_timer = fading_duration = ms;
			fading_source = source_volume;
			fading_target = target_volume;
			fading_callback = callback;

			if (internal_music_volume != source_volume)
				I_SetInternalMusicVolume(source_volume);
		}
	}

	return is_fading;
}

boolean I_FadeSong(UINT8 target_volume, UINT32 ms, void (*callback)(void))
{
	return I_FadeSongFromVolume(target_volume, internal_music_volume, ms, callback);
}

boolean I_FadeOutStopSong(UINT32 ms)
{
	return I_FadeSongFromVolume(0, internal_music_volume, ms, &I_StopSong);
}

boolean I_FadeInPlaySong(UINT32 ms, boolean looping)
{
	if (I_PlaySong(looping))
		return I_FadeSongFromVolume(100, 0, ms, NULL);
	else
		return false;
}

#endif
