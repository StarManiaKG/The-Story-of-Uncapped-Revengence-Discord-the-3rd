// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2021 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file hw_shaders.h
/// \brief Handles the shaders used by the game.

#ifndef _HW_SHADERS_H_
#define _HW_SHADERS_H_

#include "../doomtype.h"

#define GLSL_DOOM_COLORMAP \
	"float R_DoomColormap(float light, float z)\n" \
	"{\n" \
		"float lightnum = clamp(light / 17.0, 0.0, 15.0);\n" \
		"float lightz = clamp(z / 16.0, 0.0, 127.0);\n" \
		"float startmap = (15.0 - lightnum) * 4.0;\n" \
		"float scale = 160.0 / (lightz + 1.0);\n" \
		"return startmap - scale * 0.5;\n" \
	"}\n"

#define GLSL_DOOM_LIGHT_EQUATION \
	"float R_DoomLightingEquation(float light)\n" \
	"{\n" \
		"float z = gl_FragCoord.z / gl_FragCoord.w;\n" \
		"float colormap = floor(R_DoomColormap(light, z)) + 0.5;\n" \
		"return clamp(colormap, 0.0, 31.0) / 32.0;\n" \
	"}\n"

#define GLSL_SOFTWARE_TINT_EQUATION \
	"if (mix(tint_color.a, 0.0, brightmap_mix) > 0.0) {\n" \
		"float color_bright = sqrt((base_color.r * base_color.r) + (base_color.g * base_color.g) + (base_color.b * base_color.b));\n" \
		"float strength = sqrt(9.0 * mix(tint_color.a, 0.0, brightmap_mix));\n" \
		"final_color.r = clamp((color_bright * (tint_color.r * strength)) + (base_color.r * (1.0 - strength)), 0.0, 1.0);\n" \
		"final_color.g = clamp((color_bright * (tint_color.g * strength)) + (base_color.g * (1.0 - strength)), 0.0, 1.0);\n" \
		"final_color.b = clamp((color_bright * (tint_color.b * strength)) + (base_color.b * (1.0 - strength)), 0.0, 1.0);\n" \
	"}\n"

#define GLSL_SOFTWARE_FADE_EQUATION \
	"float darkness = R_DoomLightingEquation(final_lighting);\n" \
	"if (fade_start > 0.0 || fade_end < 31.0) {\n" \
		"float fs = fade_start / 31.0;\n" \
		"float fe = fade_end / 31.0;\n" \
		"float fd = fe - fs;\n" \
		"darkness = clamp((darkness - fs) * (1.0 / fd), 0.0, 1.0);\n" \
	"}\n" \
	"float colorBrightness = sqrt((final_color.r * final_color.r) + (final_color.g * final_color.g) + (final_color.b * final_color.b));\n" \
	"float fogBrightness = sqrt((fade_color.r * fade_color.r) + (fade_color.g * fade_color.g) + (fade_color.b * fade_color.b));\n" \
	"float colorIntensity = 0.0;\n" \
	"if (colorBrightness < fogBrightness) {\n" \
		"colorIntensity = 1.0 - min(final_color.r, min(final_color.g, final_color.b));\n" \
		"colorIntensity = abs(colorIntensity - (1.0 - max(fade_color.r, max(fade_color.g, fade_color.b))));\n" \
	"} else {\n" \
		"colorIntensity = max(final_color.r, max(final_color.g, final_color.b));\n" \
		"colorIntensity = abs(colorIntensity - min(fade_color.r, min(fade_color.g, fade_color.b)));\n" \
	"}\n" \
	"colorIntensity *= darkness;\n" \
	"colorIntensity *= fade_color.a * 10.0;\n" \
	"if (abs(final_color.r - fade_color.r) <= colorIntensity) {\n" \
		"final_color.r = fade_color.r;\n" \
	"} else if (final_color.r < fade_color.r) {\n" \
		"final_color.r += colorIntensity;\n" \
	"} else {\n" \
		"final_color.r -= colorIntensity;\n" \
	"}\n" \
	"if (abs(final_color.g - fade_color.g) <= colorIntensity) {\n" \
		"final_color.g = fade_color.g;\n" \
	"} else if (final_color.g < fade_color.g) {\n" \
		"final_color.g += colorIntensity;\n" \
	"} else {\n" \
		"final_color.g -= colorIntensity;\n" \
	"}\n" \
	"if (abs(final_color.b - fade_color.b) <= colorIntensity) {\n" \
		"final_color.b = fade_color.b;\n" \
	"} else if (final_color.b < fade_color.b) {\n" \
		"final_color.b += colorIntensity;\n" \
	"} else {\n" \
		"final_color.b -= colorIntensity;\n" \
	"}\n"

