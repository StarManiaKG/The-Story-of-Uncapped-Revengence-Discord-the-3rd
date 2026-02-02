Hi all, Star here!

I told you guys the update wouldn't take long! (It's been about a decade)
How have you been? Good? I hope so, but if not, I personally hope it'll all be better for you soon, and you have people that care about you and will listen to what you have to say.

I've cycled through like 50 million different computers in the span of making this but I finally have my own, so that's cool :p
Let's get into the changelog now that most certainly has something I'm forgetting but too lazy to figure out what it is...

Also sorry, it *was* supposed to come out yesterday, but I spent it fixing compiler warnings and other little stuff.
The code is a mess, just letting you know, but it does compile perfectly fine, I just didn't have the time to make it look nice and neat.

NOTE: Also, just as a heads-up, this'll be my last update for a bit. I'm leaving to go to basic training in the middle of February, so I won't be able to be reached at all.

=======================

Game/Build Changes
Updated the assets, so be sure to go get those.
'tsourdt3rd.pk3' is now smaller.
This is because I removed all the extra music from the PK3.
It can now be found in 'tsourdt3rd_music.pk3', a new optional file you can add that does some cool stuff.
Thanks to my new EXMusic system, which I'll explain in about like three seconds, the extra music lumps can now be loaded optionally.
TSoURDt3rd now takes up 20MB less of space without the extra music, just in case you wanted to know. (hooray for optimization and good (this is a lie) (not really) coding!)

Added EXMusic!
This replaces the old music system that allowed you to select between *pre-determined* music to play in levels.
Now, by adding a lump to your files, named 'EXMUSDEF', you can allow users to go set their own music to play in certain scenarios.
These scenarios include: bosses, intermissions, getting a gameover, or loading a music lump that doesn't exist.
This can be customized in TSoURDt3rd's audio options menu, under "EXMusic".

Updated coronas.
Fixed the issue where levels would crash on startup.
Fixed the issue where switching between renderers while coronas were rendering would crash the game.
Properly updated the corona light table, because *somebody* was too neglectful to do so themselves.
This means that some objects that didn't have coronas, or objects that had improper or incorrect coronas, should look as intended now.
I'm not gonna name who was behind this though. (It was STJr lol)
Fixed issues with some Chaos Emerald sprites not having the proper lights.
Improved plane lighting a little bit.
NOTE: Wall-lighting also exists, but is hidden behind a cvar in TSoURDt3rd's lighting menu (under video options) due to being in beta.

Updated the menus.
You can now navigate menus using a controller's D-Pad.
Improved controller and keybind support in menus overall.
Improved the sizes and styles of the menu text.
Fixed some issues with menu sizes.

Added the song credits feature from Ring Racers!
This required a little reworking of the musicdef system, so now most of those features from there are present in this build.
I also went ahead and added my own little touches and stylizations to it too, so I'm not directly copying it.
All present characters can run on the credits bar, the bar can change colors, etc.

Improved save file handling.
The system was kinda held together by wacky code, so I went and improved the code.
As a result, you can now also specify a custom save directory.

Removed most obligatory references to my name, such as on the window title.
I assume that if you're using this build, you already know who made it, so why plaster my name everywhere, you know?

Improved folder and file handling.
Added a couple of random crash messages.
Touched up the error handler message.
The build's name and version gets printed into the console whenever the game starts up.
Fixed some minor issues with window shaking.
Netgames do be netgaming now.
What does this mean? I have no clue.

Discord Changes
Improved activity status handling.
Fixed a bunch of strings, improved/revamped code, random stuff and whatnot.

BETA: Worked on adding Discord-Game-SDK support.
It's not present *within* the build, but the DLL is present, just in case you wanna go and compile it for yourself.
It's a bit iffy on startup though so don't expect too much functionality as of now. Only the activity status should work at the moment.

Improved the discord menu.
Added S3Sonic (by Blacknide) as a supported character. (Blacknide asked me to do this like 50 months ago I just got around to doing it like yesterday)

STARParser Changes
Added an EXMusic parser.
Text lengths are now properly enforced for the various parameters.
Better error messages are now given.
Fixed some crashes with parsing JUKEDEF lumps.

Jukebox Changes
Finally restored functionality and style back to the Sound Test's (and the Jukebox's by extension) pad graphic.
It's only been broken about as long as the jukebox was added :p
If the music lump has multiple music tracks, you can now select a specific music track you want to play.
Improved jukebox handling, both inside and outside the jukebox menu.
Improved the little box fade-outs in the menu.
Fixed some issues with the Jukebox.
Fixed some strings in the Jukebox.


And that's about everything!
I may be forgetting a couple things but that's the main gist.

Thanks again for being so patient with me guys. Hopefully, if all goes according to plan, the next update won't take as long.
Until next time!
