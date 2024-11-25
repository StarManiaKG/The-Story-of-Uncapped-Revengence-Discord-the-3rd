// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-tsourdt3rd-readme.c
/// \brief TSoURDt3rd's cool little doom referencing credits easter egg.

#include "../smkg-m_sys.h"

// ------------------------ //
//        Variables
// ------------------------ //

static boolean checked_readme_text = false;
static fixed_t cur_readme_y = 12<<FRACBITS;
static fixed_t min_readme_y = 12<<FRACBITS;
static fixed_t max_readme_y = -(24<<FRACBITS); // set when the menu first loads

static const char *TSoURDt3rd_credits[] = {
	"\1TSoURDt3rd Team",
	"StarManiaKG \"Star\"",
		"\t\t- Creator",
	"",
	"MarioMario \"Sapphire\"",
		"\t\t- Co-Creator",
		"\t\t- In Loving Memory Of",
	"",
	"Mini the Bunnyboy \"Talis\"",
		"\t\t- Co-Develtoper",
	"",
	"Bitten2Up \"Bitten\"",
		"\t\t- Co-Develtoper",
	"",
	"\1TSoURDt3rd Extras",
	"Marilyn - Emotional Support, Ideas",
		"\t\t- Emotional support",
		"\t\t- Ideas",
		"\t\t- Tester",
	"",
	"OVAPico & Other Gamer Gang GC Members",
		"\t\t- Voluntary testers, ideas",
		"\t\t- Emotional support",
	"",
	"NARBluebear",
		"\t\t- Best Friend",
		"\t\t- Emotional support",
	"",
	"\"Future\" Smiles \"The Fox\"",
		"\t\t- Best Friend",
		"\t\t- Emotional support",
	"",
	"Smash Studios",
		"\t\t- Emotional support",
		"\t\t- Coding skill improvement",
	"",
	"Flashback Guy \"Flash\"",
		"\t\t- SRB2 emblem world record",
		"\t\t- Ideas",
	"",
	"Sling",
		"\t\t- Emotional support",
		"\t\t- Voluntary tester, ideas",
	NULL
};

static void M_Sys_InitReadME(void);
static void M_Sys_DrawReadME(void);
static boolean M_Sys_HandleReadME(INT32 choice);

menuitem_t TSoURDt3rd_OP_ReadMEMenu[] =
{
	{IT_NOTHING, NULL, NULL, NULL, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_TM_OP_ReadMEMenu[] =
{
	{NULL, "Hey, if you're seeing this, play Penny's Big Breakaway!", {NULL}, 0, 0},
};

menu_t TSoURDt3rd_OP_ReadMEDef =
{
	MTREE3(MN_OP_MAIN, MN_OP_TSOURDT3RD, MN_TSOURDT3RD_README),
	NULL,
	sizeof (TSoURDt3rd_OP_ReadMEMenu)/sizeof (menuitem_t),
	&TSoURDt3rd_OP_MainMenuDef,
	TSoURDt3rd_OP_ReadMEMenu,
	M_Sys_DrawReadME,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t TSoURDt3rd_TM_OP_ReadMEDef = {
	TSoURDt3rd_TM_OP_ReadMEMenu,
	0, 0,
	0,
	NULL,
	0, 0,
	NULL,
	NULL,
	M_Sys_InitReadME,
	NULL,
	M_Sys_HandleReadME,
	&TSoURDt3rd_TM_OP_MainMenuDef
};

// ------------------------ //
//        Functions
// ------------------------ //

static void M_Sys_InitReadME(void)
{
	if (checked_readme_text)
	{
		// We've already done everything necessary.
		return;
	}

	// Calculate the max height we can scroll.
	for (UINT16 i = 0; TSoURDt3rd_credits[i]; i++)
	{
		switch (TSoURDt3rd_credits[i][0])
		{
			case 1:
				max_readme_y -= 8<<FRACBITS;
				break;
			default:
				max_readme_y -= 4<<FRACBITS;
				break;
		}
		if (FixedMul(max_readme_y, vid.dupy) > vid.height)
			break;
	}

	checked_readme_text = true;
}

static void M_Sys_DrawReadME(void)
{
	fixed_t y = cur_readme_y;
	INT32 up_flags = 0, down_flags = 0;

	V_DrawFadeScreen(0xFF00, 16); // I wanted a cool fade effect here

	if (cur_readme_y < min_readme_y)
		up_flags |= V_MENUCOLORMAP;
	else
		up_flags |= V_80TRANS;

	if (cur_readme_y > max_readme_y)
		down_flags |= V_MENUCOLORMAP;
	else
		down_flags |= V_80TRANS;

	for (UINT16 i = 0; TSoURDt3rd_credits[i]; i++)
	{
		switch (TSoURDt3rd_credits[i][0])
		{
			case 1:
				V_DrawCreditString((160 - (V_CreditStringWidth(&TSoURDt3rd_credits[i][1])>>1))<<FRACBITS, y, 0, &TSoURDt3rd_credits[i][1]);
				y += 24<<FRACBITS;
				break;
			default:
				V_DrawStringAtFixed(26<<FRACBITS, y, V_ALLOWLOWERCASE, TSoURDt3rd_credits[i]);
				y += 8<<FRACBITS;
				break;
		}
		if (FixedMul(y, vid.dupy) > vid.height)
			break;
	}

	V_DrawCharacter(2, y+15, '\x1A' | V_SNAPTOLEFT | up_flags, false); // Up arrow
	V_DrawCharacter(2, y+180, '\x1B' | V_SNAPTOLEFT | down_flags, false); // Down arrow
}

static boolean M_Sys_HandleReadME(INT32 choice)
{
	const UINT8 pid = 0;

	(void)choice;
	if (menucmd[pid].dpad_ud < 0)
	{
		// Up
		if (cur_readme_y < min_readme_y)
		{
			cur_readme_y += 8<<FRACBITS;
			S_StartSound(NULL, sfx_menu1);
		}
		return true;
	}
	else if (menucmd[pid].dpad_ud > 0)
	{
		// Down
		if (cur_readme_y > max_readme_y)
		{
			cur_readme_y -= 8<<FRACBITS;
			S_StartSound(NULL, sfx_menu1);
		}
		return true;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		// Close the menu.
		cur_readme_y = 12<<FRACBITS;
	}

	return false;
}
