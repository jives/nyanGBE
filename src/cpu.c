#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "cpu.h"
#include "memory.h"
#include "opcodes.h"

/**
 * @brief Maps optable register number to array register index
 *
 * In the GB optable, registers are ordered the following way
 *
 *      | B | C | D | E | H | L | (HL) | A |
 *      | 0 | 1 | 2 | 3 | 4 | 5 |  6   | 7 |
 *
 * but our register array in memory is ordered according to
 * the actual GB layout (in big endian), i.e.
 *
 *      | F | A | C | B | E | D | L | H |
 *      | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *
 * Unfortunately, there is no way to calculate the correct index
 * (as implemented) from the opcode order, so this function translates
 * those two mappings and handles indices that are out of bounds as
 * well as the (HL) case.
 *
 * @param regnum optable register number
 * @return uint8_t memory array register index
 */
static inline uint8_t regmap(uint8_t regnum)
{
    switch (regnum)
    {
    case 0:
        return 3;
    case 1:
        return 2;
    case 2:
        return 5;
    case 3:
        return 4;
    case 4:
        return 7;
    case 5:
        return 6;
    // case 6 would be (HL), this is handled by
    // hli functions instead
    case 7:
        return 1;
    default:
        printf("Unsupported register number %d", regnum);
        assert(!"Unsupported register number");
        return 0;
    }
}

/**
 * @brief Returns value from register indicated by opcode
 *
 * This function returns the value from the target register
 * (or memory location for the (HL) case).
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
        return mem_read_byte(gb, gb->hl);
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
    gb->registers[dst] = mem_read_byte(gb, gb->pc++);
    gb->m_cycles += 2;
}

static void ld_r16_d16(gameboy_t *gb, uint8_t opcode)
{
    uint16_t data = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);

    switch (opcode)
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
        printf("Unsupported opcode 0x%02X for LD r16,n16\n", opcode);
        assert(!"Unsupported opcode for LD r16,n16");
        break;
    }

    gb->m_cycles += 3;
}

static void ld_hli_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    mem_write_byte(gb, gb->hl, gb->registers[src]);
    gb->m_cycles += 2;
}

static void ld_hli_d8(gameboy_t *gb)
{
    uint8_t data = mem_read_byte(gb, gb->pc++);
    mem_write_byte(gb, gb->hl, data);
    gb->m_cycles += 3;
}

static void ld_r8_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode >> 3) & 0b111);
    gb->registers[dst] = mem_read_byte(gb, gb->hl);
    gb->m_cycles += 2;
}

static void ld_r16i_a(gameboy_t *gb, uint8_t opcode)
{
    uint16_t dst;

    switch (opcode)
    {
    case OP_LD_BCi_A:
        dst = gb->bc;
        break;
    case OP_LD_DEi_A:
        dst = gb->de;
        break;

    default:
        printf("Unsupported opcode 0x%02X for LD (r16),A\n", opcode);
        assert(!"Unsupported opcode for LD (r16),A");
        break;
    }

    mem_write_byte(gb, dst, gb->a);
    gb->m_cycles += 2;
}

static void ld_d16i_a(gameboy_t *gb)
{
    uint8_t src_lo = mem_read_byte(gb, gb->pc++);
    uint8_t src_hi = mem_read_byte(gb, gb->pc++);
    mem_write_byte(gb, src_lo + (src_hi << 8), gb->a);
    gb->m_cycles += 4;
}

static void ldh_d16i_a(gameboy_t *gb)
{
    uint8_t src_lo = mem_read_byte(gb, gb->pc++);
    mem_write_byte(gb, 0xFF00 + src_lo, gb->a);
    gb->m_cycles += 3;
}

static void ldh_ci_a(gameboy_t *gb)
{
    mem_write_byte(gb, 0xFF00 + gb->c, gb->a);
    gb->m_cycles += 2;
}

static void ld_a_r16i(gameboy_t *gb, uint8_t opcode)
{
    uint16_t src;

    switch (opcode)
    {
    case OP_LD_A_BCi:
        src = gb->bc;
        break;
    case OP_LD_A_DEi:
        src = gb->de;
        break;

    default:
        printf("Unsupported opcode 0x%02X for LD A,(r16)\n", opcode);
        assert(!"Unsupported opcode for LD A,(r16)");
        break;
    }

    gb->a = mem_read_byte(gb, src);
    gb->m_cycles += 2;
}

static void ld_a_d16i(gameboy_t *gb)
{
    uint8_t src_lo = mem_read_byte(gb, gb->pc++);
    uint8_t src_hi = mem_read_byte(gb, gb->pc++);
    gb->a = mem_read_byte(gb, src_lo + (src_hi << 8));
    gb->m_cycles += 4;
}

static void ldh_a_d16i(gameboy_t *gb)
{
    uint8_t src_lo = mem_read_byte(gb, gb->pc++);
    gb->a = mem_read_byte(gb, 0xFF00 + src_lo);
    gb->m_cycles += 3;
}

static void ldh_a_ci(gameboy_t *gb)
{
    gb->a = mem_read_byte(gb, 0xFF00 + gb->c);
    gb->m_cycles += 2;
}

static void ld_hlpi_a(gameboy_t *gb)
{
    mem_write_byte(gb, gb->hl, gb->a);
    gb->hl++;
    gb->m_cycles += 2;
}

static void ld_hlmi_a(gameboy_t *gb)
{
    mem_write_byte(gb, gb->hl, gb->a);
    gb->hl--;
    gb->m_cycles += 2;
}

static void ld_a_hlpi(gameboy_t *gb)
{
    gb->a = mem_read_byte(gb, gb->hl);
    gb->hl++;
    gb->m_cycles += 2;
}

static void ld_a_hlmi(gameboy_t *gb)
{
    gb->a = mem_read_byte(gb, gb->hl);
    gb->hl--;
    gb->m_cycles += 2;
}

static void ld_sp_d16(gameboy_t *gb)
{
    // Note: This is covered by the OP_LD_SP_u16 case of ld_r16_d16() already
    gb->sp = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);
    gb->m_cycles += 3;
}

static void ld_d16_sp(gameboy_t *gb)
{
    uint16_t addr = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);
    mem_write_byte(gb, addr, gb->sp & 0xFF);
    mem_write_byte(gb, addr + 1, gb->sp >> 8);
    gb->m_cycles += 5;
}

static void ld_hl_sp_i8(gameboy_t *gb)
{
    int16_t offset;
    offset = (int16_t)mem_read_byte(gb, gb->pc++);
    gb->hl = gb->sp + offset;
    gb->f = 0x00;

    if ((gb->sp & 0xF) + (offset & 0xF) > 0xF)
    {
        gb->f |= h;
    }

    if ((gb->sp & 0xFF) + (offset & 0xFF) > 0xFF)
    {
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
 * Internal add value plus carry to register a implementation.
 * Handles flags as well.
 *
 * @param gb pointer to the gameboy state struct
 * @param value the value to add to register a
 * @param carry carry value (0/1) to add to register a
 */
