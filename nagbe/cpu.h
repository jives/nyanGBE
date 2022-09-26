#pragma once

#include <stdint.h>
#include "gb.h"

/* Flags */
typedef enum __attribute__((packed)) flags
{
    z = 1 << 4, // Zero flag
    h = 1 << 5, // Half-carry flag
    n = 1 << 6, // Subtraction flag
    c = 1 << 7  // Carry flag
} flags_t;

#define FLAGS_SET(x) (registers.f |= (x))
#define FLAGS_TEST(x) (registers.f & (x))
#define FLAGS_CLEAR(x) (registers.f &= ~(x))

/* Registers */
typedef union registers
{
    uint16_t registers[GB_NUM_REG_16_BIT];
    struct
    {
        uint16_t af;
        uint16_t bc;
        uint16_t de;
        uint16_t hl;
        uint16_t sp;
        uint16_t pc;
    };
    struct
    {
        flags_t f;
        uint8_t a;
        uint8_t c;
        uint8_t b;
        uint8_t e;
        uint8_t d;
        uint8_t l;
        uint8_t h;
    };
    
} registers_t;

registers_t registers;



/** Instructions (see https://rgbds.gbdev.io/docs)
 * Naming convention:
 * r8/16: 8/16-bit register
 * lr: low 8-bit register
 * hr: high 8-bit register
 * d8/16: 8/16-bit data (immediate)
 * ..i = indirect access (pointer [..])
 * cc = condition code (z, nz, c, nc)
 */
static void nop();

// Load instructions
static void ld_r8_r8(uint8_t dst, uint8_t src);
static void ld_r8_d8(uint8_t reg);
static void ld_r16_d16(uint16_t reg);
static void ld_hli_r8(uint8_t reg);
static void ld_hli_d8();
static void ld_r8_hli(uint8_t reg);