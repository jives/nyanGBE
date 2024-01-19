#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gb.h"
#include "memory.h"

void gb_init(struct gb_s *gb)
{
    gb->a = 0x01;
    gb->f = 0xB0;
    gb->b = 0x00;
    gb->c = 0x13;
    gb->d = 0x00;
    gb->e = 0xD8;
    gb->h = 0x01;
    gb->l = 0x4D;

    gb->pc = 0x0100;
    gb->sp = 0xFFFE;
}

void gb_run(struct gb_s *gb)
{
}

int gb_load_rom(struct gb_s *gb, const char *path)
{
    FILE *f = fopen(path, "rb");

    if (!f)
    {
        printf("Could not open ROM %s: %d.\n", path, errno);
        return errno;
    }

    fseek(f, 0, SEEK_END);
    long rom_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (rom_size > 0x8000)
    {
        printf("ROM too large: %ld.\n", rom_size);
        return -1;
    }

    memset(gb->memory.rom, 0xFF, 0x8000); /* Pad with 0xFFs */
    fread(gb->memory.rom, 1, rom_size, f);
    fclose(f);

    return 0;
}

void gb_log_state(struct gb_s *gb, FILE *log_file, bool gbdoc)
{
    if (gbdoc)
    {
        fprintf(log_file, "A:%02X ", gb->a);
        fprintf(log_file, "F:%02X ", gb->f);
        fprintf(log_file, "B:%02X ", gb->b);
        fprintf(log_file, "C:%02X ", gb->c);
        fprintf(log_file, "D:%02X ", gb->d);
        fprintf(log_file, "E:%02X ", gb->e);
        fprintf(log_file, "H:%02X ", gb->h);
        fprintf(log_file, "L:%02X ", gb->l);
        fprintf(log_file, "SP:%04X ", gb->sp);
        fprintf(log_file, "PC:%04X ", gb->pc);
        fprintf(log_file, "PCMEM:%02X,", mem_read_byte(gb, gb->pc));
        fprintf(log_file, "%02X,", mem_read_byte(gb, gb->pc + 1));
        fprintf(log_file, "%02X,", mem_read_byte(gb, gb->pc + 2));
        fprintf(log_file, "%02X\n", mem_read_byte(gb, gb->pc + 3));
    }
    else
    {
        fprintf(log_file, "A: %02X ", gb->a);
        fprintf(log_file, "F: %02X ", gb->f);
        fprintf(log_file, "B: %02X ", gb->b);
        fprintf(log_file, "C: %02X ", gb->c);
        fprintf(log_file, "D: %02X ", gb->d);
        fprintf(log_file, "E: %02X ", gb->e);
        fprintf(log_file, "H: %02X ", gb->h);
        fprintf(log_file, "L: %02X ", gb->l);
        fprintf(log_file, "SP: %04X ", gb->sp);
        fprintf(log_file, "PC: 00:%04X ", gb->pc);
        fprintf(log_file, "(%02X ", mem_read_byte(gb, gb->pc));
        fprintf(log_file, "%02X ", mem_read_byte(gb, gb->pc + 1));
        fprintf(log_file, "%02X ", mem_read_byte(gb, gb->pc + 2));
        fprintf(log_file, "%02X)\n", mem_read_byte(gb, gb->pc + 3));
    }
}