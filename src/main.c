#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"
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

    struct gb_s gb;
    char *rom_path = argv[1];

    gb_init(&gb);
    if (gb_load_rom(&gb, rom_path) != 0)
        return EXIT_FAILURE;

    FILE *log_file = fopen("nyanGB.instr.log", "w");

    signal(SIGINT, sig_handler);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *surface;
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    if (SDL_CreateWindowAndRenderer(320, 240, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
        return 3;
    }

    while (keep_running)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            keep_running = false;
        }

        gb_log_state(&gb, log_file, false);
        cpu_run(&gb);

        if (mem_read_byte(&gb, 0xFF02) == 0x81)
        {
            printf("%c", mem_read_byte(&gb, 0xFF01));
            mem_write_byte(&gb, 0xFF02, 0x00);
        }

        // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        // SDL_RenderClear(renderer);
        // SDL_RenderPresent(renderer);
    }

    fclose(log_file);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return EXIT_SUCCESS;
}