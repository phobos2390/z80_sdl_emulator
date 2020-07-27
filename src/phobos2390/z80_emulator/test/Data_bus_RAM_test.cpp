/// @file phobos2390/z80_emulator/test/Data_bus_RAM_test.cpp

#include <z80_emulator/Data_bus_RAM.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Data_bus_RAM.h> // Testing include guard

using namespace z80_emulator;

TEST_CASE( "Data_bus_RAM_test", "stack" )
{
    uint16_t example_size = 0x100;
    Data_bus_RAM c(example_size);
}

TEST_CASE( "Data_bus_RAM_test.set_value", "set_value" )
{
    uint16_t example_size = 0x100;
    uint8_t first_example_data = 0x34;
    uint8_t second_example_data = 0x53;
    Data_bus_RAM c(example_size);
    c.set_data(0, first_example_data);
    c.set_data(1, second_example_data);
    REQUIRE( first_example_data == c.get_data(0) );
    REQUIRE( second_example_data == c.get_data(1) );
}
