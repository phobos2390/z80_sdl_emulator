/// @file z80_emulator/Grid_Display.cpp

#include <z80_emulator/Grid_display.h>
#include <z80_emulator/Data_bus_RAM.h>
#include <z80_emulator/Data_bus_integer.h>
#include <z80_emulator/Data_bus_function.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <z80_emulator/emulator_errors.h>

namespace z80_emulator
{

struct Grid_display::Impl
{
public:
    uint16_t m_width;
    uint16_t m_height;
    Tileset_metadata m_metadata;
    Data_bus_RAM m_vram;
    Data_bus_integer<uint16_t> m_grid_iterator;
    Data_bus_function m_display_descriptor;
    
    Impl(Tileset_metadata& metadata, uint16_t width, uint16_t height)
    : m_width(width)
    , m_height(height)
    , m_metadata()
    , m_vram(m_width * m_height)
    , m_grid_iterator()
    , m_display_descriptor(1, this, static_read_function, static_write_function)
    {
        m_metadata.m_tile_width_pixels = metadata.m_tile_width_pixels;
        m_metadata.m_tile_height_pixels = metadata.m_tile_height_pixels;
        m_metadata.m_tile_color_depth = metadata.m_tile_color_depth;
        memcpy(m_metadata.m_pallete, metadata.m_pallete, s_c_pallete_size * sizeof(uint32_t));
    }
    
    virtual ~Impl()
    {
    }
    
    void write_function(uint16_t /* address */, uint8_t value)
    {
        uint16_t address = m_grid_iterator.get_integer();
        m_vram.set_data(address, value);
        m_grid_iterator.set_integer((address + 1) % m_vram.get_section_size());
    }

    uint8_t read_function(uint16_t /* address */)
    {
        return 0;
    }
    
    static void static_write_function(void* p_context, uint16_t address, uint8_t value)
    {
        Impl* p_impl = static_cast<Impl*>(p_context);
        if(p_impl)
        {
            p_impl->write_function(address,value);
        }
    }

    static uint8_t static_read_function(void* p_context, uint16_t address)
    {
        Impl* p_impl = static_cast<Impl*>(p_context);
        uint8_t value = 0;
        if(p_impl)
        {
            value = p_impl->read_function(address);
        }
        return value;
    }
    
};

Grid_display::Grid_display(Tileset_metadata& metadata, uint16_t width, uint16_t height)
    :m_p_impl(new Impl(metadata, width, height))
{
}

Grid_display::~Grid_display()
{
    delete m_p_impl;
    m_p_impl = 0;
}

uint16_t Grid_display::get_section_size() const
{
    return m_p_impl->m_width * m_p_impl->m_height;
}

uint8_t Grid_display::get_data(uint16_t address)
{
    return m_p_impl->m_vram.get_data(address);
}
    
void Grid_display::set_data(uint16_t address, uint8_t value)
{
    m_p_impl->m_vram.set_data(address,value);
}
    
void Grid_display::render( SDL_Texture *p_sdl_tileset_texture
                         , SDL_Renderer *p_sdl_renderer
                         , Tileset_metadata& tile_data)
{
    for(size_t i = 0; i < m_p_impl->m_height; i++)
    {
        for(size_t j = 0; j < m_p_impl->m_width; j++)
        {
            SDL_Rect dest_rect;
            dest_rect.w = m_p_impl->m_metadata.m_tile_width_pixels;
            dest_rect.h = m_p_impl->m_metadata.m_tile_height_pixels;
            dest_rect.x = j * m_p_impl->m_metadata.m_tile_width_pixels;
            dest_rect.y = i * m_p_impl->m_metadata.m_tile_height_pixels;
            uint8_t ucharacter = m_p_impl->m_vram.get_data(i * m_p_impl->m_width + j);
            SDL_Rect src_rect;
            src_rect.w = m_p_impl->m_metadata.m_tile_width_pixels;
            src_rect.h = m_p_impl->m_metadata.m_tile_height_pixels;
            src_rect.x = ucharacter * m_p_impl->m_metadata.m_tile_width_pixels;
            src_rect.y = 0;
            SDL_RenderCopy( p_sdl_renderer
                          , p_sdl_tileset_texture
                          ,&src_rect
                          ,&dest_rect);
        }
    }
}

Error Grid_display::add_to_data_bus( Data_bus& db
                                   , uint16_t iter_address
                                   , uint16_t file_descriptor_address)
{
    Error return_error;
    return_error = db.add_section(iter_address, m_p_impl->m_grid_iterator);
    Error descriptor_error = db.add_section( file_descriptor_address
                                           , m_p_impl->m_display_descriptor);
    if(return_error == s_c_error_none)
    {
        return_error = descriptor_error;
    }
    return return_error;
}

}
