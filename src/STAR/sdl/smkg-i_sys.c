// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
//
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Changes by Graue <graue@oceanbase.org> are in the public domain.
//
//-----------------------------------------------------------------------------
/// \file  smkg-i_sys.c
/// \brief TSoURDt3rd main system stuff for SDL

#ifdef HAVE_SDL

#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include <signal.h>
#include <time.h>

#include "../smkg-i_sys.h"
#include "../core/smkg-p_pads.h"
#include "../smkg-cvars.h"
#include "../star_vars.h"

#include "../../d_event.h"
#include "../../d_main.h" // srb2path & srb2home
#include "../../m_argv.h"
#include "../../m_random.h"

// ------------------------ //
//        Variables
// ------------------------ //

#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
#define UNIXBACKTRACE
#endif

static const char *gamecontrollerdb_paths[] = {
	"",
	(PATHSEP "data" PATHSEP),
	(PATHSEP "TSoURDt3rd" PATHSEP),
	(PATHSEP "TSoURDt3rd" PATHSEP "data" PATHSEP),
	NULL
};
static boolean found_gamecontrollerdb_path = false;
static boolean found_gamecontrollerdb_user_path = false;

static boolean gamepad_rumble_disabled = false;
static boolean gamepad_trigger_rumble_disabled = false;

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_I_Pads_InitControllers(void)
// Sets up our lock-on input system for SDL Game Controllers!
//
void TSoURDt3rd_I_Pads_InitControllers(void)
{
	char dbpath[1024];
	UINT8 i;

	for (i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
	{
		TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[i];
		controller_data->active = false;
		controller_data->game_device = NULL;
		controller_data->joy_device = NULL;
		controller_data->id = -1;
		controller_data->real_id = 0;
		controller_data->name = NULL;
	}
	if (M_CheckParm("-nojoy") || M_CheckParm("-tsourdt3rd_nogamepadrefactor"))
		return;
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "TSoURDt3rd_I_Pads_InitControllers()...\n");

	for (i = 0; gamecontrollerdb_paths[i] != NULL; i++)
	{
		sprintf(dbpath, "%s" PATHSEP "%s" "gamecontrollerdb.txt", srb2path, gamecontrollerdb_paths[i]);
		if (SDL_GameControllerAddMappingsFromFile(dbpath))
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("TSoURDt3rd_I_Pads_InitControllers() - Initialized game controller data in game path!\n"));
			found_gamecontrollerdb_path = true;
			break;
		}
	}
	for (i = 0; gamecontrollerdb_paths[i] != NULL; i++)
	{
		sprintf(dbpath, "%s" PATHSEP "%s" "gamecontrollerdb_user.txt", srb2home, gamecontrollerdb_paths[i]);
		if (SDL_GameControllerAddMappingsFromFile(dbpath))
		{
			STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("TSoURDt3rd_I_Pads_InitControllers() - Initialized game controller data in user path!\n"));
			found_gamecontrollerdb_user_path = true;
			break;
		}
	}
	if (found_gamecontrollerdb_path == false && found_gamecontrollerdb_user_path == false)
		STAR_CONS_Printf(STAR_CONS_DEBUG, M_GetText("TSoURDt3rd_I_Pads_InitControllers() - Couldn't initialize game controller data in either game path or user path!\n"));

	if (SDL_WasInit(TSOURDT3RD_GAMEPAD_INIT_FLAGS))
	{
		// Game controller already initialized, don't do it again!
		return;
	}

	if (M_CheckParm("-noxinput"))
		SDL_SetHintWithPriority("SDL_XINPUT_ENABLED", "0", SDL_HINT_OVERRIDE);
	if (M_CheckParm("-nohidapi"))
		SDL_SetHintWithPriority("SDL_JOYSTICK_HIDAPI", "0", SDL_HINT_OVERRIDE);

	if (SDL_InitSubSystem(TSOURDT3RD_GAMEPAD_INIT_FLAGS) == -1)
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD,
			M_GetText("TSoURDt3rd_I_Pads_InitControllers() - Couldn't initialize game controllers: %s\n"),
			SDL_GetError()
		);
		return;
	}

	gamepad_rumble_disabled = M_CheckParm("-tsourdt3rd_gamepads_norumble");
	gamepad_trigger_rumble_disabled = M_CheckParm("-tsourdt3rd_gamepads_notriggerrumble");

	// Upon initialization, the gamecontroller subsystem will automatically dispatch
	// controller device added events for controllers connected before initialization.
}

void TSoURDt3rd_I_Pads_SetIndicatorColor(INT32 device_id, UINT8 red, UINT8 green, UINT8 blue)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)red;
	(void)green;
	(void)blue;
