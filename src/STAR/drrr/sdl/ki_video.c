// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2020 by Sonic Team Junior.
// Copyright (C) 2000 by DooM Legacy Team.
// Copyright (C) 1996 by id Software, Inc.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file
/// \brief SRB2 graphics stuff for SDL

#ifdef HAVE_SDL
#include "SDL.h"

#include "../ki_system.h"
#include "../kg_input.h"
#include "../../../command.h"
#include "../../../console.h"
#include "../../../d_main.h"
#include "../../../hu_stuff.h"
#include "../../../sdl/sdlmain.h"
#include "../../../i_system.h"
#include "../../../lua_hook.h"
#include "../../../m_argv.h"
#include "../../../s_sound.h"

static SDL_bool disable_mouse = SDL_FALSE;
#define USE_MOUSEINPUT (!disable_mouse && cv_usemouse.value && havefocus)
#define MOUSE_MENU false //(!disable_mouse && cv_usemouse.value && menuactive && !USE_FULLSCREEN)
#define MOUSEBUTTONS_MAX MOUSEBUTTONS

// Total mouse motion X/Y offsets
static      INT32        mousemovex = 0, mousemovey = 0;

static       SDL_bool    wrapmouseok = SDL_FALSE;

static SDL_bool      havefocus = SDL_TRUE;

static INT32 Impl_SDL_Scancode_To_Keycode(SDL_Scancode code)
{
	if (code >= SDL_SCANCODE_A && code <= SDL_SCANCODE_Z)
	{
		// get lowercase ASCII
		return code - SDL_SCANCODE_A + 'a';
	}
	if (code >= SDL_SCANCODE_1 && code <= SDL_SCANCODE_9)
	{
		return code - SDL_SCANCODE_1 + '1';
	}
	else if (code == SDL_SCANCODE_0)
	{
		return '0';
	}
	if (code >= SDL_SCANCODE_F1 && code <= SDL_SCANCODE_F10)
	{
		return KEY_F1 + (code - SDL_SCANCODE_F1);
	}
	switch (code)
	{
		// F11 and F12 are separated from the rest of the function keys
		case SDL_SCANCODE_F11: return KEY_F11;
		case SDL_SCANCODE_F12: return KEY_F12;

		case SDL_SCANCODE_KP_0: return KEY_KEYPAD0;
		case SDL_SCANCODE_KP_1: return KEY_KEYPAD1;
		case SDL_SCANCODE_KP_2: return KEY_KEYPAD2;
		case SDL_SCANCODE_KP_3: return KEY_KEYPAD3;
		case SDL_SCANCODE_KP_4: return KEY_KEYPAD4;
		case SDL_SCANCODE_KP_5: return KEY_KEYPAD5;
		case SDL_SCANCODE_KP_6: return KEY_KEYPAD6;
		case SDL_SCANCODE_KP_7: return KEY_KEYPAD7;
		case SDL_SCANCODE_KP_8: return KEY_KEYPAD8;
		case SDL_SCANCODE_KP_9: return KEY_KEYPAD9;

		case SDL_SCANCODE_RETURN:         return KEY_ENTER;
		case SDL_SCANCODE_ESCAPE:         return KEY_ESCAPE;
		case SDL_SCANCODE_BACKSPACE:      return KEY_BACKSPACE;
		case SDL_SCANCODE_TAB:            return KEY_TAB;
		case SDL_SCANCODE_SPACE:          return KEY_SPACE;
		case SDL_SCANCODE_MINUS:          return KEY_MINUS;
		case SDL_SCANCODE_EQUALS:         return KEY_EQUALS;
		case SDL_SCANCODE_LEFTBRACKET:    return '[';
		case SDL_SCANCODE_RIGHTBRACKET:   return ']';
		case SDL_SCANCODE_BACKSLASH:      return '\\';
		case SDL_SCANCODE_NONUSHASH:      return '#';
		case SDL_SCANCODE_SEMICOLON:      return ';';
		case SDL_SCANCODE_APOSTROPHE:     return '\'';
		case SDL_SCANCODE_GRAVE:          return '`';
		case SDL_SCANCODE_COMMA:          return ',';
		case SDL_SCANCODE_PERIOD:         return '.';
		case SDL_SCANCODE_SLASH:          return '/';
		case SDL_SCANCODE_CAPSLOCK:       return KEY_CAPSLOCK;
		case SDL_SCANCODE_PRINTSCREEN:    return 0; // undefined?
		case SDL_SCANCODE_SCROLLLOCK:     return KEY_SCROLLLOCK;
		case SDL_SCANCODE_PAUSE:          return KEY_PAUSE;
		case SDL_SCANCODE_INSERT:         return KEY_INS;
		case SDL_SCANCODE_HOME:           return KEY_HOME;
		case SDL_SCANCODE_PAGEUP:         return KEY_PGUP;
		case SDL_SCANCODE_DELETE:         return KEY_DEL;
		case SDL_SCANCODE_END:            return KEY_END;
		case SDL_SCANCODE_PAGEDOWN:       return KEY_PGDN;
		case SDL_SCANCODE_RIGHT:          return KEY_RIGHTARROW;
		case SDL_SCANCODE_LEFT:           return KEY_LEFTARROW;
		case SDL_SCANCODE_DOWN:           return KEY_DOWNARROW;
		case SDL_SCANCODE_UP:             return KEY_UPARROW;
		case SDL_SCANCODE_NUMLOCKCLEAR:   return KEY_NUMLOCK;
		case SDL_SCANCODE_KP_DIVIDE:      return KEY_KPADSLASH;
		case SDL_SCANCODE_KP_MULTIPLY:    return '*'; // undefined?
		case SDL_SCANCODE_KP_MINUS:       return KEY_MINUSPAD;
		case SDL_SCANCODE_KP_PLUS:        return KEY_PLUSPAD;
		case SDL_SCANCODE_KP_ENTER:       return KEY_ENTER;
		case SDL_SCANCODE_KP_PERIOD:      return KEY_KPADDEL;
		case SDL_SCANCODE_NONUSBACKSLASH: return '\\';

		case SDL_SCANCODE_LSHIFT: return KEY_LSHIFT;
		case SDL_SCANCODE_RSHIFT: return KEY_RSHIFT;
		case SDL_SCANCODE_LCTRL:  return KEY_LCTRL;
		case SDL_SCANCODE_RCTRL:  return KEY_RCTRL;
		case SDL_SCANCODE_LALT:   return KEY_LALT;
		case SDL_SCANCODE_RALT:   return KEY_RALT;
		case SDL_SCANCODE_LGUI:   return KEY_LEFTWIN;
		case SDL_SCANCODE_RGUI:   return KEY_RIGHTWIN;
		default:                  break;
	}
	return 0;
}

