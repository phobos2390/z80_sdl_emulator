/// @file phobos2390/z80_emulator/Data_bus.h

#ifndef Z80_EMULATOR_DATA_BUS_H
#define Z80_EMULATOR_DATA_BUS_H

#include <stdint.h>
#include <z80_emulator/emulator_typedefs.h>
#include <z80_emulator/emulator_errors.h>
#include <z80_emulator/I_data_bus_section.h>

namespace z80_emulator
{

class Data_bus
{
public:
    /// Creates class value
    Data_bus();

    /// Removes class value
    virtual ~Data_bus();
    
    Error add_section(uint16_t origin, I_data_bus_section& data_bus_section);
    
    uint8_t read(uint16_t address);
    
    void write(uint16_t address, uint8_t value);
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_DATA_BUS_H */
