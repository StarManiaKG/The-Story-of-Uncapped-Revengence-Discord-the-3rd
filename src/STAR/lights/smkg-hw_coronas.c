// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
// Copyright (C) 2024-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-hw_coronas.c
/// \brief TSoURDt3rd's hardware corona rendering routines

#include "../smkg-defs.h"

#ifdef HWRENDER

#include "smkg-lights.h"
#include "smkg-coronas.h"
#include "../smkg-st_hud.h"

#include "../../hardware/hw_glob.h"
#include "../../hardware/r_opengl/r_opengl.h"

#include <SDL_opengl.h>

#include "../../g_game.h"
#include "../../r_main.h"
#if 1
	#include "../../r_draw.h"
	#include "../../v_video.h"
	#include "../../z_zone.h"
#endif

// The only terms needed, the other terms are 0.
// Don't try to make this a matrix, this is much easier to understand and maintain.
float world_trans_x_to_x, world_trans_y_to_x,
  world_trans_x_to_y, world_trans_y_to_y, world_trans_z_to_y,
  world_trans_x_to_z, world_trans_y_to_z, world_trans_z_to_z;
float sprite_trans_x_to_x, sprite_trans_y_to_y, sprite_trans_z_to_y,
  sprite_trans_z_to_z, sprite_trans_y_to_z;

void HWR_set_view_transform(FTransform *stransform)
{
	FTransform *trans = stransform;

	float tr_viewsin, tr_viewcos;
	float gl_viewludsin = 0, gl_viewludcos = 0;
	float gl_fovlud;
	float fpov;

	player_t *player = &players[displayplayer];
	angle_t gl_aimingangle = 0;
	boolean shearing = false;

	// Combined transforms for position, direction, look up/down, and scaling
	// translation is separate and done first
	// In order:
	//   rotation around vertical y axis
	//   look up/down
	//   scale y before frustum so that frustum can be scaled to screen height

#define SHEARING

	if (player != NULL)
	{
		shearing = (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)));
		fpov = FIXED_TO_FLOAT(cv_fov.value + player->fovadd);
	}
	else
	{
		fpov = FIXED_TO_FLOAT(cv_fov.value);
	}
	gl_fovlud = (float)(1.0l/tan((double)(fpov*M_PIl/360l)));

	if (trans == NULL)
	{
		trans = &atransform;
	}
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
		gl_aimingangle = 0;
		//gl_aimingangle = aimingangle;

#if 0

//#define VIEWAIMING (trans->viewaiming * 2)
#define VIEWAIMING (trans->viewaiming)

#if 0
		gl_viewludsin = FIXED_TO_FLOAT(cos(VIEWAIMING));
		gl_viewludcos = FIXED_TO_FLOAT(-sin(VIEWAIMING));
#endif
#if 1
		gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE((angle_t)VIEWAIMING>>ANGLETOFINESHIFT));
		gl_viewludcos = FIXED_TO_FLOAT(-FINESINE((angle_t)VIEWAIMING>>ANGLETOFINESHIFT));
#endif
#if 0
		fixed_t fixedaiming = AIMINGTODY(aimingangle);
		gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(fixedaiming>>ANGLETOFINESHIFT));
		gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(fixedaiming>>ANGLETOFINESHIFT));
#endif
#if 0
		fixed_t fixedaiming = AIMINGTODY(aimingangle);
		gl_viewludsin = FIXED_TO_FLOAT(fixedaiming);
		gl_viewludcos = FIXED_TO_FLOAT(-fixedaiming);
#endif

#else
		// DEFAULT
		gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(gl_aimingangle>>ANGLETOFINESHIFT));
		gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(gl_aimingangle>>ANGLETOFINESHIFT));
#endif

		//trans->anglex = (float)(gl_aimingangle>>ANGLETOFINESHIFT)*(360.0f/(float)FINEANGLES);

#if 0
		tr_viewsin = gl_viewsin;
		tr_viewcos = gl_viewcos;
