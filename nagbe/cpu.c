#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "opcodes.h"

void execute_opcode(uint8_t opcode)
{
    switch(opcode)
    {
        case OP_NOP: cpu_noop(); break;

        /* 8-bit load instructions */
        case OP_LD_B_u8: ld_r8_d8(registers.b); break;
        case OP_LD_C_u8: ld_r8_d8(registers.c); break;
        case OP_LD_D_u8: ld_r8_d8(registers.d); break;
        case OP_LD_E_u8: ld_r8_d8(registers.e); break;
        case OP_LD_H_u8: ld_r8_d8(registers.h); break;
        case OP_LD_L_u8: ld_r8_d8(registers.l); break;

        case OP_LD_B_A: ld_r8_r8(registers.b, registers.a); break;
        case OP_LD_B_B: ld_r8_r8(registers.b, registers.b); break;
        case OP_LD_B_C: ld_r8_r8(registers.b, registers.c); break;
        case OP_LD_B_D: ld_r8_r8(registers.b, registers.d); break;
        case OP_LD_B_E: ld_r8_r8(registers.b, registers.e); break;
        case OP_LD_B_H: ld_r8_r8(registers.b, registers.h); break;
        case OP_LD_B_L: ld_r8_r8(registers.b, registers.l); break;
        case OP_LD_B_HLi: ld_r8_hli(registers.b); break;

        case OP_LD_C_A: ld_r8_r8(registers.c, registers.a); break;
        case OP_LD_C_B: ld_r8_r8(registers.c, registers.b); break;
        case OP_LD_C_C: ld_r8_r8(registers.c, registers.c); break;
        case OP_LD_C_D: ld_r8_r8(registers.c, registers.d); break;
        case OP_LD_C_E: ld_r8_r8(registers.c, registers.e); break;
        case OP_LD_C_H: ld_r8_r8(registers.c, registers.h); break;
        case OP_LD_C_L: ld_r8_r8(registers.c, registers.l); break;
        case OP_LD_C_HLi: ld_r8_hli(registers.c); break;

        case OP_LD_D_A: ld_r8_r8(registers.d, registers.a); break;
        case OP_LD_D_B: ld_r8_r8(registers.d, registers.b); break;
        case OP_LD_D_C: ld_r8_r8(registers.d, registers.c); break;
        case OP_LD_D_D: ld_r8_r8(registers.d, registers.d); break;
        case OP_LD_D_E: ld_r8_r8(registers.d, registers.e); break;
        case OP_LD_D_H: ld_r8_r8(registers.d, registers.h); break;
        case OP_LD_D_L: ld_r8_r8(registers.d, registers.l); break;
        case OP_LD_D_HLi: ld_r8_hli(registers.d); break;

        case OP_LD_E_A: ld_r8_r8(registers.e, registers.a); break;
        case OP_LD_E_B: ld_r8_r8(registers.e, registers.b); break;
        case OP_LD_E_C: ld_r8_r8(registers.e, registers.c); break;
        case OP_LD_E_D: ld_r8_r8(registers.e, registers.d); break;
        case OP_LD_E_E: ld_r8_r8(registers.e, registers.e); break;
        case OP_LD_E_H: ld_r8_r8(registers.e, registers.h); break;
        case OP_LD_E_L: ld_r8_r8(registers.e, registers.l); break;
        case OP_LD_E_HLi: ld_r8_hli(registers.e); break;

        case OP_LD_H_A: ld_r8_r8(registers.h, registers.a); break;
        case OP_LD_H_B: ld_r8_r8(registers.h, registers.b); break;
        case OP_LD_H_C: ld_r8_r8(registers.h, registers.c); break;
        case OP_LD_H_D: ld_r8_r8(registers.h, registers.d); break;
        case OP_LD_H_E: ld_r8_r8(registers.h, registers.e); break;
        case OP_LD_H_H: ld_r8_r8(registers.h, registers.h); break;
        case OP_LD_H_L: ld_r8_r8(registers.h, registers.l); break;
        case OP_LD_H_HLi: ld_r8_hli(registers.h); break;

        case OP_LD_L_A: ld_r8_r8(registers.l, registers.a); break;
        case OP_LD_L_B: ld_r8_r8(registers.l, registers.b); break;
        case OP_LD_L_C: ld_r8_r8(registers.l, registers.c); break;
        case OP_LD_L_D: ld_r8_r8(registers.l, registers.d); break;
        case OP_LD_L_E: ld_r8_r8(registers.l, registers.e); break;
        case OP_LD_L_H: ld_r8_r8(registers.l, registers.h); break;
        case OP_LD_L_L: ld_r8_r8(registers.l, registers.l); break;
        case OP_LD_L_HLi: ld_r8_hli(registers.l); break;

        /* 16-bit load instructions */
        


        default:
            printf("Unhandled opcode: %x", opcode);
            assert(!"Unhandled opcode");
            break;
    }
}

static void ld_r8_r8(gameboy_t *gb, uint8_t dst, uint8_t src)
{
    gb->m_cycles += 1;
    dst = src;
}