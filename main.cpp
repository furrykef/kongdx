/*
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
#include <iostream>
#include <fstream>
#include "SDL.h"
#include "SDL_mixer.h"
#include "z80/z80.h"

void runZ80();
void loadROMs();
void resetGame();
void drawScreen();
void handleInput();
SDL_Surface *makeFlippedSprites(SDL_Surface *src, bool hflip, bool vflip);
void playMusic(Mix_Music *what, bool loop);
void writebyte(uint16, uint8);
uint8 readbyte(uint16);
void writeport(uint16, uint8);
uint8 readport(uint16);
bool doFrame();

unsigned char ROM[0x4000];
unsigned char RAM[0x1000];
unsigned char VRAM[0x400];
int IN0, IN1, IN2, DSW1;
SDL_Surface *screen;
SDL_Surface *tiles;
SDL_Surface *sprite_surfs[4];   // 0 = no flip, 1 = horizontal flip, etc.
bool g_vblank_enabled;          // Tracks if vblank interrupts enabled

Mix_Music *mus_dragnet;
Mix_Music *mus_howhigh;
Mix_Music *mus_death;
Mix_Music *mus_barrels;
Mix_Music *mus_springs;
Mix_Music *mus_rivets;
Mix_Music *mus_hammer;
Mix_Chunk *snd_boom;

SDL_Joystick *joy = NULL;

const int CYCLES_PER_VBLANK = 3072000 / 60;     // Is this correct?
const unsigned char DIP_FACTORY = 0x80;


int main(int argc, char *argv[])
{
    std::cout << "Welcome to Kong DX!" << std::endl;

    std::cout << "Loading program ROMs..." << std::endl;
    loadROMs();
    std::cout << "Done loading ROMs." << std::endl;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    screen = SDL_SetVideoMode(448, 512, 32, SDL_SWSURFACE);
    SDL_WM_SetCaption("Kong DX", "Kong DX");

    std::cout << "Loading graphics..." << std::endl;
    SDL_Surface *tmp = SDL_LoadBMP("tiles.bmp");
    tiles = SDL_ConvertSurface(tmp, screen->format, SDL_SWSURFACE);
    SDL_FreeSurface(tmp);

    tmp = SDL_LoadBMP("sprites.bmp");
    SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, 0);
    sprite_surfs[1] = makeFlippedSprites(tmp, true, false);
    sprite_surfs[2] = makeFlippedSprites(tmp, false, true);
    sprite_surfs[3] = makeFlippedSprites(tmp, true, true);
    // We have to do this last since makeFlippedSprites requires 24-bit color
    sprite_surfs[0] = SDL_ConvertSurface(tmp, screen->format, SDL_SWSURFACE);
    SDL_FreeSurface(tmp);
    std::cout << "Done loading graphics." << std::endl;

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    mus_dragnet = Mix_LoadMUS("sound/dragnet.ogg");
    mus_howhigh = Mix_LoadMUS("sound/howhigh.ogg");
    mus_death = Mix_LoadMUS("sound/death.ogg");
    mus_barrels = Mix_LoadMUS("sound/barrels.ogg");
    mus_springs = Mix_LoadMUS("sound/springs.ogg");
    mus_rivets = Mix_LoadMUS("sound/rivets.ogg");
    mus_hammer = Mix_LoadMUS("sound/hammer.ogg");
    snd_boom = Mix_LoadWAV("sound/boom.wav");

    if(SDL_NumJoysticks() > 0)
    {
        joy = SDL_JoystickOpen(0);
    }

    z80_init();
    z80_readbyte = readbyte;
    z80_writebyte = writebyte;
    z80_readport = readport;
    z80_writeport = writeport;

    resetGame();
    runZ80();

    // @XXX@ - cleanup (release surfaces, joys, etc.)?
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}

void runZ80()
{
    int cycle_count = 0;
    for(;;)
    {
        z80_do_opcode();
        ++cycle_count;
        if(cycle_count >= CYCLES_PER_VBLANK)
        {
            // Z80 has hit vblank
            if(g_vblank_enabled)
            {
                z80_nmi();
            }

            // Handle events and draw
            if(!doFrame())
            {
                return;
            }

            cycle_count -= CYCLES_PER_VBLANK;
        }
    }
}

void loadROMs()
{
    {
        std::ifstream rom_file("roms/dkong/c_5et_g.bin", std::ifstream::in | std::ifstream::binary);
        rom_file.read(reinterpret_cast<char *>(ROM), 0x1000);
    }
    {
        std::ifstream rom_file("roms/dkong/c_5ct_g.bin", std::ifstream::in | std::ifstream::binary);
        rom_file.read(reinterpret_cast<char *>(&ROM[0x1000]), 0x1000);
    }
    {
        std::ifstream rom_file("roms/dkong/c_5bt_g.bin", std::ifstream::in | std::ifstream::binary);
        rom_file.read(reinterpret_cast<char *>(&ROM[0x2000]), 0x1000);
    }
    {
        std::ifstream rom_file("roms/dkong/c_5at_g.bin", std::ifstream::in | std::ifstream::binary);
        rom_file.read(reinterpret_cast<char *>(&ROM[0x3000]), 0x1000);
    }
}

void resetGame()
{
    playMusic(NULL, false);
    IN0 = 0;
    IN1 = 0;
    IN2 = 0;
    DSW1 = DIP_FACTORY;
    g_vblank_enabled = false;
    z80_reset();
}

void drawScreen()
{
    SDL_Rect src, dest;

    /*** Draw tiles ***/
    // These never change in the loop
    src.y = 0;
    src.w = 16;
    src.h = 16;
    dest.w = 16;
    dest.h = 16;

    for(int y = 0; y < 32; ++y)
    {
        for(int x = 0; x < 32; ++x)
        {
            int tile_id = VRAM[y*32+x];
            src.x = tile_id*16;

            // Note that we're rotating the display 90 degrees here
            dest.x = (29-y)*16;
            dest.y = x*16;
 
            SDL_BlitSurface(tiles, &src, screen, &dest);
        }
    }

    /*** Draw sprites ***/
    // These never change in the loop
    src.y = 0;
    src.w = 32;
    src.h = 32;
    dest.w = 32;
    dest.w = 32;

    for(int offset = 0x900; offset < 0xa80; offset += 4)
    {
        int sprite_id = RAM[offset+1] & 0x7f;
        //int palette = RAM[offset+2] & 0x7f;
        bool flip_x = (RAM[offset+1] & 0x80) != 0;
        bool flip_y = (RAM[offset+2] & 0x80) != 0;

        int sprite_surf_idx = 0;
        if(flip_x)
        {
            sprite_surf_idx |= 1;
        }
        if(flip_y)
        {
            sprite_surf_idx |= 2;
        }

        src.x = sprite_id*32;

        dest.x = (RAM[offset] - 23)*2;
        dest.y = (RAM[offset+3] - 8)*2;

        SDL_BlitSurface(sprite_surfs[sprite_surf_idx], &src, screen, &dest);
    }
}

