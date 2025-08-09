#
# Mingw, if you don't know, that's Win32/Win64
#

# disable dynamicbase if under msys2
ifdef MSYSTEM
libs+=-Wl,--disable-dynamicbase
endif

sources+=win32/Srb2win.rc
sources+=win32/win_dbg.c
opts+=-DSTDC_HEADERS
libs+=-ladvapi32 -lkernel32 -lmsvcrt -luser32

ifndef DEDICATED
ifndef DUMMY
SDL?=1
endif
endif

ifndef NOHW
opts+=-DUSE_WGL_SWAP
endif

ifdef MINGW64
libs+=-lws2_32
else
ifdef NO_IPV6
libs+=-lwsock32
else
libs+=-lws2_32
endif
endif

ifndef MINGW64
32=32
x86=x86
i686=i686
else
32=64
x86=x86_64
i686=x86_64
endif

EXENAME?=srb2tsourdt3rd_win$(32).exe
mingw:=$(i686)-w64-mingw32

define _set =
$(1)_CFLAGS?=$($(1)_opts)
$(1)_LDFLAGS?=$($(1)_libs)
endef

lib:=../libs/gme
LIBGME_opts:=-I$(lib)/include
LIBGME_libs:=-L$(lib)/win$(32) -lgme
$(eval $(call _set,LIBGME))

lib:=../libs/libopenmpt
LIBOPENMPT_opts:=-I$(lib)/inc
LIBOPENMPT_libs:=-L$(lib)/lib/$(x86)/mingw -lopenmpt
$(eval $(call _set,LIBOPENMPT))

ifndef NOMIXERX
HAVE_MIXERX=1
lib:=../libs/SDLMixerX/$(mingw)
else
lib:=../libs/SDL2_mixer/$(mingw)
endif

ifdef SDL
mixer_opts:=-I$(lib)/include/SDL2
mixer_libs:=-L$(lib)/lib

lib:=../libs/SDL2/$(mingw)
SDL_opts:=-I$(lib)/include/SDL2\
	$(mixer_opts) -Dmain=SDL_main
SDL_libs:=-L$(lib)/lib $(mixer_libs)\
	-lmingw32 -lSDL2main -lSDL2 -mwindows
$(eval $(call _set,SDL))
endif

lib:=../libs/zlib
ZLIB_opts:=-I$(lib)
ZLIB_libs:=-L$(lib)/win32 -lz$(32)
$(eval $(call _set,ZLIB))

ifndef PNG_CONFIG
lib:=../libs/libpng-src
PNG_opts:=-I$(lib)
PNG_libs:=-L$(lib)/projects -lpng$(32)
$(eval $(call _set,PNG))
endif

lib:=../libs/curl
CURL_opts:=-I$(lib)/include
CURL_libs:=-L$(lib)/lib$(32) -lcurl
$(eval $(call _set,CURL))

lib:=../libs/miniupnpc
MINIUPNPC_opts:=-I$(lib)/include -DMINIUPNP_STATICLIB
MINIUPNPC_libs:=-L$(lib)/mingw$(32) -lminiupnpc -lws2_32 -liphlpapi
$(eval $(call _set,MINIUPNPC))

##
## Mingw, if you still don't know, that's Win32/Win64
## Except now it also includes cool TSoURDt3rd stuff too :)
##
## Copyright 2024-2025 by Star "Guy Who Names Scripts After Him" ManiaKG.
##
## Library configuration flags:
## Everything here is an override.
##
## DISCORD_RPC_CFLAGS=, DISCORD_RPC_LDFLAGS=
## DISCORD_GAME_SDK_CFLAGS=, DISCORD_GAME_SDK_LDFLAGS=
## LIBAV_CFLAGS=, LIBAV_LDFLAGS=

#lib:=../libs/BugTrap
#BUGTRAP_opts+=-I$(lib) -DBUGTRAP
#ifndef MINGW64
#BUGTRAP_libs+=-L$(lib) -lBugTrap
#else
#BUGTRAP_libs+=-L$(lib) -lBugTrap-x64
#endif
#$(eval $(call _set,BUGTRAP))
#$(eval $(call Propogate_flags,BUGTRAP))

LIBBACKTRACE_opts+=-I../libs/libbacktrace/include
LIBBACKTRACE_libs+=-L../libs/libbacktrace/lib/$(i686)
$(eval $(call _set,LIBBACKTRACE))

ifdef HAVE_DISCORDRPC
  lib:=../libs/discord-rpc/win$(32)-dynamic
  DISCORD_RPC_opts:=-I$(lib)/include
  DISCORD_RPC_libs:=-L$(lib)/lib
  $(eval $(call _set,DISCORD_RPC))
endif

ifdef HAVE_DISCORDGAMESDK
  lib:=../libs/discord_game_sdk
  DISCORD_GAME_SDK_opts:=-I$(lib)/include
  DISCORD_GAME_SDK_libs:=-L$(lib)/$(x86)
  $(eval $(call _set,DISCORD_GAME_SDK))
endif

ifdef HAVE_LIBAV
  lib:=../libs/libav/$(x86)-w64-mingw32
  LIBAV_opts:=-I$(lib)/include
  LIBAV_libs:=-L$(lib)/lib
  $(eval $(call _set,LIBAV))
endif
