// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2015 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file hw_light.c
/// \brief Corona/Dynamic/Static lighting add on by Hurdler
///	!!! Under construction !!!

#include "../doomdef.h"

#ifdef HWRENDER
#include "hw_light.h"
#include "hw_drv.h"
#include "hw_batching.h"
#include "../i_video.h"
#include "../z_zone.h"
#include "../m_random.h"
#include "../m_bbox.h"
#include "../w_wad.h"
#include "../r_state.h"
#include "../r_main.h"
#include "../p_local.h"
#include "../r_fps.h"

// TSoURDt3rd
#include "../STAR/lights/smkg-coronas.h"
#include "../STAR/smkg-cvars.h"

//=============================================================================
//                                                                      DEFINES
//=============================================================================

// [WDJ] Makes debugging difficult when references like these are hidden.
// Make them visible in the code.
//#define CORONA_DEBUG

#ifdef CORONA_DEBUG
#define DL_SQRRADIUS(x)     dynlights->p_lspr[(x)]->dynamic_sqrradius
#define DL_RADIUS(x)        dynlights->p_lspr[(x)]->dynamic_radius
#define LIGHT_POS(i)        dynlights->position[(i)]
#endif

#define DL_HIGH_QUALITY
//#define DYN_LIGHT_VERTEX
//#define STATICLIGHTMAPS // Hurdler: TODO!

#ifdef CORONA_DEBUG
#define LIGHTMAPFLAGS (PF_Masked|PF_NoAlphaTest) // debug see overdraw
#else
#define LIGHTMAPFLAGS (PF_Modulated|PF_Additive)
#endif

// Proportional fade of corona from Z1 to Z2
#define Z1 (250.0f)
#define Z2 ((255.0f*8) + 250.0f)

//=============================================================================
//                                                                       GLOBAL
//=============================================================================

// Select by view, using indirection into view_dynlights.
static dynlights_t view_dynlights[2]; // 2 players in splitscreen mode
static dynlights_t *dynlights = &view_dynlights[0];

static patch_t *lightmap_patch;
static GLPatch_t *lightmap_gl_patch;
static lumpnum_t corona_lumpnum = LUMPERROR;

//=============================================================================
//                                                                       PROTOS
//=============================================================================

// -----------------+
// HWR_SetLight     : Download a disc shaped alpha map for rendering fake lights
// -----------------+
static void HWR_SetLight(void)
{
	if (lightmap_gl_patch == NULL || lightmap_gl_patch->mipmap == NULL) return;
	if (!lightmap_gl_patch->mipmap->downloaded && !lightmap_gl_patch->mipmap->data)
	{
		UINT16 *data = Z_Malloc(129*128*sizeof(UINT16), PU_HWRCACHE, lightmap_gl_patch->mipmap->data);
		for (int i = 0; i < 128; i++)
		{
			for (int j = 0; j < 128; j++)
			{
				int pos = ((i-64)*(i-64))+((j-64)*(j-64));
				if (pos <= 63*63)
					data[i*128+j] = (UINT16)(((UINT8)(255-(4*(float)sqrt((float)pos)))) << 8 | 0xff);
				else
					data[i*128+j] = 0;
			}
		}
		lightmap_gl_patch->mipmap->format = GL_TEXFMT_ALPHA_INTENSITY_88;
		lightmap_patch->width = 128;
		lightmap_patch->height = 128;
		lightmap_gl_patch->mipmap->width = 128;
		lightmap_gl_patch->mipmap->height = 128;
		lightmap_gl_patch->mipmap->flags = 0; //TF_WRAPXY; // DEBUG: view the overdraw !
		HWD.pfnSetTexture(lightmap_gl_patch->mipmap);
	}
	HWR_SetCurrentTexture(lightmap_gl_patch->mipmap);
	Z_ChangeTag(lightmap_gl_patch->mipmap->data, PU_HWRCACHE_UNLOCKED); // The system-memory data can be purged now.
}

// --------------------------------------------------------------------------
// calcul la projection d'un point sur une droite (determin�e par deux
// points) et ensuite calcul la distance (au carr�) de ce point au point
// project� sur cette droite
// --------------------------------------------------------------------------
static float HWR_DistP2D(FOutVector *p1, FOutVector *p2, FVector *p3, FVector *inter)
{
	if (p1->z >= p2->z && p2->z <= p1->z)
	{
		inter->x = p3->x;
		inter->z = p1->z;
	}
	else if (p1->x >= p2->x && p2->x <= p1->x)
	{
		inter->x = p1->x;
		inter->z = p3->z;
	}
	else
	{
		register float local, pente;
		// Wat een mooie formula! Hurdler's math ;-)
		pente = (p1->z - p2->z) / (p1->x - p2->x);
		local = p1->z - p1->x*pente;
		inter->x = (p3->z - local + p3->x/pente) * (pente/(pente*pente+1));
		inter->z = inter->x*pente + local;
	}

	return (p3->x-inter->x)*(p3->x-inter->x) + (p3->z-inter->z)*(p3->z-inter->z);
}

// check if sphere (radius r) centred in p3 touch the bounding box defined by p1, p2
static boolean SphereTouchBBox3D(FOutVector *p1, FOutVector *p2, FVector *p3, float r)
{
	float minx = p1->x, maxx = p2->x;
	float miny = p2->y, maxy = p1->y;
	float minz = p2->z, maxz = p1->z;

	if (minx > maxx)
	{
		minx = maxx;
		maxx = p1->x;
	}
	if (miny > maxy)
	{
		miny = maxy;
		maxy = p2->y;
	}
	if (minz > maxz)
	{
		minz = maxz;
		maxz = p2->z;
	}

	if (minx - r > p3->x) return false;
	if (maxx + r < p3->x) return false;
	if (miny - r > p3->y) return false;
	if (maxy + r < p3->y) return false;
	if (minz - r > p3->z) return false;
	if (maxz + r < p3->z) return false;
	return true;
}

