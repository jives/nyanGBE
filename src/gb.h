#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

/* Constants */
#define GB_NUM_REG_8_BIT 8
#define GB_NUM_REG_16_BIT 6
#define GB_CLOCK_SPEED_HZ 4194304

/* Flags */
typedef enum __attribute__((packed)) flags
{
    c = 1 << 4, // Carry flag
    h = 1 << 5, // Half-carry flag
    n = 1 << 6, // Subtraction flag
    z = 1 << 7  // Zero flag
} flags_t;

/* Interrupts */
typedef enum __attribute__((packed)) interrupts
{
    IR_VBLANK = 1 << 0,
    IR_LCD = 1 << 1,
    IR_TIMER = 1 << 2,
    IR_SERIAL = 1 << 3,
    IR_JOYPAD = 1 << 4
} interrupts_t;

/* Memory Registers located in 0xFF00 - 0xFFFF*/
/* see https://gbdev.io/pandocs/Hardware_Reg_List.html */
enum
{
    GB_JOYP = 0x00, // Joypad
    GB_SB = 0x01,   // Serial transfer data
    GB_SC = 0x02,   // Serial transfer control
    GB_DIV = 0x04,  // Divider register
    GB_TIMA = 0x05, // Timer counter
    GB_TMA = 0x06,  // Timer modulo
    GB_TAC = 0x07,  // Timer control
    GB_IF = 0x0F,   // Interrupt flag (R/W)
    GB_NR10 = 0x10, // Sound channel 1 sweep
    GB_NR11 = 0x11, // Sound channel 1 length timer & duty cycle
    GB_NR12 = 0x12, // Sound channel 1 volume & envelope
    GB_NR13 = 0x13, // Sound channel 1 period low
    GB_NR14 = 0x14, // Sound channel 1 period high & control
    GB_NR21 = 0x16, // Sound channel 2 length timer & duty cycle
    GB_NR22 = 0x17, // Sound channel 2 volume & envelope
    GB_NR23 = 0x18, // Sound channel 2 period low
    GB_NR24 = 0x19, // Sound channel 2 period high & control
    GB_NR30 = 0x1A, // Sound channel 3 DAC enable
    GB_NR31 = 0x1B, // Sound channel 3 length timer
    GB_NR32 = 0x1C, // Sound channel 3 output level
    GB_NR33 = 0x1D, // Sound channel 3 period low
    GB_NR34 = 0x1E, // Sound channel 3 period high & control
    GB_NR41 = 0x20, // Sound channel 4 length timer
    GB_NR42 = 0x21, // Sound channel 4 volume & envelope
    GB_NR43 = 0x22, // Sound channel 4 frequency & randomness
    GB_NR44 = 0x23, // Sound channel 4 control
    GB_NR50 = 0x24, // Master volume & VIN panning
    GB_NR51 = 0x25, // Sound panning
    GB_NR52 = 0x26, // Sound on/off
    // 0xFF30 - 0xFF3F: Wave RAM
    GB_LCDC = 0x40,  // LCD control
    GB_STAT = 0x41,  // LCD status
    GB_SCY = 0x42,   // Viewport Y position
    GB_SCX = 0x43,   // Viewport X position
    GB_LY = 0x44,    // LCD Y coordinate
    GB_LYC = 0x45,   // LY compare
    GB_DMA = 0x46,   // DMA source address & start
    GB_BGP = 0x47,   // BG palette data
    GB_OBP0 = 0x48,  // OBJ pallette 0 data
    GB_OBP1 = 0x49,  // OBJ pallette 1 data
    GB_WY = 0x4A,    // Window Y postion
    GB_WX = 0x4B,    // Window X position
    GB_KEY1 = 0x4D,  // Prepare speed switch
    GB_VBK = 0x4F,   // VRAM bank
    GB_HDMA1 = 0x51, // VRAM DMA source high
    GB_HDMA2 = 0x52, // VRAM DMA source low
    GB_HDMA3 = 0x53, // VRAM DMA destination high
    GB_HDMA4 = 0x54, // VRAM DMA destination low
    GB_HDMA5 = 0x55, // VRAM DMA length/mode/start
    GB_RP = 0x56,    // Infrared comm port
    GB_BCPS = 0x68,  // Background color palette specification / Background palette index
    GB_BCPD = 0x69,  // Background color palette data / Background palette data
    GB_OCPS = 0x6A,  // OBJ color palette specification / OBJ palette index
    GB_OCPD = 0x6B,  // OBJ color palette data / OBJ palette data
    GB_OPRI = 0x6C,  // Object priority mode
    GB_SVBK = 0x70,  // WRAM bank
    GB_PCM12 = 0x76, // Audio digital outputs 1 & 2
    GB_PCM34 = 0x77, // Audio digital outputs 3 & 4
    GB_IE = 0xFF     // Interrupt enable (R/W)
};

/* Main GB state */
struct gb_s
{
    union
    {
        uint8_t registers[GB_NUM_REG_8_BIT];
        // uint16_t registers[GB_NUM_REG_16_BIT];
        struct
        {
            uint16_t af;
            uint16_t bc;
            uint16_t de;
            uint16_t hl;
            uint16_t sp;
            uint16_t pc;
        };
        struct
        {
            flags_t f;
            uint8_t a;
            uint8_t c;
            uint8_t b;
            uint8_t e;
            uint8_t d;
            uint8_t l;
            uint8_t h;
        };
    };
    uint16_t m_cycles;
    bool ime;
    bool ime_enable;
    bool halted;
    bool stopped;
    struct memory_s memory;
};

void gb_init(struct gb_s *gb);
void gb_run(struct gb_s *gb);
int gb_load_rom(struct gb_s *gb, const char *path);
void gb_log_state(struct gb_s *gb, FILE *log_file, bool gbdoc);