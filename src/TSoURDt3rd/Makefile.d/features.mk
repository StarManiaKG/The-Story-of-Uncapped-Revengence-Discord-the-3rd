##
## Unique feature flags for TSoURDt3rd
## Copyright 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
##

## MAIN ##
opts+=-DUSE_STUN
sources+=$(call List,STAR/Sourcefile)
ifndef NOCURL
sources+=$(call List,STAR/curl/Sourcefile)
endif
sources+=$(call List,STAR/monocypher/Sourcefile)

## LIBS ##
ifdef MINGW
  include TSoURDt3rd/Makefile.d/win32.mk
endif

# Discord #
ifdef HAVE_DISCORDRPC
  ifndef HAVE_DISCORDGAMESDK
    opts+=-DHAVE_DISCORDRPC
    libs+=-ldiscord-rpc
    sources+=$(call List,discord/rpc/Sourcefile)
    DISCORD_SUPPORTED:=1
  else
    $(error \
      You can't have your cake and eat it too!\
      Choose either Discord RPC or Discord Game SDK!)
  endif
endif

ifdef HAVE_DISCORDGAMESDK
  ifndef HAVE_DISCORDRPC
    opts+=-DHAVE_DISCORDGAMESDK
    libs+=-ldiscord_game_sdk
    sources+=$(call List,discord/gamesdk/Sourcefile)
    DISCORD_SUPPORTED:=1
  else
    $(error \
      You can't have your cake and eat it too!\
      Choose either Discord Game SDK or Discord RPC!)
  endif
endif

ifdef DISCORD_SUPPORTED
  opts+=-DHAVE_DISCORDSUPPORT
  sources+=$(call List,discord/Sourcefile)
endif
sources+=discord/discord_net.c

# LibAV #
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

  sources+=$(call List,STAR/libav/Sourcefile)
endif
