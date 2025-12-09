#
# StarManiaKG:
#
# This new version of the portfile allows us to have the ability to speed up more music formats.
# Plus, I allowed for a couple more features to be added.
# That's it. All of my combined day-and-a-half's work was for this.
#
# ------------------------------------------------------------------
#
# At the time of writing this, the version we're using is:
#	- Commit: '88e7172ab293449efbf3439a87b7bc1dd9eb3ccf'
#	- SHA512: 'ba6d09d3c7aced2963420184562e5bff23b4ca7a952fd2790bf4c8ab0547ecfdfaef57daa85e54a6fd0f86ac79225ae897f9d99bc25fe18ec5385e1353aeefab'
#
# This is the latest stable version on the master branch.
# Therefore, I'm using those variables. If you want though, you can modify this.
#
# ------------------------------------------------------------------
#
# In case you need to know,
# VCPKG's default version is:
#   - REF:    '2.6.0'
#   - SHA512: 'bdb39122ecf8492723615421c37c0d2a8d5958110d7bf2f0a01f5c54cc1f3f6e9a54887df7d348e9dc7e34906cff67794b0f5d61ca6fe5e4019f84ed88cf07e5'
#

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO WohlSoft/SDL-Mixer-X
    REF "${VERSION}"
    SHA512 ba6d09d3c7aced2963420184562e5bff23b4ca7a952fd2790bf4c8ab0547ecfdfaef57daa85e54a6fd0f86ac79225ae897f9d99bc25fe18ec5385e1353aeefab
    HEAD_REF master
    PATCHES
        fix-dependencies.patch
)

file(REMOVE
    "${SOURCE_PATH}/cmake/find/FindOGG.cmake" # Conflicts with official configurations
    "${SOURCE_PATH}/cmake/find/FindFFMPEG.cmake" # Using FindFFMPEG.cmake provided by vcpkg
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        libvorbis      USE_OGG_VORBIS
		stbvorbis      USB_OGG_VORBIS_STB
        opusfile       USE_OPUS
        libflac        USE_FLAC
        wavpack        USE_WAVPACK
        mpg123         USE_MP3_MPG123
        libmodplug     USE_MODPLUG
        libxmp         USE_XMP
        libgme         USE_GME
        ffmpeg         USE_FFMPEG
        pxtone         USE_PXTONE
        cmd            USE_CMD
        libadlmidi     USE_MIDI_ADLMIDI
        libedmidi      USE_MIDI_EDMIDI
        libopnmidi     USE_MIDI_OPNMIDI
        timidity       USE_MIDI_TIMIDITY
        fluidlite      USE_MIDI_FLUIDLITE
        fluidsynth     USE_MIDI_FLUIDSYNTH
        nativemidi     USE_MIDI_NATIVE_ALT
        nativemidi     USE_MIDI_NATIVE
)

if("libadlmidi"     IN_LIST FEATURES OR 
    "libedmidi"     IN_LIST FEATURES OR 
    "libopnmidi"    IN_LIST FEATURES OR 
    "timidity"      IN_LIST FEATURES OR 
    "fluidlite"     IN_LIST FEATURES OR 
    "fluidsynth"    IN_LIST FEATURES OR 
    "nativemidi"    IN_LIST FEATURES)
    set(USE_MIDI ON)
else()
    set(USE_MIDI OFF)
endif()

if("libvorbis" IN_LIST FEATURES OR "stbvorbis" IN_LIST FEATURES)
    set(USE_OGG_VORBIS ON)
endif()

if("fluidlite" IN_LIST FEATURES OR "fluidsynth" IN_LIST FEATURES)
    vcpkg_find_acquire_program(PKGCONFIG)
    list(APPEND EXTRA_OPTIONS "-DPKG_CONFIG_EXECUTABLE=${PKGCONFIG}")
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        ${EXTRA_OPTIONS}
		#-DSDL_MIXER_X_SHARED=${SRB2_CONFIG_SHARED_INTERNAL_LIBRARIES}
		#-DSDL_MIXER_X_STATIC=${NOT_SRB2_CONFIG_SHARED_INTERNAL_LIBRARIES}
		#-DSDL2_INCLUDE_PATH=${SDL2_INCLUDE_DIR}
		-DSDL_MIXER_X_DISABLE_INSTALL=OFF
        -DMIXERX_ENABLE_GPL=ON
        -DMIXERX_ENABLE_LGPL=ON
        -DUSE_SYSTEM_SDL2=ON
        -DUSE_SYSTEM_AUDIO_LIBRARIES=ON
        -DUSE_DRFLAC=OFF
        -DUSE_FFMPEG_DYNAMIC=OFF
        -DUSE_MP3_DRMP3=OFF
        -DUSE_MIDI=${USE_MIDI}
        -DUSE_MIDI_EDMIDI=${USE_MIDI_EDMIDI}
        -DUSE_MIDI_FLUIDLITE=${USE_MIDI_FLUIDLITE}
        -DUSE_MIDI_FLUIDSYNTH=${USE_MIDI_FLUIDSYNTH}
        -DUSE_OGG_VORBIS=${USE_OGG_VORBIS}
        -DUSE_OGG_VORBIS_STB=${USE_OGG_VORBIS_STB}
        -USE_WAVPACK_DSD=ON
    MAYBE_UNUSED_VARIABLES
		#SDL2_INCLUDE_PATH
        USE_CMD
        USE_FFMPEG_DYNAMIC
        USE_WAVPACK_DSD
        USE_MIDI_NATIVE
        USE_MIDI_NATIVE_ALT
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(
    PACKAGE_NAME SDL2_mixer_ext
    CONFIG_PATH lib/cmake/SDL2_mixer_ext)

vcpkg_fixup_pkgconfig()

vcpkg_copy_pdbs()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/share"
    "${CURRENT_PACKAGES_DIR}/debug/include"
)

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

set(LICENSE_FILES
    "${SOURCE_PATH}/COPYING.txt"
    "${SOURCE_PATH}/GPLv2.txt"
    "${SOURCE_PATH}/GPLv3.txt"
    "${SOURCE_PATH}/SDL2_mixer_ext.License.txt"
)

vcpkg_install_copyright(FILE_LIST ${LICENSE_FILES})
