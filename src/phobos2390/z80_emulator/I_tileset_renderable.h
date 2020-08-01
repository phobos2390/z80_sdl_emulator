/// @file src/phobos2390/z80_emulator/I_tileset_renderable.h

#ifndef Z80_EMULATOR_I_TILESET_RENDERABLE_H
#define Z80_EMULATOR_I_TILESET_RENDERABLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace z80_emulator
{
static const size_t s_c_pallete_size = 0x10;    
    
struct Tileset_metadata    
{
    uint16_t m_tile_width_pixels;
    uint16_t m_tile_height_pixels;
    uint16_t m_tile_color_depth;
    uint32_t m_pallete[s_c_pallete_size];
};
    
class I_tileset_renderable
{
public:
    /// Creates class value
    I_tileset_renderable(){}

    /// Removes class value
    virtual ~I_tileset_renderable(){}
    
    /// Renders onto a texture via a renderer graphical data
    /// @param[in] p_sdl_tileset_texture
    /// @param[in] p_sdl_renderer
    /// @param[in] tile_data
    virtual void render( SDL_Texture *p_sdl_tileset_texture
                       , SDL_Renderer *p_sdl_renderer
                       , Tileset_metadata& tile_data) =0;
};

}

#endif /* Z80_EMULATOR_I_TILESET_RENDERABLE_H */
