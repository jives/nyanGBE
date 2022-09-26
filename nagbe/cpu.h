#pragma once
#include <stdint.h>

typedef enum flags
{
    // Zero flag
    z = 16,
    // Half-carry flag
    h = 32,
    // Subtraction flag
    n = 64,
    // Carry flag
    c = 128
} flags_t;

typedef struct registers
{
    union
    {
        struct
        {
            // assumes litte endian
            flags_t f;
            uint8_t a;
        };
        uint16_t af;
    };

    union
    {
        struct
        {
            // assumes litte endian
            uint8_t c, b;
        };
        uint16_t bc;
    };

    union
    {
        struct
        {
            // assumes litte endian
            uint8_t e, d;
        };
        uint16_t de;
    };
    
    union
    {
        struct
        {
            // assumes litte endian
            uint8_t l, h;
        };
        uint16_t hl;
    };

    uint16_t SP;
    uint16_t PC;
} registers_t;

registers_t registers;

#define FLAGS_SET(x) (registers.f |= (x))
#define FLAGS_TEST(x) (registers.f & (x))
#define FLAGS_CLEAR(x) (registers.f &= ~(x))