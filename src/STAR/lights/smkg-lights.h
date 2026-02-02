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
/// \file  smkg-lights.h
/// \brief TSoURDt3rd's dynamic light data, global header

#ifndef __SMKG_LIGHTS__
#define __SMKG_LIGHTS__

#include "../smkg-defs.h"

#include "../../p_mobj.h"
//#include "../../r_defs.h"

// ------------------------ //
//        Variables
// ------------------------ //

#define NUMLIGHTFREESLOTS 64 // Free light slots (for SOCs)

typedef enum lightspritenum_s
{
	NOLIGHT = 0,
	RINGSPARK_L,
	PLAYER_L, // Cool. =)
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
	ROCKET_L,
	ROCKETEXP_L,
	EMERALD_L,
	GENERIC_MOBJLIGHT_L,

	// free slots for SOCs at run-time --------------------
	FREESLOT0_L,
	//
	// ... 32 free lights here ...
	//
	LASTFREESLOT_L = (FREESLOT0_L+NUMLIGHTFREESLOTS-1),
	// end of freeslots ---------------------------------------------

	NUMLIGHTS
} lightspritenum_t;

// Special sprite lighting. Optimized for Hardware, OpenGL.
typedef struct light_s
{
	UINT16             type; // sprite_light_e, used in hwr_light.c

	float     light_xoffset; // unused
	float     light_yoffset; // y offset to adjust corona's height

	UINT32     corona_color; // color of the light for static lighting
	float     corona_radius; // radius of the coronas

	UINT32    dynamic_color; // color of the light for dynamic lighting
	float    dynamic_radius; // radius of the light ball
	float dynamic_sqrradius; // radius^2 of the light ball

	UINT16       impl_flags; // implementation flags, sprite_light_impl_flags_e

	boolean (*corona_rendering_routine)(mobj_t *mobj); // corona rendering routine, returns true if things shouldn't be rendered
	boolean (*corona_coloring_routine)(mobj_t *mobj, RGBA_t *rgba_table, UINT8 *alpha, boolean dynamic); // corona coloring routine, returns true if the coder should handle coloring on their own
} light_t;

extern light_t lspr[NUMLIGHTS];
extern light_t *t_lspr[NUMSPRITES];

// ------------------------ //
//        Functions
// ------------------------ //

// -----------------------
// Rendering
// -----------------------

boolean L_RenderPlayerLight(mobj_t *mobj);

// -----------------------
// Coloring
// -----------------------

boolean L_UseObjectColor(mobj_t *mobj, RGBA_t *rgba_table, UINT8 *alpha, boolean dynamic);
boolean L_UseEmeraldLight(mobj_t *mobj, RGBA_t *rgba_table, UINT8 *alpha, boolean dynamic);

#endif // __SMKG_LIGHTS__
