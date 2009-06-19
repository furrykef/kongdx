cmake_minimum_required(VERSION 2.6)
project(KongDX)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

find_package(SDL_mixer REQUIRED)
include_directories(${SDLMIXER_INCLUDE_DIR})

link_libraries(
   ${SDL_LIBRARY}
   ${SDLMIXER_LIBRARY}
)


add_executable(
  kongdx
    main.cpp
    file.cpp
    file.h
    gfxrip.cpp
    gfxrip.h
    video.h
    z80/opcodes_base.h
    z80/z80.cpp
    z80/z80.h
    z80/z80_cb.h
    z80/z80_ddfd.h
    z80/z80_ddfdcb.h
    z80/z80_ed.h
    z80/z80_macros.h
    z80/z80_ops.cpp
)
