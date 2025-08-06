option(TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC "TSoURDt3rd: Enable Discord Rich Presence support." ON)
option(TSOURDT3RD_CONFIG_ENABLE_DISCORDGAMESDK "TSoURDt3rd: Enable Discord Game SDK support." ON)
set(TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT OFF)

find_package(DiscordRPC CONFIG QUIET)
	if (TARGET DiscordRPC)
		message(STATUS "TSoURDt3rd: Using DiscordRPC as Discord layer")
		set(TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT ON)
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDSUPPORT -DHAVE_DISCORDRPC)
		#target_link_libraries(SRB2SDL2 PRIVATE DiscordRPC)
		#target_link_libraries(SRB2SDL2 PRIVATE DiscordRPC::DiscordRPC)
		target_link_libraries(SRB2SDL2 PRIVATE discord-rpc)
		#target_include_directories(SRB2SDL2 PRIVATE ${DiscordRPC_INCLUDE_DIRS})
		add_subdirectory(rpc)
	else()
		message(STATUS "TSoURDt3rd: No DiscordRPC found")
		set(TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC OFF)
	endif()

find_package(DiscordGameSDK QUIET)
	if (TARGET DiscordGameSDK)
		message(STATUS "TSoURDt3rd: Using DiscordGameSDK as Discord layer")
		set(TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT ON)
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDSUPPORT -DHAVE_DISCORDGAMESDK)
		#target_link_libraries(SRB2SDL2 PRIVATE DiscordGameSDK)
		target_link_libraries(SRB2SDL2 PRIVATE DiscordGameSDK::DiscordGameSDK)
		#target_include_directories(SRB2SDL2 PRIVATE ${DiscordGameSDK_INCLUDE_DIRS})
		add_subdirectory(gamesdk)
	else()
		message(STATUS "TSoURDt3rd: No DiscordGameSDK found")
		set(TSOURDT3RD_CONFIG_ENABLE_DISCORDGAMESDK OFF)
	endif()

if (${TSOURDT3RD_CONFIG_ENABLE_DISCORDGAMESDK} AND ${TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC})
	message(FATAL_ERROR "TSoURDt3rd: Can't compile with both Discord's Rich Presence and Discord's Game SDK!")
endif()

if (${TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT})
	target_sources(SRB2SDL2 PRIVATE
		discord.h
		discord_common.c
		discord_cmds.c
		discord_statuses.c
	)
endif()
target_sources(SRB2SDL2 PRIVATE discord_net.c)
