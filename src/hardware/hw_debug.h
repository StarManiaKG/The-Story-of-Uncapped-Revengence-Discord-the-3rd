
#ifndef __HW_DEBUG__
#define __HW_DEBUG__

//#pragma once

#ifdef _cplusplus
extern "C" {
#endif

// exists as a forwarder of sorts so I can use ImGui inside of SRB2
#include "SDL.h"
#include "SDL_opengl.h"

#include "../doomdef.h"

//#define IMGUI_PROCEED

void HWRD_InitImgui(SDL_GLContext context, SDL_Window *window);
void HWRD_StopImgui();
void HWRD_ImGuiRenderS();
void HWRD_ImGuiRender();
void HWRD_ImguiTest();
boolean HWRD_ImGuiInput(SDL_Event *event);

#ifdef _cplusplus
} // extern "C"
#endif

#endif // __HW_DEBUG__
