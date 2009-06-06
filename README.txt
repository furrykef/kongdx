Running the emulator
--------------------
The emulator is not easily runnable by the public at this time,
because doing so would involve distributing data ripped from the ROMs.
The ROMs themselves are also in a non-standard format.


Building the source
-------------------
We use CMake: http://www.cmake.org/

To build, you will need Marat Fayzullin's Z80 core. It has not been
included because it is not open-source software under the Open Source
Definition. Get it from http://fms.komkon.org/EMUL8/ -- it's labeled
Z80 Portable Emulation Package. Unzip it to the source directory in
a folder named "z80". It should work unmodified.


Misc. notes
-----------
A ton of credit goes to MAME. Although I didn't take any code from it,
most of my knowledge of the Donkey Kong hardware comes directly from
the MAME source.
