#
# Makefile for feature flags.
#

passthru_opts+=\
	NO_IPV6 NOHW NOMD5 NOPOSTPROCESSING\
	MOBJCONSISTANCY PACKETDROP ZDEBUG\
	NOUPNP NOEXECINFO\

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

$(eval $(call Use_pkg_config,LIBBACKTRACE))
libs+=-lbacktrace
opts+=-DLIBBACKTRACE

ifdef HAVE_DISCORDRPC
  ifndef HAVE_DISCORDGAMESDK
    DISCORD_RPC_PKGCONFIG?=discord-rpc
    $(eval $(call Use_pkg_config,DISCORD_RPC))
    opts+=-DHAVE_DISCORDRPC -DHAVE_DISCORDSUPPORT
    libs+=-ldiscord-rpc
    DISCORD_SUPPORTED:=1
  else
    $(error \
      You can't have your cake and eat it too!\
      Choose either Discord RPC or Discord Game SDK!)
  endif
endif

ifdef HAVE_DISCORDGAMESDK
  ifndef HAVE_DISCORDRPC
    DISCORD_RPC_PKGCONFIG?=discord_game_sdk
    $(eval $(call Use_pkg_config,DISCORD_RPC))
    opts+=-DHAVE_DISCORDGAMESDK -DHAVE_DISCORDSUPPORT
    libs+=-ldiscord_game_sdk
    DISCORD_SUPPORTED:=1
    $(eval $(call Use_pkg_config,DISCORD_GAME_SDK))
  else
    $(error \
      You can't have your cake and eat it too!\
      Choose either Discord Game SDK or Discord RPC!)
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
