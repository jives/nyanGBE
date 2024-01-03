#pragma once

#include <stdint.h>
#include "gb.h"

void execute_opcode(gameboy_t *gb, uint8_t opcode);
void execute_cb_opcode(gameboy_t *gb, uint8_t opcode);