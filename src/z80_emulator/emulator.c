#include <emulation/CPU/Z80.h>
//#include <gtk/gtk.h>
//#include <gdk/gdk.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emulator/emulator_constants.h>

static uint8_t halted = 0;

static uint8_t continuing;
static uint16_t grid_iter;
static uint8_t char_input;
static uint8_t interrupt_data;

static SDL_Surface* p_sdl_tileset_picture;
static SDL_Renderer *p_sdl_renderer;

static char tileset[s_c_max_byte][s_c_tile_height];

static char display_grid[s_c_grid_size];

static char* instruction_names[s_c_max_byte] = {
/*    0            1         2          3           4        5      6           7     8          9         A          B         C          D         E        F */
/* 0 */ "nop", "ld_SS_WORD" , "ld_vbc_a" , "inc_SS", "V_X", "V_X", "ld_X_BYTE", "rlca","ex_af_af_", "add_hl_SS", "ld_a_vbc", "dec_SS","V_X", "V_X","ld_X_BYTE", "rrca",
/* 1 */ "djnz_OFFSET", "ld_SS_WORD", "ld_vde_a", "inc_SS", "V_X", "V_X", "ld_X_BYTE", "rla","jr_OFFSET", "add_hl_SS", "ld_a_vde", "dec_SS","V_X", "V_X","ld_X_BYTE", "rra",
/* 2 */ "jr_Z_OFFSET", "ld_SS_WORD", "ld_vWORD_hl", "inc_SS", "V_X", "V_X", "ld_X_BYTE", "daa","jr_Z_OFFSET", "add_hl_SS", "ld_hl_vWORD", "dec_SS","V_X", "V_X","ld_X_BYTE", "cpl",
/* 3 */ "jr_Z_OFFSET", "ld_SS_WORD", "ld_vWORD_a", "inc_SS", "V_vhl", "V_vhl", "ld_vhl_BYTE", "scf","jr_Z_OFFSET", "add_hl_SS", "ld_a_vWORD", "dec_SS","V_X", "V_X","ld_X_BYTE", "ccf",
/* 4 */ "ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y", "ld_X_Y", "ld_X_Y", "ld_X_vhl", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_vhl", "ld_X_Y",
/* 5 */ "ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y", "ld_X_Y", "ld_X_Y", "ld_X_vhl", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_vhl", "ld_X_Y",
/* 6 */ "ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y", "ld_X_Y", "ld_X_Y", "ld_X_vhl", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_vhl", "ld_X_Y",
/* 7 */ "ld_vhl_Y", "ld_vhl_Y","ld_vhl_Y", "ld_vhl_Y", "ld_vhl_Y", "ld_vhl_Y", "halt", "ld_vhl_Y", "ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_Y", "ld_X_Y","ld_X_vhl", "ld_X_Y",
/* 8 */ "U_a_Y", "U_a_Y","U_a_Y", "U_a_Y", "U_a_Y", "U_a_Y", "U_a_vhl","U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_vhl", "U_a_Y",
/* 9 */ "U_a_Y", "U_a_Y","U_a_Y", "U_a_Y", "U_a_Y", "U_a_Y", "U_a_vhl","U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_vhl", "U_a_Y",
/* A */ "U_a_Y", "U_a_Y","U_a_Y", "U_a_Y", "U_a_Y", "U_a_Y", "U_a_vhl","U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_vhl", "U_a_Y",
/* B */ "U_a_Y", "U_a_Y","U_a_Y", "U_a_Y", "U_a_Y", "U_a_Y", "U_a_vhl","U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_Y", "U_a_Y","U_a_vhl", "U_a_Y",
/* C */ "ret_Z", "pop_TT","jp_Z_WORD", "jp_WORD", "call_Z_WORD", "push_TT", "U_a_BYTE", "rst_N","ret_Z", "ret","jp_Z_WORD", "CB","call_Z_WORD", "call_WORD", "U_a_BYTE", "rst_N",
/* D */ "ret_Z", "pop_TT","jp_Z_WORD", "out_vBYTE_a", "call_Z_WORD", "push_TT", "U_a_BYTE", "rst_N","ret_Z", "exx","jp_Z_WORD", "in_a_BYTE", "call_Z_WORD", "DD","U_a_BYTE", "rst_N",
/* E */ "ret_Z", "pop_TT","jp_Z_WORD", "ex_vsp_hl", "call_Z_WORD", "push_TT", "U_a_BYTE", "rst_N","ret_Z", "jp_hl","jp_Z_WORD", "ex_de_hl","call_Z_WORD", "ED","U_a_BYTE", "rst_N",
/* F */ "ret_Z", "pop_TT","jp_Z_WORD", "di", "call_Z_WORD", "push_TT", "U_a_BYTE", "rst_N","ret_Z", "ld_sp_hl","jp_Z_WORD", "ei","call_Z_WORD", "FD","U_a_BYTE", "rst_N"
};

