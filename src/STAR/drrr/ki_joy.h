// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  ki_joy.h
/// \brief share joystick information with game control code

#ifndef __KI_JOY_H__
#define __KI_JOY_H__

#include "../../doomstat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JOYANALOGS   2 // 2 sets of analog stick axes, with positive and negative each
#define JOYTRIGGERS  1 // 1 set of trigger axes, positive only
#define JOYAXISSETS (JOYANALOGS + JOYTRIGGERS)
#define JOYAXES ((4 * JOYANALOGS) + (2 * JOYTRIGGERS))

void I_SetGamepadPlayerIndex(INT32 device_id, INT32 index);
void I_GetGamepadGuid(INT32 device_id, char *out, int out_len);
void I_GetGamepadName(INT32 device_id, char *out, int out_len);
void I_GamepadRumble(INT32 device_id, UINT16 low_strength, UINT16 high_strength);
void I_GamepadRumbleTriggers(INT32 device_id, UINT16 left_strength, UINT16 right_strength);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __KI_JOY_H__
