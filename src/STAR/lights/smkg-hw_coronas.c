// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Original Copyright (C) 1998-2000 by DooM Legacy Team.
// Original Copyright (C) 1999-2023 by Sonic Team Junior.
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file hw_main.c
/// \brief hardware renderer, using the standard HardWareRender driver DLL for SRB2

#include "../../doomdef.h"

#ifdef HWRENDER

#include "smkg-lights.h"
#include "smkg-coronas.h"

#include "../../g_game.h"
#include "../../r_main.h"
#include "../../hardware/hw_glob.h"

// ------------------------ //
//        Functions
// ------------------------ //

// The only terms needed, the other terms are 0.
// Don't try to make this a matrix, this is much easier to understand and maintain.
static float world_trans_x_to_x, world_trans_y_to_x,
  world_trans_x_to_y, world_trans_y_to_y, world_trans_z_to_y,
  world_trans_x_to_z, world_trans_y_to_z, world_trans_z_to_z;
static float sprite_trans_x_to_x, sprite_trans_y_to_y, sprite_trans_z_to_y,
  sprite_trans_z_to_z, sprite_trans_y_to_z;

void HWR_set_view_transform(void)
{
	player_t *player = &players[displayplayer];
	const float fpov = FIXED_TO_FLOAT(cv_fov.value + player->fovadd);
	float tr_viewsin, tr_viewcos;
#if 0
	float gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(aimingangle>>ANGLETOFINESHIFT));
	float gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(aimingangle>>ANGLETOFINESHIFT));
#else
	float gl_viewludsin = 0, gl_viewludcos = 0;
#endif
	float gl_fovlud = (float)(1.0l/tan((double)(fpov*M_PIl/360l)));

	// Combined transforms for position, direction, look up/down, and scaling
	// translation is separate and done first
	// In order:
	//   rotation around vertical y axis
	//   look up/down
	//   scale y before frustum so that frustum can be scaled to screen height

#define SHEARING
#ifdef SHEARING
//#define FORCE_NONE_SHEARING
	boolean shearing = (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)));
#else
	boolean shearing = false;
#endif

	FTransform *trans = &atransform;
#if 0
	if (shearing)
		trans = &dometransform;
	else
		trans = &atransform;
#endif
#if 0
	shearing = trans->shearing;
#endif

	if (shearing)
	{
#define VIEWAIMING (trans->viewaiming * 2)
//#define VIEWAIMING (trans->viewaiming)

#if 1
		gl_viewludsin = (float)(cos(VIEWAIMING));
		gl_viewludcos = (float)(-sin(VIEWAIMING));
#else
		gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(aimingangle>>ANGLETOFINESHIFT));
		gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(aimingangle>>ANGLETOFINESHIFT));
#endif

#if 0
		tr_viewsin = gl_viewsin;
		tr_viewcos = gl_viewcos;
#else
		tr_viewsin = FIXED_TO_FLOAT(viewsin);
		tr_viewcos = FIXED_TO_FLOAT(viewcos);
#endif
	}
	else
	{
		gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(aimingangle>>ANGLETOFINESHIFT));
		gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(aimingangle>>ANGLETOFINESHIFT));
		tr_viewsin = gl_viewsin;
		tr_viewcos = gl_viewcos;
	}

#if 0
	tr_viewsin = gl_viewsin;
	tr_viewcos = gl_viewcos;
#endif

#ifndef FORCE_NONE_SHEARING
	if (shearing == false)
#endif
	{
		float fov_scale_x = gl_fovlud * trans->scalex; // (float)vid.fit_width / (float)vid.width;
		float fov_scale_y = gl_fovlud * trans->scaley; // ORIGINAL_ASPECT * ((float) vid.fit_height) / (float) vid.height;

		world_trans_x_to_x = tr_viewsin * fov_scale_x;
		world_trans_y_to_x = -tr_viewcos * fov_scale_x;
		world_trans_x_to_y = tr_viewcos * gl_viewludcos * fov_scale_y;
		world_trans_y_to_y = tr_viewsin * gl_viewludcos * fov_scale_y;
		world_trans_z_to_y = gl_viewludsin * fov_scale_y;
		world_trans_x_to_z = tr_viewcos * gl_viewludsin;
		world_trans_y_to_z = tr_viewsin * gl_viewludsin;
		world_trans_z_to_z = -gl_viewludcos;

		// look up/down and scaling
		sprite_trans_x_to_x = fov_scale_x;
		sprite_trans_y_to_y = gl_viewludsin * fov_scale_y;
		sprite_trans_z_to_y = gl_viewludcos * fov_scale_y;
		sprite_trans_z_to_z = gl_viewludsin;
		sprite_trans_y_to_z = -gl_viewludcos;
	}
