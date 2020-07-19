///@file phobos2390/z80_emulator/I_data_bus_section.h

#ifndef Z80_EMULATOR_I_DATA_BUS_SECTION_H
#define Z80_EMULATOR_I_DATA_BUS_SECTION_H

#include <stdint.h>

namespace z80_emulator
{

class I_data_bus_section
{
public:
    /// Creates class value
    I_data_bus_section(){}
    
    /// Deletes class value
    virtual ~I_data_bus_section(){}
    
    /// Gets the size of the section
    virtual uint16_t get_section_size() const =0;

    /// Gets the value at the address
    /// @param[in] address offset from root portion of the section
    /// @return value what to set data at the address to
    virtual uint8_t get_data(uint16_t address)=0;
    
    /// Sets the value at the address
    /// @param[in] address offset from root portion of the section
    /// @param[in] value what to set data at the address to
    virtual void set_data(uint16_t address, uint8_t value)=0;
};

}

#endif /* Z80_EMULATOR_I_DATA_BUS_SECTION_H */

