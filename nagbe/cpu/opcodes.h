#pragma once

/**
 * Opcodes taken from https://github.com/izik1/gbops
 * i8: 8-bit signed data (direct)
 * u8: 8-bit unsigned data (direct)
 * u16: 16-bit unsigned data (direct)
 * a16: 16-bit address (always indirect)
 * *i: indirect access (pointer)
 */

/* 8-bit instructions */
// 00
#define OP_NOP          0x00
#define OP_LD_BC_u16    0x01
#define OP_LD_BCi_A     0x02
#define OP_INC_BC       0x03
#define OP_INC_B        0x04
#define OP_DEC_B        0x05
#define OP_LD_B_u8      0x06
#define OP_RLCA         0x07
#define OP_LD_a16i_SP   0x08
#define OP_ADD_HL_BC    0x09
#define OP_LD_A_BCi     0x0A
#define OP_DEC_BC       0x0B
#define OP_INC_C        0x0C
#define OP_DEC_C        0x0D
#define OP_LD_C_u8      0x0E
#define OP_RRCA         0x0F

// 10
#define OP_STOP         0x10
#define OP_LD_DE_u18    0x11
#define OP_LD_DEi_A     0x12
#define OP_INC_DE       0x13
#define OP_INC_D        0x14
#define OP_DEC_D        0x15
#define OP_LD_D_u8      0x16
#define OP_RLA          0x17
#define OP_JR_i8        0x18
#define OP_ADD_HL_DE    0x19
#define OP_LD_A_DEi     0x1A
#define OP_DEC_DE       0x1B
#define OP_INC_E        0x1C
#define OP_DEC_E        0x1D
#define OP_LD_E_u8      0x1E
#define OP_RRA          0x1E

// 20
#define OP_LD_H 0x26
#define OP_LD_L 0x2E

#define OP_LD_A_A       0x7F
#define OP_LD_A_B       0x78
#define OP_LD_A_C       0x79
#define OP_LD_A_D       0x7A
#define OP_LD_A_E       0x7B
#define OP_LD_A_H       0x7C
#define OP_LD_A_L       0x7D
#define OP_LD_A_HL      0x7E
#define OP_LD_B_B       0x40
#define OP_LD_B_C       0x41
#define OP_LD_B_D       0x42
#define OP_LD_B_E       0x43
#define OP_LD_B_H       0x44
#define OP_LD_B_L       0x45
#define OP_LD_B_HL      0x46
#define OP_LD_C_B       0x48
#define OP_LD_C_C       0x49
#define OP_LD_C_D       0x4A
#define OP_LD_C_E       0x4B
#define OP_LD_C_H       0x4C
#define OP_LD_C_L       0x4D
#define OP_LD_C_HL      0x4E
#define OP_LD_D_B       0x50
#define OP_LD_D_C       0x51
#define OP_LD_D_D       0x52
#define OP_LD_D_E       0x53
#define OP_LD_D_H       0x54
#define OP_LD_D_L       0x55
#define OP_LD_D_HL      0x56
#define OP_LD_E_B       0x58
#define OP_LD_E_C       0x59
#define OP_LD_E_D       0x5A
#define OP_LD_E_E       0x5B
#define OP_LD_E_H       0x5C
#define OP_LD_E_L       0x5D
#define OP_LD_E_HL      0x5E
#define OP_LD_H_B       0x60
#define OP_LD_H_C       0x61
#define OP_LD_H_D       0x62
#define OP_LD_H_E       0x63
#define OP_LD_H_H       0x64
#define OP_LD_H_L       0x65
#define OP_LD_H_HL      0x66
#define OP_LD_L_B       0x68
#define OP_LD_L_C       0x69
#define OP_LD_L_D       0x6A
#define OP_LD_L_E       0x6B
#define OP_LD_L_H       0x6C
#define OP_LD_L_L       0x6D
#define OP_LD_L_HL      0x6E
#define OP_LD_HL_B      0x70
#define OP_LD_HL_C      0x71
#define OP_LD_HL_D      0x72
#define OP_LD_HL_E      0x73
#define OP_LD_HL_H      0x74
#define OP_LD_HL_L      0x75

// LD address, value (8-bit)
#define OP_LD_HL        0x36
//...