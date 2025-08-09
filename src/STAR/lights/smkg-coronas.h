// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-coronas.h
/// \brief TSoURDt3rd's remastered coronas

#ifndef __SMKG_CORONAS__
#define __SMKG_CORONAS__

#include "../smkg-defs.h"

#include "../../command.h"
#include "../../p_mobj.h"
#include "../../r_defs.h"
#include "../../r_things.h"

// [WDJ] Sprite light sources bit defines.
typedef enum
{
  	UNDEFINED_SPR	= 0x00, // actually just for testing

	// Effect enables //
  	CORONA_SPR		= 0x01,	// a light source which only emits a corona
  	DYNLIGHT_SPR	= 0x02, // a light source which is only used for dynamic lighting

	// Type field, can create a light source
  	TYPE_FIELD_SPR	= 0xF0, // working type setting

  	SPLT_unk      = 0x00, // phobiata, newmaps default, plain corona
  	SPLT_rocket   = 0x10, // flicker
  	SPLT_lamp     = 0x20,
  	SPLT_fire     = 0x30, // slow flicker, torch
	//MONSTER_SPR  = 0x40,
	//AMMO_SPR     = 0x50,
	//BONUS_SPR    = 0x60,
  	SPLT_light    = 0xC0, // no fade
  	SPLT_firefly  = 0xD0, // firefly flicker, un-synch
  	SPLT_random   = 0xE0, // random LED, un-synch
  	SPLT_pulse    = 0xF0, // slow pulsation, un-synch

	// Effect combinations //
  	LIGHT_SPR		= (DYNLIGHT_SPR|CORONA_SPR),
	ROCKET_SPR		= (DYNLIGHT_SPR|CORONA_SPR|SPLT_rocket),
} sprite_light_flags_e;

typedef enum
{
   	SLI_type_set= 0x02,  		// the type was set, probably by fragglescript
   	SLI_corona_set= 0x04,		// the corona was set, only by fragglescript
   	SLI_changed = 0x08,  		// the data was changed, probably by fragglescript
} sprite_light_impl_flags_e;

// Propotional fade of corona from Z1 to Z2
#define CORONA_Z1 (250.0f)
#define CORONA_Z2 ((255.0f*8) + 250.0f)

#ifdef ALAM_LIGHTING

extern float corona_size;
extern UINT8 corona_alpha, corona_bright;

light_t *Sprite_Corona_Light_lsp(int sprnum);
UINT8 Sprite_Corona_Light_fade(light_t *lsp, float cz, mobj_t *mobj);

void TSoURDt3rd_R_RenderSoftwareCoronas(vissprite_t *spr, INT32 x1, INT32 x2);
void TSoURDt3rd_R_DrawSoftwareCoronas(vissprite_t *spr);

void TSoURDt3rd_R_Load_Corona(void);
void TSoURDt3rd_R_Release_Coronas(void);

#ifdef HWRENDER
#if 1
void HWR_DebugDrawPoint(float x, float y, float z, float r, float g, float b);
void HWR_DebugDrawLine(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b);
#endif
void HWR_Transform(float *cx, float *cy, float *cz);

#if 1
void HWR_set_view_transform(void);
void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz);
#endif

patch_t *HWR_GetPic(lumpnum_t lumpnum);
#endif

#endif // ALAM_LIGHTING
#endif // __SMKG_CORONAS__
