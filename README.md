# TSOURDt3RD: An Sonic Robo Blast 2 Original
**Latest Stable Release:** [![latest release](https://badgen.net/github/release/starmaniakg/the-story-of-uncapped-revengence-discord-the-3rd/stable)](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/releases/latest)

**Latest Unstable Release:** [![latest release](https://badgen.net/github/release/starmaniakg/the-story-of-uncapped-revengence-discord-the-3rd)](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/releases/latest)

[![Build status](https://ci.appveyor.com/api/projects/status/399d4hcw9yy7hg2y?svg=true)](https://ci.appveyor.com/project/STJr/srb2)
[![Build status](https://travis-ci.org/STJr/SRB2.svg?branch=master)](https://travis-ci.org/STJr/SRB2)
[![CircleCI](https://circleci.com/gh/STJr/SRB2/tree/master.svg?style=svg)](https://circleci.com/gh/STJr/SRB2/tree/master)

[Sonic Robo Blast 2](https://srb2.org/) is a 3D Sonic the Hedgehog fangame based on a modified version of [Doom Legacy](http://doomlegacy.sourceforge.net/), and [This Custom Build](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd) is based upon those.

## Branches
  - ```main``` - Holds all the current, stable TSOURDt3RD code
  - ```unstablemain``` - Holds all the extremely current, but unstable, TSOURDt3RD code
  - ```main-2.1.25``` - Holds all the current, stable, TSOURDt3RD 2.1.25 code
  - ```androidmain``` - Holds all the current, stable, TSOURDt3RD Android Support code

## Features
**NEW FEATURES**
  - Uncapped, Technically Now In Vanilla SRB2, Is Included, And Was Featured In This Build Early Enough For Me To Call It A "New Feature"
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/uncapped.gif?raw=true)

  - Custom Discord Settings, Commands, and Options, with Custom Character Support
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/discord1.png?raw=true)
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/discord2.png?raw=true)

  - Custom Build Options, Featuring More Savedata, Server, and Extra Options
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/tsourdt3rd.png?raw=true)

  - Palette Rendering (Software Palette in OpenGL)
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/paletterendering.gif?raw=true)

  - Autoloading Addons
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/autoloading1.gif?raw=true)
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/autoloading2.gif?raw=true)
    -   NOTE: You May Want to Backup Your Saves, Just in Case. You Never Know What Could Happen.

  - A Jukebox
    
    ![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/jukebox.gif?raw=true)
    
    
  - ...And More Annoyances!

**FEATURES THAT WORK WITH ENOUGH EFFORT**
  - Holepunching; Requires Vanilla-SRB2 Based Relay Server, Kart Krew's Most Likely Won't Work
  - ...And A Few More Annoyances!

**WIP FEATURES (MOST LIKELY CAN BE FOUND ON OTHER BRANCHES)**
  - Discord's Game SDK (it compiles fine, nothing's just there yet :P)
  - Android Support (currently at the bare minimum :P)
  - ...And Even More Annoyances, Most Likely!

**FUTURE FEATURES (WITH ENOUGH TIME IN THE OVEN)**
  - Netplus (really on a limb here)
  - Autoloading Folder Support (it's a funny story)
  - Dynamic Resolutions (an annoyance to port from srb2 2.2.6 to 2.2.11)
  - Software Models (an annoyance to port just like Dynamic Resolutions)
  - ...And A Boatload of More Annoyances!
      
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
All of the following assets can be found in the "assets" folder of this repository, under a folder with the name "TSOURDT3RD-Assets".

![](https://github.com/StarManiaKG/The-Story-Of-Uncapped-Revengence-Discord-the-3rd/blob/main/assets/images/assets.png?raw=true)

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
