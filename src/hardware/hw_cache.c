// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 1999-2023 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file hw_cache.c
/// \brief load and convert graphics to the hardware format

#include "../doomdef.h"

#ifdef HWRENDER
#include "hw_glob.h"
#include "hw_drv.h"
#include "hw_batching.h"

#include "../doomstat.h"    //gamemode
#include "../i_video.h"     //rendermode
#include "../r_data.h"
#include "../r_textures.h"
#include "../w_wad.h"
#include "../z_zone.h"
#include "../v_video.h"
#include "../r_draw.h"
#include "../r_patch.h"
#include "../r_picformats.h"
#include "../p_setup.h"

INT32 patchformat = GL_TEXFMT_AP_88; // use alpha for holes
INT32 textureformat = GL_TEXFMT_P_8; // use chromakey for hole

RGBA_t mapPalette[256] = {0}; // the palette for the currently loaded level or menu etc.

// Returns a pointer to the palette which should be used for caching textures.
RGBA_t *HWR_GetTexturePalette(void)
{
	return HWR_ShouldUsePaletteRendering() ? mapPalette : pLocalPalette;
}

static INT32 format2bpp(GLTextureFormat_t format)
{
	if (format == GL_TEXFMT_RGBA)
		return 4;
	else if (format == GL_TEXFMT_ALPHA_INTENSITY_88 || format == GL_TEXFMT_AP_88)
		return 2;
	else
		return 1;
}

// This code was originally placed directly in HWR_DrawPatchInCache.
// It is now split from it for my sanity! (and the sanity of others)
// -- Monster Iestyn (13/02/19)
static void HWR_DrawColumnInCache(const column_t *patchcol, UINT8 *block, GLMipmap_t *mipmap,
								INT32 pblockheight, INT32 blockmodulo,
								fixed_t yfracstep, fixed_t scale_y,
								texpatch_t *originPatch, INT32 patchheight,
								INT32 bpp, RGBA_t *palette)
{
	fixed_t yfrac, position, count;
	UINT8 *dest;
	const UINT8 *source;
	INT32 originy = 0;

	// for writing a pixel to dest
	RGBA_t colortemp;
	UINT8 alpha;
	UINT8 texel;
	UINT16 texelu16;

	(void)patchheight; // This parameter is unused

	if (originPatch) // originPatch can be NULL here, unlike in the software version
		originy = originPatch->originy;

	for (unsigned i = 0; i < patchcol->num_posts; i++)
	{
		post_t *post = &patchcol->posts[i];
		source = patchcol->pixels + post->data_offset;
		count  = ((post->length * scale_y) + (FRACUNIT/2)) >> FRACBITS;
		position = originy + post->topdelta;

		yfrac = 0;
		if (position < 0)
		{
			yfrac = -position<<FRACBITS;
			count += (((position * scale_y) + (FRACUNIT/2)) >> FRACBITS);
			position = 0;
		}

		position = ((position * scale_y) + (FRACUNIT/2)) >> FRACBITS;

		if (position < 0)
			position = 0;

		if (position + count >= pblockheight)
			count = pblockheight - position;

		for (dest = block + (position*blockmodulo); count > 0; count--, dest += blockmodulo, yfrac += yfracstep)
		{
			texel = source[yfrac>>FRACBITS];
			alpha = 0xFF;

			// Make pixel transparent if chroma keyed
			if ((mipmap->flags & TF_CHROMAKEYED) && (texel == HWR_PATCHES_CHROMAKEY_COLORINDEX))
				alpha = 0x00;

			if (mipmap->colormap)
				texel = mipmap->colormap->data[texel];

			switch (bpp)
			{
				case 2:
				{
					texelu16 = *((UINT16*)dest);
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
					{
						if (originPatch->style == AST_TRANSLUCENT && originPatch->alpha < ASTTextureBlendingThreshold[0])
							continue;
						if (!(texelu16 & 0xFF00) && originPatch->alpha <= ASTTextureBlendingThreshold[1])
							continue;
						texel = ASTBlendPaletteIndexes(texelu16 & 0xFF, texel, originPatch->style, originPatch->alpha);
					}
					texelu16 = (UINT16)((alpha<<8) | texel);
					memcpy(dest, &texelu16, sizeof(UINT16));
					break;
				}
				case 3:
					colortemp = palette[texel];
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
					{
						RGBA_t rgbatexel;
						rgbatexel.rgba = *(UINT32 *)dest;
						colortemp.rgba = ASTBlendTexturePixel(rgbatexel, colortemp, originPatch->style, originPatch->alpha);
					}
					memcpy(dest, &colortemp, sizeof(RGBA_t)-sizeof(UINT8));
					break;
				case 4:
					colortemp = palette[texel];
					colortemp.s.alpha = alpha;
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
					{
						RGBA_t rgbatexel;
						rgbatexel.rgba = *(UINT32 *)dest;
						colortemp.rgba = ASTBlendTexturePixel(rgbatexel, colortemp, originPatch->style, originPatch->alpha);
					}
					memcpy(dest, &colortemp, sizeof(RGBA_t));
					break;
				// default is 1
				default:
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
						*dest = ASTBlendPaletteIndexes(*dest, texel, originPatch->style, originPatch->alpha);
					else
						*dest = texel;
					break;
			}
		}
	}
}

static void HWR_DrawFlippedColumnInCache(const column_t *patchcol, UINT8 *block, GLMipmap_t *mipmap,
								INT32 pblockheight, INT32 blockmodulo,
								fixed_t yfracstep, fixed_t scale_y,
								texpatch_t *originPatch, INT32 patchheight,
								INT32 bpp, RGBA_t *palette)
{
	fixed_t yfrac, position, count;
	UINT8 *dest;
	const UINT8 *source;
	INT32 topdelta;
	INT32 originy = 0;

	// for writing a pixel to dest
	RGBA_t colortemp;
	UINT8 alpha;
	UINT8 texel;
	UINT16 texelu16;

	if (originPatch) // originPatch can be NULL here, unlike in the software version
		originy = originPatch->originy;

	for (unsigned i = 0; i < patchcol->num_posts; i++)
	{
		post_t *post = &patchcol->posts[i];
		source = patchcol->pixels + post->data_offset;
		topdelta = patchheight-post->length-post->topdelta;
		count  = ((post->length * scale_y) + (FRACUNIT/2)) >> FRACBITS;
		position = originy + topdelta;

		yfrac = (post->length-1) << FRACBITS;

		if (position < 0)
		{
			yfrac += position<<FRACBITS;
			count += (((position * scale_y) + (FRACUNIT/2)) >> FRACBITS);
			position = 0;
		}

		position = ((position * scale_y) + (FRACUNIT/2)) >> FRACBITS;

		if (position < 0)
			position = 0;

		if (position + count >= pblockheight)
			count = pblockheight - position;

		for (dest = block + (position*blockmodulo); count > 0; count--, dest += blockmodulo, yfrac -= yfracstep)
		{
			texel = source[yfrac>>FRACBITS];
			alpha = 0xFF;

			// Make pixel transparent if chroma keyed
			if ((mipmap->flags & TF_CHROMAKEYED) && (texel == HWR_PATCHES_CHROMAKEY_COLORINDEX))
				alpha = 0x00;

			if (mipmap->colormap)
				texel = mipmap->colormap->data[texel];

			switch (bpp)
			{
				case 2:
					texelu16 = *((UINT16*)dest);
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
					{
						if (originPatch->style == AST_TRANSLUCENT && originPatch->alpha < ASTTextureBlendingThreshold[0])
							continue;
						if (!(texelu16 & 0xFF00) && originPatch->alpha <= ASTTextureBlendingThreshold[1])
							continue;
						texel = ASTBlendPaletteIndexes(texelu16 & 0xFF, texel, originPatch->style, originPatch->alpha);
					}
					texelu16 = (UINT16)((alpha<<8) | texel);
					memcpy(dest, &texelu16, sizeof(UINT16));
					break;
				case 3:
					colortemp = palette[texel];
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
					{
						RGBA_t rgbatexel;
						rgbatexel.rgba = *(UINT32 *)dest;
						colortemp.rgba = ASTBlendTexturePixel(rgbatexel, colortemp, originPatch->style, originPatch->alpha);
					}
					memcpy(dest, &colortemp, sizeof(RGBA_t)-sizeof(UINT8));
					break;
				case 4:
					colortemp = palette[texel];
					colortemp.s.alpha = alpha;
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
					{
						RGBA_t rgbatexel;
						rgbatexel.rgba = *(UINT32 *)dest;
						colortemp.rgba = ASTBlendTexturePixel(rgbatexel, colortemp, originPatch->style, originPatch->alpha);
					}
					memcpy(dest, &colortemp, sizeof(RGBA_t));
					break;
				// default is 1
				default:
					if ((originPatch != NULL) && (originPatch->style != AST_COPY))
						*dest = ASTBlendPaletteIndexes(*dest, texel, originPatch->style, originPatch->alpha);
					else
						*dest = texel;
					break;
			}
		}
	}
}

