#
# Unique feature flags for TSoURDt3rd
#

ifdef HAVE_DISCORDRPC
libs+=-ldiscord-rpc
opts+=-DHAVE_DISCORDRPC -DUSE_STUN
sources+=discord/discord.c discord/stun.c
endif

ifdef HAVE_DISCORDGAMESDK
opts+=-DHAVE_DISCORDGAMESDK -DUSE_STUN
sources+=discord/discord_gamesdk.c discord/stun.c
endif

ifdef HAVE_LIBAV
libs+=-lm -lz -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale
opts+=-DHAVE_LIBAV
sources+=$(call List,libav/Sourcefile)
endif
