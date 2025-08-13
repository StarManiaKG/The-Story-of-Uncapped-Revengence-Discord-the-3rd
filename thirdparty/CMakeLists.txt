
include("Ccache.cmake")

if(SRB2_CONFIG_EXECINFO AND "${SRB2_CONFIG_EXECTYPE}" MATCHES libbacktrace)
	add_subdirectory(libbacktrace)
endif()

if(TSOURDT3RD_CONFIG_ENABLE_DISCORDRPC)
	add_subdirectory(discord-rpc)
endif()
