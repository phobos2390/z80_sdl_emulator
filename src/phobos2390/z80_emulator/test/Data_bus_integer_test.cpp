/// @file phobos2390/z80_emulator/test/Data_bus_integer_test.cpp

#include <z80_emulator/Data_bus_integer.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Data_bus_integer.h> // Testing include guard

using namespace z80_emulator;

TEST_CASE( "Data_bus_integer_test.stack", "stack" )
{
    Data_bus_integer<uint32_t> c;
}

TEST_CASE( "Data_bus_integer_test.set_integer", "set_integer" )
{
    uint32_t set_value_int = 0x12345678;
    Data_bus_integer<uint32_t> c;
    c.set_integer(set_value_int);
    REQUIRE( set_value_int == c.get_integer() );
    REQUIRE( 0x12 == c.get_data(0) );
    REQUIRE( 0x34 == c.get_data(1) );
    REQUIRE( 0x56 == c.get_data(2) );
    REQUIRE( 0x78 == c.get_data(3) );
}
