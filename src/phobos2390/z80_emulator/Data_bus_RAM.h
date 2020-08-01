/// @file phobos2390/z80_emulator/Data_bus_RAM.h

#ifndef Z80_EMULATOR_DATA_BUS_RAM_H
#define Z80_EMULATOR_DATA_BUS_RAM_H

#include <z80_emulator/I_data_bus_section.h>

namespace z80_emulator
{

class Data_bus_RAM: public I_data_bus_section
{
public:
    /// Creates class value
    /// @param[in] size of RAM section
    Data_bus_RAM(uint16_t size);

    /// Removes class value
    virtual ~Data_bus_RAM();
    
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

#endif /* Z80_EMULATOR_DATA_BUS_RAM_H */
