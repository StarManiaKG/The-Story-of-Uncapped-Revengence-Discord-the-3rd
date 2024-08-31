## 	Commands
set(SRB2_CONFIG_HAVE_LIBAV OFF CACHE BOOL
	"Enable TSoURDt3rd's MP4 support.")

##  Libraries
#	LibAV
if(${SRB2_CONFIG_HAVE_LIBAV})
	if(${SRB2_CONFIG_USE_INTERNAL_LIBRARIES})
		set(SRB2_LIBAV_FOUND ON)

		if(${SRB2_SYSTEM_BITS} EQUAL 64)
			set(LIBAV_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/libav/x86_64-w64-mingw32/include)
			set(LIBAV_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/libav/x86_64-w64-mingw32/lib -lm -lz -lvfw32 -lws2_32 -lbcrypt -luser32 -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale")
		else()
			set(LIBAV_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libs/libav/i686-w64-mingw32/include)
			set(LIBAV_LIBRARIES "-L${CMAKE_SOURCE_DIR}/libs/libav/i686-w64-mingw32/lib -lm -lz -lvfw32 -lws2_32 -lbcrypt -luser32 -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale")
		endif()
	else()
		find_package(LibAV)
	endif()

	if(${SRB2_LIBAV_FOUND})
		set(SRB2_HAVE_LIBAV ON)
		target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_LIBAV)
		target_sources(SRB2SDL2 PRIVATE libav/LIBAV_coders.c)
	else()
		message(WARNING "You have specified that LibAV is avaliable but it was not found.")
	endif()
endif()

## 	Source Files
target_sources(SRB2SDL2 PRIVATE
    LIBAV_coders.c
)
