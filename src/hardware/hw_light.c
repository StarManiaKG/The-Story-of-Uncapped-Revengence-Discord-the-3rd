// SONIC ROBO BLAST 2 & SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2015 by DooM Legacy Team.
// Copyright (C) 1999-2024 by Sonic Team Junior.
// Copyright (C) 2023-2026 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file hw_light.c
/// \brief Corona/Dynamic/Static lighting add on by Hurdler
/// \brief StarManiaKG: featuring new modern enhancements by me!
///	\note !!! Under construction !!!

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
#include "../g_game.h"
#include "../r_bsp.h"
#include "../r_fps.h"

//#include "SDL.h"
//#include "SDL_opengl.h"

// TSoURDt3rd
#include "../STAR/lights/smkg-coronas.h"
#include "../STAR/smkg-cvars.h"

#ifdef ALAM_LIGHTING

//=============================================================================
//                                                                      DEFINES
//=============================================================================

// [WDJ] Makes debugging difficult when references like these are hidden.
// Make them visible in the code.
//#define CORONA_DEBUG
#define REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES

//#define REVAMPED_WALLRENDERING

#ifdef REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES
#define DL_TVERT_ASPECT   1.2f   /* historical stretch factor */
#define DL_SAFE_EPS       1e-6f
#endif

#ifdef CORONA_DEBUG
	#define LIGHTMAP_POLY_FLAGS 	PF_Masked|PF_NoAlphaTest // see overdraw
	#define LIGHTMAP_MIPMAP_FLAGS 	TF_WRAPXY // see overdraw
	//#define DL_SQRRADIUS(x)     	light->light_data->dynamic_sqrradius
	//#define DL_RADIUS()        	light->light_data->dynamic_radius
	//#define LIGHT_POS(var)        light->pos.var
#else
	#define LIGHTMAP_POLY_FLAGS 	PF_Modulated
	#define LIGHTMAP_MIPMAP_FLAGS	0
#endif

#define DL_HIGH_QUALITY
//#define STATICLIGHTMAPS // Hurdler: TODO!

#ifdef STAR_LIGHTING
	#define CORONA_VAR cv_tsourdt3rd_video_lighting_coronas.value
	#define CORONA_STATIC cv_tsourdt3rd_video_lighting_coronas_lightingtype.value == 0
	#define CORONA_DYNAMIC cv_tsourdt3rd_video_lighting_coronas_lightingtype.value == 1
#else
	#define CORONA_VAR cv_glcoronas.value
	#define CORONA_STATIC cv_glstaticlighting.value
	#define CORONA_DYNAMIC cv_gldynamiclighting.value
#endif
#define STATIC_ONLY \
	if (!(CORONA_VAR && CORONA_STATIC)) \
		return;
#define DYNAMIC_ONLY \
	if (!(CORONA_VAR && CORONA_DYNAMIC)) \
		return;
#define CORONA_VAR_ONLY \
	if (!(CORONA_VAR)) \
		return;

#define NO_SPRITELIGHT_RENDERING // reasonable since it doens't work
//#define NO_WALL_RENDERING // this is more reasonable, since wall lighting doesn't work on all walls, and it's obvious
//#define NO_PLANE_RENDERING
//#define NO_CORONA_RENDERING

//#define NO_LIGHT_ADDING
//#define NO_LIGHT_INIT

//=============================================================================
//                                                                       GLOBAL
//=============================================================================

// Dynamic light data
static FDynamicLight *dynlights_head = NULL;
INT32 num_dynamic_lights = 0;

// Corona graphic data
static lumpnum_t corona_lumpnum = LUMPERROR;
static GLMipmap_t corona_gl_patch_mipmap;
static GLPatch_t corona_gl_patch = { .mipmap = &corona_gl_patch_mipmap };

#define SUFFER_PRINT(type, ...) { CONS_Debug( type, __VA_ARGS__ ); }
//#define SUFFER_PRINT(type, ...) { (void)type; CONS_Printf( __VA_ARGS__ ); }

//=============================================================================
//                                                                       PROTOS
//=============================================================================

// -----------------+
// HWR_SetLight     : Download a disc shaped alpha map for rendering fake lights
// -----------------+
static void HWR_SetLight(void)
{
	const INT32 texSize = 128;
	const INT32 texCenter = texSize/2;

	if (!corona_gl_patch.mipmap->downloaded && !corona_gl_patch.mipmap->data)
	{
		uint16_t *data = Z_Malloc(texSize * texSize * sizeof(uint16_t), PU_HWRCACHE, &corona_gl_patch.mipmap->data);

		for (INT32 y = 0; y < texSize; y++) // Fill a radial alpha map into data using alpha bytes.
		{
			for (INT32 x = 0; x < texSize; x++)
			{
				const INT32 dx = x - texCenter;
				const INT32 dy = y - texCenter;
				const INT32 distSq = dx*dx + dy*dy;
				const INT32 centSq = (texCenter - 1) * (texCenter - 1);

				if (distSq <= centSq)
				{
					uint8_t alpha = clamp((255 - (int)(4.0f*sqrtf((float)distSq))), 0, 255);
					data[y * texSize + x] = (uint16_t)((alpha << 8) | 0xFFu);
				}
				else
				{
					data[y * texSize + x] = 0;
				}
			}
		}

		corona_gl_patch.max_s = corona_gl_patch.max_t = ((float)texSize / (float)texSize);
		corona_gl_patch.mipmap->width = corona_gl_patch.mipmap->height = (UINT16)texSize;
		corona_gl_patch.mipmap->format = GL_TEXFMT_ALPHA_INTENSITY_88;
		corona_gl_patch.mipmap->flags = LIGHTMAP_MIPMAP_FLAGS;
		//corona_gl_patch.mipmap->flags |= TF_CHROMAKEYED; // DEFAULT: OFF
		corona_gl_patch.mipmap->flags |= TF_TRANSPARENT; // DEFAULT: OFF
		//corona_gl_patch.mipmap->flags |= TF_CORONA;
		corona_gl_patch.mipmap->downloaded = 0;

		HWD.pfnSetTexture(corona_gl_patch.mipmap);
		SUFFER_PRINT(DBG_RENDER, "HWR_SetLight(): texture set\n");
	}

	// Viva-la Rendering!
	HWR_SetCurrentTexture(corona_gl_patch.mipmap);

	// The system-memory data can be purged now.
	Z_ChangeTag(corona_gl_patch.mipmap->data, PU_HWRCACHE_UNLOCKED);
}

// --------------------------------------------------------------------------
// calcul la projection d'un point sur une droite (determin�e par deux
// points) et ensuite calcul la distance (au carr�) de ce point au point
// project� sur cette droite
//
// (translated)
//
// --------------------------------------------------------------------------

//#define HWR_DISTP2D_ISSEG

#ifndef HWR_DISTP2D_ISSEG

//#ifndef REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES
#if 1
static float HWR_DistP2D(FOutVector *p1, FOutVector *p2, floatvector3_t *p3, floatvector3_t *inter)
//static inline float HWR_DistP2D(FOutVector *p1, FOutVector *p2, floatvector3_t *p3, floatvector3_t *inter)
{
//#define USE_FIXED
//#define CMP_FLOATS

#if defined (USE_FIXED)
	fixed_t p1_zf = FLOAT_TO_FIXED(p1->z), p2_zf = FLOAT_TO_FIXED(p2->z);
	fixed_t p1_xf = FLOAT_TO_FIXED(p1->x), p2_xf = FLOAT_TO_FIXED(p2->x);
	if (p1_zf == p2_zf)
#elif defined (CMP_FLOATS)
	if (p1->z == p2->z)
#else
	if (p1->z >= p2->z && p2->z <= p1->z)
#endif
	{
		inter->x = p3->x;
		inter->z = p1->z;
	}
#if defined (USE_FIXED)
	else if (p1_xf == p2_xf)
#elif defined (CMP_FLOATS)
	else if (p1->x == p2->x)
#else
	else if (p1->x >= p2->x && p2->x <= p1->x)
#endif
	{
		inter->x = p1->x;
		inter->z = p3->z;
	}
	else
	{
		// Wat een mooie formula! Hurdler's math ;-)
		register float pente = (p1->z - p2->z) / (p1->x - p2->x);
		register float local = p1->z - p1->x*pente;
		inter->x = (p3->z - local + p3->x/pente) * (pente/(pente*pente+1));
		inter->z = inter->x*pente + local;
	}
	return (p3->x-inter->x)*(p3->x-inter->x) + (p3->z-inter->z)*(p3->z-inter->z);
}

#else

// Project 3D point p3 onto the XZ line segment p1->p2 (ignores Y),
// return squared distance in XZ space and optionally the intersection point.
// Handles degenerate segments. All float; no fixed conversions.
// NOTE: inter may be NULL.
#if 1
static float HWR_DistP2D(const FOutVector *p1, const FOutVector *p2, const floatvector3_t *p3, floatvector3_t *inter)
{
	const float Ax = p1->x, Az = p1->z;
	const float Bx = p2->x, Bz = p2->z;
	const float Px = p3->x, Pz = p3->z;

	const float vx = Bx - Ax;
	const float vz = Bz - Az;
	const float segLenSq = vx*vx + vz*vz;

	float t;

	if (segLenSq <= DL_SAFE_EPS)
		t = 0.0f; // degenerate wall; treat as a point
	else
		t = (((Px - Ax)*vx + (Pz - Az)*vz) / segLenSq);

	// clamp to segment
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;

	const float Ix = Ax + t*vx;
	const float Iz = Az + t*vz;

	if (inter)
	{
		inter->x = Ix;
		inter->z = Iz;
		inter->y = 0.0f; // caller ignores
	}

	const float dx = Px - Ix;
	const float dz = Pz - Iz;
	return ((dx*dx) + (dz*dz));
}
#else

#if 0
// Safe point-to-segment squared distance in XZ; writes intersection if non-NULL.
//DL_DistToSegXZ
static inline float HWR_DistP2D(const FOutVector *A, const FOutVector *B, const floatvector3_t *P, floatvector3_t *inter)
//static float HWR_DistP2D(const FOutVector *A, const FOutVector *B, const floatvector3_t *P, floatvector3_t *inter)
{
	const float vx = B->x - A->x;
	const float vz = B->z - A->z;
	const float segLenSq = ((vx*vx) + (vz*vz));
	float t = 0.0f;

	// clamp to segment
	if (segLenSq > DL_SAFE_EPS)
		t = ((P->x - A->x)*vx + (P->z - A->z)*vz) / segLenSq;
	if (t < 0.0f) t = 0.0f;
	else if (t > 1.0f) t = 1.0f;

	const float Ix = A->x + t*vx;
	const float Iz = A->z + t*vz;
	if (inter)
	{
		inter->x = Ix;
		inter->z = Iz;
		inter->y = 0.0f;
	}

	const float dx = P->x - Ix;
	const float dz = P->z - Iz;
	return dx*dx + dz*dz;
}

#else

static double HWR_DistP2D(const FOutVector *pos, const FOutVector *B, const floatvector3_t *start, floatvector3_t *end)
{
	double u, px, py;
	double seg_dx = end->x - start->x;
	double seg_dy = end->y - start->y;
	double seg_length_sq = seg_dx * seg_dx + seg_dy * seg_dy;

	(void)B;

	u = (((pos->x - start->x) * seg_dx) + (pos->y - start->y) * seg_dy) / seg_length_sq;
	if (u < 0.0) u = 0.0;
	else if (u > 1.0) u = 1.0;

	px = start->x + (u * seg_dx);
	px -= pos->x;

	py = start->y + (u * seg_dy);
	py -= pos->y;

#if 1
	end->x = px;
	end->z = py;
	end->y = u; //0.0f; // caller ignores
#endif

	return (px * px) + (py * py);
}

#endif

#endif
#endif

#else

#if 0
//ADynamicLight::DistToSeg
static float HWR_DistP2D(const FOutVector *pos, floatvector3_t *inter, seg_t *seg)
{
   float u, px, py;
   float seg_dx = FixedToFloat(seg->v2->x - seg->v1->x);
   float seg_dy = FixedToFloat(seg->v2->y - seg->v1->y);
   float seg_length_sq = ((seg_dx * seg_dx) + (seg_dy * seg_dy));

   u = (((FixedToFloat(pos->x - seg->v1->x) * seg_dx) + (FixedToFloat(pos->y - seg->v1->y) * seg_dy)) / seg_length_sq);
   if (u < 0.f) u = 0.f; // clamp the test point to the line segment
   if (u > 1.f) u = 1.f;

   px = FixedToFloat(seg->v1->x) + (u * seg_dx);
   py = FixedToFloat(seg->v1->y) + (u * seg_dy);

   px -= FixedToFloat(pos->x);
   py -= FixedToFloat(pos->y);

#if 1
	inter->x = px;
	inter->z = py;
	inter->y = u; //0.0f; // caller ignores
#endif

   return ((px*px) + (py*py));
}
#else
//==========================================================================
//
// GZDoom: DistToSeg
// FDynamicLight::DistToSeg(const DVector3 &pos, vertex_t *start, vertex_t *end)
// Gets the light's distance to a line.
//
//==========================================================================
static double HWR_DistP2D(const FOutVector pos, vertex_t *start, vertex_t *end)
{
	double u, px, py;
	double seg_dx = end->fX() - start->fX();
	double seg_dy = end->fY() - start->fY();
	double seg_length_sq = seg_dx * seg_dx + seg_dy * seg_dy;

	u = (((pos.x - start->fX()) * seg_dx) + (pos.y - start->fY()) * seg_dy) / seg_length_sq;
	u = min(max(u, 0.), 1.); // clamp the test point to the line segment

	px = start->fX() + (u * seg_dx);
	py = start->fY() + (u * seg_dy);

	px -= pos.x;
	py -= pos.y;

	return (px*px) + (py*py);
}
#endif

#endif

//#ifndef REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES
#if 1
// check if sphere (radius r) centred in p3 touch the bounding box defined by p1, p2
static boolean SphereTouchBBox3D(FOutVector *p1, FOutVector *p2, floatvector3_t *p3, float radius)
{
	const float minx = min(p1->x, p2->x), maxx = max(p1->x, p2->x);
	const float miny = min(p1->y, p2->y), maxy = max(p1->y, p2->y);
	const float minz = min(p1->z, p2->z), maxz = max(p1->z, p2->z);

	if ((minx - radius > p3->x) || (maxx + radius < p3->x))
	{
		return false;
	}
	else if ((miny - radius > p3->y) || (maxy + radius < p3->y))
	{
		// StarManiaKG:
		// Something here is wrong.
		// I mean not really probably but it works (and looks) good off so off it is :p
		//return false;
	}
	else if ((minz - radius > p3->z) || (maxz + radius < p3->z))
	{
		return false;
	}

	return true;
}
#else
// Check if sphere (center p3, radius r) touches AABB defined by min/max from p1,p2.
// p1 & p2 may not actually be min/max; we fix it.