static void adc_internal(gameboy_t *gb, uint8_t value, uint8_t carry)
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

    if (((uint16_t)a) + value + carry > 0xFF)
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
 * @param gb pointer to the gameboy state struct
 * @param opcode gameboy opcode
 */
static void addc_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = read_opcode_reg(gb, opcode);
    uint8_t carry = (gb->f & c) != 0 && opcode & 0x08;

    adc_internal(gb, value, carry);

    gb->m_cycles += 1;
}

/**
 * @brief Common ADD/ADC A,d8 implementation
 *
 * Carry handled depending on opcode.
 *
 * @param gb pointer to the gameboy state struct
 * @param opcode gameboy opcode
 */
static void addc_a_d8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = mem_read_byte(gb, gb->pc++);
    uint8_t carry = (gb->f & c) != 0 && opcode & 0x08;

    adc_internal(gb, value, carry);

    gb->m_cycles += 2;
}

static void add_sp_i8(gameboy_t *gb)
{
    int8_t value = mem_read_byte(gb, gb->pc++);
    uint8_t sp = gb->sp;
    gb->sp += value;
    gb->f = 0x00;

    if ((sp & 0xF) + (value & 0xF) > 0xF)
    {
        gb->f |= h;
    }

    if (((uint16_t)sp) + value > 0xFF)
    {
        gb->f |= c;
    }

    gb->m_cycles += 4;
}

static void and_a_r8(gameboy_t *gb, uint8_t opcode)
{
    int8_t value = read_opcode_reg(gb, opcode);
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
    uint8_t value = mem_read_byte(gb, gb->pc++);
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
    uint8_t value = mem_read_byte(gb, gb->pc++);
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
    uint8_t dst = regmap((opcode - OP_DEC_B) >> 3);
    uint8_t value = gb->registers[dst]--;
    gb->f &= ~(z | h);
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
    uint8_t value = mem_read_byte(gb, gb->hl);
    mem_write_byte(gb, gb->hl, value - 1);
    gb->f &= ~(z | h);
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

static void inc_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap((opcode - OP_INC_B) >> 3);
    uint8_t value = gb->registers[dst]++;
    gb->f &= ~(z | h | n);

    if (gb->registers[dst] == 0)
    {
        gb->f |= z;
    }

    if ((value & 0x0F) == 0x0F)
    {
        gb->f |= h;
    }

    gb->m_cycles += 1;
}

static void inc_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    mem_write_byte(gb, gb->hl, value + 1);
    gb->f &= ~(z | h);
    gb->f |= n;

    if (value + 1 == 0)
    {
        gb->f |= z;
    }

    if ((value & 0x0F) == 0x0F)
    {
        gb->f |= h;
    }

    gb->m_cycles += 3;
}

static void or_a_r8(gameboy_t *gb, uint8_t opcode)
{
    int8_t value = read_opcode_reg(gb, opcode);
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
    uint8_t value = mem_read_byte(gb, gb->pc++);
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
 * Internal subtract value and carry from register a implementation.
 * Handles flags as well.
 *
 * @param gb pointer to the gameboy state struct
 * @param value the value to subtract from register a
 * @param carry carry value (0/1) to subtract from register a
 */
static void sbc_internal(gameboy_t *gb, uint8_t value, uint8_t carry)
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
 * @param gb pointer to the gameboy state struct
 * @param opcode gameboy opcode
 */
static void subc_a_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = read_opcode_reg(gb, opcode);
    uint8_t carry = (gb->f & c) != 0 && opcode & 0x08;

    sbc_internal(gb, value, carry);

    gb->m_cycles += 1;
}

static void subc_a_d8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t value = mem_read_byte(gb, gb->pc++);
    uint8_t carry = (gb->f & c) != 0 && opcode & 0x08;

    sbc_internal(gb, value, carry);

    gb->m_cycles += 2;
}

static void xor_a_r8(gameboy_t *gb, uint8_t opcode)
{
    int8_t value = read_opcode_reg(gb, opcode);
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
    uint8_t value = mem_read_byte(gb, gb->pc++);
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
    uint16_t hl = gb->hl;

    switch (opcode)
    {
    case OP_ADD_HL_BC:
        value = gb->bc;
        break;
    case OP_ADD_HL_DE:
        value = gb->de;
        break;
    case OP_ADD_HL_HL:
        value = gb->hl;
        break;
    case OP_ADD_HL_SP:
        value = gb->sp;
        break;

    default:
        printf("Unsupported opcode 0x%02X for ADD HL,(r16)\n", opcode);
        assert(!"Unsupported opcode for ADD HL,(r16)");
        break;
    }

    gb->hl += value;
    gb->f &= ~(n | h | c);

    if ((hl & 0xFFF) + (value & 0xFFF) > 0xFFF)
    {
        gb->f |= h;
    }

    if (((uint32_t)hl) + value > 0xFFFF)
    {
        gb->f |= c;
    }

    gb->m_cycles += 2;
}

static void dec_r16(gameboy_t *gb, uint8_t opcode)
{
    switch (opcode)
    {
    case OP_DEC_BC:
        gb->bc--;
        break;
    case OP_DEC_DE:
        gb->de--;
        break;
    case OP_DEC_HL:
        gb->hl--;
        break;
    case OP_DEC_SP:
        gb->sp--;
        break;

    default:
        printf("Unsupported opcode 0x%02X for DEC r16\n", opcode);
        assert(!"Unsupported opcode for DEC r16");
        break;
    }

    gb->m_cycles += 2;
}

