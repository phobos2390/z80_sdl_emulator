/// @file z80_emulator/Data_bus_function.h

#ifndef Z80_EMULATOR_DATA_BUS_FUNCTION_H
#define Z80_EMULATOR_DATA_BUS_FUNCTION_H

#include <z80_emulator/I_data_bus_section.h>

namespace z80_emulator
{

class Data_bus_function: public I_data_bus_section
{
public:
    typedef uint8_t (*f_read) (void* context, uint16_t address);
    typedef void (*f_write) (void* context, uint16_t address, uint8_t value);
    
    /// Creates class value
    Data_bus_function(uint16_t section_size, void* p_context, f_read read_handle, f_write write_handle);

    /// Removes class value
    virtual ~Data_bus_function();
    
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
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_DATA_BUS_FUNCTION_H */