// --------------------------------------------------------------------------
// Calculation of dynamic lighting on walls
// Coords lVerts contains the wall with mlook transformed
// --------------------------------------------------------------------------
void HWR_SpriteLighting(FOutVector *wlVerts) // SRB2CBTODO: Support sprites to be lit too
{
	FVector                     inter;
	FSurfaceInfo                 Surf;
	light_t     	          *p_lspr; // dynlights sprite light
	mobj_t                 *lspr_mobj;
	FVector                *light_pos;
	float           dist_p2d, d[4], s;
    unsigned int                 i, j;

#ifndef STAR_LIGHTING
	if (!cv_glcoronas.value || !cv_glstaticlighting.value)
		return;
#else
	if (!cv_tsourdt3rd_video_lighting_coronas.value || cv_tsourdt3rd_video_lighting_coronas_lightingtype.value)
		return;
#endif

	// dynlights->nb == 0 if dynamic lighting is off
	for (j = 0; j < dynlights->nb; j++)
	{
		p_lspr = dynlights->p_lspr[j];
		lspr_mobj = dynlights->mo[j];
		light_pos = &dynlights->position[j];

		// it's a real object which emits light
		if (!lspr_mobj)
			continue;
		if (P_MobjWasRemoved(lspr_mobj) || !lspr_mobj->state)
		{
			P_SetTarget(&lspr_mobj, NULL);
			continue;
		}
		if (p_lspr->corona_rendering_routine && !p_lspr->corona_rendering_routine(lspr_mobj))
			continue;

		// check bounding box first
		if (!(SphereTouchBBox3D(&wlVerts[2], &wlVerts[0], light_pos, p_lspr->dynamic_radius)))
			continue;

		d[0] = wlVerts[2].x - wlVerts[0].x;
		d[1] = wlVerts[2].z - wlVerts[0].z;
		d[2] = dynlights->position[j].x - wlVerts[0].x;
		d[3] = dynlights->position[j].z - wlVerts[0].z;
		// backface cull
		//if (d[2]*d[1] - d[3]*d[0] < 0)
		//	continue;

		// check exact distance
		dist_p2d = HWR_DistP2D(&wlVerts[2], &wlVerts[0], light_pos, &inter);
		if (dist_p2d >= p_lspr->dynamic_sqrradius)
			continue;

		d[0] = (float)sqrt((wlVerts[0].x-inter.x)*(wlVerts[0].x-inter.x)
						   + (wlVerts[0].z-inter.z)*(wlVerts[0].z-inter.z));
		d[1] = (float)sqrt((wlVerts[2].x-inter.x)*(wlVerts[2].x-inter.x)
						   + (wlVerts[2].z-inter.z)*(wlVerts[2].z-inter.z));
		//dAB = (float)sqrt((wlVerts[0].x-wlVerts[2].x)*(wlVerts[0].x-wlVerts[2].x)+(wlVerts[0].z-wlVerts[2].z)*(wlVerts[0].z-wlVerts[2].z));
		//if ((d[0] < dAB) && (d[1] < dAB)) // test if the intersection is on the wall
		//{
		//    d[0] = -d[0]; // if yes, the left distcance must be negative for texcoord
		//}
		// test if the intersection is on the wall
		if ((wlVerts[0].x < inter.x && wlVerts[2].x > inter.x) ||
			(wlVerts[0].x > inter.x && wlVerts[2].x < inter.x) ||
			(wlVerts[0].z < inter.z && wlVerts[2].z > inter.z) ||
			(wlVerts[0].z > inter.z && wlVerts[2].z < inter.z))
		{
			d[0] = -d[0]; // if yes, the left distcance must be negative for texcoord
		}
		d[2] = d[1]; d[3] = d[0];

		s = 0.5f / p_lspr->dynamic_radius;

		for (i = 0; i < 4; i++)
		{
			wlVerts[i].s = (float)(0.5f + d[i]*s);
			wlVerts[i].t = (float)(0.5f + (wlVerts[i].y - light_pos->y)*s*1.2f);
		}

		HWR_SetLight();

		Surf.PolyColor.rgba = LONG(p_lspr->dynamic_color);

#ifdef DL_HIGH_QUALITY
		Surf.PolyColor.s.alpha *= (1-dist_p2d/p_lspr->dynamic_sqrradius);
#endif

		// next state is null so fade out with alpha
		if (!lspr_mobj->state || lspr_mobj->state->nextstate == S_NULL)
			Surf.PolyColor.s.alpha *= (float)lspr_mobj->tics/(float)lspr_mobj->state->tics;

		HWD.pfnDrawPolygon(&Surf, wlVerts, 4, LIGHTMAPFLAGS);
	} // end for (j = 0; j < dynlights->nb; j++)
}

#if 0
// --------------------------------------------------------------------------
// Calculation of dynamic lighting on walls
// Coords lVerts contains the wall with mlook transformed
// --------------------------------------------------------------------------
static void HWR_SetShadowTexture(gl_vissprite_t *spr)
{
	GLPatch_t *gpatch; // sprite patch converted to hardware
	gpatch = (GLPatch_t *)W_CachePatchNum(W_GetNumForName(spr->gpatch), PU_CACHE);

	HWR_GetMappedPatch((patch_t *)gpatch, spr->colormap);
}