#else
		// DEFAULT
		tr_viewsin = FIXED_TO_FLOAT(viewsin);
		tr_viewcos = FIXED_TO_FLOAT(viewcos);
#endif
	}
	else
	{
		gl_aimingangle = aimingangle;
		gl_viewludsin = FIXED_TO_FLOAT(FINECOSINE(gl_aimingangle>>ANGLETOFINESHIFT));
		gl_viewludcos = FIXED_TO_FLOAT(-FINESINE(gl_aimingangle>>ANGLETOFINESHIFT));
		tr_viewsin = gl_viewsin;
		tr_viewcos = gl_viewcos;
	}

#if 0
	tr_viewsin = gl_viewsin;
	tr_viewcos = gl_viewcos;
#endif

//#define FORCE_NONE_SHEARING

#ifndef FORCE_NONE_SHEARING
	if (shearing == false)
#endif
	{ // DEFAULT
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
		//*gy = ((tr_x * world_trans_x_to_y) + (tr_y * world_trans_y_to_y) + (tr_z * world_trans_z_to_y));
		//pglTranslatef(0.0f, -fdy/BASEVIDHEIGHT, 0.0f);
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

#if 0
trans->viewaiming = FIXED_TO_FLOAT(fixedaiming) * ((float)vid.width / vid.height) / ((float)BASEVIDWIDTH / BASEVIDHEIGHT);
		if (splitscreen)
			trans->viewaiming *= 2.125; // splitscreen adjusts fov with 0.8, so compensate (but only halfway, since splitscreen means only half the screen is used)
		trans->shearing = true;
		gl_aimingangle = 0;
	}
	else
	{
		trans->shearing = false;
		gl_aimingangle = aimingangle;
	}

	trans->anglex = (float)(gl_aimingangle>>ANGLETOFINESHIFT)*(360.0f/(float)FINEANGLES);
#endif

#if 1
// temporary, to supply old call
void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz)
{
	// calculate translate position
	//HWR_set_view_transform();

#if 1
	// DEFAULT
	const float gl_viewx = FIXED_TO_FLOAT(viewx);
	const float gl_viewy = FIXED_TO_FLOAT(viewy);
	const float gl_viewz = FIXED_TO_FLOAT(viewz);
#endif
#if 0
	const float gl_viewx = FIXED_TO_FLOAT(atransform.x);
	const float gl_viewy = FIXED_TO_FLOAT(atransform.y);
	const float gl_viewz = FIXED_TO_FLOAT(atransform.z);
#endif
#if 0
	const float gl_viewx = FIXED_TO_FLOAT(atransform.x);
	const float gl_viewy = FIXED_TO_FLOAT(atransform.z);
	const float gl_viewz = FIXED_TO_FLOAT(atransform.y);
#endif
	const float gl_viewflip = (atransform.flip ? -1.0f : 1.0f); // Apply screen flip
	boolean shearing = false;
	player_t *player = &players[displayplayer];

	if (player != NULL)
	{
		shearing = (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)));
	}

	// calculate translate position
	//HWR_set_view_transform();

	// translation
	// Combined transforms for position, direction, look up/down, and scaling
	float tr_x = wx - gl_viewx;
	float tr_y = wz - gl_viewy;
	float tr_z = wy - gl_viewz;

	*gx = ((tr_x * world_trans_x_to_x) + (tr_y * world_trans_y_to_x));
	*gy = (((tr_x * world_trans_x_to_y) + (tr_y * world_trans_y_to_y) + (tr_z * world_trans_z_to_y)) * gl_viewflip);
		//*gy *= gl_viewflip;
