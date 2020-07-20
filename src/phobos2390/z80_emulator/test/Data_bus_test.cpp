/// @file phobos2390/z80_emulator/test/Data_bus_test.cpp

#include <z80_emulator/Data_bus.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Data_bus.h> // Testing include guard
#include <z80_emulator/Data_bus_integer.h>

using namespace z80_emulator;

TEST_CASE( "Data_bus_test.stack", "stack" )
{
    Data_bus c;
}

TEST_CASE("Data_bus_test.read", "read" )
{
    Data_bus c;
    REQUIRE(0 == c.read(0x0000));
}

TEST_CASE("Data_bus_test.add_section no error", "add_section no error" )
{
    uint16_t address_iter = 0;
    Data_bus c;
    Data_bus_integer<uint32_t> x;
    Data_bus_integer<uint32_t> y;
    REQUIRE(s_c_error_none == c.add_section(address_iter, x));
    address_iter += x.get_section_size();
    REQUIRE(s_c_error_none == c.add_section(address_iter, y));
}

TEST_CASE("Data_bus_test.add_section overlap", "add_section overlap" )
{
    uint16_t address_iter = 0;
    Data_bus c;
    Data_bus_integer<uint32_t> x;
    Data_bus_integer<uint32_t> y;
    REQUIRE(s_c_error_none == c.add_section(address_iter, x));
    address_iter += x.get_section_size();
    --address_iter;
    REQUIRE(s_c_error_overlap == c.add_section(address_iter, y));
}

TEST_CASE("Data_bus_test read and write", "read and write" )
{
    uint16_t address_iter = 0x3200;
    uint8_t data_value = 0x32;
    Data_bus c;
    Data_bus_integer<uint32_t> x;
    REQUIRE(s_c_error_none == c.add_section(address_iter, x));
    x.set_data(0,data_value);
    REQUIRE(c.read(address_iter) == x.get_data(0));
}