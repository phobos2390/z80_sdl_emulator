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
#include <z80_emulator/Data_bus.h>
#include <z80_emulator/Data_bus_RAM.h>
#include <z80_emulator/Data_bus_integer.h>
#include <z80_emulator/Data_bus_function.h>
#include <z80_emulator/Instruction_ROM.h>
#include <z80_emulator/Grid_display.h>
#include <z80_emulator/Sprite_table.h>
#include <z80_emulator/Tileset_definition.h>

static bool continuing;
//static uint8_t interrupt_data;

static SDL_Renderer *p_sdl_renderer;

struct emulator_context
{
    bool m_halted;
    z80_emulator::Data_bus m_data_bus;
    z80_emulator::Tileset_metadata m_metadata;
    z80_emulator::Instruction_ROM m_instructions;
    z80_emulator::Grid_display m_display;
    z80_emulator::Sprite_table m_sprites;
    z80_emulator::Tileset_definition m_tileset;
    z80_emulator::Data_bus_integer<uint8_t> m_interrupt_data;
    
    emulator_context( uint16_t rom_size
                    , z80_emulator::Tileset_metadata metadata
                    , uint16_t height
                    , uint16_t width
                    , uint16_t sprite_count)
    : m_halted(false)
    , m_data_bus()
    , m_metadata(metadata)
    , m_instructions(rom_size)
    , m_display(metadata, height, width)
    , m_sprites(sprite_count)
    , m_tileset(metadata)
    , m_interrupt_data()
    {
    }
};

void screen_refresh(emulator_context& context)
{
    SDL_Texture *p_sdl_tileset_texture 
        = SDL_CreateTextureFromSurface( p_sdl_renderer
                                      , context.m_tileset.get_tileset_surface());
    
    SDL_RenderClear(p_sdl_renderer);
    context.m_display.render( p_sdl_tileset_texture
                            , p_sdl_renderer
                            , context.m_metadata);
    context.m_sprites.render( p_sdl_tileset_texture
                            , p_sdl_renderer
                            , context.m_metadata);
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
    emulator_context* p_emu_context = static_cast<emulator_context*>(context);
    p_emu_context->m_data_bus.write(address, value);
}

zuint8 read_cb(void* context, zuint16 address)
{
    emulator_context* p_emu_context = static_cast<emulator_context*>(context);
    uint8_t ret_val = p_emu_context->m_data_bus.read(address);
    return ret_val;
}

void halt_cb(void* context, zboolean state)
{
    screen_refresh(*reinterpret_cast<emulator_context*>(context));
    emulator_context* p_emu_context = static_cast<emulator_context*>(context);
    p_emu_context->m_halted = !state;
//    continuing = !state;
}