static void inc_r16(gameboy_t *gb, uint8_t opcode)
{
    switch (opcode)
    {
    case OP_INC_BC:
        gb->bc++;
        break;
    case OP_INC_DE:
        gb->de++;
        break;
    case OP_INC_HL:
        gb->hl++;
        break;
    case OP_INC_SP:
        gb->sp++;
        break;

    default:
        printf("Unsupported opcode 0x%02X for INC r16\n", opcode);
        assert(!"Unsupported opcode for INC r16");
        break;
    }

    gb->m_cycles += 2;
}

// Bit Operation Instructions (OxCB prefixed)
static void bit_u3_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = (opcode - OP_BIT_0_B) >> 3;
    uint8_t value = read_opcode_reg(gb, opcode);
    gb->f &= ~(n | z);
    gb->f |= h;

    if (!(value & (1 << bit)))
    {
        gb->f |= c;
    }

    gb->m_cycles += 2;
}

static void res_u3_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = (opcode - OP_RES_0_B) >> 3;
    uint8_t dst = regmap(opcode & 0b111);
    gb->registers[dst] &= ~(1 << bit);

    gb->m_cycles += 2;
}

static void res_u3_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = (opcode - OP_RES_0_B) >> 3;
    uint8_t value = mem_read_byte(gb, gb->hl);
    mem_write_byte(gb, gb->hl, value & ~(1 << bit));

    gb->m_cycles += 4;
}

static void set_u3_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = (opcode - OP_SET_0_B) >> 3;
    uint8_t dst = regmap(opcode & 0b111);
    gb->registers[dst] |= 1 << bit;

    gb->m_cycles += 2;
}

static void set_u3_hli(gameboy_t *gb, uint8_t opcode)
{
    uint8_t bit = (opcode - OP_SET_0_B) >> 3;
    uint8_t value = mem_read_byte(gb, gb->hl);
    mem_write_byte(gb, gb->hl, value | (1 << bit));

    gb->m_cycles += 4;
}

static void swap_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t dst = regmap(opcode & 0b111);
    uint8_t value = gb->registers[dst];

    gb->registers[dst] = ((value >> 4) & 0x0F) | ((value << 4) & 0xF0);

    gb->m_cycles += 2;
}

static void swap_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    mem_write_byte(gb, gb->hl, ((value >> 4) & 0x0F) | ((value << 4) & 0xF0));

    gb->m_cycles += 4;
}

// Bit Shift Instructions (OxCB prefixed)
static void rl_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b7 = (value & 0x80) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value << 1) | carry;

    if (b7)
    {
        gb->f |= c;
    }

    if (gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void rl_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b7 = (value & 0x80) != 0;

    gb->f = 0x00;
    value = (value << 1) | carry;
    mem_write_byte(gb, gb->hl, value);

    if (b7)
    {
        gb->f |= c;
    }

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void rlc_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value << 1) | carry;

    if (carry)
    {
        gb->f |= c;
    }

    if (gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void rlc_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    value = (value << 1) | carry;
    mem_write_byte(gb, gb->hl, value);

    if (carry)
    {
        gb->f |= c;
    }

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void rr_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b0 = (value & 0x01) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value >> 1) | (carry << 7);

    if (b0)
    {
        gb->f |= c;
    }

    if (opcode == OP_RRA)
    {
        gb->m_cycles += 1;
    }
    else
    {
        // It seems that RRA does not set the zero flag
        if (gb->registers[src] == 0)
        {
            gb->f |= z;
        }
        gb->m_cycles += 2;
    }
}

static void rr_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    uint8_t carry = (gb->f & c) != 0;
    uint8_t b0 = (value & 0x01) != 0;

    gb->f = 0x00;
    value = (value >> 1) | (carry << 7);
    mem_write_byte(gb, gb->hl, value);

    if (b0)
    {
        gb->f |= c;
    }

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void rrc_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t carry = (value & 0x01) != 0;

    gb->f = 0x00;
    gb->registers[src] = (value >> 1) | (carry << 7);

    if (carry)
    {
        gb->f |= c;
    }

    if (opcode == OP_RRCA)
    {
        gb->m_cycles += 1;
    }
    else
    {
        // It seems that RRCA does not set the zero flag
        if (gb->registers[src] == 0)
        {
            gb->f |= z;
        }
        gb->m_cycles += 2;
    }
}

static void rrc_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    uint8_t carry = (value & 0x01) != 0;

    gb->f = 0x00;
    value = (value >> 1) | (carry << 7);
    mem_write_byte(gb, gb->hl, value);

    if (carry)
    {
        gb->f |= c;
    }

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void sla_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    gb->registers[src] = value << 1;

    if (carry)
    {
        gb->f |= c;
    }

    if (gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void sla_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    uint8_t carry = (value & 0x80) != 0;

    gb->f = 0x00;
    value = value << 1;
    mem_write_byte(gb, gb->hl, value);

    if (carry)
    {
        gb->f |= c;
    }

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void sra_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];
    uint8_t b7 = (value & 0x80);

    gb->f = 0x00;
    gb->registers[src] = (value >> 1) | b7;

    if (value & 0x01)
    {
        gb->f |= c;
    }

    if (gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void sra_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);
    uint8_t b7 = (value & 0x80);

    gb->f = 0x00;

    if (value & 0x01)
    {
        gb->f |= c;
    }

    value = (value >> 1) | b7;
    mem_write_byte(gb, gb->hl, value);

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

static void srl_r8(gameboy_t *gb, uint8_t opcode)
{
    uint8_t src = regmap(opcode & 0b111);
    uint8_t value = gb->registers[src];

    gb->f = 0x00;
    gb->registers[src] = (value >> 1);

    if (value & 0x01)
    {
        gb->f |= c;
    }

    if (gb->registers[src] == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 2;
}

static void srl_hli(gameboy_t *gb)
{
    uint8_t value = mem_read_byte(gb, gb->hl);

    gb->f = 0x00;

    if (value & 0x01)
    {
        gb->f |= c;
    }

    value = (value >> 1);
    mem_write_byte(gb, gb->hl, value);

    if (value == 0)
    {
        gb->f |= z;
    }

    gb->m_cycles += 4;
}

// Jumps and Subroutines
/**
 * @brief Checks if condition is met
 *
 * @param gb gameboy state struct
 * @param opcode gameboy opcode
 * @return true if the condition is met
 * @return false otherwise
 */
static bool check_condition(gameboy_t *gb, uint8_t opcode)
{
    switch ((opcode >> 3) & 0x3)
    {
    case 0:
        return !(gb->f & z);
    case 1:
        return (gb->f & z);
    case 2:
        return !(gb->f & c);
    case 3:
        return (gb->f & c);
    }

    return false;
}

static void call_d16(gameboy_t *gb)
{
    uint16_t addr = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);
    mem_write_byte(gb, --gb->sp, (gb->pc) >> 8);
    mem_write_byte(gb, --gb->sp, (gb->pc) & 0xFF);
    gb->pc = addr;

    gb->m_cycles += 6;
}

