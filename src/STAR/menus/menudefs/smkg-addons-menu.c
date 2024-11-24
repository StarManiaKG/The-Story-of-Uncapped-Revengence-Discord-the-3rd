// SONIC ROBO BLAST 2; TSOURDT3RD
//-----------------------------------------------------------------------------
// Copyright (C) 2020-2024 by Star "Guy Who Names Scripts After Him" ManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  menus/menudefs/smkg-addons-menu.c
/// \brief Extended data for the addons menu.

#include "../smkg-m_sys.h"
#include "../../smkg-misc.h"

#include "../../../d_main.h"
#include "../../../filesrch.h"

// ------------------------ //
//        Functions
// ------------------------ //

//
// static char *M_Sys_GetFullAddonPath(boolean decatenate)
// Lists the full directory of our currently selected addon.
//
static char *M_Sys_GetFullAddonPath(boolean decatenate)
{
	UINT32 len;
	const UINT32 len_to_decentate = 54;
	static char header[1024];

	strlcpy(header, va("%s%s", menupath, dirmenu[dir_on[menudepthleft]]+DIR_STRING), 1024);
	if (decatenate == false)
	{
		// We've done everything we needed to.
		return header;
	}

	len = strlen(header);
	if (len > len_to_decentate)
	{
		len = len-len_to_decentate;
		header[len] = header[len+1] = header[len+2] = '.';
	}
	else
		len = 0;

	return header+len;
}

//
// static void M_Sys_ConfigExec(INT32 choice);
// Executes data within config/txt files.
//
static void M_Sys_ConfigExec(INT32 choice)
{
	if (choice != MA_YES)
	{
		// We refused to execute this file!
		return;
	}
	S_StartSound(NULL, sfx_strpst);
	COM_BufAddText(va("exec \"%s%s\"", menupath, dirmenu[dir_on[menudepthleft]]+DIR_STRING));
}

//
// static void M_Sys_FolderExec(INT32 choice);
// Loads folders as addons.
//
static void M_Sys_FolderExec(INT32 choice)
{
	if (choice != MA_YES)
	{
		// We refused to load this folder!
		return;
	}
	S_StartSound(NULL, sfx_strpst);
	COM_BufAddText(va("addfolder \"%s%s\"", menupath, dirmenu[dir_on[menudepthleft]]+DIR_STRING));
}

static void M_Sys_LoadAddon(INT32 choice)
{
	boolean refresh = true;

	if (choice != MA_YES)
	{
		// We refused to load this addon!
		return;
	}

	if (!dirmenu || !dirmenu[dir_on[menudepthleft]])
	{
		// Nothing. No point. Nil. Null. Nada.
		S_StartSound(NULL, sfx_lose);
		refreshdirmenu |= REFRESHDIR_NORMAL;
		return;
	}

	switch (dirmenu[dir_on[menudepthleft]][DIR_TYPE])
	{
		case EXT_FOLDER:
			M_Sys_FolderExec(MA_YES);
			break;
		case EXT_TXT:
		case EXT_CFG:
			M_Sys_ConfigExec(MA_YES);
			break;
		case EXT_LUA:
		case EXT_SOC:
		case EXT_WAD:
#ifdef USE_KART
		case EXT_KART:
#endif
		case EXT_PK3:
			COM_BufAddText(va("addfile \"%s%s\"", menupath, dirmenu[dir_on[menudepthleft]]+DIR_STRING));
			break;
		default:
			S_StartSound(NULL, sfx_lose);
			break;
	}
	if (refresh)
		refreshdirmenu |= REFRESHDIR_NORMAL;
}