zuint32 int_data_cb(void* context)
{
    zuint32 data = 0;
    emulator_context* p_emu_context = static_cast<emulator_context*>(context);
    //printf("Interrupt callback invoked. Data: %x\n", interrupt_data);
    data = p_emu_context->m_interrupt_data.get_integer();
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

void write_percistence(void* context, uint16_t address, uint8_t value)
{
    uint32_t& fd = *reinterpret_cast<uint32_t*>(context);
    char char_value [sizeof(uint8_t)];
    char_value[0] = value;
    write(fd, char_value, sizeof(char_value[0]));
}
        
uint8_t read_percistence(void* context, uint16_t address)
{
    uint32_t& fd = *reinterpret_cast<uint32_t*>(context);
    char char_value [sizeof(uint8_t)];
    char_value[0] = 0;
    read(fd, char_value, sizeof(char_value[0]));
    return char_value[0];
}

size_t s_time_event_pushed = 0;

Uint32 emulator_timer(Uint32 interval, void * param)
{
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = 0;
    userevent.data2 = param;

    event.type = SDL_USEREVENT;
    event.user = userevent;
    
    if(s_time_event_pushed == 0)
    {
        SDL_PushEvent(&event);
        s_time_event_pushed = 1;
    }
    return(interval);
}

int main(int argc, char** argv)
{
    int width = s_c_grid_width * s_c_tile_width;
    int height = s_c_grid_height * s_c_tile_height;
    int depth = 32;
    
    int32_t error = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    
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

        SDL_Event event;

        continuing = TRUE;
        
        uint32_t iterations = 0;

        std::vector<Error> errors;
        
        if(argc > 1)
        {   
            size_t s_c_number_of_sprites = 0x40;
            size_t s_c_sprite_table_start = 0xB000;
            size_t s_c_percistence_addr = 0x80A0;
            
            z80_emulator::Tileset_metadata metadata;
            metadata.m_tile_width_pixels = s_c_tile_width;
            metadata.m_tile_height_pixels = s_c_tile_height;
            metadata.m_tile_color_depth = s_c_color_depth;
            memset(metadata.m_pallete, 0, sizeof(metadata.m_pallete));
            metadata.m_pallete[0] = s_c_foreground_color;
            metadata.m_pallete[1] = s_c_background_color;
            
            emulator_context context( s_c_memory_size
                                    , metadata
                                    , s_c_grid_height
                                    , s_c_grid_width
                                    , s_c_number_of_sprites);

            errors.push_back(context.m_data_bus.add_section(0,context.m_instructions));
            errors.push_back(context.m_data_bus.add_section(s_c_display_orig,context.m_display));
            errors.push_back(context.m_display.add_to_data_bus( context.m_data_bus
                                                              , s_c_grid_iter_addr
                                                              , s_c_stdout_addr));
            errors.push_back(context.m_data_bus.add_section(s_c_sprite_table_start,context.m_sprites));
            errors.push_back(context.m_data_bus.add_section(s_c_tileset_orig,context.m_tileset));
            
            z80_emulator::Data_bus_RAM ram(s_c_ram_size);
            z80_emulator::Data_bus_RAM stack(s_c_stack_size);
            
            errors.push_back(context.m_data_bus.add_section(s_c_ram_orig, ram));
            errors.push_back(context.m_data_bus.add_section(s_c_stack_top, stack));
            
            uint32_t percistence_file_descriptor = 0;
            
            z80_emulator::Data_bus_function perc_fd(1, &percistence_file_descriptor, 0, &write_percistence);
            
            if(argc > 2)
            {
                percistence_file_descriptor = open(argv[2], O_RDWR | O_CREAT);
                errors.push_back(context.m_data_bus.add_section(s_c_percistence_addr, perc_fd));
            }
            
            z80_emulator::Data_bus_integer<uint8_t> char_input;
            
            errors.push_back(context.m_data_bus.add_section(s_c_char_input_addr, char_input));
            
            errors.push_back(context.m_data_bus.add_section(s_c_interrupt_addr, context.m_interrupt_data));

            z80_emulator::Data_bus_integer<uint8_t> tick_data;
            
            errors.push_back(context.m_data_bus.add_section(s_c_tick_value_addr, tick_data));

            Z80 z80;
            z80.context = &context;
            z80.write = &write_cb;
            z80.read = &read_cb;
            z80.halt = &halt_cb;    
            z80.in = &in_cb;
            z80.out = &out_cb;
            z80.int_data = &int_data_cb;

            z80_power(&z80, TRUE);
            z80_reset(&z80);

            SDL_AddTimer(500, emulator_timer, &context);
            
            bool initializing = true;
            
            Error err = s_c_error_none;
            uint32_t err_index = 0;
            typedef std::vector<Error>::iterator errors_iter;
            for(errors_iter iter = errors.begin(); iter != errors.end(); ++iter)
            {
                if(err == s_c_error_none)
                {
                    err = (*iter);
                }
                err_index++;
                if((*iter) != s_c_error_none)
                {
                    printf("%d failed to add with %d", err_index, (*iter));
                }
            }
            
            while( continuing == TRUE
                /*&& (iterations++ < 0xF000)*/)
            {
                while(SDL_PollEvent(&event)) 
                {
                    bool key_entered = false;
                    if(event.type == SDL_USEREVENT)
                    {
                        s_time_event_pushed = 0;
                        tick_data.set_integer(tick_data.get_integer() + 1);
                        context.m_halted = false;
                        z80_int(&z80, TRUE);
                        while(!context.m_halted)
                        {
                            z80_run(&z80, 1);
                        }
                        screen_refresh(context);
                    }
                    else if (event.type == SDL_QUIT) 
                    {
                        printf("SDL_QUIT\n");
                        continuing = FALSE;
                    }
                    else if(event.type == SDL_TEXTINPUT)
                    {
                        char_input.set_integer(*event.text.text);
                        key_entered = true;
                    }
                    else if(event.type == SDL_KEYDOWN)
                    {
                        switch(event.key.keysym.scancode)
                        {
                            case SDL_SCANCODE_RETURN:
                                char_input.set_integer('\n');
                                key_entered = true;
                                break;
                            case SDL_SCANCODE_UP:
                                char_input.set_integer(0x01);
                                key_entered = true;
                                break;
                            case SDL_SCANCODE_DOWN:
                                char_input.set_integer(0x02);
                                key_entered = true;
                                break;
                            case SDL_SCANCODE_LEFT:
                                char_input.set_integer(0x03);
                                key_entered = true;
                                break;
                            case SDL_SCANCODE_RIGHT:
                                char_input.set_integer(0x04);
                                key_entered = true;
                                break;
                            case SDL_SCANCODE_BACKSPACE:
                                char_input.set_integer(0x08);
                                key_entered = true;
                                break;
                            default:
                                break;
                        }
                    }
                    if(key_entered)
                    {
                        context.m_halted = false;
                        z80_int(&z80, TRUE);
                        while(!context.m_halted)
                        {
                            z80_run(&z80, 50);
                        }
                    }
                }
                if(!context.m_halted)
                {
                    z80_run(&z80, 30);
                }
                else if(initializing)
                {
                    initializing = false;
                    z80_int(&z80, TRUE);
                    context.m_halted = false;
                    while(context.m_halted)
                    {
                        z80_run(&z80, 1);
                    }
                }
                if(initializing)
                {
                    screen_refresh(context);
                    SDL_RenderPresent(p_sdl_renderer);
                    SDL_Delay(10);
                }
            }
        }
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
