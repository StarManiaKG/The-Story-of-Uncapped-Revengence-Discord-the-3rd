#
# Makefile for feature flags.
#

passthru_opts+=\
	NONET NO_IPV6 NOHW NOMD5 NOPOSTPROCESSING\
	MOBJCONSISTANCY PACKETDROP ZDEBUG\
	HAVE_MINIUPNPC\

# build with debugging information
ifdef DEBUGMODE
PACKETDROP=1
opts+=-DPARANOIA -DRANGECHECK
endif

ifndef NOHW
opts+=-DHWRENDER
sources+=$(call List,hardware/Sourcefile)
endif

ifndef NOASM
ifndef NONX86
sources+=tmap.nas tmap_mmx.nas
opts+=-DUSEASM
endif
endif

ifndef NOMD5
sources+=md5.c
endif

ifndef NOZLIB
ifndef NOPNG
ifdef PNG_PKGCONFIG
$(eval $(call Use_pkg_config,PNG_PKGCONFIG))
else
PNG_CONFIG?=$(call Prefix,libpng-config)
$(eval $(call Configure,PNG,$(PNG_CONFIG) \
	$(if $(PNG_STATIC),--static),,--ldflags))
endif
ifdef LINUX
opts+=-D_LARGEFILE64_SOURCE
endif
opts+=-DHAVE_PNG
sources+=apng.c
endif
endif

ifndef NONET
ifndef NOCURL
CURLCONFIG?=curl-config
$(eval $(call Configure,CURL,$(CURLCONFIG)))
opts+=-DHAVE_CURL
endif
endif

ifdef HAVE_MINIUPNPC
libs+=-lminiupnpc
endif

## STAR STUFF ##
ifdef HAVE_DISCORDRPC
libs+=-ldiscord-rpc
CFLAGS+=-DHAVE_DISCORDRPC -DUSE_STUN
opts+=-DHAVE_DISCORDRPC -DUSE_STUN
sources+=discord.c stun.c
endif

ifdef HAVE_DISCORDGAMESDK
CFLAGS+=-DHAVE_DISCORDGAMESDK -DUSE_STUN
opts+=-DHAVE_DISCORDGAMESDK -DUSE_STUN
sources+=discord_gamesdk.c stun.c
endif

ifdef HAVE_LIBAV
libs+=-lm -lz -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale
CFLAGS+=-DHAVE_LIBAV
opts+=-DHAVE_LIBAV
sources+=$(call List,libav/Sourcefile)
endif
## END THIS PLEASE ##

# (Valgrind is a memory debugger.)
ifdef VALGRIND
VALGRIND_PKGCONFIG?=valgrind
$(eval $(call Use_pkg_config,VALGRIND))
ZDEBUG=1
opts+=-DHAVE_VALGRIND
endif

default_packages:=\
	GME/libgme/LIBGME\
	OPENMPT/libopenmpt/LIBOPENMPT\
	ZLIB/zlib\

$(foreach p,$(default_packages),\
	$(eval $(call Check_pkg_config,$(p))))
