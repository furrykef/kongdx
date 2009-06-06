#!/usr/bin/env python
from __future__ import with_statement
import sys

from PIL import Image


tile_palette = [(0, 0, 0),
                (0x88, 0x88, 0x88),
                (0xcc, 0xcc, 0xcc),
                (0xff, 0xff, 0xff)]

tile_files = [
    "v_5h_b.bin",
    "v_3pt.bin"
]

spr_files = [
    "l_4m_b.bin",
    "l_4n_b.bin",
    "l_4r_b.bin",
    "l_4s_b.bin"
]


def main(argv=None):
    if argv is None:
        argv = sys.argv[1:]

    tiledata = [0] * 0x4000
    bitplane = 0
    for filename in tile_files:
        with open('ROMs/dkong/' + filename, 'rb') as f:
            data = f.read()
        for offset, value in enumerate(data):
            value = ord(value)
            for bit in range(7, -1, -1):
                tiledata[offset*8 + bit] |= (value & 1) << bitplane
                value >>= 1
        bitplane += 1

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

    if 0:
        sprdata = []
        for filename in tile_files:
            with open(filename, 'rb') as f:
                pass
    

if __name__ == '__main__':
    main()