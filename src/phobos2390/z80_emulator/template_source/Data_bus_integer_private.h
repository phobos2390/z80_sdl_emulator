/// @file phobos2390/z80_emulator/template_source/Data_bus_integer_private.h
#ifndef Z80_EMULATOR_TEMPLATE_SOURCE_DATA_BUS_INTEGER_PRIVATE_H
#define Z80_EMULATOR_TEMPLATE_SOURCE_DATA_BUS_INTEGER_PRIVATE_H

#include <z80_emulator/Data_bus_integer.h>
#include <stddef.h>

namespace z80_emulator
{

template<typename INT_TYPE>
struct Data_bus_integer<INT_TYPE>::Impl
{
    uint8_t m_value [sizeof(INT_TYPE)];

    Impl()
    :m_value()
    {
        
    }

    Impl(const Impl& orig)
    :m_value(orig.m_value)
    {
    }

    ~Impl()
    {
    }
    
    INT_TYPE get_int_value()
    {
        INT_TYPE ret_value = 0;
        size_t single_shift_value = 0x8;
        for(int i = 0; i < sizeof(m_value); i++)
        {
            int r = sizeof(m_value) - 1 - i;
            ret_value = ret_value << single_shift_value;
            ret_value |= m_value[r];
        }
        return ret_value;
    }

    void set_int_value(INT_TYPE value)
    {
        size_t single_shift_value = 0x8;
        INT_TYPE mask_value = 0xFF;
        mask_value = (mask_value << (single_shift_value * (sizeof(m_value) - 1)));
        for(int i = 0; i < sizeof(m_value); i++)
        {
            int r = sizeof(m_value) - 1 - i;
            m_value[r] = (mask_value & value) >> (r * single_shift_value);
            mask_value = mask_value >> single_shift_value;
        }
    }
};

template<typename INT_TYPE>
Data_bus_integer<INT_TYPE>::Data_bus_integer() 
:m_p_impl(new Impl())
{
}

template<typename INT_TYPE>
Data_bus_integer<INT_TYPE>::Data_bus_integer(const Data_bus_integer& orig) 
:m_p_impl(new Impl(*orig.m_p_impl))
{
}

template<typename INT_TYPE>
Data_bus_integer<INT_TYPE>::~Data_bus_integer() 
{
    delete m_p_impl;
    m_p_impl = 0;
}

template<typename INT_TYPE>
uint16_t Data_bus_integer<INT_TYPE>::get_section_size() const
{
    return sizeof(INT_TYPE);
}

template<typename INT_TYPE>
uint8_t Data_bus_integer<INT_TYPE>::get_data(uint16_t address)
{
    return m_p_impl->m_value[address];
}

template<typename INT_TYPE>
void Data_bus_integer<INT_TYPE>::set_data(uint16_t address, uint8_t value)
{
    m_p_impl->m_value[address] = value;
}

template<typename INT_TYPE>
INT_TYPE Data_bus_integer<INT_TYPE>::get_integer()
{
    return m_p_impl->get_int_value();
}

template<typename INT_TYPE>
void Data_bus_integer<INT_TYPE>::set_integer(INT_TYPE value)
{
    m_p_impl->set_int_value(value);
}

}

#endif /* Z80_EMULATOR_TEMPLATE_SOURCE_DATA_BUS_INTEGER_PRIVATE_H */
