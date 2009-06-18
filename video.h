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
#ifndef KONGDX_VIDEO_H
#define KONGDX_VIDEO_H

const int TILE_SIZE = 16;
const int SPRITE_SIZE = TILE_SIZE*2;
const int SCREEN_WIDTH = TILE_SIZE*28;
const int SCREEN_HEIGHT = TILE_SIZE*32;
const int SCREEN_BPP = 32;

const int GFX_SCALE = TILE_SIZE/8;

#endif
