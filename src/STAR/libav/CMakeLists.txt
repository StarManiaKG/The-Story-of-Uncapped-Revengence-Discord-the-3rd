set(TSOURDT3RD_CONFIG_HAVE_LIBAV OFF CACHE BOOL
	"TSoURDt3rd: Enable MP4 support.")

find_package(LibAV QUIET)

if(TARGET LibAV)
	target_compile_definitions(SRB2SDL2 PRIVATE -DHAVE_LIBAV)
	target_include_directories(SRB2SDL2 PRIVATE ${LibAV_INCLUDE_DIRS})
	target_link_libraries(SRB2SDL2 PRIVATE LibAV::LibAV)
	target_sources(SRB2SDL2 PRIVATE LIBAV_coders.c)
	message(STATUS "TSoURDt3rd: LibAV Found")
else()
	message(STATUS "TSoURDt3rd: No LibAV found")
endif()