static boolean IgnoreMouse(void)
{
	consvar_t *cv_alwaysgrabmouse = CV_FindVar("alwaysgrabmouse");
	if (cv_alwaysgrabmouse->value)
		return false;
	if (menuactive)
		return false; // return !M_MouseNeeded();
	if (paused || con_destlines || chat_on)
		return true;
	if (gamestate != GS_LEVEL && gamestate != GS_INTERMISSION &&
			gamestate != GS_CONTINUING && gamestate != GS_CUTSCENE)
		return true;
	return false;
}

static void SDLdoGrabMouse(void)
{
	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetWindowGrab(window, SDL_TRUE);
	if (SDL_SetRelativeMouseMode(SDL_TRUE) == 0) // already warps mouse if successful
		wrapmouseok = SDL_TRUE; // TODO: is wrapmouseok or HalfWarpMouse needed anymore?
}

static void SDLdoUngrabMouse(void)
{
	SDL_ShowCursor(SDL_ENABLE);
	SDL_SetWindowGrab(window, SDL_FALSE);
	wrapmouseok = SDL_FALSE;
	SDL_SetRelativeMouseMode(SDL_FALSE);
}

static inline void SDLJoyRemap(event_t *event)
{
	(void)event;
}

static INT32 SDLJoyAxis(const Sint16 axis, UINT8 pid)
{
	// -32768 to 32767
	INT32 raxis = axis / 32;

#if 0
	if (Joystick[pid].bGamepadStyle)
	raxis = (abs(JoyInfo[pid].scale) > 1) ? ((raxis / JoyInfo[pid].scale) * JoyInfo[pid].scale) : raxis;
#else
	JoyType_t (*DRRR_Joystick) = (pid == 0 ? &Joystick : &Joystick2);
	SDLJoyInfo_t (*DRRR_JoyInfo) = (pid == 0 ? &JoyInfo : &JoyInfo2);
#endif

	if (DRRR_Joystick->bGamepadStyle)
	{
		// gamepad control type, on or off, live or die
		if (raxis < -(JOYAXISRANGE/2))
			raxis = -1;
		else if (raxis > (JOYAXISRANGE/2))
			raxis = 1;
		else
			raxis = 0;
	}
	else
	{
		raxis = (abs(DRRR_JoyInfo->scale) > 1) ? ((raxis / DRRR_JoyInfo->scale) * DRRR_JoyInfo->scale) : raxis;

#ifdef SDL_JDEADZONE
		if (-SDL_JDEADZONE <= raxis && raxis <= SDL_JDEADZONE)
			raxis = 0;
#endif
	}

	return raxis;
}

