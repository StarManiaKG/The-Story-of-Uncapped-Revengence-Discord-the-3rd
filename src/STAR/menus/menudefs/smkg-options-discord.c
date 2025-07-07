// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-options-discord.c
/// \brief Discord integration menu options

#ifdef HAVE_DISCORDSUPPORT
#include "../../../discord/discord.h"
#include "../smkg-m_sys.h"

#include "../../smkg-st_hud.h"

#include "../../../r_draw.h"
#include "../../../z_zone.h"

// ------------------------ //
//        Variables
// ------------------------ //

struct discordrequestmenu_s discordrequestmenu;

static void M_Sys_DrawDiscordMenu(void);
static void M_Sys_DiscordOptionsTicker(void);

static void M_Sys_DrawDiscordRequests(void);
static void M_Sys_DiscordRequestTick(void);
static void M_Sys_DiscordRequestHandler(INT32 choice);

menuitem_t DISCORD_OP_MainMenu[] =
{
	{IT_STRING | IT_CVAR, NULL, "Rich Presence",
		&cv_discordrp, 0},
	{IT_STRING | IT_CVAR, NULL, "Streamer Mode",
		&cv_discordstreamer, 0},

	{IT_SPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Rich Presence Settings...", NULL, 0},
		{IT_STRING | IT_CVAR, NULL, "Allow Ask to Join",
			&cv_discordasks, 0},
		{IT_STRING | IT_CVAR, NULL, "Allow Invites",
			&cv_discordinvites, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Show on Status",
			&cv_discordshowonstatus, 0},
		{IT_STRING | IT_CVAR, NULL, "Memes on Status",
			&cv_discordstatusmemes,	0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Skin Image Type",
			&cv_discordcharacterimagetype, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Custom Presence String...", NULL, 0},
		{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "Edit Details...",
			&cv_discordcustom_details, 0},
		{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "Edit State...",
			&cv_discordcustom_state, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Custom Presence Large Image...", NULL, 0},
		{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "Edit Image Text...",
			&cv_discordcustom_imagetext_large, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Image Type",
			&cv_discordcustom_imagetype_large, 0},
		{IT_STRING | IT_CVAR, NULL, "Image", // Handled by the menu ticker
			NULL, 0},

	{IT_SPACE | IT_DYBIGSPACE, NULL, NULL,
		NULL, 0},

	{IT_HEADER, NULL, "Custom Presence Small Image...", NULL, 0},
		{IT_STRING | IT_CVAR | IT_CV_STRING, NULL, "Edit Image Text...",
			&cv_discordcustom_imagetext_small, 0},

		{IT_SPACE, NULL, NULL,
			NULL, 0},

		{IT_STRING | IT_CVAR, NULL, "Image Type",
			&cv_discordcustom_imagetype_small, 0},
		{IT_STRING | IT_CVAR, NULL, "Image", // Handled by the menu ticker
			NULL, 0},
};

tsourdt3rd_menuitem_t DISCORD_TM_OP_MainMenu[] =
{
	{NULL, "Allow Discord to display game info on your status.", {NULL}, 0, 0},
	{NULL, "Prevents the logging of some sensitive Discord account information.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "Allow other people to request joining your game from Discord.", {NULL}, 0, 0},
		{NULL, "Set who is allowed to generate Discord invites to your game.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "Set the type of data to show on your status.", {NULL}, 0, 0},
		{NULL, "Allow memes on your status.", {NULL}, 0, 0},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "The type of character image to show on your status.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "The custom detail to show on your status.", {NULL}, DISC_STATUS_IMAGE_STRING_SIZE, DISC_STATUS_MIN_STRING_SIZE},
		{NULL, "The custom state to show on your status.", {NULL}, DISC_STATUS_IMAGE_STRING_SIZE, DISC_STATUS_MIN_STRING_SIZE},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "The image text (large) to show on your status.", {NULL}, DISC_STATUS_IMAGE_STRING_SIZE, DISC_STATUS_MIN_STRING_SIZE},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "The type of image (large) that should appear on your status.", {NULL}, 0, 0},
		{NULL, "The image (large) to show on your status.", {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},

	{NULL, NULL, {NULL}, 0, 0},
		{NULL, "The image text (small) to show on your status.", {NULL}, DISC_STATUS_IMAGE_STRING_SIZE, DISC_STATUS_MIN_STRING_SIZE},

		{NULL, NULL, {NULL}, 0, 0},

		{NULL, "The type of image (small) that should appear on your status.", {NULL}, 0, 0},
		{NULL, "The image (small) to show on your status.", {NULL}, 0, 0},
};

menuitem_t MISC_DiscordRequests[] =
{
	{IT_NOTHING | IT_KEYHANDLER, NULL, NULL, M_Sys_DiscordRequestHandler, 0},
};

tsourdt3rd_menuitem_t TSoURDt3rd_MISC_DiscordRequests[] =
{
	{NULL, NULL, {NULL}, 0, 0},
};

menu_t DISCORD_OP_MainDef = {
	MTREE2(MN_OP_MAIN, MN_OP_DISCORD_OPT),
	NULL,
	sizeof (DISCORD_OP_MainMenu)/sizeof (menuitem_t),
	&OP_MainDef,
	DISCORD_OP_MainMenu,
	TSoURDt3rd_M_DrawGenericOptions,
	30, 64,
	0,
	NULL
};

tsourdt3rd_menu_t DISCORD_TM_OP_MainDef = {
	DISCORD_TM_OP_MainMenu,
	SKINCOLOR_SLATE, 0,
	0,
	NULL,
	2, 5,
	M_Sys_DrawDiscordMenu,
	M_Sys_DiscordOptionsTicker,
	NULL,
	NULL,
	NULL,
	NULL
};

menu_t DISCORD_MISC_RequestsDef = {
	MN_OP_DISCORD_RQ,
	NULL,
	sizeof (MISC_DiscordRequests)/sizeof (menuitem_t),
	&MPauseDef,
	MISC_DiscordRequests,
	M_Sys_DrawDiscordRequests,
	0, 0,
	0,
	NULL
};

tsourdt3rd_menu_t DISCORD_TM_MISC_RequestsDef = {
	TSoURDt3rd_MISC_DiscordRequests,
	0, 0,
	0,
	NULL,
	0, 0,
	NULL,
	M_Sys_DiscordRequestTick,
	NULL,
	NULL,
	NULL,
	NULL
};

// ------------------------ //
//        Functions
// ------------------------ //

void TSoURDt3rd_M_DiscordOptions_Init(INT32 choice)
{
	(void)choice;

	TSoURDt3rd_M_ResetOptions();
	TSoURDt3rd_M_SetupNextMenu(&DISCORD_TM_OP_MainDef, &DISCORD_OP_MainDef, true);
}

static void M_Sys_DrawDiscordMenu(void)
{
	INT32 flags = V_SNAPTOBOTTOM;

	if (discordInfo.ConnectionStatus & DISC_CONNECTED)
	{
		// Discord's open, so let's print our username!
		V_DrawCenteredThinString(BASEVIDWIDTH/2, BASEVIDHEIGHT-8, flags|V_ALLOWLOWERCASE|V_MENUCOLORMAP, va("Connected to: %s", DISC_ReturnUsername()));
		return;
	}

	// Dang! Discord isn't open!
	flags |= V_REDMAP;
	if (discordInfo.ConnectionStatus & DISC_DISCONNECTED)
		V_DrawCenteredThinString(BASEVIDWIDTH/2, BASEVIDHEIGHT-16, flags, "Disconnected!");
	else
		V_DrawCenteredThinString(BASEVIDWIDTH/2, BASEVIDHEIGHT-16, flags, "Not Connected!");
	V_DrawCenteredThinString(BASEVIDWIDTH/2, BASEVIDHEIGHT-8, flags|V_ALLOWLOWERCASE, "Make sure Discord is open!");
}

static void M_Sys_DiscordOptionsTicker(void)
{
	INT32 i = 0, j = 0;
	INT32 menuflags;

#ifndef DISCORD_SECRETIVE
	INT32 custom_cvartype_index[] = {
		cv_discordcustom_imagetype_large.value,
		cv_discordcustom_imagetype_small.value,
		-1
	};
	static consvar_t *custom_cvar_index[2][5] = {
		[0] = {
			&cv_discordcustom_characterimage_large,
			&cv_discordcustom_supercharacterimage_large,
			&cv_discordcustom_mapimage_large,
			&cv_discordcustom_miscimage_large,
			NULL
		},
		[1] = {
			&cv_discordcustom_characterimage_small,
			&cv_discordcustom_supercharacterimage_small,
			&cv_discordcustom_mapimage_small,
			&cv_discordcustom_miscimage_small,
			NULL
		},
	};
	consvar_t *discord_itemactions = NULL;
#endif

	static INT32 discord_menuitems[7][6] = {
		[0] = {
			op_drpcs_header,
			-1
		},
		[1] = {
			op_drpcs_asks,
			op_drpcs_invites,
			op_drpcs_showonstatus,
			op_drpcs_statusmemes,
			op_drpcs_charimgtype,
			-1
		},
		[2] = {
			op_cps_header,
			op_cpli_header,
			op_cpls_header,
			-1
		},
		[3] = {
			op_cpli_largeimgtype,
			op_cpls_smallimgtype,
			-1
		},
		[4] = {
			op_cpli_largeimg,
			op_cpls_smallimg,
			-1
		},
		[5] = {
			op_cps_details,
			op_cps_state,
			op_cpli_largeimgtext,
			op_cpls_smallimgtext,
			-1
		},
		[6] = {
			-2
		}
	};

	TSoURDt3rd_M_OptionsTick(); // Tick throughout the entire menu
	DISC_UpdatePresence(); // Update DRPC info

	// Set option availability and actions
	while (discord_menuitems[i][0] != -2)
	{

#ifdef DISCORD_SECRETIVE
		switch (i)
		{
			default:
				menuflags = IT_DISABLED;
				break;
		}
#else
		switch (i)
		{
			default:
				menuflags = (cv_discordrp.value ? IT_HEADER : IT_DISABLED);
				break;
			case 1:
				menuflags = (cv_discordrp.value ? IT_CVAR|IT_STRING : IT_DISABLED);
				break;
			case 2:
				menuflags = ((cv_discordrp.value && cv_discordshowonstatus.value == 9) ? IT_HEADER : IT_DISABLED);
				break;
			case 3:
				menuflags = ((cv_discordrp.value && cv_discordshowonstatus.value == 9) ? IT_CVAR|IT_STRING : IT_DISABLED);
				break;
			case 4:
			{
				menuflags = ((cv_discordrp.value && cv_discordshowonstatus.value == 9) ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
				if (menuflags == IT_GRAYEDOUT)
				{
					discord_itemactions = NULL;
					break;
				}
				switch (custom_cvartype_index[j])
				{
					case 0: case 1: case 2:
						discord_itemactions = custom_cvar_index[j][0];
						break;
					case 3: case 4: case 5:
						discord_itemactions = custom_cvar_index[j][1];
						break;
					case 6:
						discord_itemactions = custom_cvar_index[j][2];
						break;
					case 7:
						discord_itemactions = custom_cvar_index[j][3];
						break;
					default:
						menuflags = IT_GRAYEDOUT;
						discord_itemactions = NULL;
						break;
				}
				DISCORD_OP_MainMenu[discord_menuitems[i][j]].itemaction = discord_itemactions;
				break;
			}
			case 5:
				menuflags = ((cv_discordrp.value && cv_discordshowonstatus.value == 9) ? IT_CVAR|IT_STRING|IT_CV_STRING : IT_DISABLED);
				break;
		}
		discord_itemactions = NULL;
#endif

		DISCORD_OP_MainMenu[discord_menuitems[i][j]].status = menuflags;

		if (discord_menuitems[i][++j] == -1)
		{
			j = 0;
			i++;
		}
	}

	// Handle typing data
	if (!menumessage.active && menutyping.menutypingclose && menutyping.menutypingfade == 1
	&& !TSoURDt3rd_M_VirtualStringMeetsLength())
	{
		S_StartSound(NULL, sfx_skid);
		TSoURDt3rd_M_StartMessage(
			"String too short!",
			"Sorry, Discord requires status strings to\nbe longer than two characters.\n\nPlease type a longer string.",
			NULL,
			MM_NOTHING,
			NULL,
			NULL
		);
	}
}

// ========
// REQUESTS
// ========

static const char *M_Sys_GetDiscordName(DISC_Request_t *r)
{
	if (r == NULL) return "";
	return DISC_ReturnUsername();
}

void TSoURDt3rd_M_DiscordRequests_Init(INT32 choice)
{
	static const tic_t confirmLength = 3*TICRATE/4;

	(void)choice;

	discordrequestmenu.confirmLength = confirmLength;
	DISCORD_MISC_RequestsDef.prevMenu = currentMenu;
	TSoURDt3rd_M_SetupNextMenu(&DISCORD_TM_MISC_RequestsDef, &DISCORD_MISC_RequestsDef, true);
}

static void M_Sys_DrawDiscordRequests(void)
{
	DISC_Request_t *curRequest = discordRequestList;
	UINT8 *colormap;
	patch_t *hand = NULL;

	const char *wantText = "...would like to join!";
	const char *acceptText = "Accept" ;
	const char *declineText = "Decline";

	INT32 x = 100;
	INT32 y = 133;

	INT32 slide = 0;
	INT32 maxYSlide = 18;

	if (discordrequestmenu.confirmDelay > 0)
	{
		if (discordrequestmenu.confirmAccept == true)
		{
			colormap = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_GREEN, GTC_CACHE);
			hand = W_CachePatchName("K_LAPH02", PU_CACHE);
		}
		else
		{
			colormap = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_RED, GTC_CACHE);
			hand = W_CachePatchName("K_LAPH03", PU_CACHE);
		}

		slide = discordrequestmenu.confirmLength - discordrequestmenu.confirmDelay;
	}
	else
	{
		colormap = R_GetTranslationColormap(TC_DEFAULT, SKINCOLOR_GREY, GTC_CACHE);
	}

	V_DrawFixedPatch(56*FRACUNIT, 150*FRACUNIT, FRACUNIT, 0, W_CachePatchName("K_LAPE01", PU_CACHE), colormap);

	if (hand != NULL)
	{
		fixed_t handoffset = (4 - abs((signed)(tsourdt3rd_skullAnimCounter - 4))) * FRACUNIT;
		V_DrawFixedPatch(56*FRACUNIT, 150*FRACUNIT + handoffset, FRACUNIT, 0, hand, NULL);
	}

	TSoURDt3rd_MK_DrawSticker(x + (slide * 32), y - 2, V_ThinStringWidth(M_Sys_GetDiscordName(curRequest), V_ALLOWLOWERCASE), 0, false);
	V_DrawThinString(x + (slide * 32), y - 1, V_YELLOWMAP|V_ALLOWLOWERCASE, M_Sys_GetDiscordName(curRequest));

	TSoURDt3rd_MK_DrawSticker(x, y + 12, V_ThinStringWidth(wantText, V_ALLOWLOWERCASE), 0, true);
	V_DrawThinString(x, y + 10, V_ALLOWLOWERCASE, wantText);

	INT32 confirmButtonWidth = SHORT(kp_button_a[1][0]->width);
	INT32 declineButtonWidth = SHORT(kp_button_b[1][0]->width);
	INT32 altDeclineButtonWidth = SHORT(kp_button_x[1][0]->width);
	INT32 acceptTextWidth =  V_ThinStringWidth(acceptText, V_ALLOWLOWERCASE);
	INT32 declineTextWidth = V_ThinStringWidth(declineText, V_ALLOWLOWERCASE);
	INT32 stickerWidth = (confirmButtonWidth + declineButtonWidth + altDeclineButtonWidth + acceptTextWidth + declineTextWidth);

	TSoURDt3rd_MK_DrawSticker(x, y + 26, stickerWidth, 0, true);
	TSoURDt3rd_MK_DrawButtonAnim(x, y + 22, V_SNAPTORIGHT, kp_button_a[1], discordrequestmenu.ticker);

	INT32 xoffs = confirmButtonWidth;

	V_DrawThinString((x + xoffs), y + 24, V_ALLOWLOWERCASE, acceptText);
	xoffs += acceptTextWidth;

	TSoURDt3rd_MK_DrawButtonAnim((x + xoffs), y + 22, V_SNAPTORIGHT, kp_button_b[1], discordrequestmenu.ticker);
	xoffs += declineButtonWidth;

	TSoURDt3rd_MK_DrawButtonAnim((x + xoffs), y + 22, V_SNAPTORIGHT, kp_button_x[1], discordrequestmenu.ticker);
	xoffs += altDeclineButtonWidth;

	V_DrawThinString((x + xoffs), y + 24, V_ALLOWLOWERCASE, declineText);

	y -= 18;

	while (curRequest->next != NULL)
	{
		INT32 ySlide = min(slide * 4, maxYSlide);

		curRequest = curRequest->next;

		const char *discordname = M_Sys_GetDiscordName(curRequest);

		TSoURDt3rd_MK_DrawSticker(x, y - 1 + ySlide, V_ThinStringWidth(discordname, V_ALLOWLOWERCASE), 0, false);
		V_DrawThinString(x, y + ySlide, V_ALLOWLOWERCASE, discordname);

		y -= 12;
		maxYSlide = 12;
	}
}

static void M_Sys_DiscordRequestTick(void)
{
	discordrequestmenu.ticker++;

	if (discordrequestmenu.confirmDelay > 0)
	{
		discordrequestmenu.confirmDelay--;

		if (discordrequestmenu.confirmDelay == 0)
		{
			discordrequestmenu.removeRequest = true;
		}
	}

	if (discordrequestmenu.removeRequest == true)
	{
		DISC_RemoveRequest(discordRequestList);

		if (discordRequestList == NULL)
		{
			// No other requests
			MPauseMenu[4].status = IT_DISABLED; // mpause_discordrequests

			if (currentMenu->prevMenu)
			{
				TSoURDt3rd_M_SetupNextMenu(tsourdt3rd_currentMenu->prev_menu, currentMenu->prevMenu, true);
				tsourdt3rd_itemOn = 5; // mpause_continue
			}
			else
				M_ClearMenus(true);
		}

		discordrequestmenu.removeRequest = false;
	}
}

static void M_Sys_DiscordRequestHandler(INT32 choice)
{
	const UINT8 pid = 0;
	DISC_RequestReply_t response;
	INT32 response_sound = 0;

	(void)choice;

	if (discordrequestmenu.confirmDelay > 0)
		return;

	if (TSoURDt3rd_M_MenuConfirmPressed(pid))
	{
		response = DISC_REQUEST_REPLY_YES;
		response_sound = sfx_s3k63;
	}
	else if (TSoURDt3rd_M_MenuBackPressed(pid))
	{
		response = DISC_REQUEST_REPLY_NO;
		response_sound = sfx_s3kb2;
	}
	else
		return;

	DISC_Respond(discordRequestList->userID, response);
	discordrequestmenu.confirmAccept = (boolean)response;
	discordrequestmenu.confirmDelay = discordrequestmenu.confirmLength;
	S_StartSound(NULL, response_sound);
}

#endif // HAVE_DISCORDSUPPORT
