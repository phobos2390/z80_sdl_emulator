# Number values are in hex

set(COLOR_DEPTH 20)
set(FOREGROUND_COLOR FFCCCCFF)
set(BACKGROUND_COLOR FF000000)

set(MAX_BYTE_VALUE 100)

set(MEMORY_SIZE 1000)
set(GRID_HEIGHT 0040)
set(GRID_WIDTH  0080)
set(GRID_MASK_H FF)
set(GRID_MASK_L 80)
set(GRID_SIZE_C "0x${GRID_HEIGHT} * 0x${GRID_WIDTH}")
set(GRID_SIZE_ASM "\$${GRID_HEIGHT} * \$${GRID_WIDTH}")
set(STACK_TOP  FFFF)
set(STACK_SIZE 1000)
set(RAM_ORIGIN E000)
set(RAM_SIZE   1000)

set(TILE_WIDTH  8)
set(TILE_HEIGHT 8)

set(STDOUT_ADDRESS 8000)
set(GRID_ITERATOR  8002)
set(CHAR_INPUT     8001)

set(INTERRUPT_ADDRESS DF00)

set(TILESET_ORIGIN 2000)
set(SIZEOF_TILE_C "(0x${TILE_HEIGHT} * 0x${TILE_WIDTH})/(0x8)")
set(TILESET_SIZE_C "0x100 * (${SIZEOF_TILE_C})")

set(SIZEOF_TILE_ASM "(\$${TILE_HEIGHT} * \$${TILE_WIDTH})/(\$8)")
set(TILESET_SIZE_ASM "\$100 * (\$${SIZEOF_TILE_ASM})")

set(DISPLAY_ORIGIN 2800)
set(DISPLAY_SIZE_C "0x${GRID_HEIGHT} * 0x${GRID_WIDTH}")
set(DISPLAY_END_C "0x${DISPLAY_ORIGIN} + (${DISPLAY_SIZE_C})")

set(DISPLAY_SIZE_ASM "\$${GRID_HEIGHT} * \$${GRID_WIDTH}")
set(DISPLAY_END_ASM "\$${DISPLAY_ORIGIN} + (${DISPLAY_SIZE_ASM})")

set(TILESET_SPACE_START_ASM "\$${TILESET_ORIGIN} + (' ' * ${SIZEOF_TILE_ASM})")

configure_file(${CMAKE_SOURCE_DIR}/src/constants/emulator_constants.h.in
	       ${CMAKE_SOURCE_DIR}/src/emulator/emulator_constants.h)

configure_file(${CMAKE_SOURCE_DIR}/src/constants/assembler_constants.asm.in
	       ${CMAKE_SOURCE_DIR}/src/assembler/assembler_constants.asm)