//static boolean SphereTouchBBox3D(FOutVector *p1, FOutVector *p2, floatvector3_t *p3, float r)
static inline boolean SphereTouchBBox3D(FOutVector *p1, FOutVector *p2, floatvector3_t *p3, float r)
//static boolean SphereTouchBBox3D(const FOutVector *p1, const FOutVector *p2, const floatvector3_t *p3, float r)
//static boolean SphereTouchBBox3D(FOutVector *p1, FOutVector *p2, floatvector3_t *p3, float r)
{
#if 0
	float minx = min(p1->x, p2->x), maxx = max(p1->x, p2->x);
	float miny = min(p1->y, p2->y), maxy = max(p1->y, p2->y);
	float minz = min(p1->z, p2->z), maxz = max(p1->z, p2->z);
#else
	float minx = p1->x, maxx = p2->x;
	float miny = p2->y, maxy = p1->y;
	float minz = p2->z, maxz = p1->z;
#endif
	float d = 0.0f, v; // Squared distance from point to AABB

	if      (p3->x < minx) { v = p3->x - minx; d += v*v; }
	else if (p3->x > maxx) { v = p3->x - maxx; d += v*v; }

	if      (p3->y < miny) { v = p3->y - miny; d += v*v; }
	else if (p3->y > maxy) { v = p3->y - maxy; d += v*v; }

	if      (p3->z < minz) { v = p3->z - minz; d += v*v; }
	else if (p3->z > maxz) { v = p3->z - maxz; d += v*v; }

	return (d <= r*r);
}
#endif

#ifdef REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES
// Build AABB from wall verts

#define BBOX_FIX
#ifndef BBOX_FIX

static void DL_ComputeBBox(const FOutVector *v, FOutVector *pmin, FOutVector *pmax, size_t n)
{
	float minx = v[0].x, maxx = v[0].x;
	float miny = v[0].y, maxy = v[0].y;
	float minz = v[0].z, maxz = v[0].z;
	for (size_t i = 1; i < n; i++)
	{
		if (v[i].x < minx) minx = v[i].x; else if (v[i].x > maxx) maxx = v[i].x;
		if (v[i].y < miny) miny = v[i].y; else if (v[i].y > maxy) maxy = v[i].y;
		if (v[i].z < minz) minz = v[i].z; else if (v[i].z > maxz) maxz = v[i].z;
	}
	pmin->x = minx; pmin->y = miny; pmin->z = minz;
	pmax->x = maxx; pmax->y = maxy; pmax->z = maxz;
}

#else
//static inline void DL_ComputeBBox(const FOutVector *v, FOutVector *mn, FOutVector *mx, size_t n)
static void DL_ComputeBBox(const FOutVector *v, FOutVector *mn, FOutVector *mx, size_t n)
{
	//mn->x = mx->x = v[0].x;
	mn->y = mx->y = v[0].y;
	//mn->z = mx->z = v[0].z;
	for (size_t i = 1; i < n; i++)
	{
		if (v[i].x < mn->x) mn->x = v[i].x; else if (v[i].x > mx->x) mx->x = v[i].x;
		if (v[i].y < mn->y) mn->y = v[i].y; else if (v[i].y > mx->y) mx->y = v[i].y;
		if (v[i].z < mn->z) mn->z = v[i].z; else if (v[i].z > mx->z) mx->z = v[i].z;
	}
}

#endif

// Compute normal from first 3 verts. Returns false if degenerate.
// Normalized to unit length if possible.

static boolean DL_ComputePlaneNormal(const FOutVector *v, size_t n, floatvector3_t *outN)
//static inline boolean DL_ComputePlaneNormal(const FOutVector *v, size_t n, floatvector3_t *outN)
{
	if (n < 3) { outN->x=0; outN->y=1; outN->z=0; return false; }
	const float ux = v[1].x - v[0].x;
	const float uy = v[1].y - v[0].y;
	const float uz = v[1].z - v[0].z;

	const float vx = v[2].x - v[0].x;
	const float vy = v[2].y - v[0].y;
	const float vz = v[2].z - v[0].z;

	// cross u x v
	float nx = uy*vz - uz*vy;
	float ny = uz*vx - ux*vz;
	float nz = ux*vy - uy*vx;

	float len = sqrtf(nx*nx + ny*ny + nz*nz);
	if (len < DL_SAFE_EPS)
	{
		outN->x = 0; outN->y = 1; outN->z = 0;
		return false;
	}

	len = 1.0f/len;
	outN->x = nx*len;
	outN->y = ny*len;
	outN->z = nz*len;
	return true;
}

// Return indices of the pair of verts whose XZ distance is longest.
// Used to choose the wall span for S mapping regardless of incoming order.
//static inline void DL_FindLongestXZEdge(const FOutVector *v, int *outA, int *outB)
static void DL_FindLongestXZEdge(const FOutVector *v, int *outA, int *outB)
{
	float best = -1.0f;
	int ai = 0, bi = 2;
	for (int i = 0; i < 4; i++)
	for (int j = i+1; j < 4; j++)
	{
		float dx = v[j].x - v[i].x;
		float dz = v[j].z - v[i].z;
		float d2 = dx*dx + dz*dz;
		if (d2 > best)
		{
			best = d2;
			ai = i; bi = j;
		}
	}
	*outA = ai; *outB = bi;
}

#endif // REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES

// -------------------------------------------------------------------------+
// HWR_DL_GetCoronaLighting
// Sets the lighting for coronas based on surrounding sectors and colormaps
// -------------------------------------------------------------------------+

static void HWR_DL_GetCoronaLighting(FSurfaceInfo *lSurf, sector_t *sector, mobj_t *lspr_mobj)
{
	INT32 lightlevel = 255;
	INT32 sector_lightlevel = 255;
	extracolormap_t *colormap = NULL;

	if (lspr_mobj == NULL || sector == NULL)
	{
		return;
	}

	if (sector->numlights)
	{
		INT32 light = R_GetPlaneLight(sector, lspr_mobj->z + lspr_mobj->height, false);
		sector_lightlevel = *sector->lightlist[light].lightlevel;
		if (*sector->lightlist[light].extra_colormap && !(lspr_mobj->renderflags & RF_NOCOLORMAPS))
			colormap = *sector->lightlist[light].extra_colormap;
	}
	else
	{
		sector_lightlevel = sector->lightlevel;
		if (sector->extra_colormap && !(lspr_mobj->renderflags & RF_NOCOLORMAPS))
			colormap = sector->extra_colormap;
	}

	if (R_ThingIsFullDark(lspr_mobj))
		lightlevel = 0;
	else if (R_ThingIsSemiBright(lspr_mobj))
		lightlevel = 128 + (sector_lightlevel>>1);
	else if (R_ThingIsFullBright(lspr_mobj))
		lightlevel = 255;
	else
		lightlevel = clamp(sector_lightlevel, 0, 255);

	HWR_Lighting(lSurf, lightlevel, colormap, P_MobjUsesDirectionalLighting(lspr_mobj) && !R_ThingIsFullBright(lspr_mobj));
}

// --------------------------------------------------------------------------
// Calculation of dynamic lighting on walls
// Coords lVerts contains the wall with mlook transformed
// --------------------------------------------------------------------------
void HWR_SpriteLighting(FOutVector *wlVerts) // SRB2CBTODO: Support sprites to be lit too
{
#ifdef NO_SPRITELIGHT_RENDERING
	return;
#endif

	FDynamicLight *light;
	floatvector3_t             inter;
	FSurfaceInfo                 Surf;
	float                     d[4], s;
	unsigned int                    i;

	STATIC_ONLY
	if (corona_gl_patch.mipmap == NULL)
		return;
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

	for (light = dynlights_head; light != NULL; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		floatvector3_t *light_pos = &light->pos;

		if (light->visible == false)
			continue;

		// Build a working copy of the wall verts (keep original UVs for the wall texture)
		FOutVector vtx[4];
		memcpy(vtx, wlVerts, sizeof(FOutVector) * 4);

		// check bounding box first
		if (!(SphereTouchBBox3D(&wlVerts[2], &wlVerts[0], light_pos, p_lspr->dynamic_radius)))
			continue;

		d[0] = wlVerts[2].x - wlVerts[0].x;
		d[1] = wlVerts[2].z - wlVerts[0].z;
		d[2] = light_pos->x - wlVerts[0].x;
		d[3] = light_pos->z - wlVerts[0].z;
#if 0
		if (d[2]*d[1] - d[3]*d[0] < 0)
			continue; // Culled by backface
#endif

		// check exact distance
#ifndef HWR_DISTP2D_ISSEG
		const float dist_p2d = HWR_DistP2D(&wlVerts[2], &wlVerts[0], light_pos, &inter);
#else
		const float dist_p2d = HWR_DistP2D(&wlVerts[2], &inter, curline);
#endif
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

		s = (0.5f / p_lspr->dynamic_radius);

		for (i = 0; i < 4; i++)
		{
			wlVerts[i].s = (float)(0.5f + d[i]*s);
			wlVerts[i].t = (float)(0.5f + (wlVerts[i].y - light_pos->y)*s*1.2f);
		}

		HWR_SetLight();
		//HWR_DL_GetCoronaLighting(&Surf, NULL, lspr_mobj);

#ifdef DL_HIGH_QUALITY
		Surf.PolyColor.s.alpha *= (1-dist_p2d / p_lspr->dynamic_sqrradius);
#endif

		FBITFIELD blendmode = light->poly_flags;
		blendmode |= PF_Translucent;
		HWR_ProcessPolygon(&Surf, wlVerts, 4, blendmode, SHADER_NONE, false);
	}
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
	FDynamicLight *light;

	STATIC_ONLY
	if (corona_gl_patch.mipmap == NULL)
		return;
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

	for (light = dynlights_head; light; light = light->next)
	{
		floatvector3_t inter = light->pos;
		FSurfaceInfo    Surf;
		float           dist_p2d, d[4], s;

		fixed_t xvalue = FIXED_TO_FLOAT(lspr_mobj->x);
		fixed_t yvalue = FIXED_TO_FLOAT(lspr_mobj->y);
		fixed_t zvalue = FIXED_TO_FLOAT(lspr_mobj->z);

		if (light->visible == false)
			continue;

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
#ifndef HWR_DISTP2D_ISSEG
		dist_p2d = HWR_DistP2D(&wlVerts[2], &wlVerts[0], &SHADOW_POS(j), &inter);
#else
		dist_p2d = HWR_DistP2D(&wlVerts[2], &inter, curline);
#endif
		if (dist_p2d >= DS_SQRRADIUS(j))
			continue;

		d[0] = (float)sqrt((wlVerts[0].x-inter.x)*(wlVerts[0].x-inter.x)
						   + (wlVerts[0].z-inter.z)*(wlVerts[0].z-inter.z));
		d[1] = (float)sqrt((wlVerts[2].x-inter.x)*(wlVerts[2].x-inter.x)
						   + (wlVerts[2].z-inter.z)*(wlVerts[2].z-inter->z));
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

	} // end for (j = 0; j < dynshadows->num_lights; j++)
}
#endif

// Hurdler: The old code was removed by me because I don't think it will be used one day.
//          (It's still available on the CVS for educational purpose: Revision 1.8)

//#ifndef REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES
//#ifndef REVAMPED_WALLRENDERING
#if 1

#if 1 // DEFAULT IS 1