// Simplified patch caching function
// for use by sprites and other patches that are not part of a wall texture
// no alpha or flipping should be present since we do not want non-texture graphics to have them
// no offsets are used either
// -- Monster Iestyn (13/02/19)
static void HWR_DrawPatchInCache(GLMipmap_t *mipmap,
	INT32 pblockwidth, INT32 pblockheight,
	INT32 pwidth, INT32 pheight,
	const patch_t *realpatch)
{
	INT32 ncols;
	fixed_t xfrac, xfracstep;
	fixed_t yfracstep, scale_y;
	const column_t *patchcol;
	UINT8 *block = mipmap->data;
	INT32 bpp;
	INT32 blockmodulo;
	RGBA_t *palette;

	if (pwidth <= 0 || pheight <= 0)
		return;

	palette = HWR_GetTexturePalette();

	ncols = pwidth;

	// source advance
	xfrac = 0;
	xfracstep = FRACUNIT;
	yfracstep = FRACUNIT;
	scale_y   = FRACUNIT;

	bpp = format2bpp(mipmap->format);

	if (bpp < 1 || bpp > 4)
		I_Error("HWR_DrawPatchInCache: no drawer defined for this bpp (%d)\n",bpp);

	// NOTE: should this actually be pblockwidth*bpp?
	blockmodulo = pblockwidth*bpp;

	// Draw each column to the block cache
	for (; ncols--; block += bpp, xfrac += xfracstep)
	{
		patchcol = &realpatch->columns[xfrac>>FRACBITS];

		HWR_DrawColumnInCache(patchcol, block, mipmap,
								pblockheight, blockmodulo,
								yfracstep, scale_y,
								NULL, pheight, // not that pheight is going to get used anyway...
								bpp, palette);
	}
}

// This function we use for caching patches that belong to textures
static void HWR_DrawTexturePatchInCache(GLMipmap_t *mipmap,
	INT32 pblockwidth, INT32 pblockheight,
	texture_t *texture, texpatch_t *patch,
	const patch_t *realpatch)
{
	INT32 x, x1, x2;
	INT32 col, ncols;
	fixed_t xfrac, xfracstep;
	fixed_t yfracstep, scale_y;
	const column_t *patchcol;
	UINT8 *block = mipmap->data;
	INT32 bpp;
	INT32 blockmodulo;
	INT32 width, height;
	RGBA_t *palette;
	// Column drawing function pointer.
	static void (*ColumnDrawerPointer)(const column_t *patchcol, UINT8 *block, GLMipmap_t *mipmap,
								INT32 pblockheight, INT32 blockmodulo,
								fixed_t yfracstep, fixed_t scale_y,
								texpatch_t *originPatch, INT32 patchheight,
								INT32 bpp, RGBA_t *palette);

	if (texture->width <= 0 || texture->height <= 0)
		return;

	palette = HWR_GetTexturePalette();

	ColumnDrawerPointer = (patch->flip & 2) ? HWR_DrawFlippedColumnInCache : HWR_DrawColumnInCache;

	x1 = patch->originx;
	width = realpatch->width;
	height = realpatch->height;
	x2 = x1 + width;

	if (x1 > texture->width || x2 < 0)
		return; // patch not located within texture's x bounds, ignore

	if (patch->originy > texture->height || (patch->originy + height) < 0)
		return; // patch not located within texture's y bounds, ignore

	// patch is actually inside the texture!
	// now check if texture is partly off-screen and adjust accordingly

	// left edge
	if (x1 < 0)
		x = 0;
	else
		x = x1;

	// right edge
	if (x2 > texture->width)
		x2 = texture->width;


	col = x * pblockwidth / texture->width;
	ncols = ((x2 - x) * pblockwidth) / texture->width;

	// source advance
	xfrac = 0;
	if (x1 < 0)
		xfrac = -x1<<FRACBITS;

	xfracstep = (texture->width << FRACBITS) / pblockwidth;
	yfracstep = (texture->height<< FRACBITS) / pblockheight;
	scale_y   = (pblockheight  << FRACBITS) / texture->height;

	bpp = format2bpp(mipmap->format);

	if (bpp < 1 || bpp > 4)
		I_Error("HWR_DrawTexturePatchInCache: no drawer defined for this bpp (%d)\n",bpp);

	// NOTE: should this actually be pblockwidth*bpp?
	blockmodulo = pblockwidth*bpp;

	// Draw each column to the block cache
	for (block += col*bpp; ncols--; block += bpp, xfrac += xfracstep)
	{
		if (patch->flip & 1)
			patchcol = &realpatch->columns[(width-1)-(xfrac>>FRACBITS)];
		else
			patchcol = &realpatch->columns[xfrac>>FRACBITS];

		ColumnDrawerPointer(patchcol, block, mipmap,
								pblockheight, blockmodulo,
								yfracstep, scale_y,
								patch, height,
								bpp, palette);
	}
}

static UINT8 *MakeBlock(GLMipmap_t *grMipmap)
{
	UINT8 *block;
	INT32 bpp, i;
	UINT16 bu16 = ((0x00 <<8) | HWR_PATCHES_CHROMAKEY_COLORINDEX);
	INT32 blocksize = (grMipmap->width * grMipmap->height);

	bpp =  format2bpp(grMipmap->format);
	block = Z_Malloc(blocksize*bpp, PU_HWRCACHE, &(grMipmap->data));

	switch (bpp)
	{
		case 1: memset(block, HWR_PATCHES_CHROMAKEY_COLORINDEX, blocksize); break;
		case 2:
				// fill background with chromakey, alpha = 0
				for (i = 0; i < blocksize; i++)
				//[segabor]
					memcpy(block+i*sizeof(UINT16), &bu16, sizeof(UINT16));
				break;
		case 4: memset(block, 0x00, blocksize*sizeof(UINT32)); break;
	}

	return block;
}

//
// Create a composite texture from patches, adapt the texture size to a power of 2
// height and width for the hardware texture cache.
//
static void HWR_GenerateTexture(INT32 texnum, GLMapTexture_t *grtex, GLMipmap_t *mipmap)
{
	UINT8 *block;
	texture_t *texture;
	texpatch_t *patch;
	INT32 blockwidth, blockheight, blocksize;

	INT32 i;

	texture = textures[texnum];

	blockwidth = texture->width;
	blockheight = texture->height;
	blocksize = blockwidth * blockheight;
	block = MakeBlock(mipmap);

	// Composite the columns together.
	for (i = 0, patch = texture->patches; i < texture->patchcount; i++, patch++)
	{
		UINT16 wadnum = patch->wad;
		lumpnum_t lumpnum = patch->lump;
		UINT8 *pdata = W_CacheLumpNumPwad(wadnum, lumpnum, PU_CACHE);
		patch_t *realpatch = NULL;
		boolean free_patch = true;

#ifndef NO_PNG_LUMPS
		size_t lumplength = W_LumpLengthPwad(wadnum, lumpnum);
		if (Picture_IsLumpPNG(pdata, lumplength))
			realpatch = (patch_t *)Picture_PNGConvert(pdata, PICFMT_PATCH, NULL, NULL, NULL, NULL, lumplength, NULL, 0);
		else
#endif
		if (texture->type == TEXTURETYPE_FLAT)
			realpatch = (patch_t *)Picture_Convert(PICFMT_FLAT, pdata, PICFMT_PATCH, 0, NULL, texture->width, texture->height, 0, 0, 0);
		else
		{
			// If this patch has already been loaded, we just use it from the cache.
			realpatch = W_GetCachedPatchNumPwad(wadnum, lumpnum);
			free_patch = false;

			// Otherwise, we load it here.
			if (realpatch == NULL)
				realpatch = W_CachePatchNumPwad(wadnum, lumpnum, PU_PATCH);
		}

		if (realpatch != NULL)
		{
			HWR_DrawTexturePatchInCache(mipmap, blockwidth, blockheight, texture, patch, realpatch);

			if (free_patch)
				Patch_Free(realpatch);
		}
	}
	//Hurdler: not efficient at all but I don't remember exactly how HWR_DrawPatchInCache works :(
	if (format2bpp(mipmap->format)==4)
	{
		for (i = 3; i < blocksize*4; i += 4) // blocksize*4 because blocksize doesn't include the bpp
		{
			if (block[i] == 0)
			{
				mipmap->flags |= TF_TRANSPARENT;
				break;
			}
		}
	}

	grtex->scaleX = 1.0f/(texture->width*FRACUNIT);
	grtex->scaleY = 1.0f/(texture->height*FRACUNIT);
}

// patch may be NULL if grMipmap has been initialised already and makebitmap is false
void HWR_MakePatch (const patch_t *patch, GLPatch_t *grPatch, GLMipmap_t *grMipmap, boolean makebitmap)
{
	if (grMipmap->width == 0)
	{
		grMipmap->width = grMipmap->height = 1;
		while (grMipmap->width < patch->width) grMipmap->width <<= 1;
		while (grMipmap->height < patch->height) grMipmap->height <<= 1;

		// no wrap around, no chroma key
		grMipmap->flags = 0;

		// setup the texture info
		grMipmap->format = patchformat;

		grPatch->max_s = (float)patch->width / (float)grMipmap->width;
		grPatch->max_t = (float)patch->height / (float)grMipmap->height;
	}

	Z_Free(grMipmap->data);
	grMipmap->data = NULL;

	if (makebitmap)
	{
		MakeBlock(grMipmap);

		HWR_DrawPatchInCache(grMipmap,
			grMipmap->width, grMipmap->height,
			patch->width, patch->height,
			patch);
	}
}


// =================================================
//             CACHING HANDLING
// =================================================

static size_t gl_numtextures = 0; // Texture count
static GLMapTexture_t *gl_textures; // For all textures
static GLMapTexture_t *gl_flats; // For all (texture) flats, as normal flats don't need to be cached
boolean gl_maptexturesloaded = false;

void HWR_FreeTextureData(patch_t *patch)
{
	GLPatch_t *grPatch;

	if (!patch || !patch->hardware)
		return;

	grPatch = patch->hardware;

	if (vid.glstate == VID_GL_LIBRARY_LOADED)
		HWD.pfnDeleteTexture(grPatch->mipmap);
	if (grPatch->mipmap->data)
		Z_Free(grPatch->mipmap->data);
}

void HWR_FreeTexture(patch_t *patch)
{
	if (!patch)
		return;

	if (patch->hardware)
	{
		GLPatch_t *grPatch = patch->hardware;

		HWR_FreeTextureColormaps(patch);

		if (grPatch->mipmap)
		{
			HWR_FreeTextureData(patch);
			Z_Free(grPatch->mipmap);
		}

		Z_Free(patch->hardware);
	}

	patch->hardware = NULL;
}

