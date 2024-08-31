## 	Commands
set(SRB2_CONFIG_HAVE_DISCORDRPC OFF CACHE BOOL
	"Enable Discord Rich Presence support.")

set(SRB2_CONFIG_HAVE_DISCORDGAMESDK OFF CACHE BOOL
	"Enable Discord Game SDK support.")

##	Libraries
#	Discord Game SDK
if(${SRB2_CONFIG_HAVE_DISCORDGAMESDK})
	if (${SRB2_CONFIG_HAVE_DISCORDRPC})
		message(WARNING "You can't compile SRB2 with both Discord's Game SDK and Discord's Rich Presence!")
		set(SRB2_CONFIG_HAVE_DISCORDRPC OFF)
	endif()

	if(${SRB2_CONFIG_USE_INTERNAL_LIBRARIES})
		set(DISCORDGAMESDK_FOUND ON)
		set(DISCORDGAMESDK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/discord-game-sdk/include)

		if(${SRB2_SYSTEM_BITS} EQUAL 64)
			set(DISCORDGAMESDK_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/discord-game-sdk/x86_64")
		else()
			set(DISCORDGAMESDK_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/discord-game-sdk/x86")
		endif()
	else()
		find_package(DiscordGameSDK)
	endif()

	if(${DISCORDGAMESDK_FOUND})
		set(SRB2_HAVE_DISCORDGAMESDK ON)
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDGAMESDK -DUSE_STUN -DHAVE_DISCORDSUPPORT)
		add_subdirectory(discord)
	else()
		message(WARNING "You have specified that Discord's Game SDK is available but it was not found.")
	endif()
endif()

#   TEST
if(TARGET DISCORD::discordgamesdk)
	target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_CURL)
	target_link_libraries(SRB2SDL2 PRIVATE CURL::libcurl)
	message(STATUS "libcurl Found")
else()
	message(STATUS "No libcurl Found")
endif()

#	Discord Rich Presence
if(${SRB2_CONFIG_HAVE_DISCORDRPC})
	if (${SRB2_CONFIG_HAVE_DISCORDGAMESDK})
		message(WARNING "You can't compile SRB2 with both Discord's Rich Presence SDK and Discord's Game SDK!")
		set(SRB2_CONFIG_HAVE_DISCORDGAMESDK OFF)
	endif()

	if(${SRB2_CONFIG_USE_INTERNAL_LIBRARIES})
		set(DISCORDRPC_FOUND ON)
		set(DISCORDRPC_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/discord-rpc/win32-dynamic/include)

		if(${SRB2_SYSTEM_BITS} EQUAL 64)
			set(DISCORDRPC_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/discord-rpc/win64-dynamic/include)
			set(DISCORDRPC_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/discord-rpc/win64-dynamic/lib -ldiscord-rpc")
		else()
			set(DISCORDRPC_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/discord-rpc/win32-dynamic/include)
			set(DISCORDRPC_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/discord-rpc/win32-dynamic/lib -ldiscord-rpc")
		endif()
	else()
		find_package(DiscordRPC)
	endif()

	if(${DISCORDRPC_FOUND})
		set(SRB2_HAVE_DISCORDRPC ON)
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_DISCORDRPC -DUSE_STUN -DHAVE_DISCORDSUPPORT)
		add_subdirectory(discord)
	else()
		message(WARNING "You have specified that Discord Rich Presence is available but it was not found.")
	endif()
endif()

## 	Source Files
target_sources(SRB2SDL2 PRIVATE
    discord_statuses.c
    discord.c
    stun.c
)