// --------------------------------------------------------------------------
// calcul du dynamic lighting sur les murs
// lVerts contient les coords du mur sans le mlook (up/down)
// --------------------------------------------------------------------------
void HWR_WallLighting(FSurfaceInfo *pSurf, FOutVector *wlVerts, FBITFIELD PolyFlags, FBITFIELD ExtraPolyFlags, int shader)
{
#ifdef NO_WALL_RENDERING
	return;
#endif

	floatvector3_t      inter;
	FBITFIELD extra_blendmode = 0;

	CORONA_VAR_ONLY
	if (corona_gl_patch.mipmap == NULL)
		return;
#ifdef STAR_LIGHTING
	if (!cv_tsourdt3rd_video_lighting_coronas_walllighting.value)
		return;
#endif
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

	// Choose draw settings
	//int newShader = shader;
	int newShader = SHADER_NONE;
	//int newShader = SHADER_SPRITE;
#if 1
	if (HWR_UseShader())
	{
		//newShader = SHADER_WALL;
		extra_blendmode |= PF_ColorMapped;
	}
#endif
	(void)shader;

	FDynamicLight *light;
	FSurfaceInfo Surf;
	FOutVector vtx[4];

	for (light = dynlights_head; light != NULL; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		floatvector3_t *light_pos = &light->pos;

		if (light->visible == false)
		{
			continue;
		}

		memcpy(vtx, wlVerts, sizeof(FOutVector) * 4);

#if 0
		float radius = (p_lspr->dynamic_radius > 0.0f ? p_lspr->dynamic_radius : 256.0f);
		float sqrradius = (p_lspr->dynamic_sqrradius > 0.0f ? p_lspr->dynamic_sqrradius : 65536.0f);
#else
		float radius = p_lspr->dynamic_radius;
		float sqrradius = p_lspr->dynamic_sqrradius;
#endif
#if 1
		radius /= 2;
		sqrradius /= 2;
#endif
		if (radius <= 0.0f || sqrradius <= 0.0f)
		{
			continue;
		}

		// check bounding box first
#if 1
		if (SphereTouchBBox3D(&vtx[2], &vtx[0], light_pos, radius) == false)
		//if (SphereTouchBBox3D(&vtx[2], &vtx[0], light_pos, sqrradius) == false)
		{
			continue;
		}
#endif

		float d[4], s;
		d[0] = vtx[2].x - vtx[0].x;
		d[1] = vtx[2].z - vtx[0].z;
		d[2] = light_pos->x - vtx[0].x;
		d[3] = light_pos->z - vtx[0].z;

		// backface cull
		if (d[2]*d[1] - d[3]*d[0] < 0)
		{
			continue;
		}

		// check exact distance
#ifndef HWR_DISTP2D_ISSEG
		const float dist_p2d = HWR_DistP2D(&vtx[2], &vtx[0], light_pos, &inter);
#else
		const float dist_p2d = HWR_DistP2D(&vtx[2], &inter, curline);
#endif
		if (dist_p2d >= sqrradius)
		{
			continue;
		}

#define SQUARE sqrt
//#define SQUARE sqrtf
		d[0] = (float)SQUARE((vtx[0].x-inter.x)*(vtx[0].x-inter.x)
					+ (vtx[0].z-inter.z)*(vtx[0].z-inter.z));
		d[1] = (float)SQUARE((vtx[2].x-inter.x)*(vtx[2].x-inter.x)
					+ (vtx[2].z-inter.z)*(vtx[2].z-inter.z));

#if 0
		//INT32 dAB = sqrt((vtx[0].x-vtx[2].x)*(vtx[0].x-vtx[2].x)+(vtx[0].z-vtx[2].z)*(vtx[0].z-vtx[2].z));
		float dAB = sqrtf((vtx[0].x-vtx[2].x)*(vtx[0].x-vtx[2].x)+(vtx[0].z-vtx[2].z)*(vtx[0].z-vtx[2].z));
		if ((d[0] < dAB) && (d[1] < dAB)) // test if the intersection is on the wall
		{
			d[0] = -d[0]; // if yes, the left distance must be negative for texcoord
		}
#endif

		// test if the intersection is on the wall
		if ((vtx[0].x < inter.x && vtx[2].x > inter.x) ||
			(vtx[0].x > inter.x && vtx[2].x < inter.x) ||
			(vtx[0].z < inter.z && vtx[2].z > inter.z) ||
			(vtx[0].z > inter.z && vtx[2].z < inter.z))
		{
			d[0] = -d[0]; // if yes, the left distance must be negative for texcoord
		}
		d[2] = d[1]; d[3] = d[0];

#ifdef DL_HIGH_QUALITY
		s = 0.5f / radius;
#else
		s = 0.5f / sqrt(sqrradius - dist_p2d);
#endif

		for (unsigned int i = 0; i < 4; i++)
		{
			vtx[i].s = (float)(0.5f + d[i]*s);
			vtx[i].t = (float)(0.5f + (vtx[i].y - light_pos->y) * s * 1.2f);
		}

		HWR_DL_GetCoronaLighting(&Surf, NULL, (mobj_t *)light->actor);
		Surf.PolyColor = light->color;

#ifdef DL_HIGH_QUALITY
		// dist_p2d < lsp->dynamic_sqrradius
		INT32 alpha = clamp(1 - dist_p2d/sqrradius, 128, UINT8_MAX-1);
		Surf.PolyColor.s.alpha *= (UINT8)alpha;
		CONS_Debug(DBG_RENDER, "corona alpha is %d\n", alpha);
#else
		(void)dist_p2d;
#endif

		//HWR_GetPic(corona_lumpnum); // TODO: use different coronas

#if 1
		FBITFIELD blendmode = light->poly_flags|extra_blendmode;
		//blendmode |= PF_Translucent;
		//blendmode |= PF_Occlude;
		//blendmode |= PF_Masked;
		blendmode |= PF_Modulated;
		blendmode &= ~PF_Additive|PF_Blending|PF_Multiplicative;
		blendmode &= ~PF_Corona;
		blendmode &= ~PF_NoDepthTest;
		blendmode &= ~PF_NoAlphaTest;

		//blendmode |= ExtraPolyFlags;
		(void)PolyFlags;
		(void)ExtraPolyFlags;

		//blendmode = pSurf->PolyFlags;
#endif
#if 0
		FBITFIELD blendmode = pSurf->PolyFlags|PF_Decal;
		blendmode |= PolyFlags;
		//pSurf->PolyColor = Surf.PolyColor;
		//pSurf->SurfFlags |= SF_DYNLIGHT;
#endif

#if 0
		// Skip lighting for masked/transparent walls - middle textures, etc.
		if (blendmode & (PF_Masked | PF_Translucent))
			continue;
		// Skip drawing if the light is too faint (alpha too low)
		if (Surf.PolyColor.s.alpha < 16)
			continue;
#endif
#if 0
		// Important: Don't include PF_Masked from the original wall, as it uses destructive blending
		// that can wipe out the underlying texture. Only use safe flags like depth testing.
		blendmode |= (PolyFlags & ~(PF_Masked | PF_Translucent | PF_Modulated)); // Safe flags only
		blendmode |= (blendmode & ~(PF_Masked | PF_Translucent | PF_Modulated)); // Safe flags only
#endif

		Surf.SurfFlags |= SF_DYNLIGHT;

		//const boolean horizonSpecial = true;
		const boolean horizonSpecial = false;
#if 0
		// Move new verts into previous verts
		//M_Memcpy(wlVerts, vtx, sizeof(FOutVector) * 4);
		memmove(wlVerts, vtx, sizeof(FOutVector) * 4);
#endif

		HWR_SetLight();

		HWR_ProcessPolygon(&Surf, vtx, 4, blendmode, newShader, horizonSpecial);
		//HWR_ProcessPolygon(&Surf, wlVerts, 4, blendmode, newShader, horizonSpecial);
		//HWR_ProcessPolygon(pSurf, vtx, 4, blendmode, newShader, horizonSpecial);
		//HWR_ProcessPolygon(pSurf, wlVerts, 4, blendmode, newShader, horizonSpecial);
		(void)pSurf;
	}
	(void)DL_FindLongestXZEdge;
	(void)DL_ComputePlaneNormal;
}

#else

	// Per-vertex accumulators (start from surface base color)
	FSurfaceInfo Surf;
	float racc[4], gacc[4], bacc[4];
	RGBA_t baseColor = (pSurf ? pSurf->PolyColor : Surf.PolyColor);
	FDynamicLight *light;
	INT32 i;
	for (i = 0; i < 4; ++i)
	{
		racc[i] = (float)baseColor.s.red;
		gacc[i] = (float)baseColor.s.green;
		bacc[i] = (float)baseColor.s.blue;
	}
	Surf.PolyColor = baseColor;

	boolean anyLight = false;

	// Precompute a robust AABB for the wall quad (or strip).
	FOutVector bmin, bmax;
	DL_ComputeBBox(wlVerts, &bmin, &bmax, 4);

	FOutVector         vtx[4];
	memcpy(vtx, wlVerts, sizeof(FOutVector) * 4);
	for (light = dynlights_head; light; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		floatvector3_t *lightpos = &light->pos;

		if (light->visible == false)
			continue;

		// Quick reject: sphere vs wall AABB
#ifdef FIXES
		FOutVector minV = wlVerts[0], maxV = wlVerts[0];
		for (i = 1; i < 4; i++)
		{
			if (wlVerts[i].x < minV.x) minV.x = wlVerts[i].x;
			if (wlVerts[i].y < minV.y) minV.y = wlVerts[i].y;
			if (wlVerts[i].z < minV.z) minV.z = wlVerts[i].z;
			if (wlVerts[i].x > maxV.x) maxV.x = wlVerts[i].x;
			if (wlVerts[i].y > maxV.y) maxV.y = wlVerts[i].y;
			if (wlVerts[i].z > maxV.z) maxV.z = wlVerts[i].z;
		}
		if (!SphereTouchBBox3D(&minV, &maxV, lightpos, p_lspr->dynamic_radius))
			continue;
#else
		if (!SphereTouchBBox3D(&bmin, &bmax, lightpos, p_lspr->dynamic_radius))
			continue;
#endif

		// Backface test in XZ: project light relative to wall edge A->B
		{
			const float dx = B->x - A->x;
			const float dz = B->z - A->z;
			const float lx = lightpos->x - A->x;
			const float lz = lightpos->z - A->z;
			if ((lx*dz - lz*dx) < 0.0f)
				continue; // cull backside
		}

		// Exact distance from light to segment in XZ
		floatvector3_t inter;
#ifndef HWR_DISTP2D_ISSEG
		const float distSq = HWR_DistP2D(A, B, lightpos, &inter);
#else
		const float distSq = HWR_DistP2D(&wlVerts[2], &inter, curline);
#endif
		if (distSq >= p_lspr->dynamic_sqrradius)
			continue;

		// This light affects the wall; accumulate contribution per vertex
		anyLight = true;
		float radius = p_lspr->dynamic_radius > 0.0f ? p_lspr->dynamic_radius : sqrtf(p_lspr->dynamic_sqrradius);
		for (i = 0; i < 4; ++i)
		{
			float dx = vtx[i].x - lightpos->x;
			float dy = vtx[i].y - lightpos->y;
			float dz = vtx[i].z - lightpos->z;
			float dist = sqrtf(dx*dx + dy*dy + dz*dz);
			if (dist >= radius)
				continue;
			float att = 1.0f - (dist / radius); // linear attenuation
			// Add light color scaled by attenuation
			racc[i] += (float)light->color.s.red * att;
			gacc[i] += (float)light->color.s.green * att;
			bacc[i] += (float)light->color.s.blue * att;
		}
	}

	// If any lights affected the wall, draw a single polygon using per-vertex colors
	if (anyLight)
	{
		RGBA_t colors[4];
		for (i = 0; i < 4; ++i)
		{
			int r = (int)(racc[i] + 0.5f);
			int g = (int)(gacc[i] + 0.5f);
			int b = (int)(bacc[i] + 0.5f);

			if (r < 0) r = 0;
			if (r > 255) r = 255;

			if (g < 0) g = 0;
			if (g > 255) g = 255;

			if (b < 0) b = 0;
			if (b > 255) b = 255;

			colors[i].s.red = (UINT8)r;
			colors[i].s.green = (UINT8)g;
			colors[i].s.blue = (UINT8)b;
			// Preserve original alpha
			colors[i].s.alpha = (pSurf ? pSurf->PolyColor.s.alpha : Surf.PolyColor.s.alpha);
		}

		// Bind wall texture for proper rendering (HWR_SetLight was for corona texture; we keep wall texture)
		//HWR_SetLight();

		// Draw with per-vertex colors; ensure using safe flags
#if 1
		FBITFIELD blendmode = 0;
		//blendmode = light->poly_flags|extra_blendmode;
		blendmode |= PF_Additive;
		//blendmode |= PF_Translucent;
		//blendmode |= PF_Occlude;
		//blendmode |= PF_Masked;
		blendmode |= PF_Decal;
		blendmode |= PF_Modulated;
		//blendmode |= PF_Environment;

		//blendmode &= ~PF_Additive;
		//blendmode &= ~PF_Modulated;
		//blendmode &= ~PF_Decal;
		//blendmode &= ~PF_Corona;
		blendmode &= ~PF_ColorMapped;
#endif
		//FBITFIELD drawFlags = newPolyFlags & ~(PF_Masked | PF_Translucent | PF_Modulated);
		FBITFIELD drawFlags = blendmode & ~(PF_Masked | PF_Translucent | PF_Modulated);

		HWR_ProcessPolygon(&Surf, vtx, 4, drawFlags, newShader, false);
	}
}

#endif // #if 1 (or 0 depends)
#else // REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES

//#define FIXES
#define OUTVECTOR_FIX

