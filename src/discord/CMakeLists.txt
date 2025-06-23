set(TSOURDT3RD_CONFIG_HAVE_DISCORDRPC ON CACHE BOOL
	"TSoURDt3rd: Enable Discord Rich Presence support.")

set(TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK OFF CACHE BOOL
	"TSoURDt3rd: Enable Discord Game SDK support.")

find_package(DiscordRPC QUIET)
	if(TARGET DiscordRPC)
		message(STATUS "TSoURDt3rd: DiscordRPC Found")
	else()
		message(STATUS "TSoURDt3rd: No DiscordRPC found")
		set(TSOURDT3RD_CONFIG_HAVE_DISCORDRPC OFF)
	endif()

find_package(DiscordGameSDK QUIET)
	if(TARGET DiscordGameSDK)
		message(STATUS "TSoURDt3rd: DiscordGameSDK Found")
	else()
		message(STATUS "TSoURDt3rd: No DiscordGameSDK found")
		set(TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK OFF)
	endif()

if (${TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK})
	if (${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC})
		message(WARNING "TSoURDt3rd: Can't compile with Discord's Rich Presence and Discord's Game SDK!")
		set(TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK OFF)
	endif()
	if (TARGET DiscordRPC)
		message(STATUS "TSoURDt3rd: Using DiscordRPC as Discord layer")
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDSUPPORT -DHAVE_DISCORDRPC)
		target_link_libraries(SRB2SDL2 PRIVATE DiscordRPC::DiscordRPC)
		target_include_directories(SRB2SDL2 PRIVATE ${DiscordRPC_INCLUDE_DIRS})
		add_subdirectory(rpc)
	endif()
else()
	if (${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC})
		message(WARNING "TSoURDt3rd: Can't compile with Discord's Game SDK and Discord's Rich Presence!")
		set(TSOURDT3RD_CONFIG_HAVE_DISCORDRPC OFF)
	endif()
	if (TARGET DiscordGameSDK)
		message(STATUS "TSoURDt3rd: Using DiscordGameSDK as Discord layer")
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDSUPPORT -DHAVE_DISCORDGAMESDK)
		target_link_libraries(SRB2SDL2 PRIVATE DiscordGameSDK::DiscordGameSDK)
		target_include_directories(SRB2SDL2 PRIVATE ${DiscordGameSDK_INCLUDE_DIRS})
		add_subdirectory(gamesdk)
	endif()
endif()

if (${TSOURDT3RD_CONFIG_HAVE_DISCORDGAMESDK} OR ${TSOURDT3RD_CONFIG_HAVE_DISCORDRPC})
	target_sources(SRB2SDL2 PRIVATE
		discord.h
		discord_common.c
		discord_cmds.c
		discord_statuses.c
	)
endif()
target_sources(SRB2SDL2 PRIVATE discord_net.c)
