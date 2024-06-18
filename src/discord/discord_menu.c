// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2018-2020 by Sally "TehRealSalt" Cochenour.
// Copyright (C) 2018-2024 by Kart Krew.
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  discord_menu.c
/// \brief Discord Rich Presence menu options

#include "discord_menu.h"
#include "discord_cmds.h"
#include "discord.h"
#include "../STAR/m_menu.h"
#include "../v_video.h"
#include "../z_zone.h"
#include "../r_draw.h"
#include "../w_wad.h"

#ifdef HAVE_DISCORDSUPPORT

// ------------------------ //
//        Variables
// ------------------------ //

struct discordrequestmenu_s discordrequestmenu;

static patch_t *kp_button_a[2][2];
static patch_t *kp_button_b[2][2];
static patch_t *kp_button_x[2][2];

// =====
// MENUS
// =====

menu_t OP_CustomStatusOutputDef;

static void M_DrawDiscordMenu(void);

static void M_DiscordRequestHandler(INT32 choice);
static void M_DrawDiscordRequests(void);

static menuitem_t OP_DiscordOptionsMenu[] =
{
	{IT_HEADER,							NULL, 	"Discord Rich Presence...",	NULL,					 	  	        0},
	{IT_STRING | IT_CVAR,				NULL, 	"Rich Presence",			&cv_discordrp,			 	  		    6},
	{IT_STRING | IT_CVAR,				NULL, 	"Streamer Mode",			&cv_discordstreamer,	 	 		   11},

	{IT_HEADER,							NULL,	"Rich Presence Settings",	NULL,					 	 		   20},
	{IT_STRING | IT_CVAR,				NULL, 	"Allow Ask to Join",		&cv_discordasks,		 	 		   26},
	{IT_STRING | IT_CVAR,				NULL,   "Allow Invites",			&cv_discordinvites,		 	 		   31},

	{IT_STRING | IT_CVAR,				NULL, 	"Show on Status",			&cv_discordshowonstatus, 	 	       41},
	
	{IT_HEADER,							NULL,	"Misc. Rich Presence Settings",
																			NULL,					 	 		   50},
	{IT_STRING | IT_CVAR,				NULL, 	"Memes on Status",			&cv_discordstatusmemes,	 	 		   56},
	{IT_STRING | IT_CVAR,				NULL, 	"Skin Image Type",			&cv_discordcharacterimagetype,		   61},

	{IT_HEADER,							NULL,   "Custom Discord Status",	NULL,					 	 		   70},
	{IT_STRING | IT_CVAR | IT_CV_STRING,NULL, 	"Header",			        &cv_customdiscorddetails, 	 		   76},
	{IT_STRING | IT_CVAR | IT_CV_STRING,NULL, 	"State",			        &cv_customdiscordstate, 			   90},

	{IT_STRING | IT_CVAR,		        NULL, 	"L. Image Type",			&cv_customdiscordlargeimagetype,      104},
	{IT_STRING | IT_CVAR,		        NULL, 	"S. Image Type",			&cv_customdiscordsmallimagetype,      109},

	{IT_STRING | IT_CVAR,		        NULL, 	"L. Image",					NULL, 								  119}, // Handled by discord_option_onchange //
	{IT_STRING | IT_CVAR,		        NULL, 	"S. Image",					NULL, 								  124}, // Handled by discord_option_onchange //

	{IT_STRING | IT_CVAR | IT_CV_STRING,NULL, 	"L. Image Text",		    &cv_customdiscordlargeimagetext,      134},
	{IT_STRING | IT_CVAR | IT_CV_STRING,NULL, 	"S. Image Text",			&cv_customdiscordsmallimagetext,      148},

	{IT_STRING | IT_SUBMENU,			NULL, 	"Show Output",				&OP_CustomStatusOutputDef,	          162},
};

static menuitem_t OP_CustomStatusOutputMenu[] =
{
	{IT_HEADER,		NULL,	"Custom Status Output",		NULL,	0},
};

static menuitem_t MISC_DiscordRequests[] =
{
	{IT_KEYHANDLER|IT_NOTHING, NULL, "", M_DiscordRequestHandler, 0},
};

menu_t OP_DiscordOptionsDef =
{
	MTREE2(MN_OP_MAIN, MN_OP_DISCORD_OPT),
	"M_DISC",
	sizeof (OP_DiscordOptionsMenu)/sizeof (menuitem_t),
	&OP_MainDef,
	OP_DiscordOptionsMenu,
	M_DrawDiscordMenu,
	30, 30,
	0,
	NULL
};

menu_t OP_CustomStatusOutputDef = DEFAULTMENUSTYLE(
	MTREE3(MN_OP_MAIN, MN_OP_DISCORD_OPT, MN_OP_DISCORDCS_OUTPUT), 
	"M_DISCOT", OP_CustomStatusOutputMenu, &OP_DiscordOptionsDef, 30, 30);

