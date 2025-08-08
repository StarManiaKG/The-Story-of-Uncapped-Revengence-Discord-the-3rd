// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
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

// ------------------------ //
//        Variables
// ------------------------ //

#define LUA_TAN(x) FINETANGENT(((x + ANGLE_90) >> ANGLETOFINESHIFT) & 4095) // tan function used by Lua
#define LUA_COS(x) FINECOSINE((x >> ANGLETOFINESHIFT) & FINEMASK)

//{ 	Patch Definitions
patch_t *kp_button_a[2][2];
patch_t *kp_button_b[2][2];
patch_t *kp_button_x[2][2];

patch_t *tsourdt3rd_easter_leveleggs;
patch_t *tsourdt3rd_easter_totaleggs;

#ifdef HAVE_DISCORDSUPPORT
// Discord Rich Presence
static patch_t *envelope;
#endif

static tic_t tps_lasttic;
static boolean tps_ticgraph[TICRATE];
tic_t tps_totaltics;

// ------------------------ //
//        Functions
// ------------------------ //

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
		curpatch_name = va("%s%c%c", name, letter, cur_patch_num);
		if (W_CheckNumForName(curpatch_name) == LUMPERROR)
			patch[i] = (patch_t *)W_CachePatchName("MISSING", PU_HUDGFX);
		else
			patch[i] = (patch_t *)W_CachePatchName(curpatch_name, type);
	}
}

