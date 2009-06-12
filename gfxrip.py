#!/usr/bin/env python
from __future__ import division
from __future__ import with_statement
import sys

from PIL import Image


# Note: needs trailing slash
ROMDIR = 'roms/dkong/'


black = (0, 0, 0)
fake_black = (0x01, 0, 0)       # Since true black = color key
red = (0xff, 0, 0)
darkblue = (0, 0, 0xb4)
blue = (0, 0, 0xff)
babyblue = (0xa4, 0xa5, 0xff)
peach = (0xff, 0xc2, 0x62)
orange = (0xff, 0x79, 0)
lt_orange = (0xff, 0xc2, 0)
brown = (0xc5, 0, 0)
tan = (0xe6, 0xa5, 0x62)
purple = (0xff, 0x55, 0xb4)
cyan = (0x00, 0xff, 0xff)
white = (0xff, 0xff, 0xff)

pal_grey = [black,
            (0x88, 0x88, 0x88),
            (0xcc, 0xcc, 0xcc),
             white]

pal_unknown = pal_grey

pal_girder = [black,
              (0xFF, 0x2c, 0x62),
              (0xa4, 0, 0),
              cyan]

pal_mario = [black, peach, red, blue]
pal_paultop = [black, white, orange, purple]
pal_pauline = [black, darkblue, white, purple]
pal_barrel = [black, blue, orange, peach]
pal_dktop = [black, brown, peach, white]
pal_dkbody = [black, brown, peach, orange]
pal_oilbarl = [black, white, cyan, blue]
pal_fire = [black, lt_orange, red, white]
pal_spring = [black, babyblue, cyan, red]
pal_elevbox = [black, lt_orange, red, fake_black]
pal_dktile = [black, orange, peach, white]
pal_rivet = [black, blue, cyan, lt_orange]
pal_pie = [black, tan, lt_orange, blue]

tile_palettes = [
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
    pal_girder,  pal_girder,  pal_girder,  pal_grey,    pal_girder,  pal_girder,  pal_grey,    pal_mario,
]
tile_palettes[255] = pal_mario

tile_files = [
    "v_5h_b.bin",
    "v_3pt.bin"
]

spr_palettes = [
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
    pal_mario,   pal_mario,   pal_mario,   pal_grey,    pal_grey,    pal_grey,    pal_grey,    pal_grey,
]

top_spr_files = [
    "l_4m_b.bin",
    "l_4r_b.bin",
]

bottom_spr_files = [
    "l_4n_b.bin",
    "l_4s_b.bin"
]


def main(argv=None):
    if argv is None:
        argv = sys.argv[1:]

    ### TILES ###
    tiledata = decode(tile_files)

    # Convert paletted values to RGB values
    tiledata = [tile_palettes[pixel_num//64][color] for pixel_num, color in enumerate(tiledata)]

    tile_img = Image.new("RGB", (2048, 8))
    for offset in range(0, len(tiledata), 64):
        tile_num = offset//64
        for y in range(8):
            for x in range(8):
                # Note that we're rotating each tile 90 degrees right as we do this
                # Hence putting pixels at (7-y, x) instead of (x, y)
                tile_img.putpixel((tile_num*8+(7-y), x), tiledata[offset+y*8+x])

    tile_img.resize((4096, 16)).save("tiles.bmp")


    ### SPRITES ###
    top_spr_data = decode(top_spr_files)
    bottom_spr_data = decode(bottom_spr_files)

    # Convert paletted values to RGB values
    top_spr_data = [spr_palettes[pixel_num//128][color] for pixel_num, color in enumerate(top_spr_data)]
    bottom_spr_data = [spr_palettes[pixel_num//128][color] for pixel_num, color in enumerate(bottom_spr_data)]

    spr_img = Image.new("RGB", (2048, 16))
    for offset in range(0, len(top_spr_data), 64):
        tile_num = offset//64

        # Hack to reverse horizontal order of tiles within sprites
        if tile_num % 2 == 0:
            tile_num += 1
        else:
            tile_num -= 1

        for y in range(8):
            for x in range(8):
                # Note that we're rotating each tile 90 degrees right as we do this
                # Hence putting pixels at (7-y, x) instead of (x, y)
                spr_img.putpixel((tile_num*8+(7-y), x), top_spr_data[offset+y*8+x])
                spr_img.putpixel((tile_num*8+(7-y), x+8), bottom_spr_data[offset+y*8+x])

    spr_img.resize((4096, 32)).save("sprites.bmp")


def decode(file_list):
    outdata = [0] * 0x4000
    bitplane = 0
    for filename in file_list:
        with open(ROMDIR + filename, 'rb') as f:
            indata = f.read()
        for offset, value in enumerate(indata):
            value = ord(value)
            for bit in range(7, -1, -1):
                outdata[offset*8 + bit] |= (value & 1) << bitplane
                value >>= 1
        bitplane += 1
    return outdata


if __name__ == '__main__':
    main()
