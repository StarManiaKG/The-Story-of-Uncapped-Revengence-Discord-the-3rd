include(LibFindMacros)

libfind_pkg_check_modules(DiscordGameSDK_PKGCONF discord_game_sdk)

find_path(DiscordGameSDK_INCLUDE_DIR
	NAMES discord_game_sdk.h
	PATHS
		${DiscordGameSDK_PKGCONF_INCLUDE_DIRS}
		"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
		"/usr/include"
		"/usr/local/include"
)

find_library(DiscordGameSDK_LIBRARY
	NAMES discord_game_sdk
	PATHS
		${DiscordGameSDK_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DiscordGameSDK
    REQUIRED_VARS DiscordGameSDK_LIBRARY DiscordGameSDK_INCLUDE_DIR)

if(DiscordGameSDK_FOUND AND NOT TARGET DiscordGameSDK)
	add_library(DiscordGameSDK UNKNOWN IMPORTED)
	set_target_properties(
		DiscordGameSDK
		PROPERTIES
		IMPORTED_LOCATION "${DiscordGameSDK_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${DiscordGameSDK_INCLUDE_DIR}"
	)
	add_library(DiscordGameSDK::DiscordGameSDK ALIAS DiscordGameSDK)
endif()

#mark_as_advanced(DiscordGameSDK_LIBRARY DiscordGameSDK_INCLUDE_DIR)
