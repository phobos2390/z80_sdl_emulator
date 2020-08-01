/// @file phobos2390/z80_emulator/test/Data_bus_test.cpp

#include <z80_emulator/Data_bus.h>
#include <catch2/catch.hpp>
#include <z80_emulator/Data_bus.h> // Testing include guard
#include <z80_emulator/Data_bus_integer.h>
#include <z80_emulator/Data_bus_function.h>
#include <z80_emulator/Data_bus_RAM.h>
#include <z80_emulator/Instruction_ROM.h>
#include <emulation/CPU/Z80.h>
#include <iostream>
#include <sstream>

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

TEST_CASE("Data_bus_test read", "read and write" )
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
    uint8_t ret_val = p_emu_context->m_data_bus.read(address);
    return ret_val;
}

void halt_cb(void* context, zboolean state)
{
    emu_context* p_emu_context = static_cast<emu_context*>(context);
    p_emu_context->m_halted = state;
}

zuint32 int_data_cb(void* /*context*/)
{
    zuint32 data = 0;
    return data;
}

uint8_t read_stdout_literal (void* /*context*/, uint16_t /*address*/)
{
    return 0;
}

void write_stdout_literal (void* context, uint16_t /*address*/, uint8_t value)
{
    std::ostream& os = *static_cast<std::ostream*>(context);
    os << std::hex << static_cast<uint16_t>(value);
}

uint8_t read_stdout_string (void* /*context*/, uint16_t /*address*/)
{
    return 0;
}

void write_stdout_string (void* context, uint16_t /*address*/, uint8_t value)
{
    std::ostream& os = *static_cast<std::ostream*>(context);
    os << value;
}

TEST_CASE("Data_bus_test basic_z80_test", "z80_run_test")
{
    emu_context context;
    context.m_halted = false;
    uint16_t instruction_count = 0x100;
    Instruction_ROM instructions(instruction_count);
    instructions.add_symbolic_value("stdout_addr", 0x8000)
                .add_instruction("ld a,*", 0x5)
                .add_instruction("ld b,a")
                .add_instruction("ld a,*", 0x2)
                .add_instruction("add a,b")
                .add_instruction_with_symbol("ld hl,**", "stdout_addr")
                .add_instruction("ld (hl),a")
                .add_instruction("halt");

    REQUIRE( 0 == instructions.get_failed_instructions_list().size() );
    
    REQUIRE(s_c_error_none == context.m_data_bus.add_section(0, instructions));
    std::stringstream sstr;
    Data_bus_function stdout_fd(1, &sstr, &read_stdout_literal, &write_stdout_literal);
    REQUIRE(s_c_error_none == context.m_data_bus.add_section(0x8000, stdout_fd));
    
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
    
    uint32_t cycles = 0;
    while(!context.m_halted && cycles < 0x8000)
    {
        ++cycles;
        z80_run(&z80,1);
    }
    
    uint16_t output = 0;
    sstr << std::hex;
    sstr >> output;
    
    REQUIRE( context.m_halted );
    REQUIRE( 0x7 == output );
}

TEST_CASE("Data_bus_test hello_world_z80_test", "z80_hello_world")
{
    emu_context context;
    context.m_halted = false;
    uint16_t instruction_count = 0x100;
    Instruction_ROM instructions(instruction_count);
    instructions.add_symbol_at_current_fill("main")
                .add_symbolic_value("stdout_addr", 0x8000)
                .add_symbolic_value("hello_world_string", 0xA000)
                .add_instruction_with_symbol("ld hl,**", "hello_world_string")
                .add_instruction_with_symbol("call **", "print_hl")
                .add_instruction("halt")
                .add_instruction("halt")
                .add_instruction("halt")
                .add_symbol_at_current_fill("print_hl")
                .add_instruction_with_symbol("ld de,**", "stdout_addr")
                .add_symbol_at_current_fill("_print_hl_loop")
                .add_instruction("ld a,(hl)")
                .add_instruction("sub *", 0)
                .add_instruction_with_symbol("jp nz,**", "_print_hl_next")
                .add_instruction("ret")
                .add_symbol_at_current_fill("_print_hl_next")
                .add_instruction("ld (de),a")
                .add_instruction("inc hl")
                .add_instruction_with_symbol("jp **", "_print_hl_loop");

    REQUIRE( 0 == instructions.get_failed_instructions_list().size() );
    
    REQUIRE(s_c_error_none == context.m_data_bus.add_section(0, instructions));
    
    const char hello_world_string [] = "Hello World!";
    
    Data_bus_RAM hello_world_section(sizeof(hello_world_string));
    for(uint16_t i = 0; i < sizeof(hello_world_string); i++)
    {
        hello_world_section.set_data(i, hello_world_string[i]);
    }
    
    REQUIRE(s_c_error_none == context.m_data_bus.add_section(0xA000, hello_world_section));
    
    Data_bus_RAM stack_ram(0x100);
    
    REQUIRE(s_c_error_none == context.m_data_bus.add_section(0xFEFF, stack_ram));
    
    std::stringstream sstr;
    Data_bus_function stdout_fd(1, &sstr, &read_stdout_string, &write_stdout_string);
    REQUIRE(s_c_error_none == context.m_data_bus.add_section(0x8000, stdout_fd));
    
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
    
    uint32_t cycles = 0;
    while(!context.m_halted && cycles < 0xFFFF)
    {
        ++cycles;
        z80_run(&z80,1);
    }
    
    REQUIRE( context.m_halted );
    REQUIRE( sstr.str().compare("Hello World!") == 0 );
}

TEST_CASE("Data_bus_test write", "read and write" )
{
    uint16_t address_iter = 0x3200;
    uint8_t data_value = 0x32;
    Data_bus c;
    Data_bus_integer<uint32_t> x;
    REQUIRE(s_c_error_none == c.add_section(address_iter, x));
    c.write(address_iter,data_value);
    REQUIRE(c.read(address_iter) == x.get_data(0));
}
