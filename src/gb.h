#pragma once

#include <stdint.h>

/* Constants */
#define GB_NUM_REG_8_BIT 8
#define GB_NUM_REG_16_BIT 6

/* Flags */
typedef enum __attribute__((packed)) flags
{
    z = 1 << 4, // Zero flag
    h = 1 << 5, // Half-carry flag
    n = 1 << 6, // Subtraction flag
    c = 1 << 7  // Carry flag
} flags_t;

/* Main GB state */
typedef struct gb
{
    union
    {
        uint8_t registers[GB_NUM_REG_8_BIT];
        //uint16_t registers[GB_NUM_REG_16_BIT];
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
    };
    uint16_t m_cycles;
} gameboy_t;

gameboy_t gb;