// Called by HWR_FreePatchCache.
void HWR_FreeTextureColormaps(patch_t *patch)
{
	GLPatch_t *pat;

	// The patch must be valid, obviously
	if (!patch)
		return;

	pat = patch->hardware;
	if (!pat)
		return;

	// The mipmap must be valid, obviously
	while (pat->mipmap)
	{
		// Confusing at first, but pat->mipmap->nextcolormap
		// at the beginning of the loop is the first colormap
		// from the linked list of colormaps.
		GLMipmap_t *next = NULL;

		// No mipmap in this patch, break out of the loop.
		if (!pat->mipmap)
			break;

		// No colormap mipmaps either.
		if (!pat->mipmap->nextcolormap)
			break;

		// Set the first colormap to the one that comes after it.
		next = pat->mipmap->nextcolormap;
		pat->mipmap->nextcolormap = next->nextcolormap;

		// Free image data from memory.
		if (next->data)
			Z_Free(next->data);
		if (next->colormap)
			Z_Free(next->colormap);
		HWD.pfnDeleteTexture(next);

		// Free the old colormap mipmap from memory.
		free(next);
	}
}

static boolean FreeTextureCallback(void *mem)
{
	patch_t *patch = (patch_t *)mem;
	HWR_FreeTexture(patch);
	return false;
}

static boolean FreeColormapsCallback(void *mem)
{
	patch_t *patch = (patch_t *)mem;
	HWR_FreeTextureColormaps(patch);
	return false;
}

static void HWR_FreePatchCache(boolean freeall)
{
	boolean (*callback)(void *mem) = FreeTextureCallback;

	if (!freeall)
		callback = FreeColormapsCallback;

	Z_IterateTags(PU_PATCH, PU_PATCH_ROTATED, callback);
	Z_IterateTags(PU_SPRITE, PU_HUDGFX, callback);
}

// free all textures after each level
void HWR_ClearAllTextures(void)
{
	HWD.pfnClearMipMapCache(); // free references to the textures
	HWR_FreePatchCache(true);
}

void HWR_FreeColormapCache(void)
{
	HWR_FreePatchCache(false);
}

void HWR_InitMapTextures(void)
{
	gl_textures = NULL;
	gl_flats = NULL;
	gl_maptexturesloaded = false;
}

static void DeleteTextureMipmap(GLMipmap_t *grMipmap, boolean delete_mipmap)
{
	HWD.pfnDeleteTexture(grMipmap);

	if (delete_mipmap)
		Z_Free(grMipmap->data);
}

static void FreeMapTexture(GLMapTexture_t *tex, boolean delete_chromakeys)
{
	if (tex->mipmap.nextcolormap)
	{
		DeleteTextureMipmap(tex->mipmap.nextcolormap, delete_chromakeys);
		free(tex->mipmap.nextcolormap);
		tex->mipmap.nextcolormap = NULL;
	}

	DeleteTextureMipmap(&tex->mipmap, true);
}

void HWR_FreeMapTextures(void)
{
	size_t i;

	for (i = 0; i < gl_numtextures; i++)
	{
		FreeMapTexture(&gl_textures[i], true);
		FreeMapTexture(&gl_flats[i], false);
	}

	// now the heap don't have any 'user' pointing to our
	// texturecache info, we can free it
	if (gl_textures)
		free(gl_textures);
	if (gl_flats)
		free(gl_flats);
	gl_textures = NULL;
	gl_flats = NULL;
	gl_numtextures = 0;
	gl_maptexturesloaded = false;
}

void HWR_LoadMapTextures(size_t pnumtextures)
{
	// we must free it since numtextures may have changed
	HWR_FreeMapTextures();

	gl_numtextures = pnumtextures;
	gl_textures = calloc(gl_numtextures, sizeof(*gl_textures));
	gl_flats = calloc(gl_numtextures, sizeof(*gl_flats));

	if (gl_textures == NULL || gl_flats == NULL)
		I_Error("HWR_LoadMapTextures: ran out of memory for OpenGL textures");

	gl_maptexturesloaded = true;
}

// --------------------------------------------------------------------------
// Make sure texture is downloaded and set it as the source
// --------------------------------------------------------------------------
GLMapTexture_t *HWR_GetTexture(INT32 tex, boolean chromakeyed)
{
	if (tex < 0 || tex >= (signed)gl_numtextures)
	{
#ifdef PARANOIA
		I_Error("HWR_GetTexture: Invalid texture ID %d", tex);
#else
		tex = 0;
#endif
	}

	GLMapTexture_t *grtex = &gl_textures[tex];

	GLMipmap_t *grMipmap = &grtex->mipmap;
	GLMipmap_t *originalMipmap = grMipmap;

	if (!originalMipmap->downloaded)
	{
		originalMipmap->flags = TF_WRAPXY;
		originalMipmap->width = (UINT16)textures[tex]->width;
		originalMipmap->height = (UINT16)textures[tex]->height;
		originalMipmap->format = textureformat;
	}

	// If chroma-keyed, create or use a different mipmap for the variant
	if (chromakeyed && !textures[tex]->transparency)
	{
		// Allocate it if it wasn't already
		if (!originalMipmap->nextcolormap)
		{
			GLMipmap_t *newMipmap = calloc(1, sizeof (*grMipmap));
			if (newMipmap == NULL)
				I_Error("%s: Out of memory", "HWR_GetTexture");

			newMipmap->flags = originalMipmap->flags | TF_CHROMAKEYED;
			newMipmap->width = originalMipmap->width;
			newMipmap->height = originalMipmap->height;
			newMipmap->format = originalMipmap->format;
			originalMipmap->nextcolormap = newMipmap;
		}

		// Generate, upload and bind the variant texture instead of the original one
		grMipmap = originalMipmap->nextcolormap;
	}

	if (!grMipmap->data)
		HWR_GenerateTexture(tex, grtex, grMipmap);

	if (!grMipmap->downloaded)
		HWD.pfnSetTexture(grMipmap);
	HWR_SetCurrentTexture(grMipmap);

	Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);

	return grtex;
}

static void HWR_CacheRawFlat(GLMipmap_t *grMipmap, lumpnum_t flatlumpnum)
{
	size_t size = W_LumpLength(flatlumpnum);
	UINT16 pflatsize = R_GetFlatSize(size);

	// setup the texture info
	grMipmap->format = GL_TEXFMT_P_8;
	grMipmap->flags = TF_WRAPXY;

	grMipmap->width = pflatsize;
	grMipmap->height = pflatsize;

	// the flat raw data needn't be converted with palettized textures
	W_ReadLump(flatlumpnum, Z_Malloc(size, PU_HWRCACHE, &grMipmap->data));
}

// Download a Doom 'flat' to the hardware cache and make it ready for use
void HWR_GetRawFlat(lumpnum_t flatlumpnum)
{
	GLMipmap_t *grmip;
	patch_t *patch;

	if (flatlumpnum == LUMPERROR)
		return;

	patch = HWR_GetCachedGLPatch(flatlumpnum);
	grmip = ((GLPatch_t *)Patch_AllocateHardwarePatch(patch))->mipmap;
	if (!grmip->downloaded && !grmip->data)
		HWR_CacheRawFlat(grmip, flatlumpnum);

	// If hardware does not have the texture, then call pfnSetTexture to upload it
	if (!grmip->downloaded)
		HWD.pfnSetTexture(grmip);
	HWR_SetCurrentTexture(grmip);

	// The system-memory data can be purged now.
	Z_ChangeTag(grmip->data, PU_HWRCACHE_UNLOCKED);
}

static void MakeLevelFlatMipmap(GLMipmap_t *grMipmap, INT32 texturenum, UINT16 flags)
{
	grMipmap->format = GL_TEXFMT_P_8;
	grMipmap->flags = flags;

	grMipmap->width  = (UINT16)textures[texturenum]->width;
	grMipmap->height = (UINT16)textures[texturenum]->height;
}

void HWR_GetLevelFlat(levelflat_t *levelflat, boolean chromakeyed)
{
	if (levelflat->type == LEVELFLAT_NONE || levelflat->texture_id < 0)
	{
		HWR_SetCurrentTexture(NULL);
		return;
	}

	INT32 texturenum = texturetranslation[levelflat->texture_id];

	GLMapTexture_t *grtex = &gl_flats[texturenum];

	GLMipmap_t *grMipmap = &grtex->mipmap;
	GLMipmap_t *originalMipmap = grMipmap;

	if (!originalMipmap->downloaded)
		MakeLevelFlatMipmap(originalMipmap, texturenum, TF_WRAPXY);

	if (!originalMipmap->data)
	{
		size_t size = originalMipmap->width * originalMipmap->height;
		memcpy(Z_Malloc(size, PU_HWRCACHE, &originalMipmap->data), R_GetFlatForTexture(texturenum), size);
	}

	// If chroma-keyed, create or use a different mipmap for the variant
	if (chromakeyed)
	{
		if (!originalMipmap->data)
		{
			HWR_SetCurrentTexture(NULL);
			return;
		}

		// Allocate it if it wasn't already
		if (!originalMipmap->nextcolormap)
		{
			GLMipmap_t *newMipmap = calloc(1, sizeof (*grMipmap));
			if (newMipmap == NULL)
				I_Error("%s: Out of memory", "HWR_GetLevelFlat");
			MakeLevelFlatMipmap(newMipmap, texturenum, TF_WRAPXY | TF_CHROMAKEYED);
			originalMipmap->nextcolormap = newMipmap;
		}

		// Upload and bind the variant texture instead of the original one
		grMipmap = originalMipmap->nextcolormap;

		// Use the original texture's pixel data
		// It can just be a pointer to it, since the r_opengl backend deals with the pixels
		// that are supposed to be transparent.
		grMipmap->data = originalMipmap->data;
	}

	if (!grMipmap->downloaded)
		HWD.pfnSetTexture(grMipmap);
	HWR_SetCurrentTexture(grMipmap);

	Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);
}

