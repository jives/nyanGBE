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
 * 
 *      | B | C | D | E | H | L | (HL) | A |
 *      | 0 | 1 | 2 | 3 | 4 | 5 |  6   | 7 |
 * 
 * but our register array in memory is ordered according to
 * the actual GB layout (in big endian), so
 * 
 *      | F | A | C | B | E | D | L | H |
 *      | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 * 
 * This function translates those two mappings and handles
 * indices that are out of bounds as well as the (HL) case.
 * 
 * @param regnum optable register number
 * @return uint8_t memory array register index
 */
static inline uint8_t regmap(uint8_t regnum)
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
        // hli functions instead
        case 7: return 1;
        default:
            printf("Unsupported register number %d", regnum);
            assert(!"Unsupported register number");
            break;
    }
}

/**
 * @brief Returns value from register indicated by opcode
 * 
 * This function value from the target register (or memory location for
 * the (HL) case).
 * 
 * @param gb pointer to the gameboy state struct
 * @param opcode gameboy opcode
 * @return uint8_t register value (memory value for (HL))
 */
static uint8_t read_opcode_reg(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = opcode & 0b111;
    if (src != 6)
    {
        return gb->registers[regmap(src)];
    }
    else
    {
        // (HL)
        gb->m_cycles++;
        return mem_read(gb, gb->hl);
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
        case OP_LD_BC_u16:
            gb->bc = data;
            break;
        case OP_LD_DE_u16:
            gb->de = data;
            break;
        case OP_LD_HL_u16:
            gb->hl = data;
            break;
        case OP_LD_SP_u16:
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
    mem_write(gb, gb->hl, data);
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
        case OP_LD_BCi_A:
            dst = gb->bc;
            break;
        case OP_LD_DEi_A:
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
        case OP_LD_A_BCi:
            src = gb->bc;
            break;
        case OP_LD_A_DEi:
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
    gb->f = 0x00;

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

/**
 * @brief Internal ADC implementation
 * 
 * Handles flags as well
 * 
 * @param gb 
 * @param value 
 * @param carry 
 */
static void _adc(gameboy_t *gb, uint8_t value, uint8_t carry)
{
    uint8_t a = gb->a;

    gb->a = a + carry + value;
    gb->f = 0x00;

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
}

/**
 * @brief Common ADD/ADC A,r8 implementation
 * 
 * Carry handled depending on opcode.
 * Covers (HL) as well.
 * 
 * @param gb 
 * @param opcode 
 */
static void addc_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = read_opcode_reg(gb, opcode);
    uint8_t carry = (gb->f & c) != 0 && opcode & 0x08;
    
    _adc(gb, value, carry);

    gb->m_cycles += 1;
}

static void addc_a_d8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = mem_read(gb, gb->pc++);
    uint8_t carry = (gb->f & c) != 0  && opcode & 0x08;

    _adc(gb, value, carry);

    gb->m_cycles += 2;
}

static void add_sp_i8(gameboy_t *gb)
{
    int8_t value = mem_read(gb, gb->pc++);
    uint8_t sp = gb->sp;
    gb->sp += value;
    gb->f = 0x00;

    if ((sp & 0xF) + (value & 0xF) > 0xF)
    {
        gb->f |= h;
    }

    if (((uint16_t) sp) + value > 0xFF)
    {
        gb->f |= c;
    }

    gb->m_cycles += 4;
}

static void and_a_r8(gameboy_t *gb, uint8_t opcode)
{
    int8_t value = read_opcode_reg(gb, opcode);
    uint8_t a = gb->a;
    gb->a &= value;
    gb->f = 0x00;
    gb->f |= h;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 1;
}

static void and_a_d8(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->pc++);
    uint8_t a = gb->a;
    gb->a &= value;
    gb->f = 0x00;
    gb->f |= h;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void cp_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = read_opcode_reg(gb, opcode);
    gb->f = 0x00;
    gb->f |= n;

    if (gb->a == value)
    {
        gb->f |= z;
    }

    if ((gb->a & 0x0F) < (value & 0x0F))
    {
        gb->f |= h;
    }

    if (gb->a < value)
    {
        gb->f |= c;
    }

    gb->m_cycles += 1;
}

static void cp_a_d8(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->pc++);
    gb->f = 0x00;
    gb->f |= n;

    if (gb->a == value)
    {
        gb->f |= z;
    }

    if ((gb->a & 0x0F) < (value & 0x0F))
    {
        gb->f |= h;
    }

    if (gb->a < value)
    {
        gb->f |= c;
    }

    gb->m_cycles += 2;
}

static void dec_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap(opcode - OP_DEC_B >> 3);
    uint8_t value = gb->registers[dst]--;
    gb->f &= ~(z & h);
    gb->f |= n;

    if (gb->registers[dst] == 0)
    {
        gb->f |= z;
    }

    // TODO: check if correct
    if ((value & 0x0F) < 1)
    {
        gb->f |= h;
    }

    gb->m_cycles += 1;
}

