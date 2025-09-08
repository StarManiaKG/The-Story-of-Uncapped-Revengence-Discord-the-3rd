# SRB2TSoURDt3rd - Which DLLs do I need to bundle?

Updated 09/05/2025 (v1.6)

Here are the required DLLs, per build. For each architecture, copy all the binaries from these folders:

* libs\dll-binaries\[i686/x86_64]
* libs\SDL2\[i686/x86_64]...\bin
* libs\SDL2mixerX\[i686/x86_64]...\bin
* libs\discord-rpc\[xi686/x86_64]\bin
* libs\discord_game_sdk\[x86/x86_64]

## srb2tsourdt3rd, 32-bit

* libs\dll-binaries\i686\exchndl.dll
* libs\dll-binaries\i686\libgme.dll
* libs\dll-binaries\i686\mgwhelp.dll (depend for exchndl.dll)
* libs\SDL2\i686-w64-mingw32\bin\SDL2.dll
* libs\SDL2mixerX\i686-w64-mingw32\bin\*.dll (get everything)
* libs\libopenmpt\x86\bin\mingw\libopenmpt.dll
* libs\discord-rpc\i686\bin\discord-rpc.dll
* libs\discord_game_sdk\x86\discord_game_sdk.dll

## srb2tsourdt3rd, 64-bit

* libs\dll-binaries\x86_64\exchndl.dll
* libs\dll-binaries\x86_64\libgme.dll
* libs\dll-binaries\x86_64\mgwhelp.dll (depend for exchndl.dll)
* libs\SDL2\x86_64-w64-mingw32\bin\SDL2.dll
* libs\SDL2mixerX\x86_64-w64-mingw32\bin\*.dll (get everything)
* libs\libopenmpt\x86_64\bin\mingw\libopenmpt.dll
* libs\SDL2_mixer\x86_64-w64-mingw32\bin\*.dll (get everything)
* libs\discord-rpc\x86_64\bin\discord-rpc.dll
* libs\discord_game_sdk\x86_64\discord_game_sdk.dll
