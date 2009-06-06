#include <cassert>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include "Z80/Z80.h"

void resetGame();
void drawScreen();
void handleInput();
SDL_Surface *makeFlippedSprites(SDL_Surface *src, bool hflip, bool vflip);

const char *ROM_FILENAME = "dkong.rom";

unsigned char ROM[0x4000];
unsigned char RAM[0x1000];
unsigned char VRAM[0x400];
int IN0, IN1, IN2, DSW1;
SDL_Surface *screen;
SDL_Surface *tiles;
SDL_Surface *sprite_surfs[4];   // 0 = no flip, 1 = horizontal flip, etc.
bool g_vblank;                  // Tracks if vblank interrupts enabled

Z80 g_z80;

const int CYCLES_PER_VBLANK = 3072000 / 60;     // Is this correct?


int main(int argc, char *argv[])
{
    std::cout << "Welcome to Kong DX!" << std::endl;

    std::cout << "Loading program ROM..." << std::endl;

    {
        std::ifstream rom_file(ROM_FILENAME, std::ifstream::in | std::ifstream::binary);
        rom_file.read(reinterpret_cast<char *>(ROM), 0x4000);
    }

    std::cout << "Done loading ROM." << std::endl;

    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(224, 256, 32, SDL_SWSURFACE);
    SDL_WM_SetCaption("Kong DX", "Kong DX");

    std::cout << "Loading graphics..." << std::endl;
    tiles = SDL_LoadBMP("tiles.bmp");
    sprite_surfs[0] = SDL_LoadBMP("sprites.bmp");
    SDL_SetColorKey(sprite_surfs[0], SDL_SRCCOLORKEY, 0);
    sprite_surfs[1] = makeFlippedSprites(sprite_surfs[0], true, false);
    sprite_surfs[2] = makeFlippedSprites(sprite_surfs[0], false, true);
    sprite_surfs[3] = makeFlippedSprites(sprite_surfs[0], true, true);
    std::cout << "Done loading graphics." << std::endl;

    resetGame();
    RunZ80(&g_z80);

    // @XXX@ - cleanup?
    return 0;
}

void resetGame()
{
    IN0 = 0;
    IN1 = 0;
    IN2 = 0;
    DSW1 = 0x80;        // Factory setting
    g_vblank = false;
    g_z80.IPeriod = CYCLES_PER_VBLANK;
    ResetZ80(&g_z80);
}

void drawScreen()
{
    SDL_Rect src, dest;

    /*** Draw tiles ***/
    // These never change in the loop
    src.y = 0;
    src.w = 8;
    src.h = 8;
    dest.w = 8;
    dest.h = 8;

    for(int y = 0; y < 32; ++y)
    {
        for(int x = 0; x < 32; ++x)
        {
            int tile_id = VRAM[y*32+x];
            src.x = tile_id*8;

            // Note that we're rotating the display 90 degrees here
            dest.x = (29-y)*8;
            dest.y = x*8;
 
            SDL_BlitSurface(tiles, &src, screen, &dest);
        }
    }

    /*** Draw sprites ***/
    // These never change in the loop
    src.y = 0;
    src.w = 16;
    src.h = 16;
    dest.w = 16;
    dest.w = 16;

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

        src.x = sprite_id*16;

        dest.x = RAM[offset] - 23;
        dest.y = RAM[offset+3] - 8;

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

    if(keystate[SDLK_RIGHT])
    {
        IN0 |= 1;
    }

    if(keystate[SDLK_LEFT])
    {
        IN0 |= 2;
    }

    if(keystate[SDLK_UP])
    {
        IN0 |= 4;
    }

    if(keystate[SDLK_DOWN])
    {
        IN0 |= 8;
    }

    if(keystate[SDLK_LCTRL])
    {
        IN0 |= 0x10;
    }
}

SDL_Surface *makeFlippedSprites(SDL_Surface *src, bool hflip, bool vflip)
{
    SDL_Surface *surf = SDL_CreateRGBSurface(
        SDL_SWSURFACE,
        2048, 16, 24,
        src->format->Rmask,
        src->format->Gmask,
        src->format->Bmask,
        0
    );
    SDL_SetColorKey(surf, SDL_SRCCOLORKEY, 0);

    // Note: No locking necessary since we're working solely with software surfaces
    char *src_pixels = static_cast<char *>(src->pixels);
    char *dst_pixels = static_cast<char *>(surf->pixels);
    for(int i = 0; i < 2048*16; ++i)
    {
        int sprite_num = (i % 2048)/16;
        int sprite_x = i%16;
        int sprite_y = i/2048;

        if(hflip)
        {
            sprite_x = 15 - sprite_x;
        }

        if(vflip)
        {
            sprite_y = 15 - sprite_y;
        }

        int src_x = sprite_num*16 + sprite_x;

        int src_idx = (sprite_y*2048+src_x)*3;
        int dst_idx = i*3;

        dst_pixels[dst_idx] = src_pixels[src_idx];
        dst_pixels[dst_idx+1] = src_pixels[src_idx+1];
        dst_pixels[dst_idx+2] = src_pixels[src_idx+2];
    }

    return surf;
}


