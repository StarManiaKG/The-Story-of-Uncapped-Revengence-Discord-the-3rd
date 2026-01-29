#
# Makefile for feature flags.
#

passthru_opts+=\
  NO_IPV6 NOHW NOMD5 NOPOSTPROCESSING\
  MOBJCONSISTANCY PACKETDROP ZDEBUG\
  NOUPNP NOEXECINFO\
  HAVE_DISCORDRPC\

# build with debugging information
ifdef DEBUGMODE
PACKETDROP=1
opts+=-DPARANOIA -DRANGECHECK
endif

ifndef NOHW
opts+=-DHWRENDER
sources+=$(call List,hardware/Sourcefile)
endif

ifndef NOMD5
sources+=md5.c
endif

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

ifndef NOCURL
CURLCONFIG?=curl-config
$(eval $(call Configure,CURL,$(CURLCONFIG)))
opts+=-DHAVE_CURL
endif

ifndef NOUPNP
MINIUPNPC_PKGCONFIG?=miniupnpc
$(eval $(call Use_pkg_config,MINIUPNPC))
HAVE_MINIUPNPC=1
opts+=-DHAVE_MINIUPNPC
endif

# (Valgrind is a memory debugger.)
ifdef VALGRIND
VALGRIND_PKGCONFIG?=valgrind
VALGRIND_LDFLAGS=
$(eval $(call Use_pkg_config,VALGRIND))
ZDEBUG=1
opts+=-DHAVE_VALGRIND
endif

# (drmingw was our old crash debugger on windows)
ifndef NO_DRMINGW
$(eval $(call Use_pkg_config,DRMINGW))
opts+=-DHAVE_DRMINGW
endif

# (libbacktrace is our new multiplatform crash debugger)
ifndef NOLIBBACKTRACE
$(eval $(call Propogate_flags,LIBBACKTRACE))
libs+=-lbacktrace
opts+=-DHAVE_LIBBACKTRACE
endif

# (BugTrap is another crash debugger for windows)
# Linked in-game, so we don't do anything else here
ifdef HAVE_BUGTRAP
opts+=-DHAVE_BUGTRAP
endif

default_packages:=\
	GME/libgme/LIBGME\
	OPENMPT/libopenmpt/LIBOPENMPT\
	ZLIB/zlib\

$(foreach p,$(default_packages),\
	$(eval $(call Check_pkg_config,$(p))))


##
## Unique feature flags for TSoURDt3rd
## Copyright 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
##

opts+=-DUSE_STUN

ifndef NOCURL
  sources+=$(call List,STAR/curl/Sourcefile)
endif
ifdef HAVE_LIBAV
  sources+=$(call List,STAR/libav/Sourcefile)
endif

ifdef HAVE_DISCORDRPC
  DISCORD_RPC_PKGCONFIG?=discord-rpc
  $(eval $(call Propogate_flags,DISCORD_RPC))
  libs+=-ldiscord-rpc
  opts+=-DHAVE_DISCORDRPC -DHAVE_DISCORDSUPPORT
  DISCORD_SUPPORTED:=1
endif

ifdef HAVE_DISCORDGAMESDK
  DISCORD_RPC_PKGCONFIG?=discord_game_sdk
  $(eval $(call Propogate_flags,DISCORD_GAME_SDK))
  opts+=-DHAVE_DISCORDGAMESDK -DHAVE_DISCORDSUPPORT
  libs+=-ldiscord_game_sdk
  DISCORD_SUPPORTED:=1
endif

sources+=discord/discord_net.c
ifdef DISCORD_SUPPORTED
  sources+=$(call List,discord/Sourcefile)
  ifdef HAVE_DISCORDRPC
    sources+=discord/rpc/discord_rpc.c
  else ifdef HAVE_DISCORDGAMESDK
    sources+=discord/gamesdk/discord_gamesdk.c
  endif
endif

ifdef HAVE_LIBAV
  #libav_default_packages:=\
  #  LIBAVCODEC\
  #  LIBAVDEVICE\
  #  LIBAVFILTER\
  #  LIBAVFORMAT\
  #  LIBAVRESAMPLE\
  #  LIBAVUTIL\
  #  LIBSWSCALE\

  opts+=-DHAVE_LIBAV

  #libs+=-lvfw32 -lws2_32 -lbcrypt -luser32
  #libs+=-lm -lz -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale

  #libs+=-lvfw32 -lbcrypt
  #libs+=-lm -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale

  #$(foreach libav_p,$(libav_default_packages),\
  #  $(eval $(call Check_pkg_config,$(libav_p))))
  $(eval $(call Propogate_flags,LIBAV))
endif
