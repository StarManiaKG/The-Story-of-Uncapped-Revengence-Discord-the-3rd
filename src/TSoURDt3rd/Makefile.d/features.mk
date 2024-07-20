##
## Unique feature flags for TSoURDt3rd
##

ifdef HAVE_DISCORDRPC
ifndef HAVE_DISCORDGAMESDK
libs+=-ldiscord-rpc
opts+=-DHAVE_DISCORDRPC -DUSE_STUN -DHAVE_DISCORDSUPPORT
sources+=$(call List,discord/Sourcefile)
else
$(error \
	You can't have your cake and eat it too!\
	Choose either Discord RPC or Discord Game SDK!)
endif
endif

ifdef HAVE_DISCORDGAMESDK
ifndef HAVE_DISCORDRPC # you can't have your cake and eat it too!
opts+=-DHAVE_DISCORDGAMESDK -DUSE_STUN -DHAVE_DISCORDSUPPORT
sources+=$(call List,discord/Sourcefile)
else
$(error \
	You can't have your cake and eat it too!\
	Choose either Discord Game SDK or Discord RPC!)
endif
endif

ifdef HAVE_LIBAV
libs+=-lm -lz -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale
opts+=-DHAVE_LIBAV
sources+=$(call List,STAR/libav/Sourcefile)
endif

sources+=$(call List,STAR/Sourcefile)
