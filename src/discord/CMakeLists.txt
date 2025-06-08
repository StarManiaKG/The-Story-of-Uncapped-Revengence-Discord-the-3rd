set(TSOURDT3RD_CONFIG_HAVE_DISCORDRPC OFF CACHE BOOL
	"TSoURDt3rd: Enable Discord Rich Presence support.")

set(TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK OFF CACHE BOOL
	"TSoURDt3rd: Enable Discord Game SDK support.")

find_package(DiscordGameSDK QUIET)
find_package(DiscordRPC QUIET)

if(TARGET DiscordGameSDK)
	message(STATUS "TSoURDt3rd: DiscordGameSDK Found")
else()
	message(STATUS "TSoURDt3rd: No DiscordGameSDK found")
	set(TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK OFF)
endif()

if(TARGET DiscordRPC)
	message(STATUS "TSoURDt3rd: DiscordRPC Found")
else()
	message(STATUS "TSoURDt3rd: No DiscordRPC found")
	set(TSOURDT3RD_CONFIG_HAVE_DISCORDRPC OFF)
endif()

if(${TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK} AND ${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC})
	message(WARNING "TSoURDt3rd: Can't compile with Discord's Game SDK and Discord's Rich Presence!")
	set(TSOURDT3RD_CONFIG_HAVE_DISCORDRPC OFF)
elseif (${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC} AND ${TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK})
	message(WARNING "TSoURDt3rd: Can't compile with Discord's Rich Presence and Discord's Game SDK!")
	set(TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK OFF)
endif()

if(TARGET DiscordGameSDK AND ${TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK})
	target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDSUPPORT -DHAVE_DISCORDGAMESDK)
	target_sources(SRB2SDL2 PRIVATE discord_gamesdk.c discord_gamesdk.h)
	target_link_libraries(SRB2SDL2 PRIVATE DiscordGameSDK::DiscordGameSDK)
	target_include_directories(SRB2SDL2 PRIVATE ${DiscordGameSDK_INCLUDE_DIRS})
	message(STATUS "TSoURDt3rd: Using DiscordGameSDK as Discord layer")
elseif(TARGET DiscordRPC AND ${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC})
	target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDSUPPORT -DHAVE_DISCORDRPC)
	target_sources(SRB2SDL2 PRIVATE discord.c discord.h)
	target_link_libraries(SRB2SDL2 PRIVATE DiscordRPC::DiscordRPC)
	target_include_directories(SRB2SDL2 PRIVATE ${DiscordRPC_INCLUDE_DIRS})
	message(STATUS "TSoURDt3rd: Using DiscordRPC as Discord layer")
endif()

if(${TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK} AND ${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC})
	target_sources(SRB2SDL2 PRIVATE discord_statuses.c discord_cmds.c)
endif()
target_sources(SRB2SDL2 PRIVATE discord_net.c)
