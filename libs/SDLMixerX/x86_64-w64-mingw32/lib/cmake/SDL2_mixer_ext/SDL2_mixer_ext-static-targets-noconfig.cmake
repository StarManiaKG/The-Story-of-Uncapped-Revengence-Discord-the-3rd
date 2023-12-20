#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SDL2_mixer_ext::SDL2_mixer_ext_Static" for configuration ""
set_property(TARGET SDL2_mixer_ext::SDL2_mixer_ext_Static APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(SDL2_mixer_ext::SDL2_mixer_ext_Static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C;CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libSDL2_mixer_ext.a"
  )

list(APPEND _cmake_import_check_targets SDL2_mixer_ext::SDL2_mixer_ext_Static )
list(APPEND _cmake_import_check_files_for_SDL2_mixer_ext::SDL2_mixer_ext_Static "${_IMPORT_PREFIX}/lib/libSDL2_mixer_ext.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
