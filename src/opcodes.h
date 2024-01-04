#pragma once
#include <stdint.h>

/**
 * Opcodes taken from https://github.com/izik1/gbops
 * Naming convention:
 * i8: 8-bit signed data (immediate)
 * u8: 8-bit unsigned data (immediate)
 * u16: 16-bit unsigned data (immediate)
 * a16: 16-bit address (always indirect)
 * ..i: indirect access (pointer (..))
 * ..p: increment register ..+
 * ..m: decrement register ..-
 */

/**
 * Standard 8-bit instructions
 */
// 0x0x
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

// 0x1x
#define OP_STOP         0x10
#define OP_LD_DE_u16    0x11
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
#define OP_RRA          0x1F

// 0x2x
#define OP_JR_NZ_i8     0x20
#define OP_LD_HL_u16    0x21
#define OP_LD_HLpi_A    0x22
#define OP_INC_HL       0x23
#define OP_INC_H        0x24
#define OP_DEC_H        0x25
#define OP_LD_H_u8      0x26
#define OP_DAA          0x27
#define OP_JR_Z_i8      0x28
#define OP_ADD_HL_HL    0x29
#define OP_LD_A_HLpi    0x2A
#define OP_DEC_HL       0x2B
#define OP_INC_L        0x2C
#define OP_DEC_L        0x2D
#define OP_LD_L_u8      0x2E
#define OP_CPL          0x2F

// 0x3x
#define OP_JR_NC_i8     0x30
#define OP_LD_SP_u16    0x31
#define OP_LD_HLmi_A    0x32
#define OP_INC_SP       0x33
#define OP_INC_HLi      0x34
#define OP_DEC_HLi      0x35
#define OP_LD_HLi_u8    0x36
#define OP_SCF          0x37
#define OP_JR_C_i8      0x38
#define OP_ADD_HL_SP    0x39
#define OP_LD_A_HLmi    0x3A
#define OP_DEC_SP       0x3B
#define OP_INC_A        0x3C
#define OP_DEC_A        0x3D
#define OP_LD_A_u8      0x3E
#define OP_CCF          0x3F

// 0x4x
#define OP_LD_B_B       0x40
#define OP_LD_B_C       0x41
#define OP_LD_B_D       0x42
#define OP_LD_B_E       0x43
#define OP_LD_B_H       0x44
#define OP_LD_B_L       0x45
#define OP_LD_B_HLi     0x46
#define OP_LD_B_A       0x47
#define OP_LD_C_B       0x48
#define OP_LD_C_C       0x49
#define OP_LD_C_D       0x4A
#define OP_LD_C_E       0x4B
#define OP_LD_C_H       0x4C
#define OP_LD_C_L       0x4D
#define OP_LD_C_HLi     0x4E
#define OP_LD_C_A       0x4F

// 0x5x
#define OP_LD_D_B       0x50
#define OP_LD_D_C       0x51
#define OP_LD_D_D       0x52
#define OP_LD_D_E       0x53
#define OP_LD_D_H       0x54
#define OP_LD_D_L       0x55
#define OP_LD_D_HLi     0x56
#define OP_LD_D_A       0x57
#define OP_LD_E_B       0x58
#define OP_LD_E_C       0x59
#define OP_LD_E_D       0x5A
#define OP_LD_E_E       0x5B
#define OP_LD_E_H       0x5C
#define OP_LD_E_L       0x5D
#define OP_LD_E_HLi     0x5E
#define OP_LD_E_A       0x5F

// 0x6x
#define OP_LD_H_B       0x60
#define OP_LD_H_C       0x61
#define OP_LD_H_D       0x62
#define OP_LD_H_E       0x63
#define OP_LD_H_H       0x64
#define OP_LD_H_L       0x65
#define OP_LD_H_HLi     0x66
#define OP_LD_H_A       0x67
#define OP_LD_L_B       0x68
#define OP_LD_L_C       0x69
#define OP_LD_L_D       0x6A
#define OP_LD_L_E       0x6B
#define OP_LD_L_H       0x6C
#define OP_LD_L_L       0x6D
#define OP_LD_L_HLi     0x6E
#define OP_LD_L_A       0x6F

