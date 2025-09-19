// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 2011-2016 by Matthew "Kaito Sinclaire" Walsh.
// Copyright (C) 1999-2025 by Sonic Team Junior.
// Copyright (C) 2025 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  m_menudefs.h
/// \brief A collection of all our menu definitions.

#ifndef __X_MENU_DEFS__
#define __X_MENU_DEFS__

#ifdef HAVE_DISCORDSUPPORT
#include "discord/discord.h"
#endif

//
// MENUDEFS
//

// the haxor message menu
extern menu_t MessageDef;

extern menu_t MainDef;
extern menuitem_t MainMenu[];

extern menu_t SR_MainDef;
extern menu_t SR_UnlockChecklistDef;

extern menuitem_t SP_MainMenu[];
extern menu_t SP_MainDef;

extern menu_t SP_LevelStatsDef;

extern menu_t SP_TimeAttackDef, SP_ReplayDef, SP_GuestReplayDef, SP_GhostDef;

extern menu_t SP_NightsAttackDef, SP_NightsReplayDef, SP_NightsGuestReplayDef, SP_NightsGhostDef;

extern menu_t SP_MarathonDef;

extern menu_t SP_LoadDef;

extern menuitem_t SPauseMenu[];
extern menu_t SPauseDef;
typedef enum
{
	spause_addons = 0,

	spause_pandora,
	spause_hints,
	spause_levelselect,

	spause_continue,
	spause_retry,
	spause_options,

	spause_title,
	spause_quit
} spause_e;

extern menuitem_t MPauseMenu[];
extern menu_t MPauseDef;
typedef enum
{
	mpause_addons = 0,
	mpause_scramble,
	mpause_hints,
	mpause_switchmap,

#ifdef HAVE_DISCORDSUPPORT
	mpause_discordrequests,
#endif

	mpause_continue,
	mpause_psetupsplit,
	mpause_psetupsplit2,
	mpause_spectate,
	mpause_entergame,
	mpause_switchteam,
	mpause_psetup,
	mpause_options,

	mpause_title,
	mpause_quit
} mpause_e;

extern menu_t MP_MainDef;
extern menu_t MISC_ScrambleTeamDef, MISC_ChangeTeamDef;

extern menu_t OP_MainDef;

extern menu_t OP_ChangeControlsDef;

extern menu_t OP_MPControlsDef, OP_MiscControlsDef;

extern menu_t OP_P1ControlsDef, OP_P2ControlsDef;

extern menu_t OP_MouseOptionsDef, OP_Mouse2OptionsDef;

extern menu_t OP_Joystick1Def, OP_Joystick2Def, OP_JoystickSetDef;

extern menu_t OP_CameraOptionsDef, OP_Camera2OptionsDef;

extern menu_t OP_PlaystyleDef;

extern menu_t OP_VideoOptionsDef, OP_VideoModeDef, OP_ColorOptionsDef;

#ifdef HWRENDER
extern menu_t OP_OpenGLOptionsDef;
#ifdef ALAM_LIGHTING
extern menu_t OP_OpenGLLightingDef;
#endif
#endif

extern menu_t OP_SoundOptionsDef, OP_SoundAdvancedDef;

extern menu_t OP_DataOptionsDef, OP_ScreenshotOptionsDef, OP_EraseDataDef;

extern menu_t OP_ServerOptionsDef, OP_MonitorToggleDef;

extern menu_t OP_AddonCustomOptionsDef;

//
// M_MENUFUNCS.h
//

// Called on joystick hotplug
void M_SetupJoystickMenu(INT32 choice);

#endif // __X_MENU_DEFS__