void HWR_WallShading(FOutVector *wlVerts)
{
	unsigned int i, j;

	// dynlights->nb == 0 if dynamic lighting is off
	for (j = 0; j < dynlights->nb; j++)
	{
		FVector         inter;
		FSurfaceInfo    Surf;
		float           dist_p2d, d[4], s;
		mobj_t *lspr_mobj = &dynlights->mo[j];

		fixed_t xvalue = FIXED_TO_FLOAT(lspr_mobj->x);
		fixed_t yvalue = FIXED_TO_FLOAT(lspr_mobj->y);
		fixed_t zvalue = FIXED_TO_FLOAT(lspr_mobj->z);

		// check bounding box first
		if (!(SphereTouchBBox3D(&wlVerts[2], &wlVerts[0], &SHADOW_POS(j), DS_RADIUS(j))))
			continue;
		d[0] = wlVerts[2].x - wlVerts[0].x;
		d[1] = wlVerts[2].z - wlVerts[0].z;
		d[2] = xvalue - wlVerts[0].x;
		d[3] = zvalue - wlVerts[0].z;
		// backface cull
		if (d[2]*d[1] - d[3]*d[0] < 0)
			continue;
		// check exact distance
		dist_p2d = HWR_DistP2D(&wlVerts[2], &wlVerts[0], &SHADOW_POS(j), &inter);
		if (dist_p2d >= DS_SQRRADIUS(j))
			continue;

		d[0] = (float)sqrt((wlVerts[0].x-inter.x)*(wlVerts[0].x-inter.x)
						   + (wlVerts[0].z-inter.z)*(wlVerts[0].z-inter.z));
		d[1] = (float)sqrt((wlVerts[2].x-inter.x)*(wlVerts[2].x-inter.x)
						   + (wlVerts[2].z-inter.z)*(wlVerts[2].z-inter.z));
		//dAB = (float)sqrt((wlVerts[0].x-wlVerts[2].x)*(wlVerts[0].x-wlVerts[2].x)+(wlVerts[0].z-wlVerts[2].z)*(wlVerts[0].z-wlVerts[2].z));
		//if ((d[0] < dAB) && (d[1] < dAB)) // test if the intersection is on the wall
		//{
		//    d[0] = -d[0]; // if yes, the left distcance must be negative for texcoord
		//}
		// test if the intersection is on the wall
		if ((wlVerts[0].x < inter.x && wlVerts[2].x > inter.x) ||
			(wlVerts[0].x > inter.x && wlVerts[2].x < inter.x) ||
			(wlVerts[0].z < inter.z && wlVerts[2].z > inter.z) ||
			(wlVerts[0].z > inter.z && wlVerts[2].z < inter.z))
		{
			d[0] = -d[0]; // if yes, the left distcance must be negative for texcoord
		}
		d[2] = d[1]; d[3] = d[0];

		s = 0.5f / DS_RADIUS(j);

		// SRB2CBTODO: Cut out a disk shape of texture to cast on the wall
		for (i = 0; i < 4; i++)
		{
			wlVerts[i].s = (float)(0.5f + d[i]*s);
			wlVerts[i].t = (float)(0.5f + (wlVerts[i].y-SHADOW_POS(j).y)*s*1.2f);
		}

		//HWR_SetShadowTexture(dynshadows->p_sspr[j]->spr);

		//Surf.PolyColor.rgba = LONG(dynshadows->p_sspr[j]->dynamic_color);
		// The normal color of a shadow
		Surf.PolyColor.s.red = 0x00;
		Surf.PolyColor.s.blue = 0x00;
		Surf.PolyColor.s.green = 0x00;

		Surf.PolyColor.s.alpha = (byte)((1-dist_p2d/DS_SQRRADIUS(j))*Surf.PolyColor.s.alpha);

		GL_DrawPolygon (&Surf, wlVerts, 4, PF_Modulated|PF_Decal|PF_Translucent, 0);

	} // end for (j = 0; j < dynshadows->nb; j++)
}
#endif

// Hurdler: The old code was removed by me because I don't think it will be used one day.
//          (It's still available on the CVS for educational purpose: Revision 1.8)

// --------------------------------------------------------------------------
// calcul du dynamic lighting sur les murs
// lVerts contient les coords du mur sans le mlook (up/down)
// --------------------------------------------------------------------------
void HWR_WallLighting(FOutVector *wlVerts, FBITFIELD PolyFlags, int shader)
{
#ifdef DYN_LIGHT_VERTEX
	FOutVector		   dlv[4];
#endif
	FSurfaceInfo         Surf;
	light_t           *p_lspr; // dynlights sprite light
	mobj_t         *lspr_mobj;
	FVector        *light_pos;
	FVector             inter;
	float   dist_p2d, d[4], s;
	unsigned int         i, j;

#ifdef DYN_LIGHT_VERTEX
	memcpy(dlv, wlVerts, sizeof(FOutVector)* 4);
#endif

	// dynlights->nb == 0 if dynamic lighting is off
	for (j = 0; j < dynlights->nb; j++)
	{
		p_lspr = dynlights->p_lspr[j];
		light_pos = &dynlights->position[j];
		lspr_mobj = dynlights->mo[j];

		// it's a real object which emits light
		if (!lspr_mobj)
			continue;
		if (P_MobjWasRemoved(lspr_mobj) || !lspr_mobj->state)
		{
			P_SetTarget(&lspr_mobj, NULL);
			continue;
		}
		if (p_lspr->corona_rendering_routine && !p_lspr->corona_rendering_routine(lspr_mobj))
			continue;

		// check bounding box first
		if (!SphereTouchBBox3D(&wlVerts[2], &wlVerts[0], light_pos, p_lspr->dynamic_radius))
			continue;
		d[0] = wlVerts[2].x - wlVerts[0].x;
		d[1] = wlVerts[2].z - wlVerts[0].z;
		d[2] = light_pos->x - wlVerts[0].x;
		d[3] = light_pos->z - wlVerts[0].z;

		// backface cull
		if (d[2]*d[1] - d[3]*d[0] < 0)
			continue;

		// check exact distance
		dist_p2d = HWR_DistP2D(&wlVerts[2], &wlVerts[0], light_pos, &inter);
		if (dist_p2d >= p_lspr->dynamic_sqrradius)
			continue;

		d[0] = (float)sqrt((wlVerts[0].x-inter.x)*(wlVerts[0].x-inter.x)
							+ (wlVerts[0].z-inter.z)*(wlVerts[0].z-inter.z));
		d[1] = (float)sqrt((wlVerts[2].x-inter.x)*(wlVerts[2].x-inter.x)
							+ (wlVerts[2].z-inter.z)*(wlVerts[2].z-inter.z));
		//dAB = sqrtf((wlVerts[0].x-wlVerts[2].x)*(wlVerts[0].x-wlVerts[2].x)+(wlVerts[0].z-wlVerts[2].z)*(wlVerts[0].z-wlVerts[2].z));
		//if ((d[0] < dAB) && (d[1] < dAB)) // test if the intersection is on the wall
		//{
		//    d[0] = -d[0]; // if yes, the left distance must be negative for texcoord
		//}
		// test if the intersection is on the wall
		if ((wlVerts[0].x < inter.x && wlVerts[2].x > inter.x) ||
			(wlVerts[0].x > inter.x && wlVerts[2].x < inter.x) ||
			(wlVerts[0].z < inter.z && wlVerts[2].z > inter.z) ||
			(wlVerts[0].z > inter.z && wlVerts[2].z < inter.z))
		{
			d[0] = -d[0]; // if yes, the left distance must be negative for texcoord
		}
		d[2] = d[1]; d[3] = d[0];

#ifdef DL_HIGH_QUALITY
		s = 0.5f / p_lspr->dynamic_radius;
#else
		s = 0.5f / sqrt(p_lspr->dynamic_sqrradius - dist_p2d);
#endif

		for (i = 0; i < 4; i++)
		{
#ifdef DYN_LIGHT_VERTEX
			dlv[i].s = (float)(0.5f + d[i]*s);
			dlv[i].t = (float)(0.5f + (wlVerts[i].y - light_pos->y)*s*1.2f);
#else
			wlVerts[i].s = (float)(0.5f + d[i]*s);
			wlVerts[i].t = (float)(0.5f + (wlVerts[i].y - light_pos->y)*s*1.2f);
#endif
		}

		HWR_SetLight();

		if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(lspr_mobj, &Surf.PolyColor, NULL, true))
			Surf.PolyColor.rgba = LONG(p_lspr->dynamic_color);
#ifdef DL_HIGH_QUALITY
		Surf.PolyColor.s.alpha *= (1-dist_p2d/p_lspr->dynamic_sqrradius);
#endif

		// next state is null so fade out with alpha
		if (!lspr_mobj->state || lspr_mobj->state->nextstate == S_NULL)
			Surf.PolyColor.s.alpha *= (float)lspr_mobj->tics/(float)lspr_mobj->state->tics;

#if 0
		HWR_GetPic(corona_lumpnum); // TODO: use different coronas
#endif

#ifdef DYN_LIGHT_VERTEX
		//HWD.pfnDrawPolygon(&Surf, dlv, 4, LIGHTMAPFLAGS);
		HWR_ProcessPolygon(&Surf, dlv, 4, LIGHTMAPFLAGS, shader, false);
#else
		//HWD.pfnDrawPolygon(&Surf, wlVerts, 4, LIGHTMAPFLAGS);
		HWR_ProcessPolygon(&Surf, wlVerts, 4, LIGHTMAPFLAGS, shader, false);
#endif
	} // end for (j = 0; j < dynlights->nb; j++)
}