// --------------------+
// HWR_LoadPatchMipmap : Generates a patch into a mipmap, usually the mipmap inside the patch itself
// --------------------+
static void HWR_LoadPatchMipmap(patch_t *patch, GLMipmap_t *grMipmap)
{
	GLPatch_t *grPatch = patch->hardware;
	if (!grMipmap->downloaded && !grMipmap->data)
		HWR_MakePatch(patch, grPatch, grMipmap, true);

	// If hardware does not have the texture, then call pfnSetTexture to upload it
	if (!grMipmap->downloaded)
		HWD.pfnSetTexture(grMipmap);
	HWR_SetCurrentTexture(grMipmap);

	// The system-memory data can be purged now.
	Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);
}

// ----------------------+
// HWR_UpdatePatchMipmap : Updates a mipmap.
// ----------------------+
static void HWR_UpdatePatchMipmap(patch_t *patch, GLMipmap_t *grMipmap)
{
	GLPatch_t *grPatch = patch->hardware;
	HWR_MakePatch(patch, grPatch, grMipmap, true);

	// If hardware does not have the texture, then call pfnSetTexture to upload it
	// If it does have the texture, then call pfnUpdateTexture to update it
	if (!grMipmap->downloaded)
		HWD.pfnSetTexture(grMipmap);
	else
		HWD.pfnUpdateTexture(grMipmap);
	HWR_SetCurrentTexture(grMipmap);

	// The system-memory data can be purged now.
	Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);
}

// -----------------+
// HWR_GetPatch     : Downloads a patch to the hardware cache and make it ready for use
// -----------------+
void HWR_GetPatch(patch_t *patch)
{
	if (!patch->hardware)
		Patch_CreateGL(patch);
	HWR_LoadPatchMipmap(patch, ((GLPatch_t *)patch->hardware)->mipmap);
}

// -------------------+
// HWR_GetMappedPatch : Same as HWR_GetPatch for sprite color
// -------------------+
void HWR_GetMappedPatch(patch_t *patch, const UINT8 *colormap)
{
	GLPatch_t *grPatch;
	GLMipmap_t *grMipmap, *newMipmap;

	if (!patch->hardware)
		Patch_CreateGL(patch);
	grPatch = patch->hardware;

	if (colormap == colormaps || colormap == NULL)
	{
		// Load the default (green) color in hardware cache
		HWR_GetPatch(patch);
		return;
	}

	// search for the mipmap
	// skip the first (no colormap translated)
	for (grMipmap = grPatch->mipmap; grMipmap->nextcolormap; )
	{
		grMipmap = grMipmap->nextcolormap;
		if (grMipmap->colormap && grMipmap->colormap->source == colormap)
		{
			if (memcmp(grMipmap->colormap->data, colormap, 256 * sizeof(UINT8)))
			{
				M_Memcpy(grMipmap->colormap->data, colormap, 256 * sizeof(UINT8));
				HWR_UpdatePatchMipmap(patch, grMipmap);
			}
			else
				HWR_LoadPatchMipmap(patch, grMipmap);
			return;
		}
	}
	// not found, create it!
	// If we are here, the sprite with the current colormap is not already in hardware memory

	//BP: WARNING: don't free it manually without clearing the cache of harware renderer
	//              (it have a liste of mipmap)
	//    this malloc is cleared in HWR_FreeColormapCache
	//    (...) unfortunately z_malloc fragment alot the memory :(so malloc is better
	newMipmap = calloc(1, sizeof (*newMipmap));
	if (newMipmap == NULL)
		I_Error("%s: Out of memory", "HWR_GetMappedPatch");
	grMipmap->nextcolormap = newMipmap;

	newMipmap->colormap = Z_Calloc(sizeof(*newMipmap->colormap), PU_HWRPATCHCOLMIPMAP, NULL);
	newMipmap->colormap->source = colormap;
	M_Memcpy(newMipmap->colormap->data, colormap, 256 * sizeof(UINT8));

	HWR_LoadPatchMipmap(patch, newMipmap);
}

void HWR_UnlockCachedPatch(GLPatch_t *gpatch)
{
	if (!gpatch)
		return;

	Z_ChangeTag(gpatch->mipmap->data, PU_HWRCACHE_UNLOCKED);
}

#if 1
// STAR NOTE: i respect the hustle!
// However, I kinda need this for my coronas!
// So....... it's staying for now :p

#include "hw_light.h"
#include "../STAR/lights/smkg-coronas.h"

#define PIC_DEFINED

#ifdef PIC_DEFINED
typedef enum
{
	PALETTE         = 0,  // 1 byte is the index in the doom palette (as usual)
	INTENSITY       = 1,  // 1 byte intensity
	INTENSITY_ALPHA = 2,  // 2 byte: alpha then intensity
	RGB24           = 3,  // 24 bit rgb
	RGBA32          = 4,  // 32 bit rgba
} pic_mode_t;

// a pic is an unmasked block of pixels, stored in horizontal way
typedef struct
{
	INT16 width;
	UINT8 zero;       // set to 0 allow autodetection of pic_t
					 // mode instead of patch or raw
	UINT8 mode;       // see pic_mode_t above
	INT16 height;
	INT16 reserved1; // set to 0
	UINT8 data[0];
} ATTRPACK pic_t;

static const INT32 picmode2GR[] =
{
	GL_TEXFMT_P_8,                // PALETTE
	0,                            // INTENSITY          (unsupported yet)
	GL_TEXFMT_ALPHA_INTENSITY_88, // INTENSITY_ALPHA    (corona use this)
	0,                            // RGB24              (unsupported yet)
	GL_TEXFMT_RGBA,               // RGBA32             (opengl only)
};

#define PIC_TYPE pic_t
#else
#define PIC_TYPE patch_t
#endif

static void HWR_DrawPicInCache(UINT8 *block, INT32 pblockwidth, INT32 pblockheight, INT32 blockmodulo, PIC_TYPE *pic, INT32 bpp)
{
	INT32 i,j;
	fixed_t posx, posy, stepx, stepy;
	UINT8 *dest, *src, texel;
	UINT16 texelu16;
	INT32 picbpp;
	RGBA_t col;
	RGBA_t *palette = HWR_GetTexturePalette();

	stepy = ((INT32)SHORT(pic->height)<<FRACBITS)/pblockheight;
	stepx = ((INT32)SHORT(pic->width)<<FRACBITS)/pblockwidth;
#ifdef PIC_DEFINED
	picbpp = format2bpp(picmode2GR[pic->mode]);
#else
	GLPatch_t *glPic = pic->hardware;
	if (glPic == NULL || glPic->mipmap == NULL)
		return;
	picbpp = format2bpp(glPic->mipmap->format);
#endif
	posy = 0;
	for (j = 0; j < pblockheight; j++)
	{
		posx = 0;
#ifdef PIC_DEFINED
		dest = &block[j*blockmodulo];
		src = &pic->data[(posy>>FRACBITS)*SHORT(pic->width)*picbpp];
#else
		dest = block + (j*blockmodulo);
		UINT8 *data = glPic->mipmap->data;
		//src = &data[(posy>>FRACBITS)*pic->width*picbpp];
		src = &data[(posy>>FRACBITS)*SHORT(pic->width)*picbpp];
#endif
		for (i = 0; i < pblockwidth;i++)
		{
#ifdef PIC_DEFINED
			switch (pic->mode)
			{ // source bpp
				case PALETTE :
					texel = src[(posx+FRACUNIT/2)>>FRACBITS];
					switch (bpp)
					{ // destination bpp
						case 1 :
							*dest++ = texel; break;
						case 2 :
							texelu16 = (UINT16)(texel | 0xff00);
							memcpy(dest, &texelu16, sizeof(UINT16));
							dest += sizeof(UINT16);
							break;
						case 3 :
							col = palette[texel];
							memcpy(dest, &col, sizeof(RGBA_t)-sizeof(UINT8));
							dest += sizeof(RGBA_t)-sizeof(UINT8);
							break;
						case 4 :
							memcpy(dest, &palette[texel], sizeof(RGBA_t));
							dest += sizeof(RGBA_t);
							break;
					}
					break;
				case INTENSITY :
					*dest++ = src[(posx+FRACUNIT/2)>>FRACBITS];
					break;
				case INTENSITY_ALPHA : // assume dest bpp = 2
					memcpy(dest, src + ((posx+FRACUNIT/2)>>FRACBITS)*sizeof(UINT16), sizeof(UINT16));
					dest += sizeof(UINT16);
					break;
				case RGB24 :
					break;  // not supported yet
				case RGBA32 : // assume dest bpp = 4
					dest += sizeof(UINT32);
					memcpy(dest, src + ((posx+FRACUNIT/2)>>FRACBITS)*sizeof(UINT32), sizeof(UINT32));
					break;
			}
#else
			switch (glPic->mipmap->format)
			{ // source bpp
				case GL_TEXFMT_P_8 :
					texel = src[(posx+FRACUNIT/2)>>FRACBITS];
					switch (bpp)
					{ // destination bpp
						case 1 :
							*dest++ = texel; break;
						case 2 :
							texelu16 = (UINT16)(texel | 0xff00);
							memcpy(dest, &texelu16, sizeof(UINT16));
							dest += sizeof(UINT16);
							break;
						case 3 :
							col = palette[texel];
							memcpy(dest, &col, sizeof(RGBA_t)-sizeof(UINT8));
							dest += sizeof(RGBA_t)-sizeof(UINT8);
							break;
						case 4 :
							memcpy(dest, &palette[texel], sizeof(RGBA_t));
							dest += sizeof(RGBA_t);
							break;
					}
					break;
#if 0
				case INTENSITY :
					*dest++ = src[(posx+FRACUNIT/2)>>FRACBITS];
					break;
#endif
				case GL_TEXFMT_ALPHA_INTENSITY_88 : // assume dest bpp = 2
					memcpy(dest, src + ((posx+FRACUNIT/2)>>FRACBITS)*sizeof(UINT16), sizeof(UINT16));
					dest += sizeof(UINT16);
					break;
#if 0
				case RGB24 :
					break;  // not supported yet
#endif
				case GL_TEXFMT_RGBA : // assume dest bpp = 4
					dest += sizeof(UINT32);
					memcpy(dest, src + ((posx+FRACUNIT/2)>>FRACBITS)*sizeof(UINT32), sizeof(UINT32));
					break;
				default:
					break;
			}
#endif
			posx += stepx;
		}
		posy += stepy;
	}
}

