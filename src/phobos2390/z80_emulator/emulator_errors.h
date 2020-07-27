/// @file phobos2390/z80_emulator/emulator_constants.h

#ifndef Z80_EMULATOR_EMULATOR_ERRORS
#define Z80_EMULATOR_EMULATOR_ERRORS

#include <stdint.h>

typedef int32_t Error;
static const Error s_c_error_none = 0;
static const Error s_c_error_overlap = -1;
static const Error s_c_error_too_large_instruction_size = -2;

#endif /* Z80_EMULATOR_EMULATOR_ERRORS */