// 0x7x
#define OP_LD_HLi_B     0x70
#define OP_LD_HLi_C     0x71
#define OP_LD_HLi_D     0x72
#define OP_LD_HLi_E     0x73
#define OP_LD_HLi_H     0x74
#define OP_LD_HLi_L     0x75
#define OP_HALT         0x76
#define OP_LD_HLi_A     0x77
#define OP_LD_A_B       0x78
#define OP_LD_A_C       0x79
#define OP_LD_A_D       0x7A
#define OP_LD_A_E       0x7B
#define OP_LD_A_H       0x7C
#define OP_LD_A_L       0x7D
#define OP_LD_A_HLi     0x7E
#define OP_LD_A_A       0x7F

// 0x8x
#define OP_ADD_A_B      0x80
#define OP_ADD_A_C      0x81
#define OP_ADD_A_D      0x82
#define OP_ADD_A_E      0x83
#define OP_ADD_A_H      0x84
#define OP_ADD_A_L      0x85
#define OP_ADD_A_HLi    0x86
#define OP_ADD_A_A      0x87
#define OP_ADC_A_B      0x88
#define OP_ADC_A_C      0x89
#define OP_ADC_A_D      0x8A
#define OP_ADC_A_E      0x8B
#define OP_ADC_A_H      0x8C
#define OP_ADC_A_L      0x8D
#define OP_ADC_A_HLi    0x8E
#define OP_ADC_A_A      0x8F

// 0x9x
#define OP_SUB_A_B      0x90
#define OP_SUB_A_C      0x91
#define OP_SUB_A_D      0x92
#define OP_SUB_A_E      0x93
#define OP_SUB_A_H      0x94
#define OP_SUB_A_L      0x95
#define OP_SUB_A_HLi    0x96
#define OP_SUB_A_A      0x97
#define OP_SBC_A_B      0x98
#define OP_SBC_A_C      0x99
#define OP_SBC_A_D      0x9A
#define OP_SBC_A_E      0x9B
#define OP_SBC_A_H      0x9C
#define OP_SBC_A_L      0x9D
#define OP_SBC_A_HLi    0x9E
#define OP_SBC_A_A      0x9F

// 0xAx
#define OP_AND_A_B      0xA0
#define OP_AND_A_C      0xA1
#define OP_AND_A_D      0xA2
#define OP_AND_A_E      0xA3
#define OP_AND_A_H      0xA4
#define OP_AND_A_L      0xA5
#define OP_AND_A_HLi    0xA6
#define OP_AND_A_A      0xA7
#define OP_XOR_A_B      0xA8
#define OP_XOR_A_C      0xA9
#define OP_XOR_A_D      0xAA
#define OP_XOR_A_E      0xAB
#define OP_XOR_A_H      0xAC
#define OP_XOR_A_L      0xAD
#define OP_XOR_A_HLi    0xAE
#define OP_XOR_A_A      0xAF

// 0xBx
#define OP_OR_A_B       0xB0
#define OP_OR_A_C       0xB1
#define OP_OR_A_D       0xB2
#define OP_OR_A_E       0xB3
#define OP_OR_A_H       0xB4
#define OP_OR_A_L       0xB5
#define OP_OR_A_HLi     0xB6
#define OP_OR_A_A       0xB7
#define OP_CP_A_B       0xB8
#define OP_CP_A_C       0xB9
#define OP_CP_A_D       0xBA
#define OP_CP_A_E       0xBB
#define OP_CP_A_H       0xBC
#define OP_CP_A_L       0xBD
#define OP_CP_A_HLi     0xBE
#define OP_CP_A_A       0xBF

