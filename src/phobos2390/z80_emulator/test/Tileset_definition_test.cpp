/// @file phobos2390/z80_emulator/test/Tileset_definition_test.cpp

#include <z80_emulator/Tileset_definition.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Tileset_definition.h> // Testing include guard

using namespace z80_emulator;

TEST_CASE( "Tileset_definition_test", "stack" )
{
    Tileset_metadata metadata;
    metadata.m_tile_width_pixels = 0x8;
    metadata.m_tile_height_pixels = 0x8;
    metadata.m_tile_color_depth = 0x1;
    memset(metadata.m_pallete, 0, sizeof(metadata.m_pallete));
    Tileset_definition c(metadata);
}

TEST_CASE( "Tileset_definition_test.binary_tileset", "binary_tileset" )
{
    Tileset_metadata metadata;
    metadata.m_tile_width_pixels = 0x8;
    metadata.m_tile_height_pixels = 0x8;
    metadata.m_tile_color_depth = 0x1;
    memset(metadata.m_pallete, 0, sizeof(metadata.m_pallete));
    metadata.m_pallete[0] = 0xFF000000;
    metadata.m_pallete[1] = 0xFF00FFCC;
    Tileset_definition c(metadata);
    uint16_t addr_value = 0;
    // Sets 0 as a random value
    c.set_data(addr_value++, 0b11110001);
    c.set_data(addr_value++, 0b00110010);
    c.set_data(addr_value++, 0b11001000);
    c.set_data(addr_value++, 0b00110001);
    c.set_data(addr_value++, 0b11000010);
    c.set_data(addr_value++, 0b00110100);
    c.set_data(addr_value++, 0b11000001);
    c.set_data(addr_value++, 0b00110010);

    // Sets 1 as A
    c.set_data(addr_value++, 0b00011000);
    c.set_data(addr_value++, 0b00100100);
    c.set_data(addr_value++, 0b00100100);
    c.set_data(addr_value++, 0b01000010);
    c.set_data(addr_value++, 0b01111110);
    c.set_data(addr_value++, 0b01000010);
    c.set_data(addr_value++, 0b01000010);
    c.set_data(addr_value++, 0b01000010);

    // Sets 2 as B
    c.set_data(addr_value++, 0b11111000);
    c.set_data(addr_value++, 0b10000100);
    c.set_data(addr_value++, 0b10000100);
    c.set_data(addr_value++, 0b11111100);
    c.set_data(addr_value++, 0b10000010);
    c.set_data(addr_value++, 0b10000010);
    c.set_data(addr_value++, 0b10000010);
    c.set_data(addr_value++, 0b11111100);

    c.write_tileset_to_file("test_files/binary_tileset_test.png");
}

