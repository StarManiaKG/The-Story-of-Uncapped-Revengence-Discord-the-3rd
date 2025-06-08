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
