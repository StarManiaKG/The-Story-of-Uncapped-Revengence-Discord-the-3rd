##
## Unique feature flags for TSoURDt3rd
##

## MAIN ##
opts+=-DUSE_STUN
sources+=$(call List,STAR/Sourcefile)
sources+=$(call List,STAR/monocypher/Sourcefile)

## LIBS ##
# Discord #
ifdef HAVE_DISCORDRPC
  ifndef HAVE_DISCORDGAMESDK
    DISCORD_SUPPORTED:=1
    DISCORD_RPC:=1
    opts+=-DHAVE_DISCORDRPC
    sources+=$(call List,discord/rpc/Sourcefile)
  else
    $(error \
      You can't have your cake and eat it too!\
      Choose either Discord RPC or Discord Game SDK!)
  endif
endif

ifdef HAVE_DISCORDGAMESDK
  ifndef HAVE_DISCORDRPC
    DISCORD_SUPPORTED:=1
    DISCORD_GAMESDK:=1
    opts+=-DHAVE_DISCORDGAMESDK
    sources+=$(call List,discord/gamesdk/Sourcefile)
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
  opts+=-DHAVE_LIBAV
  sources+=$(call List,STAR/libav/Sourcefile)
endif

ifdef MINGW
  include TSoURDt3rd/Makefile.d/win32.mk
endif