menu_t MISC_DiscordRequestsDef = {
	MN_OP_DISCORD_RQ,
	NULL,
	sizeof (MISC_DiscordRequests)/sizeof (menuitem_t),
	&MPauseDef,
	MISC_DiscordRequests,
	M_DrawDiscordRequests,
	0, 0,
	0,
	NULL
};

enum
{
	op_richpresenceheader = 3,

	op_discordasks,
	op_discordinvites,
	op_discordshowonstatus,

	op_discordmiscoptionsheader,
	op_discordstatusmemes,	
	op_discordcharacterimagetype,

	op_customstatusheader,
	op_customdiscorddetails,
	op_customdiscordstate,

	op_customdiscordlargeimagetype,
	op_customdiscordsmallimagetype,

	op_customdiscordlargeimage,
	op_customdiscordsmallimage,

	op_customdiscordlargeimagetext,
	op_customdiscordsmallimagetext,

	op_customstatusoutputdef,
};

// ------------------------ //
//        Functions
// ------------------------ //

// Load a HUDGFX patch or NULL/missingpat (dependent on required boolean).
static patch_t *HU_UpdateOrBlankPatch(patch_t **user, boolean required, const char *format, ...)
{
	va_list ap;
	char buffer[9];

	lumpnum_t lump = INT16_MAX;
	patch_t *patch;

	va_start (ap, format);
	vsnprintf(buffer, sizeof buffer, format, ap);
	va_end   (ap);

#if 0
	if (user && partadd_earliestfile != UINT16_MAX)
	{
		UINT16 fileref = numwadfiles;
		lump = INT16_MAX;

		while ((lump == INT16_MAX) && ((--fileref) >= partadd_earliestfile))
		{
			lump = W_CheckNumForNamePwad(buffer, fileref, 0);
		}

		/* no update in this wad */
		if (fileref < partadd_earliestfile)
			return *user;

		lump |= (fileref << 16);
	}
	else
#endif
	{
		lump = W_CheckNumForName(buffer);

		if (lump == LUMPERROR)
		{
#if 0
			if (required == true)
				*user = missingpat;
#else
			(void)required;
#endif

			return *user;
		}
	}

	patch = W_CachePatchNum(lump, PU_HUDGFX);

	if (user)
	{
		if (*user)
			Patch_Free(*user);

		*user = patch;
	}

	return patch;
}

//#define HU_CachePatch(...) HU_UpdateOrBlankPatch(NULL, false, __VA_ARGS__) -- not sure how to default the missingpat here plus not currently used
#define HU_UpdatePatch(user, ...) HU_UpdateOrBlankPatch(user, true, __VA_ARGS__)

static void K_LoadButtonGraphics(patch_t *kp[2], int letter)
{
	HU_UpdatePatch(&kp[0], "TLB_%c", letter);
	HU_UpdatePatch(&kp[1], "TLB_%cB", letter);
}

// ====
// MAIN
// ====

