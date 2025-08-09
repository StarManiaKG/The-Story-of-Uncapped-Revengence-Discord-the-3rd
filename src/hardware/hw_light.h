// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2015 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file hw_light.h
/// \brief Dynamic lighting & coronas add on by Hurdler

#ifndef _HW_LIGHTS_
#define _HW_LIGHTS_

#include "hw_glob.h"
#include "hw_defs.h"

#ifdef ALAM_LIGHTING
#define DL_MAX_LIGHT UINT16_MAX //255 // maximum number of lights (extra lights are ignored)

void HWR_Init_Light(const char *lightpatch);
void HWR_DynamicShadowing(FOutVector *clVerts, int nrClipVerts);
void HWR_SpriteLighting(FOutVector *wlVerts); // SRB2CBTODO: Support sprites to be lit too
void HWR_PlaneLighting(FOutVector *clVerts, FUINT nrClipVerts, FBITFIELD PolyFlags, int shader, boolean horizonSpecial);
void HWR_WallLighting(FOutVector *wlVerts, FBITFIELD PolyFlags, int shader);
void HWR_Reset_Lights(void);
void HWR_Set_Lights(UINT8 viewnumber);

void HWR_DL_Draw_Coronas(void);
void HWR_DL_AddLightSprite(gl_vissprite_t *spr);

void HWR_DoCoronasLighting(FOutVector *outVerts, gl_vissprite_t *spr);

void HWR_DL_CreateStaticLightmaps(INT32 bspnum);

typedef struct
{
	UINT16 nb; // number of dynamic lights
	light_t *p_lspr[DL_MAX_LIGHT];
	FVector position[DL_MAX_LIGHT]; // actually maximum DL_MAX_LIGHT lights
	mobj_t *mo[DL_MAX_LIGHT];
} dynlights_t;

#endif // ALAM_LIGHTING
#endif
