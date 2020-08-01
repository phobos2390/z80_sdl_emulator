/// @file phobos2390/z80_emulator/Sprite_table.cpp

#include <z80_emulator/Sprite_table.h>
#include <string.h>

namespace z80_emulator
{

struct Sprite_table::Impl
{
public:
    uint16_t m_number_of_sprites;
    struct sprite* m_p_sprite_table;
    
    Impl(uint16_t sprite_count)
    : m_number_of_sprites(sprite_count)
    , m_p_sprite_table(new sprite[sprite_count])
    {
        memset(m_p_sprite_table, 0, sprite_count * sizeof(sprite));
    }
    
    virtual ~Impl()
    {
        delete [] m_p_sprite_table;
    }
    
    uint8_t* get_sprite_attribute_array(uint16_t address)
    {
        uint16_t sprite_number = address / sizeof(sprite);
        return reinterpret_cast<uint8_t*>(&m_p_sprite_table[sprite_number]);
    }
    
    uint8_t get_data(uint16_t address)
    {
        uint16_t sprite_attribute = address % sizeof(sprite);
        uint8_t* sprite_attr_array = get_sprite_attribute_array(address);
        return sprite_attr_array[sprite_attribute];
    }
    
    void set_data(uint16_t address, uint8_t data)
    {
        uint16_t sprite_attribute = address % sizeof(sprite);
        uint8_t* sprite_attr_array = get_sprite_attribute_array(address);
        sprite_attr_array[sprite_attribute] = data;
    }
};

Sprite_table::Sprite_table(uint16_t sprite_count)
    :m_p_impl(new Impl(sprite_count))
{
}

Sprite_table::~Sprite_table()
{
    delete m_p_impl;
    m_p_impl = 0;
}

uint16_t Sprite_table::get_section_size() const
{
    return m_p_impl->m_number_of_sprites * sizeof(sprite);
}

uint8_t Sprite_table::get_data(uint16_t address)
{
    return m_p_impl->get_data(address);
}

void Sprite_table::set_data(uint16_t address, uint8_t value)
{
    return m_p_impl->set_data(address,value);
}

uint8_t Sprite_table::get_sprite_attribute(uint16_t sprite_num, uint16_t attribute_num)
{
    uint8_t* p_arr 
        = reinterpret_cast<uint8_t*>(&m_p_impl->m_p_sprite_table[sprite_num]);
    return p_arr[attribute_num];
}
    
void Sprite_table::set_sprite_attribute(uint16_t sprite_num, uint16_t attribute_num, uint8_t value)
{
    uint8_t* p_arr 
        = reinterpret_cast<uint8_t*>(&m_p_impl->m_p_sprite_table[sprite_num]);
    p_arr[attribute_num] = value;
}

uint8_t Sprite_table::get_x(uint16_t sprite_num)
{
    return get_sprite_attribute(sprite_num, 1);
}

uint8_t Sprite_table::get_y(uint16_t sprite_num)
{
    return get_sprite_attribute(sprite_num, 0);
}

uint8_t Sprite_table::get_tile_type(uint16_t sprite_num)
{
    return get_sprite_attribute(sprite_num, 2);
}

uint8_t Sprite_table::get_flags(uint16_t sprite_num)
{
    return get_sprite_attribute(sprite_num, 3);
}

void Sprite_table::set_x(uint16_t sprite_num, uint8_t value)
{
    set_sprite_attribute(sprite_num, 1, value);
}

void Sprite_table::set_y(uint16_t sprite_num, uint8_t value)
{
    set_sprite_attribute(sprite_num, 0, value);
}

void Sprite_table::set_tile_type(uint16_t sprite_num, uint8_t value)
{
    set_sprite_attribute(sprite_num, 2, value);
}

void Sprite_table::set_flags(uint16_t sprite_num, uint8_t value)
{
    set_sprite_attribute(sprite_num, 3, value);
}

void Sprite_table::render(SDL_Texture *p_sdl_tileset_texture, SDL_Renderer *p_sdl_renderer, Tileset_metadata& tile_data)
{
    for(size_t i = 0; i < m_p_impl->m_number_of_sprites; i++)
    {
        if((m_p_impl->m_p_sprite_table[i].m_flags & z80_emulator::s_c_sprite_visible) != 0)
        {
            SDL_Rect src_rect;
            src_rect.w = tile_data.m_tile_width_pixels;
            src_rect.h = tile_data.m_tile_height_pixels;
            src_rect.x = m_p_impl->m_p_sprite_table[i].m_character * tile_data.m_tile_width_pixels;
            src_rect.y = 0;
            
            SDL_Rect dst_rect;
            dst_rect.w = tile_data.m_tile_width_pixels;
            dst_rect.h = tile_data.m_tile_height_pixels;
            dst_rect.x = m_p_impl->m_p_sprite_table[i].m_x;
            dst_rect.y = m_p_impl->m_p_sprite_table[i].m_y;
            
            uint32_t flip = SDL_FLIP_NONE;
            
            if(m_p_impl->m_p_sprite_table[i].m_flags & s_c_sprite_flipped_horizontal)
            {
                flip |= SDL_FLIP_HORIZONTAL;
            }
            
            if(m_p_impl->m_p_sprite_table[i].m_flags & s_c_sprite_flipped_vertical)
            {
                flip |= SDL_FLIP_VERTICAL;
            }
            SDL_RendererFlip sdl_flip = static_cast<SDL_RendererFlip>(flip);

            SDL_RenderCopyEx( p_sdl_renderer
                            , p_sdl_tileset_texture
                            ,&src_rect
                            ,&dst_rect
                            , 0
                            , NULL
                            , sdl_flip);
        }
    }
}

}
