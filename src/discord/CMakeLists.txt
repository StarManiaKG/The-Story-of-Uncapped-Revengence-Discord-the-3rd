target_sources(SRB2SDL2 PRIVATE discord_net.c)

if (${TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT})
	target_sources(SRB2SDL2 PRIVATE
		discord_common.c
		discord_cmds.c
		discord_statuses.c
	)
	if(TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC)
		add_subdirectory(rpc)
	elseif(TSOURDT3RD_CONFIG_ENABLE_DISCORDGAMESDK)
		add_subdirectory(gamesdk)
	endif()
endif()