uint8_t next_opcode=0;

uint8_t opcode_sizes[] = {
//         0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
//-----------------------------------------------------------------------------------------
/* 0x00 */ 0x1, 0x3, 0x1, 0x1, 0x1, 0x1, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x1,
/* 0x10 */ 0x2, 0x3, 0x1, 0x1, 0x1, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x1,
/* 0x20 */ 0x1, 0x3, 0x3, 0x1, 0x1, 0x1, 0x2, 0x1, 0x2, 0x1, 0x3, 0x1, 0x1, 0x1, 0x2, 0x1,
/* 0x30 */ 0x1, 0x3, 0x3, 0x1, 0x1, 0x1, 0x2, 0x1, 0x2, 0x1, 0x3, 0x1, 0x1, 0x1, 0x2, 0x1,
/* 0x40 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0x50 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0x60 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0x70 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0x80 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0x90 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0xA0 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0xB0 */ 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
/* 0xC0 */ 0x1, 0x1, 0x3, 0x3, 0x3, 0x1, 0x2, 0x1, 0x1, 0x1, 0x3, 0x2, 0x3, 0x3, 0x2, 0x1,
/* 0xD0 */ 0x1, 0x1, 0x3, 0x2, 0x3, 0x1, 0x2, 0x1, 0x1, 0x1, 0x3, 0x2, 0x3, 0xF, 0x2, 0x1,
/* 0xE0 */ 0x1, 0x1, 0x3, 0x1, 0x3, 0x1, 0x2, 0x1, 0x1, 0x1, 0x3, 0x1, 0x3, 0xF, 0x2, 0x1,
/* 0xF0 */ 0x1, 0x1, 0x3, 0x1, 0x3, 0x1, 0x2, 0x1, 0x1, 0x1, 0x3, 0x1, 0x3, 0xF, 0x2, 0x1,
};

uint8_t opcode_sizes_ED[] = {
//         0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
//-----------------------------------------------------------------------------------------
/* 0x00 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* 0x10 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0x20 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0x30 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0x40 */ 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2,
/* 0x50 */ 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2,
/* 0x60 */ 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x2,
/* 0x70 */ 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x0, 0x2, 0x2, 0x2, 0x4, 0x2, 0x2, 0x2, 0x0,
/* 0x80 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* 0x90 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0xA0 */ 0x2, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0,
/* 0xB0 */ 0x2, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0,
/* 0xC0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* 0xD0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0xE0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0xF0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

uint8_t opcode_sizes_I[] = {
//         0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
//-----------------------------------------------------------------------------------------
/* 0x00 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* 0x10 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0x20 */ 0x0, 0x4, 0x4, 0x2, 0x2, 0x2, 0x3, 0x0, 0x0, 0x2, 0x4, 0x2, 0x2, 0x2, 0x3, 0x0,
/* 0x30 */ 0x0, 0x0, 0x0, 0x0, 0x3, 0x3, 0x4, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0x40 */ 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0,
/* 0x50 */ 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0,
/* 0x60 */ 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x2,
/* 0x70 */ 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0,
/* 0x80 */ 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 
/* 0x90 */ 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0,
/* 0xA0 */ 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0,
/* 0xB0 */ 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x3, 0x0,
/* 0xC0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 
/* 0xD0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0xE0 */ 0x0, 0x2, 0x0, 0x2, 0x0, 0x2, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
/* 0xF0 */ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

uint8_t instructions [s_c_memory_size];

uint8_t stack [s_c_stack_size];

uint8_t ram [s_c_ram_size];

void set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, uint32_t pixel)
{
  uint32_t *target_pixel = (uint32_t*)
                            ((uint8_t *) 
                                ( surface->pixels 
                                + ( y * surface->pitch)
                                + ( x * sizeof(*target_pixel))));
  *target_pixel = pixel;
}

