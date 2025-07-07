
if (TARGET CURL::libcurl)
    target_sources(SRB2SDL2 PRIVATE smkg-curl.c smkg-curl-routines.c)
endif()