static void Impl_HandleWindowEvent(SDL_WindowEvent evt)
{
#define FOCUSUNION (unsigned int)(mousefocus | (kbfocus << 1))
	static SDL_bool firsttimeonmouse = SDL_TRUE;
	static SDL_bool mousefocus = SDL_TRUE;
	static SDL_bool kbfocus = SDL_TRUE;

	const unsigned int oldfocus = FOCUSUNION;

	switch (evt.event)
	{
		case SDL_WINDOWEVENT_ENTER:
			mousefocus = SDL_TRUE;
			break;
		case SDL_WINDOWEVENT_LEAVE:
			mousefocus = SDL_FALSE;
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			kbfocus = SDL_TRUE;
			mousefocus = SDL_TRUE;
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			kbfocus = SDL_FALSE;
			mousefocus = SDL_FALSE;
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			break;
#if 0
		case SDL_WINDOWEVENT_MOVED:
			window_x = evt.key;
			window_y = evt.x;
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			vid.realwidth = evt.key;
			vid.realheight = evt.x;
			break;
#endif
	}

	if (FOCUSUNION == oldfocus) // No state change
	{
		return;
	}

	if (mousefocus && kbfocus)
	{
		// Tell game we got focus back, resume music if necessary
		window_notinfocus = false;

		S_SetMusicVolume(cv_digmusicvolume.value, cv_midimusicvolume.value);

		if (!firsttimeonmouse)
		{
			if (cv_usemouse.value) I_StartupMouse();
		}
		//else firsttimeonmouse = SDL_FALSE;

		if (USE_MOUSEINPUT && !IgnoreMouse())
			SDLdoGrabMouse();
	}
	else if (!mousefocus && !kbfocus)
	{
		// Tell game we lost focus, pause music
		window_notinfocus = true;
		if (!cv_playmusicifunfocused.value)
			S_PauseAudio();
		if (!cv_playsoundsifunfocused.value)
			S_StopSounds();

#if 1
		// STAR NOTE: hack for now //
		disable_mouse = M_CheckParm("-nomouse");
#endif

		if (!disable_mouse)
		{
			SDLforceUngrabMouse();
		}
		G_ResetAllDeviceGameKeyDown();
		G_ResetAllDeviceResponding();

		if (MOUSE_MENU)
		{
			SDLdoUngrabMouse();
		}
	}
#undef FOCUSUNION
}

static void Impl_HandleKeyboardEvent(SDL_KeyboardEvent evt, Uint32 type)
{
	event_t event;

	event.device = 0;

	if (type == SDL_KEYUP)
	{
		event.type = ev_keyup;
	}
	else if (type == SDL_KEYDOWN)
	{
		event.type = ev_keydown;
	}
	else
	{
		return;
	}
	event.key = Impl_SDL_Scancode_To_Keycode(evt.keysym.scancode);
	event.x = evt.repeat;
	if (event.key) D_PostEvent(&event);
}

static void Impl_HandleMouseMotionEvent(SDL_MouseMotionEvent evt)
{
	static boolean firstmove = true;

	if (USE_MOUSEINPUT)
	{
		if ((SDL_GetMouseFocus() != window && SDL_GetKeyboardFocus() != window) || (IgnoreMouse() && !firstmove))
		{
			SDLdoUngrabMouse();
			firstmove = false;
			return;
		}

		// If using relative mouse mode, don't post an event_t just now,
		// add on the offsets so we can make an overall event later.
		if (SDL_GetRelativeMouseMode())
		{
			if (SDL_GetMouseFocus() == window && SDL_GetKeyboardFocus() == window)
			{
				mousemovex +=  evt.xrel;
				mousemovey += -evt.yrel;
				SDL_SetWindowGrab(window, SDL_TRUE);
			}
			firstmove = false;
			return;
		}

		// If the event is from warping the pointer to middle
		// of the screen then ignore it.
#if 0
		if ((evt.x == realwidth/2) && (evt.y == realheight/2))
#else
		// STAR NOTE: realwidth and realheight don't exist... //
		if ((evt.x == vid.width/2) && (evt.y == vid.height/2))
#endif
		{
			firstmove = false;
			return;
		}

		// Don't send an event_t if not in relative mouse mode anymore,
		// just grab and set relative mode
		// this fixes the stupid camera jerk on mouse entering bug
		// -- Monster Iestyn
		if (SDL_GetMouseFocus() == window && SDL_GetKeyboardFocus() == window)
		{
			SDLdoGrabMouse();
		}
	}

	firstmove = false;
}

