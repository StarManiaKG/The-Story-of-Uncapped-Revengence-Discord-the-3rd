##
## Unique feature flags for TSoURDt3rd
##

# Variables
HAVE_DISCORDSUPPORT=0

# Definitions
ifdef HAVE_DISCORDRPC
HAVE_DISCORDSUPPORT=1
libs+=-ldiscord-rpc
opts+=-DHAVE_DISCORDRPC -DUSE_STUN
endif

ifdef HAVE_DISCORDGAMESDK
HAVE_DISCORDSUPPORT=1
opts+=-DHAVE_DISCORDGAMESDK -DUSE_STUN
endif

ifdef HAVE_DISCORDSUPPORT
sources+=$(call List,discord/Sourcefile)
endif

ifdef HAVE_LIBAV
libs+=-lm -lz -lavcodec -lavdevice -lavfilter -lavformat -lavresample -lavutil -lswscale
opts+=-DHAVE_LIBAV
sources+=$(call List,STAR/libav/Sourcefile)
endif

# Finale
sources+=$(call List,STAR/Sourcefile)