void M_DiscordOptionsTicker(void)
{
	// Option Availability //
	OP_DiscordOptionsMenu[op_richpresenceheader].status = (cv_discordrp.value ? IT_HEADER : IT_DISABLED);
	OP_DiscordOptionsMenu[op_discordasks].status = (cv_discordrp.value ? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_discordinvites].status =
		((cv_discordrp.value && cv_discordasks.value) ? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_discordmiscoptionsheader].status = (cv_discordrp.value ? IT_HEADER : IT_DISABLED);
	OP_DiscordOptionsMenu[op_discordstatusmemes].status = (cv_discordrp.value ? IT_CVAR|IT_STRING : IT_DISABLED);
	OP_DiscordOptionsMenu[op_discordshowonstatus].status = (cv_discordrp.value ? IT_CVAR|IT_STRING : IT_DISABLED);
	OP_DiscordOptionsMenu[op_discordcharacterimagetype].status = (cv_discordrp.value ? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customstatusheader].status =
		((cv_discordrp.value && cv_discordshowonstatus.value == 8) ? IT_HEADER : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscorddetails].status =
		((cv_discordrp.value && cv_discordshowonstatus.value == 8) ? IT_CVAR|IT_STRING|IT_CV_STRING : IT_DISABLED);
	
	OP_DiscordOptionsMenu[op_customdiscordstate].status =
		((cv_discordrp.value && cv_discordshowonstatus.value == 8) ? IT_CVAR|IT_STRING|IT_CV_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscordlargeimagetype].status =
			((cv_discordrp.value && cv_discordshowonstatus.value == 8) ? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscordsmallimagetype].status =
			((cv_discordrp.value && cv_discordshowonstatus.value == 8) ? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscordlargeimage].status =
			((cv_discordrp.value && cv_discordshowonstatus.value == 8 && cv_customdiscordlargeimagetype.value != 8)
				? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscordsmallimage].status =
			((cv_discordrp.value && cv_discordshowonstatus.value == 8 && cv_customdiscordsmallimagetype.value != 8)
				? IT_CVAR|IT_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscordlargeimagetext].status =
			((cv_discordrp.value && cv_discordshowonstatus.value == 8 && cv_customdiscordlargeimagetype.value != 8)
				? IT_CVAR|IT_STRING|IT_CV_STRING : IT_DISABLED);

	OP_DiscordOptionsMenu[op_customdiscordsmallimagetext].status =
			((cv_discordrp.value && cv_discordshowonstatus.value == 8 && cv_customdiscordsmallimagetype.value != 8)
				? IT_CVAR|IT_STRING|IT_CV_STRING : IT_DISABLED);

#if 1
	OP_DiscordOptionsMenu[op_customstatusoutputdef].status = IT_DISABLED;
#else
	OP_DiscordOptionsMenu[op_customstatusoutputdef].status =
		((cv_discordrp.value && cv_discordshowonstatus.value == 8) ? IT_STRING|IT_SUBMENU : IT_DISABLED);
#endif

	// Option Actions //
	if (cv_customdiscordlargeimagetype.value <= 2)
		OP_DiscordOptionsMenu[op_customdiscordlargeimage].itemaction = &cv_customdiscordlargecharacterimage;
	else if (cv_customdiscordlargeimagetype.value >= 3 && cv_customdiscordlargeimagetype.value <= 5)
		OP_DiscordOptionsMenu[op_customdiscordlargeimage].itemaction = &cv_customdiscordlargesupercharacterimage;
	else if (cv_customdiscordlargeimagetype.value == 6)
		OP_DiscordOptionsMenu[op_customdiscordlargeimage].itemaction = &cv_customdiscordlargemapimage;
	else
		OP_DiscordOptionsMenu[op_customdiscordlargeimage].itemaction = &cv_customdiscordlargemiscimage;

	if (cv_customdiscordsmallimagetype.value <= 2)
		OP_DiscordOptionsMenu[op_customdiscordsmallimage].itemaction = &cv_customdiscordsmallcharacterimage;
	else if (cv_customdiscordsmallimagetype.value >= 3 && cv_customdiscordsmallimagetype.value <= 5)
		OP_DiscordOptionsMenu[op_customdiscordsmallimage].itemaction = &cv_customdiscordsmallsupercharacterimage;
	else if (cv_customdiscordsmallimagetype.value == 6)
		OP_DiscordOptionsMenu[op_customdiscordsmallimage].itemaction = &cv_customdiscordsmallmapimage;
	else
		OP_DiscordOptionsMenu[op_customdiscordsmallimage].itemaction = &cv_customdiscordsmallmiscimage;

	// Finished! //
	DRPC_UpdatePresence();
}

void M_DiscordOptions(INT32 choice)
{
	(void)choice;
	M_DiscordOptionsTicker();

	M_SetupNextMenu(&OP_DiscordOptionsDef);
}

static void M_DrawDiscordMenu(void)
{
	M_DrawGenericScrollMenu();

	if (discordInfo.ConnectionStatus != DRPC_CONNECTED) // Dang! Discord isn't open!
	{
		if (discordInfo.ConnectionStatus == DRPC_DISCONNECTED)
			V_DrawCenteredString(BASEVIDWIDTH/2, 200, V_REDMAP, "Disconnected");
		else
			V_DrawCenteredString(BASEVIDWIDTH/2, 200, V_REDMAP, "Not Connected");
		V_DrawCenteredString(BASEVIDWIDTH/2, 210, V_REDMAP,	"Is Discord Open?");
		return;
	}
	V_DrawCenteredString(BASEVIDWIDTH/2, 210, V_MENUCOLORMAP, va("Connected to: %s", DRPC_ReturnUsername()));
}

// ========
// REQUESTS
// ========

static const char *M_GetDiscordName(discordRequest_t *r)
{
	if (r == NULL)
		return "";
	return DRPC_ReturnUsername();
}

void M_DiscordRequests(INT32 choice)
{
	(void)choice;
	static const tic_t confirmLength = 3*TICRATE/4;

	discordrequestmenu.confirmLength = confirmLength;
	MISC_DiscordRequestsDef.prevMenu = currentMenu;
	M_SetupNextMenu(&MISC_DiscordRequestsDef);
}