void handleInput()
{
    // Input
    int num_keys;
    Uint8 *keystate = SDL_GetKeyState(&num_keys);

    IN0 = 0;
    IN1 = 0;
    IN2 = 0;

    if(keystate[SDLK_5])
    {
        IN2 |= 0x80;
    }

    if(keystate[SDLK_1])
    {
        IN2 |= 4;
    }

    if(keystate[SDLK_2])
    {
        IN2 |= 8;
    }

    if(keystate[SDLK_RIGHT] && !keystate[SDLK_LEFT])
    {
        IN0 |= 1;
    }
    else if(keystate[SDLK_LEFT] && !keystate[SDLK_RIGHT])
    {
        IN0 |= 2;
    }

    if(keystate[SDLK_UP] && !keystate[SDLK_DOWN])
    {
        IN0 |= 4;
    }
    else if(keystate[SDLK_DOWN] && !keystate[SDLK_UP])
    {
        IN0 |= 8;
    }

    if(keystate[SDLK_LCTRL])
    {
        IN0 |= 0x10;
    }

    if(joy != NULL)
    {
        Sint16 x = SDL_JoystickGetAxis(joy, 0);
        Sint16 y = SDL_JoystickGetAxis(joy, 1);

        if(x > 8192)
        {
            IN0 |= 1;
        }
        else if(x < -8192)
        {
            IN0 |= 2;
        }

        if(y < -8192)
        {
            IN0 |= 4;
        }
        else if(y > 8192)
        {
            IN0 |= 8;
        }

        if(SDL_JoystickGetButton(joy, 0))
        {
            IN0 |= 0x10;
        }
    }
}

SDL_Surface *makeFlippedSprites(SDL_Surface *src, bool hflip, bool vflip)
{
    SDL_Surface *surf = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        4096, 32, 24,
        src->format->Rmask,
        src->format->Gmask,
        src->format->Bmask,
        0
    );
    SDL_SetColorKey(surf, SDL_SRCCOLORKEY, 0);

    // Note: No locking necessary since we're working solely with software surfaces
    char *src_pixels = static_cast<char *>(src->pixels);
    char *dst_pixels = static_cast<char *>(surf->pixels);
    for(int i = 0; i < 4096*32; ++i)
    {
        int sprite_num = (i % 4096)/32;
        int sprite_x = i%32;
        int sprite_y = i/4096;

        if(hflip)
        {
            sprite_x = 31 - sprite_x;
        }

        if(vflip)
        {
            sprite_y = 31 - sprite_y;
        }

        int src_x = sprite_num*32 + sprite_x;

        int src_idx = (sprite_y*4096+src_x)*3;
        int dst_idx = i*3;

        dst_pixels[dst_idx] = src_pixels[src_idx];
        dst_pixels[dst_idx+1] = src_pixels[src_idx+1];
        dst_pixels[dst_idx+2] = src_pixels[src_idx+2];
    }

    SDL_Surface *conv_surf = SDL_ConvertSurface(surf, screen->format, SDL_SWSURFACE);
    SDL_FreeSurface(surf);
    return conv_surf;
}

