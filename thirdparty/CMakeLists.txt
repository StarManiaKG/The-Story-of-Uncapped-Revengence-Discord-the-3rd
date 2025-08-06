
include("Ccache.cmake")

if(TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC)
	add_subdirectory(discord-rpc)
endif()