#if 1
#define SHEARING
		// Apply y-shearing
		//if (atransform.shearing)
		if (shearing)
		{
#if 0
			//float fdy = (atransform.viewaiming * 2.0f);
			float fdy = atransform.viewaiming;
			//*gy -= (fdy / (float)BASEVIDHEIGHT);
			*gy -= (fdy / (float)vid.height);
#else
			float shear_offset = atransform.viewaiming * ((float)vid.height / (float)BASEVIDHEIGHT);
  			if (atransform.flip)
	  			shear_offset = -shear_offset;

   			// This adjusts vertical placement to match software look
			*gy -= shear_offset;
#endif
		}
#endif
	*gz = ((tr_x * world_trans_x_to_z) + (tr_y * world_trans_y_to_z) + (tr_z * world_trans_z_to_z));
}
#endif

#if 0
#if 1
// Projects a world-space point (wx,wy,wz) to screen pixels (gx,gy) and depth (gz).
// Uses the renderer's cached modelMatrix/projMatrix/viewport so any GL shearing
// or custom projection is correctly respected.
void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz)
{
	// modelMatrix, projMatrix and viewport are provided by r_opengl.c (cached).
	// They must be valid and up-to-date at the time this is called.
	GLfloat out[4], clip[4];
	int i;

	// Transform by modelview matrix (object -> eye)
	for (i = 0; i < 4; ++i)
	{
		out[i] =
			wx * modelMatrix[0*4 + i] +
			wy * modelMatrix[1*4 + i] +
			wz * modelMatrix[2*4 + i] +
			modelMatrix[3*4 + i];
	}

	// Transform by projection matrix (eye -> clip)
	for (i = 0; i < 4; ++i)
	{
		clip[i] =
			out[0] * projMatrix[0*4 + i] +
			out[1] * projMatrix[1*4 + i] +
			out[2] * projMatrix[2*4 + i] +
			out[3] * projMatrix[3*4 + i];
	}

	// If behind the camera or w nearly zero, mark off-screen
	if (fpclassify(clip[3]) == FP_ZERO || clip[3] <= 1e-6f)
	{
		*gx = *gy = -1.0f;
		*gz = -1.0f;
		return;
	}

	// Perspective divide to NDC (-1..1)
	clip[0] /= clip[3];
	clip[1] /= clip[3];
	clip[2] /= clip[3];

	// Map NDC (-1..1) to 0..1
	clip[0] = clip[0] * 0.5f + 0.5f;
	clip[1] = clip[1] * 0.5f + 0.5f;
	clip[2] = clip[2] * 0.5f + 0.5f;

	// Map to viewport pixels
	// viewport[0]=x, [1]=y, [2]=width, [3]=height
	*gx = clip[0] * (float)viewport[2] + (float)viewport[0];
	*gy = clip[1] * (float)viewport[3] + (float)viewport[1];
	*gz = clip[2];
}
#else
//void transform_world_to_gr(float wx, float wy, float wz, /*OUT*/ float *gx, float *gy, float *gz)
//void transform_world_to_gr(float gx, float gy, float gz, const FTransform *trans, float *outx, float *outy)
void transform_world_to_gr(float gx, float gy, float gz, /*OUT*/ float *outx, float *outy, float *outz)
{
	// calculate translate position
	//HWR_set_view_transform();

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
#endif

//
// HWR_Transform(float *cx, float *cy, float *cz)
//
// Transforms the positions of objects (I.E Coronas).
// This allows them to accurately be placed within a level.
//

#if 1

//#define OPENGL_WORKS

#ifdef OPENGL_WORKS
	//Alam_GBC: Simple, yes?
	#include <SDL.h>
	#include <SDL_opengl.h>
	#include "../../hardware/r_opengl/r_opengl.h"
#endif

void HWR_DebugDrawPoint(float x, float y, float z, float r, float g, float b)
{
#ifdef OPENGL_WORKS
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST); // So it's always visible
	glPointSize(6.0f); // Make it visible
	glBegin(GL_POINTS);
		glColor3f(r, g, b);
		glVertex3f(x, y, z);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
#else
	(void)x;
	(void)y;
	(void)z;
	(void)r;
	(void)g;
	(void)b;
#endif
}
void HWR_DebugDrawLine(float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b)
{
#ifdef OPENGL_WORKS
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
		glColor3f(r, g, b);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
#else
	(void)x1;
	(void)y1;
	(void)z1;
	(void)x2;
	(void)y2;
	(void)z2;
	(void)r;
	(void)g;
	(void)b;
#endif
}