static void call_cc_d16(gameboy_t *gb, uint8_t opcode)
{
    // Address is always read (and therefore PC is increased)
    uint16_t addr = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);

    if (check_condition(gb, opcode))
    {
        mem_write_byte(gb, --gb->sp, (gb->pc) >> 8);
        mem_write_byte(gb, --gb->sp, (gb->pc) & 0xFF);
        gb->pc = addr;

        gb->m_cycles += 6;
    }
    else
    {
        gb->m_cycles += 3;
    }
}

static void jp_hl(gameboy_t *gb)
{
    gb->pc = gb->hl;

    gb->m_cycles += 1;
}

static void jp_d16(gameboy_t *gb)
{
    uint16_t addr = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);
    gb->pc = addr;

    gb->m_cycles += 4;
}

static void jp_cc_d16(gameboy_t *gb, uint8_t opcode)
{
    // Address is always read (and therefore PC is increased)
    uint16_t addr = mem_read_byte(gb, gb->pc++) + (mem_read_byte(gb, gb->pc++) << 8);

    if (check_condition(gb, opcode))
    {
        gb->pc = addr;

        gb->m_cycles += 4;
    }
    else
    {
        gb->m_cycles += 3;
    }
}

static void jr_d8(gameboy_t *gb)
{
    int8_t offset = mem_read_byte(gb, gb->pc++);
    gb->pc += offset;

    gb->m_cycles += 3;
}

static void jr_cc_d8(gameboy_t *gb, uint8_t opcode)
{
    // Address offset is always read (and therefore PC is increased)
    int8_t offset = mem_read_byte(gb, gb->pc++);

    if (check_condition(gb, opcode))
    {
        gb->pc += offset;

        gb->m_cycles += 3;
    }
    else
    {
        gb->m_cycles += 2;
    }
}

static void ret_cc(gameboy_t *gb, uint8_t opcode)
{
    if (check_condition(gb, opcode))
    {
        // SP is only increased if condition is met
        uint16_t addr = mem_read_byte(gb, gb->sp++) + (mem_read_byte(gb, gb->sp++) << 8);
        gb->pc = addr;

        gb->m_cycles += 5;
    }
    else
    {
        gb->m_cycles += 2;
    }
}

static void ret(gameboy_t *gb)
{
    uint16_t addr = mem_read_byte(gb, gb->sp++) + (mem_read_byte(gb, gb->sp++) << 8);
    gb->pc = addr;

    gb->m_cycles += 4;
}

static void reti(gameboy_t *gb)
{
    uint16_t addr = mem_read_byte(gb, gb->sp++) + (mem_read_byte(gb, gb->sp++) << 8);
    gb->pc = addr;
    gb->ime = true;

    gb->m_cycles += 4;
}

static void rst_vec(gameboy_t *gb, uint8_t opcode)
{
    mem_write_byte(gb, --gb->sp, (gb->pc) >> 8);
    mem_write_byte(gb, --gb->sp, (gb->pc) & 0xFF);
    gb->pc = opcode ^ 0xC7;

    gb->m_cycles += 4;
}

// Stack Operation Instructions

// ADD, DEC, INC, LD stack pointer all implemented in their respective sections

static void pop_r16(gameboy_t *gb, uint8_t opcode)
{
    uint16_t value = mem_read_byte(gb, gb->sp++) + (mem_read_byte(gb, gb->sp++) << 8);

    switch (opcode)
    {
    case OP_POP_AF:
        gb->af = value & 0xFFF0; // Make sure we don't set impossible flags in reg. f
        break;
    case OP_POP_BC:
        gb->bc = value;
        break;
    case OP_POP_DE:
        gb->de = value;
        break;
    case OP_POP_HL:
        gb->hl = value;
        break;

    default:
        printf("Unsupported opcode 0x%02X for POP r16\n", opcode);
        assert(!"Unsupported opcode for POP r16");
        break;
    }

    gb->m_cycles += 3;
}

static void push_r16(gameboy_t *gb, uint8_t opcode)
{
    switch (opcode)
    {
    case OP_PUSH_AF:
        mem_write_byte(gb, --gb->sp, (gb->af) >> 8);
        mem_write_byte(gb, --gb->sp, (gb->af) & 0xFF);
        break;
    case OP_PUSH_BC:
        mem_write_byte(gb, --gb->sp, (gb->bc) >> 8);
        mem_write_byte(gb, --gb->sp, (gb->bc) & 0xFF);
        break;
    case OP_PUSH_DE:
        mem_write_byte(gb, --gb->sp, (gb->de) >> 8);
        mem_write_byte(gb, --gb->sp, (gb->de) & 0xFF);
        break;
    case OP_PUSH_HL:
        mem_write_byte(gb, --gb->sp, (gb->hl) >> 8);
        mem_write_byte(gb, --gb->sp, (gb->hl) & 0xFF);
        break;

    default:
        printf("Unsupported opcode 0x%02X for PUSH r16\n", opcode);
        assert(!"Unsupported opcode for PUSH r16");
        break;
    }

    gb->m_cycles += 4;
}

// Miscellaneous Instructions
static void ccf(gameboy_t *gb)
{
    gb->f &= ~n;
    gb->f &= ~h;
    gb->f ^= c;

    gb->m_cycles += 1;
}

static void cpl(gameboy_t *gb)
{
    uint8_t value = gb->a;
    gb->a = ~value;
    gb->f |= n;
    gb->f |= h;

    gb->m_cycles += 1;
}

