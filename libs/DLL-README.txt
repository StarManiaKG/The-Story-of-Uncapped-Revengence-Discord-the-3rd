# SRB2 - Which DLLs do I need to bundle?

Updated 08/12/2025 (v1.5)

Here are the required DLLs, per build. For each architecture, copy all the binaries from these folders:

* libs\dll-binaries\[i686/x86_64]
* libs\SDL2\[i686/x86_64]...\bin
* libs\SDL2mixerX\[i686/x86_64]...\bin
* libs\libopenmpt\[x86/x86_64]...\bin\mingw

## srb2win, 32-bit

* libs\dll-binaries\i686\exchndl.dll
* libs\dll-binaries\i686\libgme.dll
* libs\dll-binaries\i686\mgwhelp.dll (depend for exchndl.dll)
* libs\SDL2\i686-w64-mingw32\bin\SDL2.dll
* libs\SDL2mixerX\i686-w64-mingw32\bin\*.dll (get everything)
* libs\libopenmpt\x86\bin\mingw\libopenmpt.dll
    # tsourdt3rd
* libs\dll-binaries\i686\discord-rpc.dll
* libs\discord-rpc/x86/discord_game_sdk.dll

## srb2win, 64-bit

* libs\dll-binaries\x86_64\exchndl.dll
* libs\dll-binaries\x86_64\libgme.dll
* libs\dll-binaries\x86_64\mgwhelp.dll (depend for exchndl.dll)
* libs\SDL2\x86_64-w64-mingw32\bin\SDL2.dll
* libs\SDL2mixerX\x86_64-w64-mingw32\bin\*.dll (get everything)
* libs\libopenmpt\x86_64\bin\mingw\libopenmpt.dll
* libs\SDL2_mixer\x86_64-w64-mingw32\bin\*.dll (get everything)
    # tsourdt3rd
* libs\dll-binaries\x86_64\discord-rpc.dll
* libs\discord-rpc/x86_64/discord_game_sdk.dll