void WrZ80(register word Addr, register byte Value)
{
    if(Addr >= 0x6000 && Addr < 0x7000)
    {
        RAM[Addr - 0x6000] = Value;
    }
    else if(Addr >= 0x7000 && Addr < 0x7400)
    {
        // Ignore; nobody knows what the crap this region is,
        // not even the MAME guys :)
    }
    else if(Addr >= 0x7400 && Addr < 0x7800)
    {
        VRAM[Addr - 0x7400] = Value;
    }
    else if(Addr >= 0x7800 && Addr < 0x7810)
    {
        // @XXX@
        //std::cout << "DMA write" << std::endl;
    }
    else if(Addr == 0x7c00)
    {
        /*std::cout << "Music/sound effect: ";

        switch(Value)
        {
        case 0: std::cout << "null"; break;
        case 1: std::cout << "Dragnet"; break;
        case 2: std::cout << "How high can you get?"; break;
        case 3: std::cout << "time running out"; break;
        case 4: std::cout << "Hammer Time"; break;
        case 5: std::cout << "rivet 2 complete"; break;
        case 6: std::cout << "hammer hit"; break;
        case 7: std::cout << "screen complete"; break;
        case 8: std::cout << "barrel music"; break;
        case 9: std::cout << "unknown"; break;
        case 10: std::cout << "springs music"; break;
        case 11: std::cout << "rivet music"; break;
        case 12: std::cout << "rivet 1 complete"; break;
        case 13: std::cout << "score!"; break;
        case 14: std::cout << "Kong's about to fall"; break;
        case 15: std::cout << "roar"; break;
        default:
            std::cout << "invalid";
        }

        std::cout << std::endl;*/
    }
    else if(Addr == 0x7d84)
    {
        g_vblank = (Value != 0);
    }
    else if(Addr == 0x7d85)
    {
        // @XXX@
        //std::cout << "DMA 1/0 toggle" << std::endl;
    }
    else if(Addr == 0x7d86 || Addr == 0x7d87)
    {
        //std::cout << "Palette selector" << std::endl;
    }
    else
    {
        /*std::cout << std::hex;
        std::cout << "Write " << int(Value) << " to " << Addr << std::endl;
        std::cout << std::dec;*/
    }
}

byte RdZ80(register word Addr)
{
    if(Addr < 0x4000)
    {
        return ROM[Addr];
    }
    else if(Addr >= 0x6000 && Addr < 0x7000)
    {
        return RAM[Addr - 0x6000];
    }
    else if(Addr >= 0x7400 && Addr < 0x7800)
    {
        return VRAM[Addr - 0x7400];
    }
    else if(Addr == 0x7c00)
    {
        return IN0;
    }
    else if(Addr == 0x7c80)
    {
        return IN1;
    }
    else if(Addr == 0x7d00)
    {
        return IN2;
    }
    else if(Addr == 0x7d80)
    {
        return DSW1;
    }
    else
    {
        /*std::cout << std::hex;
        std::cout << "Read from " << Addr << std::endl;
        std::cout << std::dec;*/
        return 0;
    }

    assert(!"Can't get here.");
}

void OutZ80(register word Port, register byte Value)
{
    // Not used
}

byte InZ80(register word Port)
{
    // Not used
    return 0;
}

void PatchZ80(register Z80 *R)
{
    // Not used
}

word LoopZ80(register Z80 *R)
{
    static bool fullscreen = false;

    // SDL events
    SDL_Event evt;
    while(SDL_PollEvent(&evt))
    {
        switch(evt.type)
        {
          case SDL_KEYDOWN:
            if(evt.key.keysym.sym == SDLK_RETURN && (evt.key.keysym.mod & KMOD_ALT))
            {
                fullscreen = !fullscreen;
                screen = SDL_SetVideoMode(
                    224, 256, 32,
                    fullscreen ? (SDL_FULLSCREEN | SDL_HWSURFACE) : SDL_SWSURFACE
                );
            }
            else if(evt.key.keysym.sym == SDLK_ESCAPE)
            {
                return INT_QUIT;
            }
            break;

          case SDL_QUIT:
            return INT_QUIT;
        }
    }

    handleInput();

    drawScreen();
    SDL_Flip(screen);

    // @FIXME@ -- keep track of FPS
    // (or find better timing mechanism)
    SDL_Delay(10);

    return g_vblank ? INT_NMI : INT_NONE;
}
