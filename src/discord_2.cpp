#ifdef HAVE_DISCORDGAMESDK

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "discord/discord.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

#include "i_system.h"
#include "d_clisrv.h"
#include "d_netcmd.h"
#include "i_net.h"
#include "g_game.h"
#include "p_tick.h"
#include "m_menu.h" // gametype_cons_t
#include "r_things.h" // skins
#include "mserv.h" // ms_RoomId
#include "m_cond.h" // queries about emblems
#include "z_zone.h"
#include "byteptr.h"
#include "stun.h"
#include "i_tcp.h" // current_port
#include "discord_2.h" // discord
#include "doomdef.h" // large list of defs, mainly

// Please feel free to provide your own Discord app if you're making a new custom build :)

#endif //HAVE_DISCORDGAMESDK