void HWR_WallLighting(FSurfaceInfo *pSurf, FOutVector *wlVerts, FBITFIELD PolyFlags, FBITFIELD ExtraPolyFlags, int shader)
{
#ifdef NO_WALL_RENDERING
	return;
#endif

	CORONA_VAR_ONLY

#if 1
	// Skip if no dynamic lights
	if (corona_gl_patch.mipmap == NULL)
		return;
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

#if 0
	// Don't apply overlay lighting to masked/translucent walls
	if (PolyFlags & (PF_Masked | PF_Translucent))
		return;
#endif

	// Simple per-vertex additive lighting overlay using color arrays.
	// Avoids texture unit corruption by not binding any textures.

	FSurfaceInfo lightSurf;
	lightSurf.PolyColor.rgba = 0xFFFFFFFF;
	lightSurf.TintColor.rgba = 0x00000000;
	lightSurf.FadeColor.rgba = 0x00000000;

	// For each dynamic light in range
	for (FDynamicLight *light = dynlights_head; light != NULL; light = light->next)
	{
		light_t *p_light = light->light_data;
		floatvector3_t *light_pos = &light->pos;
		float light_radius = p_light->dynamic_radius;
		float light_radius_sq = light_radius * light_radius;

		if (light->visible == false)
			continue;
		if (light_radius <= 0.0f)
			continue;

		// Quick sphere-AABB test: does this light intersect the wall?
		boolean lights_wall = false;
		for (unsigned int v = 0; v < 4; v++)
		{
			float dx = wlVerts[v].x - light_pos->x;
			float dy = wlVerts[v].y - light_pos->y;
			float dz = wlVerts[v].z - light_pos->z;
			float dist_sq = dx*dx + dy*dy + dz*dz;

			if (dist_sq < light_radius_sq)
			{
				lights_wall = true;
				break;
			}
		}

		if (!lights_wall)
			continue;

		// Compute per-vertex colors based on distance attenuation
		RGBA_t vertColors[4];
		boolean has_visible_vertex = false;

		for (unsigned int v = 0; v < 4; v++)
		{
			float dx = wlVerts[v].x - light_pos->x;
			float dy = wlVerts[v].y - light_pos->y;
			float dz = wlVerts[v].z - light_pos->z;
			float dist_sq = dx*dx + dy*dy + dz*dz;

			if (dist_sq >= light_radius_sq)
			{
				// Outside light radius - no contribution
				vertColors[v].rgba = 0x00000000;
			}
			else
			{
				float dist = sqrtf(dist_sq);
				// Smooth quadratic falloff: full intensity at center, zero at edge
				float atten = 1.0f - (dist / light_radius);
				atten = atten * atten;

				// Apply light color with attenuation
				int r = (int)(light->color.s.red   * atten);
				int g = (int)(light->color.s.green * atten);
				int b = (int)(light->color.s.blue  * atten);
				int a = (int)(atten * 255.0f);

				vertColors[v].s.red   = (r > 255) ? 255 : r;
				vertColors[v].s.green = (g > 255) ? 255 : g;
				vertColors[v].s.blue  = (b > 255) ? 255 : b;
				vertColors[v].s.alpha = a;

				if (a > 0)
					has_visible_vertex = true;
			}
		}

		// Skip this light if no visible contribution
		if (!has_visible_vertex)
			continue;

		HWR_SetLight();

		// Draw lighting overlay with per-vertex colors using additive blending
		HWD.pfnDrawPolygon(&lightSurf, wlVerts, 4, PF_Additive | PF_Decal | PF_NoDepthTest);
	}
	return;
#endif

#ifdef DYN_LIGHT_VERTEX
	FOutVector         dlv[4];
	memcpy(dlv, wlVerts, sizeof(FOutVector) * 4);
#endif
	FSurfaceInfo Surf;
	FBITFIELD newPolyFlags = 0;
	int newShader = shader;
	unsigned int i, j;

	// Skip lighting for masked/transparent walls - middle textures, etc.
	if (PolyFlags & (PF_Masked | PF_Translucent))
		return;

	// Precompute a robust AABB for the wall quad (or strip).
	FOutVector bmin, bmax;
	DL_ComputeBBox(wlVerts, &bmin, &bmax, 4);

#ifndef OUTVECTOR_FIX
	// Direction vector along wall in XZ for projection
	//const FOutVector *A = &wlVerts[0]; // we'll treat [0]..[2] as the top edge
	//const FOutVector *B = &wlVerts[2];
	FOutVector *A = &wlVerts[0]; // we'll treat [0]..[2] as the top edge
	FOutVector *B = &wlVerts[2];
	// If your winding differs, consider locating the longest horizontal edge instead.
#else
	int ia, ib;
	DL_FindLongestXZEdge(wlVerts, &ia, &ib);
	//const FOutVector *A = &wlVerts[ia];
	//const FOutVector *B = &wlVerts[ib];
	FOutVector *A = &wlVerts[ia];
	FOutVector *B = &wlVerts[ib];
#endif

	// Choose draw settings
	//newPolyFlags |= LIGHTMAP_POLY_FLAGS;
	newPolyFlags |= PF_Additive;  // Use additive blending for lighting (brightens rather than darkens)
	newPolyFlags |= PolyFlags;     // Preserve original wall's poly flags for proper blending
	newPolyFlags |= ExtraPolyFlags;  // Include the passed-in extra flags (e.g., PF_Occlude)
	//newPolyFlags |= PF_Modulated;

	//newPolyFlags |= PF_NoDepthTest;
	//newPolyFlags |= PF_Occlude;

	//newPolyFlags |= PF_Masked;
	//newPolyFlags |= PF_Environment;

	//newPolyFlags |= PF_Decal;
	//newPolyFlags |= PF_Corona;

#if 1
	if (HWR_UseShader())
	{
		newShader = SHADER_WALL;
		newPolyFlags |= PF_ColorMapped;
	}
#endif

	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

	FDynamicLight *light;
	for (light = dynlights_head; light; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		floatvector3_t *lightpos = &light->pos;
		float   denom;

		if (light->visible == false)
			continue;

		// Quick reject: sphere vs wall AABB
#ifdef FIXES
		FOutVector minV = wlVerts[0], maxV = wlVerts[0];
		for (i = 1; i < 4; i++)
		{
			if (wlVerts[i].x < minV.x) minV.x = wlVerts[i].x;
			if (wlVerts[i].y < minV.y) minV.y = wlVerts[i].y;
			if (wlVerts[i].z < minV.z) minV.z = wlVerts[i].z;
			if (wlVerts[i].x > maxV.x) maxV.x = wlVerts[i].x;
			if (wlVerts[i].y > maxV.y) maxV.y = wlVerts[i].y;
			if (wlVerts[i].z > maxV.z) maxV.z = wlVerts[i].z;
		}
		if (!SphereTouchBBox3D(&minV, &maxV, lightpos, p_lspr->dynamic_radius))
			continue;
#else
		if (!SphereTouchBBox3D(&bmin, &bmax, lightpos, p_lspr->dynamic_radius))
			continue;
#endif

#if 1
		// Backface test in XZ: project light relative to wall edge A->B (comment out for two-sided lights)
		{
			const float dx = B->x - A->x;
			const float dz = B->z - A->z;
			const float lx = lightpos->x - A->x;
			const float lz = lightpos->z - A->z;
			// 2D cross >0 means light is "front" depending on winding; keep both sides?:
			if ((lx*dz - lz*dx) < 0.0f)
				continue; // cull backside
		}
#endif

		// Exact distance from light to segment in XZ
		floatvector3_t inter;
#ifndef HWR_DISTP2D_ISSEG
		const float distSq = HWR_DistP2D(A, B, lightpos, &inter);
#else
		const float distSq = HWR_DistP2D(&wlVerts[2], &inter, curline);
#endif
		if (distSq >= p_lspr->dynamic_sqrradius)
			continue;

		// Compute distances along wall to left/right edges for UV mapping
		// d[0]=left, d[1]=right, d[2]=bottom‑right, d[3]=bottom‑left (matching wlVerts order)
		float d[4];
		const float leftLen = sqrtf((wlVerts[0].x - inter.x)*(wlVerts[0].x - inter.x) +
									 (wlVerts[0].z - inter.z)*(wlVerts[0].z - inter.z));
		const float rightLen = sqrtf((wlVerts[2].x - inter.x)*(wlVerts[2].x - inter.x) +
									 (wlVerts[2].z - inter.z)*(wlVerts[2].z - inter.z));

		// Determine if intersection lies between A & B to flip sign for left side.
		// Use param t from projection (recompute quickly):
		float vx = (B->x - A->x), vz = (B->z - A->z);
		float segLenSq = ((vx*vx) + (vz*vz));
		float t = 0.0f;
		if (segLenSq > DL_SAFE_EPS)
			t = (((inter.x - A->x)*vx + (inter.z - A->z)*vz) / segLenSq);

		// Signed distances for S texture axis
		// Negative toward the "left" side (A end) when intersection is on the segment
		boolean onSeg = (t >= 0.0f && t <= 1.0f);
		float dLeft = (onSeg ? -leftLen : leftLen);
		float dRight = (onSeg ?  rightLen : -rightLen);
		d[0] = dLeft;
		d[1] = dRight;
		d[2] = dRight;
		d[3] = dLeft;

		// Scale factor: map world units into [0..1] tex domain around disc center 0.5
#ifdef DL_HIGH_QUALITY
		denom = p_lspr->dynamic_radius;
#else
		denom = sqrtf(max(p_lspr->dynamic_sqrradius - distSq, DL_SAFE_EPS));
#endif
		const float sScale = (0.5f / denom);
		const float tScale = (sScale * DL_TVERT_ASPECT);

		// Build a per‑light copy of the wall verts with S/T set
		FOutVector vtx[4];
		memcpy(vtx, wlVerts, sizeof(FOutVector) * 4);
		for (i = 0; i < 4; i++)
		{
			vtx[i].s = 0.5f + d[i]*sScale;
			vtx[i].t = 0.5f + (vtx[i].y - lightpos->y) * tScale;
		}
		//memcpy(wlVerts, vtx, sizeof(FOutVector) * 4);

		// Bind light texture
		HWR_SetLight();

		// Color
		//HWR_DL_GetCoronaLighting(&Surf, NULL, mo);
		//HWR_Lighting(&Surf, mo->subsector->sector->lightlevel, mo->subsector->sector->extra_colormap);
		// Instead of drawing the corona texture over the wall (which can obscure
		// middle/masked textures), draw an additive per-vertex color quad that
		// adds the light contribution to the wall without replacing its texture.
		Surf.PolyColor = light->color;

#ifdef DL_HIGH_QUALITY
		Surf.PolyColor.s.alpha *= (1.0f - distSq / p_lspr->dynamic_sqrradius);
#endif

		// Skip if too faint
		if (Surf.PolyColor.s.alpha < 16)
			continue;

		// Draw (use lightmap flags only; additive)
		HWR_ProcessPolygon(&Surf, vtx, 4, newPolyFlags, newShader, false);
	}
}

#endif // REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES

//#ifndef REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES
#if 1