// 0xCx
#define OP_RET_NZ       0xC0
#define OP_POP_BC       0xC1
#define OP_JP_NZ_u16    0xC2
#define OP_JP_u16       0xC3
#define OP_CALL_NZ_u16  0xC4
#define OP_PUSH_BC      0xC5
#define OP_ADD_A_u8     0xC6
#define OP_RST_00h      0xC7
#define OP_RET_Z        0xC8
#define OP_RET          0xC9
#define OP_JP_Z_u16     0xCA
#define OP_PREFIX_CB    0xCB
#define OP_CALL_Z_u16   0xCC
#define OP_CALL_u16     0xCD
#define OP_ADC_A_u8     0xCE
#define OP_RST_08h      0xCF

// 0xDx
#define OP_RET_NC       0xD0
#define OP_POP_DE       0xD1
#define OP_JP_NC_u16    0xD2
//#define OP_UNDEFINED  0xD3
#define OP_CALL_NC_u16  0xD4
#define OP_PUSH_DE      0xD5
#define OP_SUB_A_u8     0xD6
#define OP_RST_10h      0xD7
#define OP_RET_C        0xD8
#define OP_RETI         0xD9
#define OP_JP_C_u16     0xDA
//#define OP_UNDEFINED  0xDB
#define OP_CALL_C_u16   0xDC
//#define OP_UNDEFINED  0xDD
#define OP_SBC_A_u8     0xDE
#define OP_RST_18h      0xDF

// 0xEx
#define OP_LDH_u16i_A   0xE0
#define OP_POP_HL       0xE1
#define OP_LDH_Ci_A     0xE2
//#define OP_UNDEFINED  0xE3
//#define OP_UNDEFINED  0xE4
#define OP_PUSH_HL      0xE5
#define OP_AND_A_u8     0xE6
#define OP_RST_20h      0xE7
#define OP_ADD_SP_i8    0xE8
#define OP_JP_HL        0xE9
#define OP_LD_u16i_A    0xEA
//#define OP_UNDEFINED  0xEB
//#define OP_UNDEFINED  0xEC
//#define OP_UNDEFINED  0xED
#define OP_XOR_A_u8     0xEE
#define OP_RST_28h      0xEF

// 0xFx
#define OP_LDH_A_u16i   0xF0
#define OP_POP_AF       0xF1
#define OP_LDH_A_Ci     0xF2
#define OP_DI           0xF3
//#define OP_UNDEFINED  0xF4
#define OP_PUSH_AF      0xF5
#define OP_OR_A_u8      0xF6
#define OP_RST_30h      0xF7
#define OP_LD_HL_SP_i8  0xF8
#define OP_LD_SP_HL     0xF9
#define OP_LD_A_u16i    0xFA
#define OP_EI           0xFB
//#define OP_UNDEFINED  0xFC
//#define OP_UNDEFINED  0xFD
#define OP_CP_A_u8      0xFE
#define OP_RST_38h      0xFF

/**
 * Prefix 0xCB ("16-bit") instructions
 */
// 0x0x
#define OP_RLC_B        0x00
#define OP_RLC_C        0x01
#define OP_RLC_D        0x02
#define OP_RLC_E        0x03
#define OP_RLC_H        0x04
#define OP_RLC_L        0x05
#define OP_RLC_HLi      0x06
#define OP_RLC_A        0x07
#define OP_RRC_B        0x08
#define OP_RRC_C        0x09
#define OP_RRC_D        0x0A
#define OP_RRC_E        0x0B
#define OP_RRC_H        0x0C
#define OP_RRC_L        0x0D
#define OP_RRC_HLi      0x0E
#define OP_RRC_A        0x0F