// -----------------+
// HWR_GetPic       : Download a Doom pic (raw row encoded with no 'holes')
// Returns          :
// -----------------+

#ifndef PIC_DEFINED
#define NEW_GETPIC
#endif

#ifndef NEW_GETPIC

#define VANILLA_GETPIC
#ifndef VANILLA_GETPIC // ^ depeonds on

#define HWR_RESIZEBLOCK
#ifdef HWR_RESIZEBLOCK // ^ depeonds on

// resize the patch to be 3dfx complient
// set : blocksize = blockwidth * blockheight  (no bpp used)
//       blockwidth
//       blockheight
//note :  8bit (1 byte per pixel) palettized format
INT32 blockwidth, blockheight, blocksize;
static void HWR_ResizeBlock(int orig_width, int orig_height, GLMipmap_t *mipmap)
{
#if 1
	// GL prefers power-of-two textures.
	{
		// width: size up to nearest power of 2
		blockwidth = 1;
		while(blockwidth < orig_width)
			blockwidth <<= 1;
		if(blockwidth>2048)
		{
			// scale down the original graphics to fit
			blockwidth = 2048;
		}

		//height: size up to nearest power of 2
		blockheight = 1;
		while(blockheight < orig_height)
			blockheight <<= 1;
		if(blockheight>2048)
		{
			// scale down the original graphics to fit
			blockheight = 2048;
		}
	}
#else
	blockwidth = orig_width;
	blockheight = orig_height;
#endif
	blocksize = blockwidth * blockheight;
}

#endif // HWR_RESIZEBLOCK

void HWR_GetPic(lumpnum_t lumpnum, const UINT8 *colormap)
{
	patch_t *patch;
	GLPatch_t *grPatch;

	(void)colormap;

	if (lumpnum == LUMPERROR)
		return;

	patch = HWR_GetCachedGLPatch(lumpnum);
	grPatch = (GLPatch_t *)Patch_AllocateHardwarePatch(patch);

	if (!grPatch->mipmap->downloaded && !grPatch->mipmap->data)
	{
		UINT8 *block;
		const size_t lumpsize = W_LumpLength(lumpnum);
#ifdef PIC_DEFINED
#define SIZES_ARE_SHORT
		size_t len = (lumpsize - sizeof(pic_t));
		pic_t *pic = W_CacheLumpNum(lumpnum, PU_CACHE);
#else
//#define SIZES_ARE_SHORT
		size_t len = (lumpsize - sizeof(patch_t));
		patch_t *pic = HWR_GetCachedGLPatch(lumpnum);
		//patch_t *pic = W_CacheLumpNum(lumpnum, PU_CACHE);
		GLPatch_t *glPic = (GLPatch_t *)Patch_AllocateHardwarePatch(pic);
#endif

		INT32 width = SHORT(pic->width);
		INT32 height = SHORT(pic->height);
		CONS_Printf("width is %d-%d, height is %d-%d\n", pic->width, width, pic->height, height);
#ifdef PIC_DEFINED
		patch->width = SHORT(pic->width);
		patch->height = SHORT(pic->height);
#else
#ifndef SIZES_ARE_SHORT
		patch->width = pic->width;
		patch->height = pic->height;
#else
		patch->width = SHORT(pic->width);
		patch->height = SHORT(pic->height);
#endif
#endif
		patch->leftoffset = 0;
		patch->topoffset = 0;

#ifdef PIC_DEFINED
		grPatch->mipmap->format = ((pic->mode == PALETTE) ? textureformat : picmode2GR[pic->mode]); // 'textureformat' can be set by driver
		CONS_Printf("picformat is %d\n", pic->mode);
#else
		//grPatch->mipmap->format = patchformat;
		grPatch->mipmap->format = textureformat;
		//grPatch->mipmap->format = GL_TEXFMT_ALPHA_8;
		//grPatch->mipmap->format = 0;
#endif
		CONS_Printf("format is %d\n", grPatch->mipmap->format);
		grPatch->mipmap->width = (UINT16)patch->width;
		grPatch->mipmap->height = (UINT16)patch->height;
#if 1
		grPatch->mipmap->flags = 0;
		grPatch->max_s = grPatch->max_t = 1.0f;
#endif

#if 0
		Z_Free(grPatch->mipmap->data);
#endif

#ifdef HWR_RESIZEBLOCK
		// find the good 3dfx size (boring spec)
		HWR_ResizeBlock(patch->width, patch->height, grPatch->mipmap);
#endif

#if 0
		// allocate block
#if 1
		(void)len;
		W_ReadLump(lumpnum, Z_Malloc(lumpsize, PU_HWRCACHE, &grPatch->mipmap->data));
#else
		W_ReadLump(lumpnum, Z_Malloc(len, PU_HWRCACHE, &grPatch->mipmap->data));
#endif
#endif

#define USE_BLOCK
//#define USE_GLPIC

#ifdef USE_BLOCK
		block = MakeBlock(grPatch->mipmap);
#else
		(void)block;
#endif

#ifdef PIC_DEFINED

		if (
#ifndef HWR_RESIZEBLOCK
			patch->width  == SHORT(pic->width) &&
			patch->height == SHORT(pic->height) &&
#else
			patch->width  == blockwidth &&
			patch->height == blockheight &&
#endif
			format2bpp(grPatch->mipmap->format) == format2bpp(picmode2GR[pic->mode]))

#else
#ifndef USE_GLPIC
		if (grPatch && grPatch->mipmap)
#else
		if (glPic && glPic->mipmap)
#endif
		{
			if (
#ifndef SIZES_ARE_SHORT
				patch->width  == pic->width &&
				patch->height == pic->height
#else
				patch->width  == SHORT(pic->width) &&
				patch->height == SHORT(pic->height)
#endif
				//&& format2bpp(grPatch->mipmap->format) == format2bpp(glPic->mipmap->format)
			)
#endif
			{
				// no conversion needed
				CONS_Debug(DBG_RENDER, "no HWR_GetPic conversion needed\n");
#ifdef PIC_DEFINED
				M_Memcpy(grPatch->mipmap->data, pic->data, len);
#else
				//M_Memcpy(grPatch->mipmap->data, glPic->mipmap->data, len);
				grPatch->mipmap->data = glPic->mipmap->data;
#endif
			}
			else
			{
				CONS_Debug(DBG_RENDER, "HWR_GetPic conversion needed\n");
#ifdef USE_BLOCK

				HWR_DrawPicInCache(block,
#ifndef HWR_RESIZEBLOCK
					SHORT(pic->width), SHORT(pic->height),
					SHORT(pic->width)*format2bpp(grPatch->mipmap->format),
#else
					min(pic->width,blockwidth), min(pic->height,blockheight),
					blockwidth*format2bpp(grPatch->mipmap->format),
#endif
					pic,
					format2bpp(grPatch->mipmap->format));

#else
#ifndef USE_GLPIC
				HWR_DrawPatchInCache(grPatch->mipmap,
					grPatch->mipmap->width, grPatch->mipmap->height,
					patch->width, patch->height,
					patch);
#else
				HWR_DrawPatchInCache(glPic->mipmap,
					glPic->mipmap->width, glPic->mipmap->height,
					pic->width, pic->height,
					pic);
				//HWR_MakePatch(pic, glPic, glPic->mipmap, true);
				//patch = pic;
#endif
#endif
			}
#ifndef PIC_DEFINED
		}
#endif

#if 1
		Z_Unlock(pic);
#else
		Z_ChangeTag(pic, PU_CACHE);
#endif
#ifdef USE_BLOCK
		Z_ChangeTag(block, PU_HWRCACHE); // PU_HWRCACHE_UNLOCKED // PU_HWRCACHE
#endif

		grPatch->mipmap->flags = 0;
#ifndef HWR_RESIZEBLOCK
#if 0
		grPatch->max_s = grPatch->max_t = 1.0f;
#else
		grPatch->max_s = (float)grPatch->mipmap->width  / (float)patch->width;
		grPatch->max_t = (float)grPatch->mipmap->height / (float)patch->height;
#endif
#else
		grPatch->max_s = (float)grPatch->mipmap->width  / (float)blockwidth;
		grPatch->max_t = (float)grPatch->mipmap->height / (float)blockheight;
#endif
	}

	//if (!grPatch->mipmap->downloaded)
		HWD.pfnSetTexture(grPatch->mipmap);
	//HWR_SetCurrentTexture(grPatch->mipmap);

	Z_ChangeTag(grPatch->mipmap->data, PU_HWRCACHE_UNLOCKED);

	//CONS_Debug(DBG_RENDER, "picloaded at %x as texture %d\n", grPatch->mipmap->data, grPatch->mipmap->downloaded);
}

#else

