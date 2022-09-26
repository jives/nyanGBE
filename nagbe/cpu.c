#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "opcodes.h"

// void execute_opcode(uint8_t opcode)
// {
//     switch(opcode)
//     {
//         // noop
//         case 0x00: cpu_noop(); break;

//         /* 8-bit load instructions */
//         // LD register, value (8-bit)
//         case 0x06: cpu_8bit_load(registers.B); break;
//         case 0x0E: cpu_8bit_load(registers.C); break;
//         case 0x16: cpu_8bit_load(registers.D); break;
//         case 0x1E: cpu_8bit_load(registers.E); break;
//         case 0x26: cpu_8bit_load(registers.H); break;
//         case 0x2E: cpu_8bit_load(registers.L); break;
        
//         default:
//             printf("Unhandled Extended Opcode %x", opcode);
//             assert(!"Unhandled opcode");
//             break;
//     }
// }

int main(int argc, char *argv[])
{
    printf("size flags: %d\n bytes", sizeof(registers.f));
    registers.a = 0x02;
    registers.f = 0x01;
    printf("%x, %x\n", registers.a, registers.f);
    registers.af = 0xABCD;
    printf("%x, %x\n", registers.a, registers.f);
    FLAGS_CLEAR(c);
    printf("%x, %x\n", registers.a, registers.f);
    return 0;
}