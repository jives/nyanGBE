#pragma once

#include <stdint.h>
#include "gb.h"

void execute_opcode(gameboy_t *gb, uint8_t opcode);


static void nop(gameboy_t *gb);

// Load instructions
static void ld_r8_r8(gameboy_t *gb, uint8_t opcode);
static void ld_r8_d8(gameboy_t *gb, uint8_t opcode);
static void ld_r16_d16(gameboy_t *gb, uint8_t opcode);
static void ld_hli_r8(gameboy_t *gb, uint8_t opcode);
static void ld_hli_d8(gameboy_t *gb);
static void ld_r8_hli(gameboy_t *gb, uint8_t opcode);
static void ld_r16i_a(gameboy_t *gb, uint8_t opcode);
static void ldh_d16i_a(gameboy_t *gb);
static void ldh_ci_a(gameboy_t *gb);
static void ld_a_r16i(gameboy_t *gb, uint8_t opcode);
static void ld_a_d16i(gameboy_t *gb);
static void ldh_a_d16i(gameboy_t *gb);
static void ldh_a_ci(gameboy_t *gb);