// ================
//  Vertex shaders
// ================

//
// Generic vertex shader
//

#define GLSL_DEFAULT_VERTEX_SHADER \
	"void main()\n" \
	"{\n" \
		"gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n" \
		"gl_FrontColor = gl_Color;\n" \
		"gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;\n" \
		"gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n" \
	"}\0"

// reinterpretation of sprite lighting for models
// it's a combination of how it works for normal sprites & papersprites
#define GLSL_MODEL_LIGHTING_VERTEX_SHADER \
	"uniform float lighting;\n" \
	"uniform vec3 light_dir;\n" \
	"uniform float light_contrast;\n" \
	"uniform float light_backlight;\n" \
	"void main()\n" \
	"{\n" \
		"gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n" \
		"float light = lighting;\n" \
		"if (length(light_dir) > 0.000001) {\n" \
			"mat4 m4 = gl_ProjectionMatrix * gl_ModelViewMatrix;\n" \
			"mat3 m3 = mat3( m4[0].xyz, m4[1].xyz, m4[2].xyz );\n" \
			"float extralight = -dot(normalize(gl_Normal * m3), normalize(light_dir));\n" \
			"extralight *= light_contrast - light_backlight;\n" \
			"extralight *= lighting / 255.0;\n" \
			"light += extralight * 2.5;\n" \
		"}\n" \
		"light = clamp(light / 255.0, 0.0, 1.0);\n" \
		"gl_FrontColor = vec4(light, light, light, 1.0);\n" \
		"gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;\n" \
		"gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n" \
	"}\0"

// ==================
//  Fragment shaders
// ==================

//
// Generic fragment shader
//

#define GLSL_DEFAULT_FRAGMENT_SHADER \
	"uniform sampler2D tex;\n" \
	"uniform vec4 poly_color;\n" \
	"void main(void) {\n" \
		"gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * poly_color;\n" \
	"}\0"

//
// Software fragment shader
//

#define GLSL_SOFTWARE_FRAGMENT_SHADER \
	"uniform sampler2D tex;\n" \
	"uniform sampler2D brightmap;\n" \
	"uniform vec4 poly_color;\n" \
	"uniform vec4 tint_color;\n" \
	"uniform vec4 fade_color;\n" \
	"uniform float lighting;\n" \
	"uniform float fade_start;\n" \
	"uniform float fade_end;\n" \
	GLSL_DOOM_COLORMAP \
	GLSL_DOOM_LIGHT_EQUATION \
	"void main(void) {\n" \
		"vec4 texel = texture2D(tex, gl_TexCoord[0].st);\n" \
		"vec4 base_color = texel * poly_color;\n" \
		"vec4 final_color = base_color;\n" \
		"float brightmap_mix = floor(texture2D(brightmap, gl_TexCoord[0].st).r);\n" \
		"float light_gain = (255.0 - lighting) * brightmap_mix;\n" \
		"float final_lighting = lighting + light_gain;\n" \
		GLSL_SOFTWARE_TINT_EQUATION \
		GLSL_SOFTWARE_FADE_EQUATION \
		"final_color.a = texel.a * poly_color.a;\n" \
		"gl_FragColor = final_color;\n" \
	"}\0"

// same as above but multiplies results with the lighting value from the
// accompanying vertex shader (stored in gl_Color)
#define GLSL_SOFTWARE_MODEL_LIGHTING_FRAGMENT_SHADER \
	"uniform sampler2D tex;\n" \
	"uniform sampler2D brightmap;\n" \
	"uniform vec4 poly_color;\n" \
	"uniform vec4 tint_color;\n" \
	"uniform vec4 fade_color;\n" \
	"uniform float fade_start;\n" \
	"uniform float fade_end;\n" \
	GLSL_DOOM_COLORMAP \
	GLSL_DOOM_LIGHT_EQUATION \
	"void main(void) {\n" \
		"vec4 texel = texture2D(tex, gl_TexCoord[0].st);\n" \
		"vec4 base_color = texel * poly_color;\n" \
		"vec4 final_color = base_color;\n" \
		"float final_lighting = gl_Color.r * 255.0;\n" \
		"float brightmap_mix = floor(texture2D(brightmap, gl_TexCoord[0].st).r);\n" \
		"float light_gain = (255.0 - final_lighting) * brightmap_mix;\n" \
		"final_lighting += light_gain;\n" \
		GLSL_SOFTWARE_TINT_EQUATION \
		GLSL_SOFTWARE_FADE_EQUATION \
		"final_color.a = texel.a * poly_color.a;\n" \
		"gl_FragColor = final_color;\n" \
	"}\0"

