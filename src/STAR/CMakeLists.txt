## 	Commands
set(SRB2_CONFIG_HAVE_MIXERX OFF CACHE BOOL
	"Enable advanced sound options With SDL Mixer X support.")

## 	Directories
add_subdirectory(libav)
add_subdirectory(monocypher)

##  Libraries
#	MixerX
if(${SRB2_CONFIG_HAVE_MIXERX})
	if(${SRB2_CONFIG_USE_INTERNAL_LIBRARIES})
		set(SRB2_MIXERX_FOUND ON)

		if(${SRB2_SYSTEM_BITS} EQUAL 64)
			set(MIXERX_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/SDLMixerX/x86_64-w64-mingw32/include/SDL2)
			set(MIXERX_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/SDLMixerX/x86_64-w64-mingw32/lib -lSDL2_mixer_ext")
		else()
			set(MIXERX_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/SDLMixerX/i686-w64-mingw32/include/SDL2)
			set(MIXERX_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/SDLMixerX/i686-w64-mingw32/lib -lSDL2_mixer_ext")
		endif()
	else()
		find_package(MIXERX)
	endif()

	if(${SRB2_MIXERX_FOUND})
		set(SRB2_HAVE_MIXERX ON)
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_MIXERX)
	else()
		message(WARNING "You have specified that SDL Mixer X is available but it was not found.")
	endif()
endif()

## 	Source Files
target_sources(SRB2SDL2 PRIVATE
    ../discord/discord_cmds.c
    ss_main.c
    smkg-cvars.c
    smkg_g_inputs.c
    smkg-misc.c
    smkg-misc_purefat.c
    smkg-jukebox.c
    smkg-p_saveg.c
    smkg-st_hud.c
    star_functions.c
    m_menu.c
    p_user.c
    curl/smkg-curl.c
    curl/smkg-curl_routines.c
    drrr/k_console.c
    drrr/kg_input.c
    drrr/km_menudraw.c
    drrr/km_menufunc.c
    events/smkg-easter.c
    events/smkg-aprilfools.c
    lights/smkg-lights.c
    lights/smkg-r_coronas.c
    menus/smkg_m_func.c
    menus/smkg_m_draw.c
    menus/menudefs/smkg-multi-playersetup.c
    menus/menudefs/smkg-options-discord.c
    menus/menudefs/smkg-options-tsourdt3rd.c
    menus/menudefs/smkg-options-tsourdt3rd-events.c
    menus/menudefs/smkg-options-tsourdt3rd-game.c
    menus/menudefs/smkg-options-tsourdt3rd-controls.c
    menus/menudefs/smkg-options-tsourdt3rd-video.c
    menus/menudefs/smkg-options-tsourdt3rd-audio.c
    menus/menudefs/smkg-options-tsourdt3rd-players.c
    menus/menudefs/smkg-options-tsourdt3rd-savefiles.c
    menus/menudefs/smkg-options-tsourdt3rd-server.c
    menus/menudefs/smkg-options-tsourdt3rd-jukebox.c
    menus/menudefs/smkg-options-tsourdt3rd-extras.c
    menus/menudefs/transient/drrr-message-box.c
    menus/menudefs/transient/drrr-virtual-keyboard.c
    menus/menudefs/transient/smkg-options-tsourdt3rd-extras-snake.c
    monocypher/monocypher.c
    monocypher/smkg-hasher.c
    netcode/smkg-d_clisrv.c
    netcode/smkg-i_protocol.c
    parser/parser.c
    parser/parsed.c
    sdl/smkg-i_vid.c
    sdl/smkg-i_sys.c
)
