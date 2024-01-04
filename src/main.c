#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "gb.h"
#include "cpu.h"
#include "memory.h"

static volatile sig_atomic_t keep_running = 1;

static void sig_handler(int _)
{
    (void)_;
    keep_running = 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments %d", argc);
        return EXIT_FAILURE;
    }

    gameboy_t gb;
    char *rom_path = argv[1];

    gb_init(&gb);
    if (gb_load_rom(&gb, rom_path) != 0)
        return EXIT_FAILURE;

    FILE *log_file = fopen("gb.log", "w");

    signal(SIGINT, sig_handler);
    while (keep_running)
    {
        gb_log_state(&gb, log_file, true);
        cpu_run(&gb);

        if (mem_read_byte(&gb, 0xFF02) == 0x81)
        {
            printf("%c", mem_read_byte(&gb, 0xFF01));
            mem_write_byte(&gb, 0xFF02, 0x00);
        }
    }

    fclose(log_file);

    return EXIT_SUCCESS;
}