# TSoURDt3RD: An Sonic Robo Blast 2 Original
![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/srb2.png?raw=true)

**Latest Release:** [![latest release](https://badgen.net/github/release/starmaniakg/the-story-of-uncapped-revengence-discord-the-3rd/stable)](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/releases/latest)

[![Vanilla SRB2's Build status](https://ci.appveyor.com/api/projects/status/399d4hcw9yy7hg2y?svg=true)](https://ci.appveyor.com/project/STJr/srb2)
[![Vanilla SRB2's Build status](https://travis-ci.org/STJr/SRB2.svg?branch=master)](https://travis-ci.org/STJr/SRB2)
[![Vanilla SRB2's CircleCI](https://circleci.com/gh/STJr/SRB2/tree/master.svg?style=svg)](https://circleci.com/gh/STJr/SRB2/tree/master)

[Sonic Robo Blast 2](https://srb2.org/) is a 3D Sonic the Hedgehog fangame based on a modified version of [Doom Legacy](http://doomlegacy.sourceforge.net/), and [This Custom Build](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd) is based upon those.

## Branches
  - ```main``` - Holds all the current, stable, TSoURDt3RD code
  - ```main-v2``` - Holds all the current, stable, TSoURDt3RD v2 (SRB2 2.2.10) code

  - ```unstablemain``` - Holds all the extremely current, but unstable, TSoURDt3RD code
  - ```main-2.1.25``` - Holds all the current, stable, TSoURDt3RD 2.1.25 code
  - ```androidmain``` - Holds all the current, stable, TSoURDt3RD Android code

## Features
**NEW FEATURES**
  - Uncapped, Now In Vanilla SRB2, Is Included, And Was Featured In This Build Early Enough For Me To Call It A "New Feature"
    - (By Eidolon, Old Original Source Code found [here](https://git.do.srb2.org/ShaderWraith/SRB2/-/tree/uncappedrevengeance))

    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/uncapped.gif?raw=true)

  - Custom Discord Settings, Commands, and Options, with Custom Character Support
    - (Original Source Code by the Kart Krew and Fafabis, can be found [here](https://git.do.srb2.org/Fafabis/SRB2/-/tree/discord))

    - Currently Supported Characters:
      - Sonic, Super Sonic, Sonic & Tails
      - Tails
      - Knuckles
      - Amy
      - Fang
      - Metal Sonic
      - Adventure Sonic
      - Shadow
      - Skip
      - Jana
      - Surge
      - Cacee
      - Milne
      - Maimy
      - Mario
      - Luigi
      - Blaze
      - Marine
      - Tails Doll
      - Metal Knuckles
      - Smiles
      - Whisper
      - Speccy
    
    - Currently Supported Charatcer Portraits:
      - CSS Portraits & Super CSS Portraits
      - Continue Sprite Portraits & Super Continue Sprite Portraits
      - Life Icon Portraits & Super Life Icon Portraits
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/discord.png?raw=true)

  - Custom Build Options, directed towards Saves, Servers, and Extra Features
    - (Holepunching Ported from SRB2 Kart by Me)
    - (Original Leavebug Fix Coded by flarn2006)
    - Here's a General Overview on the Other Extra Features this Build Offers:
        - Legacy Features, such as
          - Startup Screens
          - Commands
          - Graphics
          - Holiday Events
        - Game and Engine Modifying Features, such as
          - Realistic Shadows
          - Loading Screens
        - Audio Features, such as
          - Customizable Jingle Music
          - Default Map Music, for Maps With No Music
        - Savefile Features, such as
          - Perfect Saves
          - Continues
          - The Ability to Store Savefiles in Folders
        - Server Features, such as
          - A Removed SOCK_SEND Error
          - Holepunching
          - The Ability to Change Your Character While Moving
        - Miscellaneous Features, such as
          - Extra Events and Event Features
          - QoL Changes and Features
          - Customizable and Dynamic Window Titles
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/tsourdt3rd.png?raw=true)

  - Palette Rendering
    - (Software Palette in OpenGL)
    - (Made by Hannu Hanhi, source code found [here](https://git.do.srb2.org/Hannu_Hanhi/SRB2/-/tree/ogl-palette-rendering))
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/paletterendering.gif?raw=true)

  - Autoloading Addons
    - (Made by Me)

    - Using Right Shift over an add-on or folder in the add-ons menu allows you to select add-ons or folders that will autoload when SRB2 first starts up.

    - This feature also allows you to bypass modifiedgame checks when the game first starts up, allowing you to play all the add-ons you want, in whatever savefiles or modes you want.

    - This feature also creates new savefiles for whenever autoloading is active, meaning that your normal savefiles are safe and secure. Your gamedata is still used and intact, however.
    
    - To stop certain, or all, addons from autoloading on startup, look at the ```autoload.cfg``` file, located in your SRB2 directory, and remove the add-ons and folder names there.
    
    - Autoloading add-ons still does affect netgames and the ability to join them, though. If you've loaded game-changig add-ons, you still need to relaunch with the base IWADS loaded in other to join other netgames.
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/autoloading1.gif?raw=true)
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/autoloading2.gif?raw=true)

  - A Jukebox
    - (Made by Me)
    
    - Play any of your favorite songs, even during a game, all from the comfort of your own Jukebox.
    
    - (You need to have the Sound Test unlocked in order to use this feature.)
    
    - How To Add Custom Music to the Jukebox
      - First, you need to have the ```jukebox.pk3``` file and your custom music on hand. Make sure your music is in the proper format.
      - Then, you need to add your music to the ```jukebox.pk3``` file.
      - Lastly, open the ```MUSICDEF``` file within ```jukebox.pk3``` and add your custom music to that file. If you need to, use some of the pre-existing lumps in the ```MUSICDEF``` file as an example on how to add your music to the file.
        - Make Sure you Name the Files Correctly. Remember that The First Two Characters in the Name of a Music Lump, I.E O_ or D_, Are Exempted From the Lump Name in the ```MUSICDEF`` file, with the Other Characters Being Required in that Field.
      - Alternatively, you could just follow this SRB2 Wiki tutorial on how to create ```MUSICDEF``` files.

    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/jukebox.gif?raw=true)

**WIP FEATURES (MOST LIKELY CAN BE FOUND ON OTHER BRANCHES)**
  - Discord's Game SDK (it compiles fine, but nothing's there yet :P)
  - Android Support (currently at the extremely bare minimum :P)
  - ...And Even Some Annoyances!

**FUTURE FEATURES (WITH ENOUGH TIME IN THE OVEN)**
  - Netplus (really on a limb here)
  - Dynamic Resolutions (annoying to port from SRB2 v2.2.6 to v2.2.11)
  - Software Models (its going to be annoying to port, just like the above)
  - MP4 Support (this is all lj sonic's fault, because he inspired me to do this, and i'm definitely not doing this of my own volition)
  - ...And A Boatload of Even More Annoyances!
      
## Dependencies
**NECESSARY DEPENDENCIES**
  - NASM (x86 builds only)
  - SDL2 (Linux/OS X only)
  - SDL2-Mixer (Linux/OS X only)
  - libupnp (Linux/OS X only)
  - libgme (Linux/OS X only)
  - libopenmpt (Linux/OS X only)

**OPTIONAL DEPENDENCIES**
  - Discord-RPC (Discord RPC Support; Windows Included, Mac/Linux users can find/compile them [here](https://github.com/discord/discord-rpc))
  - Discord Game SDK (Discord Game SDK Support; Windows Included, Mac/Linux users can find them [here](https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip) and place them in the right directories)
  - SDL-Mixer-X (Advanced Sound/MIDI Support and Options; Windows Included, Mac/Linux users can find/compile them [here](https://github.com/WohlSoft/SDL-Mixer-X))

## Assets
All of the following assets can be found in the current zip file of TSoURDt3rd, found on the SRB2 Message Board [here](https://mb.srb2.org/addons/the-story-of-uncapped-revengence-discord-the-3rd.4932/download).

**REQUIRED ASSETS**
  - srb2.pk3
  - player.dta
  - zones.pk3
  - patch.dta (if required for the current version)
  - tsourdt3rd.pk3

**OPTIONAL ASSETS**
  - music.dta
  - jukebox.pk3

## Compiling
- See [SRB2 Wiki/Source code compiling](http://wiki.srb2.org/wiki/Source_code_compiling)
- Also see [SRB2 Wiki/Source code compiling/Makefiles](http://wiki.srb2.org/wiki/Source_code_compiling/Makefiles), and [SRB2 Wiki/Source code compiling/CMake](http://wiki.srb2.org/wiki/Source_code_compiling/CMake), depending on the type of compiler you're using

**ALTERNATIVES**
- You can check out the releases page! These include console scripts and .exe files, both for Linux and Windows users, respectively. AppImage support will also come soon, but Wine also exists, so if you're into that type of thing, .exe files will work for you too I guess.
- You can also Cross-Compile Between Different Operating Systems too!
- You could ask a friend to do it for you too, since friends are the best after all.

## Disclaimers
- Sonic Team Junior is in no way affiliated with SEGA or Sonic Team. We do not claim ownership of any of SEGA's intellectual property used in SRB2.
- Despite the title of this file making this seem like the title of a movie, I regret to inform you that this is not, sadly, a movie, but instead, an autobiography, on how I am a constant and passionate nusiance on my society.