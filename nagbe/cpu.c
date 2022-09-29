#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "memory.h"
#include "opcodes.h"

void execute_opcode(gameboy_t *gb, uint8_t opcode)
{
    switch(opcode)
    {
        case OP_NOP:
        {
            nop(gb);
            break;
        }

        case OP_LD_B_u8: case OP_LD_C_u8: case OP_LD_D_u8:
        case OP_LD_E_u8: case OP_LD_H_u8: case OP_LD_L_u8: case OP_LD_A_u8:
        {
            ld_r8_d8(gb, opcode);
            break;
        }

        case OP_LD_BC_u16: case OP_LD_DE_u16:
        case OP_LD_HL_u16: case OP_LD_SP_u16:
        {
            ld_r16_d16(gb, opcode);
            break;
        }

        case OP_LD_B_B: case OP_LD_B_C: case OP_LD_B_D:
        case OP_LD_B_E: case OP_LD_B_H: case OP_LD_B_L: case OP_LD_B_A:
        case OP_LD_C_B: case OP_LD_C_C: case OP_LD_C_D:
        case OP_LD_C_E: case OP_LD_C_H: case OP_LD_C_L: case OP_LD_C_A:
        case OP_LD_D_B: case OP_LD_D_C: case OP_LD_D_D:
        case OP_LD_D_E: case OP_LD_D_H: case OP_LD_D_L: case OP_LD_D_A:
        case OP_LD_E_B: case OP_LD_E_C: case OP_LD_E_D:
        case OP_LD_E_E: case OP_LD_E_H: case OP_LD_E_L: case OP_LD_E_A:
        case OP_LD_H_B: case OP_LD_H_C: case OP_LD_H_D:
        case OP_LD_H_E: case OP_LD_H_H: case OP_LD_H_L: case OP_LD_H_A:
        case OP_LD_L_B: case OP_LD_L_C: case OP_LD_L_D:
        case OP_LD_L_E: case OP_LD_L_H: case OP_LD_L_L: case OP_LD_L_A:
        {
            ld_r8_r8(gb, opcode);
            break;
        }

        case OP_LD_HLi_B: case OP_LD_HLi_C: case OP_LD_HLi_D:
        case OP_LD_HLi_E: case OP_LD_HLi_H: case OP_LD_HLi_L: case OP_LD_HLi_A:
        {
            ld_hli_r8(gb, opcode);
            break;
        }

        case OP_LD_HLi_u8: ld_hli_d8(gb); break;

        case OP_LD_B_HLi: case OP_LD_C_HLi: case OP_LD_D_HLi:
        case OP_LD_E_HLi: case OP_LD_H_HLi: case OP_LD_L_HLi: case OP_LD_A_HLi:
        {
            ld_r8_hli(gb, opcode);
            break;
        }

        // ...

        default:
        {
            printf("Unhandled opcode: %02X", opcode);
            assert(!"Unhandled opcode");
            break;
        }
    }
}

/**
 * @brief Maps optable register number to array register index
 * 
 * The GB optable assumes the following register order
 *      B C D E H L (HL) A
 *      0 1 2 3 4 5  6   7
 * but our register array in memory is ordered according to
 * the actual GB layout (in big endian), so
 *      F A C B E D L H
 *      0 1 2 3 4 5 6 7
 * This function translates those two mappings and handles
 * indices that are out of bounds as well as the (HL) case.
 * 
 * @param regnum optable register number
 * @return uint8_t memory array register index
 */
static inline uint8_t regmap(const uint8_t regnum)
{
    switch(regnum)
    {
        case 0: return 3;
        case 1: return 2;
        case 2: return 5;
        case 3: return 4;
        case 4: return 7;
        case 5: return 6;
        // case 6 would be (HL), this is handled by
        // ld_r8_hli and ld_hli_r8 instead
        case 7: return 1;
        default:
            printf("Unsupported register number %d", regnum);
            assert(!"Unsupported register number");
            break;
    }
}

/** Instructions (see https://rgbds.gbdev.io/docs)
 * Naming convention:
 * r8/16: 8/16-bit register
 * d8/16: 8/16-bit data (immediate)
 * ..i: indirect access (pointer (..))
 * ..p: increment register ..+
 * ..m: decrement register ..-
 * cc: condition code (z, nz, c, nc)
 */

static void nop(gameboy_t *gb)
{
    gb->m_cycles += 1;
}

// Load Instructions
static void ld_r8_r8(gameboy_t *gb, uint8_t opcode)
{
    // May be more performant if we don't decode the register numbers
    // Generate functions using macros?
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    uint8_t src = regmap(opcode & 0b111);
    gb->registers[dst] = gb->registers[src];
    gb->m_cycles += 1;
}

static void ld_r8_d8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    gb->registers[dst] = mem_read(gb, gb->pc++);
    gb->m_cycles += 2;
}

static void ld_r16_d16(gameboy_t *gb, uint8_t opcode)
{
    uint16_t data = mem_read(gb, gb->pc++);

    switch(opcode)
    {
    case 0x01:
        gb->bc = data;
        break;
    case 0x11:
        gb->de = data;
        break;
    case 0x21:
        gb->hl = data;
        break;
    case 0x31:
        gb->sp = data;
        break;
    
    default:
        printf("Unsupported opcode %02X for LD r16,n16", opcode);
        assert(!"Unsupported opcode for LD r16,n16");
        break;
    }

    gb->m_cycles += 3;
}

