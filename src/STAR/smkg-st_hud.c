// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-st_hud.c
/// \brief TSoURDt3rd cool HUD data

#include "smkg-st_hud.h"
#include "star_vars.h"

#include "../d_main.h"
#include "../i_time.h"
#include "../lua_hud.h"
#include "../r_main.h"
#include "../v_video.h"
#include "../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

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

// ------------------------ //
//        Functions
// ------------------------ //

//
// static void TSoURDt3rd_HUD_LoadGraphics(const char *name, patch_t *patch, int patches, int letter, int type)
// Loads the graphics into the given struct based on the given patch properties.
//
static void TSoURDt3rd_HUD_LoadGraphics(const char *name, patch_t **patch, int patches, int letter, int type)
{
	int i = 0, cur_patch_num = 65;
	const char *curpatch_name;

	for (i = 0; i < patches; i++)
	{
		cur_patch_num = (i+65 == 65 ? '\0' : i+65);
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
	TSoURDt3rd_HUD_LoadGraphics("SS_LVEGG", &tsourdt3rd_easter_leveleggs, 1, '\0', PU_HUDGFX);
	TSoURDt3rd_HUD_LoadGraphics("SS_TLEGG", &tsourdt3rd_easter_totaleggs, 1, '\0', PU_HUDGFX);

#ifdef HAVE_DISCORDSUPPORT
	// Discord Rich Presence
	TSoURDt3rd_HUD_LoadGraphics("K_REQUES", &envelope, 1, '\0', PU_HUDGFX);
#endif

	TSoURDt3rd_HUD_LoadGraphics("TLB_", kp_button_a[0], 2, 'A', PU_HUDGFX);
	TSoURDt3rd_HUD_LoadGraphics("TLB_", kp_button_a[1], 2, 'N', PU_HUDGFX);
	TSoURDt3rd_HUD_LoadGraphics("TLB_", kp_button_b[0], 2, 'B', PU_HUDGFX);
	TSoURDt3rd_HUD_LoadGraphics("TLB_", kp_button_b[1], 2, 'O', PU_HUDGFX);
	TSoURDt3rd_HUD_LoadGraphics("TLB_", kp_button_x[0], 2, 'D', PU_HUDGFX);
	TSoURDt3rd_HUD_LoadGraphics("TLB_", kp_button_x[1], 2, 'Q', PU_HUDGFX);
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
	// maybe draw number of requests with V_DrawPingNum ?
}
#endif

void TSoURDt3rd_SCR_DisplayTpsRate(void)
{
	static tic_t lasttic;
	static boolean ticsgraph[TICRATE];

	INT32 tpscntcolor = 0;
	const INT32 h = vid.height-(8*vid.dupy);

	tic_t i;
	tic_t ontic = I_GetTime();
	tic_t totaltics = 0;

	if (gamestate == GS_NULL)
		return;

	for (i = lasttic + 1; i < TICRATE+lasttic && i < ontic; ++i)
		ticsgraph[i % TICRATE] = false;
	ticsgraph[ontic % TICRATE] = true;

	for (i = 0; i < TICRATE; ++i)
		if (ticsgraph[i])
			++totaltics;

	if (totaltics <= TICRATE/2) tpscntcolor = V_REDMAP;
	else if (totaltics <= TICRATE-8) tpscntcolor = V_YELLOWMAP;
	else tpscntcolor = V_TPSCOLORMAP;

	if (cv_tsourdt3rd_video_showtps.value == 2) // compact counter
		V_DrawRightAlignedString(vid.width, h-(8*vid.dupy),
			tpscntcolor|V_NOSCALESTART|V_USERHUDTRANS, va("%02d", totaltics));
	else if (cv_tsourdt3rd_video_showtps.value == 1) // full counter
	{
		const char *drawntpsStr;
		INT32 tpswidth;

		drawntpsStr = va("%02d/ %02u", totaltics, TICRATE);
		tpswidth = V_StringWidth(drawntpsStr, V_NOSCALESTART);

		V_DrawString(vid.width - ((7 * 8 * vid.dupx) + V_StringWidth("TPS: ", V_NOSCALESTART)), h-(8*vid.dupy),
			V_MENUCOLORMAP|V_NOSCALESTART|V_USERHUDTRANS, "TPS:");
		V_DrawString(vid.width - tpswidth, h-(8*vid.dupy),
			tpscntcolor|V_NOSCALESTART|V_USERHUDTRANS, drawntpsStr);
	}

	lasttic = ontic;
}

INT32 TSoURDt3rd_SCR_SetPingHeight(void)
{
	INT32 pingy;

	if ((cv_ticrate.value && cv_tsourdt3rd_video_showtps.value) || cv_tsourdt3rd_video_showtps.value)
		pingy = 171;
	else if (!cv_ticrate.value)
		pingy = 189;
	else
		pingy = 180;

	return pingy;
}
