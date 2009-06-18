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
#include <vector>
#include "file.h"
#include "gfxrip.h"
#include "video.h"

typedef Uint32 Color;
typedef Color Palette[4];

const Color black = 0;
const Color fake_black = 0x010000;  // Since true black = color key
const Color red = 0xff0000;
const Color darkblue = 0xb4;
const Color blue = 0xff;
const Color babyblue = 0xa4a5ff;
const Color peach = 0xffc262;
const Color orange = 0xff7900;
const Color lt_orange = 0xffc200;
const Color brown = 0xc50000;
const Color tan = 0xe6a562;
const Color purple = 0xff55b4;
const Color cyan = 0xffff;
const Color white = 0xffffff;

#define pal_grey {black, 0x888888, 0xcccccc, white}
#define pal_unknown pal_grey
#define pal_girder {black, 0xff2c62, 0xa40000, cyan}
#define pal_mario {black, peach, red, blue}
#define pal_paultop {black, white, orange, purple}
#define pal_pauline {black, darkblue, white, purple}
#define pal_barrel {black, blue, orange, peach}
#define pal_dktop {black, brown, peach, white}
#define pal_dkbody {black, brown, peach, orange}
#define pal_oilbarl {black, white, cyan, blue}
#define pal_fire {black, lt_orange, red, white}
#define pal_spring {black, babyblue, cyan, red}
#define pal_elevbox {black, lt_orange, red, fake_black}
#define pal_dktile {black, orange, peach, white}
#define pal_rivet {black, blue, cyan, lt_orange}
#define pal_pie {black, tan, lt_orange, blue}

const Palette tile_palettes[] = {
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_unknown, pal_unknown, pal_unknown,
    pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,
    pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,
    pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,  pal_dktile,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_rivet,   pal_dktile,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_rivet,
    pal_rivet,   pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,  pal_girder,
    pal_girder,  pal_girder,  pal_girder,  pal_grey,    pal_girder,  pal_girder,  pal_grey,    pal_mario
};

const Palette spr_palettes[] = {
    pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,
    pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,   pal_mario,
    pal_paultop, pal_pauline, pal_pauline, pal_pauline, pal_pauline, pal_barrel,  pal_barrel,  pal_barrel,
    pal_barrel,  pal_oilbarl, pal_oilbarl, pal_oilbarl, pal_unknown, pal_unknown, pal_dktop,   pal_dktop,
    pal_dktop,   pal_dktop,   pal_dktop,   pal_dktop,   pal_dktop,   pal_dkbody,  pal_dkbody,  pal_dkbody,
    pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,
    pal_dktop,   pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,  pal_dkbody,
    pal_unknown, pal_elevbox, pal_elevbox, pal_spring,  pal_spring,  pal_fire,    pal_fire,    pal_unknown,
    pal_fire,    pal_fire,    pal_fire,    pal_fire,    pal_girder,  pal_elevbox, pal_grey,    pal_unknown,
    pal_unknown, pal_oilbarl, pal_unknown, pal_pie,     pal_pie,     pal_fire,    pal_fire,    pal_unknown,
    pal_spring,  pal_spring,  pal_spring,  pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown,
    pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown,
    pal_oilbarl, pal_oilbarl, pal_oilbarl, pal_oilbarl, pal_unknown, pal_unknown, pal_unknown, pal_unknown,
    pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown, pal_unknown,
    pal_unknown, pal_unknown, pal_oilbarl, pal_pauline, pal_pauline, pal_pauline, pal_pauline, pal_pauline,
    pal_mario,   pal_mario,   pal_mario,   pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey
};