// 0x1x
#define OP_RL_B         0x10
#define OP_RL_C         0x11
#define OP_RL_D         0x12
#define OP_RL_E         0x13
#define OP_RL_H         0x14
#define OP_RL_L         0x15
#define OP_RL_HLi       0x16
#define OP_RL_A         0x17
#define OP_RR_B         0x18
#define OP_RR_C         0x19
#define OP_RR_D         0x1A
#define OP_RR_E         0x1B
#define OP_RR_H         0x1C
#define OP_RR_L         0x1D
#define OP_RR_HLi       0x1E
#define OP_RR_A         0x1F

// 0x2x
#define OP_SLA_B        0x20
#define OP_SLA_C        0x21
#define OP_SLA_D        0x22
#define OP_SLA_E        0x23
#define OP_SLA_H        0x24
#define OP_SLA_L        0x25
#define OP_SLA_HLi      0x26
#define OP_SLA_A        0x27
#define OP_SRA_B        0x28
#define OP_SRA_C        0x29
#define OP_SRA_D        0x2A
#define OP_SRA_E        0x2B
#define OP_SRA_H        0x2C
#define OP_SRA_L        0x2D
#define OP_SRA_HLi      0x2E
#define OP_SRA_A        0x2F

// 0x3x
#define OP_SWAP_B       0x30
#define OP_SWAP_C       0x31
#define OP_SWAP_D       0x32
#define OP_SWAP_E       0x33
#define OP_SWAP_H       0x34
#define OP_SWAP_L       0x35
#define OP_SWAP_HLi     0x36
#define OP_SWAP_A       0x37
#define OP_SRL_B        0x38
#define OP_SRL_C        0x39
#define OP_SRL_D        0x3A
#define OP_SRL_E        0x3B
#define OP_SRL_H        0x3C
#define OP_SRL_L        0x3D
#define OP_SRL_HLi      0x3E
#define OP_SRL_A        0x3F

// 0x4x
#define OP_BIT_0_B      0x40
#define OP_BIT_0_C      0x41
#define OP_BIT_0_D      0x42
#define OP_BIT_0_E      0x43
#define OP_BIT_0_H      0x44
#define OP_BIT_0_L      0x45
#define OP_BIT_0_HLi    0x46
#define OP_BIT_0_A      0x47
#define OP_BIT_1_B      0x48
#define OP_BIT_1_C      0x49
#define OP_BIT_1_D      0x4A
#define OP_BIT_1_E      0x4B
#define OP_BIT_1_H      0x4C
#define OP_BIT_1_L      0x4D
#define OP_BIT_1_HLi    0x4E
#define OP_BIT_1_A      0x4F

// 0x5x
#define OP_BIT_2_B      0x50
#define OP_BIT_2_C      0x51
#define OP_BIT_2_D      0x52
#define OP_BIT_2_E      0x53
#define OP_BIT_2_H      0x54
#define OP_BIT_2_L      0x55
#define OP_BIT_2_HLi    0x56
#define OP_BIT_2_A      0x57
#define OP_BIT_3_B      0x58
#define OP_BIT_3_C      0x59
#define OP_BIT_3_D      0x5A
#define OP_BIT_3_E      0x5B
#define OP_BIT_3_H      0x5C
#define OP_BIT_3_L      0x5D
#define OP_BIT_3_HLi    0x5E
#define OP_BIT_3_A      0x5F

// 0x6x
#define OP_BIT_4_B      0x60
#define OP_BIT_4_C      0x61
#define OP_BIT_4_D      0x62
#define OP_BIT_4_E      0x63
#define OP_BIT_4_H      0x64
#define OP_BIT_4_L      0x65
#define OP_BIT_4_HLi    0x66
#define OP_BIT_4_A      0x67
#define OP_BIT_5_B      0x68
#define OP_BIT_5_C      0x69
#define OP_BIT_5_D      0x6A
#define OP_BIT_5_E      0x6B
#define OP_BIT_5_H      0x6C
#define OP_BIT_5_L      0x6D
#define OP_BIT_5_HLi    0x6E
#define OP_BIT_5_A      0x6F

