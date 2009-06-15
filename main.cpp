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
#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include "SDL.h"
#include "SDL_mixer.h"
#include "z80/z80.h"

const int SCREEN_WIDTH = 448;
const int SCREEN_HEIGHT = 512;
const int SCREEN_BPP = 32;
const int AUDIO_BUF_SIZE = 4096;    // @TODO@ -- what value to use?

enum SOUND_ID
{
    SND_BOOM,
    SND_HAMMERHIT,
    SND_JUMP,
    SND_SPRING,
    SND_FALL,
    SND_SCORE,
    SND_RIVET,
    NUM_SOUNDS
};

bool g_sound_regs[NUM_SOUNDS];

void runZ80();
void loadROMs(const char *romset);
void loadROM(const char *filename, std::size_t where, std::size_t size);
void resetGame();
void drawScreen();
void handleInput();
SDL_Surface *makeFlippedSprites(SDL_Surface *src, bool hflip, bool vflip);
void playMusic(Mix_Music *what, bool loop);
void playSound(Mix_Chunk *what, SOUND_ID snd_reg, int value);
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


Mix_Music *mus_intro;
Mix_Music *mus_howhigh;
Mix_Music *mus_death;
Mix_Music *mus_barrels;
Mix_Music *mus_pies;
Mix_Music *mus_springs;
Mix_Music *mus_rivets;
Mix_Music *mus_lowtime;
Mix_Music *mus_hammer;
Mix_Music *mus_screencomplete;
Mix_Music *mus_rescue1;
Mix_Music *mus_rescue2;
Mix_Music *mus_kongfall;
Mix_Chunk *snd_boom;
Mix_Chunk *snd_hammerhit;
Mix_Chunk *snd_jump;
Mix_Chunk *snd_spring;
Mix_Chunk *snd_fall;
Mix_Chunk *snd_score;

SDL_Joystick *joy = NULL;

// 3.072 MHz divided by 60 frames per second.
// (@TODO@ - Is this the correct way to calculate this?)
const int CYCLES_PER_VBLANK = 3072000 / 60;

const unsigned char DIP_FACTORY = 0x80;


int main(int argc, char *argv[])
{
    const char *romset;

    if(argc > 1)
    {
        romset = argv[1];
    }
    else
    {
        romset = "dkong";
    }

    loadROMs(romset);

    SDL_Init(
        SDL_INIT_VIDEO
        | SDL_INIT_AUDIO
        | SDL_INIT_JOYSTICK
#ifndef WIN32
        | SDL_INIT_EVENTTHREAD
#endif
    );
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
    SDL_WM_SetCaption("Kong DX", "Kong DX");

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

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, AUDIO_BUF_SIZE);
    mus_intro = Mix_LoadMUS("sounds/dkong/intro.ogg");
    mus_howhigh = Mix_LoadMUS("sounds/dkong/howhigh.ogg");
    mus_death = Mix_LoadMUS("sounds/dkong/death.ogg");
    mus_barrels = Mix_LoadMUS("sounds/dkong/barrels.ogg");
    mus_pies = Mix_LoadMUS("sounds/dkong/pies.ogg");
    mus_springs = Mix_LoadMUS("sounds/dkong/springs.ogg");
    mus_rivets = Mix_LoadMUS("sounds/dkong/rivets.ogg");
    mus_lowtime = Mix_LoadMUS("sounds/dkong/lowtime.ogg");
    mus_hammer = Mix_LoadMUS("sounds/dkong/hammer.ogg");
    mus_screencomplete = Mix_LoadMUS("sounds/dkong/screencomplete.ogg");
    mus_rescue1 = Mix_LoadMUS("sounds/dkong/rescue1.ogg");
    mus_rescue2 = Mix_LoadMUS("sounds/dkong/rescue2.ogg");
    mus_kongfall = Mix_LoadMUS("sounds/dkong/kongfall.ogg");
    snd_boom = Mix_LoadWAV("sounds/dkong/boom.ogg");
    snd_hammerhit = Mix_LoadWAV("sounds/dkong/hammerhit.ogg");
    snd_jump = Mix_LoadWAV("sounds/dkong/jump.ogg");
    snd_spring = Mix_LoadWAV("sounds/dkong/spring.ogg");
    snd_fall = Mix_LoadWAV("sounds/dkong/fall.ogg");
    snd_score = Mix_LoadWAV("sounds/dkong/score.ogg");

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

