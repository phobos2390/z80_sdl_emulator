/// @file z80_emulator/Grid_Display.h

#ifndef Z80_EMULATOR_GRID_DISPLAY_H
#define Z80_EMULATOR_GRID_DISPLAY_H

#include <z80_emulator/I_data_bus_section.h>
#include <z80_emulator/I_tileset_renderable.h>
#include <z80_emulator/Data_bus.h>

namespace z80_emulator
{

class Grid_Display:public I_data_bus_section, public I_tileset_renderable
{
public:
    /// Creates class value
    /// @param[in] metadata - tile dimensions
    /// @param[in] width - left right dimensions of the grid
    /// @param[in] height - up down dimensions of the grid
    Grid_Display(Tileset_metadata& metadata, uint16_t width, uint16_t height);

    /// Removes class value
    virtual ~Grid_Display();
    
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
    
    /// Renders onto a texture via a renderer graphical data
    /// @param[in] p_sdl_tileset_texture
    /// @param[in] p_sdl_renderer
    /// @param[in] tile_data
    virtual void render( SDL_Texture *p_sdl_tileset_texture
                       , SDL_Renderer *p_sdl_renderer
                       , Tileset_metadata& tile_data);
    
    Error add_to_data_bus( Data_bus& db
                         , uint16_t iter_address
                         , uint16_t file_descriptor_address);
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_GRID_DISPLAY_H */
