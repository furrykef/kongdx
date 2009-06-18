/*
    Kong DX - an enhanced Donkey Kong emulator
    Copyright 2009 Kef Schecter

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include <fstream>
#include "file.h"

// @XXX@ -- no error checking
void readFile(const char *filename, unsigned char *buf, std::size_t size)
{
    std::ifstream rom_file(filename, std::ifstream::in | std::ifstream::binary);
    rom_file.read(reinterpret_cast<char *>(buf), size);
}
