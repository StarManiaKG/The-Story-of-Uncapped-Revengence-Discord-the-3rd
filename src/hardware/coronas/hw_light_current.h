// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief Dynamic lighting & coronas

#ifndef _HW_LIGHTS_
#define _HW_LIGHTS_

#include "hw_glob.h"
#include "hw_defs.h"

#define NUMLIGHTFREESLOTS 32 // Free light slots (for SOCs)

#ifdef ALAM_LIGHTING
#define DL_MAX_SHADOW 256
#define DL_MAX_LIGHT 256 // Maximum number of lights per frame (extra lights are ignored)

void HWR_InitLight(void);
void HWR_AddDynLight(gl_vissprite_t *spr, double x, double y, double z, byte radius);
void HWR_AddDynShadow(double x, double y, double z, ULONG color, byte radius, gl_vissprite_t *spr);
void HWR_PlaneLighting(FOutVector *clVerts, int nrClipVerts);
void HWR_RenderFloorSplat(FOutVector *clVerts, int nrClipVerts);
void HWR_WallShading(FOutVector *wlVerts);
void HWR_WallLighting(FOutVector *wlVerts);
void HWR_SpriteLighting(FOutVector *wlVerts);
void HWR_ResetLights(void);
void HWR_ResetShadows(void);
void HWR_SetLights(int viewnumber);
void HWR_SetShadows(int viewnumber);
void HWR_DrawCoronas(double x, double y, double z, double size);

// OpenGL dynamic shadow support
typedef struct shadow_s
{
	ULONG dynamic_color;  // color of the light for dynamic lighting
	float dynamic_radius; // radius of the light ball
	gl_vissprite_t *spr; // Store data for a sprite
} shadow_t;

typedef struct
{
	int nb; // SRB2CBTODO: What is this?
	light_t *p_lspr[DL_MAX_LIGHT]; // SRB2CBTODO: Rename
	FVector position[DL_MAX_LIGHT]; // actually maximum DL_MAX_LIGHT lights
	mobj_t *mo[DL_MAX_LIGHT];
} dynlights_t;


typedef struct
{
	int nb;
	shadow_t *p_sspr[DL_MAX_LIGHT];
	FVector position[DL_MAX_LIGHT]; // actually maximum DL_MAX_LIGHT lights
} dynshadows_t;
#endif

typedef enum
{
	NOLIGHT = 0,
	RINGSPARK_L,
	SUPERSONIC_L, // Cool. =)
	SUPERSPARK_L,
	INVINCIBLE_L,
	GREENSHIELD_L,
	BLUESHIELD_L,
	YELLOWSHIELD_L,
	REDSHIELD_L,
	BLACKSHIELD_L,
	WHITESHIELD_L,
	SMALLREDBALL_L,
	RINGLIGHT_L,
	GREENSMALL_L,
	REDSMALL_L,
	GREENSHINE_L,
	ORANGESHINE_L,
	PINKSHINE_L,
	BLUESHINE_L,
	REDSHINE_L,
	LBLUESHINE_L,
	GREYSHINE_L,
	REDBALL_L,
	GREENBALL_L,
	BLUEBALL_L,
	NIGHTSLIGHT_L,
	JETLIGHT_L,
	GOOPLIGHT_L,
	STREETLIGHT_L,

	// free slots for SOCs at run-time --------------------
	FREESLOT0_L,
	//
	// ... 32 free lights here ...
	//
	LASTFREESLOT_L = (FREESLOT0_L+NUMLIGHTFREESLOTS-1),
	// end of freeslots ---------------------------------------------

	NUMLIGHTS
} lightspritenum_t;

extern light_t lspr[NUMLIGHTS];
extern light_t *t_lspr[NUMSPRITES];
#endif
