#pragma once

#include <stdint.h>

uint8_t mem_read_byte(gameboy_t *gb, uint16_t loc);
uint8_t mem_write_byte(gameboy_t *gb, uint16_t loc, uint8_t data);