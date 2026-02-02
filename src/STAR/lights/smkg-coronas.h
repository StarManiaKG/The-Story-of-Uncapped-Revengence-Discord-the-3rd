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

typedef struct sprite_light_data_s
{
	float size;
	UINT8 alpha;
	UINT8 bright; // used by software draw to brighten active light sources
} sprite_light_data_t;

// Propotional fade of corona from Z1 to Z2
// Hurdler: more realistique corona !
#define CORONA_Z1 (250.0f)
#define CORONA_Z2 ((255.0f*8) + CORONA_Z1)

#ifdef ALAM_LIGHTING

light_t *Sprite_Corona_Light_lsp(int sprnum);
boolean Sprite_Corona_Light_fade(light_t *lsp, float cz, mobj_t *mobj, sprite_light_data_t *light_data);

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
void HWR_OG_Transform(float *cx, float *cy, float *cz);
void HWR_Transform_CB(float *cx, float *cy, float *cz, boolean sprite);

#if 1
#include "../../hardware/hw_main.h"
void HWR_set_view_transform(FTransform *stransform);
void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz);
#endif

// The only terms needed, the other terms are 0.
// Don't try to make this a matrix, this is much easier to understand and maintain.
extern float world_trans_x_to_x, world_trans_y_to_x,
  world_trans_x_to_y, world_trans_y_to_y, world_trans_z_to_y,
  world_trans_x_to_z, world_trans_y_to_z, world_trans_z_to_z;
extern float sprite_trans_x_to_x, sprite_trans_y_to_y, sprite_trans_z_to_y,
  sprite_trans_z_to_z, sprite_trans_y_to_z;

#endif

#endif // ALAM_LIGHTING
#endif // __SMKG_CORONAS__
