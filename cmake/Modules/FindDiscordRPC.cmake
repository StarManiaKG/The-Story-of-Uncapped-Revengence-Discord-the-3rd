include(LibFindMacros)

libfind_pkg_check_modules(DISCORDRPC_PKGCONF DISCORDRPC)
libfind_pkg_check_modules(DISCORDGAMESDK_PKGCONF DISCORDGAMESDK)

find_path(DISCORDRPC_INCLUDE_DIR
	NAMES discord_rpc.h
	PATHS
		${DISCORDRPC_PKGCONF_INCLUDE_DIRS}
		"/usr/include"
		"/usr/local/include"
)

find_library(DISCORDRPC_LIBRARY
	NAMES discord-rpc
	PATHS
		${DISCORDRPC_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
	
	NAMES discord-rpc
	PATHS
		${DISCORDRPC_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)

set(DISCORDRPC_PROCESS_INCLUDES DISCORDRPC_INCLUDE_DIR)
set(DISCORDRPC_PROCESS_LIBS DISCORDRPC_LIBRARY)

find_library(DISCORDGAMESDK_LIBRARY
	NAMES discord_game_sdk
	PATHS
		${DISCORDGAMESDK_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)

set(DISCORDGAMESDK_PKGCONF_LIBRARY_DIRS DISCORDGAMESDK_LIBRARY)

libfind_process(DISCORDRPC)
libfind_process(DISCORDGAMESDK)
