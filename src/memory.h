#pragma once

#include <stdint.h>

struct memory_s
{
    uint8_t rom[0x8000];
    uint8_t ram[0x8000];
};

struct gb_s;

uint8_t mem_read_byte(struct gb_s *gb, uint16_t loc);
void mem_write_byte(struct gb_s *gb, uint16_t loc, uint8_t data);