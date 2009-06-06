Running the emulator
--------------------
It's currently a little difficult to run the emulator, since the
graphics files need to be generated beforehand. But the general
procedure is this:

Unzip dkong.zip (the MAME ROMs) to the roms\dkong folder. I.e.,
if Kong DX is installed in C:\KongDX, then unzip your ROMs to the
C:\KongDX\roms\dkong folder.

After you have extracted the ROMs, run the Python script gfxrip.py.
(The script is written for Python 2.5.) It should create two files
named tiles.bmp and sprites.bmp. You will still need your ROMs, so
don't delete them afterward.

The emulator is now ready to run. :)



Building the source
-------------------
We use CMake: http://www.cmake.org/

You will need SDL to build.


Misc. notes
-----------
A ton of credit goes to MAME. Although I didn't take any code from it,
most of my knowledge of the Donkey Kong hardware comes directly from
the MAME source.
