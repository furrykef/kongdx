cmake_minimum_required (VERSION 2.6)
project (KONGDX)

include(TestBigEndian)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

link_libraries(
   ${SDL_LIBRARY}
   SDLmain
)

add_executable(kongdx main.cpp z80/Z80.c)


# Needed for Marat's CPU core
test_big_endian(BIG_ENDIAN)
if(BIG_ENDIAN)
  set(ENDIAN_FLAG "MSB_FIRST")
else(BIG_ENDIAN)
  set(ENDIAN_FLAG "LSB_FIRST")
endif(BIG_ENDIAN)

set_property(
   TARGET kongdx
   PROPERTY COMPILE_DEFINITIONS ${ENDIAN_FLAG}
)
