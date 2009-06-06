#!/usr/bin/env python
from __future__ import with_statement
import sys

from PIL import Image


# Note: needs trailing slash
ROMDIR = 'roms/dkong/'


tile_palette = [(0, 0, 0),
                (0xFA, 0x28, 0x99),
                (0xBA, 0x0D, 0x33),
                (0x00, 0xFB, 0xFF)]

tile_files = [
    "v_5h_b.bin",
    "v_3pt.bin"
]

spr_palette = [(0, 0, 0),
               (0xB7, 0, 0),
               (0xFB, 0xBA, 0x99),
               (0x44, 0x8C, 0xCB)]

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
    tiledata = [tile_palette[x] for x in tiledata]

    tile_img = Image.new("RGB", (2048, 8))
    for offset in range(0, len(tiledata), 64):
        tile_num = offset/64
        for y in range(8):
            for x in range(8):
                # Note that we're rotating each tile 90 degrees right as we do this
                # Hence putting pixels at (7-y, x) instead of (x, y)
                tile_img.putpixel((tile_num*8+(7-y), x), tiledata[offset+y*8+x])

    tile_img.save("tiles.bmp")


    ### SPRITES ###
    top_spr_data = decode(top_spr_files)
    bottom_spr_data = decode(bottom_spr_files)

    # Convert paletted values to RGB values
    top_spr_data = [spr_palette[x] for x in top_spr_data]
    bottom_spr_data = [spr_palette[x] for x in bottom_spr_data]

    spr_img = Image.new("RGB", (2048, 16))
    for offset in range(0, len(top_spr_data), 64):
        tile_num = offset/64

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

    spr_img.save("sprites.bmp")


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
