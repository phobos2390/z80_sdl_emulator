/// @file phobos2390/z80_emulator/Tileset_definition.cpp

#include <z80_emulator/Tileset_definition.h>
#include <z80_emulator/Data_bus.h>

namespace z80_emulator
{

struct Tileset_definition::Impl
{
public:
    static const uint16_t s_c_bits_in_byte = 0x8;
    static const uint16_t s_c_tile_dimension = 0x10;
    static const uint16_t s_c_total_characters = 0x100;
    static const uint32_t s_c_sdl_color_depth = 0x20;
    

    Tileset_metadata m_metadata;
    uint16_t m_tile_width_bytes;
    uint16_t m_tile_size_bytes;
    uint16_t m_tileset_total_size;
    uint8_t* m_p_character_set;
    SDL_Surface* m_p_sdl_tileset_picture;
    SDL_Surface* m_p_sdl_square_tileset_picture;
    
    Impl(Tileset_metadata& metadata)
    : m_metadata()
    , m_tile_width_bytes((metadata.m_tile_width_pixels / s_c_bits_in_byte) * metadata.m_tile_color_depth)
    , m_tile_size_bytes(m_tile_width_bytes * metadata.m_tile_height_pixels)
    , m_tileset_total_size( s_c_total_characters
                          * m_tile_size_bytes)
    , m_p_character_set(new uint8_t[m_tileset_total_size])
    , m_p_sdl_tileset_picture(SDL_CreateRGBSurface( 0
                                                  , ( s_c_total_characters
                                                    * metadata.m_tile_width_pixels)
                                                  , metadata.m_tile_height_pixels
                                                  , s_c_sdl_color_depth
                                                  , 0x00FF0000
                                                  , 0x0000FF00
                                                  , 0x000000FF
                                                  , 0xFF000000))
    , m_p_sdl_square_tileset_picture(SDL_CreateRGBSurface( 0
                                                         , ( s_c_tile_dimension
                                                           * metadata.m_tile_width_pixels)
                                                         , ( s_c_tile_dimension
                                                           * metadata.m_tile_height_pixels)
                                                         , s_c_sdl_color_depth
                                                         , 0x00FF0000
                                                         , 0x0000FF00
                                                         , 0x000000FF
                                                         , 0xFF000000))
    {
        m_metadata.m_tile_width_pixels = metadata.m_tile_width_pixels;
        m_metadata.m_tile_height_pixels = metadata.m_tile_height_pixels;
        m_metadata.m_tile_color_depth = metadata.m_tile_color_depth;
        memcpy(m_metadata.m_pallete, metadata.m_pallete, s_c_pallete_size * sizeof(uint32_t));
        memset(m_p_character_set, 0, m_tileset_total_size);
    }
    
    virtual ~Impl()
    {
        SDL_FreeSurface(m_p_sdl_square_tileset_picture);
        SDL_FreeSurface(m_p_sdl_tileset_picture);
        delete [] m_p_character_set;
    }
    
    uint8_t get_pixel_mask()
    {
        uint8_t mask = 0;
        switch(m_metadata.m_tile_color_depth)
        {
            case 0x1:
                mask = 0x1;
                break;
            case 0x2:
                mask = 0x3;
                break;
            case 0x4:
                mask = 0xF;
                break;
            case 0x8:
                mask = 0xFF;
                break;
            default:
                break;
        }
        return mask;
    }
    
    uint8_t get_pixel_byte_density()
    {
        uint8_t density = 0;
        switch(m_metadata.m_tile_color_depth)
        {
            case 0x1:
                density = 0x8;
                break;
            case 0x2:
                density = 0x4;
                break;
            case 0x4:
                density = 0x2;
                break;
            case 0x8:
                density = 0x1;
                break;
            default:
                break;
        }
        return density;
    }
    
    uint8_t get_pixel_column_offset()
    {
        uint8_t column_offset = 0;
        switch(m_metadata.m_tile_color_depth)
        {
            case 0x1:
                column_offset = 0x1;
                break;
            case 0x2:
                column_offset = 0x4;
                break;
            case 0x4:
                column_offset = 0x2;
                break;
            case 0x8:
                column_offset = 0x8;
                break;
            default:
                break;
        }
        return column_offset;
    }
    
    void set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, uint32_t pixel)
    {
      uint32_t *target_pixel = (reinterpret_cast<uint32_t*>
                                (static_cast<uint8_t*>( 
                                    ( static_cast<uint8_t*>(surface->pixels)
                                    + ( y * surface->pitch)
                                    + ( x * sizeof(*target_pixel))))));
      *target_pixel = pixel;
    }
    
