// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Vivian "toastergrl" Grannell.
// Copyright (C) 2024 by Kart Krew.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/transient/drrr-virtual-keyboard.c
/// \brief Keyboard input

#include "../../smkg-m_sys.h"

#include "../../../smkg-cvars.h"

#include "../../../../s_sound.h"
#include "../../../../i_system.h" // I_Clipboard funcs
#include "../../../../z_zone.h"
#include "../../../../g_input.h"

// ------------------------ //
//        Variables
// ------------------------ //

// Typing "sub"-menu
struct menutyping_s menutyping;

// keyboard layouts
INT16 virtualKeyboard[5][NUMVIRTUALKEYSINROW] = {

	{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',  KEY_BACKSPACE, 1},
	{'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',  '-', '='},
	{'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '/',  '[', ']'},
	{'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '\\', ';', '\''},
	{KEY_RSHIFT, 1, 1, KEY_SPACE, 1, 1, 1, 1, KEY_ENTER, 1, 1, 1}
};

INT16 shift_virtualKeyboard[5][NUMVIRTUALKEYSINROW] = {

	{'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', KEY_BACKSPACE, 1},
	{'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '_', '+'},
	{'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '?', '{', '}'},
	{'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '|', ':', '\"'},
	{KEY_RSHIFT, 1, 1, KEY_SPACE, 1, 1, 1, 1, KEY_ENTER, 1, 1, 1}
};

typedef enum
{
	CVCPM_NONE,
	CVCPM_COPY,
	CVCPM_CUT,
	CVCPM_PASTE
} cvarcopypastemode_t;

// ------------------------ //
//        Functions
// ------------------------ //

static INT32 CON_ShiftChar(INT32 ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		// Standard Latin-script uppercase translation
		if (shiftdown ^ capslock)
			ch = shiftxform[ch];
	}
	else if (ch >= KEY_KEYPAD7 && ch <= KEY_KPADDEL)
	{
		// Numpad keycodes mapped to printable equivalent
		const char keypad_translation[] =
		{
			'7','8','9','-',
			'4','5','6','+',
			'1','2','3',
			'0','.'
		};

		ch = keypad_translation[ch - KEY_KEYPAD7];
	}
	else if (ch == KEY_KPADSLASH)
	{
		// Ditto, but non-contiguous keycode
		ch = '/';
	}
	else
	{
		// QWERTY keycode translation
		if (shiftdown)
			ch = shiftxform[ch];
	}

	return ch;
}

static boolean M_ChangeStringCvar(INT32 choice)
{
	size_t len;
	cvarcopypastemode_t copypastemode = CVCPM_NONE;

	if (menutyping.keyboardtyping == true)
	{
		// We can only use global modifiers in key mode.

		if (ctrldown)
		{
			if (choice == 'c' || choice == 'C' || choice == KEY_INS)
			{
				// ctrl+c, ctrl+insert, copying
				copypastemode = CVCPM_COPY;
			}
			else if (choice == 'x' || choice == 'X')
			{
				// ctrl+x, cutting
				copypastemode = CVCPM_CUT;
			}
			else if (choice == 'v' || choice == 'V')
			{
				// ctrl+v, pasting
				copypastemode = CVCPM_PASTE;
			}
			else
			{
				// not a known ctrl code
				return false;
			}
		}
		else if (shiftdown)
		{
			if (choice == KEY_INS)
			{
				// shift+insert, pasting
				copypastemode = CVCPM_PASTE;
			}
			else if (choice == KEY_DEL)
			{
				// shift+delete, cutting
				copypastemode = CVCPM_CUT;
			}
		}

		if (copypastemode != CVCPM_NONE)
		{
			len = strlen(menutyping.cache);

			if (copypastemode == CVCPM_PASTE)
			{
				const char *paste = I_ClipboardPaste();
				if (paste == NULL || paste[0] == '\0')
					;
				else if (len < menutyping.cachelen)
				{
					strlcat(menutyping.cache, paste, menutyping.cachelen + 1);

					S_StartSoundFromEverywhere(sfx_tmxbdn); // Tails
				}
			}
			else if (len > 0 /*&& (copypastemode == CVCPM_COPY
				|| copypastemode == CVCPM_CUT)*/
				)
			{
				I_ClipboardCopy(menutyping.cache, len);

				if (copypastemode == CVCPM_CUT)
				{
					// A cut should wipe.
					strcpy(menutyping.cache, "");
					S_StartSoundFromEverywhere(sfx_tmxbup); // Tails
				}
				else
				{
					S_StartSoundFromEverywhere(sfx_tmxbdn); // Tails
				}
			}

			return true;
		}

		// Okay, now we can auto-modify the character.
		choice = CON_ShiftChar(choice);
	}

	switch (choice)
	{
		case KEY_BACKSPACE:
			if (menutyping.cache[0])
			{
				len = strlen(menutyping.cache);
				menutyping.cache[len - 1] = 0;

				S_StartSoundFromEverywhere(sfx_tmxbup); // Tails
			}
			return true;
		case KEY_DEL:
			if (menutyping.cache[0])
			{
				strcpy(menutyping.cache, "");

				S_StartSoundFromEverywhere(sfx_tmxbup); // Tails
			}
			return true;
		default:
			if (choice >= 32 && choice <= 127)
			{
				len = strlen(menutyping.cache);
				if (len < menutyping.cachelen)
				{
					menutyping.cache[len++] = (char)choice;
					menutyping.cache[len] = 0;

					S_StartSoundFromEverywhere(sfx_tmxbdn); // Tails
				}
				return true;
			}
			break;
	}

	return false;
}

static const char *M_MenuTypingCroppedString(void)
{
	static char buf[36];
	const char *p = menutyping.cache;
	size_t n = strlen(p);
	if (n > sizeof buf)
	{
		p += n - sizeof buf;
		n = sizeof buf;
	}
	memcpy(buf, p, n);
	buf[n] = '\0';
	return buf;
}

// Determine if cache is past string length based on mvar2
boolean TSoURDt3rd_M_VirtualStringMeetsLength(void)
{
	size_t len, min_len = 0;

	if (!menutyping.active)
		return false;

	len = strlen(menutyping.cache);
	if (tsourdt3rd_currentMenu != NULL && tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar2)
		min_len = tsourdt3rd_currentMenu->menuitems[tsourdt3rd_itemOn].mvar2;

	return (len >= min_len);
}

static void M_ToggleVirtualShift(void)
{
	if (menutyping.keyboardcapslock == true)
	{
		menutyping.keyboardcapslock = false;
	}
	else
	{
		menutyping.keyboardshift ^= true;
		if (menutyping.keyboardshift == false)
		{
			menutyping.keyboardcapslock = true;
		}
	}
}

static void M_CloseVirtualKeyboard(void)
{
	menutyping.menutypingclose = true;	// close menu.
	if (!TSoURDt3rd_M_VirtualStringMeetsLength())
		return;
	menutyping.queryfn(menutyping.cache);
}

void TSoURDt3rd_M_AbortVirtualKeyboard(void)
{
	if (!menutyping.active)
		return;

	menutyping.active = false;
	menutyping.menutypingfade = 0;
	Z_Free(menutyping.cache);

	if (currentMenu == menutyping.dummymenu)
		TSoURDt3rd_M_GoBack(0);
}

void TSoURDt3rd_M_MenuTypingInput(INT32 key)
{
	const UINT8 pid = 0;

	// Determine when to check for keyboard inputs or controller inputs using menuKey, which is the key passed here as argument.
	if (key > 0)
	{
		boolean gamepad = (key >= NUMKEYS);
		TSoURDt3rd_M_SwitchVirtualKeyboard(gamepad);
		if (gamepad)
			return;
	}

	if (!menutyping.active)
		return;

	// Fade-in

	if (menutyping.menutypingclose)
	{
		// Closing
		menutyping.menutypingfade--;
		if (!menutyping.menutypingfade)
			TSoURDt3rd_M_AbortVirtualKeyboard();

		return;	// prevent inputs while closing the menu.
	}
	else
	{
		// Opening
		const UINT8 destination = (menutyping.keyboardtyping ? 9 : 18);

		if (menutyping.menutypingfade > destination)
		{
			menutyping.menutypingfade--;
		}
		else if (menutyping.menutypingfade < destination)
		{
			menutyping.menutypingfade++;
		}

		if (menutyping.menutypingfade >= 9) // either is visible
		{
			if (key == KEY_ENTER || key == KEY_ESCAPE)
			{
				M_CloseVirtualKeyboard();

				TSoURDt3rd_M_SetMenuDelay(pid);
				S_StartSoundFromEverywhere(sfx_s3k5b);

				return;
			}

			if (menutyping.keyboardtyping)
			{
				M_ChangeStringCvar(key);
				return;
			}
		}

		if (menutyping.menutypingfade != destination)
		{
			// Don't allow typing until it's fully opened.
			return;
		}
	}

	if (menucmd[pid].delay == 0 && !menutyping.keyboardtyping)	// We must check for this here because we bypass the normal delay check to allow for normal keyboard inputs
	{
		if (menucmd[pid].dpad_ud > 0)	// down
		{
			menutyping.keyboardy++;
			if (menutyping.keyboardy > 4)
				menutyping.keyboardy = 0;

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (menucmd[pid].dpad_ud < 0) // up
		{
			menutyping.keyboardy--;
			if (menutyping.keyboardy < 0)
				menutyping.keyboardy = 4;

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (menucmd[pid].dpad_lr > 0)	// right
		{
			do
			{
				menutyping.keyboardx++;
				if (menutyping.keyboardx > NUMVIRTUALKEYSINROW-1)
				{
					menutyping.keyboardx = 0;
					break;
				}
			}
			while (virtualKeyboard[menutyping.keyboardy][menutyping.keyboardx] == 1);

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (menucmd[pid].dpad_lr < 0)	// left
		{
			while (virtualKeyboard[menutyping.keyboardy][menutyping.keyboardx] == 1)
			{
				menutyping.keyboardx--;
				if (menutyping.keyboardx < 0)
				{
					menutyping.keyboardx = NUMVIRTUALKEYSINROW-1;
					break;
				}
			}

			menutyping.keyboardx--;
			if (menutyping.keyboardx < 0)
			{
				menutyping.keyboardx = NUMVIRTUALKEYSINROW-1;
			}

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuButtonPressed(pid, MBT_START))
		{
			// Shortcut for close menu.
			M_CloseVirtualKeyboard();

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuBackPressed(pid))
		{
			// Shortcut for backspace.
			M_ChangeStringCvar(KEY_BACKSPACE);

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuExtraPressed(pid))
		{
			// Shortcut for shift/caps lock.
			M_ToggleVirtualShift();

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSoundFromEverywhere(sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuConfirmPressed(pid))
		{
			// Add the character. First though, check what we're pressing....
			INT32 tempkeyboardx = menutyping.keyboardx;
			INT16 c = 0;
			while ((c = virtualKeyboard[menutyping.keyboardy][tempkeyboardx]) == 1
			&& tempkeyboardx > 0)
				tempkeyboardx--;

			if (c > 1)
			{
				if (menutyping.keyboardshift ^ menutyping.keyboardcapslock)
					c = shift_virtualKeyboard[menutyping.keyboardy][tempkeyboardx];

				if (c == KEY_RSHIFT)
				{
					M_ToggleVirtualShift();
				}
				else if (c == KEY_ENTER)
				{
					M_CloseVirtualKeyboard();
				}
				else
				{
					M_ChangeStringCvar((INT32)c);	// Write!
					menutyping.keyboardshift = false;			// undo shift if it had been pressed
				}

				TSoURDt3rd_M_SetMenuDelay(pid);
				S_StartSoundFromEverywhere(sfx_s3k5b);
			}
		}
	}
}

void TSoURDt3rd_M_OpenVirtualKeyboard(size_t cachelen, vkb_query_fn_t queryfn, menu_t *dummymenu)
{
	menutyping.active = true;
	menutyping.menutypingclose = false;

	menutyping.queryfn = queryfn;
	menutyping.dummymenu = dummymenu;
	menutyping.cachelen = cachelen;
	Z_Malloc(cachelen + 1, PU_STATIC, &menutyping.cache);
	strlcpy(menutyping.cache, queryfn(NULL), cachelen + 1);

	if (dummymenu)
	{
		if (!menuactive)
		{
			M_StartControlPanel();
			dummymenu->prevMenu = NULL;
		}
		else
			dummymenu->prevMenu = currentMenu;

		M_SetupNextMenu(dummymenu);
	}
}

void TSoURDt3rd_M_SwitchVirtualKeyboard(boolean gamepad)
{
	menutyping.keyboardtyping = cv_tsourdt3rd_debug_drrr_virtualkeyboard.value ? false : !gamepad;
}

// Draws the typing submenu
void TSoURDt3rd_M_DrawMenuTyping(void)
{
	const UINT8 pid = 0;

	INT32 i, j;

	INT32 x, y;

	char buf[8];	// We write there to use drawstring for convenience.

	V_DrawFadeScreen(31, (menutyping.menutypingfade+1)/2);

	// Draw the string we're editing at the top.

	const INT32 boxwidth = (8*(MAXSTRINGLENGTH + 1)) + 7;
	x = (BASEVIDWIDTH - boxwidth)/2;
	y = 80;
	if (menutyping.menutypingfade < 9)
		y += floor(pow(2, (double)(9 - menutyping.menutypingfade)));
	else
		y += (9-menutyping.menutypingfade);

	if (currentMenu->menuitems[tsourdt3rd_itemOn].text)
	{
		V_DrawThinString(x + 5, y - 2, tsourdt3rd_highlightflags|V_ALLOWLOWERCASE, currentMenu->menuitems[tsourdt3rd_itemOn].text);
	}

	TSoURDt3rd_M_DrawMenuTooltips(
		0, 0, 0, NULL, false,
		BASEVIDWIDTH/2, 13, 0, true
	);

	//M_DrawTextBox(x, y + 4, MAXSTRINGLENGTH, 1);
	V_DrawFill(x + 5, y + 4 + 5, boxwidth - 8, 8+6, 159);

	V_DrawFill(x + 4, y + 4 + 4, boxwidth - 6, 1, 121);
	V_DrawFill(x + 4, y + 4 + 5 + 8 + 6, boxwidth - 6, 1, 121);

	V_DrawFill(x + 4, y + 4 + 5, 1, 8+6, 121);
	V_DrawFill(x + 5 + boxwidth - 8, y + 4 + 5, 1, 8+6, 121);

	INT32 textwidth = TSoURDt3rd_M_DrawCaretString(
		x + 8, y + 12, V_ALLOWLOWERCASE,
		FRACUNIT, FRACUNIT,
		M_MenuTypingCroppedString(), tny_font
	);
	if (tsourdt3rd_skullAnimCounter < 4
		&& menutyping.menutypingclose == false
		&& menutyping.menutypingfade == (menutyping.keyboardtyping ? 9 : 18))
	{
		V_DrawCharacter(x + 8 + textwidth, y + 12 + 1, '_', false);
	}

	const INT32 buttonwidth = ((boxwidth + 1)/NUMVIRTUALKEYSINROW);
#define BUTTONHEIGHT (11)

	// Now the keyboard itself
	x += 5;
	INT32 returnx = x;

	if (menutyping.menutypingfade > 9)
	{
		y += 26;

		if (menutyping.menutypingfade < 18)
		{
			y += floor(pow(2, (double)(18 - menutyping.menutypingfade))); // double yoffs for animation
		}

		INT32 tempkeyboardx = menutyping.keyboardx;

		while (virtualKeyboard[menutyping.keyboardy][tempkeyboardx] == 1
		&& tempkeyboardx > 0)
			tempkeyboardx--;

		for (i = 0; i < 5; i++)
		{
			j = 0;
			while (j < NUMVIRTUALKEYSINROW)
			{
				INT32 mflag = 0;
				INT16 c = virtualKeyboard[i][j];

				INT32 buttonspacing = 1;

				UINT8 col = 27;

				INT32 arrowoffset = 0;

				while (j + buttonspacing < NUMVIRTUALKEYSINROW
				&& virtualKeyboard[i][j + buttonspacing] == 1)
				{
					buttonspacing++;
				}

				if (menutyping.keyboardshift ^ menutyping.keyboardcapslock)
					c = shift_virtualKeyboard[i][j];

				if (i < 4 && j < NUMVIRTUALKEYSINROW-2)
				{
					col = 25;
				}

				boolean canmodifycol = (menutyping.menutypingfade == 18);

				if (c == KEY_BACKSPACE)
				{
					arrowoffset = 1;
					buf[0] = '\x1C'; // left arrow
					buf[1] = '\0';

					if (canmodifycol && TSoURDt3rd_M_MenuBackHeld(pid))
					{
						col -= 4;
						canmodifycol = false;
					}
				}
				else if (c == KEY_RSHIFT)
				{
					arrowoffset = 2;
					buf[0] = '\x1A'; // up arrow
					buf[1] = '\0';

					if (menutyping.keyboardcapslock || menutyping.keyboardshift)
					{
						col = 22;
					}

					if (canmodifycol && TSoURDt3rd_M_MenuExtraHeld(pid))
					{
						col -= 4;
						canmodifycol = false;
					}
				}
				else if (c == KEY_ENTER)
				{
					strcpy(buf, "OK");

					if (menutyping.menutypingclose)
					{
						col -= 4;
						canmodifycol = false;
					}
				}
				else if (c == KEY_SPACE)
				{
					strcpy(buf, "Space");
				}
				else
				{
					buf[0] = c;
					buf[1] = '\0';
				}

				INT32 width = (buttonwidth * buttonspacing) - 1;

				// highlight:
				/*if (menutyping.keyboardtyping)
				{
					mflag |= V_TRANSLUCENT;	// grey it out if we can't use it.
				}
				else*/
				{
					if (tempkeyboardx == j && menutyping.keyboardy == i)
					{
						if (canmodifycol && TSoURDt3rd_M_MenuConfirmHeld(pid))
						{
							col -= 4;
							canmodifycol = false;
						}

						V_DrawFill(x + 1, y + 1, width - 2, BUTTONHEIGHT - 2, col - 3);

						V_DrawFill(x, y,                    width, 1, 121);
						V_DrawFill(x, y + BUTTONHEIGHT - 1, width, 1, 121);

						V_DrawFill(x,             y + 1, 1, BUTTONHEIGHT - 2, 121);
						V_DrawFill(x + width - 1, y + 1, 1, BUTTONHEIGHT - 2, 121);

						mflag |= tsourdt3rd_highlightflags;
					}
					else
					{
						V_DrawFill(x, y, width, BUTTONHEIGHT, col);
					}
				}

				if (arrowoffset != 0)
				{
					if (c == KEY_RSHIFT)
					{
						V_DrawFill(x + width - 5, y + 1, 4, 4, 31);

						if (menutyping.keyboardcapslock)
						{
							V_DrawFill(x + width - 4, y + 2, 2, 2, 121);
						}
					}

					V_DrawCenteredString(x + (width/2), y + 1 + arrowoffset, mflag|V_ALLOWLOWERCASE, buf);
				}
				else
				{
					V_DrawCenteredThinString(x + (width/2), y + 1, mflag|V_ALLOWLOWERCASE, buf);
				}

				x += width + 1;
				j += buttonspacing;
			}
			x = returnx;
			y += BUTTONHEIGHT + 1;
		}
	}

#undef BUTTONHEIGHT

	y = 187;

	if (menutyping.menutypingfade < 9)
	{
		y += 3 * (9 - menutyping.menutypingfade);
	}

	// Some contextual stuff
	if (menutyping.keyboardtyping)
	{
		V_DrawThinString(returnx, y, V_GRAYMAP|V_ALLOWLOWERCASE,
			"Type using your keyboard. Press Enter to confirm & exit."
			//"\nPress any button on your controller to use the Virtual Keyboard."
		);
	}
	else
	{
		V_DrawThinString(returnx, y, V_GRAYMAP|V_ALLOWLOWERCASE,
			"Type using the Virtual Keyboard. Use the \'OK\' button to confirm & exit."
			//"\nPress any keyboard key to type normally."
		);
	}
}
