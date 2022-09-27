#pragma once

#include <stdint.h>

#include "gb.h"

/** Instructions (see https://rgbds.gbdev.io/docs)
 * Naming convention:
 * r8/16: 8/16-bit register
 * lr: low 8-bit register
 * hr: high 8-bit register
 * d8/16: 8/16-bit data (immediate)
 * ..i = indirect access (pointer [..])
 * cc = condition code (z, nz, c, nc)
 */
static void nop(gameboy_t *gb);

// Load instructions
static void ld_r8_r8(gameboy_t *gb, uint8_t opcode);
static void ld_r8_d8(gameboy_t *gb, uint8_t opcode);
static void ld_r16_d16(uint16_t reg);
static void ld_hli_r8(uint8_t reg);
static void ld_hli_d8();
static void ld_r8_hli(gameboy_t *gb, uint8_t opcode);