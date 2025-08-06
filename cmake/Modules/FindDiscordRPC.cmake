include(LibFindMacros)

libfind_pkg_check_modules(DiscordRPC_PKGCONF discord-rpc)

find_path(DiscordRPC_INCLUDE_DIR
	NAMES discord_rpc.h
	PATHS
		${DiscordRPC_PKGCONF_INCLUDE_DIRS}
		"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
		"/usr/include"
		"/usr/local/include"
)

find_library(DiscordRPC_LIBRARY
	NAMES discord-rpc
	PATHS
		${DiscordRPC_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DiscordRPC REQUIRED_VARS DiscordRPC_LIBRARY DiscordRPC_INCLUDE_DIR)

if(DiscordRPC_FOUND AND NOT TARGET DiscordRPC)
	add_library(DiscordRPC UNKNOWN IMPORTED)
	set_target_properties(
		DiscordRPC
		PROPERTIES
		IMPORTED_LOCATION "${DiscordRPC_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${DiscordRPC_INCLUDE_DIR}"
	)
	add_library(DiscordRPC::DiscordRPC ALIAS DiscordRPC)
endif()

#mark_as_advanced(DiscordRPC_LIBRARY DiscordRPC_INCLUDE_DIR)
