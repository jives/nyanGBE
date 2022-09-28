#pragma once

#include <stdint.h>

uint8_t mem_cycle_read(gameboy_t *gb, uint16_t loc);
uint8_t mem_cycle_write(gameboy_t *gb, uint16_t loc, uint8_t data);