// 0x7x
#define OP_BIT_6_B      0x70
#define OP_BIT_6_C      0x71
#define OP_BIT_6_D      0x72
#define OP_BIT_6_E      0x73
#define OP_BIT_6_H      0x74
#define OP_BIT_6_L      0x75
#define OP_BIT_6_HLi    0x76
#define OP_BIT_6_A      0x77
#define OP_BIT_7_B      0x78
#define OP_BIT_7_C      0x79
#define OP_BIT_7_D      0x7A
#define OP_BIT_7_E      0x7B
#define OP_BIT_7_H      0x7C
#define OP_BIT_7_L      0x7D
#define OP_BIT_7_HLi    0x7E
#define OP_BIT_7_A      0x7F

// 0x8x
#define OP_RES_0_B      0x80
#define OP_RES_0_C      0x81
#define OP_RES_0_D      0x82
#define OP_RES_0_E      0x83
#define OP_RES_0_H      0x84
#define OP_RES_0_L      0x85
#define OP_RES_0_HLi    0x86
#define OP_RES_0_A      0x87
#define OP_RES_1_B      0x88
#define OP_RES_1_C      0x89
#define OP_RES_1_D      0x8A
#define OP_RES_1_E      0x8B
#define OP_RES_1_H      0x8C
#define OP_RES_1_L      0x8D
#define OP_RES_1_HLi    0x8E
#define OP_RES_1_A      0x8F

// 0x9x
#define OP_RES_2_B      0x90
#define OP_RES_2_C      0x91
#define OP_RES_2_D      0x92
#define OP_RES_2_E      0x93
#define OP_RES_2_H      0x94
#define OP_RES_2_L      0x95
#define OP_RES_2_HLi    0x96
#define OP_RES_2_A      0x97
#define OP_RES_3_B      0x98
#define OP_RES_3_C      0x99
#define OP_RES_3_D      0x9A
#define OP_RES_3_E      0x9B
#define OP_RES_3_H      0x9C
#define OP_RES_3_L      0x9D
#define OP_RES_3_HLi    0x9E
#define OP_RES_3_A      0x9F

// 0xAx
#define OP_RES_4_B      0xA0
#define OP_RES_4_C      0xA1
#define OP_RES_4_D      0xA2
#define OP_RES_4_E      0xA3
#define OP_RES_4_H      0xA4
#define OP_RES_4_L      0xA5
#define OP_RES_4_HLi    0xA6
#define OP_RES_4_A      0xA7
#define OP_RES_5_B      0xA8
#define OP_RES_5_C      0xA9
#define OP_RES_5_D      0xAA
#define OP_RES_5_E      0xAB
#define OP_RES_5_H      0xAC
#define OP_RES_5_L      0xAD
#define OP_RES_5_HLi    0xAE
#define OP_RES_5_A      0xAF

// 0xBx
#define OP_RES_6_B      0xB0
#define OP_RES_6_C      0xB1
#define OP_RES_6_D      0xB2
#define OP_RES_6_E      0xB3
#define OP_RES_6_H      0xB4
#define OP_RES_6_L      0xB5
#define OP_RES_6_HLi    0xB6
#define OP_RES_6_A      0xB7
#define OP_RES_7_B      0xB8
#define OP_RES_7_C      0xB9
#define OP_RES_7_D      0xBA
#define OP_RES_7_E      0xBB
#define OP_RES_7_H      0xBC
#define OP_RES_7_L      0xBD
#define OP_RES_7_HLi    0xBE
#define OP_RES_7_A      0xBF

// 0xCx
#define OP_SET_0_B      0xC0
#define OP_SET_0_C      0xC1
#define OP_SET_0_D      0xC2
#define OP_SET_0_E      0xC3
#define OP_SET_0_H      0xC4
#define OP_SET_0_L      0xC5
#define OP_SET_0_HLi    0xC6
#define OP_SET_0_A      0xC7
#define OP_SET_1_B      0xC8
#define OP_SET_1_C      0xC9
#define OP_SET_1_D      0xCA
#define OP_SET_1_E      0xCB
#define OP_SET_1_H      0xCC
#define OP_SET_1_L      0xCD
#define OP_SET_1_HLi    0xCE
#define OP_SET_1_A      0xCF