void init_tileset()
{
    p_sdl_tileset_picture = SDL_CreateRGBSurface(0, s_c_max_byte * s_c_tile_width , s_c_tile_height, s_c_color_depth, 0, 0, 0, 0);

    if (p_sdl_tileset_picture == NULL) 
    {
        SDL_Log("SDL_CreateRGBSurface() failed for tileset create: %s", SDL_GetError());
    }
}

void free_tileset()
{
    if (p_sdl_tileset_picture == NULL) 
    {
        SDL_Log("Can't free a null tileset");
    }
    else
    {
        SDL_FreeSurface(p_sdl_tileset_picture);
    }
}

void write_tileset(char* filename)
{
    if(p_sdl_tileset_picture == NULL)
    {
        SDL_Log("Cannot write a null picture to a file");
    }
    else if(filename == NULL)
    {
        SDL_Log("Cannot write a picture to a null filename");
    }
    else
    {
        IMG_SavePNG(p_sdl_tileset_picture, filename);
    }
}

void set_tileset_character(char character)
{
    if(p_sdl_tileset_picture == NULL)
    {
        printf("Tileset surface not loaded");
    }
    else
    {
        uint8_t ucharacter = character;
        size_t character_offset = ucharacter * s_c_tile_width;
        for(uint32_t i = 0; i < s_c_tile_height; i++)
        {
            uint32_t character_row = tileset[ucharacter][i];
            for(uint32_t j = 0; j < s_c_tile_width; j++)
            {
                uint32_t r = (s_c_tile_width - 1) - j;
                uint8_t pixel = ((character_row & (0x1 << r)) >> r);
                uint32_t pixel_value = s_c_background_color;
                if(pixel)
                {
                    pixel_value = s_c_foreground_color;
                }
                set_pixel(p_sdl_tileset_picture, j + character_offset, i, pixel_value);
            }
        }
    }
}

void screen_refresh()
{
    SDL_Texture *p_sdl_tileset_texture 
        = SDL_CreateTextureFromSurface(p_sdl_renderer, p_sdl_tileset_picture);
    
    SDL_RenderClear(p_sdl_renderer);
    for(size_t i = 0; i < s_c_grid_height; i++)
    {
        for(size_t j = 0; j < s_c_grid_width; j++)
        {
            SDL_Rect dest_rect;
            dest_rect.w = s_c_tile_width;
            dest_rect.h = s_c_tile_height;
            dest_rect.x = j * s_c_tile_width;
            dest_rect.y = i * s_c_tile_height;
            uint8_t ucharacter = display_grid[i * s_c_grid_width + j];
            SDL_Rect src_rect;
            src_rect.w = s_c_tile_width;
            src_rect.h = s_c_tile_height;
            src_rect.x = ucharacter * s_c_tile_width;
            src_rect.y = 0;
            SDL_RenderCopy( p_sdl_renderer
                          , p_sdl_tileset_texture
                          ,&src_rect
                          ,&dest_rect);
        }
    }
    SDL_RenderPresent(p_sdl_renderer);
    SDL_DestroyTexture(p_sdl_tileset_texture);
}

zuint8 in_cb(void *context, zuint16 port)
{
    printf("Value read from port: 0x%d: 0x0\n", port);
    return 0x0;
}

void out_cb(void *context, zuint16 port, zuint8 value)
{
    printf("Value written to port: 0x%d: 0x%x\n", port, value);
}

void write_cb(void* context, zuint16 address, zuint8 value)
{
    if(((s_c_stack_top - s_c_stack_size) < address)
     && (address <= s_c_stack_top))
    {
        //printf("Writing to stack %x %x\n", address, value);
        stack[s_c_stack_top - address] = value;
    }
    else if(address == s_c_stdout_addr)
    {
        //printf("'%c'\n", value);
        display_grid[grid_iter++] = value;
        screen_refresh();
	grid_iter = (grid_iter % s_c_grid_size);
    }
    else if( (s_c_grid_iter_addr <= address)
          && (address < (s_c_grid_iter_addr + sizeof(grid_iter))))
    {
        uint8_t* p_grid_iter = (uint8_t*)(&grid_iter);
	p_grid_iter[address - s_c_grid_iter_addr] = value;
	grid_iter = (grid_iter % s_c_grid_size);
        //printf("Grid Iterator: %x\n", grid_iter);
    }
    else if(address == s_c_char_input_addr)
    {
        //printf("Setting buffered char input: %x\n", value);
        char_input = value;
    } 
    else if(address == s_c_interrupt_addr)
    {
        interrupt_data = value;
    } 
    else if((s_c_ram_orig <= address) 
         && (address < (s_c_ram_size + s_c_ram_orig)))
    {
//	printf("0x%x: 0x%x RAM WRITE\n", address, value);
        ram[address - s_c_ram_orig] = value;
    }
    else if((s_c_tileset_orig <= address)
         && (address < (s_c_tileset_size + s_c_tileset_orig)))
    {
        zuint16 tile_value =  address - s_c_tileset_orig;
        zuint16 character = tile_value / s_c_tile_height;
        zuint16 character_row = tile_value % s_c_tile_width;
        tileset[character][character_row] = value;
        set_tileset_character(character);
    }
    else if((s_c_display_orig <= address)
         && (address < (s_c_display_orig + s_c_grid_size)))
    {
        display_grid[address - s_c_display_orig] = value;
        screen_refresh();
    }
    fflush(stdout);
}