void ripGfxROMs(const char *romset, SDL_Surface **tiles, SDL_Surface **sprites)
{
    std::vector<unsigned char> raw_tiles1(0x800);
    std::vector<unsigned char> raw_tiles2(0x800);
    std::vector<unsigned char> raw_spr_top1(0x800);
    std::vector<unsigned char> raw_spr_top2(0x800);
    std::vector<unsigned char> raw_spr_bot1(0x800);
    std::vector<unsigned char> raw_spr_bot2(0x800);

    if(strcmp(romset, "dkong") == 0)
    {
        readFile("roms/dkong/v_5h_b.bin", &raw_tiles1[0],   0x800);
        readFile("roms/dkong/v_3pt.bin",  &raw_tiles2[0],   0x800);
        readFile("roms/dkong/l_4m_b.bin", &raw_spr_top1[0], 0x800);
        readFile("roms/dkong/l_4r_b.bin", &raw_spr_top2[0], 0x800);
        readFile("roms/dkong/l_4n_b.bin", &raw_spr_bot1[0], 0x800);
        readFile("roms/dkong/l_4s_b.bin", &raw_spr_bot2[0], 0x800);
    }
    else
    {
        // @FIXME@ - just assume dkongjp for now
        readFile("roms/dkong/v_5h_b.bin",   &raw_tiles1[0],   0x800);
        readFile("roms/dkongjp/v_5k_b.bin", &raw_tiles2[0],   0x800);
        readFile("roms/dkong/l_4m_b.bin",   &raw_spr_top1[0], 0x800);
        readFile("roms/dkong/l_4r_b.bin",   &raw_spr_top2[0], 0x800);
        readFile("roms/dkong/l_4n_b.bin",   &raw_spr_bot1[0], 0x800);
        readFile("roms/dkong/l_4s_b.bin",   &raw_spr_bot2[0], 0x800);
    }

    Uint32 *surf_pixels;

    *tiles = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        TILE_SIZE*256, TILE_SIZE, 32,
        0, 0, 0, 0
    );

    // Note: No locking necessary since this is a software surface
    surf_pixels = static_cast<Uint32 *>((*tiles)->pixels);

    int src_pixel_num = 0;
    for(int tile_id = 0; tile_id < 256; ++tile_id)
    {
        const Palette &palette = tile_palettes[tile_id];

        for(int y = 0; y < 8; ++y)
        {
            for(int x = 0; x < 8; ++x)
            {
                int src_index = src_pixel_num/8;
                int shift_count = 7-x;
                int src_color = ((raw_tiles2[src_index] & (1 << shift_count)) << 1) |
                                 (raw_tiles1[src_index] & (1 << shift_count));
                src_color >>= shift_count;
                Color dst_color = palette[src_color];

                // These loops are for nearest-neighbor scaling
                for(int j = 0; j < GFX_SCALE; ++j)
                {
                    for(int i = 0; i < GFX_SCALE; ++i)
                    {
                        // Note that we're rotating 90 degrees here
                        int dst_x = (tile_id*TILE_SIZE)+(7-y)*GFX_SCALE+i;
                        int dst_y = x*GFX_SCALE+j;

                        int dst_index = dst_y*(TILE_SIZE*256)+dst_x;
                        surf_pixels[dst_index] = dst_color;
                    }
                }

                ++src_pixel_num;
            }
        }
    }

    *sprites = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        SPRITE_SIZE*128, SPRITE_SIZE, 32,
        0, 0, 0, 0
    );

    surf_pixels = static_cast<Uint32 *>((*sprites)->pixels);

    src_pixel_num = 0;
    for(int true_tile_id = 0; true_tile_id < 256; ++true_tile_id)
    {
        int sprite_id = true_tile_id/2;
        const Palette &palette = spr_palettes[sprite_id];

        // Hack to reverse horizontal order of tiles within sprites
        int tile_id;
        if(true_tile_id % 2 == 0)
        {
            tile_id = true_tile_id + 1;
        }
        else
        {
            tile_id = true_tile_id - 1;
        }

        for(int y = 0; y < 8; ++y)
        {
            for(int x = 0; x < 8; ++x)
            {
                int src_index = src_pixel_num/8;
                int shift_count = 7-x;
                int src_top_color = ((raw_spr_top2[src_index] & (1 << shift_count)) << 1) |
                                     (raw_spr_top1[src_index] & (1 << shift_count));
                int src_bot_color = ((raw_spr_bot2[src_index] & (1 << shift_count)) << 1) |
                                     (raw_spr_bot1[src_index] & (1 << shift_count));
                src_top_color >>= shift_count;
                src_bot_color >>= shift_count;
                Color dst_top_color = palette[src_top_color];
                Color dst_bot_color = palette[src_bot_color];

                // These loops are for nearest-neighbor scaling
                for(int j = 0; j < GFX_SCALE; ++j)
                {
                    for(int i = 0; i < GFX_SCALE; ++i)
                    {
                        // Note that we're rotating 90 degrees here
                        int dst_x = (tile_id*TILE_SIZE)+((7-y)*GFX_SCALE)+i;
                        int dst_y = x*GFX_SCALE+j;

                        int dst_index = dst_y*(TILE_SIZE*256)+dst_x;
                        surf_pixels[dst_index] = dst_top_color;
                        surf_pixels[dst_index+(SPRITE_SIZE*128*8*GFX_SCALE)] = dst_bot_color;
                    }
                }

                ++src_pixel_num;
            }
        }
    }
}
