/// @file phobos2390/z80_emulator/Sprite_table.h

#ifndef Z80_EMULATOR_SPRITE_TABLE_H
#define Z80_EMULATOR_SPRITE_TABLE_H

#include <z80_emulator/I_data_bus_section.h>
#include <z80_emulator/I_tileset_renderable.h>

namespace z80_emulator
{
    
struct sprite
{
    uint8_t m_y;
    uint8_t m_x;
    uint8_t m_character;
    uint8_t m_flags;
}__attribute__((packed));

static const uint8_t s_c_sprite_visible = 0x80;
static const uint8_t s_c_sprite_flipped_vertical = 0x40;
static const uint8_t s_c_sprite_flipped_horizontal = 0x20;
    
class Sprite_table:public I_data_bus_section, public I_tileset_renderable
{
public:
    /// Creates class value
    /// @param[in] sprite_count number of sprites
    Sprite_table(uint16_t sprite_count);

    /// Removes class value
    virtual ~Sprite_table();
    
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
    
    /// Gets the attribute for a given sprite:
    /// @note attribute_nums are as follows:
    ///    0 - y;
    ///    1 - x;
    ///    2 - tile type;
    ///    3 - flags;
    /// @param[in] sprite_num
    /// @param[in] attribute_num
    uint8_t get_sprite_attribute(uint16_t sprite_num, uint16_t attribute_num);
    
    /// Gets the attribute for a given sprite:
    /// @note attribute_nums are as follows:
    ///    0 - y;
    ///    1 - x;
    ///    2 - tile type;
    ///    3 - flags;
    /// @param[in] sprite_num
    /// @param[in] attribute_num
    void set_sprite_attribute(uint16_t sprite_num, uint16_t attribute_num, uint8_t value);
    
    uint8_t get_x(uint16_t sprite_num);

    uint8_t get_y(uint16_t sprite_num);
    
    uint8_t get_tile_type(uint16_t sprite_num);

    uint8_t get_flags(uint16_t sprite_num);
    
    void set_x(uint16_t sprite_num, uint8_t value);
    
    void set_y(uint16_t sprite_num, uint8_t value);
    
    void set_tile_type(uint16_t sprite_num, uint8_t value);

    void set_flags(uint16_t sprite_num, uint8_t value);
    
    void render(SDL_Texture *p_sdl_tileset_texture
               ,SDL_Renderer *p_sdl_renderer
               ,Tileset_metadata& tile_data);
    
private:
    struct Impl;
    Impl* m_p_impl;
};

}

#endif /* Z80_EMULATOR_SPRITE_TABLE_H */
