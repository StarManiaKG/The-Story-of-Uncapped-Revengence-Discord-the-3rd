# Find SDL2_MixerX
# Once done, this will define
#
#  SDL2_MIXERX_FOUND - system has SDL2_MixerX
#  SDL2_MIXERX_INCLUDE_DIRS - SDL2_MixerX include directories
#  SDL2_MIXERX_LIBRARIES - link libraries

include(LibFindMacros)

libfind_pkg_check_modules(SDL2_MIXERX_PKGCONF SDL2_mixerx)

# includes
find_path(SDL2_MIXERX_INCLUDE_DIR
	NAMES SDL_mixer_ext.h
	PATHS
		${SDL2_MIXERX_PKGCONF_INCLUDE_DIRS}
		"/usr/include/SDL2"
		"/usr/local/include/SDL2"
)

# library
find_library(SDL2_MIXERX_LIBRARY
	NAMES SDL2_mixer_ext
	PATHS
		${SDL2_MIXERX_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)


# set include dir variables
set(SDL2_MIXERX_PROCESS_INCLUDES SDL2_MIXERX_INCLUDE_DIR)
set(SDL2_MIXERX_PROCESS_LIBS SDL2_MIXERX_LIBRARY)
libfind_process(MIXERX)
