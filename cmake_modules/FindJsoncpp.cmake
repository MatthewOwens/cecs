include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Jsoncpp_PKGCONF} jsoncpp)

find_path(Jsoncpp_INCLUDE_DIR
	NAMES json/features.h
	PATH_SUFFIXES jsoncpp
	PATHS ${Jsoncpp_PKGCONF_INCLUDE_DIRS}
	)

# Finally the library itself
find_library(Jsoncpp_LIBRARY
	NAMES jsoncpp
	PATHS ${Jsoncpp_PKGCONF_LIBRARY_DIRS})

set(Jsoncpp_PROCESS_INCLUDES ${Jsoncpp_INCLUDE_DIR})
set(Jsoncpp_PROCESS_LIBS ${Jsoncpp_LIBRARY})
libfind_process(Jsoncpp)

message("include dir: " ${Jsoncpp_INCLUDE_DIR})
message("final path: " ${Jsoncpp_PKGCONF_INCLUDE_DIRS})
