Running the emulator
--------------------
Unzip dkong.zip (the MAME ROMs) to the roms\dkong folder. I.e.,
if Kong DX is installed in C:\KongDX, then unzip your ROMs to the
C:\KongDX\roms\dkong folder.

The emulator is now ready to run. :)


Sound
-----
To get sound, place the following files in the folder "sounds/dkong".
Currently you need to provide these files yourself.

Music:
* intro.ogg
* howhigh.ogg
* barrels.ogg
* pies.ogg
* springs.ogg
* rivets.ogg
* hammer.ogg
* screencomplete.ogg
* kongfall.ogg
* rescue1.ogg
* rescue2.ogg
* lowtime.ogg
* death.ogg

Sound effects:
* boom.ogg
* jump.ogg
* spring.ogg
* hammerhit.ogg
* fall.ogg
* score.ogg


Japanese version
----------------
The Japanese version of the game is the same as the US version except
for the level order. In the US version, the levels go like this:

  Level 1: Barrels, rivets
  Level 2: Barrels, elevators, rivets
  Level 3: Barrels, pie factory, elevators, rivets
  Level 4: Barrels, pie factory, barrels, elevators, rivets
  Level 5: Barrels, pie factory, barrels, elevators, barrels, rivets

From there on, every level is a copy of level 5, until the kill screen
on level 22.

In the Japanese version, all levels follow the order of barrels,
pie factory, elevators, rivets. And yep, the kill screen is still
waiting for you on level 22.

To run the Japanese version of Donkey Kong, follow the above procedure
with dkong.zip (the American ROMs), then unzip dkongjp.zip to the
roms\dkongjp folder. In fact, the American ROMs are only needed for
ripping the graphics, so if you already have that done, you don't need
the American ROMs.

Then, from the command line, run Kong DX with this command:

  kongdx dkongjp


Building the source
-------------------
We use CMake: http://www.cmake.org/

You will need SDL and SDL_mixer to build.


Misc. notes
-----------
A ton of credit goes to MAME. Although I didn't take any code from it,
most of my knowledge of the Donkey Kong hardware comes directly from
the MAME source. It's very unlikely I'd have been able to code this
without such a reference.
