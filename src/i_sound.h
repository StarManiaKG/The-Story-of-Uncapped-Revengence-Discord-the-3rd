// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2025 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  i_sound.h
/// \brief System interface, sound, music

#ifndef __I_SOUND__
#define __I_SOUND__

#include "doomdef.h"
#include "sounds.h"
#include "command.h"

// mostly copied from SDL mixer, plus GME and wavpack
typedef enum {
	MU_NONE,
	MU_WAV,
	MU_MOD,
	MU_MID,
	MU_OGG,
	MU_MP3,
	MU_FLAC,
	MU_OPUS,
	MU_WAVPACK,
	MU_GME,
	MU_MOD_EX, // libopenmpt
	MU_MID_EX, // Non-native MIDI
	MU_UNKNOWN
} musictype_t;

/**	\brief Sound subsystem running and waiting
*/
extern boolean sound_started;

/**	\brief random sound pitching
*/
//extern consvar_t cv_rndsoundpitch;

/**	\brief info of samplerate
*/
extern consvar_t cv_samplerate;

/**	\brief info of buffer size
*/
extern consvar_t cv_buffersize;

/**	\brief	The I_GetSfx function

	\param	sfx	sfx to setup

	\return	data for sfx
*/
void *I_GetSfx(sfxinfo_t *sfx);

/**	\brief	The I_FreeSfx function

	\param	sfx	sfx to be freed up

	\return	void
*/
void I_FreeSfx(sfxinfo_t *sfx);

/**	\brief Init at program start...
*/
void I_StartupSound(void);

/**	\brief ... shut down and relase at program termination.
*/
void I_ShutdownSound(void);

/// ------------------------
///  SFX I/O
/// ------------------------

/**	\brief	Starts a sound in a particular sound channel.
	\param	id	sfxid
	\param	vol	volume for sound
	\param	sep	left-right balancle
	\param  speed playback speed for sound
	\param	pitch	not used
	\param	priority	not used

	\return	sfx handle
*/
INT32 I_StartSound(sfxenum_t id, UINT8 vol, UINT8 sep, float speed, UINT8 pitch, UINT8 priority, INT32 channel);

/**	\brief	Stops a sound channel.

	\param	handle	stop sfx handle

	\return	void
*/
void I_StopSound(INT32 handle);

/**	\brief Some digital sound drivers need this.
*/
void I_UpdateSound(void);

/**	\brief	Called by S_*() functions to see if a channel is still playing.

	\param	handle	sfx handle

	\return	0 if no longer playing, 1 if playing.
*/
boolean I_SoundIsPlaying(INT32 handle);

/**	\brief	Updates the sfx handle

	\param	handle	sfx handle
	\param	vol	volume
	\param	sep	separation
	\param  speed playback speed
	\param	pitch playback pitch

	\return	void
*/
void I_UpdateSoundParams(INT32 handle, UINT8 vol, UINT8 sep, float speed, UINT8 pitch);

/**	\brief	The I_SetSfxVolume function

	\param	volume	volume to set at

	\return	void
*/
void I_SetSfxVolume(UINT8 volume);

/**	\brief	The I_SetInternalSfxVolume function

	\param	volume	internal volume to play sounds

	\return	void
*/
void I_SetInternalSfxVolume(UINT8 volume);

/**	\brief	The I_SetSoundSpeed function

	\param	channel	sfx handle to use
	\param	speed	playback speed to set at

	\return	void
*/
boolean I_SetSoundSpeed(INT32 handle, float speed);

/// ------------------------
//  MUSIC SYSTEM
/// ------------------------

/** \brief Init the music systems
*/
void I_InitMusic(void);

/** \brief Shutdown the music systems
*/
void I_ShutdownMusic(void);

/// ------------------------
//  MUSIC PROPERTIES
/// ------------------------

musictype_t I_SongType(void);
boolean I_SongLoaded(void);
boolean I_SongPlaying(void);
boolean I_SongPaused(void);

/// ------------------------
//  MUSIC EFFECTS
/// ------------------------

boolean I_SetSongSpeed(float speed);
float I_GetSongSpeed(void);

boolean I_SetSongPitch(float pitch);
float I_GetSongPitch(void);

/// ------------------------
//  MUSIC SEEKING
/// ------------------------

UINT32 I_GetSongLength(void);

boolean I_SetSongLoopPoint(UINT32 looppoint);
UINT32 I_GetSongLoopPoint(void);

boolean I_SetSongPosition(UINT32 position);
UINT32 I_GetSongPosition(void);

/// ------------------------
//  MUSIC PLAYBACK
/// ------------------------

/**	\brief	Registers a song handle to song data.

	\param	data	pointer to song data
	\param	len	len of data

	\return	song handle

	\todo Remove this
*/
boolean I_LoadSong(char *data, size_t len);

/**	\brief	See ::I_LoadSong, then think backwards

	\sa I_LoadSong
*/
void I_UnloadSong(void);

/**	\brief	Called by anything that wishes to start music

	\param	looping	loop the song if true

	\return	if true, it's playing the song
*/
boolean I_PlaySong(boolean looping);

/**	\brief	Stops a song over 3 seconds
*/
void I_StopSong(void);

/**	\brief	PAUSE game handling.
*/
void I_PauseSong(void);

/**	\brief	RESUME game handling
*/
void I_ResumeSong(void);

/**	\brief	Set music volume.

	\param	volume	volume to set at

	\return	void
*/
void I_SetMusicVolume(UINT8 volume);

/**	\brief	Set music track.

	\param	track	music track to play

	\return	void
*/
boolean I_SetSongTrack(INT32 track);

/// ------------------------
/// MUSIC FADING
/// ------------------------

void I_SetInternalMusicVolume(UINT8 volume);
void I_StopFadingSong(void);
boolean I_FadeSongFromVolume(UINT8 target_volume, UINT8 source_volume, UINT32 ms, void (*callback)(void));
boolean I_FadeSong(UINT8 target_volume, UINT32 ms, void (*callback)(void));
boolean I_FadeOutStopSong(UINT32 ms);
boolean I_FadeInPlaySong(UINT32 ms, boolean looping);

#endif