// NULL = stop music
void playMusic(Mix_Music *what, bool loop)
{
    static Mix_Music *prev_tune = NULL;

    if(what != prev_tune)
    {
        if(what != NULL)
        {
            Mix_PlayMusic(what, loop ? -1 : 1);
        }
        else
        {
            Mix_HaltMusic();
        }
        prev_tune = what;
    }
}


void writebyte(uint16 addr, uint8 value)
{
    if(addr >= 0x6000 && addr < 0x7000)
    {
        RAM[addr - 0x6000] = value;
    }
    else if(addr >= 0x7400 && addr < 0x7800)
    {
        VRAM[addr - 0x7400] = value;
    }
    else if(addr == 0x7c00)
    {
        switch(value)
        {
          case 0:
            // Do nothing
            break;
          case 1:
            playMusic(mus_dragnet, false);
            break;
          case 2:
            playMusic(mus_howhigh, false);
            break;
          case 3:
            // Time running out
            break;
          case 4:
            playMusic(mus_hammer, true);
            break;
          case 5:
            // Rivet 2 complete
            break;
          case 6:
            // Hammer hit
            break;
          case 7:
            // Screen complete
            break;
          case 8:
            playMusic(mus_barrels, true);
            break;
          case 10:
            // Springs music
            // (Weird considering that the arcade machine didn't even
            //  PLAY music on this stage)
            playMusic(mus_springs, true);
            break;
          case 11:
            // Rivets music
            playMusic(mus_rivets, true);
            break;
          case 12:
            // Rivet 1 complete
            break;
          case 13:
            // Score!
            break;
          case 14:
            // Kong's about to fall
            break;
          case 15:
            // Roar!
            break;
          default:
            std::cerr << "Unknown tune/sfx: " << int(value) << std::endl;
        }
    }
    else if(addr == 0x7d84)
    {
        g_vblank_enabled = (value != 0);
    }
    else if(addr == 0x7d86 || addr == 0x7d87)
    {
        //std::cout << "Palette selector" << std::endl;
    }
}

uint8 readbyte(uint16 addr)
{
    if(addr < 0x4000)
    {
        return ROM[addr];
    }
    else if(addr >= 0x6000 && addr < 0x7000)
    {
        return RAM[addr - 0x6000];
    }
    else if(addr >= 0x7400 && addr < 0x7800)
    {
        return VRAM[addr - 0x7400];
    }
    else if(addr == 0x7c00)
    {
        return IN0;
    }
    else if(addr == 0x7c80)
    {
        return IN1;
    }
    else if(addr == 0x7d00)
    {
        return IN2;
    }
    else if(addr == 0x7d80)
    {
        return DSW1;
    }

    return 0;
}

void writeport(uint16 port, uint8 value)
{
    // Not used
}

uint8 readport(uint16 port)
{
    // Not used
    return 0;
}

// Return value:
//  true = continue emulation
//  false = exit program
bool doFrame()
{
    // SDL events
    SDL_Event evt;
    while(SDL_PollEvent(&evt))
    {
        switch(evt.type)
        {
          case SDL_KEYDOWN:
            if(evt.key.keysym.sym == SDLK_RETURN && (evt.key.keysym.mod & KMOD_ALT))
            {
                static bool fullscreen = false;

                fullscreen = !fullscreen;
                screen = SDL_SetVideoMode(
                    448, 512, 32,
                    fullscreen ? (SDL_FULLSCREEN | SDL_HWSURFACE) : SDL_SWSURFACE
                );
                SDL_ShowCursor(fullscreen ? SDL_DISABLE : SDL_ENABLE);
            }
            else if(evt.key.keysym.sym == SDLK_r)
            {
                resetGame();
            }
            else if(evt.key.keysym.sym == SDLK_ESCAPE)
            {
                return false;
            }
            break;

          case SDL_QUIT:
            return false;
        }
    }

    handleInput();

    drawScreen();
    SDL_Flip(screen);

    // @FIXME@ -- keep track of FPS
    // (or find better timing mechanism)
    SDL_Delay(10);

    return true;
}