static void daa(gameboy_t *gb)
{
    uint16_t result = gb->a;

    if (gb->f & n)
    {
        // Subtraction
        if (gb->f & c)
            result -= 0x60;

        if (gb->f & h)
            result = (result - 0x06) & 0xFF;
    }
    else
    {
        // Addition
        if ((gb->f & c) || (result > 0x9F))
        {
            result += 0x60;
            // TODO: Check if c-flag handling is correct
            gb->f |= c;
        }

        if ((gb->f & h) || ((result & 0x0F) > 0x9))
            result += 0x06;
    }

    if ((result & 0xFF) == 0)
        gb->f |= z;

    gb->f &= ~h;
    gb->a = result & 0xFF;

    gb->m_cycles += 1;
}

static void di(gameboy_t *gb)
{
    gb->ime = false;

    gb->m_cycles += 1;
}

static void ei(gameboy_t *gb)
{
    // EI actually enables interrupts after one additional cycle
    gb->ime_enable = true;

    gb->m_cycles += 1;
}

static void halt(gameboy_t *gb)
{
    // TODO: Implement halt bug
    gb->halted = true;

    gb->m_cycles += 1;
}

// NOP covered already above

static void scf(gameboy_t *gb)
{
    gb->f &= ~n;
    gb->f &= ~h;
    gb->f |= c;

    gb->m_cycles += 1;
}

static void stop(gameboy_t *gb)
{
    // TODO: Extend behavior?
    gb->stopped = true;

    gb->m_cycles += 1;
}

/**
 * @brief Executes opcode
 *
 * Executes standard opcode
 *
 * @param gb pointer to the gameboy state struct
 * @param opcode gameboy opcode
 */
