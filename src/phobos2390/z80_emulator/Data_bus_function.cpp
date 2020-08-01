/// @file z80_emulator/Data_bus_function.cpp

#include <z80_emulator/Data_bus_function.h>

namespace z80_emulator
{

struct Data_bus_function::Impl
{
public:
    uint16_t m_section_size;
    void* m_p_context;
    f_read m_read_handle;
    f_write m_write_handle;
    
    Impl( uint16_t section_size
        , void* p_context
        , f_read read_handle
        , f_write write_handle)
    : m_section_size(section_size)
    , m_p_context(p_context)
    , m_read_handle(read_handle)
    , m_write_handle(write_handle)
    {
    }
    
    virtual ~Impl(){}
};

Data_bus_function::Data_bus_function( uint16_t section_size
                                    , void* p_context
                                    , f_read read_handle
                                    , f_write write_handle)
    :m_p_impl(new Impl(section_size, p_context, read_handle, write_handle))
{
}

Data_bus_function::~Data_bus_function()
{
    delete m_p_impl;
    m_p_impl = 0;
}

uint16_t Data_bus_function::get_section_size() const
{
    return m_p_impl->m_section_size;
}

uint8_t Data_bus_function::get_data(uint16_t address)
{
    return (*m_p_impl->m_read_handle)(m_p_impl->m_p_context, address);
}

void Data_bus_function::set_data(uint16_t address, uint8_t value)
{
    (*m_p_impl->m_write_handle)(m_p_impl->m_p_context, address, value);
}

}
