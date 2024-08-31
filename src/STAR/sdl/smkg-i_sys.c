// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
//
// Copyright (C) 2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
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

#ifdef _MSC_VER
#pragma warning(disable : 4214 4244)
#endif

#ifdef HAVE_SDL
#ifdef _MSC_VER
#include <windows.h>
#pragma warning(default : 4214 4244)
#endif

#include <signal.h>
#include <time.h>

#include "../smkg-i_sys.h"
#include "../star_vars.h"

#include "../drrr/kg_input.h"

#include "../../d_event.h"
#include "../../m_argv.h"
#include "../../m_random.h"

// ------------------------ //
//        Functions
// ------------------------ //

// ====
// PADS
// ====

void TSoURDt3rd_Pads_I_SetGamepadPlayerIndex(INT32 device_id, INT32 player)
{
#if !(SDL_VERSION_ATLEAST(2,0,12))
	(void)device_id;
	(void)player;
	return;
#endif

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher
	I_Assert(player >= 0 && player < MAXSPLITSCREENPLAYERS);

	SDL_Joystick *controller = SDL_JoystickFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_JoystickSetPlayerIndex(controller, player);
}

void TSoURDt3rd_Pads_I_SetGamepadIndicatorColor(INT32 device_id, UINT8 red, UINT8 green, UINT8 blue)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)red;
	(void)green;
	(void)blue;
	return;
#endif

	SDL_Joystick *controller = SDL_JoystickFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_JoystickSetLED(controller, red, green, blue);
}

void TSoURDt3rd_Pads_I_GamepadRumble(INT32 device_id, fixed_t low_strength, fixed_t high_strength)
{
#if !(SDL_VERSION_ATLEAST(2,0,9))
	(void)device_id;
	(void)low_strength;
	(void)high_strength;
	return;
#endif

	UINT16 small_magnitude = max(0, min(low_strength, UINT16_MAX));
	UINT16 large_magnitude = max(0, min(high_strength, UINT16_MAX));

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_Joystick *controller = SDL_JoystickFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_JoystickRumble(controller, small_magnitude, large_magnitude, 0);
}

void TSoURDt3rd_Pads_I_GamepadRumbleTriggers(INT32 device_id, fixed_t left_strength, fixed_t right_strength)
{
#if !(SDL_VERSION_ATLEAST(2,0,14))
	(void)device_id;
	(void)left_strength;
	(void)right_strength;
	return;
#endif

	UINT16 left_magnitude = max(0, min(left_strength, UINT16_MAX));
	UINT16 right_magnitude = max(0, min(right_strength, UINT16_MAX));

	I_Assert(device_id > 0); // Gamepad devices are always ID 1 or higher

	SDL_Joystick *controller = SDL_JoystickFromInstanceID(device_id - 1);
	if (controller == NULL)
	{
		return;
	}

	SDL_JoystickRumbleTriggers(controller, left_magnitude, right_magnitude, 0);
}

// =======
// WINDOWS
// =======

void TSoURDt3rd_I_CursedWindowMovement(int xd, int yd)
{
	//if (cv_tsourdt3rd_video_windowquake.value)
	//	return;
	SDL_SetWindowPosition(window, window_x + xd, window_y + yd);
}

//
// static const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped)
// Generates a Funny Crash Message Everytime TSoURDt3rd Crashes
//
static const char *TSoURDt3rd_GenerateFunnyCrashMessage(INT32 crashnum, boolean coredumped)
{
	// Make Variables //
	const char *jokemsg;
	char underscoremsg[256] = "";
	size_t i, tabend, current;

	// We have to run these checks first because any
	// P_Rand or M_Rand functions may not have been initialized yet.
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
			switch (rand() % 3)
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

				// References to game bugs
				default: jokemsg = "The Leader of the Chaotix would be very disappointed in you."; break;
			}
			break;
		}
	}

	// Underscore our Funny Crash Message, Return it, and We're Done :) //
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
		else if ((isupper(jokemsg[current]))// && !isupper(jokemsg[current+1]))
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
// Inspired by I_ShowErrorMessageBox() from DRRR!
//
void TSoURDt3rd_I_ShowErrorMessageBox(const char *messagefordevelopers, const SDL_MessageBoxData *messageboxdata, int *buttonid, int num, boolean coredumped)
{
	const char *dumptype = "", *logtype = "uh oh, one wasn't made!?", *err_msg = "";
	static char finalmessage[2048];

#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
	dumptype = "crash-log.txt (very important!) and";
#elif defined (_WIN32)
	dumptype = ".rpt crash dump (very important!) and";
#endif

#ifdef LOGMESSAGES
	if (logfilename[0])
		logtype = logfilename;
#endif

	if (messagefordevelopers)
		err_msg = "\nCrash Reason:\n";
	else
		messagefordevelopers = "";

	snprintf(
		finalmessage,
		sizeof(finalmessage),
			"%s\n"
			"\n"
			"\"SRB2 %s; %s %s\" has encountered an unrecoverable error and needs to close.\n"
			"This is (usually) not your fault, but we encourage you to report it to the creator.\n"
			"This should be done alongside your %s log file (%s).\n"
			"\n"
			"The information in the log file is generally useful for developers, like StarManiaKG, and maybe even STJr.\n"
			"Developers tend to screw up, especially those working for free, so please be nice to them!\n"
			"The information may also be useful for server hosts and add-on creators too.\n"
			"\n"
			"Visit the SRB2 Discord below, or reach out to StarManiaKG on Discord, if you have any info regarding this crash.\n"
			"\n"
			"See you next game!\n"
			"%s"
			"%s",
		TSoURDt3rd_GenerateFunnyCrashMessage(num, coredumped),
		SRB2VERSION, TSOURDT3RDVERSIONSTRING, TSOURDT3RDBYSTARMANIAKGSTRING,
		dumptype, logtype,
		err_msg, messagefordevelopers
	);

	// Just in case SDL_MessageBoxData is NULL
	const SDL_MessageBoxButtonData tsourdt3rd_buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0,		"OK" },
		{ 										0, 1,  "Discord" },
	};

	const SDL_MessageBoxData tsourdt3rd_messageboxdata = {
		(messageboxdata ? messageboxdata->flags : SDL_MESSAGEBOX_ERROR),
		(messageboxdata ? messageboxdata->window : NULL),
		(messageboxdata ? messageboxdata->title :
			("SRB2 "VERSIONSTRING"; "TSOURDT3RDVERSIONSTRING" "TSOURDT3RDBYSTARMANIAKGSTRING" Recursive Error")),
		finalmessage,
		SDL_arraysize(tsourdt3rd_buttons),
		(messageboxdata ? messageboxdata->buttons : tsourdt3rd_buttons),
		(messageboxdata ? messageboxdata->colorScheme : NULL),
	};

	// Implement message box with SDL_ShowMessageBox,
	// which should fail gracefully if it can't put a message box up
	// on the target system
	SDL_ShowMessageBox(&tsourdt3rd_messageboxdata, buttonid);

#if SDL_VERSION_ATLEAST(2,0,14)
	if (!messageboxdata && *buttonid == 1)
		SDL_OpenURL("https://www.srb2.org/discord");
#endif
}

#endif // HAVE_SDL