// --------------------------------------------------------------------------
// calcul du dynamic lighting sur le sol
// clVerts contient les coords du sol avec le mlook (up/down)
// --------------------------------------------------------------------------
void HWR_PlaneLighting(FSurfaceInfo *pSurf, FOutVector *clVerts, size_t nrClipVerts, FBITFIELD PolyFlags, boolean horizonSpecial)
{
#ifdef NO_PLANE_RENDERING
	return;
#endif

	FDynamicLight *light;
	FOutVector p1, p2;
	//static FOutVector *vtx = NULL;
	FOutVector *vtx = NULL;
	INT32 shader = SHADER_NONE;

	CORONA_VAR_ONLY
	if (corona_gl_patch.mipmap == NULL)
	{
		return;
	}
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
	{
		return;
	}

	if (vtx == NULL)
	{
		vtx = calloc(nrClipVerts, sizeof(FOutVector));
		if (vtx == NULL)
		{
			return;
		}
	}

	FBITFIELD extra_poly_flags = 0;
	if (HWR_UseShader())
	{
		//if (!HWR_ShouldUsePaletteRendering())
		{
			if (PolyFlags & PF_Fog)
			{
				//shader = SHADER_FOG;
				;
			}
			else if (PolyFlags & PF_Ripple)
			{
				//shader = SHADER_WATER;
				;
			}
			else
			{
				//shader = SHADER_FLOOR;
				;
			}
		}
		extra_poly_flags |= PF_ColorMapped;
	}

#if 0
	// AABB for broad‑phase reject
#if 1
	p1.z = FixedToFloat(hwbbox[BOXTOP   ]);
	p1.x = FixedToFloat(hwbbox[BOXLEFT  ]);
	p2.z = FixedToFloat(hwbbox[BOXBOTTOM]);
	p2.x = FixedToFloat(hwbbox[BOXRIGHT ]);
#endif
#if 0
	p1.z = FixedToFloat(hwbbox[BOXTOP   ]>>FRACBITS);
	p1.x = FixedToFloat(hwbbox[BOXLEFT  ]>>FRACBITS);
	p2.z = FixedToFloat(hwbbox[BOXBOTTOM]>>FRACBITS);
	p2.x = FixedToFloat(hwbbox[BOXRIGHT ]>>FRACBITS);
#endif
#if 0
	p1.z = hwbbox[BOXTOP   ]>>FRACBITS;
	p1.x = hwbbox[BOXLEFT  ]>>FRACBITS;
	p2.z = hwbbox[BOXBOTTOM]>>FRACBITS;
	p2.x = hwbbox[BOXRIGHT ]>>FRACBITS;
#endif
#if 0
	p1.z = hwbbox[BOXTOP   ];
	p1.x = hwbbox[BOXLEFT  ];
	p2.z = hwbbox[BOXBOTTOM];
	p2.x = hwbbox[BOXRIGHT ];
#endif
	p1.y = p2.y = vtx[0].y; // DEFAULT: ON
	//DL_ComputeBBox(vtx, &p1, &p2, nrClipVerts);
	(void)DL_ComputeBBox;
#endif
#if 0
	#define FACTOR 8.0f

	//#define MATH_PROBLEM *=
	#define MATH_PROBLEM /=

	#define NEW_MATH_PROBLEM *=
	//#define NEW_MATH_PROBLEM /=

	p1.x MATH_PROBLEM FACTOR;
	//p1.y MATH_PROBLEM FACTOR;
	p1.z MATH_PROBLEM FACTOR;

	p2.x NEW_MATH_PROBLEM FACTOR;
	//p2.y NEW_MATH_PROBLEM FACTOR;
	p2.z NEW_MATH_PROBLEM FACTOR;
#endif

	FSurfaceInfo Surf;
	for (light = dynlights_head; light != NULL; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		floatvector3_t *light_pos = &light->pos;
		FBITFIELD blend = light->poly_flags|extra_poly_flags;
		UINT8 alpha;

		floatvector3_t N;
		boolean haveNormal = false;
		fixed_t floordiff;
		float dist_p2d, denom;
		size_t vert;

		if (light->visible == false)
		{
			continue;
		}

		M_Memcpy(vtx, clVerts, sizeof(FOutVector) * nrClipVerts);

#if 1
	// AABB for broad‑phase reject
#if 1
	p1.z = FixedToFloat(hwbbox[BOXTOP   ]);
	p1.x = FixedToFloat(hwbbox[BOXLEFT  ]);
	p2.z = FixedToFloat(hwbbox[BOXBOTTOM]);
	p2.x = FixedToFloat(hwbbox[BOXRIGHT ]);
#endif
#if 0
	p1.z = FixedToFloat(hwbbox[BOXTOP   ]>>FRACBITS);
	p1.x = FixedToFloat(hwbbox[BOXLEFT  ]>>FRACBITS);
	p2.z = FixedToFloat(hwbbox[BOXBOTTOM]>>FRACBITS);
	p2.x = FixedToFloat(hwbbox[BOXRIGHT ]>>FRACBITS);
#endif
#if 0
	p1.z = hwbbox[BOXTOP   ]>>FRACBITS;
	p1.x = hwbbox[BOXLEFT  ]>>FRACBITS;
	p2.z = hwbbox[BOXBOTTOM]>>FRACBITS;
	p2.x = hwbbox[BOXRIGHT ]>>FRACBITS;
#endif
#if 0
	p1.z = hwbbox[BOXTOP   ];
	p1.x = hwbbox[BOXLEFT  ];
	p2.z = hwbbox[BOXBOTTOM];
	p2.x = hwbbox[BOXRIGHT ];
#endif
	p1.y = p2.y = vtx[0].y; // DEFAULT: ON
	//DL_ComputeBBox(vtx, &p1, &p2, nrClipVerts);
	(void)DL_ComputeBBox;
#endif
#if 0
	#define FACTOR 8.0f

	//#define MATH_PROBLEM *=
	#define MATH_PROBLEM /=

	#define NEW_MATH_PROBLEM *=
	//#define NEW_MATH_PROBLEM /=

	p1.x MATH_PROBLEM FACTOR;
	//p1.y MATH_PROBLEM FACTOR;
	p1.z MATH_PROBLEM FACTOR;

	p2.x NEW_MATH_PROBLEM FACTOR;
	//p2.y NEW_MATH_PROBLEM FACTOR;
	p2.z NEW_MATH_PROBLEM FACTOR;
#endif

		blend &= ~PF_Corona;
		blend &= ~PF_NoDepthTest;
		blend &= ~PF_NoAlphaTest;

		floordiff = abs(FloatToFixed(vtx[0].y) - FloatToFixed(light_pos->y));
		alpha = clamp((floordiff / (4*FRACUNIT) + 75), 0, 255);
		alpha = (255 - alpha);

		if (alpha <= 0)
		{
			continue;
		}

#if 0
		// Precompute plane normal for backface test (slope‑aware)
		haveNormal = DL_ComputePlaneNormal(vtx, nrClipVerts, &N);
#endif

#if 0
		float radius = (p_lspr->dynamic_radius > 0.0f ? p_lspr->dynamic_radius : 256.0f);
		float sqrradius = (p_lspr->dynamic_sqrradius > 0.0f ? p_lspr->dynamic_sqrradius : 65536.0f);
#else
		float radius = p_lspr->dynamic_radius;
		float sqrradius = p_lspr->dynamic_sqrradius;
#endif
#if 1
		radius /= 2;
		sqrradius /= 2;
#endif
		if (radius <= 0.0f || sqrradius <= 0.0f)
		{
			continue;
		}

#if 1
		// StarManiaKG:
		// Kinda broken, might just disable all BBox checking for now...
		// Leave room for a lot of lag, but it's worth it cause they look nice...

		//const float float_radius = radius; // default before update
		const float float_radius = radius*2; // current default
		//const float float_radius = sqrradius;
		//const float float_radius = sqrradius/2;
		fixed_t x = FloatToFixed(light_pos->x);
		fixed_t y = FloatToFixed(light_pos->y);
		fixed_t z = FloatToFixed(light_pos->z);
		fixed_t fixed_radius = FloatToFixed(float_radius);
#if 1
		// BP: The kickass Optimization: check if light touch bounding box
		if (!(PolyFlags & PF_Translucent))
		{
			if (SphereTouchBBox3D(&p1, &p2, light_pos, float_radius) == false)
			{
				continue;
			}
		}
#endif
#if 0
		// check bounding box first
		//CONS_Printf("light data is: %f, %f, %f\n", light_pos->x, light_pos->y, light_pos->z);
		//CONS_Printf("dynamic radius is %f, %d\n", float_radius, fixed_radius);
		if (M_CircleTouchBox(hwbbox, x, z, fixed_radius) == false)
		{
			continue;
		}
#endif
#if 0
		// check bounding box first
		if (HWR_CheckBBox(hwbbox) == false)
		{
			continue;
		}
#endif

		(void)x;
		(void)y;
		(void)z;
		(void)float_radius;
		(void)fixed_radius;
#endif

		(void)p1;
		(void)p2;
		//CONS_Printf("passed radius check!\n");

#if 0
		// Precompute plane normal for backface test (slope‑aware)
		haveNormal = DL_ComputePlaneNormal(vtx, nrClipVerts, &N);
#endif

#if 0
#if 0
		// For legacy callers we still need fixed hwbbox based fallback; we’ll rely on computed AABB.
		// Slope‑aware backface (if we have a valid normal)
		if (haveNormal)
		{
			// Vector from plane point to light
			float lx = light_pos->x - vtx->x;
			float ly = light_pos->y - vtx->y;
			float lz = light_pos->z - vtx->z;
			const float side = lx*N.x + ly*N.y + lz*N.z;

			// Vertical distance (signed) from light to plane for scale calc.
			// If we have normal, use true perpendicular distance; else fallback to Y diff.
			dist_p2d = side * side;

#if 0
			// View‑facing determination: we assume vtx is wound CCW as seen by camera.
			// If side < 0 the light is behind the plane (no contribution unless 2‑sided).
			if (side < 0.0f)
				continue;
#endif
		}
		else
#endif
#if 1
		{
#if 1
			// Fallback legacy vertical test (rare)
			if ((vtx[0].y > atransform.z) ^ (light_pos->y < vtx[0].y))
				continue;
#endif
			// Vertical distance (signed) from light to plane for scale calc.
			// If we have normal, use true perpendicular distance; else fallback to Y diff.
			float dy = (vtx[0].y - light_pos->y);
			dist_p2d = dy*dy;
		}
#endif
#endif

		//CONS_Debug(DBG_RENDER, "rendering light %d, %d\n", light, p_lspr->type);
		//CONS_Printf("light data is: %f, %f, %f\n", light_pos->x, light_pos->y, light_pos->z);
		//CONS_Printf("dynamic radius is %f, %d\n", radius, FloatToFixed(p_lspr->dynamic_radius));

#if 1
		// backface cull
		// Hurdler: doesn't work with new TANDL code
		//
		// StarManiaKG: To explain it in another way, think of water.
		// Without this block of code, it allows coronas to shine through the floor and ceiling of FOFs.
		// Technically, we DO want that for water.
		// But for the meantime, this stays.
		// It does more good and prevents more visual bugs than it causes.
		// Eventually though, I will get rid of it for water FOFs.
		//
		if (!(PolyFlags & PF_Translucent))
		//if (!(PolyFlags & (PF_Translucent|PF_Fog|PF_Additive|PF_Subtractive|PF_ReverseSubtract|PF_Multiplicative|PF_Environment)))
		{
			if ((vtx[0].y > atransform.z)   	// true mean it is a ceiling false is a floor
				^ (light_pos->y < vtx[0].y))	// true mean light is down plane, false light is up plane
			{
				continue;
			}
		}
#endif

#if 1
		dist_p2d = (vtx[0].y - light_pos->y);
		dist_p2d *= dist_p2d;
#endif

#if 0
		// done in SphereTouchBBox3D
		//if (dist_p2d >= sqrradius)
		if (dist_p2d/25 >= sqrradius)
		{
			//CONS_Printf("HWR_PlaneLighting() - Invalid sqrraudis!\n");
			continue;
		}
#endif

#if 0
		// Debug
		//CONS_Debug(DBG_RENDER, "DL plane: n=%d y0=%.1f..%.1f lightY=%.1f r=%.1f dist=%.1f culled=%d\n",
		CONS_Printf("DL plane: n=%d y0=%.1f..%.1f lightY=%.1f r=%.1f dist=%.1f culled=%d\n",
			(int)nrClipVerts,
			p1.y, p2.y,
			light_pos->y,
			radius,
			sqrtf(dist_p2d),
			(boolean)(dist_p2d >= sqrradius));
#endif

#ifdef DL_HIGH_QUALITY
		denom = 0.5f / radius;
#else
		denom = 0.5f / sqrt(sqrradius - dist_p2d);
#endif

		for (vert = 0; vert < nrClipVerts; vert++)
		{
			vtx[vert].s = 0.5f + (vtx[vert].x - light_pos->x) * denom;
			vtx[vert].t = 0.5f + (vtx[vert].z - light_pos->z) * denom * 1.2f;
		}
#if 0
#if 0
		haveNormal = DL_ComputePlaneNormal(vtx, nrClipVerts, &N);
#endif
		//float nudge = 0.1f; // world units; tweak
		//float nudge = 1.0f; // world units; tweak
		float nudge = 2.0f; // world units; tweak
		//if (haveNormal)
		{
			for (vert = 0; vert < nrClipVerts; vert++)
			{
				vtx[vert].x += N.x * nudge;
				vtx[vert].y += N.y * nudge;
				vtx[vert].z += N.z * nudge;
			}
		}
#endif

#if 0
#ifdef DL_HIGH_QUALITY
		// dist_p2d < lsp->dynamic_sqrradius
#if 0
		UINT32 alpha = (1 - dist_p2d/sqrradius);
		if (alpha < 128)
			alpha = 128;
		if (alpha < 0)
			alpha = 0;
		if (alpha > UINT8_MAX)
			alpha = UINT8_MAX;
		Surf.PolyColor.s.alpha *= alpha;
		CONS_Debug(DBG_RENDER, "corona alpha is %d\n", alpha);
#else
		int32_t multiplier = 2; // 1 // 2 // 6
		//Surf.PolyColor.s.alpha *= (UINT8)(1 - dist_p2d/sqrradius)*multiplier;

		//Surf.PolyColor.s.alpha *= (1 - dist_p2d/sqrradius)*multiplier;
		//Surf.PolyColor.s.alpha *= (1 - (dist_p2d/sqrradius))*multiplier;
		//Surf.PolyColor.s.alpha *= (UINT8)((1 - (dist_p2d/sqrradius))*multiplier);

		//Surf.PolyColor.s.alpha *= FixedMul(((mobj_t *)(light->actor))->alpha, (1 - dist_p2d/sqrradius));
		(void)multiplier;
#endif

		//alpha = (1 - dist_p2d/sqrradius);
#endif
#endif

		//HWR_DL_GetCoronaLighting(&Surf, light->sector, (mobj_t *)light->actor);
		Surf.PolyColor.s.alpha = alpha;
		Surf.PolyColor = light->color;

		HWR_SetLight();

#if 0
		// Move new verts into previous verts
		M_Memcpy(clVerts, vtx, sizeof(FOutVector) * nrClipVerts);
		//memmove(clVerts, vtx, sizeof(FOutVector) * nrClipVerts);
#endif

		Surf.SurfFlags = SF_DYNLIGHT;

		//horizonSpecial = false;
#if 1
		HWR_ProcessPolygon(&Surf, vtx, nrClipVerts, blend, shader, horizonSpecial); // DEFAULT
		//HWR_ProcessPolygon(&Surf, clVerts, nrClipVerts, blend, shader, horizonSpecial);
#endif
#if 0
		pSurf->PolyColor = Surf.PolyColor;
		pSurf->SurfFlags |= Surf.SurfFlags;
		HWR_ProcessPolygon(pSurf, vtx, nrClipVerts, blend, shader, horizonSpecial);
		//HWR_ProcessPolygon(pSurf, clVerts, nrClipVerts, blend, shader, horizonSpecial);
#endif

#if 0
		//HWD.pfnSetBlend(blend &~ PF_Occlude);
		//HWD.pfnSetBlend(blend|PF_Occlude);
		//HWD.pfnSetBlend(blend|PF_Translucent);
		HWD.pfnSetBlend(blend|PF_Translucent|PF_NoDepthTest|PF_Modulated);

		//HWR_ProcessPolygon(&Surf, vtx, nrClipVerts, blend, shader, horizonSpecial);
		HWR_ProcessPolygon(&Surf, clVerts, nrClipVerts, blend, shader, horizonSpecial);

		HWD.pfnSetBlend(blend|PF_Translucent|PF_NoDepthTest|PF_Modulated);
		//HWD.pfnSetBlend(blend);
		//HWD.pfnSetBlend(blend|PF_Occlude);
#endif

		(void)N;
		(void)haveNormal;
		(void)dist_p2d;

		(void)horizonSpecial;
	}
#if 1
	free(vtx);
#endif
	(void)pSurf;
}

#else // REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES

#define FIXES

void HWR_PlaneLighting(FSurfaceInfo *pSurf, FOutVector *clVerts, size_t nrClipVerts, FBITFIELD PolyFlags, boolean horizonSpecial)
{
#ifdef NO_PLANE_RENDERING
	return;
#endif

	FSurfaceInfo Surf;
	floatvector3_t N;
	FOutVector bmin, bmax;
	FBITFIELD newPolyFlags = 0;
	int newShader = SHADER_NONE;
	size_t i;

#ifndef FIXES
	// Precompute plane reference point
	//const FOutVector *P0 = &clVerts[0];
	FOutVector *P0 = &clVerts[0];
#endif
	// Precompute plane normal for backface test (slope‑aware)
	boolean haveNormal = DL_ComputePlaneNormal(clVerts, nrClipVerts, &N);
#ifdef FIXES
	// Precompute plane reference point
	//const FOutVector *P0 = &clVerts[0];
	FOutVector *P0 = &clVerts[0];
#endif

	// AABB for broad‑phase reject
	DL_ComputeBBox(clVerts, &bmin, &bmax, nrClipVerts);

	// Choose draw settings
#if 1
	if (HWR_UseShader())
	{
		//newShader = SHADER_FLOOR;
		newPolyFlags |= PF_ColorMapped;
	}
#endif

	// For legacy callers we still need fixed hwbbox based fallback; we’ll rely on computed AABB.

	// Iterate through each light using the linked-list head.
	// The old array `dynlights_list` has been retired; rely on the
	// authoritative `dynlights_head` to avoid stale pointers.
	CORONA_VAR_ONLY
	if (corona_gl_patch.mipmap == NULL)
		return;
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

	FDynamicLight *light;
	for (light = dynlights_head; light; light = light->next)
	{
		light_t *p_lspr           = light->light_data;
		floatvector3_t *lightpos = &light->pos;
		mobj_t  *mo               = (mobj_t *)light->actor;
		float   distSq, denom;

		if (light->visible == false)
			continue;

#if 0
		// Broad‑phase
		if (!SphereTouchBBox3D(&bmin, &bmax, lightpos, p_lspr->dynamic_radius))
			continue;
#endif

#if 1
		// Slope‑aware backface (if we have a valid normal)
		if (haveNormal)
		{
			// Vector from plane point to light
			float lx = lightpos->x - P0->x;
			float ly = lightpos->y - P0->y;
			float lz = lightpos->z - P0->z;
			float side = lx*N.x + ly*N.y + lz*N.z;

			// Vertical distance (signed) from light to plane for scale calc.
			// If we have normal, use true perpendicular distance; else fallback to Y diff.
			float dperp = lx*N.x + ly*N.y + lz*N.z; // signed distance
			distSq = dperp * dperp;

#if 0
			// View‑facing determination: we assume clVerts are wound CCW as seen by camera.
			// If side < 0 the light is behind the plane (no contribution unless 2‑sided).
			if (side < 0.0f)
				continue;
#else
			(void)side;
#endif
		}
		else
#else
		(void)P0;
#endif
		{
#if 0
			// Fallback legacy vertical test (rare)
			if ((clVerts[0].y > atransform.z) ^ (lightpos->y < clVerts[0].y))
				continue;
#endif
			// Vertical distance (signed) from light to plane for scale calc.
			// If we have normal, use true perpendicular distance; else fallback to Y diff.
			float dy = (clVerts[0].y - lightpos->y);
			distSq = dy*dy;
		}

#if 1
		// Instead of using clVerts[0].y, compute distance for each vertex.
		float minY = clVerts[0].y;
		for (i = 1; i < nrClipVerts; i++) {
			if (clVerts[i].y < minY) minY = clVerts[i].y;
		}
		float planeY = minY; // approximate plane height for distance
		distSq = (planeY - lightpos->y);
		distSq *= distSq;
#endif
#if 0
		// Debug
		//CONS_Debug(DBG_RENDER, "DL plane: n=%d y0=%.1f..%.1f lightY=%.1f r=%.1f dist=%.1f culled=%d\n",
		CONS_Printf("DL plane: n=%d y0=%.1f..%.1f lightY=%.1f r=%.1f dist=%.1f culled=%d\n",
			(int)nrClipVerts,
			bmin.y, bmax.y,
			lightpos->y,
			p_lspr->dynamic_radius,
			sqrtf(distSq),
			(boolean)(distSq >= p_lspr->dynamic_radius));
#endif
#if 0
		if (distSq >= p_lspr->dynamic_sqrradius)
			continue;
#endif

		// Scale factors
#ifdef DL_HIGH_QUALITY
		denom = p_lspr->dynamic_radius;
#else
		denom = sqrtf(max(p_lspr->dynamic_sqrradius - distSq, DL_SAFE_EPS));
#endif
		const float sScale = 0.5f / denom;
		const float tScale = sScale * DL_TVERT_ASPECT;

		// Per‑light copy
		FOutVector *vtx = (FOutVector *)malloc(sizeof(FOutVector) * nrClipVerts);
		memcpy(vtx, clVerts, (sizeof(FOutVector) * nrClipVerts));

		// Map plane verts around the light center in XZ
		for (i = 0; i < nrClipVerts; i++)
		{
			vtx[i].s = (0.5f + ((vtx[i].x - lightpos->x) * sScale));
			vtx[i].t = (0.5f + ((vtx[i].z - lightpos->z) * tScale));
		}

#if 1
		//float nudge = 0.1f; // world units; tweak
		float nudge = 1.0f; // world units; tweak
		//if (haveNormal)
		{
			for (i = 0; i < nrClipVerts; i++)
			{
				vtx[i].x += N.x * nudge;
				vtx[i].y += N.y * nudge;
				vtx[i].z += N.z * nudge;
			}
		}
#endif
#if 0
		//float floorheight = P_FloorHeightAtPos(mo->x, mo->y, mo->subsector);
		for (i = 0; i < nrClipVerts; i++)
		{
			vtx[i].y = mo->z + (mo->height / 2.0f);
			//if (vtx[i].y < floorheight + 0.1f)
			//	vtx[i].y = floorheight + 0.1f;
		}
#endif

		// Bind texture
		HWR_SetLight();

		// Color
		//HWR_DL_GetCoronaLighting(&Surf, NULL, mo);
		//HWR_Lighting(&Surf, mo->subsector->sector->lightlevel, mo->subsector->sector->extra_colormap);

#ifdef DL_HIGH_QUALITY
		Surf.PolyColor.s.alpha *= (1.0f - distSq / p_lspr->dynamic_sqrradius);
#endif

		FBITFIELD blend = light->poly_flags|newPolyFlags;
		blend &= ~PF_Decal;
		blend &= ~PF_Corona;
		blend &= ~PF_NoDepthTest;
		blend &= ~PF_NoAlphaTest;
		//blend |= Surf.PolyFlags;

		HWR_ProcessPolygon(&Surf, vtx, nrClipVerts, blend, newShader, horizonSpecial);
		free(vtx);
	}
}