static void dec_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    mem_write(gb, gb->hl, value-1);
    gb->f &= ~(z & h);
    gb->f |= n;

    if (value - 1 == 0)
    {
        gb->f |= z;
    }

    // TODO: check if correct
    if ((value & 0x0F) < 1)
    {
        gb->f |= h;
    }

    gb->m_cycles += 3;
}

static void dec_r16(gameboy_t *gb, uint8_t opcode)
{
    switch(opcode)
    {
        case OP_DEC_BC:
            gb->bc--;
        case OP_DEC_DE:
            gb->de--;
        case OP_DEC_HL:
            gb->hl--;
        case OP_DEC_SP:
            gb->sp--;
        
        default:
        printf("Unsupported opcode %02X for DEC r16", opcode);
        assert(!"Unsupported opcode for DEC r16");
        break;
    }

    gb->m_cycles += 2;
}

static void inc_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap(opcode - OP_INC_B >> 3);
    uint8_t value = gb->registers[dst]++;
    gb->f &= ~(z & h & n);

    if (gb->registers[dst] == 0)
    {
        gb->f |= z;
    }

    // TODO: check if correct
    if ((value & 0x0F) > 0x0F)
    {
        gb->f |= h;
    }

    gb->m_cycles += 1;
}

static void inc_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    mem_write(gb, gb->hl, value+1);
    gb->f &= ~(z & h);
    gb->f |= n;

    if (value + 1 == 0)
    {
        gb->f |= z;
    }

    // TODO: check if correct
    if ((value & 0x0F) > 0x0F)
    {
        gb->f |= h;
    }

    gb->m_cycles += 3;
}

static void inc_r16(gameboy_t *gb, uint8_t opcode)
{
    switch(opcode)
    {
        case OP_INC_BC:
            gb->bc++;
        case OP_INC_DE:
            gb->de++;
        case OP_INC_HL:
            gb->hl++;
        case OP_INC_SP:
            gb->sp++;
        
        default:
        printf("Unsupported opcode %02X for INC r16", opcode);
        assert(!"Unsupported opcode for INC r16");
        break;
    }

    gb->m_cycles += 2;
}

static void or_a_r8(gameboy_t *gb, uint8_t opcode)
{
    int8_t value = read_opcode_reg(gb, opcode);
    uint8_t a = gb->a;
    gb->a |= value;
    gb->f = 0x00;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 1;
}

static void or_a_d8(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->pc++);
    uint8_t a = gb->a;
    gb->a |= value;
    gb->f = 0x00;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

/**
 * @brief Internal SBC implementation
 * 
 * Handles flags as well
 * 
 * @param gb 
 * @param value 
 * @param carry 
 */
static void _sbc(gameboy_t *gb, uint8_t value, uint8_t carry)
{
    uint8_t a = gb->a;

    gb->a = a - carry - value;
    gb->f = 0x00;
    gb->f |= n;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    if ((a & 0x0F) < (value & 0x0F) + carry)
    {
        gb->f |= h;
    }

    if (value + carry > a)
    {
        gb->f |= c;
    }
}

/**
 * @brief Common SUB/SBC A,r8 implementation
 * 
 * Carry handled depending on opcode.
 * Covers (HL) as well.
 * 
 * @param gb 
 * @param opcode 
 */
static void subc_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = read_opcode_reg(gb, opcode);
    uint8_t carry = (gb->f & c) != 0 && opcode & 0x08;
    
    _sbc(gb, value, carry);

    gb->m_cycles += 1;
}

static void subc_a_d8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = mem_read(gb, gb->pc++);
    uint8_t carry = (gb->f & c) != 0  && opcode & 0x08;

    _sbc(gb, value, carry);

    gb->m_cycles += 2;
}

static void xor_a_r8(gameboy_t *gb, uint8_t opcode)
{
    int8_t value = read_opcode_reg(gb, opcode);
    uint8_t a = gb->a;
    gb->a ^= value;
    gb->f = 0x00;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 1;
}

