// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2023-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  smkg-curl-routines.c
/// \brief Easy curl routines for TSoURDt3rd

#ifdef HAVE_CURL

#include <curl/curl.h>
#include "smkg-curl.h"

#include "../smkg-cvars.h"
#include "../core/smkg-g_game.h"
#include "../menus/smkg-m_sys.h" // menumessage //
#include "../misc/smkg-m_misc.h"

#include "../../d_main.h"
#include "../../g_game.h"
#include "../../i_system.h"
#include "../../i_video.h"
#include "../../v_video.h"
#include "../../z_zone.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// void TSoURDt3rd_CurlRoutine_FindUpdates(void)
// Finds the current version of TSoURDt3rd from the Github Repository.
//
void TSoURDt3rd_CurlRoutine_FindUpdates(void)
{
	const char *internet_data_filename = "tsourdt3rd_findupdate_data.txt";
	const char *build_path = TSoURDt3rd_FOL_ReturnHomepath_Build();

	char *return_info = NULL;
	char *return_version = NULL;
	INT32 return_code = 0;

	char version_url[256];
	UINT32 version_number = 0;

	static boolean send_event_message = false;
	static const char *header_string = NULL;
	static char message_string[256];
	INT32 message_type = 0;

	if (tsourdt3rd_local.curl.checked_version)
	{
		if (!dedicated && !menumessage.active && send_event_message)
		{
			// We use some checks to screen the message up here, as it could get skipped out by another message otherwise
			TSoURDt3rd_M_StartPlainMessage(header_string, message_string);
			send_event_message = false;
		}
		return;
	}

	if (!dedicated)
	{
		M_DrawTextBox(52, BASEVIDHEIGHT/2-10, 25, 2);
		V_DrawCenteredString(BASEVIDWIDTH/2, BASEVIDHEIGHT/2, V_MENUCOLORMAP, "Checking for updates...");
		I_FinishUpdate(); // page flip or blit buffer
	}
	STAR_CONS_Printf(STAR_CONS_TSOURDT3RD, "Checking for updates...\n");

	sprintf(version_url,
		"https://raw.githubusercontent.com/StarManiaKG/The-Story-of-Uncapped-Revengence-Discord-the-3rd/%s/src/STAR/star_webinfo.h",
		compbranch
	);
#if 0
	return;
#endif
	TSoURDt3rd_Curl_FindStringWithinURL(
		TSoURDt3rd_FIL_AccessFile(build_path, internet_data_filename, "w+"),
		"#define TSOURDT3RDVERSION",
		version_url,
		&return_info,
		&return_code
	);

	switch (return_code)
	{
		case TSOURDT3RD_CURL_DATAFOUND:
		{
			return_info = return_info + 26;
			TSoURDt3rd_M_RemoveStringChars(return_info, "\"");

			return_version = Z_StrDup(return_info);
			return_info = TSoURDt3rd_M_RemoveStringChars(return_info, ".");

			//version_number = strtol(return_info, NULL, 10);
			version_number = (UINT32)atoi(return_info);
			if (version_number < 100)
				version_number *= 10; // add another decimal for subversions

			if (TSoURDt3rd_CurrentVersion() < version_number)
			{
				header_string = "Update TSoURDt3rd, please";
				sprintf(message_string,
					"You're using an outdated version of TSoURDt3rd.\n"
					"The most recent version is: \x82%s\x80\n"
					"You're using version: \x82%s\x80\n"
					"Check the SRB2 Message Board for the latest version!\n",
				return_version, TSOURDT3RDVERSION);
				message_type = STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR;
				send_event_message = true;
			}
			else if (TSoURDt3rd_CurrentVersion() > version_number)
			{
				header_string = "Hello TSoURDt3rd beta user!";
				sprintf(message_string,
					"You're using a version of TSoURDt3rd that hasn't even released yet.\n"
					"You're probably a tester or coder, and in that case, hello!\n"
					"Enjoy messing around with the build!"
				);
				message_type = STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR;
				send_event_message = true;
			}
			else
			{
				header_string = "No updates found!";
				sprintf(message_string, "Your TSoURDt3rd Executable is up-to-date! Have fun!");
				message_type = STAR_CONS_TSOURDT3RD|STAR_CONS_NOTICE;
			}
			break;
		}
		default:
			header_string = "Failed to check for updates!";
			sprintf(message_string,
				"Couldn't properly check for updates.\n"
				"Maybe try again?\n"
				"\n"
				"If you don't know how to try again,\n"
				"just head to the 'Main Options' submenu,\n"
				"under the 'TSoURDt3rd Options' menu.\n"
				"You'll find the option to retry there."
			);
			message_type = STAR_CONS_TSOURDT3RD|STAR_CONS_ERROR;
			break;
	}
	TSoURDt3rd_FIL_RemoveFile(build_path, internet_data_filename);
	Z_Free(return_version);

	STAR_CONS_Printf(message_type, "%s\n", message_string);
	tsourdt3rd_local.curl.checked_version = true;
}

#endif // HAVE_CURL