// --------------------------------------------------------------------------
// calcul du dynamic lighting sur le sol
// clVerts contient les coords du sol avec le mlook (up/down)
// --------------------------------------------------------------------------
void HWR_PlaneLighting(FOutVector *clVerts, FUINT nrClipVerts, FBITFIELD PolyFlags, int shader, boolean horizonSpecial)
{
	FSurfaceInfo	       Surf;
	light_t     	    *p_lspr; // dynlights sprite light
	FVector          *light_pos;
	mobj_t           *lspr_mobj;
	float           dist_p2d, s;
	FOutVector     	      p1,p2;
	FUINT                     i;
	unsigned int              j;

	p1.z = FIXED_TO_FLOAT(hwbbox[BOXTOP   ]);
	p1.x = FIXED_TO_FLOAT(hwbbox[BOXLEFT  ]);
	p2.z = FIXED_TO_FLOAT(hwbbox[BOXBOTTOM]);
	p2.x = FIXED_TO_FLOAT(hwbbox[BOXRIGHT ]);
	p2.y = clVerts[0].y;
	p1.y = clVerts[0].y;

	for (j = 0; j < dynlights->nb; j++)
	{
		p_lspr = dynlights->p_lspr[j];
		light_pos = &dynlights->position[j];
		lspr_mobj = dynlights->mo[j];

		// it's a real object which emits light
		if (!lspr_mobj)
			continue;
		if (P_MobjWasRemoved(lspr_mobj) || !lspr_mobj->state)
		{
			P_SetTarget(&lspr_mobj, NULL);
			continue;
		}
		if (p_lspr->corona_rendering_routine && !p_lspr->corona_rendering_routine(lspr_mobj))
			continue;

		// BP: The kickass Optimization: check if light touch bounding box
		if (!SphereTouchBBox3D(&p1, &p2, light_pos, p_lspr->dynamic_radius))
			continue;

		// backface cull
		// Hurdler: doesn't work with new TANDL code
		if ((clVerts[0].y > atransform.z)   	// true mean it is a ceiling false is a floor
			^ (light_pos->y < clVerts[0].y))	// true mean light is down plane, false light is up plane
			continue;

		dist_p2d = (clVerts[0].y - light_pos->y);
		dist_p2d *= dist_p2d;
		// done in SphereTouchBBox3D
		//if (dist_p2d >= p_lspr->dynamic_sqrradius)
		//    continue;

#ifdef DL_HIGH_QUALITY
		s = 0.5f / p_lspr->dynamic_radius;
#else
		s = 0.5f / sqrt(p_lspr->dynamic_sqrradius - dist_p2d);
#endif

		for (i = 0; i < nrClipVerts; i++)
		{
			clVerts[i].s = 0.5f + (clVerts[i].x - light_pos->x)*s;
			clVerts[i].t = 0.5f + (clVerts[i].z - light_pos->z)*s*1.2f;
		}

		HWR_SetLight();

		if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(lspr_mobj, &Surf.PolyColor, NULL, true))
			Surf.PolyColor.rgba = LONG(p_lspr->dynamic_color);

#ifdef DL_HIGH_QUALITY
		// dist_p2d < lsp->dynamic_sqrradius
		Surf.PolyColor.s.alpha *= (1 - dist_p2d/p_lspr->dynamic_sqrradius);
#endif

		// next state is null so fade out with alpha
		if (!lspr_mobj->state || lspr_mobj->state->nextstate == S_NULL)
			Surf.PolyColor.s.alpha *= (float)lspr_mobj->tics/(float)lspr_mobj->state->tics;

#if 0
		HWR_GetPic(corona_lumpnum); // TODO: use different coronas
#endif

		//HWD.pfnDrawPolygon(&Surf, clVerts, nrClipVerts, LIGHTMAPFLAGS);
		HWR_ProcessPolygon(&Surf, clVerts, nrClipVerts, LIGHTMAPFLAGS, shader, horizonSpecial);
	} // end for (j = 0; j < dynlights->nb; j++)
}

