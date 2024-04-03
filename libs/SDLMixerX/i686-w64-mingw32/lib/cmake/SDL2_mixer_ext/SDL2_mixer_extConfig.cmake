include(FeatureSummary)
set_package_properties(SDL2_mixer_ext PROPERTIES
  URL "https://wohlsoft.github.io/SDL-Mixer-X"
  DESCRIPTION "SDL Mixer X, a fork of SDL_mixer, a mixer library for Simple DirectMedia Layer"
)


####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was SDL2_mixer_extConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer_ext-shared-targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer_ext-shared-targets.cmake")
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer_ext-static-targets.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/SDL2_mixer_ext-static-targets.cmake")
endif()
