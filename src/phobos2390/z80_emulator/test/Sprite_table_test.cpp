/// @file phobos2390/z80_emulator/test/Sprite_table_test.cpp

#include <z80_emulator/Sprite_table.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Sprite_table.h> // Testing include guard

using namespace z80_emulator;

TEST_CASE( "Sprite_table_test", "stack" )
{
    uint16_t sprite_count = 0x40;
    Sprite_table c(sprite_count);
}

TEST_CASE( "Sprite_table_test.set_sprite_values", "set_sprite_values")
{
    uint16_t sprite_count = 0x40;
    uint16_t sprite_y = 0x20;
    uint16_t sprite_x = 0x32;
    uint16_t sprite_tile = 'C';
    uint16_t sprite_flags = s_c_sprite_flipped_horizontal | s_c_sprite_visible;
    Sprite_table c(sprite_count);
    c.set_y(sprite_count - 1, sprite_y);
    c.set_x(sprite_count - 1, sprite_x);
    c.set_tile_type(sprite_count - 1, sprite_tile);
    c.set_flags(sprite_count - 1, sprite_flags);
}