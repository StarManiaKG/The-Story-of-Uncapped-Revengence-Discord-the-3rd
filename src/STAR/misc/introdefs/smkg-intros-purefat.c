// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-intros-purefat.c
/// \brief Pure Fat intro (from SRB2 versions pre-2.2.7)

#include "../smkg-m_intro.h"

#include "../../../s_sound.h"
#include "../../../w_wad.h"
#include "../../../z_zone.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// boolean STAR_F_PureFatDrawer(char *stjrintro, patch_t *background, void *patch, INT32 intro_scenenum, INT32 bgxoffs)
// Draws the Pure Fat intro.
//
boolean STAR_F_PureFatDrawer(char *stjrintro, patch_t *background, void *patch, INT32 intro_scenenum, INT32 bgxoffs)
{
	if (intro_scenenum == 0)
	{
		strncpy(stjrintro, "STJRI029", 9); // Move the Frames of the Graphic Along While The Pure Fat is Fattening, So The Graphic Is At It's Final Frame When Shown

		if (finalecount-TICRATE/2 < 4*TICRATE+23) {
			// aspect is FRACUNIT/2 for 4:3 (source) resolutions, smaller for 16:10 (SRB2) resolutions
			fixed_t aspect = (FRACUNIT + (FRACUNIT*4/3 - FRACUNIT*vid.width/vid.height)/2)>>1;
			fixed_t x,y;
			V_DrawFill(0, 0, BASEVIDWIDTH, BASEVIDHEIGHT, 2);
			if (finalecount < 30) { // Cry!
				if (finalecount < 4)
					S_StopMusic();
				if (finalecount == 4)
					S_ChangeMusicInternal("_pure", false);
				x = (BASEVIDWIDTH<<FRACBITS)/2 - FixedMul(334<<FRACBITS, aspect)/2;
				y = (BASEVIDHEIGHT<<FRACBITS)/2 - FixedMul(358<<FRACBITS, aspect)/2;
				V_DrawSciencePatch(x, y, 0, (patch = W_CachePatchName("WAHH1", PU_PATCH)), aspect);
				W_UnlockCachedPatch(patch);
				if (finalecount > 6) {
					V_DrawSciencePatch(x, y, 0, (patch = W_CachePatchName("WAHH2", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
				}
				if (finalecount > 10) {
					V_DrawSciencePatch(x, y, 0, (patch = W_CachePatchName("WAHH3", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
				}
				if (finalecount > 14) {
					V_DrawSciencePatch(x, y, 0, (patch = W_CachePatchName("WAHH4", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
				}
			}
			else if (finalecount-30 < 20) { // Big eggy
				background = W_CachePatchName("FEEDIN", PU_PATCH);
				x = (BASEVIDWIDTH<<FRACBITS)/2 - FixedMul(560<<FRACBITS, aspect)/2;
				y = (BASEVIDHEIGHT<<FRACBITS) - FixedMul(477<<FRACBITS, aspect);
				V_DrawSciencePatch(x, y, V_SNAPTOBOTTOM, background, aspect);
			}
			else if (finalecount-50 < 30) { // Zoom out
				fixed_t scale = FixedDiv(aspect, FixedDiv((finalecount-50)<<FRACBITS, (15<<FRACBITS))+FRACUNIT);
				background = W_CachePatchName("FEEDIN", PU_PATCH);
				x = (BASEVIDWIDTH<<FRACBITS)/2 - FixedMul(560<<FRACBITS, aspect)/2 + (FixedMul(560<<FRACBITS, aspect) - FixedMul(560<<FRACBITS, scale));
				y = (BASEVIDHEIGHT<<FRACBITS) - FixedMul(477<<FRACBITS, scale);
				V_DrawSciencePatch(x, y, V_SNAPTOBOTTOM, background, scale);
			}
			else
			{
				{
					// Draw tiny eggy
					fixed_t scale = FixedMul(FRACUNIT/3, aspect);
					background = W_CachePatchName("FEEDIN", PU_PATCH);
					x = (BASEVIDWIDTH<<FRACBITS)/2 - FixedMul(560<<FRACBITS, aspect)/2 + (FixedMul(560<<FRACBITS, aspect) - FixedMul(560<<FRACBITS, scale));
					y = (BASEVIDHEIGHT<<FRACBITS) - FixedMul(477<<FRACBITS, scale);
					V_DrawSciencePatch(x, y, V_SNAPTOBOTTOM, background, scale);
				}

				if (finalecount-84 < 58) { // Pure Fat is driving up!
					int ftime = (finalecount-84);
					x = (-189*FRACUNIT) + (FixedMul((6<<FRACBITS)+FRACUNIT/3, ftime<<FRACBITS) - FixedMul((6<<FRACBITS)+FRACUNIT/3, FixedDiv(FixedMul(ftime<<FRACBITS, ftime<<FRACBITS), 120<<FRACBITS)));
					y = (BASEVIDHEIGHT<<FRACBITS) - FixedMul(417<<FRACBITS, aspect);
					// Draw the body
					V_DrawSciencePatch(x, y, V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("PUREFAT1", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
					// Draw the door
					V_DrawSciencePatch(x+FixedMul(344<<FRACBITS, aspect), y+FixedMul(292<<FRACBITS, aspect), V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("PUREFAT2", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
					// Draw the wheel
					V_DrawSciencePatch(x+FixedMul(178<<FRACBITS, aspect), y+FixedMul(344<<FRACBITS, aspect), V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName(va("TYRE%02u",(abs(finalecount-144)/3)%16), PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
					// Draw the wheel cover
					V_DrawSciencePatch(x+FixedMul(88<<FRACBITS, aspect), y+FixedMul(238<<FRACBITS, aspect), V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("PUREFAT3", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
				} else { // Pure Fat has stopped!
					y = (BASEVIDHEIGHT<<FRACBITS) - FixedMul(417<<FRACBITS, aspect);
					// Draw the body
					V_DrawSciencePatch(0, y, V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("PUREFAT1", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
					// Draw the wheel
					V_DrawSciencePatch(FixedMul(178<<FRACBITS, aspect), y+FixedMul(344<<FRACBITS, aspect), V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("TYRE00", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
					// Draw the wheel cover
					V_DrawSciencePatch(FixedMul(88<<FRACBITS, aspect), y+FixedMul(238<<FRACBITS, aspect), V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("PUREFAT3", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
					// Draw the door
					if (finalecount-TICRATE/2 > 4*TICRATE) { // Door is being raised!
						int ftime = (finalecount-TICRATE/2-4*TICRATE);
						y -= FixedDiv((ftime*ftime)<<FRACBITS, 23<<FRACBITS);
					}
					V_DrawSciencePatch(FixedMul(344<<FRACBITS, aspect), y+FixedMul(292<<FRACBITS, aspect), V_SNAPTOLEFT|V_SNAPTOBOTTOM, (patch = W_CachePatchName("PUREFAT2", PU_PATCH)), aspect);
					W_UnlockCachedPatch(patch);
				}
			}
		}
		else
		{
			background = W_CachePatchName(stjrintro, PU_PATCH_LOWPRIORITY);
			V_DrawSmallScaledPatch(bgxoffs, 84, 0, background);
		}
		return true;
	}
	return false;
}

//
// boolean STAR_F_PureFatTicker(INT32 intro_scenenum, INT32 intro_curtime, INT32 animtimer, INT32 next_time)
// Tick routine for the Pure Fat intro.
//
boolean STAR_F_PureFatTicker(INT32 intro_scenenum, INT32 intro_curtime, INT32 animtimer, INT32 next_time)
{
	INT32 wuh = intro_curtime;

	if ((intro_scenenum == 5 && intro_curtime == 5*TICRATE)
		|| (intro_scenenum == 7 && intro_curtime == 6*TICRATE)
		//|| (intro_scenenum == 11 && intro_curtime == 7*TICRATE)
		|| (intro_scenenum == 15 && intro_curtime == 7*TICRATE))
	{
		F_WipeStartScreen();
		F_WipeColorFill(31);

		F_IntroDrawer();

		F_WipeEndScreen();
		F_RunWipe(99,true);
	}

	if (animtimer)
		animtimer--;

	wuh = (7*TICRATE + (TICRATE/2)) - next_time;
	return (intro_scenenum == 0 && (wuh < ((7*TICRATE + (TICRATE/2)))));
}
