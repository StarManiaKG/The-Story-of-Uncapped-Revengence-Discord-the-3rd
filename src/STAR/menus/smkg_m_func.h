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

#include "smkg-m_sys.h" // tsourdt3rd_menu_t //

typedef enum
{
	TSOURDT3RD_LLM_CREATESERVER,
	TSOURDT3D_LLM_LEVELSELECT,
	TSOURDT3RD_LLM_RECORDATTACK,
	TSOURDT3RD_LLM_NIGHTSATTACK
} tsourdt3rd_levellist_mode_t;
extern tsourdt3rd_levellist_mode_t tsourdt3rd_levellistmode;

void TSoURDt3rd_M_InitQuitMessages(const char **msg_table);
const char *TSoURDt3rd_M_GenerateQuitMessage(void);

void TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_menu_t *tsourdt3rd_menudef, menu_t *menudef, boolean notransition);

void TSoURDt3rd_M_UpdateMenuCMD(UINT8 i);
INT32 STAR_M_DoesMenuHaveKeyHandler(void);

boolean TSoURDt3rd_M_Responder(INT32 *ch, event_t *ev);

void TSoURDt3rd_M_PlayMenuJam(void);

boolean TSoURDt3rd_M_StartControlPanel(void);
void TSoURDt3rd_M_ClearMenus(boolean callexitmenufunc);

void STAR_M_GoBack(INT32 choice);

void TSoURDt3rd_M_ChangeCvar(INT32 choice, consvar_t *cv);
void TSoURDt3rd_M_ChangeCvarDirect(INT32 amount, float amount_f, consvar_t *cv);

boolean STAR_M_NextOpt(void);
boolean STAR_M_PrevOpt(void);

void STAR_M_Ticker(INT16 *item, boolean *input, INT16 skullAnimCounter, INT32 levellistmode);

void TSoURDt3rd_M_Controls_MapProfileControl(event_t *ev);

#endif // __SMKG_M_FUNC__
