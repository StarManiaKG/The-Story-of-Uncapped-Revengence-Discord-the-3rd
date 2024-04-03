
#include "p_saveg.h"
#include "../byteptr.h"
#include "../doomstat.h"
#include "../i_net.h"
#include "../d_main.h"
#include "../p_local.h"
#include "../z_zone.h"

#include "ss_main.h" // TSOURDT3RD_DEBUGGING //

static void Write(INT32 playernum, boolean archive)
{
#ifdef TSOURDT3RD_DEBUGGING
	FILE *f;
	const char *path;
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	if (archive)
		path = va("%s"PATHSEP"%s", srb2home, "STAR_bye.txt");
	else
		path = va("%s"PATHSEP"%s", srb2home, "STAR_hi.txt");
	f = fopen(path, (!playernum ? "w+" : "a+"));

	fputs(va("CURRENT TIC: %d\n", gametic), f);
	if (archive)
		fputs("Type: SENDING!\n", f);
	else
		fputs("Type: RECEIVING!\n", f);

	fputs(va("\nName: %s\n", cv_playername.string), f);
	fputs(va("Player: %d\n", playernum), f);

	fputs(va("%d\n", TSoURDt3rd->checkedVersion), f);
	fputs(va("%d\n", TSoURDt3rd->usingTSoURDt3rd), f);

	fputs(va("%d\n", TSoURDt3rd->num), f);

	fputs(va("%d\n", TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd), f);
	fputs(va("%d\n", TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion), f);

	fclose(f);
#else
	(void)playernum;
	(void)archive;
	return;
#endif
}

UINT8 TSOURDT3RD_READUINT8(UINT8 *save_p, TSoURDt3rd_t *TSoURDt3rd, UINT8 fallback)
{
	if (!TSoURDt3rd || !TSoURDt3rd->usingTSoURDt3rd || !netbuffer->u.servercfg.tsourdt3rd)
	    return fallback;
	return READUINT8(save_p);
}

UINT32 TSOURDT3RD_READUINT32(UINT8 *save_p, TSoURDt3rd_t *TSoURDt3rd, UINT32 fallback)
{
	if (!TSoURDt3rd || !TSoURDt3rd->usingTSoURDt3rd || !netbuffer->u.servercfg.tsourdt3rd)
	    return fallback;
	return READUINT32(save_p);
}

void TSoURDt3rd_NetArchiveUsers(UINT8 *save_p, INT32 playernum)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	WRITEUINT8(save_p, TSoURDt3rd->usingTSoURDt3rd);
	WRITEUINT8(save_p, TSoURDt3rd->checkedVersion);

	WRITEUINT8(save_p, TSoURDt3rd->num);

	WRITEUINT8(save_p, TSoURDt3rd->reachedSockSendErrorLimit);
	WRITEUINT8(save_p, TSoURDt3rd->masterServerAddressChanged);

	WRITEUINT8(save_p, TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd);

	WRITEUINT8(save_p, TSoURDt3rd->serverPlayers.majorVersion);
	WRITEUINT8(save_p, TSoURDt3rd->serverPlayers.minorVersion);
	WRITEUINT8(save_p, TSoURDt3rd->serverPlayers.subVersion);

	WRITEUINT32(save_p, TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion);

	Write(playernum, true);
}

void TSoURDt3rd_NetUnArchiveUsers(UINT8 *save_p, INT32 playernum)
{
	TSoURDt3rd_t *TSoURDt3rd = &TSoURDt3rdPlayers[playernum];

	TSoURDt3rd->usingTSoURDt3rd = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, false);
	TSoURDt3rd->checkedVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, true);

	TSoURDt3rd->num = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, playernum+1);

	TSoURDt3rd->reachedSockSendErrorLimit = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, 0);
	TSoURDt3rd->masterServerAddressChanged = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, false);

	TSoURDt3rd->serverPlayers.serverUsesTSoURDt3rd = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, false);

	TSoURDt3rd->serverPlayers.majorVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentMajorVersion());
	TSoURDt3rd->serverPlayers.minorVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentMinorVersion());
	TSoURDt3rd->serverPlayers.subVersion = TSOURDT3RD_READUINT8(save_p, TSoURDt3rd, TSoURDt3rd_CurrentSubversion());

	TSoURDt3rd->serverPlayers.serverTSoURDt3rdVersion = TSOURDT3RD_READUINT32(save_p, TSoURDt3rd, TSoURDt3rd_CurrentVersion());

	Write(playernum, false);
}
