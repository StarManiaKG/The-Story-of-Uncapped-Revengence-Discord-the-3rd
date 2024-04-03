
#include "ss_cmds.h"
#include "ss_main.h"
#include "m_menu.h"
#include "../doomstat.h"
#include "../m_menu.h"
#include "../v_video.h"
#include "../g_game.h"

// ------------------------ //
//        Variables
// ------------------------ //
// ====
// Game
// ====
static void STAR_TimeOver_OnChange(void);

// =======
// Players
// =======
static void STAR_SuperWithShield_OnChange(void);

static void STAR_InvulnAndShield_OnChange(void);

// =========
// Savefiles
// =========
static void STAR_UseContinues_OnChange(void);

// ------------------------ //
//        Commands
// ------------------------ //
// ====
// Game
// ====
// Ported from Uncapped Plus, TPS is Back! (for some reason)
static CV_PossibleValue_t tpsrate_cons_t[] = {{0, "No"}, {1, "Full"}, {2, "Compact"}, {0, NULL}};
consvar_t cv_tpsrate = CVAR_INIT ("showtps", "No", CV_SAVE|CV_CALL, tpsrate_cons_t, STAR_TPSRate_OnChange);

static CV_PossibleValue_t color_cons_t[] = {
	{V_MAGENTAMAP, "Magenta"},
	{V_YELLOWMAP, "Yellow"},
	{V_GREENMAP, "Green"},
	{V_BLUEMAP, "Blue"},
	{V_REDMAP, "Red"},
	{V_GRAYMAP, "Gray"},
	{V_ORANGEMAP, "Orange"},
	{V_SKYMAP, "Sky"},
	{V_PURPLEMAP, "Purple"},
	{V_AQUAMAP, "Aqua"},
	{V_PERIDOTMAP, "Peridot"},
	{V_AZUREMAP, "Azure"},
	{V_BROWNMAP, "Brown"},
	{V_ROSYMAP, "Rosy"},
	{V_INVERTMAP, "Inverted"},
	{0, NULL}};
consvar_t cv_menucolor = CVAR_INIT ("menucolor", "Yellow", CV_SAVE, color_cons_t, NULL);
consvar_t cv_fpscountercolor = CVAR_INIT ("fpscountercolor", "Green", CV_SAVE, color_cons_t, NULL);
consvar_t cv_tpscountercolor = CVAR_INIT ("tpscountercolor", "Green", CV_SAVE, color_cons_t, NULL);

consvar_t cv_storesavesinfolders = CVAR_INIT ("storesavesinfolders", "Off", CV_SAVE|CV_CALL, CV_OnOff, STAR_SetSavefileProperties);

consvar_t cv_allowtypicaltimeover = CVAR_INIT ("allowtypicaltimeover", "No", CV_SAVE|CV_CALL, CV_YesNo, STAR_TimeOver_OnChange);

consvar_t cv_soniccd = CVAR_INIT ("soniccd", "Off", CV_SAVE|CV_ALLOWLUA, CV_OnOff, NULL);

// =====
// Audio
// =====
consvar_t cv_watermuffling = CVAR_INIT ("watermuffling", "Off", CV_SAVE|CV_ALLOWLUA, CV_OnOff, NULL);

// =======
// Players
// =======
consvar_t cv_shieldblockstransformation = CVAR_INIT ("shieldblockstransformation", "Off", CV_SAVE|CV_CALL, CV_OnOff, STAR_SuperWithShield_OnChange);

consvar_t cv_alwaysoverlayinvuln = CVAR_INIT ("alwaysoverlayinvincibility", "Off", CV_SAVE|CV_CALL, CV_OnOff, STAR_InvulnAndShield_OnChange);

// =========
// Savefiles
// =========
consvar_t cv_continues = CVAR_INIT ("continues", "Off", CV_SAVE|CV_CALL, CV_OnOff, STAR_UseContinues_OnChange);

// =======
// Servers
// =======
consvar_t cv_movingplayersetup = CVAR_INIT ("movingplayersetup", "Off", CV_SAVE, CV_OnOff, NULL);

// ------------------------ //
//        Functions
// ------------------------ //
#if 0
static void TSoURDt3rd_InitServerCommands(void)
{

}

static void TSoURDt3rd_InitClientCommands(void)
{
#if 0	
	// Game //
	CV_RegisterVar(&cv_startupscreen);
	CV_RegisterVar(&cv_stjrintro);

	CV_RegisterVar(&cv_loadingscreen);
	CV_RegisterVar(&cv_loadingscreenimage);

	CV_RegisterVar(&cv_quitscreen);

	CV_RegisterVar(&cv_isitcalledsingleplayer);

	CV_RegisterVar(&cv_tpsrate);

	CV_RegisterVar(&cv_menucolor);
	CV_RegisterVar(&cv_fpscountercolor);
	CV_RegisterVar(&cv_tpscountercolor);

	CV_RegisterVar(&cv_allowtypicaltimeover);
	CV_RegisterVar(&cv_pausegraphicstyle);
	CV_RegisterVar(&cv_automapoutsidedevmode);

	CV_RegisterVar(&cv_soniccd);

	// Savefiles //
	CV_RegisterVar(&cv_continues);

	// Servers //
	CV_RegisterVar(&cv_movingplayersetup);
#endif
}
#endif

void TSoURDt3rd_DiscordCommands_OnChange(void)
{
	UINT8 buf[3];

	if (!server)
		return;

	buf[0] = (UINT8)(min((dedicated ? MAXPLAYERS-1 : MAXPLAYERS), cv_maxplayers.value));
	buf[1] = (UINT8)cv_allownewplayer.value;
	buf[2] = (UINT8)cv_discordinvites.value;

	SendNetXCmd(XD_TSOURDT3RD, &buf, 3);
}

// ====
// Game
// ====
void STAR_TPSRate_OnChange(void)
{
	OP_Tsourdt3rdOptionsMenu[op_tpscountercolor].status = (cv_tpsrate.value ? IT_CVAR|IT_STRING : IT_GRAYEDOUT);
}

static void STAR_TimeOver_OnChange(void)
{
	if (netgame)
	{
		if (Playing())
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Sorry, you can't change this while in a netgame.\n");
		CV_StealthSetValue(&cv_allowtypicaltimeover, !cv_allowtypicaltimeover.value);
	}
}

// =======
// Players
// =======
static void STAR_SuperWithShield_OnChange(void)
{
	if (netgame)
	{
		if (Playing())
			STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Sorry, you can't change this while in a netgame.\n");
		CV_StealthSetValue(&cv_shieldblockstransformation, !cv_shieldblockstransformation.value);
	}
}

static void STAR_InvulnAndShield_OnChange(void)
{
#if 0
	if (players[consoleplayer].powers[pw_invulnerability] && ((players[consoleplayer].powers[pw_shield] & SH_NOSTACK) != SH_NONE))
	{
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Sorry, you can't change this while you have both invincibility and a shield.\n");
		CV_StealthSetValue(&cv_alwaysoverlayinvuln, !cv_alwaysoverlayinvuln.value);
	}
#endif
	return;
}

// =========
// Savefiles
// =========
static void STAR_UseContinues_OnChange(void)
{
	if (Playing())
		return;

	if (netgame || multiplayer)
		STAR_CONS_Printf(STAR_CONS_TSOURDT3RD_ALERT, "Please note that this only works in Singleplayer.\n");
	useContinues = cv_continues.value;
}