static void execute_opcode(gameboy_t *gb, uint8_t opcode)
{
    switch (opcode)
    {
    case OP_NOP:
    {
        nop(gb);
        break;
    }

    // Load Instructions
    case OP_LD_A_u8:
    case OP_LD_B_u8:
    case OP_LD_C_u8:
    case OP_LD_D_u8:
    case OP_LD_E_u8:
    case OP_LD_H_u8:
    case OP_LD_L_u8:
    {
        ld_r8_d8(gb, opcode);
        break;
    }

    case OP_LD_BC_u16:
    case OP_LD_DE_u16:
    case OP_LD_HL_u16:
    case OP_LD_SP_u16:
    {
        ld_r16_d16(gb, opcode);
        break;
    }

    case OP_LD_A_A:
    case OP_LD_A_B:
    case OP_LD_A_C:
    case OP_LD_A_D:
    case OP_LD_A_E:
    case OP_LD_A_H:
    case OP_LD_A_L:
    case OP_LD_B_A:
    case OP_LD_B_B:
    case OP_LD_B_C:
    case OP_LD_B_D:
    case OP_LD_B_E:
    case OP_LD_B_H:
    case OP_LD_B_L:
    case OP_LD_C_A:
    case OP_LD_C_B:
    case OP_LD_C_C:
    case OP_LD_C_D:
    case OP_LD_C_E:
    case OP_LD_C_H:
    case OP_LD_C_L:
    case OP_LD_D_A:
    case OP_LD_D_B:
    case OP_LD_D_C:
    case OP_LD_D_D:
    case OP_LD_D_E:
    case OP_LD_D_H:
    case OP_LD_D_L:
    case OP_LD_E_A:
    case OP_LD_E_B:
    case OP_LD_E_C:
    case OP_LD_E_D:
    case OP_LD_E_E:
    case OP_LD_E_H:
    case OP_LD_E_L:
    case OP_LD_H_A:
    case OP_LD_H_B:
    case OP_LD_H_C:
    case OP_LD_H_D:
    case OP_LD_H_E:
    case OP_LD_H_H:
    case OP_LD_H_L:
    case OP_LD_L_A:
    case OP_LD_L_B:
    case OP_LD_L_C:
    case OP_LD_L_D:
    case OP_LD_L_E:
    case OP_LD_L_H:
    case OP_LD_L_L:
    {
        ld_r8_r8(gb, opcode);
        break;
    }

    case OP_LD_HLi_B:
    case OP_LD_HLi_C:
    case OP_LD_HLi_D:
    case OP_LD_HLi_E:
    case OP_LD_HLi_H:
    case OP_LD_HLi_L:
    case OP_LD_HLi_A:
    {
        ld_hli_r8(gb, opcode);
        break;
    }

    case OP_LD_HLi_u8:
        ld_hli_d8(gb);
        break;

    case OP_LD_B_HLi:
    case OP_LD_C_HLi:
    case OP_LD_D_HLi:
    case OP_LD_E_HLi:
    case OP_LD_H_HLi:
    case OP_LD_L_HLi:
    case OP_LD_A_HLi:
    {
        ld_r8_hli(gb, opcode);
        break;
    }

    case OP_LD_BCi_A:
    case OP_LD_DEi_A:
    {
        ld_r16i_a(gb, opcode);
        break;
    }

    case OP_LD_u16i_A:
    {
        ld_d16i_a(gb);
        break;
    }

    case OP_LDH_u16i_A:
    {
        ldh_d16i_a(gb);
        break;
    }

    case OP_LDH_Ci_A:
    {
        ldh_ci_a(gb);
        break;
    }

    case OP_LD_A_BCi:
    case OP_LD_A_DEi:
    {
        ld_a_r16i(gb, opcode);
        break;
    }

    case OP_LD_A_u16i:
    {
        ld_a_d16i(gb);
        break;
    }

    case OP_LDH_A_u16i:
    {
        ldh_a_d16i(gb);
        break;
    }

    case OP_LDH_A_Ci:
    {
        ldh_a_ci(gb);
        break;
    }

    case OP_LD_HLpi_A:
    {
        ld_hlpi_a(gb);
        break;
    }

    case OP_LD_HLmi_A:
    {
        ld_hlmi_a(gb);
        break;
    }

    case OP_LD_A_HLpi:
    {
        ld_a_hlpi(gb);
        break;
    }

    case OP_LD_A_HLmi:
    {
        ld_a_hlmi(gb);
        break;
    }

    case OP_LD_a16i_SP:
    {
        ld_d16_sp(gb);
        break;
    }

    case OP_LD_HL_SP_i8:
    {
        ld_hl_sp_i8(gb);
        break;
    }

    case OP_LD_SP_HL:
    {
        ld_sp_hl(gb);
        break;
    }

    // 8-bit Arithmetic and Logic Instructions
    case OP_ADD_A_A:
    case OP_ADD_A_B:
    case OP_ADD_A_C:
    case OP_ADD_A_D:
    case OP_ADD_A_E:
    case OP_ADD_A_H:
    case OP_ADD_A_L:
    case OP_ADD_A_HLi:
    case OP_ADC_A_A:
    case OP_ADC_A_B:
    case OP_ADC_A_C:
    case OP_ADC_A_D:
    case OP_ADC_A_E:
    case OP_ADC_A_H:
    case OP_ADC_A_L:
    case OP_ADC_A_HLi:
    {
        addc_a_r8(gb, opcode);
        break;
    }

    case OP_ADD_A_u8:
    case OP_ADC_A_u8:
    {
        addc_a_d8(gb, opcode);
        break;
    }

    case OP_ADD_SP_i8:
    {
        add_sp_i8(gb);
        break;
    }

    case OP_AND_A_A:
    case OP_AND_A_B:
    case OP_AND_A_C:
    case OP_AND_A_D:
    case OP_AND_A_E:
    case OP_AND_A_H:
    case OP_AND_A_L:
    case OP_AND_A_HLi:
    {
        and_a_r8(gb, opcode);
        break;
    }

    case OP_AND_A_u8:
    {
        and_a_d8(gb);
        break;
    }

    case OP_CP_A_A:
    case OP_CP_A_B:
    case OP_CP_A_C:
    case OP_CP_A_D:
    case OP_CP_A_E:
    case OP_CP_A_H:
    case OP_CP_A_L:
    case OP_CP_A_HLi:
    {
        cp_a_r8(gb, opcode);
        break;
    }

    case OP_CP_A_u8:
    {
        cp_a_d8(gb);
        break;
    }

    case OP_DEC_A:
    case OP_DEC_B:
    case OP_DEC_C:
    case OP_DEC_D:
    case OP_DEC_E:
    case OP_DEC_H:
    case OP_DEC_L:
    {
        dec_r8(gb, opcode);
        break;
    }

    case OP_DEC_HLi:
    {
        dec_hli(gb);
        break;
    }

    case OP_INC_A:
    case OP_INC_B:
    case OP_INC_C:
    case OP_INC_D:
    case OP_INC_E:
    case OP_INC_H:
    case OP_INC_L:
    {
        inc_r8(gb, opcode);
        break;
    }

    case OP_INC_HLi:
    {
        inc_hli(gb);
        break;
    }

    case OP_OR_A_A:
    case OP_OR_A_B:
    case OP_OR_A_C:
    case OP_OR_A_D:
    case OP_OR_A_E:
    case OP_OR_A_H:
    case OP_OR_A_L:
    case OP_OR_A_HLi:
    {
        or_a_r8(gb, opcode);
        break;
    }

    case OP_OR_A_u8:
    {
        or_a_d8(gb);
        break;
    }

    case OP_SUB_A_A:
    case OP_SUB_A_B:
    case OP_SUB_A_C:
    case OP_SUB_A_D:
    case OP_SUB_A_E:
    case OP_SUB_A_H:
    case OP_SUB_A_L:
    case OP_SUB_A_HLi:
    case OP_SBC_A_A:
    case OP_SBC_A_B:
    case OP_SBC_A_C:
    case OP_SBC_A_D:
    case OP_SBC_A_E:
    case OP_SBC_A_H:
    case OP_SBC_A_L:
    case OP_SBC_A_HLi:
    {
        subc_a_r8(gb, opcode);
        break;
    }

    case OP_SUB_A_u8:
    case OP_SBC_A_u8:
    {
        subc_a_d8(gb, opcode);
        break;
    }

    case OP_XOR_A_A:
    case OP_XOR_A_B:
    case OP_XOR_A_C:
    case OP_XOR_A_D:
    case OP_XOR_A_E:
    case OP_XOR_A_H:
    case OP_XOR_A_L:
    case OP_XOR_A_HLi:
    {
        xor_a_r8(gb, opcode);
        break;
    }

    case OP_XOR_A_u8:
    {
        xor_a_d8(gb);
        break;
    }

    case OP_RRA:
    {
        rr_r8(gb, opcode);
        break;
    }

    case OP_RRCA:
    {
        rrc_r8(gb, opcode);
        break;
    }

    // 16-bit Arithmetic Instructions
    case OP_ADD_HL_BC:
    case OP_ADD_HL_DE:
    case OP_ADD_HL_HL:
    case OP_ADD_HL_SP:
    {
        add_hl_r16(gb, opcode);
        break;
    }

    case OP_DEC_BC:
    case OP_DEC_DE:
    case OP_DEC_HL:
    case OP_DEC_SP:
    {
        dec_r16(gb, opcode);
        break;
    }

    case OP_INC_BC:
    case OP_INC_DE:
    case OP_INC_HL:
    case OP_INC_SP:
    {
        inc_r16(gb, opcode);
        break;
    }

    // Jumps and Subroutines
    case OP_CALL_u16:
    {
        call_d16(gb);
        break;
    }

    case OP_CALL_C_u16:
    case OP_CALL_Z_u16:
    case OP_CALL_NC_u16:
    case OP_CALL_NZ_u16:
    {
        call_cc_d16(gb, opcode);
        break;
    }

    case OP_JP_HL:
    {
        jp_hl(gb);
        break;
    }

    case OP_JP_u16:
    {
        jp_d16(gb);
        break;
    }

    case OP_JP_C_u16:
    case OP_JP_Z_u16:
    case OP_JP_NC_u16:
    case OP_JP_NZ_u16:
    {
        jp_cc_d16(gb, opcode);
        break;
    }

    case OP_JR_i8:
    {
        jr_d8(gb);
        break;
    }

    case OP_JR_C_i8:
    case OP_JR_Z_i8:
    case OP_JR_NC_i8:
    case OP_JR_NZ_i8:
    {
        jr_cc_d8(gb, opcode);
        break;
    }

    case OP_RET_C:
    case OP_RET_Z:
    case OP_RET_NC:
    case OP_RET_NZ:
    {
        ret_cc(gb, opcode);
        break;
    }

    case OP_RET:
    {
        ret(gb);
        break;
    }

    case OP_RETI:
    {
        reti(gb);
        break;
    }

    case OP_RST_00h:
    case OP_RST_08h:
    case OP_RST_10h:
    case OP_RST_18h:
    case OP_RST_20h:
    case OP_RST_28h:
    case OP_RST_30h:
    case OP_RST_38h:
    {
        rst_vec(gb, opcode);
        break;
    }

    // Stack Operation Instructions
    case OP_POP_AF:
    case OP_POP_BC:
    case OP_POP_DE:
    case OP_POP_HL:
    {
        pop_r16(gb, opcode);
        break;
    }

    case OP_PUSH_AF:
    case OP_PUSH_BC:
    case OP_PUSH_DE:
    case OP_PUSH_HL:
    {
        push_r16(gb, opcode);
        break;
    }

    // Miscellaneous Instructions
    case OP_CCF:
    {
        ccf(gb);
        break;
    }

    case OP_CPL:
    {
        cpl(gb);
        break;
    }

    case OP_DAA:
    {
        daa(gb);
        break;
    }

    case OP_DI:
    {
        di(gb);
        break;
    }

    case OP_EI:
    {
        ei(gb);
        break;
    }

    case OP_HALT:
    {
        halt(gb);
        break;
    }

    case OP_SCF:
    {
        scf(gb);
        break;
    }

    case OP_STOP:
    {
        stop(gb);
        break;
    }

    default:
    {
        printf("Unhandled opcode: 0x%02X\n", opcode);
        assert(!"Unhandled opcode");
        break;
    }
    }
}