#else
	TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[device_id];

	if (controller_data == NULL || controller_data->active == false || controller_data->id < 0)
	{
		return;
	}

	if (controller_data->game_device)
		SDL_GameControllerSetLED(controller_data->game_device, red, green, blue);
	else if (controller_data->joy_device)
		SDL_JoystickSetLED(controller_data->joy_device, red, green, blue);
#endif
}

void TSoURDt3rd_I_Pads_Rumble(INT32 device_id, fixed_t low_strength, fixed_t high_strength, tic_t duration_tics)
{
#if !(SDL_VERSION_ATLEAST(2,0,9))
	(void)device_id;
	(void)low_strength;
	(void)high_strength;
	(void)duration;
#else
	TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[device_id];

	UINT16 small_magnitude = max(0, min(low_strength, UINT16_MAX));
	UINT16 large_magnitude = max(0, min(high_strength, UINT16_MAX));
	UINT16 duration = min(TICS_TO_MS(duration_tics), UINT16_MAX);

	if (gamepad_rumble_disabled == true)
	{
		return;
	}
	if (controller_data == NULL || controller_data->active == false || controller_data->id < 0)
	{
		return;
	}
	if (!controller_data->rumble.supported || controller_data->rumble.paused)
	{
		return;
	}
	if (cv_tsourdt3rd_drrr_controls_rumble[device_id].value == 0)
	{
		return;
	}

	controller_data->rumble.small_magnitude = small_magnitude;
	controller_data->rumble.large_magnitude = large_magnitude;
	controller_data->rumble.duration = duration;

#if 0
	STAR_CONS_Printf(STAR_CONS_DEBUG, "Starting rumble effect for controller %d:\n", controller_data->id);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "* - Small motor magnitude: %f\n", controller_data->rumble.small_magnitude / 65535.0f);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "* - Large motor magnitude: %f\n", controller_data->rumble.large_magnitude / 65535.0f);
	if (!duration)
		STAR_CONS_Printf(STAR_CONS_DEBUG, "Duration: forever\n");
	else
		STAR_CONS_Printf(STAR_CONS_DEBUG, "Duration: %dms\n", controller_data->rumble.duration);
#endif

	if (controller_data->game_device)
		SDL_GameControllerRumble(controller_data->game_device, controller_data->rumble.small_magnitude, controller_data->rumble.large_magnitude, controller_data->rumble.duration);
	else if (controller_data->joy_device)
		SDL_JoystickRumble(controller_data->joy_device, controller_data->rumble.small_magnitude, controller_data->rumble.large_magnitude, controller_data->rumble.duration);
#endif
}

void TSoURDt3rd_I_Pads_RumbleTriggers(INT32 device_id, fixed_t left_strength, fixed_t right_strength, tic_t duration_tics)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)left_strength;
	(void)right_strength;
#else
	TSoURDt3rd_ControllerInfo *controller_data = &tsourdt3rd_controllers[device_id];

	UINT16 left_magnitude = max(0, min(left_strength, UINT16_MAX));
	UINT16 right_magnitude = max(0, min(right_strength, UINT16_MAX));
	UINT16 duration = min(TICS_TO_MS(duration_tics), UINT16_MAX);

	if (gamepad_trigger_rumble_disabled == true)
	{
		return;
	}
	if (controller_data == NULL || controller_data->active == false || controller_data->id < 0)
	{
		return;
	}
	if (!controller_data->trigger_rumble.supported || controller_data->rumble.paused)
	{
		return;
	}

	controller_data->trigger_rumble.right_magnitude = left_magnitude;
	controller_data->trigger_rumble.left_magnitude = right_magnitude;
	controller_data->trigger_rumble.duration = duration;

#if 0
	STAR_CONS_Printf(STAR_CONS_DEBUG, "Starting rumble effect for controller %d:\n", controller_data->id);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "* - Left trigger motor magnitude: %f\n", controller_data->trigger_rumble.left_magnitude / 65535.0f);
	STAR_CONS_Printf(STAR_CONS_DEBUG, "* - Right trigger motor magnitude: %f\n", controller_data->trigger_rumble.right_magnitude / 65535.0f);
	if (!controller_data->trigger_rumble.duration)
		STAR_CONS_Printf(STAR_CONS_DEBUG, "Duration: forever\n");
	else
		STAR_CONS_Printf(STAR_CONS_DEBUG, "Duration: %dms\n", controller_data->trigger_rumble.duration);