static void Impl_HandleMouseButtonEvent(SDL_MouseButtonEvent evt, Uint32 type)
{
	event_t event;

	SDL_memset(&event, 0, sizeof(event_t));

	// Ignore the event if the mouse is not actually focused on the window.
	// This can happen if you used the mouse to restore keyboard focus;
	// this apparently makes a mouse button down event but not a mouse button up event,
	// resulting in whatever key was pressed down getting "stuck" if we don't ignore it.
	// -- Monster Iestyn (28/05/18)
	if (SDL_GetMouseFocus() != window || IgnoreMouse())
		return;

	/// \todo inputEvent.button.which
	if (USE_MOUSEINPUT)
	{
		event.device = 0;

		if (type == SDL_MOUSEBUTTONUP)
		{
			event.type = ev_keyup;
		}
		else if (type == SDL_MOUSEBUTTONDOWN)
		{
			event.type = ev_keydown;
		}
		else return;
		if (evt.button == SDL_BUTTON_MIDDLE)
			event.key = KEY_MOUSE1+2;
		else if (evt.button == SDL_BUTTON_RIGHT)
			event.key = KEY_MOUSE1+1;
		else if (evt.button == SDL_BUTTON_LEFT)
			event.key = KEY_MOUSE1;
		else if (evt.button == SDL_BUTTON_X1)
			event.key = KEY_MOUSE1+3;
		else if (evt.button == SDL_BUTTON_X2)
			event.key = KEY_MOUSE1+4;
		if (event.type == ev_keyup || event.type == ev_keydown)
		{
			D_PostEvent(&event);
		}
	}
}

static void Impl_HandleMouseWheelEvent(SDL_MouseWheelEvent evt)
{
	event_t event;

	SDL_memset(&event, 0, sizeof(event_t));

	event.device = 0;

	if (evt.y > 0)
	{
		event.key = KEY_MOUSEWHEELUP;
		event.type = ev_keydown;
	}
	if (evt.y < 0)
	{
		event.key = KEY_MOUSEWHEELDOWN;
		event.type = ev_keydown;
	}
	if (evt.y == 0)
	{
		event.key = 0;
		event.type = ev_keyup;
	}
	if (event.type == ev_keyup || event.type == ev_keydown)
	{
		D_PostEvent(&event);
	}
}

static void Impl_HandleControllerAxisEvent(SDL_ControllerAxisEvent evt)
{
	event_t event;
	INT32 value;

	event.type = ev_gamepad_axis;

	event.device = 1 + evt.which;
	if (event.device == INT32_MAX)
	{
		return;
	}

	event.key = event.x = event.y = INT32_MAX;

	//axis
	if (evt.axis > 2 * JOYAXISSETS)
	{
		return;
	}

	//vaule[sic]
	value = SDLJoyAxis(evt.value, evt.which);

	if (evt.axis & 1)
	{
		event.y = value;
	}
	else
	{
		event.x = value;
	}

	event.key = evt.axis / 2;

	D_PostEvent(&event);
}

static void Impl_HandleControllerButtonEvent(SDL_ControllerButtonEvent evt, Uint32 type)
{
	event_t event;

	event.device = 1 + evt.which;

	if (event.device == INT32_MAX)
	{
		return;
	}

	event.key = KEY_JOY1;
	event.x = 0;

	if (type == SDL_CONTROLLERBUTTONUP)
	{
		event.type = ev_keyup;
	}
	else if (type == SDL_CONTROLLERBUTTONDOWN)
	{
		event.type = ev_keydown;
	}
	else
	{
		return;
	}

	if (evt.button < JOYBUTTONS)
	{
		event.key += evt.button;
	}
	else
	{
		return;
	}

	SDLJoyRemap(&event);

	if (event.type != ev_console)
	{
		D_PostEvent(&event);
	}
}

