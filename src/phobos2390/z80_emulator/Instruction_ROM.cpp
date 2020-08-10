/// @file z80_emulator//Instruction_ROM.cpp

#include <z80_emulator/Instruction_ROM.h>
#include <z80_emulator/Data_bus_RAM.h>
#include <z80_emulator/emulator_errors.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <map>

namespace z80_emulator
{
static const uint16_t s_c_instruction_array_size_bytes = 0x100;    
static const size_t s_c_instruction_name_max_length = 0x20;

static const char* instruction_names[s_c_instruction_array_size_bytes] = {
/*       0              1             2                 3               4              5              6               7            8            9            A             B            C             D           E             F */
/* 0 */ "nop",         "ld bc,**",   "ld (bc),a",      "inc bc",       "inc b",       "dec b",       "ld b,*",       "rlca",      "ex af,af'", "add hl,bc", "ld a,(bc)",  "dec bc",    "inc c",      "dec c",    "ld c,*",     "rrca",
/* 1 */ "djnz *",      "ld de,**",   "ld (de),a",      "inc de",       "inc d",       "dec d",       "ld d,*",       "rla",       "jr *",      "add hl,de", "ld a,(de)",  "dec de",    "inc e",      "dec e",    "ld e,*",     "rra",
/* 2 */ "jr nz,*",     "ld hl,**",   "ld (**),hl",     "inc hl",       "inc h",       "dec h",       "ld h,*",       "daa",       "jr z,*",    "add hl,hl", "ld hl,(**)", "dec hl",    "inc l",      "dec l",    "ld l,*",     "cpl",
/* 3 */ "jr nc,*",     "ld sp,**",   "ld (**),a",      "inc sp",       "inc (hl)",    "dec (hl)",    "ld (hl),*",    "scf",       "jr c,*",    "add hl,sp", "ld a,(**)",  "dec sp",    "inc a",      "dec a",    "ld a,*",     "ccf",
/* 4 */ "ld b,b",      "ld b,c",     "ld b,d",         "ld b,e",       "ld b,h",      "ld b,l",      "ld b,(hl)",    "ld b,a",    "ld c,b",    "ld c,c",    "ld c,d",     "ld c,e",    "ld c,h",     "ld c,l",   "ld c,(hl)",  "ld c,a",
/* 5 */ "ld d,b",      "ld d,c",     "ld d,d",         "ld d,e",       "ld d,h",      "ld d,l",      "ld d,(hl)",    "ld d,a",    "ld e,b",    "ld e,c",    "ld e,d",     "ld e,e",    "ld e,h",     "ld e,l",   "ld e,(hl)",  "ld e,a",
/* 6 */ "ld h,b",      "ld h,c",     "ld h,d",         "ld h,e",       "ld h,h",      "ld h,l",      "ld h,(hl)",    "ld h,a",    "ld l,b",    "ld l,c",    "ld l,d",     "ld l,e",    "ld l,h",     "ld l,l",   "ld l,(hl)",  "ld l,a",
/* 7 */ "ld (hl),b",   "ld (hl),c",  "ld (hl),d",      "ld (hl),e",    "ld (hl),h",   "ld (hl),l",   "halt",         "ld (hl),a", "ld a,b",    "ld a,c",    "ld a,d",     "ld a,e",    "ld a,h",     "ld a,l",   "ld a,(hl)",  "ld a,a",
/* 8 */ "add a,b",     "add a,c",    "add a,d",        "add a,e",      "add a,h",     "add a,l",     "add a,(hl)",   "add a,a",   "adc a,b",   "adc a,c",   "adc a,d",    "adc a,e",   "adc a,h",    "adc a,l",  "adc a,(hl)", "adc a,a",
/* 9 */ "sub b",       "sub c",      "sub d",          "sub e",        "sub h",       "sub l",       "sub (hl)",     "sub a",     "sbc a,b",   "sbc a,c",   "sbc a,d",    "sbc a,e",   "sbc a,h",    "sbc a,l",  "sbc a,(hl)", "sbc a,a",
/* A */ "and b",       "and c",      "and d",          "and e",        "and h",       "and l",       "and (hl)",     "and a",     "xor b",     "xor c",     "xor d",      "xor e",     "xor h",      "xor l",    "xor (hl)",   "xor a",
/* B */ "or b",        "or c",       "or d",           "or e",         "or h",        "or l",        "or (hl)",      "or a",      "cp b",      "cp c",      "cp d",       "cp e",      "cp h",       "cp l",     "cp (hl)",    "cp a",
/* C */ "ret nz",      "pop bc",     "jp nz,**",       "jp **",        "call nz,**",  "push bc",     "add a,*",      "rst 00h",   "ret z",     "ret",       "jp z,**",    "BITS",      "call z,**",  "call **",  "adc a,*",    "rst 08h",
/* D */ "ret nc",      "pop de",     "jp nc,**",       "out (*),a ",   "call nc,**",  "push de",     "sub *",        "rst 10h",   "ret c",     "exx",       "jp c,**",    "in a,(*)",  "call c,**",  "IX",       "sbc a,*",    "rst 18h",
/* E */ "ret po",      "pop hl",     "jp po,**",       "ex (sp),h l",  "call po,**",  "push hl",     "and *",        "rst 20h",   "ret pe",    "jp (hl)",   "jp pe,**",   "ex de,hl",  "call pe,**", "EXTD",     "xor *",      "rst 28h",
/* F */ "ret p",       "pop af",     "jp p,**",        "di",           "call p,**",   "push af",     "or *",         "rst 30h",   "ret m",     "ld sp,hl",  "jp m,**",    "ei",        "call m,**",  "IY",       "cp *",       "rst 38h"
};

static const char* ED_instruction_names[s_c_instruction_array_size_bytes] = {
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "in b,(c)", "out (c),b", "sbc hl,bc", "ld (**),bc", "neg", "retn", "im 0", "ld i,a", "in c,(c)", "out (c),c", "adc hl,bc", "ld bc,(**)", "neg", "reti", "im 0/1", "ld r,a",
    "in d,(c)", "out (c),d", "sbc hl,de", "ld (**),de", "neg", "retn", "im 1", "ld a,i", "in e,(c)", "out (c),e", "adc hl,de", "ld de,(**)", "neg", "retn", "im 2",   "ld a,r",
    "in h,(c)", "out (c),h", "sbc hl,hl", "ld (**),hl", "neg", "retn", "im 0", "rrd",    "in l,(c)", "out (c),l", "adc hl,hl", "ld hl,(**)", "neg", "retn", "im 0/1", "rld",
    "in (c)",   "out (c),0", "sbc hl,sp", "ld (**),sp", "neg", "retn", "im 1", "",       "in a,(c)", "out (c),a", "adc hl,sp", "ld sp,(**)", "neg", "retn", "im 2",   "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "ldi",      "cpi",       "ini",       "outi",       "",    "",     "",     "",       "ldd",      "cpd",       "ind",       "outd",       "",    "",     "",       "", 
    "ldir",     "cpir",       "inir",     "otir",       "",    "",     "",     "",       "lddr",     "cpdr",      "indr",      "otdr",       "",    "",     "",       "", 
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       "",
    "",         "",          "",          "",           "",    "",     "",     "",       "",         "",          "",          "",           "",    "",     "",       ""
};

static const char* CB_instruction_names[s_c_instruction_array_size_bytes] = {
    "rlc b",   "rlc c",   "rlc d",   "rlc e",   "rlc h",   "rlc l",   "rlc (hl)",   "rlc a",   "rrc b",   "rrc c",   "rrc d",   "rrc e",   "rrc h",   "rrc l",   "rrc (hl)",   "rrc a",
    "rl b",    "rl c",    "rl d",    "rl e",    "rl h",    "rl l",    "rl (hl)",    "rl a",    "rr b",    "rr c",    "rr d",    "rr e",    "rr h",    "rr l",    "rr (hl)",    "rr a",
    "sla b",   "sla c",   "sla d",   "sla e",   "sla h",   "sla l",   "sla (hl)",   "sla a",   "sra b",   "sra c",   "sra d",   "sra e",   "sra h",   "sra l",   "sra (hl)",   "sra a",
    "sll b",   "sll c",   "sll d",   "sll e",   "sll h",   "sll l",   "sll (hl)",   "sll a",   "srl b",   "srl c",   "srl d",   "srl e",   "srl h",   "srl l",   "srl (hl)",   "srl a",
    "bit 0,b", "bit 0,c", "bit 0,d", "bit 0,e", "bit 0,h", "bit 0,l", "bit 0,(hl)", "bit 0,a", "bit 1,b", "bit 1,c", "bit 1,d", "bit 1,e", "bit 1,h", "bit 1,l", "bit 1,(hl)", "bit 1,a",
    "bit 2,b", "bit 2,c", "bit 2,d", "bit 2,e", "bit 2,h", "bit 2,l", "bit 2,(hl)", "bit 2,a", "bit 3,b", "bit 3,c", "bit 3,d", "bit 3,e", "bit 3,h", "bit 3,l", "bit 3,(hl)", "bit 3,a",
    "bit 4,b", "bit 4,c", "bit 4,d", "bit 4,e", "bit 4,h", "bit 4,l", "bit 4,(hl)", "bit 4,a", "bit 5,b", "bit 5,c", "bit 5,d", "bit 5,e", "bit 5,h", "bit 5,l", "bit 5,(hl)", "bit 5,a",
    "bit 6,b", "bit 6,c", "bit 6,d", "bit 6,e", "bit 6,h", "bit 6,l", "bit 6,(hl)", "bit 6,a", "bit 7,b", "bit 7,c", "bit 7,d", "bit 7,e", "bit 7,h", "bit 7,l", "bit 7,(hl)", "bit 7,a",
    "res 0,b", "res 0,c", "res 0,d", "res 0,e", "res 0,h", "res 0,l", "res 0,(hl)", "res 0,a", "res 1,b", "res 1,c", "res 1,d", "res 1,e", "res 1,h", "res 1,l", "res 1,(hl)", "res 1,a",
    "res 2,b", "res 2,c", "res 2,d", "res 2,e", "res 2,h", "res 2,l", "res 2,(hl)", "res 2,a", "res 3,b", "res 3,c", "res 3,d", "res 3,e", "res 3,h", "res 3,l", "res 3,(hl)", "res 3,a",
    "res 4,b", "res 4,c", "res 4,d", "res 4,e", "res 4,h", "res 4,l", "res 4,(hl)", "res 4,a", "res 5,b", "res 5,c", "res 5,d", "res 5,e", "res 5,h", "res 5,l", "res 5,(hl)", "res 5,a",
    "res 6,b", "res 6,c", "res 6,d", "res 6,e", "res 6,h", "res 6,l", "res 6,(hl)", "res 6,a", "res 7,b", "res 7,c", "res 7,d", "res 7,e", "res 7,h", "res 7,l", "res 7,(hl)", "res 7,a",
    "set 0,b", "set 0,c", "set 0,d", "set 0,e", "set 0,h", "set 0,l", "set 0,(hl)", "set 0,a", "set 1,b", "set 1,c", "set 1,d", "set 1,e", "set 1,h", "set 1,l", "set 1,(hl)", "set 1,a",
    "set 2,b", "set 2,c", "set 2,d", "set 2,e", "set 2,h", "set 2,l", "set 2,(hl)", "set 2,a", "set 3,b", "set 3,c", "set 3,d", "set 3,e", "set 3,h", "set 3,l", "set 3,(hl)", "set 3,a",
    "set 4,b", "set 4,c", "set 4,d", "set 4,e", "set 4,h", "set 4,l", "set 4,(hl)", "set 4,a", "set 5,b", "set 5,c", "set 5,d", "set 5,e", "set 5,h", "set 5,l", "set 5,(hl)", "set 5,a",
    "set 6,b", "set 6,c", "set 6,d", "set 6,e", "set 6,h", "set 6,l", "set 6,(hl)", "set 6,a", "set 7,b", "set 7,c", "set 7,d", "set 7,e", "set 7,h", "set 7,l", "set 7,(hl)", "set 7,a"
};

static const char* IX_instructions_names[s_c_instruction_array_size_bytes] = {
    "",            "",            "", "", "", "", "", "", "", "add ix,bc", "", "", "", "", "", "",
    "",            "",            "", "", "", "", "", "", "", "add ix,de", "", "", "", "", "", "",
    "",            "ld ix,**",    "ld (**),ix", "inc ix", "inc ixh", "dec ixh", "ld ixh,*", "", "", "add ix,ix", "ld ix,(**)", "dec ix", "inc ixl", "dec ixl", "ld ixl,*", "",
    "",            "",            "", "", "inc (ix+*)", "dec (ix+*)", "ld (ix+*),*", "", "", "add ix,sp", "", "", "", "", "", "",
    "",            "",            "", "", "ld b,ixh", "ld b,ixl", "ld b,(ix+*)", "", "", "", "", "", "ld c,ixh", "ld c,ixl", "ld c,(ix+*)", "",
    "",            "",            "", "", "ld d,ixh", "ld d,ixl", "ld d,(ix+*)", "", "", "", "", "", "ld e,ixh", "ld e,ixl", "ld e,(ix+*)", "",
    "ld ixh,b",    "ld ixh,c", "ld ixh,d", "ld ixh,e", "ld ixh,ixh", "ld ixh,ixl", "ld h,(ix+*)", "ld ixh,a", "ld ixl,b", "ld ixl,c", "ld ixl,d", "ld ixl,e", "ld ixl,ixh", "ld ixl,ixl", "ld l,(ix+*)", "ld ixl,a",
    "ld (ix+*),b", "ld (ix+*),c", "ld (ix+*),d", "ld (ix+*),e", "ld (ix+*),h", "ld (ix+*),l", "", "ld (ix+*),a", "", "", "", "", "ld a,ixh", "ld a,ixl", "ld a,(ix+*)", "",
    "",            "",            "", "", "add a,ixh", "add a,ixl", "add a,(ix+*)", "", "", "", "", "", "adc a,ixh", "adc a,ixl", "adc a,(ix+*)", "",
    "",            "",            "", "", "sub ixh", "sub ixl", "sub (ix+*)", "", "", "", "", "", "sbc a,ixh", "sbc a,ixl", "sbc a,(ix+*)", "",
    "",            "",            "", "", "and ixh", "and ixl", "and (ix+*)", "", "", "", "", "", "xor ixh", "xor ixl", "xor (ix+*)", "",
    "",            "",            "", "", "or ixh", "or ixl", "or (ix+*)", "", "", "", "", "", "cp ixh", "cp ixl", "cp (ix+*)", "",
    "",            "",            "", "", "", "", "", "", "", "", "", "", "IX BITS", "", "", "",
    "",            "",            "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "",            "pop ix", "", "ex (sp),ix", "", "push ix", "", "", "", "jp (ix)", "", "", "", "", "", "",
    "",            "",       "", "", "", "", "", "", "", "ld sp,ix", "", "", "", "", "", ""    
};

static const char* IX_bit_instructions_names[s_c_instruction_array_size_bytes] = {
    "rlc (ix+*),b", "rlc (ix+*),c", "rlc (ix+*),d", "rlc (ix+*),e", "rlc (ix+*),h", "rlc (ix+*),l", "rlc (ix+*)", "rlc (ix+*),a", "rrc (ix+*),b", "rrc (ix+*),c", "rrc (ix+*),d", "rrc (ix+*),e", "rrc (ix+*),h", "rrc (ix+*),l", "rrc (ix+*)", "rrc (ix+*),a",
    "rl (ix+*),b", "rl (ix+*),c", "rl (ix+*),d", "rl (ix+*),e", "rl (ix+*),h", "rl (ix+*),l", "rl (ix+*)", "rl (ix+*),a", "rr (ix+*),b", "rr (ix+*),c", "rr (ix+*),d", "rr (ix+*),e", "rr (ix+*),h", "rr (ix+*),l", "rr (ix+*)", "rr (ix+*),a",
    "sla (ix+*),b", "sla (ix+*),c", "sla (ix+*),d", "sla (ix+*),e", "sla (ix+*),h", "sla (ix+*),l", "sla (ix+*)", "sla (ix+*),a", "sra (ix+*),b", "sra (ix+*),c", "sra (ix+*),d", "sra (ix+*),e", "sra (ix+*),h", "sra (ix+*),l", "sra (ix+*)", "sra (ix+*),a",
    "sll (ix+*),b", "sll (ix+*),c", "sll (ix+*),d", "sll (ix+*),e", "sll (ix+*),h", "sll (ix+*),l", "sll (ix+*)", "sll (ix+*),a", "srl (ix+*),b", "srl (ix+*),c", "srl (ix+*),d", "srl (ix+*),e", "srl (ix+*),h", "srl (ix+*),l", "srl (ix+*)", "srl (ix+*),a",
    "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 0,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)", "bit 1,(ix+*)",
    "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 2,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)", "bit 3,(ix+*)",
    "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 4,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)", "bit 5,(ix+*)",
    "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 6,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)", "bit 7,(ix+*)",
    "res 0,(ix+*),b", "res 0,(ix+*),c", "res 0,(ix+*),d", "res 0,(ix+*),e", "res 0,(ix+*),h", "res 0,(ix+*),l", "res 0,(ix+*)", "res 0,(ix+*),a", "res 1,(ix+*),b", "res 1,(ix+*),c", "res 1,(ix+*),d", "res 1,(ix+*),e", "res 1,(ix+*),h", "res 1,(ix+*),l", "res 1,(ix+*)", "res 1,(ix+*),a",
    "res 2,(ix+*),b", "res 2,(ix+*),c", "res 2,(ix+*),d", "res 2,(ix+*),e", "res 2,(ix+*),h", "res 2,(ix+*),l", "res 2,(ix+*)", "res 2,(ix+*),a", "res 3,(ix+*),b", "res 3,(ix+*),c", "res 3,(ix+*),d", "res 3,(ix+*),e", "res 3,(ix+*),h", "res 3,(ix+*),l", "res 3,(ix+*)", "res 3,(ix+*),a",
    "res 4,(ix+*),b", "res 4,(ix+*),c", "res 4,(ix+*),d", "res 4,(ix+*),e", "res 4,(ix+*),h", "res 4,(ix+*),l", "res 4,(ix+*)", "res 4,(ix+*),a", "res 5,(ix+*),b", "res 5,(ix+*),c", "res 5,(ix+*),d", "res 5,(ix+*),e", "res 5,(ix+*),h", "res 5,(ix+*),l", "res 5,(ix+*)", "res 5,(ix+*),a",
    "res 6,(ix+*),b", "res 6,(ix+*),c", "res 6,(ix+*),d", "res 6,(ix+*),e", "res 6,(ix+*),h", "res 6,(ix+*),l", "res 6,(ix+*)", "res 6,(ix+*),a", "res 7,(ix+*),b", "res 7,(ix+*),c", "res 7,(ix+*),d", "res 7,(ix+*),e", "res 7,(ix+*),h", "res 7,(ix+*),l", "res 7,(ix+*)", "res 7,(ix+*),a",
    "set 0,(ix+*),b", "set 0,(ix+*),c", "set 0,(ix+*),d", "set 0,(ix+*),e", "set 0,(ix+*),h", "set 0,(ix+*),l", "set 0,(ix+*)", "set 0,(ix+*),a", "set 1,(ix+*),b", "set 1,(ix+*),c", "set 1,(ix+*),d", "set 1,(ix+*),e", "set 1,(ix+*),h", "set 1,(ix+*),l", "set 1,(ix+*)", "set 1,(ix+*),a",
    "set 2,(ix+*),b", "set 2,(ix+*),c", "set 2,(ix+*),d", "set 2,(ix+*),e", "set 2,(ix+*),h", "set 2,(ix+*),l", "set 2,(ix+*)", "set 2,(ix+*),a", "set 3,(ix+*),b", "set 3,(ix+*),c", "set 3,(ix+*),d", "set 3,(ix+*),e", "set 3,(ix+*),h", "set 3,(ix+*),l", "set 3,(ix+*)", "set 3,(ix+*),a",
    "set 4,(ix+*),b", "set 4,(ix+*),c", "set 4,(ix+*),d", "set 4,(ix+*),e", "set 4,(ix+*),h", "set 4,(ix+*),l", "set 4,(ix+*)", "set 4,(ix+*),a", "set 5,(ix+*),b", "set 5,(ix+*),c", "set 5,(ix+*),d", "set 5,(ix+*),e", "set 5,(ix+*),h", "set 5,(ix+*),l", "set 5,(ix+*)", "set 5,(ix+*),a",
    "set 6,(ix+*),b", "set 6,(ix+*),c", "set 6,(ix+*),d", "set 6,(ix+*),e", "set 6,(ix+*),h", "set 6,(ix+*),l", "set 6,(ix+*)", "set 6,(ix+*),a", "set 7,(ix+*),b", "set 7,(ix+*),c", "set 7,(ix+*),d", "set 7,(ix+*),e", "set 7,(ix+*),h", "set 7,(ix+*),l", "set 7,(ix+*)", "set 7,(ix+*),a"
};

static const char* IY_instructions_names[s_c_instruction_array_size_bytes] = {
    "",            "",            "",            "",            "",            "",            "",             "",            "",         "add iy,bc", "",           "",         "",           "",           "",             "",
    "",            "",            "",            "",            "",            "",            "",             "",            "",         "add iy,de", "",           "",         "",           "",           "",             "",
    "",            "ld iy,**",    "ld (**),iy",  "inc iy",      "inc iyh",     "dec iyh",     "ld iyh,*",     "",            "",         "add iy,iy", "ld iy,(**)", "dec iy",   "inc iyl",    "dec iyl",    "ld iyl,*",     "",
    "",            "",            "",            "",            "inc (iy+*)",  "dec (iy+*)",  "ld (iy+*),*",  "",            "",         "add iy,sp", "",           "",         "",           "",           "",             "",
    "",            "",            "",            "",            "ld b,iyh",    "ld b,iyl",    "ld b,(iy+*)",  "",            "",         "",          "",           "",         "ld c,iyh",   "ld c,iyl",   "ld c,(iy+*)",  "",
    "",            "",            "",            "",            "ld d,iyh",    "ld d,iyl",    "ld d,(iy+*)",  "",            "",         "",          "",           "",         "ld e,iyh",   "ld e,iyl",   "ld e,(iy+*)",  "",
    "ld iyh,b",    "ld iyh,c",    "ld iyh,d",    "ld iyh,e",    "ld iyh,iyh",  "ld iyh,iyl",  "ld h,(iy+*)",  "ld iyh,a",    "ld iyl,b", "ld iyl,c",  "ld iyl,d",   "ld iyl,e", "ld iyl,iyh", "ld iyl,iyl", "ld l,(iy+*)",  "ld iyl,a",
    "ld (iy+*),b", "ld (iy+*),c", "ld (iy+*),d", "ld (iy+*),e", "ld (iy+*),h", "ld (iy+*),l", "",             "ld (iy+*),a", "",         "",          "",           "",         "ld a,iyh",   "ld a,iyl",   "ld a,(iy+*)",  "",
    "",            "",            "",            "",            "add a,iyh",   "add a,iyl",   "add a,(iy+*)", "",            "",         "",          "",           "",         "adc a,iyh",  "adc a,iyl",  "adc a,(iy+*)", "",
    "",            "",            "",            "",            "sub iyh",     "sub iyl",     "sub (iy+*)",   "",            "",         "",          "",           "",         "sbc a,iyh",  "sbc a,iyl",  "sbc a,(iy+*)", "",
    "",            "",            "",            "",            "and iyh",     "and iyl",     "and (iy+*)",   "",            "",         "",          "",           "",         "xor iyh",    "xor iyl",    "xor (iy+*)",   "",
    "",            "",            "",            "",            "or iyh",      "or iyl",      "or (iy+*)",    "",            "",         "",          "",           "",         "cp iyh",     "cp iyl",     "cp (iy+*)",    "",
    "",            "",            "",            "",            "",            "",            "",             "",            "",         "",          "",           "",         "IY BITS",    "",           "",             "",
    "",            "",            "",            "",            "",            "",            "",             "",            "",         "",          "",           "",         "",           "",           "",             "",
    "",            "pop iy",      "",            "ex (sp),iy",  "",            "push iy",     "",             "",            "",         "jp (iy)",   "",           "",         "",           "",           "",             "",
    "",            "",            "",            "",            "",            "",            "",             "",            "",         "ld sp,iy",  "",           "",         "",           "",           "",             "",
};

static const char* IY_bit_instructions_names[s_c_instruction_array_size_bytes] = {
    "rlc (iy+*),b", "rlc (iy+*),c", "rlc (iy+*),d", "rlc (iy+*),e", "rlc (iy+*),h", "rlc (iy+*),l", "rlc (iy+*)", "rlc (iy+*),a", "rrc (iy+*),b", "rrc (iy+*),c", "rrc (iy+*),d", "rrc (iy+*),e", "rrc (iy+*),h", "rrc (iy+*),l", "rrc (iy+*)", "rrc (iy+*),a",
    "rl (iy+*),b", "rl (iy+*),c", "rl (iy+*),d", "rl (iy+*),e", "rl (iy+*),h", "rl (iy+*),l", "rl (iy+*)", "rl (iy+*),a", "rr (iy+*),b", "rr (iy+*),c", "rr (iy+*),d", "rr (iy+*),e", "rr (iy+*),h", "rr (iy+*),l", "rr (iy+*)", "rr (iy+*),a",
    "sla (iy+*),b", "sla (iy+*),c", "sla (iy+*),d", "sla (iy+*),e", "sla (iy+*),h", "sla (iy+*),l", "sla (iy+*)", "sla (iy+*),a", "sra (iy+*),b", "sra (iy+*),c", "sra (iy+*),d", "sra (iy+*),e", "sra (iy+*),h", "sra (iy+*),l", "sra (iy+*)", "sra (iy+*),a",
    "sll (iy+*),b", "sll (iy+*),c", "sll (iy+*),d", "sll (iy+*),e", "sll (iy+*),h", "sll (iy+*),l", "sll (iy+*)", "sll (iy+*),a", "srl (iy+*),b", "srl (iy+*),c", "srl (iy+*),d", "srl (iy+*),e", "srl (iy+*),h", "srl (iy+*),l", "srl (iy+*)", "srl (iy+*),a",
    "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 0,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)", "bit 1,(iy+*)",
    "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 2,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)", "bit 3,(iy+*)",
    "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 4,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)", "bit 5,(iy+*)",
    "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 6,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)", "bit 7,(iy+*)",
    "res 0,(iy+*),b", "res 0,(iy+*),c", "res 0,(iy+*),d", "res 0,(iy+*),e", "res 0,(iy+*),h", "res 0,(iy+*),l", "res 0,(iy+*)", "res 0,(iy+*),a", "res 1,(iy+*),b", "res 1,(iy+*),c", "res 1,(iy+*),d", "res 1,(iy+*),e", "res 1,(iy+*),h", "res 1,(iy+*),l", "res 1,(iy+*)", "res 1,(iy+*),a",
    "res 2,(iy+*),b", "res 2,(iy+*),c", "res 2,(iy+*),d", "res 2,(iy+*),e", "res 2,(iy+*),h", "res 2,(iy+*),l", "res 2,(iy+*)", "res 2,(iy+*),a", "res 3,(iy+*),b", "res 3,(iy+*),c", "res 3,(iy+*),d", "res 3,(iy+*),e", "res 3,(iy+*),h", "res 3,(iy+*),l", "res 3,(iy+*)", "res 3,(iy+*),a",
    "res 4,(iy+*),b", "res 4,(iy+*),c", "res 4,(iy+*),d", "res 4,(iy+*),e", "res 4,(iy+*),h", "res 4,(iy+*),l", "res 4,(iy+*)", "res 4,(iy+*),a", "res 5,(iy+*),b", "res 5,(iy+*),c", "res 5,(iy+*),d", "res 5,(iy+*),e", "res 5,(iy+*),h", "res 5,(iy+*),l", "res 5,(iy+*)", "res 5,(iy+*),a",
    "res 6,(iy+*),b", "res 6,(iy+*),c", "res 6,(iy+*),d", "res 6,(iy+*),e", "res 6,(iy+*),h", "res 6,(iy+*),l", "res 6,(iy+*)", "res 6,(iy+*),a", "res 7,(iy+*),b", "res 7,(iy+*),c", "res 7,(iy+*),d", "res 7,(iy+*),e", "res 7,(iy+*),h", "res 7,(iy+*),l", "res 7,(iy+*)", "res 7,(iy+*),a",
    "set 0,(iy+*),b", "set 0,(iy+*),c", "set 0,(iy+*),d", "set 0,(iy+*),e", "set 0,(iy+*),h", "set 0,(iy+*),l", "set 0,(iy+*)", "set 0,(iy+*),a", "set 1,(iy+*),b", "set 1,(iy+*),c", "set 1,(iy+*),d", "set 1,(iy+*),e", "set 1,(iy+*),h", "set 1,(iy+*),l", "set 1,(iy+*)", "set 1,(iy+*),a",
    "set 2,(iy+*),b", "set 2,(iy+*),c", "set 2,(iy+*),d", "set 2,(iy+*),e", "set 2,(iy+*),h", "set 2,(iy+*),l", "set 2,(iy+*)", "set 2,(iy+*),a", "set 3,(iy+*),b", "set 3,(iy+*),c", "set 3,(iy+*),d", "set 3,(iy+*),e", "set 3,(iy+*),h", "set 3,(iy+*),l", "set 3,(iy+*)", "set 3,(iy+*),a",
    "set 4,(iy+*),b", "set 4,(iy+*),c", "set 4,(iy+*),d", "set 4,(iy+*),e", "set 4,(iy+*),h", "set 4,(iy+*),l", "set 4,(iy+*)", "set 4,(iy+*),a", "set 5,(iy+*),b", "set 5,(iy+*),c", "set 5,(iy+*),d", "set 5,(iy+*),e", "set 5,(iy+*),h", "set 5,(iy+*),l", "set 5,(iy+*)", "set 5,(iy+*),a",
    "set 6,(iy+*),b", "set 6,(iy+*),c", "set 6,(iy+*),d", "set 6,(iy+*),e", "set 6,(iy+*),h", "set 6,(iy+*),l", "set 6,(iy+*)", "set 6,(iy+*),a", "set 7,(iy+*),b", "set 7,(iy+*),c", "set 7,(iy+*),d", "set 7,(iy+*),e", "set 7,(iy+*),h", "set 7,(iy+*),l", "set 7,(iy+*)", "set 7,(iy+*),a"
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

instruction::instruction(const char* label)
:m_instruction_data(get_opcode(label))
,m_valid(is_valid_instruction(label))
{
}

instruction::instruction(const char* label, uint8_t direct)
:m_instruction_data(get_opcode(label))
,m_valid(is_valid_instruction(label, direct))
{
    if(m_valid)
    {
        if(((m_instruction_data & 0xFFFF0000) == 0xDDCB0000)
         ||((m_instruction_data & 0xFFFF0000) == 0xFDCB0000))
        {
            uint16_t direct_cpy = direct;
            m_instruction_data |= (direct_cpy << 8);
        }
        else
        {
            m_instruction_data = (m_instruction_data << 8) | direct;
        }
    }
}

instruction::instruction(const char* label, uint8_t high, uint8_t low)
:m_instruction_data(get_opcode(label))
,m_valid(is_valid_instruction(label, high, low))
{
    if(m_valid)
    {
        uint16_t direct = (low << 8) | high;
        m_instruction_data = (m_instruction_data << 16) | direct;
    }
}

instruction::~instruction()
{
    
}

void instruction::append_bytes(std::vector<uint8_t>& bytes)
{
    if(m_valid)
    {
        uint32_t current_mask = 0xFF000000;
        bool nonzero_byte_added = false;
        for(uint16_t i = 0; i < sizeof(current_mask); i++)
        {
            uint8_t current_byte = static_cast<uint8_t>( (current_mask & m_instruction_data) 
                                                         >> ((sizeof(current_mask) - 1 - i) * 8));
            if(current_byte != 0 || nonzero_byte_added)
            {
                nonzero_byte_added = true;
                bytes.push_back(current_byte);
            }
            current_mask = (current_mask >> 8);
        }
        if(m_instruction_data == 0)
        {
            bytes.push_back(static_cast<uint8_t>(m_instruction_data));
        }
    }
}

bool instruction::is_valid()
{
    return m_valid;
}

std::string instruction::to_string()
{
    std::vector<uint8_t> bytes;
    append_bytes(bytes);
    uint8_t iter = 0;
    std::stringstream sstr;
    if(bytes.size() > 0)
    {
        if(bytes[iter] == 0xED)
        {
            sstr << ED_instruction_names[bytes[++iter]];
        }
        else if(bytes[iter] == 0xCB)
        {
            sstr << CB_instruction_names[bytes[++iter]];
        }
        else if(bytes[iter] == 0xDD)
        {
            if(bytes[++iter] == 0xCB)
            {
                sstr << IX_bit_instructions_names[bytes[++iter]];
            }
            else
            {
                sstr << IX_instructions_names[bytes[iter]];
            }
        }
        else if(bytes[iter] == 0xFD)
        {
            if(bytes[++iter] == 0xCB)
            {
                sstr << IY_bit_instructions_names[bytes[++iter]];
            }
            else
            {
                sstr << IY_instructions_names[bytes[iter]];
            }
        }
        else
        {
            sstr << instruction_names[bytes[iter]];
        }
        sstr << std::hex << " $";
        uint8_t riter = bytes.size() - 1;
        while(iter < riter)
        {
            uint16_t byte = bytes[riter--];
            if(byte < 0x10)
            {
                sstr << "0";
            }
            sstr << static_cast<uint16_t>(byte);
        }
    }
    return sstr.str();
}

uint32_t get_opcode_from_name(const char* p_instruction_name)
{
    uint32_t opcode = 0;
    for(uint16_t i = 0; i < s_c_instruction_array_size_bytes; i++)
    {
        if(0 == strncmp(p_instruction_name, instruction_names[i], s_c_instruction_name_max_length))
        {
            opcode = i;
        }
        else if(0 == strncmp(p_instruction_name, ED_instruction_names[i], s_c_instruction_name_max_length))
        {
            opcode = 0xED00 | (0xFF & i);
        }
        else if(0 == strncmp(p_instruction_name, CB_instruction_names[i], s_c_instruction_name_max_length))
        {
            opcode = 0xCB00 | (0xFF & i);
        }
        else if(0 == strncmp(p_instruction_name, IX_instructions_names[i], s_c_instruction_name_max_length))
        {
            opcode = 0xDD00 | (0xFF & i);
        }
        else if(0 == strncmp(p_instruction_name, IY_instructions_names[i], s_c_instruction_name_max_length))
        {
            opcode = 0xFD00 | (0xFF & i);
        }
        else if(0 == strncmp(p_instruction_name, IX_bit_instructions_names[i], s_c_instruction_name_max_length))
        {
            opcode = 0xDDCB0000 | (0xFF & i);
        }
        else if(0 == strncmp(p_instruction_name, IY_bit_instructions_names[i], s_c_instruction_name_max_length))
        {
            opcode = 0xFDCB0000 | (0xFF & i);
        }
    }
    return opcode;
}

uint16_t get_instruction_size(const char* p_instruction_name)
{
    uint16_t instruction_size = 0;
    uint32_t opcode = get_opcode_from_name(p_instruction_name);
    if( ( (opcode == 0)
       && (0 == strncmp( p_instruction_name
                       , "noop"
                       , s_c_instruction_name_max_length)))
     || (((opcode & 0xFFFFFF00) == 0) && (opcode != 0) ))
    {
        instruction_size = opcode_sizes[opcode];
    }
    else if( ((opcode & 0xFF00) == 0xDD00)
          || ((opcode & 0xFF00) == 0xFD00) )
    {
        instruction_size = opcode_sizes_I[(opcode & 0xFF)];
    }
    else if( ((opcode & 0xFFFF0000) == 0xDDCB0000)
          || ((opcode & 0xFFFF0000) == 0xFDCB0000) )
    {
        instruction_size = opcode_sizes_I[(opcode & 0x00FF0000) >> 16];
    }
    else if((opcode & 0xFF00) == 0xED00)
    {
        instruction_size = opcode_sizes_ED[opcode & 0xFF];
    }
    return instruction_size;
}

uint32_t instruction::get_opcode(const char* p_instruction_name)
{
    return get_opcode_from_name(p_instruction_name);
}

uint16_t instruction::get_indirect_count(const char* label)
{
    std::string s(label);
    return std::count(s.begin(), s.end(), '*');
}

bool instruction::is_valid_instruction(const char* label)
{
    return (m_instruction_data != 0) 
       ||  (strncmp(label, "noop", s_c_instruction_name_max_length) == 0);
}

bool instruction::is_valid_instruction(const char* label, uint8_t direct)
{
    return is_valid_instruction(label)
        && get_indirect_count(label) == 1;
}

bool instruction::is_valid_instruction(const char* label, uint8_t, uint8_t)
{
    return is_valid_instruction(label)
        && get_indirect_count(label) == 2;
}

struct Instruction_ROM::Impl
{
public:
    Data_bus_RAM m_instruction_data;
    std::map<std::string, uint16_t> m_symbols;
    std::vector<std::string> m_failed_commands;
    std::map<uint16_t, std::pair<std::string, std::string> > m_undefined_symbol_location;
    std::map<std::string, std::vector<uint16_t> > m_undefined_symbol_list;
    uint16_t m_instruction_size;
    uint16_t m_fill_size;
    uint16_t m_opcode_instructions;
    
    Impl(uint16_t instruction_size)
    : m_instruction_data(instruction_size)
    , m_instruction_size(instruction_size)
    , m_fill_size(0)
    , m_opcode_instructions(0)
    {
    }

    virtual ~Impl(){}
    
    void parse_instruction_list(std::vector<uint16_t>& instruction_list)
    {
        uint16_t current_address = 0;
        while(current_address < m_fill_size)
        {
            instruction_list.push_back(current_address);
            uint8_t opcode = m_instruction_data.get_data(current_address);
            switch(opcode)
            {
                case 0xDD:
                case 0xFD:
                    ++current_address;
                    current_address 
                        += opcode_sizes_I[m_instruction_data
                                            .get_data(current_address)];
                    break;
                case 0xED:
                    ++current_address;
                    current_address 
                        += opcode_sizes_ED[m_instruction_data
                                            .get_data(current_address)];
                    break;
                default:
                    current_address += opcode_sizes[opcode];
                    break;
            }
        }
    }
    
    uint16_t add_instruction_binary_data(instruction& new_instruction, uint16_t start_address)
    {
        uint16_t current_address = start_address;
        if(new_instruction.is_valid())
        {
            std::vector<uint8_t> bytes;
            new_instruction.append_bytes(bytes);
            for(uint32_t i = 0; i < bytes.size(); i++)
            {
                m_instruction_data.set_data(current_address++, bytes[i]);
            }
        }
        else
        {
            m_failed_commands.push_back(new_instruction.to_string());
        }
        return current_address;
    }
    
    void add_instruction_binary_data(instruction& new_instruction)
    {
        m_fill_size = add_instruction_binary_data(new_instruction, m_fill_size);
    }
    
    std::string address_to_string(uint16_t address)
    {
        std::string ret_string = "";
        std::string instruction_string = "";
        uint8_t opcode = m_instruction_data.get_data(address);
        uint16_t size = 0;
        uint16_t direct_addr = 0;
        if(opcode == 0xCB)
        {
            instruction_string = CB_instruction_names[m_instruction_data.get_data(address + 1)];
            size = opcode_sizes[opcode];
            direct_addr = 0;
        }
        else if(opcode == 0xDD)
        {
            if(m_instruction_data.get_data(address + 1) == 0xCB)
            {
                instruction_string = IX_bit_instructions_names[m_instruction_data.get_data(address + 3)];
            }
            else
            {
                instruction_string = IX_bit_instructions_names[m_instruction_data.get_data(address + 1)];
            }
            size = opcode_sizes_I[m_instruction_data.get_data(address + 1)];
            direct_addr = address + 2;
        }
        else if(opcode == 0xFD)
        {
            if(m_instruction_data.get_data(address + 1) == 0xCB)
            {
                instruction_string = IY_bit_instructions_names[m_instruction_data.get_data(address + 3)];
            }
            else
            {
                instruction_string = IY_bit_instructions_names[m_instruction_data.get_data(address + 1)];
            }
            size = opcode_sizes_I[m_instruction_data.get_data(address + 1)];
            direct_addr = address + 2;
        }
        else if(opcode == 0xED)
        {
            instruction_string = ED_instruction_names[m_instruction_data.get_data(address + 1)];
            size = opcode_sizes_ED[m_instruction_data.get_data(address + 1)];
            direct_addr = address + 2;
        }
        else
        {
            instruction_string = instruction_names[opcode];
            size = opcode_sizes[m_instruction_data.get_data(address + 1)];
            direct_addr = address + 1;
        }
        if(size == 1)
        {
            instruction single_byte(instruction_string.c_str());
            ret_string = single_byte.to_string();
        }
        else if(size == 2)
        {
            instruction double_byte(instruction_string.c_str(), m_instruction_data.get_data(direct_addr));
            ret_string = double_byte.to_string();
        }
        else if(size == 3)
        {
            instruction triple_byte( instruction_string.c_str()
                                   , m_instruction_data.get_data(direct_addr)
                                   , m_instruction_data.get_data(direct_addr + 1));

            ret_string = triple_byte.to_string();
        }
        return ret_string;
    }
};

Instruction_ROM::Instruction_ROM(uint16_t instruction_size)
    :m_p_impl(new Impl(instruction_size))
{
}

Instruction_ROM::~Instruction_ROM()
{
    delete m_p_impl;
    m_p_impl = 0;
}

uint16_t Instruction_ROM::get_section_size() const
{
    return m_p_impl->m_instruction_size;
}

uint8_t Instruction_ROM::get_data(uint16_t address)
{
    uint8_t data = 0;
    if( (address < m_p_impl->m_instruction_size)
     && (address < m_p_impl->m_instruction_data.get_section_size()))
    {
        data = m_p_impl->m_instruction_data.get_data(address);
    }
    return data;
}

void Instruction_ROM::set_data(uint16_t, uint8_t)
{
    
}

void Instruction_ROM::parse_instruction_list(std::vector<uint16_t>& instruction_list)
{
    m_p_impl->parse_instruction_list(instruction_list);
}
    
Error Instruction_ROM::read_binary(const char* p_filename)
{
    Error return_error = s_c_error_none;
    uint8_t array [m_p_impl->m_instruction_data.get_section_size()];
    uint8_t* p_iterator = array;
    int32_t fd = open(p_filename, O_RDONLY);
    uint8_t continue_reading = true;
    memset(array, 0, sizeof(array));
    uint16_t total_read_bytes = 0;
    while(continue_reading == true)
    {
        ssize_t read_bytes = read(fd, p_iterator, sizeof(array));
        printf("Read %ld bytes\n", read_bytes);
        if(read_bytes <= 0)
        {
            continue_reading = false;
        }
        total_read_bytes += read_bytes;
        p_iterator += read_bytes;
        if(total_read_bytes > sizeof(array))
        {
            return_error = s_c_error_too_large_instruction_size;
        }
    }
    close(fd);
    m_p_impl->m_fill_size = total_read_bytes;
    if(return_error == s_c_error_none)
    {
        for(uint16_t i = 0; i < total_read_bytes; i++)
        {
            m_p_impl->m_instruction_data.set_data(i, array[i]);
        }
    }
    return return_error;
}

std::string Instruction_ROM::address_to_string(uint16_t address)
{
    return m_p_impl->address_to_string(address);
}

Instruction_ROM& Instruction_ROM::add_instruction(const char* label)
{
    instruction new_instruction(label);
    m_p_impl->add_instruction_binary_data(new_instruction);
    return *this;
}

Instruction_ROM& Instruction_ROM::add_instruction(const char* label, uint8_t direct)
{
    instruction new_instruction(label, direct);
    m_p_impl->add_instruction_binary_data(new_instruction);
    return *this;
}

Instruction_ROM& Instruction_ROM::add_instruction(const char* label, uint8_t high, uint8_t low)
{
    instruction new_instruction(label, high, low);
    m_p_impl->add_instruction_binary_data(new_instruction);
    return *this;
}

Instruction_ROM& Instruction_ROM::add_instruction_with_symbol(const char* label, const char* identifier)
{
    if(m_p_impl->m_symbols.find(identifier) == m_p_impl->m_symbols.end())
    {
        uint16_t instruction_size = get_instruction_size(label);
        if(instruction_size > 0)
        {
            if( m_p_impl->m_undefined_symbol_list.find(identifier)
             == m_p_impl->m_undefined_symbol_list.end())
            {
                m_p_impl->m_undefined_symbol_list.emplace(identifier,std::vector<uint16_t>());
            }
            m_p_impl->m_undefined_symbol_list[identifier]
                .push_back(m_p_impl->m_fill_size);

            m_p_impl->m_undefined_symbol_location
                .emplace( m_p_impl->m_fill_size
                        , std::make_pair(label, identifier));
            m_p_impl->m_fill_size += instruction_size;
        }
        else
        {
            std::string failed_instruction = label;
            failed_instruction += " ";
            failed_instruction += identifier;
            m_p_impl->m_failed_commands.push_back(failed_instruction);
        }
    }
    else
    {
        uint16_t value = m_p_impl->m_symbols[identifier];
        std::string instruction = label;
        if(std::count(instruction.begin(), instruction.end(), '*') == 1)
        {
            (void)add_instruction(label, value);
        }
        else if(std::count(instruction.begin(), instruction.end(), '*') == 2)
        {
            uint8_t high = (value & 0xFF00) >> 8;
            uint8_t low  = value & 0xFF;
            (void)add_instruction(label, high, low);
        }
        else
        {
            std::string failed_instruction = label;
            failed_instruction += " ";
            failed_instruction += identifier;
            m_p_impl->m_failed_commands.push_back(failed_instruction);
        }
    }
    return *this;
}

std::vector<std::string>& Instruction_ROM::get_failed_instructions_list()
{
    return m_p_impl->m_failed_commands;
}

Instruction_ROM& Instruction_ROM::add_symbolic_value(const char* identifier, uint16_t value)
{
    if(m_p_impl->m_symbols.find(identifier) == m_p_impl->m_symbols.end())
    {
        m_p_impl->m_symbols.emplace(identifier, value);
        if( m_p_impl->m_undefined_symbol_list.find(identifier)
         != m_p_impl->m_undefined_symbol_list.end())
        {
            std::vector<uint16_t>& list 
                = m_p_impl->m_undefined_symbol_list[identifier];
            for(size_t i = 0; i < list.size(); i++)
            {
                if( m_p_impl->m_undefined_symbol_location.find(list[i])
                 != m_p_impl->m_undefined_symbol_location.end())
                {
                    std::pair<std::string, std::string>& pair_string
                        = m_p_impl->m_undefined_symbol_location[list[i]];
                    
                    std::string in_name = pair_string.first;
                    if(std::count(in_name.begin(), in_name.end(), '*') == 1)
                    {
                        instruction instruct(in_name.c_str(), value);
                        m_p_impl->add_instruction_binary_data(instruct, list[i]);
                    }
                    else if(std::count(in_name.begin(), in_name.end(), '*') == 2)
                    {
                        uint8_t high = (value & 0xFF00) >> 8;
                        uint8_t low  = value & 0xFF;
                        instruction instruct(in_name.c_str(), high, low);
                        m_p_impl->add_instruction_binary_data(instruct, list[i]);
                    }
                }
            }
        }
    }
    return *this;
}

Instruction_ROM& Instruction_ROM::add_symbol_at_current_fill(const char* identifier)
{
    return add_symbolic_value(identifier, m_p_impl->m_fill_size);
}

}

