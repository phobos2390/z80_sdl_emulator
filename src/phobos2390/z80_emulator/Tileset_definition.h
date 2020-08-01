/// @file phobos2390/z80_emulator/Tileset_definition.h

#ifndef Z80_EMULATOR_TILESET_DEFINITION_H
#define Z80_EMULATOR_TILESET_DEFINITION_H

#include <z80_emulator/I_tileset_renderable.h>
#include <z80_emulator/I_data_bus_section.h>

namespace z80_emulator
{

class Data_bus;

class Tileset_definition:public I_data_bus_section
{
public:
    /// Creates class value
    Tileset_definition(Tileset_metadata& metadata);

    /// Removes class value
    virtual ~Tileset_definition();
    
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
    
    SDL_Surface* get_tileset_surface();
    
    void write_tileset_to_file(const char* p_filename);
    
    void set_full_tileset();
    
    void set_from_databus(Data_bus& data_bus, uint16_t start, uint16_t end);
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_TILESET_DEFINITION_H */