#endif // REMASTERED_CORONA_LOCATIONS_AND_BOUNDARIES

// --------------------------------------------------------------------------
// coronas lighting with the sprite
// --------------------------------------------------------------------------
void HWR_DoCoronasLighting(FOutVector *outVerts, gl_vissprite_t *spr)
{
	float               cx = 0.0f, cy = 0.0f, cz = 0.0f; // gravity center
	FSurfaceInfo	    Surf;
	FOutVector          lightVerts[4];
	light_t   		    *p_lspr;
	sprite_light_data_t spr_light_data;

#if 0
	STATIC_ONLY
	// Objects which emit light.
	// dynlights->num_lights == 0 if dynamic lighting is off or has errored
	if (dynlights->num_lights <= 0)
	{
		return;
	}

	//Sprite_Corona_Light_setup(spr);
	p_lspr = Sprite_Corona_Light_lsp(spr->mobj->sprite);
	//if (p_lspr == NULL || spr->mobj == NULL)
	if (p_lspr == NULL)
		return;
#if 0
	if (!(p_lspr->type & (CORONA_SPR|TYPE_FIELD_SPR)))
		return;
#endif

#if 0
	//p_lspr = dynlights->p_lspr[j];
	floatvector3_t *light_pos = &dynlights->pos[j];

	for (int j = 0; j < dynlights->num_lights; j++)
#endif

	{
		cz = ((outVerts[0].z + outVerts[2].z) / 2.0);
#if 0
		if (cz >= CORONA_Z2)
			return;
#endif

#if 0
#if 0
		// transform light positions
		cx = light_pos->x;
		cy = light_pos->y;
		cz = light_pos->z; // gravity center
		HWR_Transform(&cx, &cy, &cz);
#else
		//transform_world_to_gr(/*IN*/ light_pos->x, light_pos->y, light_pos->z, /*OUT*/ &cx, &cy, &cz);
		transform_world_to_gr(/*IN*/ cx, cy, cz, /*OUT*/ &cx, &cy, &cz);
#endif
#endif

#if 1
		HWR_Transform_CB(&cx, &cy, &cz, false);
		//HWR_Transform_CB(&cx, &cy, &cz, true);
#endif
#if 1
		HWR_set_view_transform(&atransform);
#endif
#if 0
		// fastest, use transform terms in optimized shared code
		// Combined transforms for look up/down and scaling
		//float topty = spr->ty - gpatch->height;
		float topty = spr->gz - spr->gpatch->height;
		outVerts[0].x = outVerts[3].x = (spr->x1 * sprite_trans_x_to_x);
		outVerts[1].x = outVerts[2].x = (spr->x2 * sprite_trans_x_to_x);
		float tranzy = spr->tz * sprite_trans_z_to_y;
		outVerts[0].y = outVerts[1].y = (topty * sprite_trans_y_to_y) + tranzy;
		outVerts[2].y = outVerts[3].y = (spr->gz * sprite_trans_y_to_y) + tranzy;
		float tranzz = spr->tz * sprite_trans_z_to_z;
		outVerts[0].z = outVerts[1].z = (topty * sprite_trans_y_to_z) + tranzz;
		outVerts[2].z = outVerts[3].z = (spr->gz * sprite_trans_y_to_z) + tranzz;
#endif

#if 1
		// mobj dependent light selector
		if (Sprite_Corona_Light_fade(p_lspr, cz, spr->mobj, &spr_light_data) == false)
			return;
#endif

		// Sprite has a corona, and coronas are enabled.
		float size = (spr_light_data.size * 2.0);
		//Surf.PolyColor.rgba = dynlights->color[j];
		if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(spr->mobj, &Surf.PolyColor, NULL, false))
			Surf.PolyColor.rgba = p_lspr->corona_color;
		Surf.PolyColor.s.alpha = spr_light_data.alpha;

#if 1
#if 0
		// compute position doing average
		cx = (outVerts[0].x + outVerts[2].x) / 2.0;
		cy = (outVerts[0].y + outVerts[2].y) / 2.0;
#else
		//cx = 0.0f, cy = 0.0f; // gravity center
		// compute position doing average
		for (unsigned int i = 0; i < 4; i++)
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

#endif

#if 1
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
		lightVerts[0].x = lightVerts[3].x = cx - size;
		lightVerts[1].x = lightVerts[2].x = cx + size;
		lightVerts[0].y = lightVerts[1].y = cy - (size*1.33f);
		lightVerts[2].y = lightVerts[3].y = cy + (size*1.33f);
		lightVerts[0].z = lightVerts[1].z = lightVerts[2].z = lightVerts[3].z = cz;
		lightVerts[0].s = lightVerts[0].t = 0.0f;
		lightVerts[1].s = 1.0f; lightVerts[1].t = 0.0f;
		lightVerts[2].s = lightVerts[2].t = 1.0f;
		lightVerts[3].s = 0.0f; lightVerts[3].t = 1.0f;
#else
#if 0
		// put light little forward the sprite so there is no
		// z-buffer problem (coplanar polygons)
		// BP: use PF_Decal do not help :(
		cz = cz - 5.0f;

		lightVerts[0].x = cx-size;  lightVerts[0].z = cz;
		lightVerts[0].y = cy-size*1.33f;
		lightVerts[0].s = 0.0f;   lightVerts[0].t = 0.0f;

		lightVerts[1].x = cx+size;  lightVerts[1].z = cz;
		lightVerts[1].y = cy-size*1.33f;
		lightVerts[1].s = 1.0f;   lightVerts[1].t = 0.0f;

		lightVerts[2].x = cx+size;  lightVerts[2].z = cz;
		lightVerts[2].y = cy+size*1.33f;
		lightVerts[2].s = 1.0f;   lightVerts[2].t = 1.0f;

		lightVerts[3].x = cx-size;  lightVerts[3].z = cz;
		lightVerts[3].y = cy+size*1.33f;
		lightVerts[3].s = 0.0f;   lightVerts[3].t = 1.0f;
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

		lightVerts[0].x = lightVerts[3].x = cx - size;
		lightVerts[1].x = lightVerts[2].x = cx + size;
		lightVerts[0].y = lightVerts[1].y = cy - (size*1.33f);
		lightVerts[2].y = lightVerts[3].y = cy + (size*1.33f);
		lightVerts[0].z = lightVerts[1].z = lightVerts[2].z = lightVerts[3].z = cz;
		lightVerts[0].s = 0.0f;   lightVerts[0].t = 0.0f;
		lightVerts[1].s = 1.0f;   lightVerts[1].t = 0.0f;
		lightVerts[2].s = 1.0f;   lightVerts[2].t = 1.0f;
		lightVerts[3].s = 0.0f;   lightVerts[3].t = 1.0f;
#endif
#endif

		HWR_GetPic(corona_lumpnum, NULL); // TODO: use different coronas
		//HWR_ProcessPolygon(&Surf, lightVerts, 4, dynlights->poly_flags[j], SHADER_NONE, false);
		if (spr->mobj->type == MT_PLAYER)
			HWR_ProcessPolygon(&Surf, lightVerts, 4, LIGHTMAP_POLY_FLAGS | PF_Additive | PF_NoDepthTest, SHADER_NONE, false);
		else
			HWR_ProcessPolygon(&Surf, lightVerts, 4, LIGHTMAP_POLY_FLAGS | PF_Additive, SHADER_NONE, false);
	}
#else
	(void)outVerts;
	(void)spr;

	(void)cx;
	(void)cy;
	(void)cz;
	(void)Surf;
	(void)lightVerts;
	(void)p_lspr;
	(void)spr_light_data;
#endif
}

// Draw coronas from dynamic light list
#if 1

void HWR_DL_Draw_Coronas(void)
{
#ifdef NO_CORONA_RENDERING
	return;
#endif

	FDynamicLight *light;
	FSurfaceInfo Surf;
	FOutVector lightVerts[4];
	FBITFIELD extra_blends = 0;
	INT32 shader = SHADER_NONE;
	const boolean horizonSpecial = false;

	DYNAMIC_ONLY
	if (corona_lumpnum == LUMPERROR)
	{
		return;
	}
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
	{
		return;
	}

#if 1
	if (HWR_UseShader())
	{
		//shader = SHADER_SPRITE;
		extra_blends |= PF_ColorMapped;
	}
#endif

#if 1
	//extra_blends |= PF_Occlude;
	//extra_blends |= PF_Environment;
	//extra_blends |= PF_Masked;
	//extra_blends |= PF_NoTexture;
	//extra_blends |= PF_RemoveYWrap;
#endif

	// TODO: use different coronas
#if 1
#if 1
	HWR_GetPic(corona_lumpnum, NULL);
#else
#if 1
	corona_gl_patch = (GLPatch_t *)Patch_AllocateHardwarePatch(corona_patch);
	if (!corona_gl_patch->mipmap->downloaded || !corona_gl_patch->mipmap->data)
	{
		corona_patch->width = corona_patch->height = 128;
		corona_gl_patch->mipmap->width = corona_gl_patch->mipmap->height = 128;
		corona_gl_patch->max_s = ((float)corona_patch->width / (float)corona_gl_patch->mipmap->width);
		corona_gl_patch->max_t = ((float)corona_patch->height / (float)corona_gl_patch->mipmap->height);
		corona_gl_patch->mipmap->flags = LIGHTMAP_MIPMAP_FLAGS;
		corona_gl_patch->mipmap->format = GL_TEXFMT_ALPHA_INTENSITY_88;
	}
#endif

#if 0
	HWR_GetPatch(corona_patch);
#endif
#if 0
	if (!corona_gl_patch->mipmap->downloaded)
		HWD.pfnSetTexture(corona_gl_patch->mipmap);
	HWR_SetCurrentTexture(corona_gl_patch->mipmap);
#endif
#if 1
	HWR_MakePatch(corona_patch, corona_gl_patch, corona_gl_patch->mipmap, true);
	//HWR_GetPatch(corona_patch);
#endif

	//Z_ChangeTag(corona_gl_patch.mipmap->data, PU_HWRCACHE_UNLOCKED);
	//Z_Free(corona_gl_patch.mipmap->data);
#endif
#endif

	for (light = dynlights_head; light != NULL; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		//const patch_t *corona = corona_patch;
		sprite_light_data_t spr_light_data;
		const floatvector3_t *light_pos = &light->pos;

		FBITFIELD blend = light->poly_flags|extra_blends;
		float cx = 0.0f, cy = 0.0f, cz = 0.0f; // gravity center
		float size, alpha;
		const UINT8 *colormap = light->colormap;

#if 1
		//if (!(p_lspr->type & CORONA_SPR))
		if (!(p_lspr->type & (CORONA_SPR|TYPE_FIELD_SPR)))
			continue;
#endif

		if (light->visible == false)
		{
			continue;
		}

		// transform light positions
#if 0
		cx = light_pos->x;
		cy = light_pos->y;
		cz = light_pos->z; // gravity center
		//HWR_Transform(&cx, &cy, &cz);
		//HWR_OG_Transform(&cx, &cy, &cz);
		HWR_Transform_CB(&cx, &cy, &cz, false);
		//HWR_Transform_CB(&cx, &cy, &cz, true);
#else
		// DEFAULT
		transform_world_to_gr(/*IN*/ light_pos->x, light_pos->y, light_pos->z, /*OUT*/ &cx, &cy, &cz);
#endif
#if 0
		// -- Draw debug point in screen space
		HWR_DebugDrawPoint(cx, cy, cz, 1.0f, 0.0f, 0.0f); // Red dot
		// -- (optional) Draw the original world-space point (to compare)
		HWR_DebugDrawPoint(light_pos->x, light_pos->y, light_pos->z, 0.0f, 1.0f, 0.0f); // Green dot (before transform)
		// -- Line from camera to transformed corona
		HWR_DebugDrawLine(0, 0, 0, light_pos->x, light_pos->y, light_pos->z, 1.0f, 1.0f, 0.0f); // Yellow line
#endif

#if 1
		if (cz >= CORONA_Z2)
		{
			// Corona is past fading distance!
			continue;
		}
#endif

		// mobj dependent light selector
		if (Sprite_Corona_Light_fade(p_lspr, cz, (mobj_t *)light->actor, &spr_light_data) == false)
		{
			continue;
		}

		// Sprite has a corona, and coronas are enabled.
		size = (spr_light_data.size * 2.0);
		alpha = clamp(spr_light_data.alpha, 0.0, (double)UINT8_MAX-1);
		//alpha = clamp(spr_light_data.alpha, 128.0, (double)UINT8_MAX-1);
		if (alpha <= 0.0f)
		{
			continue;
		}

		//HWR_GetPic(corona_lumpnum, colormap);
		(void)colormap;

		HWR_DL_GetCoronaLighting(&Surf, light->sector, (mobj_t *)light->actor);
		Surf.PolyColor = light->color;
		Surf.PolyColor.s.alpha = alpha;

#if 1
#if 0
		// put light little forward the sprite so there is no
		// z-buffer problem (coplanar polygons)
		// BP: use PF_Decal do not help :(
		cz = cz - 5.0f;

		lightVerts[0].x = cx-size;  lightVerts[0].z = cz;
		lightVerts[0].y = cy-size*1.33f;
		lightVerts[0].s = 0.0f;   lightVerts[0].t = 0.0f;

		lightVerts[1].x = cx+size;  lightVerts[1].z = cz;
		lightVerts[1].y = cy-size*1.33f;
		lightVerts[1].s = 1.0f;   lightVerts[1].t = 0.0f;

		lightVerts[2].x = cx+size;  lightVerts[2].z = cz;
		lightVerts[2].y = cy+size*1.33f;
		lightVerts[2].s = 1.0f;   lightVerts[2].t = 1.0f;

		lightVerts[3].x = cx-size;  lightVerts[3].z = cz;
		lightVerts[3].y = cy+size*1.33f;
		lightVerts[3].s = 0.0f;   lightVerts[3].t = 1.0f;
#else
		// put light little forward the sprite so there is no
		// z-buffer problem (coplanaire polygons)
		// BP: use PF_Decal do not help :(
		if (cz > 0.5f) // correction for side drift due to cz change
		{
		   cx += (cx * ((-3.8f) / cz));
		   cy += (cy * ((-3.8f) / cz));
		}
		cz = cz - 5.0f;

		lightVerts[0].x = lightVerts[3].x = cx - size;
		lightVerts[1].x = lightVerts[2].x = cx + size;
		lightVerts[0].y = lightVerts[1].y = cy - (size*1.33f);
		lightVerts[2].y = lightVerts[3].y = cy + (size*1.33f);
		lightVerts[0].z = lightVerts[1].z = lightVerts[2].z = lightVerts[3].z = cz;
		lightVerts[0].s = 0.0f;   lightVerts[0].t = 0.0f;
		lightVerts[1].s = 1.0f;   lightVerts[1].t = 0.0f;
		lightVerts[2].s = 1.0f;   lightVerts[2].t = 1.0f;
		lightVerts[3].s = 0.0f;   lightVerts[3].t = 1.0f;
#endif
#endif

		blend &= ~PF_Decal;
		blend &= ~PF_Corona;
		blend &= ~PF_NoAlphaTest;
		HWR_ProcessPolygon(&Surf, lightVerts, 4, blend, shader, horizonSpecial);
	}
}

