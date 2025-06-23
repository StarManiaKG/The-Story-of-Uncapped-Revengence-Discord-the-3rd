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

ifdef HAVE_DISCORDRPC
  lib:=../libs/discord-rpc/win$(32)-dynamic
  DISCORD_RPC_opts:=-I$(lib)/include
  DISCORD_RPC_libs:=-L$(lib)/lib
  $(eval $(call _set,DISCORD_RPC))
  $(eval $(call Propogate_flags,DISCORD_RPC))
endif

ifdef HAVE_DISCORDGAMESDK
  lib:=../libs/discord_game_sdk
  DISCORD_GAME_SDK_opts:=-I$(lib)/include
  DISCORD_GAME_SDK_libs:=-L$(lib)/$(x86)
  $(eval $(call _set,DISCORD_GAME_SDK))
  $(eval $(call Propogate_flags,DISCORD_GAME_SDK))
endif

ifdef HAVE_LIBAV
  lib:=../libs/libav/$(x86)-w64-mingw32
  LIBAV_opts:=-I$(lib)/include
  LIBAV_libs:=-L$(lib)/lib
  $(eval $(call _set,LIBAV))
  $(eval $(call Propogate_flags,LIBAV))
endif
