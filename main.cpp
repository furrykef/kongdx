#include <cassert>
#include <iostream>
#include <fstream>
#include <SDL.h>
#include "Z80/Z80.h"

void resetGame();
void drawScreen();
void handleInput();

const char *ROM_FILENAME = "dkong.rom";

unsigned char ROM[0x4000];
unsigned char RAM[0x1000];
unsigned char VRAM[0x400];
int IN0, IN1, IN2, DSW1;
SDL_Surface *screen;
SDL_Surface *tiles;
SDL_Surface *sprites;
bool g_vblank;          // Tracks if vblank interrupts enabled

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
    screen = SDL_SetVideoMode(224, 256, 8, SDL_SWSURFACE);
    SDL_WM_SetCaption("Kong DX", "Kong DX");

    std::cout << "Loading graphics..." << std::endl;
    tiles = SDL_LoadBMP("tiles.bmp");
    sprites = SDL_LoadBMP("sprites.bmp");
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
        //bool flip_x = RAM[offset+1] & 0x80;
        //bool flip_y = RAM[offset+2] & 0x80;

        src.x = sprite_id*16;

        dest.x = RAM[offset] - 23;
        dest.y = RAM[offset+3] - 8;

        //std::cout << "Sprite: " << dest.x << ", " << dest.y << std::endl;

        SDL_BlitSurface(sprites, &src, screen, &dest);
    }
}

void handleInput()
{
    // Input
    int num_keys;
    Uint8 *keystate = SDL_GetKeyState(&num_keys);

    if(keystate[SDLK_5])
    {
        IN2 |= 0x80;
    }
    else
    {
        IN2 &= ~0x80;
    }

    if(keystate[SDLK_1])
    {
        IN2 |= 4;
    }
    else
    {
        IN2 &= ~4;
    }

    if(keystate[SDLK_2])
    {
        IN2 |= 8;
    }
    else
    {
        IN2 &= ~8;
    }
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
    // SDL events
    SDL_Event evt;
    while(SDL_PollEvent(&evt))
    {
        switch(evt.type)
        {
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