// --------------------------------------------------------------------------
// coronas lighting with the sprite
// --------------------------------------------------------------------------
void HWR_DoCoronasLighting(FOutVector *outVerts, gl_vissprite_t *spr)
{
	float           cx = 0.0f, cy = 0.0f, cz = 0.0f; // gravity center
	float			size;
	FSurfaceInfo	Surf;
	FOutVector      light[4];
	light_t   		*p_lspr;
    unsigned int          i;

#ifndef STAR_LIGHTING
	if (!cv_glcoronas.value || !cv_glstaticlighting.value)
		return;
#else
	if (!cv_tsourdt3rd_video_lighting_coronas.value || cv_tsourdt3rd_video_lighting_coronas_lightingtype.value)
		return;
#endif

	p_lspr = Sprite_Corona_Light_lsp(spr->mobj->sprite);
	if (p_lspr == NULL || corona_lumpnum == LUMPERROR)
		return;
	if (p_lspr->corona_rendering_routine && !p_lspr->corona_rendering_routine(spr->mobj))
		return;

	// Objects which emit light.
	if (p_lspr->type & (CORONA_SPR|TYPE_FIELD_SPR))
	{
		cz = (outVerts[0].z + outVerts[2].z) / 2.0;

		// more realistique corona !
		if (cz >= Z2)
			return;

		// mobj dependent light selector
		if (!Sprite_Corona_Light_fade(p_lspr, cz, spr->mobj))
			return;

		// Sprite has a corona, and coronas are enabled.
		size = corona_size * 2.0;
		if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(spr->mobj, &Surf.PolyColor, NULL, false))
			Surf.PolyColor.rgba = p_lspr->corona_color;
		Surf.PolyColor.s.alpha = corona_alpha;

#if 0
		// compute position doing average
		cx = (outVerts[0].x + outVerts[2].x) / 2.0;
		cy = (outVerts[0].y + outVerts[2].y) / 2.0;
#else
		cx = 0.0f, cy = 0.0f; // gravity center
		// compute position doing average
		for (i = 0; i < 4; i++)
		{
			cx += outVerts[i].x;
			cy += outVerts[i].y;
#if 1
			cz += outVerts[i].z;
#endif
		}
		cx /= 4.0f;  cy /= 4.0f;
#if 1
		cz /= 4.0f;
#endif
#endif

		// put light little forward of the sprite so there is no
		// z-blocking or z-fighting
		if (cz > 0.5f) // correction for side drift due to cz change
		{
			// -0.75 per unit of cz
		   	cx += cx * ((-6.0f) / cz);
		   	cy += cy * ((-6.0f) / cz);
		}
		// need larger value to avoid z-blocking when look down
		cz -= 8.0f;  // larger causes more side-to-side drift

		// Bp; je comprend pas, ou est la rotation haut/bas ?
		//     tu ajoute un offset a y mais si la tu la reguarde de haut
		//     sa devrais pas marcher ... comprend pas :(
		//     (...) bon je croit que j'ai comprit il est tout pourit le code ?
		//           car comme l'offset est minime sa ce voit pas !
		cy += p_lspr->light_yoffset;
		light[0].x = light[3].x = cx - size;
		light[1].x = light[2].x = cx + size;
		light[0].y = light[1].y = cy - (size*1.33f);
		light[2].y = light[3].y = cy + (size*1.33f);
		light[0].z = light[1].z = light[2].z = light[3].z = cz;
		light[0].s = 0.0f;   light[0].t = 0.0f;
		light[1].s = 1.0f;   light[1].t = 0.0f;
		light[2].s = 1.0f;   light[2].t = 1.0f;
		light[3].s = 0.0f;   light[3].t = 1.0f;

#if 0
		HWR_GetPic(corona_lumpnum); // TODO: use different coronas
#endif

#if 1
		if (spr->mobj->type == MT_PLAYER)
			HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive | PF_NoDepthTest);
		else
			HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive);
#else
		// STAR NOTE: FUN FACT: did you know that PF_Corona not only makes the coronas appear through walls,
		//	but also lags everything out too?
		HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive | PF_Corona | PF_NoDepthTest);
#endif
	}
}