#ifndef FORCE_NONE_SHEARING
	else
	{
		world_trans_x_to_x = gl_viewsin * gl_fovlud;
		world_trans_y_to_x = -gl_viewcos * gl_fovlud;
		world_trans_x_to_y = gl_viewcos * gl_viewludcos * ORIGINAL_ASPECT * gl_fovlud;
		world_trans_y_to_y = gl_viewsin * gl_viewludcos * ORIGINAL_ASPECT * gl_fovlud;
		world_trans_z_to_y = gl_viewludsin * ORIGINAL_ASPECT * gl_fovlud;
		world_trans_x_to_z = gl_viewcos * gl_viewludsin;
		world_trans_y_to_z = gl_viewsin * gl_viewludsin;
		world_trans_z_to_z = -gl_viewludcos;

		// look up/down and scaling
		sprite_trans_x_to_x = gl_fovlud;
		sprite_trans_y_to_y = gl_viewludsin * ORIGINAL_ASPECT * gl_fovlud;
		sprite_trans_z_to_y = gl_viewludcos * ORIGINAL_ASPECT * gl_fovlud;
		sprite_trans_z_to_z = gl_viewludsin;
		sprite_trans_y_to_z = -gl_viewludcos;
	}
#endif

#if 0
	if (shearing)
	{
		float fdy = stransform->viewaiming * 2;
		if (stransform->flip)
			fdy *= -1.0f;
		pglTranslatef(0.0f, -fdy/BASEVIDHEIGHT, 0.0f);
	}
#endif
}
#undef VIEWAIMING

#if 0
// unused because of confused wall drawing
// wx,wy,wz in world coord, to screen vxtx3d_t
void HWR_transform_world_FOut(float wx, float wy, float wz, vxtx3d_t * fovp)
{
	// Combined transforms for position, direction, look up/down, and scaling.
	// translation
	// x world, to x screen
	// vert z world, to vert y screen
	// y world, to screen depth
	float tr_x = wx - gl_viewx;
	float tr_y = wy - gl_viewy;
	float tr_z = wz - gl_viewz;
	fovp->x = (tr_x * world_trans_x_to_x)
	   + (tr_y * world_trans_y_to_x);
	fovp->y = (tr_x * world_trans_x_to_y )
	   + (tr_y * world_trans_y_to_y )
	   + (tr_z * world_trans_z_to_y );
	fovp->z = (tr_x * world_trans_x_to_z )
	   + (tr_y * world_trans_y_to_z )
	   + (tr_z * world_trans_z_to_z );
}
#endif

#if 0
// unused
void HWR_transform_sprite_FOut(float cx, float cy, float cz, vxtx3d_t * fovp)
{
	// Combined transforms for look up/down, and scaling
   fovp->y = (cy * sprite_trans_y_to_y) + (cz * sprite_trans_z_to_y);
   fovp->z = (cy * sprite_trans_y_to_z) + (cz * sprite_trans_z_to_z);
   fovp->x = (cx * sprite_trans_x_to_x);
}
#endif

#if 1
// temporary, to supply old call
void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz)
{
#if 1
	// calculate translate position
	HWR_set_view_transform();
#endif

#if 1
	const float gl_viewx = FIXED_TO_FLOAT(viewx);
	const float gl_viewy = FIXED_TO_FLOAT(viewy);
	const float gl_viewz = FIXED_TO_FLOAT(viewz);
#else
	const float gl_viewx = FIXED_TO_FLOAT(atransform.x);
	const float gl_viewy = FIXED_TO_FLOAT(atransform.y);
	const float gl_viewz = FIXED_TO_FLOAT(atransform.z);
#endif
	const float gl_viewflip = (atransform.flip ? -1.0f : 1.0f);

#if 0
	// calculate translate position
	HWR_set_view_transform();
#endif

	// translation
	// Combined transforms for position, direction, look up/down, and scaling
	float tr_x = wx - gl_viewx;
	float tr_y = wz - gl_viewy;
	float tr_z = wy - gl_viewz;

	*gx = ((tr_x * world_trans_x_to_x) + (tr_y * world_trans_y_to_x));
	*gy = ((tr_x * world_trans_x_to_y) + (tr_y * world_trans_y_to_y) + (tr_z * world_trans_z_to_y));
		// Apply screen flip
		*gy *= gl_viewflip;
#if 0
		// Apply y-shearing
		if (atransform.shearing)
		{
			float fdy = (atransform.viewaiming * 2.0f);
			if (atransform.flip)
				fdy *= -1.0f;
			*gy -= (fdy / (float)BASEVIDHEIGHT);
		}
#endif
	*gz = ((tr_x * world_trans_x_to_z) + (tr_y * world_trans_y_to_z) + (tr_z * world_trans_z_to_z));
}
#else
//void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz)
//void transform_world_to_gr(float gx, float gy, float gz, const FTransform *trans, float *outx, float *outy)
void transform_world_to_gr(float gx, float gy, float gz, /*OUT*/ float *outx, float *outy, float *outz)
{
#if 1
	// calculate translate position
	HWR_set_view_transform();
#endif

	FTransform *trans;
#if 1
	trans = &atransform;
#else
	if (shearing)
		trans = &dometransform;
	else
		trans = &atransform;
#endif

	// Step 1: Translate to view origin
	float tx = gx - trans->x;
	float ty = gy - trans->y;
	float tz = gz - trans->z;

	// Step 2: Rotate around vertical axis (yaw)
	float sinz = sinf(trans->anglez);
	float cosz = cosf(trans->anglez);
	float x1 = tx * cosz + ty * sinz;
	float y1 = -tx * sinz + ty * cosz;

	// Step 3: Rotate around horizontal axis (pitch)
	float sinx = sinf(trans->anglex);
	float cosx = cosf(trans->anglex);
	float z1 = tz * cosx - y1 * sinx;
	y1 = tz * sinx + y1 * cosx;

	// Step 4: Flip Y if needed
	if (trans->flip)
		y1 = -y1;

	// Step 5: Apply y-shearing (screen-space simulation)
	if (trans->shearing)
	{
		// Simulate what `pglTranslatef(0.0f, -fdy/BASEVIDHEIGHT, 0.0f)` does
		// viewaiming is in screen pixels → convert to GL space (usually normalized -1 to 1)
		// If you're rendering to a quad, multiply by something like (2 / screen height) here
		y1 -= trans->viewaiming; // shift corona vertically in screen-space
	}

	// Step 6: Perspective divide
	// Note: Field of view values (fovx/fovy) and screen dimensions assumed to be known
	float znear = 1.0f;
	if (y1 < znear)
		y1 = znear;

	// Apply FOV scaling — these constants depend on your projection math
	float fx = x1 / (y1 * trans->fovxangle); // fovx
	float fy = z1 / (y1 * trans->fovyangle); // fovy

	// Final output to screen-space
	*outx = (fx + 1.0f) * (float)vid.width * 0.5f;
	*outy = (1.0f - fy) * (float)vid.height * 0.5f;
#if 1
	*outz = ((tx * world_trans_x_to_z) + (ty * world_trans_y_to_z) + (tz * world_trans_z_to_z));
#endif
}
#endif

