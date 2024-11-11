// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-jukebox.h
/// \brief Globalizes TSoURDt3rd's unique sound library

#ifndef __SMKG_JUKEBOX__
#define __SMKG_JUKEBOX__

#include "../s_sound.h"

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------ //
//        Variables
// ------------------------ //

// PAGES
#define TSOURDT3RD_MAX_JUKEBOX_PAGES 64

typedef struct tsourdt3rd_jukebox_pages_s {
	INT32                              id;
	char                               page_name[38];
	struct tsourdt3rd_jukebox_pages_s *prev;
	struct tsourdt3rd_jukebox_pages_s *next;
} tsourdt3rd_jukebox_pages_t;

extern tsourdt3rd_jukebox_pages_t   tsourdt3rd_jukeboxpage_mainpage;
extern tsourdt3rd_jukebox_pages_t  *tsourdt3rd_jukeboxpages_start;
extern tsourdt3rd_jukebox_pages_t **tsourdt3rd_jukebox_available_pages;
extern INT32 tsourdt3rd_jukebox_numpages;

// DEFS
typedef struct tsourdt3rd_jukeboxdef_s {
	musicdef_t                     *linked_musicdef;
	tsourdt3rd_jukebox_pages_t     *supported_pages;
	struct tsourdt3rd_jukeboxdef_s *next;
} tsourdt3rd_jukeboxdef_t;

extern tsourdt3rd_jukeboxdef_t   jukebox_def_soundtestsfx;
extern tsourdt3rd_jukeboxdef_t  *jukebox_def_start; // Pointer to the initial jukebox def
extern tsourdt3rd_jukeboxdef_t **tsourdt3rd_jukebox_defs; // Jukebox definition pointer that can point towards any defintion desired, if valid.

// MAIN
typedef struct tsourdt3rd_jukebox_s {
	boolean     unlocked; // General data
	boolean     playing;

	boolean     hud_initialized; // HUD data
	INT32       hud_box_w;
	INT32       hud_string_w;
	INT32       hud_track_w;
	INT32       hud_speed_w;

	fixed_t     track_tics; // Track data
	musicdef_t *curtrack;
	musicdef_t *prevtrack;
} tsourdt3rd_jukebox_t;

extern tsourdt3rd_jukebox_t *tsourdt3rd_global_jukebox;

// ------------------------ //
//        Functions
// ------------------------ //

boolean TSoURDt3rd_Jukebox_Unlocked(void);

boolean TSoURDt3rd_Jukebox_PrepareDefs(void);

void TSoURDt3rd_Jukebox_Reset(void);
void TSoURDt3rd_Jukebox_RefreshLevelMusic(void);

void TSoURDt3rd_Jukebox_ST_drawJukebox(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __SMKG_JUKEBOX__
