// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Vivian "toastergrl" Grannell.
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
// \file menus/menudefs/transient/drrr-message-box.c
// \brief MESSAGE BOX (aka: a hacked, cobbled together menu) (but one way better than Vanilla SRB2s')

#include "../../smkg-m_sys.h" // menuwipe & menumessage //
#include "../../../smkg-st_hud.h" // kp_buttons //

#include "../../../../v_video.h"
#include "../../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

// message prompt struct
struct menumessage_s menumessage;

// ------------------------ //
//        Functions
// ------------------------ //

//
// M_Sys_StringHeight
//
// Find string height from hu_font chars
//
// Ported from M_StringHeight() in Dr.Robotnik's Ring Racers!
//
static inline size_t M_Sys_StringHeight(const char *string)
{
	size_t h = 16, i, len = strlen(string);

	for (i = 0; i < len-1; i++)
	{
		if (string[i] != '\n')
			continue;
		h += 8;
	}

	return h;
}

// default message handler
// (...For Ring Racers at least)
void TSoURDt3rd_M_StartMessage(const char *header, const char *string, void (*routine)(INT32), menumessagetype_t itemtype, const char *confirmstr, const char *defaultstr)
{
	const UINT8 pid = 0;
	char *message = V_WordWrap(
		0, (BASEVIDWIDTH - 8),
		V_ALLOWLOWERCASE,
		string
	);

	DEBFILE(string);

	strncpy(menumessage.message, message, MAXMENUMESSAGE-1);
	Z_Free(message);

	menumessage.header = header;
	menumessage.flags = itemtype;
	menumessage.routine = routine;
	menumessage.answer = MA_NONE;
	menumessage.fadetimer = 1;
	menumessage.timer = 0;
	menumessage.closing = 0;
	menumessage.active = true;

	if (!routine)
	{
		menumessage.flags = MM_NOTHING;
	}

	// Set action strings
	switch (menumessage.flags)
	{
		// Send 1 to the routine if we're pressing A, 2 if B/X, 0 otherwise.
		case MM_YESNO:
			menumessage.defaultstr = defaultstr ? defaultstr : "No";
			menumessage.confirmstr = confirmstr ? confirmstr : "Yes";
			break;

		default:
			menumessage.defaultstr = defaultstr ? defaultstr : "OK";
			menumessage.confirmstr = NULL;
			break;
	}

	// event routine
	/*if (menumessage.flags == MM_EVENTHANDLER)
	{
		*(void**)&menumessage.eroutine = routine;
		menumessage.routine = NULL;
	}*/

	//added : 06-02-98: now draw a textbox around the message
	// oogh my god this was replaced in 2023

	menumessage.x = (8 * MAXSTRINGLENGTH) - 1;
	menumessage.y = M_Sys_StringHeight(menumessage.message);

	TSoURDt3rd_M_SetMenuDelay(pid);	// Set menu delay to avoid setting off any of the handlers.
}

void TSoURDt3rd_M_StopMessage(INT32 choice)
{
	if (!menumessage.active || menumessage.closing)
		return;

	const char pid = 0;

	// Set the answer.
	menumessage.answer = choice;

#if 1
	// The below was cool, but it felt annoyingly unresponsive.
	menumessage.closing = MENUMESSAGECLOSE+1;
#else
	// Intended length of time.
	menumessage.closing = (TICRATE/2);

	// This weird operation is necessary so the text flash is consistently timed.
	menumessage.closing |= ((2*MENUMESSAGECLOSE) - 1);
#endif

	TSoURDt3rd_M_SetMenuDelay(pid);
}

boolean TSoURDt3rd_M_MenuMessageTick(void)
{
	if (menuwipe)
		return false;

	if (menumessage.closing)
	{
		if (menumessage.closing > MENUMESSAGECLOSE)
		{
			menumessage.closing--;
		}
		else
		{
			if (menumessage.fadetimer > 0)
			{
				menumessage.fadetimer--;
			}

			if (menumessage.fadetimer == 0)
			{
				menumessage.active = false;

				if (menumessage.routine)
				{
					menumessage.routine(menumessage.answer);
				}
			}
		}

		return false;
	}
	else if (menumessage.fadetimer < 9)
	{
		menumessage.fadetimer++;
		return false;
	}

	menumessage.timer++;
	return true;
}

// regular handler for MM_NOTHING and MM_YESNO
void TSoURDt3rd_M_HandleMenuMessage(void)
{
	const UINT8 pid = 0;
	boolean btok = TSoURDt3rd_M_MenuConfirmPressed(pid);
	boolean btnok = TSoURDt3rd_M_MenuBackPressed(pid);

	if (!TSoURDt3rd_M_MenuMessageTick())
		return;

	switch (menumessage.flags)
	{
		// Send 1 to the routine if we're pressing A, 2 if B/X, 0 otherwise.
		case MM_YESNO:
		{
			if (btok)
				TSoURDt3rd_M_StopMessage(MA_YES);
			else if (btnok)
				TSoURDt3rd_M_StopMessage(MA_NO);
			break;
		}
		default:
		{
			if (btok || btnok)
				TSoURDt3rd_M_StopMessage(MA_NONE);
			break;
		}
	}
}