    void set_pixel_byte( uint8_t pixel_byte_data
                       , uint32_t pix_column
                       , uint32_t pix_row
                       , uint32_t offset)
    {
        if(m_metadata.m_tile_color_depth == 0x8)
        {
            uint32_t bit_pixel_value [] = 
            {
                0x00, 0x55, 0xAA, 0xFF
            };

            uint32_t alpha = (bit_pixel_value[(0xC0 & pixel_byte_data) >> 6] << 24);
            uint32_t red =   (bit_pixel_value[(0x30 & pixel_byte_data) >> 4] << 16);
            uint32_t green = (bit_pixel_value[(0x0C & pixel_byte_data) >> 2] << 8);
            uint32_t blue =   bit_pixel_value[(0x03 & pixel_byte_data)];
            uint32_t pixel_value = alpha | red | green | blue;
            set_pixel(m_p_sdl_tileset_picture, pix_column + offset, pix_row, pixel_value);

            set_pixel( m_p_sdl_square_tileset_picture
                     , pix_column + (offset & 0xF)
                     , pix_row + ((offset & 0xF0) >> 4)
                     , pixel_value);
        }
        else if( (m_metadata.m_tile_color_depth <= 0x4) 
              && (m_metadata.m_tile_color_depth > 0))
        {
//            0b00000001
//            0b00000010
//            0b00000100
//            0b00001000
//            0b00010000
//            0b00100000
//            0b01000000
//            0b10000000
            
//            0b00000011
//            0b00001100
//            0b00110000
//            0b11000000
            
//            0x0F
//            0xF0            
            for(uint32_t i = 0; i < get_pixel_byte_density(); i++)
            {
                uint32_t r = (get_pixel_byte_density() - 1) - i;
                uint32_t pixel_color = 0;
                uint8_t shift_value = i * m_metadata.m_tile_color_depth;
                uint8_t pallete_index 
                    = ((pixel_byte_data & (get_pixel_mask() << shift_value)) 
                        >> shift_value);
                pixel_color = m_metadata.m_pallete[pallete_index];
                
                set_pixel( m_p_sdl_tileset_picture
                         , (pix_column * get_pixel_column_offset()) + r + offset
                         , pix_row
                         , pixel_color);
                
                uint32_t character = (offset / m_metadata.m_tile_width_pixels);
                uint32_t offset_x = (character & 0xF) * m_metadata.m_tile_width_pixels;
                uint32_t offset_y = ((character & 0xF0) >> 4) * m_metadata.m_tile_height_pixels;

                set_pixel( m_p_sdl_square_tileset_picture
                         , (pix_column * get_pixel_column_offset()) + r + offset_x
                         , pix_row + offset_y
                         , pixel_color);
            }
        }
        else
        {
            SDL_Log("Invalid color depth");
        }
    }
    
    void set_tileset_character(uint8_t character)
    {
        if(m_p_sdl_tileset_picture == NULL)
        {
            SDL_Log("Tileset surface not loaded");
        }
        else
        {
            size_t character_offset = character * m_metadata.m_tile_width_pixels;
            size_t character_start = character * m_tile_size_bytes;
            for(uint32_t i = 0; i < m_tile_size_bytes; i++)
            {
                uint32_t pix_column = i % m_tile_width_bytes;
                uint32_t pix_row = i / m_tile_width_bytes;
                uint8_t pixel_byte_data = m_p_character_set[character_start + i];

                set_pixel_byte(pixel_byte_data, pix_column, pix_row, character_offset);
            }
        }
    }
    
    void set_full_tileset()
    {
        for(uint16_t i = 0; i < s_c_total_characters; i++)
        {
            set_tileset_character(i);
        }
    }
};

Tileset_definition::Tileset_definition(Tileset_metadata& metadata)
    :m_p_impl(new Impl(metadata))
{
}

Tileset_definition::~Tileset_definition()
{
    delete m_p_impl;
    m_p_impl = 0;
}

uint16_t Tileset_definition::get_section_size() const
{
    return m_p_impl->m_tileset_total_size;
}

uint8_t Tileset_definition::get_data(uint16_t address)
{
    return m_p_impl->m_p_character_set[address];
}

void Tileset_definition::set_data(uint16_t address, uint8_t value)
{
    m_p_impl->m_p_character_set[address] = value;

    uint32_t character = address / m_p_impl->m_tile_size_bytes;
    uint32_t character_offset = character * m_p_impl->m_metadata.m_tile_width_pixels;
    uint32_t byte_of_char = address % m_p_impl->m_tile_size_bytes;
    uint32_t pix_column = byte_of_char % m_p_impl->m_tile_width_bytes;
    uint32_t pix_row = byte_of_char / m_p_impl->m_tile_width_bytes;
    m_p_impl->set_pixel_byte(value, pix_column, pix_row, character_offset);
}

SDL_Surface* Tileset_definition::get_tileset_surface()
{
    return m_p_impl->m_p_sdl_tileset_picture;
}

void Tileset_definition::set_full_tileset()
{
    m_p_impl->set_full_tileset();
}

void Tileset_definition::write_tileset_to_file(const char* p_filename)
{
    if(m_p_impl->m_p_sdl_square_tileset_picture == NULL)
    {
        SDL_Log("Cannot write a null picture to a file");
    }
    else if(p_filename == NULL)
    {
        SDL_Log("Cannot write a picture to a null filename");
    }
    else
    {
        m_p_impl->set_full_tileset();
        IMG_SavePNG(m_p_impl->m_p_sdl_square_tileset_picture, p_filename);
    }
}

void Tileset_definition::set_from_databus(Data_bus& data_bus, uint16_t start, uint16_t end)
{
    uint16_t dest = 0;
    for(uint16_t i = start; i < end; i++)
    {
        if(dest < m_p_impl->m_tileset_total_size)
        {
            m_p_impl->m_p_character_set[dest++] = data_bus.read(i);
        }
    }
}

}