static void Impl_HandleControllerDeviceAddedEvent(SDL_ControllerDeviceEvent event)
{
	// The game is always interested in controller events, even if they aren't internally assigned to a player.
	// Thus, we *always* open SDL controllers as they become available, to begin receiving their events.

	SDL_GameController* controller = SDL_GameControllerOpen(event.which);
	if (controller == NULL)
	{
		return;
	}

	SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
	SDL_JoystickID joystick_instance_id = SDL_JoystickInstanceID(joystick);

	event_t engine_event = {};

	engine_event.type = ev_gamepad_device_added;
	engine_event.device = 1 + joystick_instance_id;

	D_PostEvent(&engine_event);
}

static void Impl_HandleControllerDeviceRemovedEvent(SDL_ControllerDeviceEvent event)
{
	// SDL only posts Device Removed events for controllers that have actually been opened.
	// Thus, we don't need to filter out controllers that may not have opened successfully prior to this event.
	event_t engine_event = {};

	engine_event.type = ev_gamepad_device_removed;
	engine_event.device = 1 + event.which;

	D_PostEvent(&engine_event);
}

void DRRR_I_GetEvent(void)
{
	SDL_Event evt;

	// We only want the first motion event,
	// otherwise we'll end up catching the warp back to center.
	//int mouseMotionOnce = 0;

	if (!graphics_started)
	{
		return;
	}

	mousemovex = mousemovey = 0;

#if 0
	// STAR NOTE: imgui doesn't exist (...yet >:) ) //
	ImGuiIO& io = ImGui::GetIO();
#endif

	while (SDL_PollEvent(&evt))
	{
#if 0
		// STAR NOTE: imgui doesn't exist (...yet >:) ) //
		ImGui_ImplSDL2_ProcessEvent(&evt);
		if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		{
			continue;
		}
#endif

		switch (evt.type)
		{
			case SDL_WINDOWEVENT:
				Impl_HandleWindowEvent(evt.window);
				break;
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				Impl_HandleKeyboardEvent(evt.key, evt.type);
				break;
			case SDL_MOUSEMOTION:
				//if (!mouseMotionOnce)
				Impl_HandleMouseMotionEvent(evt.motion);
				//mouseMotionOnce = 1;
				break;
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				Impl_HandleMouseButtonEvent(evt.button, evt.type);
				break;
			case SDL_MOUSEWHEEL:
				Impl_HandleMouseWheelEvent(evt.wheel);
				break;
			case SDL_CONTROLLERAXISMOTION:
				Impl_HandleControllerAxisEvent(evt.caxis);
				break;
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERBUTTONDOWN:
				Impl_HandleControllerButtonEvent(evt.cbutton, evt.type);
				break;

			case SDL_CONTROLLERDEVICEADDED:
				Impl_HandleControllerDeviceAddedEvent(evt.cdevice);
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				Impl_HandleControllerDeviceRemovedEvent(evt.cdevice);
				break;

			case SDL_QUIT:
				LUA_HookBool(true, HOOK(GameQuit));
				I_Quit();
				break;
		}
	}

	// Send all relative mouse movement as one single mouse event.
	if (mousemovex || mousemovey)
	{
		event_t event;
		int wwidth, wheight;
		SDL_GetWindowSize(window, &wwidth, &wheight);
		//SDL_memset(&event, 0, sizeof(event_t));
		event.type = ev_mouse;
		event.key = 0;
#if 0
		event.x = (INT32)lround(mousemovex * ((float)wwidth / (float)realwidth));
		event.y = (INT32)lround(mousemovey * ((float)wheight / (float)realheight));
#else
		// STAR NOTE: realwidth and realheight don't exist... //
		event.x = (INT32)lround(mousemovex * ((float)wwidth / (float)vid.width));
		event.y = (INT32)lround(mousemovey * ((float)wheight / (float)vid.height));
#endif
		D_PostEvent(&event);
	}

	// In order to make wheels act like buttons, we have to set their state to Up.
	// This is because wheel messages don't have an up/down state.
	G_GetDeviceGameKeyDownArray(0)[KEY_MOUSEWHEELDOWN] = G_GetDeviceGameKeyDownArray(0)[KEY_MOUSEWHEELUP] = 0;
}

#endif