//
// static void M_Sys_AutoLoadAddons(INT32 choice);
// Exports certain mods to autoload.cfg, which allows TSoURDt3rd to autoload those files on startup.
//
static void M_Sys_AutoLoadAddons(INT32 choice)
{
	const char *addon_name = dirmenu[dir_on[menudepthleft]]+DIR_STRING;
	FILE *autoload_config = NULL;

	autoload_config = TSoURDt3rd_FIL_AccessFile(NULL, AUTOLOADCONFIGFILENAME, "r");
	if (autoload_config != NULL)
	{
		// Let's move this old file to a new directory!
		fclose(autoload_config);
		TSoURDt3rd_FIL_RenameFile(AUTOLOADCONFIGFILENAME, "TSoURDt3rd" PATHSEP AUTOLOADCONFIGFILENAME);
	}
	autoload_config = TSoURDt3rd_FIL_AccessFile("TSoURDt3rd", AUTOLOADCONFIGFILENAME, "a");

	if (autoload_config == NULL)
	{
		// Uh-oh! We couldn't find the actual autoload config!
		return;
	}

	if (choice != MA_YES)
	{
		// We refused to autoload this addon!
		return;
	}

	// Execute routines based on the file's type!
	switch (dirmenu[dir_on[menudepthleft]][DIR_TYPE])
	{
		case EXT_FOLDER:
			fprintf(autoload_config, "%s\n", M_Sys_GetFullAddonPath(false));
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Added folder \x82\"%s\"\x80 to the autoload configuration list.\n", addon_name);
			break;
		case EXT_TXT:
		case EXT_CFG:
			fprintf(autoload_config, "%s\n", addon_name);
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Added console script \x82\"%s\"\x80 to the autoload configuration list.\n", addon_name);
			break;
		case EXT_LUA:
		case EXT_SOC:
		case EXT_WAD:
#ifdef USE_KART
		case EXT_KART:
#endif
		case EXT_PK3:
		default:
			fprintf(autoload_config, "%s\n", addon_name);
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_NOTICE, "Added addon \x82\"%s\"\x80 to the autoload configuration list.\n", addon_name);
			break;
	}

	// We're done! :)
	S_StartSound(NULL, sfx_spdpad);
	if (!(refreshdirmenu & REFRESHDIR_MAX))
	{
		TSoURDt3rd_M_StartMessage(
			M_Sys_GetFullAddonPath(true),
			M_GetText(
				"Autoloading was successful!\n"
				"Keep in mind that the effects of\nautoloading this addon won't happen\nuntil you \x82restart TSoURDt3rd\x80.\n\n"
				"However, you can load the mod like normal right now. Would you like to do so?\n"
			),
			M_Sys_LoadAddon,
			MM_YESNO,
			"Yes, load this addon like normal!",
			"No, thanks."
		);
	}
	fclose(autoload_config);
}

