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
#include "../../../d_main.h"
#include "../../../sdl/sdlmain.h"
#include "../../../i_system.h"

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

void DRRR_I_GetEvent(SDL_Event evt)
{
	// We only want the first motion event,
	// otherwise we'll end up catching the warp back to center.
	//int mouseMotionOnce = 0;

	if (!graphics_started)
	{
		return;
	}

#if 0
    // STAR NOTE: not needed //
	mousemovex = mousemovey = 0;

	ImGuiIO& io = ImGui::GetIO();
#endif

	while (SDL_PollEvent(&evt))
    {
        switch (evt.type)
		{
            case SDL_CONTROLLERDEVICEADDED:
				Impl_HandleControllerDeviceAddedEvent(evt.cdevice);
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				Impl_HandleControllerDeviceRemovedEvent(evt.cdevice);
				break;
        }
    }
}

#endif
