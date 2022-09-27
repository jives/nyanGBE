#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "opcodes.h"

void execute_opcode(uint8_t opcode)
{
    switch(opcode)
    {
        case OP_NOP: nop(&gb); break;

        /* 8-bit load instructions */
        case OP_LD_B_u8: ld_r8_d8(&gb, opcode); break;
        case OP_LD_C_u8: ld_r8_d8(&gb, opcode); break;
        case OP_LD_D_u8: ld_r8_d8(&gb, opcode); break;
        case OP_LD_E_u8: ld_r8_d8(&gb, opcode); break;
        case OP_LD_H_u8: ld_r8_d8(&gb, opcode); break;
        case OP_LD_L_u8: ld_r8_d8(&gb, opcode); break;

        case OP_LD_B_A: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_B: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_C: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_D: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_E: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_H: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_L: ld_r8_r8(&gb, opcode); break;
        case OP_LD_B_HLi: ld_r8_hli(&gb, opcode); break;

        case OP_LD_C_A: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_B: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_C: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_D: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_E: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_H: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_L: ld_r8_r8(&gb, opcode); break;
        case OP_LD_C_HLi: ld_r8_hli(&gb, opcode); break;

        case OP_LD_D_A: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_B: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_C: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_D: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_E: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_H: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_L: ld_r8_r8(&gb, opcode); break;
        case OP_LD_D_HLi: ld_r8_hli(&gb, opcode); break;

        case OP_LD_E_A: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_B: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_C: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_D: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_E: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_H: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_L: ld_r8_r8(&gb, opcode); break;
        case OP_LD_E_HLi: ld_r8_hli(&gb, opcode); break;

        case OP_LD_H_A: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_B: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_C: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_D: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_E: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_H: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_L: ld_r8_r8(&gb, opcode); break;
        case OP_LD_H_HLi: ld_r8_hli(&gb, opcode); break;

        case OP_LD_L_A: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_B: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_C: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_D: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_E: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_H: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_L: ld_r8_r8(&gb, opcode); break;
        case OP_LD_L_HLi: ld_r8_hli(&gb, opcode); break;

        /* 16-bit load instructions */
        // ...

        default:
            printf("Unhandled opcode: %x", opcode);
            assert(!"Unhandled opcode");
            break;
    }
}

/**
 * @brief 
 * 
 * @param regnum 
 * @return int 
 */
static inline int regmap(const int regnum)
{
    switch(regnum)
    {
        case 0: return 3;
        case 1: return 2;
        case 2: return 5;
        case 3: return 4;
        case 4: return 7;
        case 5: return 6;
        case 6: return 0;
        case 7: return 1;
    }
}

static void nop(gameboy_t *gb)
{
    gb->m_cycles += 1;
}

static void ld_r8_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    uint8_t src = regmap(opcode & 0b111);
    gb->registers[dst] = gb->registers[src];
    gb->m_cycles += 1;
}

static void ld_r8_d8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    gb->registers[dst] = mem_read(gb->pc++);
    gb->m_cycles += 2;
}

static void ld_r8_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    gb->registers[dst] = mem_read(gb->hl);
    gb->m_cycles += 2;
}