zuint8 read_cb(void* context, zuint16 address)
{
    zuint8 read_value = 0;
    if(address < s_c_memory_size)
    {
        read_value = instructions[address];
	if(next_opcode == 0)
        {
            switch(read_value)
            {
                 case 0xDD:
                 case 0xFD:
                      next_opcode = opcode_sizes_I[instructions[address+1]];
                      break;
                 case 0xED:
                      next_opcode = opcode_sizes_ED[instructions[address+1]];
                      break;
                 default:
                      next_opcode=opcode_sizes[read_value]; 
                      break;
            }
            switch(next_opcode)
            {
                 case 0x0:
//                      printf("Instruction Error: %s Failed to include proper opcode information (%x)",instruction_names[read_value], read_value);
                      break;
                 case 0x1:
//                      printf("0x%x: %s (%x)\n", address, instruction_names[read_value], read_value);
                      break;
                 case 0x2:
//                      printf("0x%x: %s (%x, %x)\n", address, instruction_names[read_value], read_value, instructions[address + 1]);
                      break;
                 case 0x3:
//                      printf("0x%x: %s (%x, %x, %x)\n", address, instruction_names[read_value], read_value, instructions[address + 1], instructions[address + 2]);
                      break;
                 case 0x4:
//                      printf("0x%x: %s (%x, %x, %x, %x)\n", address, instruction_names[read_value], read_value, instructions[address + 1], instructions[address + 2], instructions[address + 3]);
                      break;
                 case 0xF:
//                      printf("Instruction Error: %s Failed to include proper opcode information (%x)",instruction_names[read_value], read_value);
                      break;
                 default:
                     break;
            }
        }
        --next_opcode;
    }
    if(((s_c_stack_top - s_c_stack_size) < address)
     && (address <= s_c_stack_top))
    {
        read_value = stack[s_c_stack_top - address];
    }
    else if((s_c_ram_orig <= address) 
         && (address < (s_c_ram_size + s_c_ram_orig)))
    {
        read_value = ram[address - s_c_ram_orig];
//	printf("0x%x: 0x%x RAM READ", address, read_value);
    }
    else if( (s_c_grid_iter_addr <= address)
          && (address < (s_c_grid_iter_addr + sizeof(grid_iter))))
    {
        uint8_t* p_grid_iter = (uint8_t*)(&grid_iter);
	read_value = p_grid_iter[address - s_c_grid_iter_addr];
    }
    else if(address == s_c_char_input_addr)
    {
//        printf("char input: '%c'\n", char_input);
        read_value = char_input;
    }
    else if((s_c_display_orig <= address)
         && (address < (s_c_display_orig + s_c_grid_size)))
    {
        read_value = display_grid[address - s_c_display_orig];
    }
    fflush(stdout);
    return read_value;
}

void halt_cb(void* context, zboolean state)
{
    screen_refresh();
    halted = !state;
//    continuing = !state;
}

zuint32 int_data_cb(void* context)
{
    zuint32 data = 0;
    //printf("Interrupt callback invoked. Data: %x\n", interrupt_data);
    data = interrupt_data;
    return data;
}

void read_binary(char* filename, uint8_t* instruct, size_t memory_size)
{
    int32_t fd = open(filename, O_RDONLY);
    uint8_t continue_reading = TRUE;
    memset(instruct, 0, memory_size);
    while(continue_reading == TRUE)
    {
        ssize_t read_bytes = read(fd, instruct, memory_size);
        printf("Read %ld bytes\n", read_bytes);
        if(read_bytes <= 0)
        {
            continue_reading = FALSE;
        }
    }
    close(fd);
}

