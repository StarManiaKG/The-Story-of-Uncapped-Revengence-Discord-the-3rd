target_sources(SRB2SDL2 PRIVATE discord_net.c)

if (${TSOURDT3RD_CONFIG_HAVE_DISCORDSUPPORT})
	target_sources(SRB2SDL2 PRIVATE
		discord.h
		discord_common.c
		discord_cmds.c
		discord_statuses.c
	)

	if (TARGET DiscordRPC)
		add_subdirectory(rpc)
	elseif (TARGET DiscordGameSDK)
		add_subdirectory(gamesdk)
	endif()
endif()