static void HWR_MakePic(const size_t len, pic_t *pic, const patch_t *patch, GLPatch_t *grpatch, GLMipmap_t *grMipmap, boolean makebitmap)
{
	//if (!grMipmap->downloaded && !grMipmap->data)
	{
		//pic_t *pic = W_CacheLumpNum(lumpnum, PU_CACHE);
		UINT8 *block;
		//size_t len = W_LumpLength(lumpnum) - sizeof (pic_t);

		grMipmap->width = (UINT16)patch->width; // grpatch
		grMipmap->height = (UINT16)patch->height; // grpatch

		if (pic->mode == PALETTE)
			grMipmap->format = textureformat; // can be set by driver
		else
			grMipmap->format = picmode2GR[pic->mode];

		Z_Free(grMipmap->data);
		//grMipmap->data = NULL;

#if 1
		grMipmap->flags = 0;
		grpatch->max_s = (float)patch->width / (float)grMipmap->width;
		grpatch->max_t = (float)patch->height / (float)grMipmap->height;
#endif

		if (makebitmap == false)
		{
			return;
		}

		// allocate block
		block = MakeBlock(grMipmap);

		if (patch->width == SHORT(pic->width) && patch->height == SHORT(pic->height) &&
			format2bpp(grMipmap->format) == format2bpp(picmode2GR[pic->mode]))
		{
			// no conversion needed
			//grMipmap->data = pic->data;
			M_Memcpy(grMipmap->data, pic->data, len);
		}
		else
		{
			HWR_DrawPicInCache(block, SHORT(pic->width), SHORT(pic->height),
			//HWR_DrawPicInCache(grMipmap->data, SHORT(pic->width), SHORT(pic->height),
			                   SHORT(pic->width)*format2bpp(grMipmap->format),
			                   pic,
			                   format2bpp(grMipmap->format));
		}

#if 0
#if 0
		Z_ChangeTag(pic, PU_CACHE);
		Z_Unlock(pic);
#endif
#if 1
		Z_Unlock(pic);
		Z_ChangeTag(pic, PU_HWRCACHE_UNLOCKED);
		Z_ChangeTag(block, PU_HWRCACHE_UNLOCKED);
#endif
#if 0
		Z_ChangeTag(pic, PU_CACHE);
        Z_ChangeTag(block, PU_HWRCACHE);
#endif
#if 0
		Z_Free(pic);
        Z_Free(block);
#endif
#else
		Z_Unlock(pic);
		Z_ChangeTag(block, PU_HWRCACHE_UNLOCKED);
#endif

		//Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);

#if 0
		grMipmap->flags = 0;
		grpatch->max_s = (float)patch->width / (float)grMipmap->width;
		grpatch->max_t = (float)patch->height / (float)grMipmap->height;
#endif
	}

#if 0
	// The system-memory data can be purged now.
	Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);
#endif
}

GLPatch_t *HWR_GetPic(lumpnum_t lumpnum, const UINT8 *colormap)
{
	if (lumpnum == LUMPERROR)
		return NULL;

	patch_t *patch = HWR_GetCachedGLPatch(lumpnum);
	GLPatch_t *grpatch = (GLPatch_t *)Patch_AllocateHardwarePatch(patch);

	pic_t *pic = W_CacheLumpNum(lumpnum, PU_CACHE);
	size_t len = (W_LumpLength(lumpnum) - sizeof(pic_t));

	patch->width = SHORT(pic->width); // grpatch
	patch->height = SHORT(pic->height); // grpatch
	patch->leftoffset = 0; // grpatch
	patch->topoffset = 0; // grpatch

#if 1
	// search for the mipmap
	// skip the first (no colormap translated)
	if (!(colormap == colormaps || colormap == NULL))
	{
		GLMipmap_t *grMipmap, *newMipmap;

		for (grMipmap = grpatch->mipmap; grMipmap->nextcolormap; )
		{
			grMipmap = grMipmap->nextcolormap;
			if (grMipmap->colormap && grMipmap->colormap->source == colormap)
			{
				if (memcmp(grMipmap->colormap->data, colormap, 256 * sizeof(UINT8)))
				{
					M_Memcpy(grMipmap->colormap->data, colormap, 256 * sizeof(UINT8));

					// If hardware does not have the texture, then call pfnSetTexture to upload it
					// If it does have the texture, then call pfnUpdateTexture to update it
					GLPatch_t *gr_Patch = patch->hardware;
					HWR_MakePic(len, pic, patch, gr_Patch, grMipmap, true);
					if (!grMipmap->downloaded)
						HWD.pfnSetTexture(grMipmap);
					else
						HWD.pfnUpdateTexture(grMipmap);
					HWR_SetCurrentTexture(grMipmap);

					// The system-memory data can be purged now.
					Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);
					//Z_Free(grMipmap->data);
				}
				else
				{
					GLPatch_t *gr_Patch = patch->hardware;
					if (!grMipmap->downloaded && !grMipmap->data)
						HWR_MakePic(len, pic, patch, gr_Patch, grMipmap, true);
					if (!grMipmap->downloaded)
						HWD.pfnSetTexture(grMipmap);
					HWR_SetCurrentTexture(grMipmap);

					// The system-memory data can be purged now.
					Z_ChangeTag(grMipmap->data, PU_HWRCACHE_UNLOCKED);
					//Z_Free(grMipmap->data);
				}
			}
		}

		newMipmap = calloc(1, sizeof (*newMipmap));
		if (newMipmap == NULL)
			I_Error("%s: Out of memory", "HWR_GetPic");
		grMipmap->nextcolormap = newMipmap;

		newMipmap->colormap = Z_Calloc(sizeof(*newMipmap->colormap), PU_HWRPATCHCOLMIPMAP, NULL);
		newMipmap->colormap->source = colormap;
		M_Memcpy(newMipmap->colormap->data, colormap, 256 * sizeof(UINT8));

		GLPatch_t *gr_Patch = patch->hardware;
		if (!newMipmap->downloaded && !newMipmap->data)
			HWR_MakePic(len, pic, patch, gr_Patch, newMipmap, true);
		if (!newMipmap->downloaded)
			HWD.pfnSetTexture(newMipmap);
		HWR_SetCurrentTexture(newMipmap);

		//CONS_Alert(CONS_NOTICE, "picloaded at %p as texture %d\n", newMipmap->data, newMipmap->downloaded);
		CONS_Debug(DBG_RENDER, "COLORMAP: picloaded at %p as texture %d\n", newMipmap->data, newMipmap->downloaded);

		// The system-memory data can be purged now.
		Z_ChangeTag(newMipmap->data, PU_HWRCACHE_UNLOCKED);
		//Z_Free(newMipmap->data);

		//Z_ChangeTag(grpatch->mipmap->data, PU_HWRCACHE_UNLOCKED);
		//Z_Free(grpatch->mipmap->data);
		return grpatch;
	}
#else
	(void)colormap;
#endif

#if 1
	GLPatch_t *gr_Patch = patch->hardware;
	if (!grpatch->mipmap->downloaded && !grpatch->mipmap->data)
		HWR_MakePic(len, pic, patch, gr_Patch, grpatch->mipmap, true);
	if (!grpatch->mipmap->downloaded)
		HWD.pfnSetTexture(grpatch->mipmap);
	HWR_SetCurrentTexture(grpatch->mipmap); // DEFAULT: OFF
#endif

	//CONS_Alert(CONS_NOTICE, "picloaded at %p as texture %d\n", grpatch->mipmap->data, grpatch->mipmap->downloaded);
	CONS_Debug(DBG_RENDER, "picloaded at %p as texture %d\n", grpatch->mipmap->data, grpatch->mipmap->downloaded);

	// DEFAULT: ON
	Z_ChangeTag(grpatch->mipmap->data, PU_HWRCACHE_UNLOCKED);
	//Z_Free(grpatch->mipmap->data);
	return gr_Patch;
}
#endif

#else

#define NEW_DRAWPATCH

#ifdef NEW_DRAWPATCH
static void HWR_NEW_DrawPatchInCache(UINT8 *block, INT32 pblockwidth, INT32 pblockheight, INT32 blockmodulo, patch_t *patch, INT32 bpp)
{
	INT32 i, j;
	fixed_t posx, posy, stepx, stepy;
	UINT8 *dest, *src, texel;
	UINT16 texelu16;
	RGBA_t col;
	RGBA_t *palette = HWR_GetTexturePalette();
	GLPatch_t *glPatch = patch->hardware;

	if (!glPatch || !glPatch->mipmap)
		return;

	stepy = ((INT32)patch->height << FRACBITS) / pblockheight;
	stepx = ((INT32)patch->width << FRACBITS) / pblockwidth;

	const INT32 srcbpp = format2bpp(glPatch->mipmap->format);
	posy = 0;

	for (j = 0; j < pblockheight; j++)
	{
		posx = 0;
		dest = block + (j * blockmodulo);
#if 0
		//UINT8 *data = glPatch->mipmap->data;
		UINT8 *data = W_CacheLumpNum(lumpnum, PU_CACHE);
		if (data == NULL)
			return;
		src = &data[(posy >> FRACBITS) * patch->width * srcbpp];
#else
		src = &block[(posy >> FRACBITS) * patch->width * srcbpp];
#endif

		for (i = 0; i < pblockwidth; i++)
		{
			switch (glPatch->mipmap->format)
			{
			case GL_TEXFMT_P_8:
				texel = src[(posx + FRACUNIT / 2) >> FRACBITS];
				switch (bpp)
				{
				case 1: *dest++ = texel; break;
				case 2: texelu16 = (UINT16)(texel | 0xff00); memcpy(dest, &texelu16, 2); dest += 2; break;
				case 3: col = palette[texel]; memcpy(dest, &col, 3); dest += 3; break;
				case 4: memcpy(dest, &palette[texel], 4); dest += 4; break;
				}
				break;

			case GL_TEXFMT_ALPHA_INTENSITY_88:
				memcpy(dest, src + ((posx + FRACUNIT / 2) >> FRACBITS) * 2, 2);
				dest += 2;
				break;

			case GL_TEXFMT_RGBA:
				memcpy(dest, src + ((posx + FRACUNIT / 2) >> FRACBITS) * 4, 4);
				dest += 4;
				break;

			default:
				break;
			}
			posx += stepx;
		}
		posy += stepy;
	}
}
#endif