// 0xDx
#define OP_SET_2_B      0xD0
#define OP_SET_2_C      0xD1
#define OP_SET_2_D      0xD2
#define OP_SET_2_E      0xD3
#define OP_SET_2_H      0xD4
#define OP_SET_2_L      0xD5
#define OP_SET_2_HLi    0xD6
#define OP_SET_2_A      0xD7
#define OP_SET_3_B      0xD8
#define OP_SET_3_C      0xD9
#define OP_SET_3_D      0xDA
#define OP_SET_3_E      0xDB
#define OP_SET_3_H      0xDC
#define OP_SET_3_L      0xDD
#define OP_SET_3_HLi    0xDE
#define OP_SET_3_A      0xDF

// 0xEx
#define OP_SET_4_B      0xE0
#define OP_SET_4_C      0xE1
#define OP_SET_4_D      0xE2
#define OP_SET_4_E      0xE3
#define OP_SET_4_H      0xE4
#define OP_SET_4_L      0xE5
#define OP_SET_4_HLi    0xE6
#define OP_SET_4_A      0xE7
#define OP_SET_5_B      0xE8
#define OP_SET_5_C      0xE9
#define OP_SET_5_D      0xEA
#define OP_SET_5_E      0xEB
#define OP_SET_5_H      0xEC
#define OP_SET_5_L      0xED
#define OP_SET_5_HLi    0xEE
#define OP_SET_5_A      0xEF

// 0xFx
#define OP_SET_6_B      0xF0
#define OP_SET_6_C      0xF1
#define OP_SET_6_D      0xF2
#define OP_SET_6_E      0xF3
#define OP_SET_6_H      0xF4
#define OP_SET_6_L      0xF5
#define OP_SET_6_HLi    0xF6
#define OP_SET_6_A      0xF7
#define OP_SET_7_B      0xF8
#define OP_SET_7_C      0xF9
#define OP_SET_7_D      0xFA
#define OP_SET_7_E      0xFB
#define OP_SET_7_H      0xFC
#define OP_SET_7_L      0xFD
#define OP_SET_7_HLi    0xFE
#define OP_SET_7_A      0xFF

const uint8_t opcode_cycles[256] = {
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
    1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    2, 3, 2, 2, 1, 1, 2, 1, 2, 2, 2, 2, 1, 1, 2, 1,
    2, 3, 2, 2, 3, 3, 3, 1, 2, 2, 2, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 3, 3, 4, 3, 4, 2, 4, 2, 4, 3, 0, 3, 6, 2, 4,
    2, 3, 3, 0, 3, 4, 2, 4, 2, 4, 3, 0, 3, 0, 2, 4,
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4
};

const uint8_t opcode_cycles_branched[256] = {
    1, 3, 2, 2, 1, 1, 2, 1, 5, 2, 2, 2, 1, 1, 2, 1,
    1, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    3, 3, 2, 2, 1, 1, 2, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    3, 3, 2, 2, 3, 3, 3, 1, 3, 2, 2, 2, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
    5, 3, 4, 4, 6, 4, 2, 4, 5, 4, 4, 0, 6, 6, 2, 4,
    5, 3, 4, 0, 6, 4, 2, 4, 5, 4, 4, 0, 6, 0, 2, 4,
    3, 3, 2, 0, 0, 4, 2, 4, 4, 1, 4, 0, 0, 0, 2, 4,
    3, 3, 2, 1, 0, 4, 2, 4, 3, 2, 4, 1, 0, 0, 2, 4
};

const uint8_t opcode_cycles_cb[256] = {
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2,
    2, 2, 2, 2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 4, 2
};