void M_DiscordRequestTick(void)
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
		DRPC_RemoveRequest(discordRequestList);

		if (discordRequestList == NULL)
		{
			// No other requests
			MPauseMenu[4].status = IT_DISABLED; // mpause_discordrequests

			if (currentMenu->prevMenu)
			{
				M_SetupNextMenu(currentMenu->prevMenu);
				itemOn = 5; // mpause_continue
			}
			else
				M_ClearMenus(true);
		}

		discordrequestmenu.removeRequest = false;
	}
}

static void M_DiscordRequestHandler(INT32 choice)
{
	if (discordrequestmenu.confirmDelay > 0)
		return;

	switch (choice)
	{
		case KEY_ENTER:
			Discord_Respond(discordRequestList->userID, DISCORD_REPLY_YES);
			discordrequestmenu.confirmAccept = true;
			discordrequestmenu.confirmDelay = discordrequestmenu.confirmLength;
			S_StartSound(NULL, sfx_s3k63);
			break;

		case KEY_ESCAPE:
			Discord_Respond(discordRequestList->userID, DISCORD_REPLY_NO);
			discordrequestmenu.confirmAccept = false;
			discordrequestmenu.confirmDelay = discordrequestmenu.confirmLength;
			S_StartSound(NULL, sfx_s3kb2);
			break;
	}
}

static void M_DrawDiscordRequests(void)
{
	discordRequest_t *curRequest = discordRequestList;
	UINT8 *colormap;
	patch_t *hand = NULL;

	const char *wantText = "...would like to join!";
	const char *acceptText = "Accept" ;
	const char *declineText = "Decline";

	INT32 x = 100;
	INT32 y = 133;

	INT32 slide = 0;
	INT32 maxYSlide = 18;

	//K_LoadButtonGraphics(kp_button_a[0], 'A');
	K_LoadButtonGraphics(kp_button_a[1], 'N');
	//K_LoadButtonGraphics(kp_button_b[0], 'B');
	K_LoadButtonGraphics(kp_button_b[1], 'O');
	//K_LoadButtonGraphics(kp_button_x[0], 'D');
	K_LoadButtonGraphics(kp_button_x[1], 'Q');

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
		fixed_t handoffset = (4 - abs((signed)(skullAnimCounter - 4))) * FRACUNIT;
		V_DrawFixedPatch(56*FRACUNIT, 150*FRACUNIT + handoffset, FRACUNIT, 0, hand, NULL);
	}

	K_DrawSticker(x + (slide * 32), y - 2, V_ThinStringWidth(M_GetDiscordName(curRequest), 0), 0, false);
	V_DrawThinString(x + (slide * 32), y - 1, V_YELLOWMAP, M_GetDiscordName(curRequest));

	K_DrawSticker(x, y + 12, V_ThinStringWidth(wantText, 0), 0, true);
	V_DrawThinString(x, y + 10, 0, wantText);

	INT32 confirmButtonWidth = SHORT(kp_button_a[1][0]->width);
	INT32 declineButtonWidth = SHORT(kp_button_b[1][0]->width);
	INT32 altDeclineButtonWidth = SHORT(kp_button_x[1][0]->width);
	INT32 acceptTextWidth =  V_ThinStringWidth(acceptText, 0);
	INT32 declineTextWidth = V_ThinStringWidth(declineText, 0);
	INT32 stickerWidth = (confirmButtonWidth + declineButtonWidth + altDeclineButtonWidth + acceptTextWidth + declineTextWidth);

	K_DrawSticker(x, y + 26, stickerWidth, 0, true);
	K_drawButtonAnim(x, y + 22, V_SNAPTORIGHT, kp_button_a[1], discordrequestmenu.ticker);

	INT32 xoffs = confirmButtonWidth;

	V_DrawThinString((x + xoffs), y + 24, 0, acceptText);
	xoffs += acceptTextWidth;

	K_drawButtonAnim((x + xoffs), y + 22, V_SNAPTORIGHT, kp_button_b[1], discordrequestmenu.ticker);
	xoffs += declineButtonWidth;

	K_drawButtonAnim((x + xoffs), y + 22, V_SNAPTORIGHT, kp_button_x[1], discordrequestmenu.ticker);
	xoffs += altDeclineButtonWidth;

	V_DrawThinString((x + xoffs), y + 24, 0, declineText);

	y -= 18;

	while (curRequest->next != NULL)
	{
		INT32 ySlide = min(slide * 4, maxYSlide);

		curRequest = curRequest->next;

		const char *discordname = M_GetDiscordName(curRequest);

		K_DrawSticker(x, y - 1 + ySlide, V_ThinStringWidth(discordname, 0), 0, false);
		V_DrawThinString(x, y + ySlide, 0, discordname);

		y -= 12;
		maxYSlide = 12;
	}
}

#endif // HAVE_DISCORDSUPPORT
