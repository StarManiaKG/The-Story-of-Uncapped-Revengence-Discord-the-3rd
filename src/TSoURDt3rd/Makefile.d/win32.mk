#
# Mingw, if you still don't know, that's Win32/Win64
# Except now it also includes cool TSoURDt3rd stuff too :)
#

ifdef HAVE_DISCORDRPC
ifdef MINGW64
opts+=-I../libs/discord-rpc/win64-dynamic/include
libs+=-L../libs/discord-rpc/win64-dynamic/lib
else
opts+=-I../libs/discord-rpc/win32-dynamic/include
libs+=-L../libs/discord-rpc/win32-dynamic/lib
endif
libs+=-ldiscord-rpc
endif

ifdef HAVE_DISCORDGAMESDK
opts+=-I../libs/discord-game-sdk/include
ifdef MINGW64
libs+=-L../libs/discord-game-sdk/x86_64
else
libs+=-L../libs/discord-game-sdk/x86
endif
endif

ifdef HAVE_LIBAV
ifdef MINGW64
opts+=-I../libs/libav/x86_64-w64-mingw32/include
libs+=-L../libs/libav/x86_64-w64-mingw32/lib
else
opts+=-I../libs/libav/i686-w64-mingw32/include
libs+=-L../libs/libav/i686-w64-mingw32/lib
endif
libs+=-lvfw32 -lws2_32 -lbcrypt -luser32
endif