#endif

	if (controller_data->game_device)
		SDL_GameControllerRumbleTriggers(controller_data->game_device, controller_data->trigger_rumble.left_magnitude, controller_data->trigger_rumble.right_magnitude, controller_data->trigger_rumble.duration);
	else if (controller_data->joy_device)
		SDL_JoystickRumbleTriggers(controller_data->joy_device, controller_data->trigger_rumble.left_magnitude, controller_data->trigger_rumble.right_magnitude, controller_data->trigger_rumble.duration);
#endif
}

// =======
// WINDOWS
// =======

void TSoURDt3rd_I_CursedWindowMovement(int xd, int yd)
{
	SDL_SetWindowPosition(window, window_x + xd, window_y + yd);
}

void TSoURDt3rd_I_QuakeWindow(int xd, int yd)
{
	if (!cv_tsourdt3rd_video_sdl_window_shaking.value || cv_fullscreen.value || (window_x == -1 || window_y == -1))
	{
#if 0
		SDL_SetWindowPosition(window,
			SDL_WINDOWPOS_CENTERED_DISPLAY(SDL_GetWindowDisplayIndex(window)),
			SDL_WINDOWPOS_CENTERED_DISPLAY(SDL_GetWindowDisplayIndex(window))
		);
#endif
		SDL_GetWindowPosition(window, &window_x, &window_y);
		return;
	}
	TSoURDt3rd_I_CursedWindowMovement(xd, yd);
}

