/// @file z80_emulator//test/Instruction_ROM_test.cpp

#include <z80_emulator/Instruction_ROM.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Instruction_ROM.h> // Testing include guard

using namespace z80_emulator;

TEST_CASE( "Instruction_ROM_test", "stack" )
{
    uint16_t sample_rom_size = 0x1000;
    Instruction_ROM c(sample_rom_size);
}
