#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "memory.h"

/**
 * @brief Read byte from memory
 *
 * Does not advance cycles!
 *
 * @param gb gameboy state struct
 * @param loc 16-bit memory address to read from
 * @return uint8_t byte at memory address
 */
uint8_t mem_read_byte(gameboy_t *gb, uint16_t loc)
{
    if (loc < 0x8000)
    {
        return gb->rom[loc];
    }
    else if (loc == 0xFF44)
    {
        // LY register read debug implementation
        // to work with gameboy doctor logs
        return 0x90;
    }
    else
    {
        return gb->ram[loc - 0x8000];
    }
}

/**
 * @brief Read byte from memory
 *
 * Does not advance cycles!
 *
 * @param gb gameboy state struct
 * @param loc 16-bit memory address to write to
 * @param data byte to write
 */
void mem_write_byte(gameboy_t *gb, uint16_t loc, uint8_t data)
{
    if (loc < 0x8000)
        // Protect ROM from writes
        return;

    gb->ram[loc - 0x8000] = data;
}