void HWR_GetPic(lumpnum_t lumpnum, const UINT8 *colormap)
{
	patch_t *patch = NULL;
	GLPatch_t *grPatch = NULL;

	(void)colormap;

	if (lumpnum == LUMPERROR)
		return;

	patch = HWR_GetCachedGLPatch(lumpnum);
	if (patch == NULL)
		return;

#if 1
	//CONS_Printf("isdoompatch is %d\n", Picture_CheckIfDoomPatch(patch));

#if 1
	#if 0
		#define WIDTH SHORT(patch->width)
		#define HEIGHT SHORT(patch->height)
	#else
		#define WIDTH patch->width
		#define HEIGHT patch->height
	#endif
	#define TOPOFFSET patch->topoffset
	#define LEFTOFFSET patch->leftoffset
#else
	#define WIDTH 0
	#define HEIGHT 0
	#define TOPOFFSET 0
	#define LEFTOFFSET 0
#endif
	INT32 inflags = PICFMT_FLAT32; // PICFMT_DOOMPATCH // PICFMT_FLAT32 // PICFMT_DOOMPATCH32
	INT32 outflags = PICFMT_PATCH; // PICFMT_PATCH // PICFMT_FLAT32
	INT32 flags = PICFLAGS_USE_TRANSPARENTPIXEL; // PICFLAGS_USE_TRANSPARENTPIXEL

	void *realpatch = Picture_PatchConvert(inflags, 0, patch, outflags, NULL, WIDTH, HEIGHT, TOPOFFSET, LEFTOFFSET, flags);
	if (realpatch)
	{
		CONS_Printf("patch exists\n");
#if 0
		Z_SetUser(realpatch, (void **)&patch);
#else
		patch = realpatch;
#endif
		Z_ChangeTag(realpatch, PU_HWRCACHE_UNLOCKED);
		//Z_Free(realpatch);
	}
	else
		CONS_Printf("patch no exists\n");
#endif
	grPatch = (GLPatch_t *)Patch_AllocateHardwarePatch(patch);
	//HWR_MakePatch(patch, grPatch, grPatch->mipmap, true);

	if (!grPatch->mipmap->downloaded && !grPatch->mipmap->data)
	{
		UINT8 *block = NULL;
		size_t lumpsize = W_LumpLength(lumpnum);
		size_t len;

//#define COPY_INTO_NEW_PATCH
#ifdef COPY_INTO_NEW_PATCH
		len = (lumpsize - sizeof(patch_t));
		patch_t *pic = HWR_GetCachedGLPatch(lumpnum);
		//patch_t *pic = W_CacheLumpNum(lumpnum, PU_CACHE);
		GLPatch_t *glPic = (GLPatch_t *)Patch_AllocateHardwarePatch(pic);
		//HWR_MakePatch(pic, glPic, glPic->mipmap, true);
#endif

#if 1
		len = (lumpsize - sizeof(W_CacheLumpNum(lumpnum, PU_CACHE)));
#endif

#if 0
		// allocate block
#if 0
		W_ReadLump(lumpnum, Z_Malloc(lumpsize, PU_HWRCACHE, &grPatch->mipmap->data));
#else
		W_ReadLump(lumpnum, Z_Malloc(len, PU_HWRCACHE, &grPatch->mipmap->data));
#endif
#endif

#ifdef COPY_INTO_NEW_PATCH
#if 1
		patch->width = pic->width;
		patch->height = pic->height;
#else
		patch->width = SHORT(pic->width);
		patch->height = SHORT(pic->height);
#endif

#else

#if 1
		patch->width = SHORT(patch->width);
		patch->height = SHORT(patch->height);
#endif

#endif
		patch->leftoffset = 0;
		patch->topoffset = 0;

#if 0
		Z_Free(grPatch->mipmap->data);
#endif

#if 0
		// allocate block
#if 0
		W_ReadLump(lumpnum, Z_Malloc(lumpsize, PU_HWRCACHE, &grPatch->mipmap->data));
#else
		W_ReadLump(lumpnum, Z_Malloc(len, PU_HWRCACHE, &grPatch->mipmap->data));
#endif
#endif

		grPatch->mipmap->width = patch->width;
		grPatch->mipmap->height = patch->height;

#if 1
		//grPatch->mipmap->format = patchformat;
		//grPatch->mipmap->format = textureformat;
		grPatch->mipmap->format = GL_TEXFMT_ALPHA_INTENSITY_88;
		//grPatch->mipmap->format = GL_TEXFMT_ALPHA_8;
		//grPatch->mipmap->format = GL_TEXFMT_P_8
#endif

		grPatch->mipmap->flags = 0;
		grPatch->max_s = grPatch->max_t = 1.0f;
		CONS_Printf("format is %d\n", grPatch->mipmap->format);

		// Optionally convert patch data
//#define ALLOW_FORMAT_CHECKING

#ifndef COPY_INTO_NEW_PATCH
#ifdef ALLOW_FORMAT_CHECKING
		if (grPatch->mipmap->format != (GLTextureFormat_t)patchformat)
#endif
#else
#ifdef ALLOW_FORMAT_CHECKING
		if (grPatch->mipmap->format != glPic->mipmap->format)
#endif
#endif
		{
			block = MakeBlock(grPatch->mipmap);
#ifdef NEW_DRAWPATCH
			HWR_NEW_DrawPatchInCache(block,
				grPatch->mipmap->width,
				grPatch->mipmap->height,
				grPatch->mipmap->width * format2bpp(grPatch->mipmap->format),
				patch,
				format2bpp(grPatch->mipmap->format));
#else
			HWR_DrawPatchInCache(grPatch->mipmap,
				grPatch->mipmap->width, grPatch->mipmap->height,
				patch->width, patch->height,
				patch);
#endif
			Z_ChangeTag(block, PU_HWRCACHE_UNLOCKED);
		}
#if defined(COPY_INTO_NEW_PATCH) && defined(ALLOW_FORMAT_CHECKING)
		else
			grPatch->mipmap->data = glPic->mipmap->data;
#endif


#if 1
		grPatch->mipmap->flags = 0;
		grPatch->max_s = grPatch->max_t = 1.0f;
#endif

#ifdef COPY_INTO_NEW_PATCH
		Z_Unlock(pic);
		Z_Unlock(glPic);
#endif
	}

#if 1
	if (!grPatch->mipmap->downloaded)
		HWD.pfnSetTexture(grPatch->mipmap);
	HWR_SetCurrentTexture(grPatch->mipmap);
#else
	HWD.pfnSetTexture(grPatch->mipmap);
	//HWR_SetCurrentTexture(grPatch->mipmap);
#endif
	Z_ChangeTag(grPatch->mipmap->data, PU_HWRCACHE_UNLOCKED);
}
#endif
#endif

patch_t *HWR_GetCachedGLPatchPwad(UINT16 wadnum, UINT16 lumpnum)
{
	lumpcache_t *lumpcache = wadfiles[wadnum]->patchcache;
	if (!lumpcache[lumpnum])
	{
		void *ptr = Patch_Create(0, 0);
		Z_SetUser(ptr, &lumpcache[lumpnum]);
		Patch_AllocateHardwarePatch(ptr);
	}
	return (patch_t *)(lumpcache[lumpnum]);
}

patch_t *HWR_GetCachedGLPatch(lumpnum_t lumpnum)
{
	return HWR_GetCachedGLPatchPwad(WADFILENUM(lumpnum),LUMPNUM(lumpnum));
}

// Need to do this because they aren't powers of 2
static void HWR_DrawFadeMaskInCache(GLMipmap_t *mipmap, INT32 pblockwidth, INT32 pblockheight,
	lumpnum_t fademasklumpnum, UINT16 fmwidth, UINT16 fmheight)
{
	INT32 i,j;
	fixed_t posx, posy, stepx, stepy;
	UINT8 *block = mipmap->data; // places the data directly into here
	UINT8 *flat;
	UINT8 *dest, *src, texel;
	RGBA_t col;
	RGBA_t *palette = HWR_GetTexturePalette();

	// Place the flats data into flat
	W_ReadLump(fademasklumpnum, Z_Malloc(W_LumpLength(fademasklumpnum),
		PU_HWRCACHE, &flat));

	stepy = ((INT32)fmheight<<FRACBITS)/pblockheight;
	stepx = ((INT32)fmwidth<<FRACBITS)/pblockwidth;
	posy = 0;
	for (j = 0; j < pblockheight; j++)
	{
		posx = 0;
		dest = &block[j*(mipmap->width)]; // 1bpp
		src = &flat[(posy>>FRACBITS)*SHORT(fmwidth)];
		for (i = 0; i < pblockwidth;i++)
		{
			// fademask bpp is always 1, and is used just for alpha
			texel = src[(posx)>>FRACBITS];
			col = palette[texel];
			*dest = col.s.red; // take the red level of the colour and use it for alpha, as fademasks do

			dest++;
			posx += stepx;
		}
		posy += stepy;
	}

	Z_Free(flat);
}

