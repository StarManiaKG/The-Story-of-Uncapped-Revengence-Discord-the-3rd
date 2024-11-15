// SONIC ROBO BLAST 2; TSOURDT3RD
// PORTED FROM DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2024 by Kart Krew.
// Copyright (C) 2016 by Kay "Kaito" Sinclaire.
// Copyright (C) 2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  km_menudraw.c
/// \brief SRB2Kart's menu drawer functions

#include "k_menu.h"

#include "../../m_fixed.h"
#include "../../w_wad.h"
#include "../../v_video.h"
#include "../../z_zone.h"

#include "../smkg_g_inputs.h"

#include "../menus/smkg-m_sys.h"
#include "../netcode/drrr-m_swap.h"

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

// Draws the typing submenu
void M_DrawMenuTyping(void)
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

void TSoURDt3rd_M_DrawMediocreKeyboardKey(const char *text, INT32 *workx, INT32 worky, boolean push, boolean rightaligned)
{
	INT32 buttonwidth = V_StringWidth(text, 0) + 2;

	if (rightaligned)
	{
		(*workx) -= buttonwidth;
	}

	if (push)
	{
		worky += 2;
	}
	else
	{
		V_DrawFill((*workx)-1, worky+10, buttonwidth, 2, 24);
	}

	V_DrawFill((*workx)-1, worky, buttonwidth, 10, 16);
	V_DrawString(
		(*workx), worky + 1,
		V_ALLOWLOWERCASE, text
	);
}
