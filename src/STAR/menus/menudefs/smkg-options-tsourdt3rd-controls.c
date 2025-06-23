// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-controls.c
/// \brief TSoURDt3rd's control menu options

#include "../smkg-m_sys.h"

#include "../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

static void M_Sys_DrawControls(void);
static void M_Sys_HandleProfileControls(void);
static boolean M_Sys_ProfileControlsInputs(INT32 ch);
static void M_Sys_SetControl(INT32 ch);
static void M_Sys_ProfileControlsConfirm(INT32 choice);

menuitem_t TSoURDt3rd_OP_ControlsMenu[] =
{
	{IT_HEADER, NULL, "Jukebox Menu", NULL, 0},
		{IT_CONTROL, NULL, "Open the Jukebox",
			M_Sys_SetControl, 0},
		{IT_CONTROL, NULL, "Stop the Jukebox",
			M_Sys_SetControl, 0},
		{IT_CONTROL, NULL, "Music Speed - Increase",
			M_Sys_SetControl, 0},
		{IT_CONTROL, NULL, "Music Speed - Decrease",
			M_Sys_SetControl, 0},
		{IT_CONTROL, NULL, "Music Pitch - Increase",
			M_Sys_SetControl, 0},
		{IT_CONTROL, NULL, "Music Pitch - Decrease",
			M_Sys_SetControl, 0},
		{IT_CONTROL, NULL, "Replay Recent Track",
			M_Sys_SetControl, 0},

	{IT_HEADER, NULL, "Settings", NULL, 0},
		{IT_STRING2 | IT_CVAR, NULL, "Player 1 Controller Rumble",
			&cv_tsourdt3rd_drrr_controls_rumble[0], 0},
		{IT_STRING2 | IT_CVAR, NULL, "Player 2 Controller Rumble",
			&cv_tsourdt3rd_drrr_controls_rumble[1], 0},

	{IT_HEADER, NULL, "Confirmation", NULL, 0},
		{IT_STRING | IT_CALL, NULL, "Confirm Controls",
			M_Sys_ProfileControlsConfirm, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ControlsMenu[] =
{
	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_OPENJUKEBOX, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_STOPJUKEBOX, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_INCREASEMUSICSPEED, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_DECREASEMUSICSPEED, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_INCREASEMUSICPITCH, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_DECREASEMUSICPITCH, 0},
		{NULL, "Edit the Jukebox controls for your ease-of-use!", {NULL}, JB_PLAYMOSTRECENTTRACK, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Enable rumble support for Player 1's controller.", { .cvar = &cv_tsourdt3rd_drrr_controls_rumble[0] }, 0, 0},
		{NULL, "Enable rumble support for Player 2's controller.", { .cvar = &cv_tsourdt3rd_drrr_controls_rumble[1] }, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Save your control changes.", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_ControlsDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_OP_TSOURDT3RD),
	NULL,
	sizeof (TSoURDt3rd_OP_ControlsMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_ControlsMenu,
	M_Sys_DrawControls,
	32, 80,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ControlsDef = {
	TSoURDt3rd_TM_OP_ControlsMenu,
	SKINCOLOR_MARINE, 0,
	MBF_DRAWBGWHILEPLAYING,
	NULL,
	0, 0,
	NULL,
	M_Sys_HandleProfileControls,
	NULL,
	NULL,
	M_Sys_ProfileControlsInputs,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_DrawBindMediumString(INT32 y, INT32 flags, const char *string)
{
	fixed_t w = V_FontStringWidth(string, flags, tny_font);
	fixed_t x = BASEVIDWIDTH/2 * FRACUNIT - w/2;
	V_DrawFontStringAtFixed(
		x,
		y * FRACUNIT,
		flags,
		FRACUNIT,
		FRACUNIT,
		string,
		tny_font
	);
}

static INT32 M_Sys_DrawProfileLegend(INT32 x, INT32 y, const char *legend, const char *mediocre_key)
{
	INT32 w = V_ThinStringWidth(legend, V_ALLOWLOWERCASE);

	V_DrawThinString(x - w, y, V_ALLOWLOWERCASE, legend);
	x -= w + 2;

	if (mediocre_key)
		TSoURDt3rd_M_DrawMediocreKeyboardKey(mediocre_key, &x, y, false, true);

	return x;
}

// the control stuff.
// Dear god.
static void M_Sys_DrawControls(void)
{
	const UINT8 spacing = 34;
	INT32 y = 16 - (optionsmenu.controlscroll*spacing);
	INT32 x = 8;
	INT32 i, j, k;
	INT32 hintofs = 3;

	V_DrawFill(0, -vid.height, 138, vid.height*2, 31); // Black border

	// Draw the menu options...
	for (i = 0; i < currentMenu->numitems; i++)
	{
		char buf[256];
		char buf2[256];
		INT32 keys[2];

		if (i == tsourdt3rd_itemOn)
		{
			// cursor
			for (j = 0; j < 24; j++)
				V_DrawFill(0, (y)+j, 128+j, 1, 73);
		}

		switch (currentMenu->menuitems[i].status & IT_DISPLAY)
		{
			case IT_HEADERTEXT:
				V_DrawFill(0, y+18, 124, 1, 0);	// underline
				V_DrawString(x, y+8, V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
				y += spacing;
				break;

			case IT_STRING:
				V_DrawThinString(x, y+2, (i == tsourdt3rd_itemOn ? tsourdt3rd_highlightflags : 0)|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
				y += spacing;
				break;

			case IT_STRING2:
			{
				boolean drawnpatch = false;

				if (currentMenu->menuitems[i].patch)
				{
					V_DrawScaledPatch(x-4, y+1, 0, W_CachePatchName(currentMenu->menuitems[i].patch, PU_CACHE));
					V_DrawThinString(x+12, y+2, (i == tsourdt3rd_itemOn ? tsourdt3rd_highlightflags : 0)|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);
					drawnpatch = true;
				}
				else
					V_DrawThinString(x, y+2, (i == tsourdt3rd_itemOn ? tsourdt3rd_highlightflags : 0)|V_ALLOWLOWERCASE, currentMenu->menuitems[i].text);

				if (currentMenu->menuitems[i].status & IT_CVAR)	// not the proper way to check but this menu only has normal onoff cvars.
				{
					consvar_t *cv = (consvar_t *)tsourdt3rd_currentMenu->menuitems[i].itemaction.cvar;
					INT32 w = V_StringWidth(cv->string, V_ALLOWLOWERCASE);

					V_DrawString(x + 12, y + 13, ((cv->flags & CV_CHEAT) && !CV_IsSetToDefault(cv) ? tsourdt3rd_warningflags : tsourdt3rd_highlightflags)|V_ALLOWLOWERCASE, cv->string);
					if (i == tsourdt3rd_itemOn)
					{
						V_DrawString(x - (tsourdt3rd_skullAnimCounter/5), y+12, tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1C"); // left arrow
						V_DrawString(x + 12 + w + 2 + (tsourdt3rd_skullAnimCounter/5) , y+13, tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, "\x1D"); // right arrow
					}
				}
				else if (currentMenu->menuitems[i].status & IT_CONTROL)
				{
					UINT32 vflags = 0;
					INT32 gc = tsourdt3rd_currentMenu->menuitems[i].mvar1;
					UINT8 available = 0, set = 0;
					char *p = NULL;

					if (i != tsourdt3rd_itemOn)
						vflags |= V_GRAYMAP;

					// Get userbound controls...
					for (k = 0; k < 2; k++)
					{
						keys[k] = optionsmenu.tempcontrols[gc][k];

						if (keys[k] == KEY_NULL)
							continue;

						set++;
						available++;
					};

					buf[0] = '\0';
					buf2[0] = '\0';
					p = buf;

					if (!set)
					{
						vflags &= ~V_CHARCOLORMASK;
						vflags |= V_REDMAP;
						strcpy(buf, "NOT BOUND");
					}
					else if (!buf[0])
					{
						for (k = 0; k < 2; k++)
						{
							if (keys[k] == KEY_NULL)
								continue;

							if (k > 0)
								strcat(p," / ");

							if (k == 2)   // hacky...
								p = buf2;

							strcat(p, G_KeyNumToName(keys[k]));
						}
					}

					INT32 bindx = x;
					INT32 benx = 142;
					INT32 beny = y - 8;
					if (i == tsourdt3rd_itemOn)
					{
						// Extend yellow wedge down behind the extra line.
						if (buf2[0])
						{
							for (j=24; j < 34; j++)
								V_DrawFill(0, (y)+j, 128+j, 1, 73);
							benx += 10;
							beny += 10;
						}

						// Scroll text into optionsmenu.bindben.
						bindx += optionsmenu.bindben * 3;

						if (buf2[0])
						{
							// Bind Ben: suck characters off
							// the end of the first line onto
							// the beginning of the second
							// line.
							UINT16 n = strlen(buf);
							UINT16 t = min(optionsmenu.bindben, n);
							memmove(&buf2[t], buf2, t + 1);
							memcpy(buf2, &buf[n - t], t);
							buf[n - t] = '\0';
						}
					}

					if (i != tsourdt3rd_itemOn || !optionsmenu.bindben_swallow)
					{
						// don't shift the text if we didn't draw a patch.
						V_DrawThinString(bindx + (drawnpatch ? 13 : 1), y + 12, vflags, buf);
						V_DrawThinString(bindx + (drawnpatch ? 13 : 1), y + 22, vflags, buf2);
					}
				}

				y += spacing;
				break;
			}
		}
	}

	// Tooltip
	// Draw it at the bottom of the screen
	{
		static UINT8 blue[256];
		INT32 xpos = BASEVIDWIDTH - 12;
		INT32 ypos = BASEVIDHEIGHT + hintofs - 9 - 12;

		blue[31] = 253;

		TSoURDt3rd_M_DrawMenuTooltips(
			0, BASEVIDHEIGHT/2+hintofs+72, V_SNAPTOBOTTOM, blue, true,
			12, ypos, V_ALLOWLOWERCASE|V_SNAPTOBOTTOM|V_MENUCOLORMAP, false
		);

		ypos -= (vid.dup+4)*4;
		xpos = (cv_usejoystick.value ?
			M_Sys_DrawProfileLegend(xpos, ypos, "Clear", "BKSP/Fire/Y/Joy3") :
			M_Sys_DrawProfileLegend(xpos, ypos, "Clear", "BKSP/Fire"));
	}

	// Overlay for control binding
	if (optionsmenu.bindtimer)
	{
		INT16 reversetimer = TICRATE*5 - optionsmenu.bindtimer;
		INT32 fade = reversetimer;
		INT32 ypos;

		if (fade > 9)
			fade = 9;

		ypos = (BASEVIDHEIGHT/2) - 20 +16*(9 - fade);
		V_DrawFadeScreen(31, fade);

		M_DrawTextBox((BASEVIDWIDTH/2) - (120), ypos - 12, 30, 8);

		V_DrawCenteredThinString(BASEVIDWIDTH/2, ypos, V_GRAYMAP|V_ALLOWLOWERCASE, "Hold and release inputs for");
		V_DrawCenteredThinString(BASEVIDWIDTH/2, ypos + 10, V_GRAYMAP|V_ALLOWLOWERCASE, va("\"%s\"", currentMenu->menuitems[tsourdt3rd_itemOn].text));

		if (optionsmenu.bindtimer > 0)
		{
			M_Sys_DrawBindMediumString(
				ypos + 50,
				tsourdt3rd_highlightflags|V_ALLOWLOWERCASE,
				va("(WAIT %d SEC TO SKIP)", (optionsmenu.bindtimer + (TICRATE-1)) / TICRATE)
			);
		}
		else
		{
			for (i = 0; i < 2 && optionsmenu.bindinputs[i]; ++i)
			{
				M_Sys_DrawBindMediumString(
					ypos + (2 + i)*10,
					tsourdt3rd_highlightflags,
					G_KeyNumToName(optionsmenu.bindinputs[i])
				);
			}
		}
	}
}

static boolean M_Sys_ClearCurrentControl(void)
{
	tsourdt3rd_menuitem_t *menuitem = NULL;

	// check if we're on a valid menu option...
	if (tsourdt3rd_currentMenu == NULL)
		return false;
	menuitem = &tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn];
	if (!menuitem || !menuitem->mvar1)
		return false;

	// since we are, clear controls for our key
	for (INT32 i = 0; i < 2; i++)
		optionsmenu.tempcontrols[menuitem->mvar1][i] = KEY_NULL;

	return true;
}

static void M_ProfileControlSaveResponse(INT32 choice)
{
	if (choice == MA_YES)
	{
		// Save the profile
		memcpy(gamecontrol, optionsmenu.tempcontrols, sizeof(optionsmenu.tempcontrols));
	}
	else
	{
		// Revert changes
		memcpy(optionsmenu.tempcontrols, gamecontrol, sizeof(gamecontrol));
	}
	TSoURDt3rd_M_GoBack(0);
}

static void M_Sys_ProfileControlsConfirm(INT32 choice)
{
	if (!memcmp(gamecontrol, optionsmenu.tempcontrols, sizeof(optionsmenu.tempcontrols)))
	{
		// no change
		TSoURDt3rd_M_GoBack(0);
		return;
	}

	if (choice == 0)
	{
		// Unsaved changes
		TSoURDt3rd_M_StartMessage(
			"Profiles",
			"You have unsaved changes to your controls.\n"
			"Please confirm if you wish to save them.\n",
			&M_ProfileControlSaveResponse,
			MM_YESNO,
			NULL,
			NULL
		);
		return;
	}

	// Success!
	M_ProfileControlSaveResponse(MA_YES);
}

static boolean M_Sys_ProfileControlsInputs(INT32 ch)
{
	const UINT8 pid = 0;
	(void)ch;

	if (optionsmenu.bindtimer)
	{
		// Eat all inputs there. We'll use a stupid hack in M_Responder instead.
		return true;
	}

	if (TSoURDt3rd_M_MenuExtraPressed(pid))
	{
		if (M_Sys_ClearCurrentControl())
			S_StartSound(NULL, sfx_adderr);
		optionsmenu.bindben = 0;
		optionsmenu.bindben_swallow = M_OPTIONS_BINDBEN_QUICK;
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		M_Sys_ProfileControlsConfirm(0);
		TSoURDt3rd_M_SetMenuDelay(pid);
		return true;
	}

	if (menucmd[pid].dpad_ud)
	{
		if (optionsmenu.bindben_swallow)
		{
			// Control would be cleared, but we're
			// interrupting the animation so clear it
			// immediately.
			M_Sys_ClearCurrentControl();
		}
		optionsmenu.bindben = 0;
		optionsmenu.bindben_swallow = 0;
	}

	return false;
}

static void M_Sys_HandleProfileControls(void)
{
	const UINT8 pid = 0;
	UINT8 maxscroll = currentMenu->numitems - 5;

	TSoURDt3rd_M_OptionsTick();

	optionsmenu.controlscroll = tsourdt3rd_itemOn - 3;	// very barebones scrolling, but it works just fine for our purpose.
	if (optionsmenu.controlscroll > maxscroll)
		optionsmenu.controlscroll = maxscroll;

	if (optionsmenu.controlscroll < 0)
		optionsmenu.controlscroll = 0;

	// bindings, cancel if timer is depleted.
	if (optionsmenu.bindtimer)
	{
		if (optionsmenu.bindtimer > 0)
			optionsmenu.bindtimer--;
	}
	else if (tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1) // check if we're on a valid menu option...
	{
		// Hold right to begin clearing the control.
		//
		// If bindben timer increases enough, bindben_swallow
		// will be set.
		// This is a commitment to clear the control.
		// You can keep holding right to postpone the clear
		// but once you let go, you are locked out of
		// pressing it again until the animation finishes.
		if (menucmd[pid].dpad_lr > 0 && (optionsmenu.bindben || !optionsmenu.bindben_swallow))
		{
			optionsmenu.bindben++;
		}
		else
		{
			optionsmenu.bindben = 0;

			if (optionsmenu.bindben_swallow)
			{
				optionsmenu.bindben_swallow--;

				if (optionsmenu.bindben_swallow == 100) // special countdown for the "quick" animation
					optionsmenu.bindben_swallow = 0;
				else if (!optionsmenu.bindben_swallow) // long animation, clears control when done
					M_Sys_ClearCurrentControl();
			}
		}
	}
}

static void M_Sys_SetControl(INT32 ch)
{
	(void)ch;

	optionsmenu.bindtimer = TICRATE*5;
	memset(optionsmenu.bindinputs, 0, sizeof optionsmenu.bindinputs);
}

#define KEYHOLDFOR 1
// Map the event to the profile.
void TSoURDt3rd_M_Controls_MapProfileControl(event_t *ev)
{
	const UINT8 pid = 0;
	boolean noinput = true;
	INT32 controln = 0;

	if (ev->type == ev_keydown && ev->repeated)
	{
		// ignore repeating keys
		return;
	}

	if (optionsmenu.bindtimer > TICRATE*5 - 9)
	{
		// grace period after entering the bind dialog
		return;
	}

	// Find every held button.
	for (INT32 c = 1; c < NUMINPUTS; ++c)
	{
		if (gamekeydown[c] <= 0)
			continue;
		noinput = false;

		for (UINT8 i = 0; i < 2; ++i)
		{
			// If this key is already bound, don't bind it again.
			if (optionsmenu.bindinputs[i] == c)
				break;

			// Find the first available slot.
			if (!optionsmenu.bindinputs[i])
			{
				optionsmenu.bindinputs[i] = c;
				break;
			}
		}
	}

	if (noinput)
	{
		{
			// You can hold a button before entering this
			// dialog, then buffer a keyup without pressing
			// anything else. If this happens, don't wipe the
			// binds, just ignore it.
			const UINT8 zero[sizeof optionsmenu.bindinputs] = {0};
			if (!memcmp(zero, optionsmenu.bindinputs, sizeof zero))
				return;
		}

		controln = tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar1;
		memcpy(&optionsmenu.tempcontrols[controln], optionsmenu.bindinputs, sizeof(optionsmenu.bindinputs));
		optionsmenu.bindtimer = 0;

		// Set menu delay regardless of what we're doing to avoid stupid stuff.
		TSoURDt3rd_M_SetMenuDelay(pid);
	}
	else
		optionsmenu.bindtimer = -1; // prevent skip countdown
}
#undef KEYHOLDFOR