//
// HWR_Transform(float *cx, float *cy, float *cz)
// Used by Coronas to place them on maps
//
void HWR_Transform(float *cx, float *cy, float *cz)
{
	float gl_viewx, gl_viewy, gl_viewz;
	float gl_viewludsin, gl_viewludcos;
	float gl_fovlud;
	angle_t gl_aimingangle;

	float tr_x, tr_y;
	float tr_viewx, tr_viewy, tr_viewz;
	float tr_viewsin, tr_viewcos;
	float tr_viewludsin, tr_viewludcos;

	INT32 player_num[] = {
		displayplayer,
		secondarydisplayplayer,
		-1
	};

	for (INT32 i = 0; player_num[i] > -1; i++)
	{
		player_t *player = &players[player_num[i]];

		// ...Is the player ok?
		if (player == NULL || !P_IsLocalPlayer(player))
			continue;

		// POV: FOV
		const float fpov = FIXED_TO_FLOAT(cv_fov.value + player->fovadd);
		if (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)))
		{
			tr_viewx = FIXED_TO_FLOAT(viewx);
			tr_viewy = FIXED_TO_FLOAT(viewy);
			tr_viewz = FIXED_TO_FLOAT(viewz);
			tr_viewsin = FIXED_TO_FLOAT(viewsin);
			tr_viewcos = FIXED_TO_FLOAT(viewcos);
			tr_viewludsin = FIXED_TO_FLOAT(FINECOSINE(aimingangle>>ANGLETOFINESHIFT));
			tr_viewludcos = FIXED_TO_FLOAT(-FINESINE(aimingangle>>ANGLETOFINESHIFT));
		}
		else
		{
			gl_viewx = FIXED_TO_FLOAT(viewx);
			gl_viewy = FIXED_TO_FLOAT(viewy);
			gl_viewz = FIXED_TO_FLOAT(viewz);
			if (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)))
				gl_aimingangle = 0;
			else
				gl_aimingangle = aimingangle;
			gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(gl_aimingangle>>ANGLETOFINESHIFT));
			gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(gl_aimingangle>>ANGLETOFINESHIFT));
			tr_viewx = gl_viewx;
			tr_viewy = gl_viewy;
			tr_viewz = gl_viewz;
			tr_viewsin = gl_viewsin;
			tr_viewcos = gl_viewcos;
			tr_viewludsin = gl_viewludsin;
			tr_viewludcos = gl_viewludcos;
		}
		gl_fovlud = (float)(1.0l/tan((double)(fpov*M_PIl/360l)));

		// Translation
		tr_x = *cx - tr_viewx;
		tr_y = *cz - tr_viewy;
		//*cy = *cy;

		// Rotation around the Vertical Y-Axis
		*cx = (tr_x * tr_viewsin) - (tr_y * tr_viewcos);
		tr_x = (tr_x * tr_viewcos) + (tr_y * tr_viewsin);

		// Looking Up/Down (both done right here)
		tr_y = *cy - tr_viewz;
		*cy = (tr_x * tr_viewludcos) + (tr_y * tr_viewludsin);
		*cz = (tr_x * tr_viewludsin) - (tr_y * tr_viewludcos);

		// Scale the Y-Value before frustum so that frustum can be scaled to the screen's height.
		*cy *= ORIGINAL_ASPECT * gl_fovlud;
		*cx *= gl_fovlud;
		break;
	}
}

#endif
