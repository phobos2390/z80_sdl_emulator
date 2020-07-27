/// @file z80_emulator/test/Grid_Display_test.cpp

#include <z80_emulator/Grid_Display.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Grid_Display.h> // Testing include guard

using namespace z80_emulator;

TEST_CASE( "Grid_Display_test", "stack" )
{
    Tileset_metadata metadata;
    metadata.m_tile_width_pixels = 0x8;
    metadata.m_tile_height_pixels = 0x8;
    metadata.m_tile_color_depth = 0x4;
    memset(metadata.m_pallete, 0, sizeof(metadata.m_pallete));
    uint16_t height = 0x10;
    uint16_t width = 0x10;
    Grid_Display c(metadata, width, height);
}
