#include "../i_sound.h"

boolean sound_started = false;

void *I_GetSfx(sfxinfo_t *sfx)
{
        (void)sfx;
        return NULL;
}

void I_FreeSfx(sfxinfo_t *sfx)
{
        (void)sfx;
}

void I_StartupSound(void){}

void I_ShutdownSound(void){}

//
//  SFX I/O
//

INT32 I_StartSound(sfxenum_t id, INT32 vol, INT32 sep, float speed, INT32 pitch, INT32 priority, INT32 channel)
{
        (void)id;
        (void)vol;
        (void)sep;
        (void)speed;
        (void)pitch;
        (void)priority;
        (void)channel;
        return -1;
}

void I_StopSound(INT32 handle)
{
        (void)handle;
}

INT32 I_SoundIsPlaying(INT32 handle)
{
        (void)handle;
        return false;
}

void I_UpdateSoundParams(INT32 handle, INT32 vol, INT32 sep, float speed, UINT8 pitch)
{
        (void)handle;
        (void)vol;
        (void)sep;
        (void)speed;
        (void)pitch;
}

void I_SetSfxVolume(INT32 volume)
{
        (void)volume;
}

boolean I_SetSoundSpeed(INT32 handle, float speed)
{
        (void)handle;
        (void)speed;
        return false;
}

/// ------------------------
//  MUSIC SYSTEM
/// ------------------------

UINT8 music_started = 0;
UINT8 digmusic_started = 0;

void I_InitMusic(void){}

void I_ShutdownMusic(void){}

/// ------------------------
//  MUSIC PROPERTIES
/// ------------------------

musictype_t I_SongType(void)
{
	return MU_NONE;
}

boolean I_SongLoaded(void)
{
        return false;
}

boolean I_SongPlaying(void)
{
	return false;
}

boolean I_SongPaused(void)
{
	return false;
}

/// ------------------------
//  MUSIC EFFECTS
/// ------------------------

boolean I_SetSongSpeed(float speed)
{
	(void)speed;
	return false;
}

float I_GetSongSpeed(void)
{
        return 1.0f;
}

boolean I_SetSongPitch(float pitch)
{
        (void)pitch;
	return false;
}

float I_GetSongPitch(void)
{
        return 1.0f;
}

/// ------------------------
//  MUSIC SEEKING
/// ------------------------

UINT32 I_GetSongLength(void)
{
        return 0;
}

boolean I_SetSongLoopPoint(UINT32 looppoint)
{
        (void)looppoint;
        return false;
}

UINT32 I_GetSongLoopPoint(void)
{
	return 0;
}

boolean I_SetSongPosition(UINT32 position)
{
        (void)position;
        return false;
}

UINT32 I_GetSongPosition(void)
{
        return 0;
}

/// ------------------------
//  MUSIC PLAYBACK
/// ------------------------

UINT8 midimusic_started = 0;

boolean I_LoadSong(char *data, size_t len)
{
        (void)data;
        (void)len;
        return -1;
}

void I_UnloadSong()
{

}

boolean I_PlaySong(boolean looping)
{
        (void)looping;
        return false;
}

void I_StopSong(void)
{
        
}

void I_PauseSong(void)
{

}

void I_ResumeSong(void)
{

}

void I_SetMusicVolume(INT32 volume)
{
        (void)volume;
}

/// ------------------------
//  MUSIC FADING
/// ------------------------

void I_SetInternalMusicVolume(UINT8 volume)
{
	(void)volume;
}

void I_StopFadingSong(void)
{
}

boolean I_FadeSongFromVolume(UINT8 target_volume, UINT8 source_volume, UINT32 ms, void (*callback)(void))
{
	(void)target_volume;
	(void)source_volume;
	(void)ms;
        return false;
}

boolean I_FadeSong(UINT8 target_volume, UINT32 ms, void (*callback)(void))
{
	(void)target_volume;
	(void)ms;
	return false;
}

boolean I_FadeOutStopSong(UINT32 ms)
{
        (void)ms;
        return false;
}

boolean I_FadeInPlaySong(UINT32 ms, boolean looping)
{
        (void)ms;
        (void)looping;
        return false;
}