#endif

#if 1

void HWR_Transform(float *cx, float *cy, float *cz)
{
	angle_t gl_aimingangle;
	float gl_fovlud;
	const float gl_viewflip = (atransform.flip ? -1.0f : 1.0f);

	float tr_x, tr_y, tr_z;
	float tr_viewx, tr_viewy, tr_viewz;
	float tr_viewsin, tr_viewcos;
	float tr_viewludsin, tr_viewludcos;

	INT32 player_num[] = {
		displayplayer,
		secondarydisplayplayer,
		-1
	};
	player_t *player = NULL;

	for (INT32 i = 0; player_num[i] > -1; i++)
	{
		player = &players[player_num[i]];

		// ...Is the player ok?
		if (player == NULL || !P_IsLocalPlayer(player))
			continue;

		// POV: FOV
		const float fpov = FIXED_TO_FLOAT(cv_fov.value + player->fovadd);
		boolean shearing = (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)));

#if 1
		tr_viewx = FIXED_TO_FLOAT(viewx);
		tr_viewy = FIXED_TO_FLOAT(viewy);
		tr_viewz = FIXED_TO_FLOAT(viewz);
#else
		tr_viewx = FIXED_TO_FLOAT(player->mo->x);
		tr_viewy = FIXED_TO_FLOAT(player->mo->z);
		tr_viewz = FIXED_TO_FLOAT(player->mo->y);
#endif

		if (shearing)
		{
#if 1
#define VIEWAIMING (atransform.viewaiming * 2)
//#define VIEWAIMING (atransform.viewaiming)
		tr_viewludsin = (float)(cos(VIEWAIMING));
		tr_viewludcos = (float)(-sin(VIEWAIMING));
#endif
			gl_aimingangle = aimingangle;
			tr_viewsin = FIXED_TO_FLOAT(viewsin);
			tr_viewcos = FIXED_TO_FLOAT(viewcos);
		}
		else
		{
			//gl_aimingangle = 0;
			gl_aimingangle = aimingangle;
			tr_viewsin = gl_viewsin;
			tr_viewcos = gl_viewcos;
		}
		tr_viewludsin = FIXED_TO_FLOAT(FINECOSINE(gl_aimingangle>>ANGLETOFINESHIFT));
		tr_viewludcos = FIXED_TO_FLOAT(-FINESINE(gl_aimingangle>>ANGLETOFINESHIFT));
		gl_fovlud = (float)(1.0l/tan((double)(fpov*M_PIl/360l)));

#if 1
		// Translation
		tr_x = *cx - tr_viewx;
		tr_y = *cz - tr_viewy;
		tr_z = *cy - tr_viewz;

		// Rotation around the Vertical Y-Axis
		*cx = (tr_x * tr_viewsin) - (tr_y * tr_viewcos);
		tr_x = (tr_x * tr_viewcos) + (tr_y * tr_viewsin);

		// Looking Up/Down (both done right here)
		tr_y = *cy - tr_viewz;
#if 0
		*cy = (tr_x * tr_viewludcos) + (tr_y * tr_viewludsin) + (tr_z * tr_viewcos);
		*cz = (tr_x * tr_viewludsin) - (tr_y * tr_viewludcos);
#else
		*cy = (tr_x * tr_viewludcos) + (tr_y * tr_viewludsin);
		*cz = (tr_x * tr_viewludsin) - (tr_y * tr_viewludcos) + (tr_z * tr_viewludcos);