#else

void HWR_DL_Draw_Coronas(void)
{
#ifdef NO_CORONA_RENDERING
	return;
#endif

	static GLMipmap_t *current_corona = NULL;

	DYNAMIC_ONLY
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
		return;

	// Setup render state for corona rendering
	HWD.pfnSetBlend(PF_Additive|PF_Modulated);
	//glPushMatrix();

#if 1
	// Ensure corona texture is uploaded to GPU
	if (current_corona == NULL || current_corona->downloaded == 0)
	{
		GLPatch_t *gr_patch = HWR_GetPic(corona_lumpnum, NULL);

		if (gr_patch == NULL)
			return;
		current_corona = gr_patch->mipmap;

		// Generate texture name
		GLuint texName = 0;
#if 0
		glGenTextures(1, &texName);
		current_corona->downloaded = (int)texName;
#else
		texName = current_corona->downloaded;
#endif

		// Bind and configure texture
		glBindTexture(GL_TEXTURE_2D, texName);
#if 0
		{
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		}
		{
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload texture data
		// current_corona->data is an array of uint16_t where low byte = intensity
		// and high byte = alpha. We must expand it to RGBA unsigned bytes for
		// glTexImage2D (GL_UNSIGNED_BYTE) to avoid passing 16-bit data as bytes
		// which resulted in a white screen.
		const int w = (int)current_corona->width;
		const int h = (int)current_corona->height;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, current_corona->data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, current_corona->data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, current_corona->data);

		SUFFER_PRINT(DBG_RENDER, "HWR_DL_Draw_Coronas(): Corona texture uploaded (ID: %d)\n", texName);
	}
	else
	{
		// Texture already uploaded, just bind it
		glBindTexture(GL_TEXTURE_2D, (GLuint)current_corona->downloaded);
	}
#endif

	FOutVector lightVerts[4];
	FDynamicLight *light;
	for (light = dynlights_head; light != NULL; light = light->next)
	{
		light_t *p_lspr = light->light_data;
		floatvector3_t *light_pos = &light->pos;
		sprite_light_data_t spr_light_data;

		float cx = 0.0f, cy = 0.0f, cz = 0.0f;
		float size, alpha;

		if (light->visible == false)
			continue;

		// Transform light position to screen/camera space
		transform_world_to_gr(light_pos->x, light_pos->y, light_pos->z, &cx, &cy, &cz);

		// Determine corona size and alpha based on fade settings
		if (Sprite_Corona_Light_fade(p_lspr, cz, (mobj_t *)light->actor, &spr_light_data) == false)
			continue;

		size = spr_light_data.size * 2.0f;
		//size = spr_light_data.size;
		alpha = (float)spr_light_data.alpha;

		// Clamp alpha to valid range
		if (alpha < 128.0f)
			alpha = 128.0f;
		if (alpha < 0.0f)
			alpha = 0.0f;
		if (alpha > 255.0f)
			alpha = 255.0f;

		// Apply perspective correction for side drift
		if (cz > 0.5f)
		{
			cx += (cx * (-3.8f / cz));
			cy += (cy * (-3.8f / cz));
		}
		cz -= 5.0f;

		// Build quad vertices for corona billboard
		lightVerts[0].x = lightVerts[3].x = cx - size;
		lightVerts[1].x = lightVerts[2].x = cx + size;
		lightVerts[0].y = lightVerts[1].y = cy - (size * 1.33f);
		lightVerts[2].y = lightVerts[3].y = cy + (size * 1.33f);
		lightVerts[0].z = lightVerts[1].z = lightVerts[2].z = lightVerts[3].z = cz;

		// Texture coordinates (0-1 normalized)
		lightVerts[0].s = 0.0f; lightVerts[0].t = 0.0f;
		lightVerts[1].s = 1.0f; lightVerts[1].t = 0.0f;
		lightVerts[2].s = 1.0f; lightVerts[2].t = 1.0f;
		lightVerts[3].s = 0.0f; lightVerts[3].t = 1.0f;

		// Setup the corona quad
#if 0
		if (light->poly_flags & PF_NoDepthTest)
		{
			//glDisable(GL_DEPTH_TEST);
			HWD.pfnSetBlend(PF_Additive|PF_Modulated|PF_NoDepthTest);
		}
		else
		{
			//glEnable(GL_DEPTH_TEST);
			//HWD.pfnSetBlend(PF_Additive|PF_Modulated);
		}
#endif
#if 0
		HWD.pfnSetBlend(PF_Additive|PF_Modulated|PF_NoDepthTest);
#endif
#if 1
		INT32 extra_blends     = 0;
		INT32 shader           = SHADER_NONE;

#if 1
		if (HWR_UseShader())
		{
			//shader = SHADER_SPRITE;
			extra_blends |= PF_ColorMapped;
		}
#endif

#if 1
		//extra_blends |= PF_Occlude;
		//extra_blends |= PF_Environment;
		//extra_blends |= PF_Masked;
		//extra_blends |= PF_NoTexture;
		//extra_blends |= PF_RemoveYWrap;
#endif


#if 1
		FBITFIELD blend = light->poly_flags|extra_blends;
		//blend &= ~PF_Decal;
		//blend &= ~PF_Corona;
		blend &= ~PF_NoAlphaTest;
		HWD.pfnSetBlend(blend);
#endif
#endif

		HWD.pfnSetShader((shader != SHADER_NONE) ? HWR_GetShaderFromTarget(shader) : shader);
		{
			glColor4ubv((GLubyte*)&light->color.s);
			glVertexPointer(3, GL_FLOAT, sizeof(FOutVector), &lightVerts[0].x);
			glTexCoordPointer(2, GL_FLOAT, sizeof(FOutVector), &lightVerts[0].s);
		}
		{
			// Draw the corona quad
			//glDrawArrays(GL_QUADS, 0, 4);
			glDrawArrays(blend & PF_WireFrame ? GL_LINES : GL_TRIANGLE_FAN, 0, 4);
		}

		// Debug: check for GL errors after draw
		{
			GLenum err = glGetError();
			if (err != GL_NO_ERROR)
			{
				//SUFFER_PRINT(DBG_RENDER, "HWR_DL_Draw_Coronas(): glDrawArrays error: 0x%X\n", err);
				CONS_Alert(DBG_RENDER, "HWR_DL_Draw_Coronas(): glDrawArrays error: 0x%X\n", err);
			}
		}
	}

	//glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

#endif

// =================================
// LIGHT MANAGEMENT
// =================================

#if 0

#define SUFFER_MORE_PRINT(func_debug, type, ...) { \
	if (func_debug == 2) \
		CONS_Debug( type, __VA_ARGS__ ); \
	else if (func_debug == 1) \
		CONS_Printf( __VA_ARGS__ ); \
	else \
	{ \
		; \
	} \
}

#else

//#define SUFFER_MORE_PRINT(func_debug, type, ...) CONS_Printf( __VA_ARGS__ );
#define SUFFER_MORE_PRINT(func_debug, type, ...) { ; }

#endif

//#define LIGHT_TAG PU_STATIC
#define LIGHT_TAG PU_LEVEL

// --------------------------------------------------------------------------
// static inline boolean HWR_DL_RemoveNode(FDynamicLight *node)
//
// Removes a light node from the light head, unlinks it, and frees it.
// The head is then is shifted down.
// --------------------------------------------------------------------------
static inline boolean HWR_DL_RemoveNode(FDynamicLight *node)
//static boolean HWR_DL_RemoveNode(FDynamicLight *node)
{
	if (node == NULL)
		return false;

#if 0
	// Unlink from list
	if (node->prev)
		node->prev->next = node->next;
	else
		dynlights_head = node->next;
	if (node->next)
		node->next->prev = node->prev;

	node->actor = NULL;
	Z_Free(node);
#else
	node->actor = NULL;
	M_DLListRemove((mdllistitem_t *)node);
	Z_Free(node);
#endif

	if (num_dynamic_lights > 0)
		num_dynamic_lights--;
	return true;
}

// --------------------------------------------------------------------------
// Public removal by origin pointer. (e.g. actor or vis sprite pointer)
// Scans array and removes the first matching light.
// --------------------------------------------------------------------------
void HWR_DL_RemoveLight(void *origin)
{
	if (dynlights_head == NULL || num_dynamic_lights <= 0)
	{
		num_dynamic_lights = 0;
		return;
	}
	else if (origin == NULL)
	{
		return;
	}

	for (FDynamicLight *node = dynlights_head; node != NULL; node = node->next)
	{
		if (node->actor == origin)
		{
			HWR_DL_RemoveNode(node);
			return;
		}
	}
}

// --------------------------------------------------------------------------
// void HWR_Update_Lights(void)
// Called from HWR_RenderPlayerView and HWR_RenderSkyboxView.
//
// Updates all lights within the light list.
// Makes sure they're all still valid and existing.
// --------------------------------------------------------------------------
void HWR_Update_Lights(void)
{
	FDynamicLight *light;
	INT32 current_light = 0;

	FDynamicLight *next;
	mobj_t *mobj;
	light_t *p_lspr;
	interpmobjstate_t interp = {0};

	if (dynlights_head == NULL || num_dynamic_lights <= 0)
	{
		num_dynamic_lights = 0;
		return;
	}

	for ( light = dynlights_head; light != NULL; )
	{
		next = light->next;
		mobj = (mobj_t *)light->actor;
		p_lspr = light->light_data;

		if (current_light >= cv_tsourdt3rd_video_lighting_coronas_maxlights.value)
		{
			light->visible = false;
			light = next;
			current_light++;
			continue;
		}

		if (light->actor_type == DYNLIGHT_ORIGIN_VISSPRITE || light->actor_type == DYNLIGHT_ORIGIN_MOBJ)
		{
			fixed_t limit_dist, hoop_limit_dist;
			floatvector3_t pos, offset;
			RGBA_t color;
			sector_t *sector = NULL;

			if (mobj == NULL || P_MobjWasRemoved(mobj))
			{
				SUFFER_MORE_PRINT(false, DBG_RENDER, "HWR_Update_Lights() - Mobj %p removed (existing light count: %d)\n", (void *)mobj, num_dynamic_lights);
				HWR_DL_RemoveNode(light);
				light = next;
				continue;
			}

			// If object is out of draw distance, or isn't visible, hide it's light!
			if (!R_ThingVisible(mobj))
			{
				light->visible = false;
				light = next;
				current_light++;
				continue;
			}
			else
			{
				limit_dist = (fixed_t)(cv_drawdist.value) << FRACBITS;
				hoop_limit_dist = (fixed_t)(cv_drawdist_nights.value) << FRACBITS;
				if (!R_ThingWithinDist(mobj, limit_dist, hoop_limit_dist))
				{
					light->visible = false;
					light = next;
					current_light++;
					continue;
				}
				if (p_lspr->corona_rendering_routine && !p_lspr->corona_rendering_routine(mobj))
				{
					light->visible = false;
					light = next;
					current_light++;
					continue;
				}
			}

#if 1
			// uncapped/interpolation
			if (R_UsingFrameInterpolation() && !paused)
			{
				R_InterpolateMobjState(mobj, rendertimefrac, &interp);
			}
			else
			{
				R_InterpolateMobjState(mobj, FRACUNIT, &interp);
			}

			// Update light position
			offset.x = p_lspr->light_xoffset;
			offset.y = p_lspr->light_yoffset;
			offset.z = 0;
			pos.x = FixedToFloat(interp.x) + offset.x;
			pos.z = FixedToFloat(interp.y) + offset.z;
			switch (mobj->sprite)
			{
				case SPR_STPT:
				case SPR_XMS4: // full height
					offset.y -= 32.0f;
					pos.y = FixedToFloat(interp.z) + FixedToFloat(interp.height) + offset.y;
					break;
				default:
					pos.y = FixedToFloat(interp.z) + FixedToFloat(interp.height >> 1) + offset.y;
					break;
			}
			if (interp.subsector != NULL)
			{
				sector = interp.subsector->sector;
			}
#else
			// Update light position
			offset.x = p_lspr->light_xoffset;
			offset.y = p_lspr->light_yoffset;
			offset.z = 0;
			pos.x = FixedToFloat(mobj->x) + offset.x;
			pos.z = FixedToFloat(mobj->y) + offset.z;
			switch (mobj->sprite)
			{
				case SPR_STPT:
				case SPR_XMS4: // full height
					offset.y -= 32.0f;
					pos.y = FixedToFloat(mobj->z) + FixedToFloat(mobj->height) + offset.y;
					break;
				default:
					pos.y = FixedToFloat(mobj->z) + FixedToFloat(mobj->height >> 1) + offset.y;
					break;
			}
			if (mobj->subsector != NULL)
			{
				sector = mobj->subsector->sector;
			}
#endif
			light->pos = pos;
			light->offset = offset;
			light->sector = sector;

			if (p_lspr->corona_coloring_routine == NULL || !p_lspr->corona_coloring_routine(mobj, &color, NULL, true))
			{
				if (L_UseObjectColor(mobj, &color, NULL, true) == false)
					color.rgba = LONG(p_lspr->dynamic_color);
			}
			if (mobj->state == NULL || mobj->state == &states[S_NULL] || mobj->state == &states[S_INVISIBLE])
				color.s.alpha *= (float)mobj->tics/(float)TICRATE;
			else if (mobj->state->nextstate == S_NULL || mobj->state->nextstate == S_INVISIBLE)
				color.s.alpha *= (float)mobj->tics/(float)mobj->state->tics;
			light->color = color;
		}

		light->visible = true;
		light = next;
		current_light++;
	}
}

