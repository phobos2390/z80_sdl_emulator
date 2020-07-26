/// @file phobos2390/z80_emulator/Data_bus_RAM.cpp

#include <phobos2390/z80_emulator/Data_bus_RAM.h>
#include <string.h>

namespace z80_emulator
{

struct Data_bus_RAM::Impl
{
public:
    uint16_t m_size;
    uint8_t* m_p_ram_values;
    Impl(uint16_t size)
    : m_size(size)
    , m_p_ram_values(new uint8_t[m_size])
    {
        memset(m_p_ram_values, 0, m_size);
    }
    virtual ~Impl()
    {
        delete [] m_p_ram_values;
        m_p_ram_values = 0;
    }
};

Data_bus_RAM::Data_bus_RAM(uint16_t size)
    :m_p_impl(new Impl(size))
{
}

Data_bus_RAM::~Data_bus_RAM()
{
    delete m_p_impl;
    m_p_impl = 0;
}

uint16_t Data_bus_RAM::get_section_size() const
{
    return m_p_impl->m_size;
}

uint8_t Data_bus_RAM::get_data(uint16_t address)
{
    return m_p_impl->m_p_ram_values[address];    
}

void Data_bus_RAM::set_data(uint16_t address, uint8_t value)
{
    m_p_impl->m_p_ram_values[address] = value;
}

}
