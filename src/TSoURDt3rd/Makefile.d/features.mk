##
## Unique feature flags for TSoURDt3rd
##

ifdef HAVE_DISCORDRPC
libs+=-ldiscord-rpc
opts+=-DHAVE_DISCORDRPC -DUSE_STUN -DHAVE_DISCORDSUPPORT
endif

ifdef HAVE_DISCORDGAMESDK
opts+=-DHAVE_DISCORDGAMESDK -DUSE_STUN -DHAVE_DISCORDSUPPORT
endif

ifdef HAVE_DISCORDSUPPORT
sources+=$(call List,discord/Sourcefile)
endif

ifdef HAVE_LIBAV
libs+=-lm -lz -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale
opts+=-DHAVE_LIBAV
sources+=$(call List,STAR/libav/Sourcefile)
endif

sources+=$(call List,STAR/Sourcefile)
