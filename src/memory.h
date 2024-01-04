#pragma once

#include <stdint.h>
#include "gb.h"

uint8_t mem_read_byte(gameboy_t *gb, uint16_t loc);
void mem_write_byte(gameboy_t *gb, uint16_t loc, uint8_t data);