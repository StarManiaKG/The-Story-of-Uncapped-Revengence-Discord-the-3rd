include(LibFindMacros)

libfind_pkg_check_modules(LIBAV_PKGCONF LIBAV)

find_path(LIBAV_INCLUDE_DIR
	NAMES avcodec.h
	PATHS
		${LIBAV_PKGCONF_INCLUDE_DIRS}
		"/usr/include"
		"/usr/local/include"
)

find_library(LIBAV_LIBRARY
	NAMES libavcodec
	PATHS
		${LIBAV_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)

set(LIBAV_PROCESS_INCLUDES LIBAV_INCLUDE_DIR)
set(LIBAV_PROCESS_LIBS LIBAV_LIBRARY)

libfind_process(LIBAV)