// Draw coronas from dynamic light list
void HWR_DL_Draw_Coronas(void)
{
	FSurfaceInfo          Surf;
	FOutVector        light[4];
	light_t            *p_lspr;
	FVector         *light_pos;
	mobj_t          *lspr_mobj;
	float           cx, cy, cz;
	float                 size;
	unsigned int             j;

#ifndef STAR_LIGHTING
	if (!cv_glcoronas.value || !cv_gldynamiclighting.value)
		return;
#else
	if (!cv_tsourdt3rd_video_lighting_coronas.value || !cv_tsourdt3rd_video_lighting_coronas_lightingtype.value)
		return;
#endif

	if (dynlights->nb <= 0 || corona_lumpnum == LUMPERROR)
		return;
	HWR_GetPic(corona_lumpnum); // TODO: use different coronas

	for (j = 0; j < dynlights->nb; j++)
	{
		p_lspr = dynlights->p_lspr[j];
		light_pos = &dynlights->position[j];
		lspr_mobj = dynlights->mo[j];

		// it's a real object which emits light
		if (!lspr_mobj)
			continue;
		if (P_MobjWasRemoved(lspr_mobj) || !lspr_mobj->state)
		{
			P_SetTarget(&lspr_mobj, NULL);
			continue;
		}
#if 0
		if (!(p_lspr->type & CORONA_SPR))
			continue;
#endif
		if (p_lspr->corona_rendering_routine && !p_lspr->corona_rendering_routine(lspr_mobj))
			continue;

		// transform light positions
#if 0
		cx = light_pos->x;
		cy = light_pos->y;
		cz = light_pos->z; // gravity center
		HWR_Transform(&cx, &cy, &cz);
#else
		transform_world_to_gr(/*IN*/ light_pos->x, light_pos->y, light_pos->z, /*OUT*/ &cx, &cy, &cz);
#endif

		// more realistique corona !
		if (cz >= Z2)
			continue;

		// mobj dependent light selector
		if (!Sprite_Corona_Light_fade(p_lspr, cz, lspr_mobj))
			continue;

		// Sprite has a corona, and coronas are enabled.
		size = corona_size * 2.0;
		if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(lspr_mobj, &Surf.PolyColor, NULL, false))
			Surf.PolyColor.rgba = p_lspr->corona_color;
		Surf.PolyColor.s.alpha = corona_alpha;

#if 0
		// put light little forward the sprite so there is no
		// z-buffer problem (coplanar polygons)
		// BP: use PF_Decal do not help :(
		cz = cz - 5.0f;

		light[0].x = cx-size;  light[0].z = cz;
		light[0].y = cy-size*1.33f;
		light[0].s = 0.0f;   light[0].t = 0.0f;

		light[1].x = cx+size;  light[1].z = cz;
		light[1].y = cy-size*1.33f;
		light[1].s = 1.0f;   light[1].t = 0.0f;

		light[2].x = cx+size;  light[2].z = cz;
		light[2].y = cy+size*1.33f;
		light[2].s = 1.0f;   light[2].t = 1.0f;

		light[3].x = cx-size;  light[3].z = cz;
		light[3].y = cy+size*1.33f;
		light[3].s = 0.0f;   light[3].t = 1.0f;
#else
		// put light little forward the sprite so there is no
		// z-buffer problem (coplanaire polygons)
		// BP: use PF_Decal do not help :(
		if (cz > 0.5f) // correction for side drift due to cz change
		{
		   cx += cx * ((-3.8f) / cz);
		   cy += cy * ((-3.8f) / cz);
		}
		cz = cz - 5.0f;

		light[0].x = light[3].x = cx - size;
		light[1].x = light[2].x = cx + size;
		light[0].y = light[1].y = cy - (size*1.33f);
		light[2].y = light[3].y = cy + (size*1.33f);
		light[0].z = light[1].z = light[2].z = light[3].z = cz;
		light[0].s = 0.0f;   light[0].t = 0.0f;
		light[1].s = 1.0f;   light[1].t = 0.0f;
		light[2].s = 1.0f;   light[2].t = 1.0f;
		light[3].s = 0.0f;   light[3].t = 1.0f;
#endif

#if 0
		// STAR NOTE: FUN FACT: did you know that PF_Corona not only makes the coronas appear through walls,
		//	but also lags everything out too?
		if (lspr_mobj->type == MT_PLAYER)
			HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive | PF_NoDepthTest | PF_Corona);
		else
			HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive | PF_Corona);
#else
		if (lspr_mobj->type == MT_PLAYER)
			HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive | PF_NoDepthTest);
		else
			HWD.pfnDrawPolygon(&Surf, light, 4, PF_Modulated | PF_Additive);
#endif
	}
}

// --------------------------------------------------------------------------
// Remove all the dynamic lights at each frame
// Called from P_SetupLevel, HWR_RenderPlayerView, and HWR_RenderSkyboxView
// --------------------------------------------------------------------------
void HWR_Reset_Lights(void)
{
	memset(dynlights, 0, sizeof(dynlights_t));
}

// --------------------------------------------------------------------------
// Change view, thus change lights (splitscreen)
// Called from HWR_RenderPlayerView and HWR_RenderSkyboxView
// --------------------------------------------------------------------------
void HWR_Set_Lights(UINT8 viewnumber)
{
	dynlights = &view_dynlights[viewnumber];
}

// --------------------------------------------------------------------------
// Add a light for dynamic lighting
// The light position is already transformed except for mlook
// --------------------------------------------------------------------------
void HWR_DL_AddLightSprite(gl_vissprite_t *spr)
{
	FVector		*light_pos;
	light_t		*p_lspr;

	if (!spr || !spr->mobj || !spr->mobj->subsector)
	{
		// Invalid vissprite object.
		return;
	}

	// Hurdler: moved here because it's better ;-)
#ifndef STAR_LIGHTING
	if (!cv_glcoronas.value)
		return;
	if (cv_glstaticlighting.value)
		return;
#else
	if (!cv_tsourdt3rd_video_lighting_coronas.value)
		return;
	if (!cv_tsourdt3rd_video_lighting_coronas_lightingtype.value)
		return;
#endif

	if (dynlights->nb >= DL_MAX_LIGHT)
	{
		//CONS_Alert(CONS_WARNING, "maxlights reached !!!\n"); // STAR_CONS_Printf
		return;
	}

	// uncapped/interpolation
	interpmobjstate_t interp = {0};

	if (R_UsingFrameInterpolation() && !paused)
	{
		R_InterpolateMobjState(spr->mobj, rendertimefrac, &interp);
	}
	else
	{
		R_InterpolateMobjState(spr->mobj, FRACUNIT, &interp);
	}

	// check if sprite contains dynamic light
	p_lspr = t_lspr[spr->mobj->sprite];
	if (p_lspr->type == NOLIGHT)
		return;

#if 0
	// check for the proper light type
	if (!(p_lspr->type & DYNLIGHT_SPR))
		return;
	if ((p_lspr->type & (LIGHT_SPR|CORONA_SPR|ROCKET_SPR)) != LIGHT_SPR)
		return;
#endif

	// Create a dynamic light, and give the light a position.
	light_pos = &dynlights->position[dynlights->nb];
	light_pos->x = FIXED_TO_FLOAT(interp.x);
#if 0
	if (P_MobjFlip(spr->mobj))
		light_pos->y = FIXED_TO_FLOAT(interp.z) + FIXED_TO_FLOAT(interp.height>>1) + p_lspr->light_yoffset;
	else
	{
		light_pos->y = FIXED_TO_FLOAT(interp.z) + p_lspr->light_yoffset;
		light_pos->y += FIXED_TO_FLOAT(P_MobjFlip(spr->mobj));
	}
#else
#if 0
	if (spr->mobj->eflags & MFE_VERTICALFLIP)
		light_pos->y = FIXED_TO_FLOAT(interp.z) + FIXED_TO_FLOAT(interp.height) + FIXED_TO_FLOAT(p_lspr->light_yoffset);
	else
		light_pos->y = FIXED_TO_FLOAT(interp.z);
#else
	//light_pos->y = FIXED_TO_FLOAT(interp.z) + FIXED_TO_FLOAT(interp.height) + FIXED_TO_FLOAT(p_lspr->light_yoffset);
	light_pos->y = FIXED_TO_FLOAT(interp.z) + FIXED_TO_FLOAT(interp.height>>1)+ p_lspr->light_yoffset;
#endif
#endif
	light_pos->z = FIXED_TO_FLOAT(interp.y);

	dynlights->mo[dynlights->nb] = spr->mobj;
	P_SetTarget(&dynlights->mo[dynlights->nb], spr->mobj);

	dynlights->p_lspr[dynlights->nb] = p_lspr;

	dynlights->nb++;
}