static void HWR_CacheFadeMask(GLMipmap_t *grMipmap, lumpnum_t fademasklumpnum)
{
	size_t size;
	UINT16 fmheight = 0, fmwidth = 0;

	// setup the texture info
	grMipmap->format = GL_TEXFMT_ALPHA_8; // put the correct alpha levels straight in so I don't need to convert it later
	grMipmap->flags = 0;

	size = W_LumpLength(fademasklumpnum);

	switch (size)
	{
		// None of these are powers of 2, so I'll need to do what is done for textures and make them powers of 2 before they can be used
		case 256000: // 640x400
			fmwidth = 640;
			fmheight = 400;
			break;
		case 64000: // 320x200
			fmwidth = 320;
			fmheight = 200;
			break;
		case 16000: // 160x100
			fmwidth = 160;
			fmheight = 100;
			break;
		case 4000: // 80x50 (minimum)
			fmwidth = 80;
			fmheight = 50;
			break;
		default: // Bad lump
			CONS_Alert(CONS_WARNING, "Fade mask lump of incorrect size, ignored\n"); // I should avoid this by checking the lumpnum in HWR_RunWipe
			break;
	}

	// Thankfully, this will still work for this scenario
	grMipmap->width  = fmwidth;
	grMipmap->height = fmheight;

	MakeBlock(grMipmap);

	HWR_DrawFadeMaskInCache(grMipmap, fmwidth, fmheight, fademasklumpnum, fmwidth, fmheight);

	// I DO need to convert this because it isn't power of 2 and we need the alpha
}


void HWR_GetFadeMask(lumpnum_t fademasklumpnum)
{
	patch_t *patch = HWR_GetCachedGLPatch(fademasklumpnum);
	GLMipmap_t *grmip = ((GLPatch_t *)Patch_AllocateHardwarePatch(patch))->mipmap;
	if (!grmip->downloaded && !grmip->data)
		HWR_CacheFadeMask(grmip, fademasklumpnum);

	HWD.pfnSetTexture(grmip);

	// The system-memory data can be purged now.
	Z_ChangeTag(grmip->data, PU_HWRCACHE_UNLOCKED);
}

// =================================================
//             PALETTE HANDLING
// =================================================

void HWR_SetPalette(RGBA_t *palette)
{
	if (HWR_ShouldUsePaletteRendering())
	{
		// set the palette for palette postprocessing

		if (cv_glpalettedepth.value == 16)
		{
			// crush to 16-bit rgb565, like software currently does in the standard configuration
			// Note: Software's screenshots have the 24-bit palette, but the screen gets
			// the 16-bit version! For making comparison screenshots either use an external screenshot
			// tool or set the palette depth to 24 bits.
			RGBA_t crushed_palette[256];
			int i;
			for (i = 0; i < 256; i++)
			{
				float fred = (float)(palette[i].s.red >> 3);
				float fgreen = (float)(palette[i].s.green >> 2);
				float fblue = (float)(palette[i].s.blue >> 3);
				crushed_palette[i].s.red = (UINT8)(fred / 31.0f * 255.0f);
				crushed_palette[i].s.green = (UINT8)(fgreen / 63.0f * 255.0f);
				crushed_palette[i].s.blue = (UINT8)(fblue / 31.0f * 255.0f);
				crushed_palette[i].s.alpha = 255;
			}
			HWD.pfnSetScreenPalette(crushed_palette);
		}
		else
		{
			HWD.pfnSetScreenPalette(palette);
		}

		// this part is responsible for keeping track of the palette OUTSIDE of a level.
		if (!(gamestate == GS_LEVEL || (gamestate == GS_TITLESCREEN && titlemapinaction)))
			HWR_SetMapPalette();
	}
	else
	{
		// set the palette for the textures
		HWD.pfnSetTexturePalette(palette);
		// reset mapPalette so next call to HWR_SetMapPalette will update everything correctly
		memset(mapPalette, 0, sizeof(mapPalette));
		// hardware driver will flush there own cache if cache is non paletized
		// now flush data texture cache so 32 bit texture are recomputed
		if (patchformat == GL_TEXFMT_RGBA || textureformat == GL_TEXFMT_RGBA)
		{
			Z_FreeTag(PU_HWRCACHE);
			Z_FreeTag(PU_HWRCACHE_UNLOCKED);
		}
	}
}

static void HWR_SetPaletteLookup(RGBA_t *palette)
{
	int r, g, b;
	UINT8 *lut = Z_Malloc(
		HWR_PALETTE_LUT_SIZE*HWR_PALETTE_LUT_SIZE*HWR_PALETTE_LUT_SIZE*sizeof(UINT8),
		PU_STATIC, NULL);
#define STEP_SIZE (256/HWR_PALETTE_LUT_SIZE)
	for (b = 0; b < HWR_PALETTE_LUT_SIZE; b++)
	{
		for (g = 0; g < HWR_PALETTE_LUT_SIZE; g++)
		{
			for (r = 0; r < HWR_PALETTE_LUT_SIZE; r++)
			{
				lut[b*HWR_PALETTE_LUT_SIZE*HWR_PALETTE_LUT_SIZE+g*HWR_PALETTE_LUT_SIZE+r] =
					NearestPaletteColor(r*STEP_SIZE, g*STEP_SIZE, b*STEP_SIZE, palette);
			}
		}
	}
#undef STEP_SIZE
	HWD.pfnSetPaletteLookup(lut);
	Z_Free(lut);
}

// Updates mapPalette to reflect the loaded level or other game state.
// Textures are flushed if needed.
// Call this function only in palette rendering mode.
void HWR_SetMapPalette(void)
{
	RGBA_t RGBA_converted[256];
	RGBA_t *palette;
	int i;

	if (!(gamestate == GS_LEVEL || (gamestate == GS_TITLESCREEN && titlemapinaction)))
	{
		// outside of a level, pMasterPalette should have PLAYPAL ready for us
		palette = pMasterPalette;
	}
	else
	{
		// in a level pMasterPalette might have a flash palette, but we
		// want the map's original palette.
		lumpnum_t lumpnum = W_GetNumForName(GetPalette());
		size_t palsize = W_LumpLength(lumpnum);
		UINT8 *RGB_data;
		if (palsize < 768) // 256 * 3
			I_Error("HWR_SetMapPalette: A programmer assumed palette lumps are at least 768 bytes long, but apparently this was a wrong assumption!\n");
		RGB_data = W_CacheLumpNum(lumpnum, PU_CACHE);
		// we got the RGB palette now, but we need it in RGBA format.
		for (i = 0; i < 256; i++)
		{
			RGBA_converted[i].s.red = *(RGB_data++);
			RGBA_converted[i].s.green = *(RGB_data++);
			RGBA_converted[i].s.blue = *(RGB_data++);
			RGBA_converted[i].s.alpha = 255;
		}
		palette = RGBA_converted;
	}

	// check if the palette has changed from the previous one
	if (memcmp(mapPalette, palette, sizeof(mapPalette)))
	{
		memcpy(mapPalette, palette, sizeof(mapPalette));
		// in palette rendering mode, this means that all rgba textures now have wrong colors
		// and the lookup table is outdated
		HWR_SetPaletteLookup(mapPalette);
		HWD.pfnSetTexturePalette(mapPalette);
		if (patchformat == GL_TEXFMT_RGBA || textureformat == GL_TEXFMT_RGBA)
		{
			Z_FreeTag(PU_HWRCACHE);
			Z_FreeTag(PU_HWRCACHE_UNLOCKED);
		}
	}
}

// Creates a hardware lighttable from the supplied lighttable.
// Returns the id of the hw lighttable, usable in FSurfaceInfo.
UINT32 HWR_CreateLightTable(UINT8 *lighttable, RGBA_t *hw_lighttable)
{
	UINT32 i;
	RGBA_t *palette = HWR_GetTexturePalette();

	// To make the palette index -> RGBA mapping easier for the shader,
	// the hardware lighttable is composed of RGBA colors instead of palette indices.
	for (i = 0; i < 256 * 32; i++)
		hw_lighttable[i] = palette[lighttable[i]];

	return HWD.pfnCreateLightTable(hw_lighttable);
}

// Updates a hardware lighttable of a given id from the supplied lighttable.
void HWR_UpdateLightTable(UINT32 id, UINT8 *lighttable, RGBA_t *hw_lighttable)
{
	UINT32 i;
	RGBA_t *palette = HWR_GetTexturePalette();

	for (i = 0; i < 256 * 32; i++)
		hw_lighttable[i] = palette[lighttable[i]];

	HWD.pfnUpdateLightTable(id, hw_lighttable);
}

// get hwr lighttable id for colormap, create it if it doesn't already exist
UINT32 HWR_GetLightTableID(extracolormap_t *colormap)
{
	boolean default_colormap = false;
	if (!colormap)
	{
		colormap = R_GetDefaultColormap(); // a place to store the hw lighttable id
		// alternatively could just store the id in a global variable if there are issues
		default_colormap = true;
	}

	UINT8 *colormap_pointer;

	if (default_colormap)
		colormap_pointer = colormaps; // don't actually use the data from the "default colormap"
	else
		colormap_pointer = colormap->colormap;

	// create hw lighttable if there isn't one
	if (colormap->gl_lighttable.data == NULL)
	{
		Z_Malloc(256 * 32 * sizeof(RGBA_t), PU_HWRLIGHTTABLEDATA, &colormap->gl_lighttable.data);
	}

	// Generate the texture for this light table
	if (!colormap->gl_lighttable.id)
	{
		colormap->gl_lighttable.id = HWR_CreateLightTable(colormap_pointer, colormap->gl_lighttable.data);
	}
	// Update the texture if it was directly changed by a script
	else if (colormap->gl_lighttable.needs_update)
	{
		HWR_UpdateLightTable(colormap->gl_lighttable.id, colormap_pointer, colormap->gl_lighttable.data);
	}

	colormap->gl_lighttable.needs_update = false;

	return colormap->gl_lighttable.id;
}

// Note: all hardware lighttable ids assigned before this
// call become invalid and must not be used.
void HWR_ClearLightTables(void)
{
	Z_FreeTag(PU_HWRLIGHTTABLEDATA);

	if (vid.glstate == VID_GL_LIBRARY_LOADED)
		HWD.pfnClearLightTables();
}

#endif //HWRENDER
