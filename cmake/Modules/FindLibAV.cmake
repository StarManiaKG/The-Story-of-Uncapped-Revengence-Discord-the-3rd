include(LibFindMacros)

libfind_pkg_check_modules(LibAV_PKGCONF LibAV)

find_path(LibAV_INCLUDE_DIR
	NAMES avcodec.h
	PATHS
		${LibAV_PKGCONF_INCLUDE_DIRS}
		"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include"
		"/usr/include"
		"/usr/local/include"
)

find_library(LibAV_LIBRARY
	NAMES libavcodec
	PATHS
		${LibAV_PKGCONF_LIBRARY_DIRS}
		"/usr/lib"
		"/usr/local/lib"
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibAV
    REQUIRED_VARS LibAV_LIBRARY LibAV_INCLUDE_DIR)

if(LibAV_FOUND AND NOT TARGET LibAV)
	add_library(LibAV UNKNOWN IMPORTED)
	set_target_properties(
		LibAV
		PROPERTIES
		IMPORTED_LOCATION "${LibAV_LIBRARY}"
		INTERFACE_INCLUDE_DIRECTORIES "${LibAV_INCLUDE_DIR}"
	)
	add_library(LibAV::LibAV ALIAS LibAV)
endif()

mark_as_advanced(LibAV_LIBRARY LibAV_INCLUDE_DIR)