// --------------------------------------------------------------------------
// void HWR_DL_ClearLights(void)
// Called from P_SetupLevel via HWR_LoadLevel.
//
// Frees the light head and removes all dynamic lights attached to the head.
// Clears all possible references to avoid dangling pointers.
// Uses 'HWR_DL_RemoveNode' repeatedly until the head is empty
// --------------------------------------------------------------------------
void HWR_DL_ClearLights(void)
{
	while (dynlights_head != NULL && num_dynamic_lights > 0)
	{
		HWR_DL_RemoveNode(dynlights_head);
	}

	Z_Free(dynlights_head);
	dynlights_head = NULL;

	num_dynamic_lights = 0;
}

// --------------------------------------------------------------------------
// Add a light for dynamic lighting.
// The light position is already transformed except for mlook
// --------------------------------------------------------------------------
static boolean add_mobj_light(mobj_t *mobj, FDynamicLight *light);
static boolean add_regular_light(dynlight_basic_t *params, FDynamicLight *light);

//boolean HWR_DL_AddBasicLight(INT32 type, double x, double y, double z, ULONG color, byte radius, INT32 flags)
boolean HWR_DL_AddBasicLight(INT32 type, double x, double y, double z, long color, long radius, INT32 flags)
{
#ifdef NO_LIGHT_ADDING
	return false;
#endif

	dynlight_basic_t basic_light_data;

	if (type <= NOLIGHT || type >= NUMLIGHTS)
	{
		CONS_Debug(DBG_RENDER, "HWR_DL_AddBasicLight() - Invalid light given!\n");
		return false;
	}

	basic_light_data.type = &lspr[type];
	basic_light_data.flags = flags;
	basic_light_data.pos.x = x;
	basic_light_data.pos.y = z;
	basic_light_data.pos.z = y;
	basic_light_data.offset.x = 0.0f;
	basic_light_data.offset.y = 0.0f;
	basic_light_data.offset.z = 0.0f;
	basic_light_data.color.rgba = color;
	basic_light_data.radius = radius;
	return HWR_DL_AddLight(&basic_light_data, DYNLIGHT_ORIGIN_NONE);
}

boolean HWR_DL_AddLight(void *origin, dynlight_origin_t origin_type)
{
#ifdef NO_LIGHT_ADDING
	return false;
#endif

	FDynamicLight *new_light;
	gl_vissprite_t *spr = NULL;
	mobj_t *mobj = NULL;
	UINT8 *colormap = NULL;
	light_t *p_lspr;

	// Hurdler: moved here because it's better ;-)
	// StarManiaKG: We need to manage lights properly! They're fickle(-hearted)!
	if (origin == NULL)
	{
		return false;
	}

	// Check if this is a duplicate light before initialization!
	// Helps us save on memory and performance :p
	if (origin_type == DYNLIGHT_ORIGIN_MOBJ || origin_type == DYNLIGHT_ORIGIN_VISSPRITE)
	{
		FDynamicLight *node;
		mobj_t *originmobj = NULL;
		gl_vissprite_t *originspr = NULL;

		if (origin_type == DYNLIGHT_ORIGIN_VISSPRITE)
		{
			originspr = (gl_vissprite_t *)origin;
			originmobj = originspr->mobj;
		}
		if (originmobj == NULL)
		{
			originmobj = (mobj_t *)origin;
		}

		if (originmobj == NULL)
		{
			SUFFER_MORE_PRINT(false, DBG_RENDER, "HWR_DL_AddLight: Attempted to add light for invalid mobj\n");
			return false;
		}

		p_lspr = t_lspr[originmobj->sprite];
		if (p_lspr == NULL || p_lspr->type == NOLIGHT)
		{
			return false;
		}
		else if (!(p_lspr->type & DYNLIGHT_SPR))
		{
			//return false;
		}
		else if ((p_lspr->type & (LIGHT_SPR|CORONA_SPR|ROCKET_SPR)) != LIGHT_SPR)
		{
			//return false;
		}

		for (node = dynlights_head; node != NULL; node = node->next)
		{
			if (node->actor == NULL)
			{
				continue;
			}
			else if (node->actor == originmobj)
			{
				SUFFER_MORE_PRINT(2, DBG_RENDER, "HWR_DL_AddLight: Duplicate light detected for mobj %p (existing light count: %d)\n", (void *)originmobj, num_dynamic_lights);
				return true;
			}
		}
	}

	// Using the origin given, we gotta get the proper source of the light object!
	new_light = Z_Malloc(sizeof(*new_light), LIGHT_TAG, NULL);
	new_light->prev = new_light->next = NULL;
	new_light->sector = NULL;
	new_light->actor = NULL;
	new_light->actor_type = origin_type;
	new_light->colormap = NULL;
	switch (cv_tsourdt3rd_video_lighting_coronas_drawingmode.value)
	{
		default: new_light->poly_flags = PF_Blending; break;
		case 1:	 new_light->poly_flags = PF_Blending; break;
		case 2:  new_light->poly_flags = PF_Additive; break;
		case 3:  new_light->poly_flags = PF_Additive; break;
		case 4:  new_light->poly_flags = PF_Multiplicative; break;
	}
	switch (origin_type)
	{
		case DYNLIGHT_ORIGIN_VISSPRITE:
			spr = (gl_vissprite_t *)origin;
			mobj = spr->mobj;
			colormap = spr->colormap;
			/* FALLTHRU */
		case DYNLIGHT_ORIGIN_MOBJ:
			if (mobj == NULL)
			{
				mobj = (mobj_t *)origin;
			}
			if (add_mobj_light(mobj, new_light) == true)
			{
				new_light->colormap = colormap;
				new_light->actor = mobj;
				break;
			}
			/* FALLTHRU */
		default:
			if (origin_type == DYNLIGHT_ORIGIN_NONE)
			{
				if (add_regular_light(origin, new_light) == true)
				{
					break;
				}
			}
			Z_Free(new_light);
			new_light = NULL;
			return false;
	}

	// Insert new light at the head of the linked list. This keeps insertion
	// simple and avoids any array reallocation or dense-array maintenance.
#if 0
	if (dynlights_head)
	{
		new_light->next = dynlights_head;
		dynlights_head->prev = new_light;
	}
	dynlights_head = new_light;
#else
	M_DLListInsert((mdllistitem_t *)new_light, (mdllistitem_t **)(void *)(&dynlights_head));
#endif
	num_dynamic_lights++;
	SUFFER_MORE_PRINT(2, DBG_RENDER, "HWR_DL_AddLight: Light added! (mobj: %p) (existing lights: %d)\n", (void *)new_light->actor, num_dynamic_lights);
	return true;
}

// --------------------------------------------------------------------------
// Hurdler: this adds lights by mobj.
// --------------------------------------------------------------------------
static boolean add_mobj_light(mobj_t *mobj, FDynamicLight *light)
{
	FBITFIELD PolyFlags = 0;

	if (mobj == NULL || P_MobjWasRemoved(mobj))
	{
		// Invalid vissprite object.
		// We only bind the actor to a light if it exists.
		return false;
	}
	I_Assert(mobj == NULL || P_MobjWasRemoved(mobj));

	// Object has a proper corona.
	// Therefore, create a dynamic light, and give the light a position.
	PolyFlags |= LIGHTMAP_POLY_FLAGS;
	PolyFlags |= PF_Decal;
	//PolyFlags |= PF_Corona;
	PolyFlags |= PF_NoAlphaTest;
#if 1
	//if (mobj->sprite == SPR_PLAY || (mobj->type == MT_PLAYER && mobj->player != NULL && P_IsLocalPlayer(mobj->player)))
	//if (mobj->type == MT_PLAYER && mobj->player != NULL && P_IsLocalPlayer(mobj->player))
	if (mobj->type == MT_PLAYER && mobj->player != NULL && mobj->player == &players[displayplayer])
		PolyFlags |= PF_NoDepthTest;
#endif

	light->light_data = t_lspr[mobj->sprite];
	light->poly_flags |= PolyFlags;
	return true;
}

// --------------------------------------------------------------------------
// Add a light for dynamic lighting
// The light position is already transformed execpt for mlook
// --------------------------------------------------------------------------
static boolean add_regular_light(dynlight_basic_t *params, FDynamicLight *light)
{
	light->light_data = params->type;
	light->poly_flags |= LIGHTMAP_POLY_FLAGS;
	light->poly_flags |= PF_Decal;
	//light->poly_flags |= PF_Corona;
	light->poly_flags |= PF_NoAlphaTest;
	light->pos = params->pos;
	light->offset = params->pos;
	light->color = params->color;
	return true;
}
#if 0
FDynamicLight::FDynamicLight(floatvector3_t pos, floatvector3_t off, float rad, FColorRGBA col, mobj_t *act)
{
	if (act != NULL)
	{
		// only bind the actor if it exists
		this->actor = act;
	}
	this->color = col;
	this->radius = rad;
	this->offset = off;
	this->position = pos;
}
#endif

#if 0
boolean HWR_DL_SetLightVisible(void *origin, dynlight_origin_t origin_type)
{

}
#endif

#if 0
void HWR_DL_SetLightsVisible(void)
{

}
#endif

// ===============================================================
//
// ===============================================================

static boolean create_command = false;
static const char *light_patch_name = NULL;

static void LoadCorona(void)
{
	const boolean loaded_corona_before = (corona_gl_patch.mipmap->downloaded != 0);
	if (light_patch_name == NULL)
	{
		return;
	}
	else
	{
		corona_lumpnum = W_CheckNumForName(light_patch_name);
		corona_gl_patch.mipmap->downloaded = 0;
		if (loaded_corona_before == true)
		{
			Z_Free(corona_gl_patch.mipmap->data);
			corona_gl_patch.mipmap->data = NULL;
			HWR_SetLight();
		}
	}
}

void HWR_Init_Light(const char *light_patch)
{
#ifdef NO_LIGHT_INIT
	return;
#endif

	light_patch_name = light_patch;
	LoadCorona();

	if (create_command == false)
	{
		COM_AddCommand("reloadcoronas", LoadCorona, 0);
		create_command = true;
	}

	CONS_Printf("HWR_Init_Light()...\n");
	for (int i = 0; i < NUMLIGHTS; i++) // precalculate sqr radius
	{
		light_t *p_lspr = &lspr[i];
		p_lspr->dynamic_sqrradius = (p_lspr->dynamic_radius * p_lspr->dynamic_radius);
	}
}

void HWR_DynamicShadowing(FOutVector *clVerts, size_t nrClipVerts)
{
	FSurfaceInfo Surf;
	FBITFIELD PolyFlags = PF_Modulated|PF_Environment;

	//HWR_GetPic(corona_lumpnum); // TODO: use different coronas

	for (size_t i = 0; i < nrClipVerts; i++)
	{
		clVerts[i].s = 0.5f + clVerts[i].x*0.01f;
		clVerts[i].t = 0.5f + clVerts[i].z*0.01f*1.2f;
	}

	HWR_SetLight();
	Surf.PolyColor.rgba = 0x70707070;

	//HWD.pfnDrawPolygon(&Surf, clVerts, nrClipVerts, LIGHTMAP_POLY_FLAGS|PolyFlags);
	HWR_ProcessPolygon(&Surf, clVerts, nrClipVerts, LIGHTMAP_POLY_FLAGS|PolyFlags, SHADER_NONE, false);
}

//**********************************************************
// Hurdler: new code for faster static lighting and and T&L
//**********************************************************

#ifdef STATICLIGHTMAPS
// est ce bien necessaire ?
static sector_t *lgl_backsector;
static seg_t *lgl_curline;

// p1 et p2 c'est le deux bou du seg en float
static void HWR_Create_WallLightmaps(floatvector3_t *p1, floatvector3_t *p2, int lightnum, seg_t *line)
{
	lightmap_t *lp = NULL;

#if 0
	// (...) calcul presit de la projection et de la distance
	if (dist_p2d >= dynamic_light->p_lspr[lightnum]->dynamic_sqrradius)
		return;
#endif

	// (...) attention faire le backfase cull histoir de faire mieux que Q3 !
	(void)p1;
	(void)p2;
	(void)lightnum;
	lp = malloc(sizeof(lightmap_t));
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
	floatvector3_t    p1, p2;

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
//	if (!(CircleTouchBBox(&p1, &p2, &dynlights->pos[lightnum], p_lspr[lightnum]->dynamic_radius)))
//		return;

	HWR_Create_WallLightmaps(&p1, &p2, lightnum, line);
}

// TODO: see what HWR_AddLine does
static void HWR_CheckSubsector(size_t num, fixed_t *bbox)
{
	int              count;
	seg_t            *line;
	subsector_t       *sub;
	floatvector3_t  p1, p2;
	int           lightnum;

	p1.y = FIXED_TO_FLOAT(bbox[BOXTOP	]);
	p1.x = FIXED_TO_FLOAT(bbox[BOXLEFT	]);
	p2.y = FIXED_TO_FLOAT(bbox[BOXBOTTOM]);
	p2.x = FIXED_TO_FLOAT(bbox[BOXRIGHT	]);

	if (num < numsubsectors)
	{
		sub = &subsectors[num]; // subsector
		for (lightnum = 0; lightnum < dynlights->num_lights; lightnum++)
		{
#if 0
			// check bbox of the seg
			if (!(CircleTouchBBox(&p1, &p2, dynlights->pos[lightnum], p_lspr[lightnum]->dynamic_radius)))
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

	//HWR_ProcessSeg();
}

//
// Hurdler: The goal of this function is to walk through all the bsp starting on the top.
//         We need to do that to know all the lights in the map and all the walls
//
static void HWR_ComputeLightMapsInBSPNode(int bspnum, fixed_t *bbox)
{
	if (bspnum & NF_SUBSECTOR) // Found a subsector?
	{
		// 0 is probably unecessary: see boris' comment in hw_bsp
		HWR_CheckSubsector(((bspnum == -1) ? 0 : (bspnum & (~NF_SUBSECTOR))), bbox);
		return;
	}
	HWR_ComputeLightMapsInBSPNode(nodes[bspnum].children[0], nodes[bspnum].bbox[0]);
	HWR_ComputeLightMapsInBSPNode(nodes[bspnum].children[1], nodes[bspnum].bbox[1]);
}

static void HWR_SearchLightsInMobjs(void)
{
	// search in the list of thinkers
	for (thinker_t *th = thlist[THINK_MOBJ].next; th != &thlist[THINK_MOBJ]; th = th->next)
		if (th->function != (actionf_p1)P_RemoveThinkerDelayed)
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
  - Add a batching system for lights? It would help reduce load on the GPU,
		just store all the lights and eventually render them all at once.

 \name: Completed TO-DOs
  - finalement virer le hack splitscreen, il n'est plus necessaire !
*/

#endif // ALAM_LIGHTING
#endif // HWRENDER