void HWR_Init_Light(const char *lightpatch)
{
	// Make sure the corona graphic exists
	corona_lumpnum = W_GetNumForName(lightpatch);
	lightmap_patch = HWR_GetCachedGLPatch(corona_lumpnum);
	Z_ChangeTag(lightmap_patch, PU_CACHE);
	lightmap_gl_patch = (GLPatch_t *)Patch_AllocateHardwarePatch(lightmap_patch);
	Z_ChangeTag(lightmap_gl_patch, PU_CACHE);
	lightmap_gl_patch->mipmap->downloaded = 0;
	// precalculate sqr radius
	for (size_t i = 0; i < NUMLIGHTS; i++)
		lspr[i].dynamic_sqrradius = (lspr[i].dynamic_radius * lspr[i].dynamic_radius);
}

void HWR_DynamicShadowing(FOutVector *clVerts, int nrClipVerts)
{
	FSurfaceInfo Surf;
	int i;

#ifndef STAR_LIGHTING
	if (!cv_glcoronas.value || !cv_glstaticlighting.value)
		return;
#else
	if (!cv_tsourdt3rd_video_lighting_coronas.value || cv_tsourdt3rd_video_lighting_coronas_lightingtype.value)
		return;
#endif

	for (i = 0; i < nrClipVerts; i++)
	{
		clVerts[i].s = 0.5f + clVerts[i].x*0.01f;
		clVerts[i].t = 0.5f + clVerts[i].z*0.01f*1.2f;
	}

	HWR_SetLight();

	Surf.PolyColor.rgba = 0x70707070;

	HWD.pfnDrawPolygon(&Surf, clVerts, nrClipVerts, LIGHTMAPFLAGS);
}

//**********************************************************
// Hurdler: new code for faster static lighting and and T&L
//**********************************************************

#ifdef STATICLIGHTMAPS
// est ce bien necessaire ?
static sector_t *lgl_backsector;
static seg_t *lgl_curline;

// p1 et p2 c'est le deux bou du seg en float
static void HWR_Create_WallLightmaps(FVector *p1, FVector *p2, int lightnum, seg_t *line)
{
	lightmap_t *lp;

	// (...) calcul presit de la projection et de la distance

//	if (dist_p2d >= dynamic_light->lsp[lightnum]->dynamic_sqrradius)
//		return;

	// (...) attention faire le backfase cull histoir de faire mieux que Q3 !

	(void)p1;
	(void)p2;
	(void)lightnum;
	lp = malloc(sizeof(lp));
	lp->next = line->lightmaps;
	line->lightmaps = lp;

	// (...) encore des b� calcul bien lourd et on stock tout sa dans la lightmap
}

static void HWR_AddLightMapForLine(int lightnum, seg_t *line)
{
	/*
	int                 x1;
	int                 x2;
	angle_t             angle1;
	angle_t             angle2;
	angle_t             span;
	angle_t             tspan;
	*/
	FVector 			p1,p2;

	lgl_curline = line;
	lgl_backsector = line->backsector;

	// Reject empty lines used for triggers and special events.
	// Identical floor and ceiling on both sides,
	//  identical light levels on both sides,
	//  and no middle texture.
/*
	if (lgl_backsector->ceilingpic == gl_frontsector->ceilingpic
		&& lgl_backsector->floorpic == gl_frontsector->floorpic
		&& lgl_backsector->lightlevel == gl_frontsector->lightlevel
		&& lgl_curline->sidedef->midtexture == 0)
	{
		return;
	}
*/

	p1.y = FIXED_TO_FLOAT(lgl_curline->v1->y);
	p1.x = FIXED_TO_FLOAT(lgl_curline->v1->x);
	p2.y = FIXED_TO_FLOAT(lgl_curline->v2->y);
	p2.x = FIXED_TO_FLOAT(lgl_curline->v2->x);

	// check bbox of the seg
//	if (!(CircleTouchBBox(&p1, &p2, &dynlights->position[lightnum], p_lspr[lightnum]->dynamic_radius)))
//		return;

	HWR_Create_WallLightmaps(&p1, &p2, lightnum, line);
}

