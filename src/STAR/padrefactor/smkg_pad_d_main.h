// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_pad_d_main.h
/// \brief Gamepad refactror main program data

#ifdef __SMKG_PAD_D_MAIN__
#define __SMKG_PAD_D_MAIN__

#include "../../d_event.h"

void STAR_GamepadR_D_ProcessEvents(event_t *ev);
void STAR_GamepadR_D_UpdateMenuControls(void);

void STAR_GamepadR_D_ResetGamepadData(event_t *ev);

#endif // __SMKG_PAD_D_MAIN__
