// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg_m_func.h
/// \brief Unique TSoURDt3rd menu routines and structures, global header

#ifndef __SMKG_M_FUNC__
#define __SMKG_M_FUNC__

#include "../m_menu.h" // tsourdt3rd_menu_t //

void TSoURDt3rd_M_AddNewMenu(tsourdt3rd_menu_t *newmenu, menu_t *menup);
void TSoURDt3rd_M_SetupNextMenu(menu_t *menudef);

boolean STAR_M_DoesMenuHaveKeyHandler(void);

void STAR_M_RevampedMenuInputWrapper(INT32 *key);
boolean STAR_M_Responder(event_t *ev);
boolean STAR_M_StartControlPanel(void);

void STAR_M_GoBack(INT32 choice);

void STAR_M_ChangeCvarDirect(INT32 choice, consvar_t *cv);

boolean STAR_M_NextOpt(void);
boolean STAR_M_PrevOpt(void);

void STAR_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter);

void STAR_M_MenuMessageTick(boolean run);

#endif // __SMKG_M_FUNC__
