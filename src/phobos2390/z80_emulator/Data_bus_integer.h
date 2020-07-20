/// @file phobos2390/z80_emulator/Data_bus_integer.h
#ifndef DATA_BUS_INTEGER_H
#define DATA_BUS_INTEGER_H

#include <z80_emulator/I_data_bus_section.h>

namespace z80_emulator
{

template<typename INT_TYPE>
class Data_bus_integer : public I_data_bus_section
{
public:
    Data_bus_integer();

    Data_bus_integer(const Data_bus_integer& orig);

    virtual ~Data_bus_integer();
    
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
    
    INT_TYPE get_integer();

    void set_integer(INT_TYPE value);
    
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#include <z80_emulator/template_source/Data_bus_integer_private.h>

#endif /* DATA_BUS_INTEGER_H */

