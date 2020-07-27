/// @file z80_emulator//Instruction_ROM.h

#ifndef Z80_EMULATOR_INSTRUCTION_ROM_H
#define Z80_EMULATOR_INSTRUCTION_ROM_H

#include <z80_emulator/I_data_bus_section.h>
#include <z80_emulator/emulator_errors.h>
#include <vector>

namespace z80_emulator
{

class instruction
{
public:
    instruction(){}
    virtual ~instruction(){}
    
    virtual void append_bytes(std::vector<uint8_t>& bytes)=0;
};

enum register_id
{
    register_b,
    register_c,
    register_d,
    register_e,
    register_h,
    register_l,
    register_hl_ind,
    register_a,

    register_bc_direct,
    register_de_direct,
    register_hl_direct,
    register_sp_direct,
    
    register_bc_ind,
    register_de_ind,
    register_f,
};

class ld: public instruction
{
public:
    ld(register_id dst, register_id src);
    ld(register_id dst, uint16_t immediate);
    ld(uint16_t immediate, register_id src);
    virtual ~ld();
    
    virtual void append_bytes(std::vector<uint8_t>& bytes);
private:
    uint8_t m_opcode;
    uint16_t m_immediate;
};

class Instruction_ROM:public I_data_bus_section
{
public:
    /// Creates class value
    Instruction_ROM(uint16_t instruction_size);

    /// Removes class value
    virtual ~Instruction_ROM();
    
    /// Gets the size of the section
    virtual uint16_t get_section_size() const;

    /// Gets the value at the address
    /// @param[in] address offset from root portion of the section
    /// @return value what to set data at the address to
    virtual uint8_t get_data(uint16_t address);
    
    /// Sets the value at the address
    /// @param[in] address offset from root portion of the section
    /// @param[in] value what to set data at the address to
    virtual void set_data(uint16_t address, uint8_t value);
    
    Error read_binary(const char* filename);
    
    void parse_instruction_list(std::vector<uint16_t>& instruction_list);
    
    void add_instruction(instruction& instruction);
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_INSTRUCTION_ROM_H */
