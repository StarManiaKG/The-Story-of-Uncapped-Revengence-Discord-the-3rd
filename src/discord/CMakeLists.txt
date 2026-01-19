target_sources(SRB2SDL2 PRIVATE discord_net.c)

set(DRPC_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rpc")
set(DGAMESDK_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/gamekdk")

if (${TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT})
	target_sources(SRB2SDL2 PRIVATE
		discord_common.c
		discord_cmds.c
		discord_statuses.c
	)
	if(TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC)
		target_sources(SRB2SDL2 PRIVATE
			${DRPC_SOURCE_DIRECTORY}/discord_rpc.c
		)
	elseif(TSOURDT3RD_CONFIG_ENABLE_DISCORDGAMESDK)
		target_sources(SRB2SDL2 PRIVATE
			${DGAMESDK_SOURCE_DIRECTORY}/discord_gamesdk.c
		)
	endif()
endif()
