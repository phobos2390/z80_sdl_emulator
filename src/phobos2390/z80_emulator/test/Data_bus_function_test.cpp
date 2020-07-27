/// @file z80_emulator/test/Data_bus_function_test.cpp

#include <z80_emulator/Data_bus_function.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Data_bus_function.h> // Testing include guard

using namespace z80_emulator;

void write_function(void* /*p_context */, uint16_t /* address */, uint8_t /* value */)
{
    
}

uint8_t read_function(void* /*p_context */, uint16_t /* address */)
{
    return 0;
}

TEST_CASE( "Data_bus_function_test", "stack" )
{
    Data_bus_function c(1, NULL, read_function, write_function);
}
