/// @file z80_emulator//Instruction_ROM.h

#ifndef Z80_EMULATOR_INSTRUCTION_ROM_H
#define Z80_EMULATOR_INSTRUCTION_ROM_H

#include <z80_emulator/I_data_bus_section.h>
#include <z80_emulator/emulator_errors.h>
#include <vector>
#include <string>

namespace z80_emulator
{

class instruction
{
public:
    instruction(const char* label);
    instruction(const char* label, uint8_t direct);
    instruction(const char* label, uint8_t high, uint8_t low);

    virtual ~instruction();
    
    void append_bytes(std::vector<uint8_t>& bytes);
    
    bool is_valid();
    
    std::string to_string();
private:
    uint32_t get_opcode(const char* p_instruction_name);
    uint16_t get_indirect_count(const char* label);
    bool is_valid_instruction(const char* label);
    bool is_valid_instruction(const char* label, uint8_t direct);
    bool is_valid_instruction(const char* label, uint8_t high, uint8_t low);
    
    uint32_t m_instruction_data;
    bool m_valid;
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
    
    std::string address_to_string(uint16_t address);
    
    Instruction_ROM& add_instruction(const char* label);

    Instruction_ROM& add_instruction(const char* label, uint8_t direct);

    Instruction_ROM& add_instruction(const char* label, uint8_t high, uint8_t low);

    Instruction_ROM& add_instruction_with_symbol(const char* label, const char* identifier);

    Instruction_ROM& add_symbolic_value(const char* identifier, uint16_t value);

    Instruction_ROM& add_symbol_at_current_fill(const char* identifier);
    
    std::vector<std::string>& get_failed_instructions_list();
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_INSTRUCTION_ROM_H */