//
// void TSoURDt3rd_M_HandleAddonsMenu(INT32 choice)
// Handles various routines for different choices in the addons menu.
//
void TSoURDt3rd_M_HandleAddonsMenu(INT32 choice)
{
	const char *addon_name = dirmenu[dir_on[menudepthleft]]+DIR_STRING;
	char *path = M_Sys_GetFullAddonPath(true);
	boolean refresh = true;

	if (!dirmenu || !dirmenu[dir_on[menudepthleft]])
	{
		// Nothing. No point. Nil. Null. Nada.
		S_StartSound(NULL, sfx_lose);
		refreshdirmenu |= REFRESHDIR_NORMAL;
		return;
	}

	switch (choice)
	{
		// Autoloading Files
		case KEY_LEFTARROW:
		{
			switch (dirmenu[dir_on[menudepthleft]][DIR_TYPE])
			{
				case EXT_UP:
					S_StartSound(NULL, sfx_skid);
					TSoURDt3rd_M_StartMessage(
						path,
						M_GetText("Nice try.\n"),
						NULL,
						MM_NOTHING,
						NULL,
						NULL
					);
					break;
				case EXT_FOLDER:
					if (!menudepthleft)
					{
						S_StartSound(NULL, sfx_skid);
						TSoURDt3rd_M_StartMessage(
							path,
							M_GetText("This folder is too deep to navigate to!\n\n(What kind of person needs folders\nthis deep anyways?)\n"),
							NULL,
							MM_NOTHING,
							NULL,
							NULL
						);
						menupath[menupathindex[menudepthleft]] = 0;
					}
					else
					{
						if (!preparefilemenu(false))
						{
							S_StartSound(NULL, sfx_skid);
							TSoURDt3rd_M_StartMessage(
								path,
								M_GetText("This folder is empty.\n"),
								NULL,
								MM_NOTHING,
								NULL,
								NULL
							);
							menupath[menupathindex[++menudepthleft]] = 0;
						}
						else
						{
							TSoURDt3rd_M_StartMessage(
								path,
								M_GetText(
									"Do you want to\x82 autoload this folder\x80?\n"
									"If so, this folder will bypass most game-modifying checks\nthe next time you start\nTSoURDt3rd.\n\n"
									"(Bare in mind that the file structure\nfor folders should be similar\nto the structure of a PK3.)\n"
								),
								M_Sys_AutoLoadAddons,
								MM_YESNO,
								"Yes, autoload this folder!",
								"No, nevermind..."
							);
						}
						refresh = false;
					}
					break;
				case EXT_TXT:
				case EXT_CFG:
					if (STAR_DoesStringMatchHarcodedFileName(addon_name))
					{
						S_StartSound(NULL, sfx_skid);
						TSoURDt3rd_M_StartMessage(
							path,
							M_GetText("You can't\x82 autoload the base configuration files\x80, silly!\nThey're already loaded on startup!\n"),
							NULL,
							MM_NOTHING,
							NULL,
							NULL
						);
					}
					else
					{
						TSoURDt3rd_M_StartMessage(
							path,
							M_GetText(
								"You're attempting to\x82 autoload a console script\x80.\n"
								"This may cause some weird effects to happen until you restart the game.\n"
								"Ignore this warning anyways?\n"
							),
							M_Sys_AutoLoadAddons,
							MM_YESNO,
							"Yes, do as I say!",
							"Fine, I'll listen to you..."
						);
					}
					break;
				case EXT_LUA:
				case EXT_SOC:
				case EXT_WAD:
#ifdef USE_KART
				case EXT_KART:
#endif
				case EXT_PK3:
					TSoURDt3rd_M_StartMessage(
						path,
						M_GetText(
							"Do you want to\x82 autoload this addon\x80?\n"
							"If so, this addon will bypass most game-modifying checks\nthe next time you start\nTSoURDt3rd.\n"
						),
						M_Sys_AutoLoadAddons,
						MM_YESNO,
						"Yes, autoload this addon!",
						"No, nevermind..."
					);
					break;
				default:
					if (STAR_DoesStringMatchHarcodedFileName(addon_name))
					{
						S_StartSound(NULL, sfx_skid);
						TSoURDt3rd_M_StartMessage(
							path,
							M_GetText("You can't\x82 autoload the base files\x80, silly!\nThey're already loaded on startup!\n"),
							NULL,
							MM_NOTHING,
							NULL,
							NULL
						);
					}
					else
					{
						TSoURDt3rd_M_StartMessage(
							path,
							M_GetText(
								"It could be dangerous to\x82 autoload a this unknown file\x80.\n"
								"But, you're the boss, and I'm just the machine program.\n"
								"Ignore this warning anyways?\n\n"
								"(If you do, don't say I didn't warn you...)\n"
							),
							M_Sys_AutoLoadAddons,
							MM_YESNO,
							"Yes, do as I say!",
							"Fine, I'll listen to you..."
						);
					}
					break;
			}
			break;
		}

		// Loading Folders
		case KEY_RIGHTARROW:
		{
			switch (dirmenu[dir_on[menudepthleft]][DIR_TYPE])
			{
				case EXT_FOLDER:
					if (!menudepthleft)
					{
						S_StartSound(NULL, sfx_skid);
						TSoURDt3rd_M_StartMessage(
							path,
							M_GetText("This folder is too deep to navigate to!\n\n(What kind of person needs folders\nthis deep anyways?)\n"),
							NULL,
							MM_NOTHING,
							NULL,
							NULL
						);
						menupath[menupathindex[menudepthleft]] = 0;
					}
					else
					{
						if (!preparefilemenu(false))
						{
							S_StartSound(NULL, sfx_skid);
							TSoURDt3rd_M_StartMessage(
								path,
								M_GetText("This folder is empty.\n"),
								NULL,
								MM_NOTHING,
								NULL,
								NULL
							);
							menupath[menupathindex[++menudepthleft]] = 0;
						}
						else
						{
							TSoURDt3rd_M_StartMessage(
								path,
								M_GetText(
									"Do you want to \x82load this folder\x80?\n\n"
									"(Bare in mind that the file structure\nfor folders should be similar\nto the structure of a PK3.)\n"
								),
								M_Sys_FolderExec,
								MM_YESNO,
								"Yes, load this folder!",
								"No, nevermind..."
							);
						}
						refresh = false;
					}
					break;
				default:
					S_StartSound(NULL, sfx_skid);
					TSoURDt3rd_M_StartMessage(
						path,
						M_GetText("You can only use this key to load folders.\n"),
						NULL,
						MM_NOTHING,
						NULL,
						NULL
					);
					break;
			}
			break;
		}

		default:
			refresh = false;
			break;
	}

	if (refresh)
		refreshdirmenu |= REFRESHDIR_NORMAL;
}
