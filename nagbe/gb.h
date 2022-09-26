#pragma once

#include <stdint.h>

#define GB_NUM_REG_16_BIT 6

typedef struct gb
{
    uint16_t m_cycles;
} gameboy_t;