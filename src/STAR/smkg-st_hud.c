// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-st_hud.c
/// \brief TSoURDt3rd cool HUD data

#include "smkg-st_hud.h"
#include "smkg-cvars.h"
#include "star_vars.h"

#include "../d_main.h"
#include "../g_game.h"
#include "../i_time.h"
#include "../i_video.h"
#include "../lua_hud.h"
#include "../r_main.h"
#include "../v_video.h"
#include "../z_zone.h"
#include "../r_patch.h" // Patch_Free

#define LUA_TAN(x) FINETANGENT(((x + ANGLE_90) >> ANGLETOFINESHIFT) & 4095) // tan function used by Lua
#define LUA_COS(x) FINECOSINE((x >> ANGLETOFINESHIFT) & FINEMASK)

//
//{ 	Patch Definitions
patch_t *kp_button_a[2][2];
patch_t *kp_button_b[2][2];
patch_t *kp_button_x[2][2];

patch_t *gen_button_keyleft[2];
patch_t *gen_button_keyright[2];
patch_t *gen_button_keycenter[2];

patch_t *tsourdt3rd_easter_leveleggs;
patch_t *tsourdt3rd_easter_totaleggs;

#ifdef HAVE_DISCORDSUPPORT
// Discord Rich Presence
static patch_t *envelope;
#endif

//
// }
//

//
// static void HUD_LoadGraphics(const char *name, patch_t *patch, int patches, int letter, int type)
// Loads the graphics into the given struct based on the given patch properties.
//
static void HUD_LoadGraphics(const char *name, patch_t **patch, int patches, int letter, int type)
{
	int i = 0, cur_patch_num = 'A';
	const char *curpatch_name;

	for (i = 0; i < patches; i++)
	{
		cur_patch_num = ((i+'A' == 'A') ? '\0' : i+'A');
		curpatch_name = va("%s%c%c", name, cur_patch_num, letter);
		if (W_CheckNumForName(curpatch_name) == LUMPERROR)
			patch[i] = (patch_t *)W_CachePatchName("MISSING", PU_HUDGFX);
		else
			patch[i] = (patch_t *)W_CachePatchName(curpatch_name, type);
	}
}
#define HUD_LoadGraphicsSingular(name, kp, tag) HUD_LoadGraphics(name, kp, 1, '\0', tag)

patch_t *HU_UpdateOrBlankPatch(patch_t **user, boolean required, const char *format, ...)
{
	va_list ap;
	char buffer[256 + 1];

	lumpnum_t lump = INT16_MAX;
	patch_t *patch = NULL;

	va_start (ap, format);
	vsnprintf(buffer, sizeof buffer, format, ap);
	va_end   (ap);
	buffer[sizeof buffer - 1] = '\0';

	lump = W_CheckNumForLongName(buffer);

	if (lump == LUMPERROR)
	{
		if (required == true)
			*user = W_CachePatchName("MISSING", PU_HUDGFX);
		return *user;
	}

	patch = W_CachePatchNum(lump, PU_HUDGFX);

	if (user)
	{
		*user = patch;
	}

	return patch;
}
#define HU_UpdatePatch(user, ...) HU_UpdateOrBlankPatch(user, true, __VA_ARGS__)

static void HUD_LoadButtonGraphics(patch_t *kp[2][2], const char *code)
{
	HU_UpdatePatch(&kp[0][0], "TLB_%s", code);
	HU_UpdatePatch(&kp[0][1], "TLB_%sB", code);
	HU_UpdatePatch(&kp[1][0], "TLBS%s", code);
	HU_UpdatePatch(&kp[1][1], "TLBS%sB", code);
}

//
// void TSoURDt3rd_ST_LoadGraphics(void)
// Loads unique TSoURDt3rd graphics.
//
void TSoURDt3rd_ST_LoadGraphics(void)
{
	HUD_LoadGraphicsSingular("SS_LVEGG", &tsourdt3rd_easter_leveleggs, PU_HUDGFX);
	HUD_LoadGraphicsSingular("SS_TLEGG", &tsourdt3rd_easter_totaleggs, PU_HUDGFX);

#ifdef HAVE_DISCORDSUPPORT
	// Discord Rich Presence
	HUD_LoadGraphicsSingular("K_REQUES", &envelope, PU_HUDGFX);
#endif

	// HU (hu_stuff.c)
	HUD_LoadButtonGraphics(kp_button_a, "A");
	HUD_LoadButtonGraphics(kp_button_b, "B");
	HUD_LoadButtonGraphics(kp_button_x, "X");

	HUD_LoadGraphicsSingular("TLK_L", &gen_button_keyleft[0], PU_HUDGFX);
	HUD_LoadGraphicsSingular("TLK_LB", &gen_button_keyleft[1], PU_HUDGFX);
	HUD_LoadGraphicsSingular("TLK_R", &gen_button_keyright[0], PU_HUDGFX);
	HUD_LoadGraphicsSingular("TLK_RB", &gen_button_keyright[1], PU_HUDGFX);
	HUD_LoadGraphicsSingular("TLK_M", &gen_button_keycenter[0], PU_HUDGFX);
	HUD_LoadGraphicsSingular("TLK_MB", &gen_button_keycenter[1], PU_HUDGFX);
}