//
// void TSoURDt3rd_ST_LoadGraphics(void)
// Loads unique TSoURDt3rd graphics.
//
void TSoURDt3rd_ST_LoadGraphics(void)
{
	HUD_LoadGraphics("SS_LVEGG", &tsourdt3rd_easter_leveleggs, 1, '\0', PU_HUDGFX);
	HUD_LoadGraphics("SS_TLEGG", &tsourdt3rd_easter_totaleggs, 1, '\0', PU_HUDGFX);

#ifdef HAVE_DISCORDSUPPORT
	// Discord Rich Presence
	HUD_LoadGraphics("K_REQUES", &envelope, 1, '\0', PU_HUDGFX);
#endif

	HUD_LoadGraphics("TLB_", kp_button_a[0], 2, 'A', PU_HUDGFX);
	HUD_LoadGraphics("TLB_", kp_button_a[1], 2, 'N', PU_HUDGFX);
	HUD_LoadGraphics("TLB_", kp_button_b[0], 2, 'B', PU_HUDGFX);
	HUD_LoadGraphics("TLB_", kp_button_b[1], 2, 'O', PU_HUDGFX);
	HUD_LoadGraphics("TLB_", kp_button_x[0], 2, 'D', PU_HUDGFX);
	HUD_LoadGraphics("TLB_", kp_button_x[1], 2, 'Q', PU_HUDGFX);
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

void TSoURDt3rd_SCR_CalculateTPS(void)
{
	tic_t i;
	tic_t ontic = 0;

	if (netgame && client)
	{
		// This line of code is from LuigiBudd's SRB2Edit client, it looked cool
		ontic = (gametic + (neededtic - gametic));
	}
	else
		ontic = I_GetTime();
	tps_totaltics = 0;

	for (i = tps_lasttic + 1; (i < (TICRATE+tps_lasttic) && i < ontic); i++)
		tps_ticgraph[i % TICRATE] = false;
	tps_ticgraph[ontic % TICRATE] = true;

	for (i = 0; i < TICRATE; ++i)
		if (tps_ticgraph[i]) ++tps_totaltics;

	tps_lasttic = ontic;
}

typedef struct drawingData_s {
	INT32 x; INT32 y;
	INT32 option;
	const char *string;
	fontdef_t *font;
} drawingData_t;

void TSoURDt3rd_SCR_DisplayTPS(void)
{
	/// \todo custom font support

#if 1
	INT32 tpscntcolor = 0;
	INT32 tpswidth = 0;
	const char *drawntpsStr = NULL;
#endif

	INT32 x, y = (vid.height-(8*vid.dup));
	INT32 option;
	fixed_t pscale = (FRACUNIT/2), vscale = (FRACUNIT/2);
	const char *string = NULL;
	fontdef_t *font = NULL;

#if 0
	#define V_DrawSmallThinString(x,y,o,str) V_DrawFontString(x,y,o,FRACUNIT/2,FRACUNIT/2,str,tny_font)
	#define V_DrawCenteredSmallThinString(x,y,o,str) V_DrawAlignedFontString(x,y,o,FRACUNIT/2,FRACUNIT/2,str,tny_font,aligncenter)
	#define V_DrawRightAlignedSmallThinString(x,y,o,str) V_DrawAlignedFontString(x,y,o,FRACUNIT/2,FRACUNIT/2,str,tny_font,alignright)
#endif

	if (gamestate == GS_NULL)
	{
		// We're not supposed to see anything, so what's the point?
		return;
	}

	if (tps_totaltics <= TICRATE/2) tpscntcolor = V_REDMAP;
	else if (tps_totaltics <= TICRATE-8) tpscntcolor = V_YELLOWMAP;
	else tpscntcolor = V_TPSCOLORMAP;

	drawntpsStr = va("%02d/%02u", tps_totaltics, TICRATE);
	//string

	// Calculate the right HUD size for our strings
	switch (cv_tsourdt3rd_video_font_tps.value)
	{
		case 1: // thin
			option = (tpscntcolor|V_NOSCALESTART|V_SNAPTOBOTTOM|V_SNAPTORIGHT|V_USERHUDTRANS);
			font = &hu_font;
			break;
		default: // normal
			option = (tpscntcolor|V_NOSCALESTART|V_SNAPTOBOTTOM|V_SNAPTORIGHT|V_USERHUDTRANS);
			font = &tny_font;
			break;
	}

	switch (cv_tsourdt3rd_video_showtps.value)
	{
		case 1: // full counter
			tpswidth = V_StringWidth(drawntpsStr, V_NOSCALESTART);
			V_DrawString(vid.width - ((7 * 8 * vid.dup) + V_StringWidth("TPS:", V_NOSCALESTART)), y-(8*vid.dup),
				V_MENUCOLORMAP|V_NOSCALESTART|V_USERHUDTRANS, "TPS:");
			V_DrawString(vid.width - tpswidth, y-(8*vid.dup),
				tpscntcolor|V_NOSCALESTART|V_USERHUDTRANS, drawntpsStr);
			break;
		case 3: // kart-style counter
			tpswidth = V_ThinStringWidth(drawntpsStr, V_NOSCALESTART);
			V_DrawThinString(vid.width - V_ThinStringWidth("TPS:", V_NOSCALESTART), y-(18*vid.dup),
				V_MENUCOLORMAP|V_NOSCALESTART|V_SNAPTOBOTTOM|V_SNAPTORIGHT|V_USERHUDTRANS, "TPS:");
			V_DrawThinString(vid.width - tpswidth, y-(10*vid.dup),
				tpscntcolor|V_NOSCALESTART|V_SNAPTOBOTTOM|V_SNAPTORIGHT|V_USERHUDTRANS, drawntpsStr);
			break;
		default: // compact counter
			V_DrawRightAlignedString(vid.width, y-(8*vid.dup),
				tpscntcolor|V_NOSCALESTART|V_USERHUDTRANS, va("%02d", tps_totaltics));
			break;
	}

#if 0
	V_DrawFontString();
#endif
}

INT32 TSoURDt3rd_SCR_SetPingHeight(INT32 *y)
{
	if ((cv_ticrate.value && cv_tsourdt3rd_video_showtps.value) || cv_tsourdt3rd_video_showtps.value)
		(*y) = 171;
	else if (!cv_ticrate.value)
		(*y) = 189;
	else
		(*y) = 180;
	return (*y);
}

//
// void TSoURDt3rd_ST_ObjectTracking(player_t *player, tsourdt3rd_trackingResult_t *result, const vector3_t *point, boolean object_reverse)
// Projects world objects to the screen.
//
// This version of the function was prototyped in Lua by Nev3r ... a HUGE thank you goes out to them!
// Plus, this *new* version of the function was added back to Lua, and THEN re-added back to C by me, StarManiaKG, so uh..... thank me too I guess?
//
// Based on K_ObjectTracking() from Dr.Robotnik's Ring Racers!
//
static INT32 AngleDeltaSigned(angle_t a1, angle_t a2)
{
	// Silly but easy way to do it through integer conversion.
	return (INT32)(a1) - (INT32)(a2);
}

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