/**
 * @brief Executes prefixed opcode
 *
 * Executes CB-prefixed opcode
 *
 * @param gb pointer to the gameboy state struct
 * @param opcode gameboy opcode (without the CB prefix)
 */
static void execute_cb_opcode(gameboy_t *gb, uint8_t opcode)
{
    switch (opcode)
    {
    // Bit Operation Instructions
    case OP_BIT_0_A:
    case OP_BIT_0_B:
    case OP_BIT_0_C:
    case OP_BIT_0_D:
    case OP_BIT_0_E:
    case OP_BIT_0_H:
    case OP_BIT_0_L:
    case OP_BIT_0_HLi:
    case OP_BIT_1_A:
    case OP_BIT_1_B:
    case OP_BIT_1_C:
    case OP_BIT_1_D:
    case OP_BIT_1_E:
    case OP_BIT_1_H:
    case OP_BIT_1_L:
    case OP_BIT_1_HLi:
    case OP_BIT_2_A:
    case OP_BIT_2_B:
    case OP_BIT_2_C:
    case OP_BIT_2_D:
    case OP_BIT_2_E:
    case OP_BIT_2_H:
    case OP_BIT_2_L:
    case OP_BIT_2_HLi:
    case OP_BIT_3_A:
    case OP_BIT_3_B:
    case OP_BIT_3_C:
    case OP_BIT_3_D:
    case OP_BIT_3_E:
    case OP_BIT_3_H:
    case OP_BIT_3_L:
    case OP_BIT_3_HLi:
    case OP_BIT_4_A:
    case OP_BIT_4_B:
    case OP_BIT_4_C:
    case OP_BIT_4_D:
    case OP_BIT_4_E:
    case OP_BIT_4_H:
    case OP_BIT_4_L:
    case OP_BIT_4_HLi:
    case OP_BIT_5_A:
    case OP_BIT_5_B:
    case OP_BIT_5_C:
    case OP_BIT_5_D:
    case OP_BIT_5_E:
    case OP_BIT_5_H:
    case OP_BIT_5_L:
    case OP_BIT_5_HLi:
    case OP_BIT_6_A:
    case OP_BIT_6_B:
    case OP_BIT_6_C:
    case OP_BIT_6_D:
    case OP_BIT_6_E:
    case OP_BIT_6_H:
    case OP_BIT_6_L:
    case OP_BIT_6_HLi:
    case OP_BIT_7_A:
    case OP_BIT_7_B:
    case OP_BIT_7_C:
    case OP_BIT_7_D:
    case OP_BIT_7_E:
    case OP_BIT_7_H:
    case OP_BIT_7_L:
    case OP_BIT_7_HLi:
    {
        bit_u3_r8(gb, opcode);
        break;
    }

    case OP_RES_0_A:
    case OP_RES_0_B:
    case OP_RES_0_C:
    case OP_RES_0_D:
    case OP_RES_0_E:
    case OP_RES_0_H:
    case OP_RES_0_L:
    case OP_RES_1_A:
    case OP_RES_1_B:
    case OP_RES_1_C:
    case OP_RES_1_D:
    case OP_RES_1_E:
    case OP_RES_1_H:
    case OP_RES_1_L:
    case OP_RES_2_A:
    case OP_RES_2_B:
    case OP_RES_2_C:
    case OP_RES_2_D:
    case OP_RES_2_E:
    case OP_RES_2_H:
    case OP_RES_2_L:
    case OP_RES_3_A:
    case OP_RES_3_B:
    case OP_RES_3_C:
    case OP_RES_3_D:
    case OP_RES_3_E:
    case OP_RES_3_H:
    case OP_RES_3_L:
    case OP_RES_4_A:
    case OP_RES_4_B:
    case OP_RES_4_C:
    case OP_RES_4_D:
    case OP_RES_4_E:
    case OP_RES_4_H:
    case OP_RES_4_L:
    case OP_RES_5_A:
    case OP_RES_5_B:
    case OP_RES_5_C:
    case OP_RES_5_D:
    case OP_RES_5_E:
    case OP_RES_5_H:
    case OP_RES_5_L:
    case OP_RES_6_A:
    case OP_RES_6_B:
    case OP_RES_6_C:
    case OP_RES_6_D:
    case OP_RES_6_E:
    case OP_RES_6_H:
    case OP_RES_6_L:
    case OP_RES_7_A:
    case OP_RES_7_B:
    case OP_RES_7_C:
    case OP_RES_7_D:
    case OP_RES_7_E:
    case OP_RES_7_H:
    case OP_RES_7_L:
    {
        res_u3_r8(gb, opcode);
        break;
    }

    case OP_RES_0_HLi:
    case OP_RES_1_HLi:
    case OP_RES_2_HLi:
    case OP_RES_3_HLi:
    case OP_RES_4_HLi:
    case OP_RES_5_HLi:
    case OP_RES_6_HLi:
    case OP_RES_7_HLi:
    {
        res_u3_hli(gb, opcode);
        break;
    }

    case OP_SET_0_A:
    case OP_SET_0_B:
    case OP_SET_0_C:
    case OP_SET_0_D:
    case OP_SET_0_E:
    case OP_SET_0_H:
    case OP_SET_0_L:
    case OP_SET_1_A:
    case OP_SET_1_B:
    case OP_SET_1_C:
    case OP_SET_1_D:
    case OP_SET_1_E:
    case OP_SET_1_H:
    case OP_SET_1_L:
    case OP_SET_2_A:
    case OP_SET_2_B:
    case OP_SET_2_C:
    case OP_SET_2_D:
    case OP_SET_2_E:
    case OP_SET_2_H:
    case OP_SET_2_L:
    case OP_SET_3_A:
    case OP_SET_3_B:
    case OP_SET_3_C:
    case OP_SET_3_D:
    case OP_SET_3_E:
    case OP_SET_3_H:
    case OP_SET_3_L:
    case OP_SET_4_A:
    case OP_SET_4_B:
    case OP_SET_4_C:
    case OP_SET_4_D:
    case OP_SET_4_E:
    case OP_SET_4_H:
    case OP_SET_4_L:
    case OP_SET_5_A:
    case OP_SET_5_B:
    case OP_SET_5_C:
    case OP_SET_5_D:
    case OP_SET_5_E:
    case OP_SET_5_H:
    case OP_SET_5_L:
    case OP_SET_6_A:
    case OP_SET_6_B:
    case OP_SET_6_C:
    case OP_SET_6_D:
    case OP_SET_6_E:
    case OP_SET_6_H:
    case OP_SET_6_L:
    case OP_SET_7_A:
    case OP_SET_7_B:
    case OP_SET_7_C:
    case OP_SET_7_D:
    case OP_SET_7_E:
    case OP_SET_7_H:
    case OP_SET_7_L:
    {
        set_u3_r8(gb, opcode);
        break;
    }

    case OP_SET_0_HLi:
    case OP_SET_1_HLi:
    case OP_SET_2_HLi:
    case OP_SET_3_HLi:
    case OP_SET_4_HLi:
    case OP_SET_5_HLi:
    case OP_SET_6_HLi:
    case OP_SET_7_HLi:
    {
        set_u3_hli(gb, opcode);
        break;
    }

    case OP_SWAP_A:
    case OP_SWAP_B:
    case OP_SWAP_C:
    case OP_SWAP_D:
    case OP_SWAP_E:
    case OP_SWAP_H:
    case OP_SWAP_L:
    {
        swap_r8(gb, opcode);
        break;
    }

    case OP_SWAP_HLi:
    {
        swap_hli(gb);
        break;
    }

    case OP_RL_A:
    case OP_RL_B:
    case OP_RL_C:
    case OP_RL_D:
    case OP_RL_E:
    case OP_RL_H:
    case OP_RL_L:
    {
        rl_r8(gb, opcode);
        break;
    }

    case OP_RL_HLi:
    {
        rl_hli(gb);
        break;
    }

    case OP_RLC_A:
    case OP_RLC_B:
    case OP_RLC_C:
    case OP_RLC_D:
    case OP_RLC_E:
    case OP_RLC_H:
    case OP_RLC_L:
    {
        rlc_r8(gb, opcode);
        break;
    }

    case OP_RLC_HLi:
    {
        rlc_hli(gb);
        break;
    }

    case OP_RR_A:
    case OP_RR_B:
    case OP_RR_C:
    case OP_RR_D:
    case OP_RR_E:
    case OP_RR_H:
    case OP_RR_L:
    {
        rr_r8(gb, opcode);
        break;
    }

    case OP_RR_HLi:
    {
        rr_hli(gb);
        break;
    }

    case OP_RRC_A:
    case OP_RRC_B:
    case OP_RRC_C:
    case OP_RRC_D:
    case OP_RRC_E:
    case OP_RRC_H:
    case OP_RRC_L:
    {
        rrc_r8(gb, opcode);
        break;
    }

    case OP_RRC_HLi:
    {
        rrc_hli(gb);
        break;
    }

    case OP_SLA_A:
    case OP_SLA_B:
    case OP_SLA_C:
    case OP_SLA_D:
    case OP_SLA_E:
    case OP_SLA_H:
    case OP_SLA_L:
    {
        sla_r8(gb, opcode);
        break;
    }

    case OP_SLA_HLi:
    {
        sla_hli(gb);
        break;
    }

    case OP_SRA_A:
    case OP_SRA_B:
    case OP_SRA_C:
    case OP_SRA_D:
    case OP_SRA_E:
    case OP_SRA_H:
    case OP_SRA_L:
    {
        sra_r8(gb, opcode);
        break;
    }

    case OP_SRA_HLi:
    {
        sra_hli(gb);
        break;
    }

    case OP_SRL_A:
    case OP_SRL_B:
    case OP_SRL_C:
    case OP_SRL_D:
    case OP_SRL_E:
    case OP_SRL_H:
    case OP_SRL_L:
    {
        srl_r8(gb, opcode);
        break;
    }

    case OP_SRL_HLi:
    {
        srl_hli(gb);
        break;
    }

    default:
    {
        printf("Unhandled 0xCB prefixed opcode: 0x%02X\n", opcode);
        assert(!"Unhandled 0xCB prefixed opcode");
        break;
    }
    }
}

/**
 * @brief Run one cpu cycle
 *
 * @param gb pointer to the gameboy state struct
 */
void cpu_run(gameboy_t *gb)
{
    uint16_t current_cycles = gb->m_cycles;

    if (gb->stopped)
    {
        return;
    }

    if (!gb->halted)
    {
        // We don't have to advance the cycles here because
        // fetch and execute overlap (except for the first fetch, which we accept)
        uint8_t opcode = mem_read_byte(gb, gb->pc++);

        if (opcode == 0xCB)
        {
            opcode = mem_read_byte(gb, gb->pc++);
            execute_cb_opcode(gb, opcode);
        }
        else
        {
            execute_opcode(gb, opcode);
        }

        uint16_t cycles_passed = gb->m_cycles - current_cycles;
    }
}