//
// static const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped)
// Generates a Funny Crash Message Everytime TSoURDt3rd Crashes
//
static const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped)
{
	static const char *jokemsg;
	char underscoremsg[256];
	size_t i, tabend, current;

	// If we crashed too early, the P_Rand or M_Rand functions may not have been initialized yet.
	// So, just in case, let's initialize them!
	if (P_GetRandSeed() == P_GetInitSeed())
	{
		if (!M_RandomSeedFromOS())
			M_RandomSeed((UINT32)time(NULL)); // less good but serviceable
		P_SetRandSeed(M_RandomizedSeed());
	}

	// Come up With a Random Funny Joke //
	switch (M_RandomRange(0, 10))
	{
		// Static //
		default: jokemsg = "Uh..."; break;
		case 1: jokemsg = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAH!"; break;
		case 2: jokemsg = "This will make the TSoURDt3rd Devs sad..."; break;
		case 3: jokemsg = "This will make STJr sad..."; break;
		case 4: jokemsg = "Sonic, what did you do this time?"; break;

		// References //
		// Sonic Adventure
		case 5:
		{
			switch (rand() % 2)
			{
				case 1: jokemsg = "Watch out, you're gonna crash! AAAAH!"; break;
				default: jokemsg = "OH NO!"; break;
			}
			break;
		}

		// Grand Theft Auto
		case 6:
		{
			switch (rand() % 2)
			{
				case 1: jokemsg = "All you had to do, was not crash the game, Sonic!"; break;
				default: jokemsg = "All we had to do, was follow the dang train, CJ!"; break;
			}
			break;
		}

		// Baldi's Basics
		case 7: jokemsg = "OOPS! You messed up!"; break;

		// Sonic Rush
		case 8:
		{
			switch (rand() % 4)
			{
				case 1: jokemsg = "Should we try this again?"; break;
				case 2: jokemsg = "Never get on my bad side!"; break;

				case 3: jokemsg = "Feeling hounded and surrounded!"; break;
				default: jokemsg = "Step by step."; break;
			}
			break;
		}

		// Parappa the Rapper
		case 9:
		{
			switch (rand() % 4)
			{
				case 1:
				{
					switch (rand() % 7)
					{
						case 1: jokemsg = "I gotta, I gotta... No! I can't believe..."; break;
						case 2: jokemsg = "Remember, you gotta believe!"; break;

						case 3: jokemsg = "TSoURDt3rd cut a few corners but we didn't think it would be this bad..."; break;
						case 4: jokemsg = "No cutting corners!"; break;

						case 5: jokemsg = "Oops!"; break;
						case 6: jokemsg = "Uh oh!"; break;

						default: jokemsg = "\t\t\tTRY AGAIN!!\n\n\t\t(X) YES\t\t      (O) NO\t\t\t\t"; break;
					}
					break;
				}
				case 2:
				{
					switch (rand() % 13)
					{
						case 1: jokemsg = "Coding, TSoURDt3rd, it's all in the mind."; break;

						case 2: jokemsg = "Alright, we're here, just playing TSoURDt3rd, I want you to show me if you can get far."; break;
						case 3: jokemsg = "Whoa ho ho ho, stop TSoURDt3rd!\nWe got an emergency, can't you see?"; break;

						case 4: jokemsg = "TSoURDt3rd is all you need!"; break;
						case 5: jokemsg = "All you ever need is to be nice and friendly!"; break;

						case 6: jokemsg = "Every single day, stress comes in every way."; break;
						case 7: jokemsg = "Seafood cake comes just like the riddle."; break;
						case 8: jokemsg = "TSoURDt3rd comes just like the riddle."; break;

						case 9: jokemsg = "Nuh uh, nuh uh, no way!"; break;

						case 10: jokemsg = "Whatcha gonna do, when they come?"; break;
						case 11: jokemsg = "I gotta redeem!\nI gotta relieve!\nI gotta receive!\nI GOTTA BELIEVE!"; break;
						case 12: jokemsg = "Breakin' out was the name of the game for me, you, you, You, and YOU!"; break;
						default: jokemsg = "Somebody say ho! Say ho ho! Say ho ho ho! Now scream!\nEverybody say ho! Say ho ho! Say ho ho ho! Now scream!"; break;
					}
					break;
				}
				case 3:
				{
					switch (rand() % 7)
					{
						case 1: jokemsg = "Again."; break;
						case 2: jokemsg = "You gotta do it again!"; break;
						case 3: jokemsg = "Uh uh, uh uh, come on, let's do this all over again."; break;
						case 4: jokemsg = "Peep! Peep! (TRY AGAIN!!)"; break;
						case 5: jokemsg = "Maybe next time..."; break;
						case 6: jokemsg = "O, oh, I didn't think you'd do THIS bad!"; break;
						default: jokemsg = "U rappin' AWFUL!"; break;
					}
					break;
				}
				default:
				{
					switch (rand() % 4)
					{
						case 1: jokemsg = "TSoURDt3rd's Devs are codin' AWFUL!"; break;
						case 2: jokemsg = "We're crashin' AWFUL!"; break;
						case 3: jokemsg = "U playin' COOL!"; break;
						default: jokemsg = "I shouldn't cut corners.\nYou shouldn't cut corners.\nTSoURDt3rd Devs and STJr shouldn't cut corners."; break;
					}
					break;
				}
			}
			break;
		}

		// SRB2
		case 10:
		{
			switch (rand() % 5)
			{
				// STJr Members
				case 1: jokemsg = (crashnum == SIGSEGV ? (coredumped ? "SIGSEGV - seventh sentinel (core dumped)" : "SIGSEGV - seventh sentinel") : "...Huh. This is awkward..."); break;

				// Zone Builder
				case 2:
				{
					switch (rand() % 3)
					{
						case 1: jokemsg = "I'll miss you, you know."; break;
						case 2: jokemsg = "GAME OVER!"; break;
						default: jokemsg = "You were always my favorite user."; break;
					}
					break;
				}

				// References to past versions
				case 3: jokemsg = "Did you get all those Chaos Emeralds?"; break;
				case 4: jokemsg = "All your rings are belong to us!"; break;

				// References to game bugs
				default: jokemsg = "The Leader of the Chaotix would be very disappointed in you."; break;
			}
			break;
		}
	}

	// Underscore our Funny Crash Message, Return it, and We're Done :) //
	memset(underscoremsg, 0, sizeof(underscoremsg));
	for (i = current = 0; jokemsg[current]; i++, current++)
	{
		// Run Special Operations //
		if (jokemsg[current] == '\t') // Tabs
			for (tabend = i+8; i < tabend; i++)
				underscoremsg[i] = '_';
		else if (jokemsg[current] == '\n') // New Lines
			i = 0;

		// Uppercase Letters
#if 0
		else if ((isupper(jokemsg[current]) && !isupper(jokemsg[current+1]))
#else
		else if ((isupper(jokemsg[current]))
#endif
			&& (jokemsg[current+1] != ' ')
			&& (jokemsg[current+1] != '\t')
			&& (jokemsg[current+1] != '\n'))
		{
			underscoremsg[i++] = '_';
		}

		underscoremsg[i] = '_';
	}

	return va("%s\n%s", jokemsg, underscoremsg);
}

//
// void TSoURDt3rd_I_ShowErrorMessageBox(const char *messagefordevelopers, const SDL_MessageBoxData *messageboxdata, int *buttonid, int num, boolean coredumped)
//
// Displays an error box popup when the game crashes,
//	telling the user to check logfiiles for the reason as to why.
//
// Inspired by I_ShowErrorMessageBox() from Dr.Robotnik's Ring Racers!
//
void TSoURDt3rd_I_ShowErrorMessageBox(const char *messagefordevelopers, const SDL_MessageBoxData *messageboxdata, int *buttonid, int num, boolean coredumped)
{
	static char crash_reason_header[20];
	static char underscoremsg[24];
	static char finalmessage[2048];
	static size_t underscore_interval = 0;

	memset(crash_reason_header, 0, sizeof(crash_reason_header));
	if (messagefordevelopers)
	{
		snprintf(crash_reason_header, sizeof(crash_reason_header), "\n\nCRASH REASON:\n");
		while (underscore_interval < strlen(crash_reason_header))
			underscoremsg[underscore_interval++] = '_';
		underscoremsg[underscore_interval++] = '\n';
		underscoremsg[underscore_interval++] = '\n';
	}

	snprintf(
		finalmessage,
		sizeof(finalmessage),
			"%s\n"
			"\n"
			"\"SRB2 %s; %s\" has encountered an unrecoverable error and needs to close.\n"
			"This is (usually) not your fault, but we encourage you to report it to the creator, StarManiaKG!\n"
			"This should be done alongside your %s log file (%s).\n"
			"\n"
			"The information in the log file is generally useful for developers, like this mod's creator, and maybe even STJr too.\n"
			"Developers can screw up, and we literally work for free, and some of us aren't even adults, so try to be nice and considerate!\n"
			"The information may also be useful for server hosts and add-on creators too.\n"
			"\n"
			"To share any info regarding this crash, there are several things you can do:\n"
			" - Visit the SRB2 Discord, using the button below.\n"
			" - Reach out to StarManiaKG on Discord, by sending them a message.\n"
			" - Visit TSoURDt3rd's message board post, using the button below.\n"
			"\n"
			"See you next game!\n"
			"%s"
			"%s"
			"%s",
		TSoURDt3rd_GenerateFunnyCrashMessage(num, coredumped),
		SRB2VERSION, TSOURDT3RDVERSIONSTRING,
#if defined (UNIXBACKTRACE)
		"crash-log.txt"
#elif defined (_WIN32)
		".rpt crash dump"
#endif
		" (very important!) and",
#ifdef LOGMESSAGES
		logfilename[0] ? logfilename :
#endif
		"uh oh, one wasn't made!?",
		crash_reason_header,
		underscoremsg,
		(messagefordevelopers ? messagefordevelopers : "")
	);

	// Just in case SDL_MessageBoxData is NULL
	const SDL_MessageBoxButtonData tsourdt3rd_buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0,		      "OK" },
		{ 										0, 1,        "Discord" },
		{ 										0, 2,  "Message Board" },
	};
	const SDL_MessageBoxData tsourdt3rd_messageboxdata = {
		(messageboxdata ? messageboxdata->flags : SDL_MESSAGEBOX_ERROR),
		(messageboxdata ? messageboxdata->window : NULL),
		(messageboxdata ? messageboxdata->title :
			("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" Recursive Error")),
		finalmessage,
		SDL_arraysize(tsourdt3rd_buttons),
		tsourdt3rd_buttons,
		(messageboxdata ? messageboxdata->colorScheme : NULL),
	};
	int tsourdt3rd_buttonid = (buttonid != NULL ? (*buttonid) : 0);

	// Implement message box with SDL_ShowMessageBox,
	// which should fail gracefully if it can't put a message box up
	// on the target system
	SDL_ShowMessageBox(&tsourdt3rd_messageboxdata, &tsourdt3rd_buttonid);
	switch (tsourdt3rd_buttonid)
	{
		case 1:
			if (buttonid == NULL)
			{
				// We can't rely on the base error handler for this one!
				TSoURDt3rd_I_OpenURL("https://www.srb2.org/discord");
			}
			break;
		case 2:
			TSoURDt3rd_I_OpenURL("https://mb.srb2.org/addons/the-story-of-uncapped-revengence-discord-the-3rd.4932/");
			break;
		default:
			break;
	}
}

//
// void TSoURDt3rd_I_ShutdownSystem(void)
// Some exclusive TSoURDt3rd things to run when shutting down SRB2.
//
void TSoURDt3rd_I_ShutdownSystem(void)
{
	I_OutputMsg("TSoURDt3rd_I_ShutdownSystem() - Shutting down TSoURDt3rd...\n");

	for (UINT8 i = 0; i < TSOURDT3RD_NUM_GAMEPADS; i++)
		TSoURDt3rd_I_Pads_SetIndicatorColor(i, 0, 0, 255);
	TSoURDt3rd_P_Pads_ResetDeviceRumble(-1);
}

#endif // HAVE_SDL