static void xor_a_d8(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->pc++);
    uint8_t a = gb->a;
    gb->a ^= value;
    gb->f = 0x00;

    if (gb->a == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

// 16-bit Arithmetic Instructions
// This incudes ADD HL,SP
static void add_hl_r16(gameboy_t *gb, uint8_t opcode)
{
    uint16_t value;
    uint8_t hl = gb->hl;

    switch(opcode)
    {
        case OP_ADD_HL_BC:
            value = gb->bc;
        case OP_ADD_HL_DE:
            value = gb->de;
        case OP_ADD_HL_HL:
            value = gb->hl;
        case OP_ADD_HL_SP:
            value = gb->sp;
        
        default:
        printf("Unsupported opcode %02X for ADD HL,(r16)", opcode);
        assert(!"Unsupported opcode for ADD HL,(r16)");
        break;
    }
    
    gb->hl += value;
    gb->f &= ~(n & h & c);

    if ((hl & 0xFFF) + (value & 0xFFF) > 0xFFF)
    {
        gb->f |= h;
    }

    if (((uint32_t) hl) + value > 0xFFFF)
    {
        gb->f |= c;
    }

    gb->m_cycles += 2;
}

static void dec_r16(gameboy_t *gb, uint8_t opcode)
{
    switch(opcode)
    {
        case OP_DEC_BC:
            gb->bc--;
        case OP_DEC_DE:
            gb->de--;
        case OP_DEC_HL:
            gb->hl--;
        case OP_DEC_SP:
            gb->sp--;
        
        default:
        printf("Unsupported opcode %02X for DEC r16", opcode);
        assert(!"Unsupported opcode for DEC r16");
        break;
    }

    gb->m_cycles += 2;
}

static void inc_r16(gameboy_t *gb, uint8_t opcode)
{
    switch(opcode)
    {
        case OP_INC_BC:
            gb->bc++;
        case OP_INC_DE:
            gb->de++;
        case OP_INC_HL:
            gb->hl++;
        case OP_INC_SP:
            gb->sp++;
        
        default:
        printf("Unsupported opcode %02X for INC r16", opcode);
        assert(!"Unsupported opcode for INC r16");
        break;
    }

    gb->m_cycles += 2;
}

// Bit Operation Instructions
static void bit_u3_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = opcode - OP_BIT_0_B >> 3;
    uint8_t value = read_opcode_reg(gb, opcode);
    gb->f &= ~(n & z);
    gb->f |= h;

    if(!(value & (1 << bit)))
    {
        gb->f |= c;
    }

    gb->m_cycles += 2;
}

static void res_u3_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = opcode - OP_RES_0_B >> 3;
    uint8_t dst = regmap(opcode);
    gb->registers[dst] &= ~(1 << bit);

    gb->m_cycles += 2;
}

static void res_u3_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = opcode - OP_RES_0_B >> 3;
    uint8_t value = mem_read(gb, gb->hl);
    mem_write(gb, gb->hl, value & ~(1 << bit));

    gb->m_cycles += 4;
}

static void set_u3_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = opcode - OP_SET_0_B >> 3;
    uint8_t dst = regmap(opcode);
    gb->registers[dst] |= 1 << bit;

    gb->m_cycles += 2;
}

static void set_u3_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = opcode - OP_SET_0_B >> 3;
    uint8_t value = mem_read(gb, gb->hl);
    mem_write(gb, gb->hl, value | (1 << bit));

    gb->m_cycles += 4;
}

static void swap_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap(opcode);
    uint8_t value = gb->registers[dst];

    gb->registers[dst] = ((value >> 4) & 0x0F) | ((value << 4) & 0xF0);

    gb->m_cycles += 2;
}

static void swap_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = mem_read(gb, gb->hl);
    mem_write(gb, gb->hl, ((value >> 4) & 0x0F) | ((value << 4) & 0xF0));

    gb->m_cycles += 4;
}

// Bit Shift Instructions
static void rl_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b7 = (value & 0x80) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value << 1) | carry;

    if(b7)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void rl_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b7 = (value & 0x80) != 0;

    gb->f = 0x00;
    value = (value << 1) | carry;
    mem_write(gb, gb->hl, value);

    if(b7)
    {
        gb->f |= c;
    }

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void rlc_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value << 1) | carry;

    if(carry)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void rlc_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    value = (value << 1) | carry;
    mem_write(gb, gb->hl, value);

    if(carry)
    {
        gb->f |= c;
    }

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void rr_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b0 = (value & 0x01) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value >> 1) | (carry << 7);

    if(b0)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void rr_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b0 = (value & 0x01) != 0;

    gb->f = 0x00;
    value = (value >> 1) | (carry << 7);
    mem_write(gb, gb->hl, value);

    if(b0)
    {
        gb->f |= c;
    }

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void rrc_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];
    uint8_t carry = (value & 0x01) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value >> 1) | (carry << 7);

    if(carry)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void rrc_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    uint8_t carry = (value & 0x01) != 0;

    gb->f = 0x00;
    value = (value >> 1) | (carry << 7);
    mem_write(gb, gb->hl, value);

    if(carry)
    {
        gb->f |= c;
    }

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void sla_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    gb->registers[src] = value << 1;

    if(carry)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void sla_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    value = value << 1;
    mem_write(gb, gb->hl, value);

    if(carry)
    {
        gb->f |= c;
    }

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void sra_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];
    uint8_t b7 = (value & 0x80);

    gb->f = 0x00;
    gb->registers[src] = (value >> 1) | b7;

    if(value & 0x01)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void sra_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);
    uint8_t b7 = (value & 0x80);

    gb->f = 0x00;

    if(value & 0x01)
    {
        gb->f |= c;
    }

    value = (value >> 1) | b7;
    mem_write(gb, gb->hl, value);

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void srl_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode);
    uint8_t value = gb->registers[src];

    gb->f = 0x00;
    gb->registers[src] = (value >> 1);

    if(value & 0x01)
    {
        gb->f |= c;
    }

    if(gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void srl_hli(gameboy_t *gb)
{
    uint8_t value = mem_read(gb, gb->hl);

    gb->f = 0x00;

    if(value & 0x01)
    {
        gb->f |= c;
    }

    value = (value >> 1);
    mem_write(gb, gb->hl, value);

    if(value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}