// TODO: see what HWR_AddLine does
static void HWR_CheckSubsector(size_t num, fixed_t *bbox)
{
	int         count;
	seg_t       *line;
	subsector_t *sub;
	FVector     p1,p2;
	int         lightnum;

	p1.y = FIXED_TO_FLOAT(bbox[BOXTOP	]);
	p1.x = FIXED_TO_FLOAT(bbox[BOXLEFT	]);
	p2.y = FIXED_TO_FLOAT(bbox[BOXBOTTOM]);
	p2.x = FIXED_TO_FLOAT(bbox[BOXRIGHT	]);

	if (num < numsubsectors)
	{
		sub = &subsectors[num]; // subsector
		for (lightnum = 0; lightnum < dynlights->nb; lightnum++)
		{
#if 0
			// check bbox of the seg
			if (!(CircleTouchBBox(&p1, &p2, &dynlights->position[lightnum], p_lspr[lightnum]->dynamic_radius)))
				continue;
#else
			(void)p1;
			(void)p2;
#endif

			count = sub->numlines;          // how many linedefs
			line = &segs[sub->firstline];   // first line seg
			while (count--)
			{
				HWR_AddLightMapForLine(lightnum, line);	// compute lightmap
				line++;
			}
		}
	}
}


// --------------------------------------------------------------------------
// Hurdler: this adds lights by mobj.
// --------------------------------------------------------------------------
static void HWR_AddMobjLights(mobj_t *thing)
{
	if (!(t_lspr[thing->sprite]->type == NOLIGHT)
		|| !(t_lspr[thing->sprite]->type & CORONA_SPR))
		return;

	// uncapped/interpolation
	interpmobjstate_t interp = {0};

	if (R_UsingFrameInterpolation() && !paused)
	{
		R_InterpolateMobjState(thing, rendertimefrac, &interp);
	}
	else
	{
		R_InterpolateMobjState(thing, FRACUNIT, &interp);
	}

	// Sprite has a corona. Thereore, create a corona dynamic light.
	FVector *light_pos = &dynlights->position[dynlights->nb];
	light_pos->x = FIXED_TO_FLOAT(interp.x);
	if (P_MobjFlip(thing))
		light_pos->y = FIXED_TO_FLOAT(interp.z) + FIXED_TO_FLOAT(interp.height>>1) + p_lspr->light_yoffset;
	else
	{
		light_pos->y = FIXED_TO_FLOAT(interp.z) + t_lspr[thing->sprite]->light_yoffset;
		light_pos->y += FIXED_TO_FLOAT(P_MobjFlip(thing));
	}
	light_pos->z = FIXED_TO_FLOAT(interp.y);

	p_lspr[dynlights->nb] = t_lspr[thing->sprite];

	dynlights->mo[dynlights->nb] = spr->mobj;
	P_SetTarget(&dynlights->mo[dynlights->nb], thing);

	if (dynlights->nb >= DL_MAX_LIGHT)
		dynlights->nb = DL_MAX_LIGHT; // reuse last
	else
		dynlights->nb++;
}

//
// Hurdler: The goal of this function is to walk through all the bsp starting on the top.
//         We need to do that to know all the lights in the map and all the walls
//
static void HWR_ComputeLightMapsInBSPNode(int bspnum, fixed_t *bbox)
{
	if (bspnum & NF_SUBSECTOR) // Found a subsector?
	{
		if (bspnum == -1)
			HWR_CheckSubsector(0, bbox); // probably unecessary: see boris' comment in hw_bsp
		else
			HWR_CheckSubsector(bspnum&(~NF_SUBSECTOR), bbox);
		return;
	}
	HWR_ComputeLightMapsInBSPNode(nodes[bspnum].children[0], nodes[bspnum].bbox[0]);
	HWR_ComputeLightMapsInBSPNode(nodes[bspnum].children[1], nodes[bspnum].bbox[1]);
}

static void HWR_SearchLightsInMobjs(void)
{
	// search in the list of thinkers
	for (thinker_t *th = thlist[THINK_MOBJ].next; th != &thlist[THINK_MOBJ]; th = th->next)
		if (th->function.acp1 != (actionf_p1)P_RemoveThinkerDelayed)
			HWR_AddMobjLights((mobj_t *)th);
}

//
// HWR_DL_CreateStaticLightmaps(INT32 bspnum)
// Called from P_SetupLevel
//
// Hurdler: TODO!
//
void HWR_DL_CreateStaticLightmaps(INT32 bspnum)
{
	// First: Searching for lights
	// BP: if i was you, I will make it in create mobj since mobj can be create
	//     at runtime now with fragle scipt
	HWR_SearchLightsInMobjs();
	CONS_Printf("HWR_DL_CreateStaticLightmaps(): %d lights found\n", dynlights->num_lights);

	// Second: Build all lightmap for walls covered by lights
	validcount++; // to be sure
	//HWR_ComputeLightMapsInBSPNode(numnodes-1, NULL);
	HWR_ComputeLightMapsInBSPNode(bspnum, NULL);

#if 0
	HWR_Reset_Lights();
#endif
}
#else
void HWR_DL_CreateStaticLightmaps(INT32 bspnum) { (void)bspnum; }
#endif

/**
 \todo

  - Les coronas ne sont pas g�rer avec le nouveau systeme, seul le dynamic lighting l'est
  - calculer l'offset des coronas au chargement du level et non faire la moyenne
	au moment de l'afficher
	 BP: euh non en fait il faux encoder la position de la light dans le sprite
		 car c'est pas focement au mileux de plus il peut en y avoir plusieur (chandelier)
  - changer la comparaison pour l'affichage des coronas (+ un epsilon)
	BP: non non j'ai trouver mieux :) : lord du AddSprite tu rajoute aussi la coronas
		dans la sprite list ! avec un z de epsilon (attention au ZCLIP_PLANE) et donc on
		l'affiche en dernier histoir qu'il puisse etre cacher par d'autre sprite :)
		Bon fait metre pas mal de code special dans hwr_project sprite mais sa vaux le
		coup
  - gerer dynamic et static : retenir le nombre de lightstatic et clearer toute les
		light>lightstatic (les dynamique) et les lightmap correspondant dans les segs
		puit refaire une passe avec le code si dessus mais rien que pour les dynamiques
		(tres petite modification)
  - finalement virer le hack splitscreen, il n'est plus necessaire !
*/

#endif // HWRENDER
