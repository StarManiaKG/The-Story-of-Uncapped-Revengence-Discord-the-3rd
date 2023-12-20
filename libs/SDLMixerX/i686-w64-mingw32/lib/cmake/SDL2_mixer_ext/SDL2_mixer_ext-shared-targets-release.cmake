#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SDL2_mixer_ext::SDL2_mixer_ext" for configuration "Release"
set_property(TARGET SDL2_mixer_ext::SDL2_mixer_ext APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SDL2_mixer_ext::SDL2_mixer_ext PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libSDL2_mixer_ext.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/SDL2_mixer_ext.dll"
  )

list(APPEND _cmake_import_check_targets SDL2_mixer_ext::SDL2_mixer_ext )
list(APPEND _cmake_import_check_files_for_SDL2_mixer_ext::SDL2_mixer_ext "${_IMPORT_PREFIX}/lib/libSDL2_mixer_ext.dll.a" "${_IMPORT_PREFIX}/bin/SDL2_mixer_ext.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