static void ld_hli_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    mem_write(gb, gb->hl, gb->registers[src]);
    gb->m_cycles += 2;
}

static void ld_hli_d8(gameboy_t *gb)
{
    uint16_t data = mem_read(gb, gb->pc++);
    mem_cycle_write(gb, gb->hl, data);
    gb->m_cycles += 3;
}

static void ld_r8_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    gb->registers[dst] = mem_read(gb, gb->hl);
    gb->m_cycles += 2;
}

static void ld_r16i_a(gameboy_t *gb, uint8_t opcode)
{
    uint16_t dst;

    switch(opcode)
    {
    case 0x02:
        dst = gb->bc;
        break;
    case 0x12:
        dst = gb->de;
        break;
    
    default:
        printf("Unsupported opcode %02X for LD (r16),A", opcode);
        assert(!"Unsupported opcode for LD (r16),A");
        break;
    }

    mem_write(gb, dst, gb->a);
    gb->m_cycles += 2;
}

static void ldh_d16i_a(gameboy_t *gb)
{
    uint8_t src_lo = mem_read(gb, gb->pc++);
    mem_write(gb, 0xFF00+src_lo, gb->a);
    gb->m_cycles += 3;
}

static void ldh_ci_a(gameboy_t *gb)
{
    mem_write(gb, 0xFF00+gb->c, gb->a);
    gb->m_cycles += 2;
}

static void ld_a_r16i(gameboy_t *gb, uint8_t opcode)
{
    uint16_t src;

    switch(opcode)
    {
    case 0x02:
        src = gb->bc;
        break;
    case 0x12:
        src = gb->de;
        break;
    
    default:
        printf("Unsupported opcode %02X for LD A,(r16)", opcode);
        assert(!"Unsupported opcode for LD A,(r16)");
        break;
    }

    gb->a = mem_read(gb, src);
    gb->m_cycles += 2;
}

static void ld_a_d16i(gameboy_t *gb)
{
    uint8_t src_lo = mem_read(gb, gb->pc++);
    uint8_t src_hi = mem_read(gb, gb->pc++);
    gb->a = mem_read(gb, src_lo + (src_hi << 8));
    gb->m_cycles += 4;
}

static void ldh_a_d16i(gameboy_t *gb)
{
    uint8_t src_lo = mem_read(gb, gb->pc++);
    gb->a = mem_read(gb, 0xFF00+src_lo);
    gb->m_cycles += 3;
}

static void ldh_a_ci(gameboy_t *gb)
{
    gb->a = mem_read(gb, 0xFF00+gb->c);
    gb->m_cycles += 2;
}

static void ld_hlpi_a(gameboy_t *gb)
{
    mem_write(gb, gb->hl, gb->a);
    gb->hl++;
    gb->m_cycles += 2;
}

static void ld_hlmi_a(gameboy_t *gb)
{
    mem_write(gb, gb->hl, gb->a);
    gb->hl--;
    gb->m_cycles += 2;
}

static void ld_a_hlpi(gameboy_t *gb)
{
    gb->a = mem_read(gb, gb->hl);
    gb->hl++;
    gb->m_cycles += 2;
}

static void ld_a_hlmi(gameboy_t *gb)
{
    gb->a = mem_read(gb, gb->hl);
    gb->hl--;
    gb->m_cycles += 2;
}

static void ld_sp_d16(gameboy_t *gb)
{
    gb->sp = mem_read(gb, gb->pc++) + (mem_read(gb, gb->pc++) << 8);
    gb->m_cycles += 3;
}

static void ld_d16_sp(gameboy_t *gb)
{
    uint16_t addr = mem_read(gb, gb->pc++) + (mem_read(gb, gb->pc++) << 8);
    mem_write(gb, addr, gb->sp & 0xFF);
    mem_write(gb, addr + 1, gb->sp >> 8);
    gb->m_cycles += 5;
}

static void ld_hl_sp_i8(gameboy_t *gb)
{
    int16_t offset;
    offset = (int8_t) mem_read(gb, gb->pc++);
    gb->hl = gb->sp + offset;
    gb->f &= ~z;
    gb->f &= ~n;

    if ((gb->sp & 0xF) + (offset & 0xF) > 0xF) {
        gb->f |= h;
    }

    if ((gb->sp & 0xFF) + (offset & 0xFF) > 0xFF) {
        gb->f |= c;
    }

    gb->m_cycles += 3;
}

static void ld_sp_hl(gameboy_t *gb)
{
    gb->sp = gb->hl;
    gb->m_cycles += 2;
}

// 8-bit Arithmetic and Logic Instructions
static void adc_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t a = gb->a;
    uint8_t carry = (gb->f & c);
    gb->a += carry + value;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    if ((a & 0xF) + (value & 0xF) + carry > 0xF)
    {
        gb->f |= h;
    }

    if (((uint16_t) a) + value + carry > 0xFF)
    {
        gb->f |= c;
    }

    gb->m_cycles += 1;
}

static void add_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t a = gb->a;
    gb->a += value;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    if ((a & 0xF) + (value & 0xF) > 0xF)
    {
        gb->f |= h;
    }

    if (((uint16_t) a) + value > 0xFF)
    {
        gb->f |= c;
    }

    gb->m_cycles += 1;
}