TEST_CASE( "Tileset_definition_test.quad_tileset", "quad_tileset" )
{
    Tileset_metadata metadata;
    metadata.m_tile_width_pixels = 0x8;
    metadata.m_tile_height_pixels = 0x8;
    metadata.m_tile_color_depth = 0x2;
    memset(metadata.m_pallete, 0, sizeof(metadata.m_pallete));
    metadata.m_pallete[0] = 0x00000000;
    metadata.m_pallete[1] = 0xFF005500;
    metadata.m_pallete[2] = 0xFF00AA00;
    metadata.m_pallete[3] = 0xFF00FF00;
    Tileset_definition c(metadata);
    uint16_t addr_value = 0;
    // Sets 0 as a random value
    c.set_data(addr_value++, 0b00000000); c.set_data(addr_value++, 0b01010101);
    c.set_data(addr_value++, 0b10101010); c.set_data(addr_value++, 0b11111111);
    c.set_data(addr_value++, 0b00000000); c.set_data(addr_value++, 0b01010101);
    c.set_data(addr_value++, 0b10101010); c.set_data(addr_value++, 0b11111111);
    c.set_data(addr_value++, 0b00000000); c.set_data(addr_value++, 0b01010101);
    c.set_data(addr_value++, 0b10101010); c.set_data(addr_value++, 0b11111111);
    c.set_data(addr_value++, 0b00000000); c.set_data(addr_value++, 0b01010101);
    c.set_data(addr_value++, 0b10101010); c.set_data(addr_value++, 0b11111111);

    // Sets 1 as lightest a
    c.set_data(addr_value++, 0b00000011); c.set_data(addr_value++, 0b11000000);
    c.set_data(addr_value++, 0b00001100); c.set_data(addr_value++, 0b00110000);
    c.set_data(addr_value++, 0b00001100); c.set_data(addr_value++, 0b00110000);
    c.set_data(addr_value++, 0b00110000); c.set_data(addr_value++, 0b00001100);
    c.set_data(addr_value++, 0b00111111); c.set_data(addr_value++, 0b11111100);
    c.set_data(addr_value++, 0b00110000); c.set_data(addr_value++, 0b00001100);
    c.set_data(addr_value++, 0b00110000); c.set_data(addr_value++, 0b00001100);
    c.set_data(addr_value++, 0b00110000); c.set_data(addr_value++, 0b00001100);

    // Sets 1 as mid dark a
    c.set_data(addr_value++, 0b00000010); c.set_data(addr_value++, 0b10000000);
    c.set_data(addr_value++, 0b00001000); c.set_data(addr_value++, 0b00100000);
    c.set_data(addr_value++, 0b00001000); c.set_data(addr_value++, 0b00100000);
    c.set_data(addr_value++, 0b00100000); c.set_data(addr_value++, 0b00001000);
    c.set_data(addr_value++, 0b00101010); c.set_data(addr_value++, 0b10101000);
    c.set_data(addr_value++, 0b00100000); c.set_data(addr_value++, 0b00001000);
    c.set_data(addr_value++, 0b00100000); c.set_data(addr_value++, 0b00001000);
    c.set_data(addr_value++, 0b00100000); c.set_data(addr_value++, 0b00001000);

    // Sets 1 as darkest a
    c.set_data(addr_value++, 0b00000001); c.set_data(addr_value++, 0b01000000);
    c.set_data(addr_value++, 0b00000100); c.set_data(addr_value++, 0b00010000);
    c.set_data(addr_value++, 0b00000100); c.set_data(addr_value++, 0b00010000);
    c.set_data(addr_value++, 0b00010000); c.set_data(addr_value++, 0b00000100);
    c.set_data(addr_value++, 0b00010101); c.set_data(addr_value++, 0b01010100);
    c.set_data(addr_value++, 0b00010000); c.set_data(addr_value++, 0b00000100);
    c.set_data(addr_value++, 0b00010000); c.set_data(addr_value++, 0b00000100);
    c.set_data(addr_value++, 0b00010000); c.set_data(addr_value++, 0b00000100);


    c.write_tileset_to_file("test_files/quad_tileset_test.png");
}

TEST_CASE( "Tileset_definition_test.nibble_tileset", "nibble_tileset" )
{
    Tileset_metadata metadata;
    metadata.m_tile_width_pixels = 0x8;
    metadata.m_tile_height_pixels = 0x8;
    metadata.m_tile_color_depth = 0x4;
    memset(metadata.m_pallete, 0, sizeof(metadata.m_pallete));
    uint16_t pallete_iter = 0;
    metadata.m_pallete[pallete_iter++] = 0x00000000;
    metadata.m_pallete[pallete_iter++] = 0xFF000000;
    metadata.m_pallete[pallete_iter++] = 0xFF000088;
    metadata.m_pallete[pallete_iter++] = 0xFF008800;

    metadata.m_pallete[pallete_iter++] = 0xFF880000;
    metadata.m_pallete[pallete_iter++] = 0xFF008888;
    metadata.m_pallete[pallete_iter++] = 0xFF888800;
    metadata.m_pallete[pallete_iter++] = 0xFF880088;

    metadata.m_pallete[pallete_iter++] = 0xFF888888;
    metadata.m_pallete[pallete_iter++] = 0xFF0000FF;
    metadata.m_pallete[pallete_iter++] = 0xFF00FF00;
    metadata.m_pallete[pallete_iter++] = 0xFFFF0000;

    metadata.m_pallete[pallete_iter++] = 0xFFFF00FF;
    metadata.m_pallete[pallete_iter++] = 0xFF00FFFF;
    metadata.m_pallete[pallete_iter++] = 0xFFFFFF00;
    metadata.m_pallete[pallete_iter++] = 0xFFFFFFFF;

    Tileset_definition c(metadata);
    uint16_t addr_value = 0;
    uint8_t incrementing_value = 0;
    uint8_t first_tile [] = { 0x01, 0x23, 0x45, 0x67
                            , 0x89, 0xAB, 0xCD, 0xEF
                            , 0x01, 0x23, 0x45, 0x67
                            , 0x89, 0xAB, 0xCD, 0xEF
                            , 0xFE, 0xDC, 0xBA, 0x98
                            , 0x76, 0x54, 0x32, 0x10
                            , 0xFE, 0xDC, 0xBA, 0x98
                            , 0x76, 0x54, 0x32, 0x10 };

    for(uint32_t i = 0; i < sizeof(first_tile); i++)
    {
        c.set_data(i, first_tile[i]);
    }

    c.write_tileset_to_file("test_files/nibble_tileset_test.png");
}