#ifdef HAVE_DISCORDSUPPORT
void TSoURDt3rd_ST_AskToJoinEnvelope(void)
{
	const tic_t freq = TICRATE/2;

	if (menuactive)
		return;

	if ((leveltime % freq) < freq/2)
		return;

	V_DrawFixedPatch(296*FRACUNIT, 2*FRACUNIT, FRACUNIT, V_SNAPTOTOP|V_SNAPTORIGHT, envelope, NULL);
	// Ring Racers: maybe draw number of requests with V_DrawPingNum ?
}
#endif

//
// void TSoURDt3rd_ST_ObjectTracking(player_t *player, tsourdt3rd_trackingResult_t *result, const vector3_t *point, boolean object_reverse)
// Projects world objects to the screen.
//
// This version of the function was prototyped in Lua by Nev3r ... a HUGE thank you goes out to them!
// Plus, this *new* version of the function was added back to Lua, and THEN re-added back to C by me, StarManiaKG, so uh..... thank me too I guess?
//
// Based on K_ObjectTracking() from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_ST_ObjectTracking(player_t *player, tsourdt3rd_trackingResult_t *result, const vector3_t *point, boolean object_reverse)
{
	angle_t viewpointAngle, viewpointAiming, viewpointRoll;

	INT32 screenWidth, screenHeight;
	fixed_t screenHalfW, screenHalfH;

	const fixed_t baseFov = 90*FRACUNIT;
	fixed_t fovDiff, fov, fovTangent, fg;

	fixed_t h;
	INT32 da, dp;

	I_Assert(result != NULL);
	I_Assert(point != NULL);

	// Initialize defaults
	result->x = result->y = 0;
	result->scale = FRACUNIT;
	result->onScreen = false;

	// Take the view's properties as necessary.
	if (object_reverse)
	{
		viewpointAngle = (INT32)(viewangle + ANGLE_180);
		viewpointAiming = (INT32)InvAngle(aimingangle);
		viewpointRoll = (INT32)player->viewrollangle;
	}
	else
	{
		viewpointAngle = (INT32)viewangle;
		viewpointAiming = (INT32)aimingangle;
		viewpointRoll = (INT32)InvAngle(player->viewrollangle);
	}

	// Calculate screen size adjustments.
	screenWidth = (vid.width/vid.dup);
	screenHeight = (vid.height/vid.dup);

	if (splitscreen)
	{
		// Half-tall screens
		screenHeight >>= 1;
	}

	screenHalfW = (screenWidth >> 1) << FRACBITS;
	screenHalfH = (screenHeight >> 1) << FRACBITS;

	// Calculate FOV adjustments.
	fovDiff = cv_fov.value - baseFov;
	fov = ((baseFov - fovDiff) / 2) - (player->fovadd / 2);
	fovTangent = LUA_TAN(FixedAngle(fov));

	if (splitscreen)
	{
		// Splitscreen FOV is adjusted to maintain expected vertical view
		fovTangent = 10*fovTangent/17;
	}

	fg = (screenWidth >> 1) * fovTangent;

	// Determine viewpoint factors.
	h = R_PointToDist2(point->x, point->y, viewx, viewy);
	da = AngleDeltaSigned(viewpointAngle, R_PointToAngle2(viewx, viewy, point->x, point->y));
	dp = AngleDeltaSigned(viewpointAiming, R_PointToAngle2(0, 0, h, viewz));

	if (object_reverse)
	{
		da = -(da);
	}

	// Set results relative to top left!
	result->x = FixedMul(LUA_TAN(da), fg);
	result->y = FixedMul((LUA_TAN(viewpointAiming) - FixedDiv((point->z - viewz), 1 + FixedMul(LUA_COS(da), h))), fg);

	result->angle = da;
	result->pitch = dp;
	result->fov = fg;

	// Adjust for GL Perspective correction
	if (rendermode == render_opengl)
	{
		result->x /= 16;
	}

	// Rotate for screen roll...
	if (viewpointRoll)
	{
		fixed_t tempx = result->x;
		viewpointRoll >>= ANGLETOFINESHIFT;
		result->x = FixedMul(FINECOSINE(viewpointRoll), tempx) - FixedMul(FINESINE(viewpointRoll), result->y);
		result->y = FixedMul(FINESINE(viewpointRoll), tempx) + FixedMul(FINECOSINE(viewpointRoll), result->y);
	}

	// Flipped screen?
	if ((P_MobjFlip(player->mo) == -1) && CV_FindVar(va("flipcam%s", (player == &players[secondarydisplayplayer] ? "2" : ""))))
	{
		result->x = -result->x;
	}

	// Center results.
	result->x += screenHalfW;
	result->y += screenHalfH;
	result->scale = FixedDiv(screenHalfW, h+1);
	result->onScreen = !((abs(da) > ANG60) || (abs(AngleDeltaSigned(viewpointAiming, R_PointToAngle2(0, 0, h, (viewz - point->z)))) > ANGLE_45));

	// Cheap dirty hacks for some split-screen related cases
	if (result->x < 0 || result->x > (screenWidth << FRACBITS))
	{
		result->onScreen = false;
	}
	if (result->y < 0 || result->y > (screenHeight << FRACBITS))
	{
		result->onScreen = false;
	}

	// adjust to non-green-resolution screen coordinates
	result->x -= ((vid.width/vid.dup) - BASEVIDWIDTH)<<(FRACBITS-1);//((r_splitscreen >= 2) ? 2 : 1));
	result->y -= ((vid.height/vid.dup) - BASEVIDHEIGHT)<<(FRACBITS-(splitscreen ? 2 : 1));
}