//
// Water surface shader
//
// Mostly guesstimated, rather than the rest being built off Software science.
// Still needs to distort things underneath/around the water...
//

#define GLSL_WATER_FRAGMENT_SHADER \
	"uniform sampler2D tex;\n" \
	"uniform sampler2D brightmap;\n" \
	"uniform vec4 poly_color;\n" \
	"uniform vec4 tint_color;\n" \
	"uniform vec4 fade_color;\n" \
	"uniform float lighting;\n" \
	"uniform float fade_start;\n" \
	"uniform float fade_end;\n" \
	"uniform float leveltime;\n" \
	"const float freq = 0.025;\n" \
	"const float amp = 0.025;\n" \
	"const float speed = 2.0;\n" \
	"const float pi = 3.14159;\n" \
	GLSL_DOOM_COLORMAP \
	GLSL_DOOM_LIGHT_EQUATION \
	"void main(void) {\n" \
		"float z = (gl_FragCoord.z / gl_FragCoord.w) / 2.0;\n" \
		"float a = -pi * (z * freq) + (leveltime * speed);\n" \
		"float sdistort = sin(a) * amp;\n" \
		"float cdistort = cos(a) * amp;\n" \
		"vec4 texel = texture2D(tex, vec2(gl_TexCoord[0].s - sdistort, gl_TexCoord[0].t - cdistort));\n" \
		"vec4 base_color = texel * poly_color;\n" \
		"vec4 final_color = base_color;\n" \
		"float brightmap_mix = floor(texture2D(brightmap, gl_TexCoord[0].st).r);\n" \
		"float light_gain = (255.0 - lighting) * brightmap_mix;\n" \
		"float final_lighting = lighting + light_gain;\n" \
		GLSL_SOFTWARE_TINT_EQUATION \
		GLSL_SOFTWARE_FADE_EQUATION \
		"final_color.a = texel.a * poly_color.a;\n" \
		"gl_FragColor = final_color;\n" \
	"}\0"

//
// Fog block shader
//
// Alpha of the planes themselves are still slightly off -- see HWR_FogBlockAlpha
//

#define GLSL_FOG_FRAGMENT_SHADER \
	"uniform vec4 tint_color;\n" \
	"uniform vec4 fade_color;\n" \
	"uniform float lighting;\n" \
	"uniform float fade_start;\n" \
	"uniform float fade_end;\n" \
	GLSL_DOOM_COLORMAP \
	GLSL_DOOM_LIGHT_EQUATION \
	"void main(void) {\n" \
		"vec4 base_color = gl_Color;\n" \
		"vec4 final_color = base_color;\n" \
		"float brightmap_mix = 0.0;\n" \
		"float light_gain = 0.0;\n" \
		"float final_lighting = lighting + light_gain;\n" \
		GLSL_SOFTWARE_TINT_EQUATION \
		GLSL_SOFTWARE_FADE_EQUATION \
		"gl_FragColor = final_color;\n" \
	"}\0"

// Sprite clipping makes me want to McFucking Die

#define GLSL_SPRITECLIP_HACK_VERTEX_SHADER \
	"void main(void) {\n" \
		"gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;\n" \
		"gl_FrontColor = gl_Color;\n" \
		"gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;\n" \
		"gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n" \
		"gl_Position.z -= 80.0 / gl_Position.z;\n" \
	"}\0"

//
// Sky fragment shader
// Modulates poly_color with gl_Color
//
#define GLSL_SKY_FRAGMENT_SHADER \
	"uniform sampler2D tex;\n" \
	"uniform vec4 poly_color;\n" \
	"void main(void) {\n" \
		"gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color * poly_color;\n" \
	"}\0"

#endif