void loadROMs(const char *romset)
{
    if(strcmp(romset, "dkong") == 0)
    {
        loadROM("roms/dkong/c_5et_g.bin", 0, 0x1000);
        loadROM("roms/dkong/c_5ct_g.bin", 0x1000, 0x1000);
        loadROM("roms/dkong/c_5bt_g.bin", 0x2000, 0x1000);
        loadROM("roms/dkong/c_5at_g.bin", 0x3000, 0x1000);
    }
    else
    {
        // Assume dkongjp for now
        loadROM("roms/dkongjp/c_5f_b.bin", 0, 0x1000);
        loadROM("roms/dkongjp/5g.cpu", 0x1000, 0x1000);
        loadROM("roms/dkongjp/5h.cpu", 0x2000, 0x1000);
        loadROM("roms/dkongjp/c_5k_b.bin", 0x3000, 0x1000);
    }
}

// @XXX@ -- no error checking
void loadROM(const char *filename, std::size_t where, std::size_t size)
{
    std::ifstream rom_file(filename, std::ifstream::in | std::ifstream::binary);
    rom_file.read(reinterpret_cast<char *>(&ROM[where]), size);
}


void resetGame()
{
    playMusic(NULL, false);
    Mix_HaltChannel(-1);                // Stop sound effects
    std::fill(g_sound_regs, g_sound_regs+NUM_SOUNDS, false);
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

    // This is necessary because most music is controlled by the same
    // sound register as these effects, so a command to play music
    // means we're NOT playing one of these sound effects right now.
    g_sound_regs[SND_HAMMERHIT] = 0;
    g_sound_regs[SND_RIVET] = 0;

    if(what != prev_tune)
    {
        if(what != NULL)
        {
            // @HACK@: game keeps calling "low time" music even when
            // Mario is already dead, so ignore in this case
            if(prev_tune == mus_death && what == mus_lowtime)
            {
                return;
            }

            Mix_PlayMusic(what, loop ? -1 : 1);
        }
        else
        {
            Mix_HaltMusic();
        }
        prev_tune = what;
    }
}

void playSound(Mix_Chunk *what, SOUND_ID snd_reg, int value)
{
    assert(snd_reg >= 0);
    assert(snd_reg < NUM_SOUNDS);

    // We play a sound effect ONLY if value is nonzero AND if
    // the previous value written to this sound register was zero
    // (Prevents sounds being triggered multiple times)
    if(value != 0 && !g_sound_regs[snd_reg])
    {
        Mix_PlayChannel(-1, what, 0);
    }

    g_sound_regs[snd_reg] = (value != 0);
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
            // No sound
            g_sound_regs[SND_HAMMERHIT] = 0;
            g_sound_regs[SND_RIVET] = 0;
            break;
          case 1:
            playMusic(mus_intro, false);
            break;
          case 2:
            playMusic(mus_howhigh, false);
            break;
          case 3:
            playMusic(mus_lowtime, true);
            break;
          case 4:
            playMusic(mus_hammer, true);
            break;
          case 5:
            // Rivet 2 complete
            playMusic(mus_rescue2, false);
            break;
          case 6:
            // Hammer hit
            playSound(snd_hammerhit, SND_HAMMERHIT, value);
            break;
          case 7:
            playMusic(mus_screencomplete, false);
            break;
          case 8:
            playMusic(mus_barrels, true);
            break;
          case 9:
            playMusic(mus_pies, true);
            break;
          case 10:
            playMusic(mus_springs, true);
            break;
          case 11:
            playMusic(mus_rivets, true);
            break;
          case 12:
            // Rivet 1 complete
            playMusic(mus_rescue1, false);
            break;
          case 13:
            // Rivet removed
            playSound(snd_score, SND_RIVET, value);
            break;
          case 14:
            // Kong's about to fall
            playMusic(mus_kongfall, false);
            break;
          case 15:
            // Roar!
            break;
          default:
            std::cerr << "Unknown tune/sfx: " << int(value) << std::endl;
        }
    }
    else if(addr == 0x7d00)
    {
        // Walk noise
    }
    else if(addr == 0x7d01)
    {
        playSound(snd_jump, SND_JUMP, value);
    }
    else if(addr == 0x7d02)
    {
        playSound(snd_boom, SND_BOOM, value);
    }
    else if(addr == 0x7d03)
    {
        playSound(snd_spring, SND_SPRING, value);
    }
    else if(addr == 0x7d04)
    {
        playSound(snd_fall, SND_FALL, value);
    }
    else if(addr == 0x7d05)
    {
        playSound(snd_score, SND_SCORE, value);
    }
    else if(addr == 0x7d80 && value != 0)
    {
        playMusic(mus_death, false);
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
                    SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
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