#endif

		// Scale the Y-Value before frustum so that frustum can be scaled to the screen's height.
		*cy *= ORIGINAL_ASPECT * gl_fovlud;
			*cy *= gl_viewflip; // Apply screen flip
		*cx *= gl_fovlud;
#else
		// Translation relative to camera
		tr_x = *cx - tr_viewx;
		tr_y = *cy - tr_viewy;
		float tr_z = *cz - tr_viewz;

		// Horizontal (yaw) rotation around Y-axis
		float rotated_x = (tr_x * tr_viewcos) - (tr_z * tr_viewsin);
		float rotated_z = (tr_x * tr_viewsin) + (tr_z * tr_viewcos);

		// Vertical (pitch) rotation
		float rotated_y = tr_y;
		float final_y = (rotated_z * tr_viewludsin) + (rotated_y * tr_viewludcos);
		float final_z = (rotated_z * tr_viewludcos) - (rotated_y * tr_viewludsin);

		// Assign back transformed positions
		*cx = rotated_x * gl_fovlud;
		*cy = final_y * ORIGINAL_ASPECT * gl_fovlud;
			*cy *= gl_viewflip; // Apply screen flip
		*cz = final_z;
#endif

		break;
	}
}

#else

void HWR_Transform(float *cx, float *cy, float *cz)
{
	float tr_viewx, tr_viewy, tr_viewz;
	float yaw_cos, yaw_sin;
	float pitch_cos, pitch_sin;
	float gl_fov_scale;

	INT32 player_num[] = {
		displayplayer,
		secondarydisplayplayer,
		-1
	};
	player_t *player = NULL;

	for (INT32 i = 0; player_num[i] > -1; i++)
	{
		player = &players[player_num[i]];

		if (!player || !P_IsLocalPlayer(player))
			continue;

		const float fov = FIXED_TO_FLOAT(cv_fov.value + player->fovadd);
		boolean shearing = (cv_glshearing.value == 1 || (cv_glshearing.value == 2 && R_IsViewpointThirdPerson(player, false)));

		// Camera position
		tr_viewx = FIXED_TO_FLOAT(player->mo->x);
		tr_viewy = FIXED_TO_FLOAT(player->mo->y); // Z in Doom = Y in OpenGL
		tr_viewz = FIXED_TO_FLOAT(player->mo->z); // Y in Doom = Z in OpenGL

		// Get aiming angle
		angle_t pitch_angle = (shearing ? aimingangle : 0);

		if (shearing)
		{
			yaw_sin = FIXED_TO_FLOAT(viewsin);
			yaw_cos = FIXED_TO_FLOAT(viewcos);
		}
		else
		{
			yaw_sin = FIXED_TO_FLOAT(gl_viewsin);
			yaw_cos = FIXED_TO_FLOAT(gl_viewcos);
		}
		pitch_sin = FIXED_TO_FLOAT(FINESINE(pitch_angle >> ANGLETOFINESHIFT));
		pitch_cos = FIXED_TO_FLOAT(FINECOSINE(pitch_angle >> ANGLETOFINESHIFT));

		gl_fov_scale = 1.0f / tan(fov * (float)M_PI / 360.0f); // vertical FOV

		// Translate to camera space
		float rel_x = *cx - tr_viewx;
		float rel_y = *cy - tr_viewy;
		float rel_z = *cz - tr_viewz;

		// Rotate around Y-axis (yaw)
		float x1 = rel_x * yaw_cos - rel_z * yaw_sin;
		float z1 = rel_x * yaw_sin + rel_z * yaw_cos;

		// Rotate around X-axis (pitch)
		float y1 = rel_y * pitch_cos - z1 * pitch_sin;
		float z2 = rel_y * pitch_sin + z1 * pitch_cos;

		// Apply FOV scaling
		*cx = x1 * gl_fov_scale;
		*cy = y1 * ORIGINAL_ASPECT * gl_fov_scale;
		*cz = z2;

		break; // Only apply once
	}
}

#endif

#endif // HWRENDER
