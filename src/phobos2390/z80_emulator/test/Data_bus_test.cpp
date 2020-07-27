/// @file phobos2390/z80_emulator/test/Data_bus_test.cpp

#include <z80_emulator/Data_bus.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Data_bus.h> // Testing include guard
#include <z80_emulator/Data_bus_integer.h>
#include <z80_emulator/Instruction_ROM.h>
#include <emulation/CPU/Z80.h>

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

struct emu_context
{
    Z80 z80;
    Data_bus m_data_bus;
    bool m_halted;
};

zuint8 in_cb(void * /* context */, zuint16 port)
{
    printf("Value read from port: 0x%d: 0x0\n", port);
    return 0x0;
}

void out_cb(void * /* context */, zuint16 port, zuint8 value)
{
    printf("Value written to port: 0x%d: 0x%x\n", port, value);
}

void write_cb(void* context, zuint16 address, zuint8 value)
{
    emu_context* p_emu_context = static_cast<emu_context*>(context);
    p_emu_context->m_data_bus.write(address, value);
}

zuint8 read_cb(void* context, zuint16 address)
{
    emu_context* p_emu_context = static_cast<emu_context*>(context);
    return p_emu_context->m_data_bus.read(address);
}

void halt_cb(void* context, zboolean state)
{
    emu_context* p_emu_context = static_cast<emu_context*>(context);
    p_emu_context->m_halted = !state;
}

zuint32 int_data_cb(void* /*context*/)
{
    zuint32 data = 0;
    return data;
}


TEST_CASE("Data_bus_test z80_test", "z80_run_test")
{
    emu_context context;
    context.m_halted = false;
    uint16_t instruction_count = 0x100;
    Instruction_ROM instructions(instruction_count);
    context.m_data_bus.add_section(0, instructions);
    Z80& z80 = context.z80;
    z80.context = &context;
    z80.write = &write_cb;
    z80.read = &read_cb;
    z80.halt = &halt_cb;    
    z80.in = &in_cb;
    z80.out = &out_cb;
    z80.int_data = &int_data_cb;

    z80_power(&z80, TRUE);
    z80_reset(&z80);

}