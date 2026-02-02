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

#ifndef __HW_LIGHTS__
#define __HW_LIGHTS__

#include "hw_glob.h"
#include "hw_defs.h"

#include "../vector3d.h" // Real vectors! No more fake ones!

#ifdef ALAM_LIGHTING

// hw_main.c
#define RENDER_CORONAS_BEFORE_TRANS

// ^^^
// HWR Plane and WALL rendering might work
// better with this off

typedef enum
{
	DYNLIGHT_ORIGIN_NONE       = -1,
	DYNLIGHT_ORIGIN_VISSPRITE,
	DYNLIGHT_ORIGIN_MOBJ,
} dynlight_origin_t;

typedef struct
{
	light_t *type;
	int flags;
	floatvector3_t pos;
	floatvector3_t offset;
	RGBA_t color;
	UINT32 radius;
} dynlight_basic_t;

#pragma once

// (C) GZDoom & UZDoom Contributors/Developers
// Ashi: The node system for lights is brilliant and I'm borrowing it
//typedef struct FDynamicLight FDynamicLight;
typedef struct FDynamicLight_s FDynamicLight;
typedef struct FDynamicLightTouchLists FDynamicLightTouchLists;
typedef struct FLightNode FLightNode;
struct FLightNode
{
	FLightNode **prevTarget;
	FLightNode *nextTarget;
	FLightNode **prevLight;
	FLightNode *nextLight;
	FDynamicLight *lightsource;
	union
	{
		side_t *targLine;
		subsector_t *targSubsector;
		void *targ;
	};
};

struct FDynamicLightTouchLists
{
	//TArray<FSection*> flat_tlist;
	//TArray<side_t*> wall_tlist;
	sector_t *flat_tlist;
	side_t *wall_tlist;
};

//struct FDynamicLight
typedef struct FDynamicLight_s
{
	FDynamicLight *next, *prev;
	FDynamicLightTouchLists touchlists;
	sector_t *sector;
	//FLevelLocals *Level;
	void *actor;

	light_t *light_data;
	dynlight_origin_t actor_type;

	boolean visible;
	bool owned;

	floatvector3_t pos;
	floatvector3_t offset;

	float max_radius; // The maximum size the light can be with its current settings.
	float radius;     // The current light size.
	RGBA_t color;
	UINT8 *colormap;
	INT32 poly_flags;
} FDynamicLight;

extern INT32 num_dynamic_lights;

void HWR_DynamicShadowing(FOutVector *clVerts, size_t nrClipVerts);
void HWR_SpriteLighting(FOutVector *wlVerts); // SRB2CBTODO: Support sprites to be lit too
void HWR_WallLighting(FSurfaceInfo *pSurf, FOutVector *wlVerts, FBITFIELD PolyFlags, FBITFIELD ExtraPolyFlags, int shader);
void HWR_PlaneLighting(FSurfaceInfo *pSurf, FOutVector *clVerts, size_t nrClipVerts, FBITFIELD PolyFlags, boolean horizonSpecial);

void HWR_DoCoronasLighting(FOutVector *outVerts, gl_vissprite_t *spr);
void HWR_DL_Draw_Coronas(void);

boolean HWR_DL_AddBasicLight(INT32 type, double x, double y, double z, long color, long radius, INT32 flags);
boolean HWR_DL_AddLight(void *origin, dynlight_origin_t origin_type);
void HWR_Update_Lights(void);
void HWR_DL_RemoveLight(void *origin);
void HWR_DL_ClearLights(void);
void HWR_Init_Light(const char *light_patch);

void HWR_DL_CreateStaticLightmaps(INT32 bspnum);

#endif // ALAM_LIGHTING
#endif // __HW_LIGHTS__