// Draw the message popup submenu
void TSoURDt3rd_M_DrawMenuMessage(void)
{
	INT32 x, y;
	size_t i, start = 0;
	char string[MAXMENUMESSAGE];
	const char *msg = NULL;

	if (!menumessage.active)
		return;
	x = (BASEVIDWIDTH - menumessage.x)/2;
	y = (BASEVIDHEIGHT - menumessage.y)/2 + floor(pow(2, (double)(9 - menumessage.fadetimer)));
	msg = menumessage.message;

	V_DrawFadeScreen(31, menumessage.fadetimer);
	V_DrawFill(0, y, BASEVIDWIDTH, menumessage.y, 159);

	if (menumessage.header != NULL)
	{
		V_DrawThinString(x, y - 10, tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, menumessage.header);
	}

	if (menumessage.defaultstr)
	{
		INT32 workx = x + menumessage.x;
		INT32 worky = y + menumessage.y;

		boolean standardbuttons = (cv_usejoystick.value || cv_usejoystick2.value);
		boolean push;

		if (menumessage.closing)
			push = (menumessage.answer != MA_YES);
		else
		{
			const UINT8 anim_duration = 16;
			push = ((menumessage.timer % (anim_duration * 2)) < anim_duration);
		}

		workx -= V_ThinStringWidth(menumessage.defaultstr, V_ALLOWLOWERCASE);
		V_DrawThinString(
			workx, worky + 1,
			((push && (menumessage.closing & MENUMESSAGECLOSE))
				? tsourdt3rd_highlightflags : 0)|V_ALLOWLOWERCASE,
			menumessage.defaultstr
		);

		workx -= 2;

		if (standardbuttons)
		{
			workx -= SHORT(kp_button_x[1][0]->width);
			TSoURDt3rd_MK_DrawButton(
				workx * FRACUNIT, worky * FRACUNIT,
				0, kp_button_x[1],
				push
			);

			workx -= SHORT(kp_button_b[1][0]->width);
			TSoURDt3rd_MK_DrawButton(
				workx * FRACUNIT, worky * FRACUNIT,
				0, kp_button_b[1],
				push
			);
		}
		else
		{
			TSoURDt3rd_M_DrawMediocreKeyboardKey("ESC", &workx, worky, push, true);
		}

		if (menumessage.confirmstr)
		{
			workx -= 12;

			if (menumessage.closing)
				push = !push;

			workx -= V_ThinStringWidth(menumessage.confirmstr, V_ALLOWLOWERCASE);
			V_DrawThinString(
				workx, worky + 1,
				((push && (menumessage.closing & MENUMESSAGECLOSE))
					? tsourdt3rd_highlightflags : 0)|V_ALLOWLOWERCASE,
				menumessage.confirmstr
			);

			workx -= 2;
		}

		if (standardbuttons)
		{
			workx -= SHORT(kp_button_a[1][0]->width);
			TSoURDt3rd_MK_DrawButton(
				workx * FRACUNIT, worky * FRACUNIT,
				0, kp_button_a[1],
				push
			);
		}
		else
		{
			TSoURDt3rd_M_DrawMediocreKeyboardKey("ENTER", &workx, worky, push, true);
		}
	}

	x -= 4;
	y += 4;

	while (*(msg+start))
	{
		size_t len = strlen(msg+start);

		for (i = 0; i < len; i++)
		{
			if (*(msg+start+i) == '\n')
			{
				memset(string, 0, MAXMENUMESSAGE);
				if (i >= MAXMENUMESSAGE)
				{
					CONS_Printf("TSoURDt3rd_M_DrawMenuMessage: too long segment in %s\n", msg);
					return;
				}
				else
				{
					strncpy(string,msg+start, i);
					string[i] = '\0';
					start += i;
					i = (size_t)-1; //added : 07-02-98 : damned!
					start++;
				}
				break;
			}
		}

		if (i == strlen(msg+start))
		{
			if (i >= MAXMENUMESSAGE)
			{
				CONS_Printf("TSoURDt3rd_M_DrawMenuMessage: too long segment in %s\n", msg);
				return;
			}
			else
			{
				strcpy(string, msg + start);
				start += i;
			}
		}

		V_DrawString((BASEVIDWIDTH - V_StringWidth(string, V_ALLOWLOWERCASE))/2, y, V_ALLOWLOWERCASE, string);
		y += 8;
	}
}

void TSoURDt3rd_M_DrawMenuMessageOnTitle(INT32 count)
{
	if (count > 0)
		TSoURDt3rd_M_DrawMenuMessage();
}
