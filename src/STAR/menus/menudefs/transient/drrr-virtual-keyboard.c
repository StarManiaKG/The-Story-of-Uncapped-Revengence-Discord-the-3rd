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

#include "../../smkg_m_func.h"
#include "../../../smkg-cvars.h"

#include "../../../drrr/k_menu.h" // menutyping //

#include "../../../../s_sound.h"
#include "../../../../i_system.h" // I_Clipboard funcs
#include "../../../../z_zone.h"
#include "../../../../g_input.h"

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

					S_StartSound(NULL, sfx_tmxbdn); // Tails
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
					S_StartSound(NULL, sfx_tmxbup); // Tails
				}
				else
				{
					S_StartSound(NULL, sfx_tmxbdn); // Tails
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

				S_StartSound(NULL, sfx_tmxbup); // Tails
			}
			return true;
		case KEY_DEL:
			if (menutyping.cache[0])
			{
				strcpy(menutyping.cache, "");

				S_StartSound(NULL, sfx_tmxbup); // Tails
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

					S_StartSound(NULL, sfx_tmxbdn); // Tails
				}
				return true;
			}
			break;
	}

	return false;
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

void M_AbortVirtualKeyboard(void)
{
	if (!menutyping.active)
		return;

	menutyping.active = false;
	menutyping.menutypingfade = 0;
	Z_Free(menutyping.cache);

	if (currentMenu == menutyping.dummymenu)
		STAR_M_GoBack(0);
}

void M_MenuTypingInput(INT32 key)
{
	const UINT8 pid = 0;

	// Determine when to check for keyboard inputs or controller inputs using menuKey, which is the key passed here as argument.
	if (key > 0)
	{
		boolean gamepad = (key >= NUMKEYS);
		M_SwitchVirtualKeyboard(gamepad);
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
			M_AbortVirtualKeyboard();

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
				S_StartSound(NULL, sfx_s3k5b);

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
			S_StartSound(NULL, sfx_s3k5b);
		}
		else if (menucmd[pid].dpad_ud < 0) // up
		{
			menutyping.keyboardy--;
			if (menutyping.keyboardy < 0)
				menutyping.keyboardy = 4;

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSound(NULL, sfx_s3k5b);
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
			S_StartSound(NULL, sfx_s3k5b);
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
			S_StartSound(NULL, sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuButtonPressed(pid, MBT_START))
		{
			// Shortcut for close menu.
			M_CloseVirtualKeyboard();

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSound(NULL, sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuBackPressed(pid))
		{
			// Shortcut for backspace.
			M_ChangeStringCvar(KEY_BACKSPACE);

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSound(NULL, sfx_s3k5b);
		}
		else if (TSoURDt3rd_M_MenuExtraPressed(pid))
		{
			// Shortcut for shift/caps lock.
			M_ToggleVirtualShift();

			TSoURDt3rd_M_SetMenuDelay(pid);
			S_StartSound(NULL, sfx_s3k5b);
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
				S_StartSound(NULL, sfx_s3k5b);
			}
		}
	}
}

void M_OpenVirtualKeyboard(size_t cachelen, vkb_query_fn_t queryfn, menu_t *dummymenu)
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

void M_SwitchVirtualKeyboard(boolean gamepad)
{
	menutyping.keyboardtyping = cv_tsourdt3rd_debug_drrr_virtualkeyboard.value ? false : !gamepad;
}