int main(int argc, char** argv)
{
    int width = s_c_grid_width * s_c_tile_width;
    int height = s_c_grid_height * s_c_tile_height;
    int depth = 32;
    
    memset(tileset, 0, s_c_max_byte * s_c_tile_height);
    memset(display_grid, 0, s_c_grid_size);
    
    init_tileset();
    
    int32_t error = SDL_Init(SDL_INIT_VIDEO);
    
    if(error == 0)
    {
        SDL_Window *window = SDL_CreateWindow( "Z80 Test"
                                             , SDL_WINDOWPOS_UNDEFINED
                                             , SDL_WINDOWPOS_UNDEFINED
                                             , width
                                             , height
                                             , 0);

        p_sdl_renderer = SDL_CreateRenderer( window
                                           , -1
                                           , SDL_RENDERER_SOFTWARE);
        SDL_SetRenderDrawColor(p_sdl_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(p_sdl_renderer);
        SDL_RenderPresent(p_sdl_renderer);

        for(uint16_t i = 0; i < s_c_max_byte; i++)
        {
            set_tileset_character(i);
        }

        write_tileset("tileset_initial.png");
        
        SDL_Event event;

        halted = FALSE;
        continuing = TRUE;
        grid_iter = 0;
        
        uint32_t iterations = 0;

        if(argc > 1)
        {
            read_binary(argv[1], instructions, s_c_memory_size);

            Z80 z80;
            z80.context = &z80;
            z80.write = &write_cb;
            z80.read = &read_cb;
            z80.halt = &halt_cb;    
            z80.in = &in_cb;
            z80.out = &out_cb;
            z80.int_data = &int_data_cb;

            z80_power(&z80, TRUE);
            z80_reset(&z80);

            uint8_t initializing = TRUE;
            
            while( continuing == TRUE
                /*&& (iterations++ < 0xF000)*/)
            {
                while(SDL_PollEvent(&event)) 
                {
                    uint8_t key_entered = FALSE;
                    if (event.type == SDL_QUIT) 
                    {
                        printf("SDL_QUIT\n");
                        continuing = FALSE;
                    }
                    else if(event.type == SDL_TEXTINPUT)
                    {
//                        printf("\"%s\"\n", event.text.text);
                        char_input = *event.text.text;
                        key_entered = TRUE;
                    }
                    else if(event.type == SDL_KEYDOWN)
                    {
                        switch(event.key.keysym.scancode)
                        {
                            case SDL_SCANCODE_RETURN:
                                char_input = '\n';
//                                printf("\"Return key entered\"\n");
                                key_entered = TRUE;
                                break;
                            case SDL_SCANCODE_UP:
                                char_input = 0x01;
//                                printf("\"UP   entered\"\n");
                                key_entered = TRUE;
                                break;
                            case SDL_SCANCODE_DOWN:
                                char_input = 0x02;
//                                printf("\"DOWN entered\"\n");
                                key_entered = TRUE;
                                break;
                            case SDL_SCANCODE_LEFT:
                                char_input = 0x03;
//                                printf("\"LEFT entered\"\n");
                                key_entered = TRUE;
                                break;
                            case SDL_SCANCODE_RIGHT:
                                char_input = 0x04;
//                                printf("\"RIGHT entered\"\n");
                                key_entered = TRUE;
                                break;
                            case SDL_SCANCODE_BACKSPACE:
                                char_input = 0x08;
//                                printf("\"Backspace entered\"\n");
                                key_entered = TRUE;
                                break;
                            default:
                                break;
                        }
                    }
                    if(key_entered == TRUE)
                    {
                        halted = FALSE;
                        z80_int(&z80, TRUE);
                        while(halted == FALSE)
                        {
                            z80_run(&z80, 1);
                        }
                    }
                }
                if(halted == FALSE)
                {
                    z80_run(&z80, 30);
                }
                else if(initializing == TRUE)
                {
                    initializing = FALSE;
                    z80_int(&z80, TRUE);
                    halted = FALSE;
                    while(halted == FALSE)
                    {
                        z80_run(&z80, 1);
                    }
                }
                if(initializing == FALSE)
                {
                    screen_refresh();
                    SDL_RenderPresent(p_sdl_renderer);
                    SDL_Delay(10);
                }
            }
        }       
        
        write_tileset("tileset.png");

        free_tileset();
        SDL_DestroyRenderer(p_sdl_renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    else
    {
        SDL_Log("SDL_Init() failed: %s", SDL_GetError());
    }

    return 0;
}
