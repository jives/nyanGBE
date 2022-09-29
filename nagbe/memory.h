#pragma once

#include <stdint.h>

uint8_t mem_read(gameboy_t *gb, uint16_t loc);
uint8_t mem_write(gameboy_t *gb, uint16_t loc, uint8_t data);