/// @file z80_emulator//test/Instruction_ROM_test.cpp

#include <z80_emulator/Instruction_ROM.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Instruction_ROM.h> // Testing include guard
#include <string.h>
#include <iostream>

using namespace z80_emulator;

TEST_CASE( "Instruction_ROM_test", "stack" )
{
    uint16_t sample_rom_size = 0x1000;
    Instruction_ROM c(sample_rom_size);
}

TEST_CASE( "Instrucion_test.stack", "instruction.stack")
{
    instruction c("noop");
}

TEST_CASE( "Instruction_test.noop", "instruction.noop")
{
    instruction c("noop");
    REQUIRE( c.is_valid() );
    std::vector<uint8_t> binary;
    c.append_bytes(binary);
    REQUIRE( binary.size() == 1 );
    REQUIRE( binary[0] == 0 );
}

TEST_CASE( "Instruction_test.ld_a_5", "instruction.ld_a_5")
{
    instruction c("ld a,*", 0x5);
    REQUIRE( c.is_valid() );
    std::vector<uint8_t> binary;
    c.append_bytes(binary);
    REQUIRE( binary.size() == 2 );
    REQUIRE( binary[0] == 0x3E );
    REQUIRE( binary[1] == 0x5 );
}

TEST_CASE( "Instruction_test.ld_hl_8002", "instruction.ld_hl_8002")
{
    instruction c("ld hl,**", 0x80, 0x02);
    REQUIRE( c.is_valid() );
    std::vector<uint8_t> binary;
    c.append_bytes(binary);
    REQUIRE( binary.size() == 3 );
    REQUIRE( binary[0] == 0x21 );
    REQUIRE( binary[1] == 0x02 );
    REQUIRE( binary[2] == 0x80 );
}

TEST_CASE( "Instruction_test.ld_hl_8002 to_string", "instruction.ld_hl_8002 to_string")
{
    instruction c("ld hl,**", 0x80, 0x02);
    REQUIRE( c.is_valid() );
    REQUIRE( strncmp(c.to_string().c_str(), "ld hl,** $8002", strlen("ld hl,** $8002") + 1) == 0);
}

TEST_CASE( "Instruction_ROM_test.instruction_list", "sample_instruction_list_test")
{
    uint16_t sample_rom_size = 0x1000;
    Instruction_ROM c(sample_rom_size);
    c.add_symbolic_value("stdout_addr", 0x8000)
     .add_instruction("ld a,*", 0x1)
     .add_instruction("ld b,a")
     .add_instruction("inc a")
     .add_instruction_with_symbol("ld hl,**", "stdout_addr")
     .add_instruction("ld (hl),a")
     .add_instruction("ld (hl),b")
     .add_instruction("halt");
    
    std::vector<std::string>& failed_list = c.get_failed_instructions_list();
    
    REQUIRE( failed_list.size() == 0 );
    
    std::vector<uint16_t> instruction_list;
    c.parse_instruction_list(instruction_list);
    
    REQUIRE( instruction_list.size() == 7 );
    REQUIRE( instruction_list[0] == 0x0000 );
    REQUIRE( instruction_list[1] == 0x0002 );
    REQUIRE( instruction_list[2] == 0x0003 );
    REQUIRE( instruction_list[3] == 0x0004 );
    REQUIRE( instruction_list[4] == 0x0007 );
    REQUIRE( instruction_list[5] == 0x0008 );
    REQUIRE( instruction_list[6] == 0x0009 );
}
