/// @file phobos2390/z80_emulator/Data_bus.cpp

#include <z80_emulator/Data_bus.h>
#include <vector>
#include <algorithm>
#include <utility>

namespace z80_emulator
{

struct Data_bus::Impl
{
public:
    typedef std::pair<uint16_t, I_data_bus_section&> section_pair;
    typedef std::vector<section_pair>::iterator section_pair_iterator;
    
    std::vector<section_pair> m_section_list;
    
    Impl()
    : m_section_list()
    {
    }
    
    virtual ~Impl()
    {
    }
    
    Error detect_overlap(uint16_t origin, I_data_bus_section& data_bus_section)
    {
        Error ret_error = s_c_error_none;
        for(section_pair_iterator iter = m_section_list.begin(); 
            iter != m_section_list.end();
            ++iter)
        {
            section_pair& bus_sect = *iter;
            uint16_t new_section_end = origin 
                                     + data_bus_section.get_section_size();
            uint16_t old_section_end = bus_sect.first
                                     + bus_sect.second.get_section_size();
            if( ((bus_sect.first <= origin) && (origin < old_section_end))
             || ((bus_sect.first <= new_section_end) && (new_section_end <= old_section_end))
             || ((origin <= bus_sect.first) && ( bus_sect.first < new_section_end))
                    )
            {
                ret_error = s_c_error_overlap;
            }
        }
        return ret_error;
    }
    
    Error add_section(uint16_t origin, I_data_bus_section& data_bus_section)
    {
        Error section_error = detect_overlap(origin, data_bus_section);
        if(section_error == s_c_error_none)
        {
//            section_pair created_pair = std::make_pair
//                <uint16_t, I_data_bus_section&>(origin, data_bus_section);
            m_section_list.emplace_back(origin, data_bus_section);
//                (std::make_pair<uint16_t, I_data_bus_section&>
//                    (origin, data_bus_section));
        }
        return section_error;
    }
    
    section_pair* get_section_at_address(uint16_t address)
    {
        section_pair* p_ret_pair = 0;
        for(section_pair_iterator iter = m_section_list.begin(); 
            iter != m_section_list.end(); 
            ++iter)
        {
            section_pair& bus_sect = *iter;
            if( (bus_sect.first <= address) 
             && (address < ( bus_sect.first 
                           + bus_sect.second.get_section_size())))
            {
                p_ret_pair = &bus_sect;
            }
        }
        return p_ret_pair;
    }
    
    uint8_t read(uint16_t address)
    {
        uint8_t value = 0;
        section_pair* p_bus_section 
            = get_section_at_address(address);
        if(p_bus_section != 0)
        {
            uint16_t section_address = address - p_bus_section->first;
            value = p_bus_section->second.get_data(section_address);
        } 
        return value;
    }

    void write(uint16_t address, uint8_t value)
    {
        section_pair* p_bus_section 
            = get_section_at_address(address);
        if(p_bus_section != 0)
        {
            uint16_t section_address = address - p_bus_section->first;
            p_bus_section->second.set_data(section_address, value);
        } 
    }
};

Data_bus::Data_bus()
    :m_p_impl(new Impl)
{
}

Data_bus::~Data_bus()
{
    delete m_p_impl;
    m_p_impl = 0;
}

Error Data_bus::add_section(uint16_t origin, I_data_bus_section& data_bus_section)
{
    return m_p_impl->add_section(origin, data_bus_section);
}
    
uint8_t Data_bus::read(uint16_t address)
{
    return m_p_impl->read(address);
}

void Data_bus::write(uint16_t address, uint8_t value)
{
    m_p_impl->write(address, value);
}

}
