#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "gb.h"
#include "cpu.h"

static uint16_t timer_get_divider(struct gb_s *gb)
{
    uint8_t tac_clock = mem_read_byte(gb, GB_TAC) & 0b11;

    switch (tac_clock)
    {
    case 0b00:
        return 1024;

    case 0b01:
        return 16;

    case 0b10:
        return 64;

    case 0b11:
        return 256;

    default:
        printf("Unkown timer clock %d\n", tac_clock);
        assert(!"Unkown timer clock");
        return 0;
    }
}

void timer_run(struct gb_s *gb, uint16_t m_cycles)
{
    static uint32_t div_cycles = 0;
    static uint32_t timer_cycles = 0;
    uint16_t t_cycles = m_cycles * 4;

    div_cycles += t_cycles;
    if (div_cycles >= GB_DIV_CYCLES)
    {
        // Since we're not cycle accurate, we may have already executed
        // some (machine) cycles that belong to the next DIV cycle, so we don't set
        // the cycles to 0, but to the difference instead
        div_cycles -= GB_DIV_CYCLES;

        // Use direct memory write instead of mem_write_byte() because
        // writing any value to DIV must reset it - which is not what we want here
        gb->memory.ram[GB_DIV - 0x8000]++;
    }

    if (mem_read_byte(gb, GB_TAC) & (1 << 2)) // TAC bit 2 enables the timer
    {
        uint16_t timer_target = timer_get_divider(gb);
        timer_cycles += t_cycles;

        if (timer_cycles >= timer_target)
        {
            // See comment for DIV above as to why we're not resetting to 0
            timer_cycles -= timer_target;

            uint8_t timer_counter = mem_read_byte(gb, GB_TIMA);
            mem_write_byte(gb, GB_TIMA, timer_counter + 1);

            if (timer_counter == 0xFF)
            {
                uint8_t timer_modulo = mem_read_byte(gb, GB_TMA);
                mem_write_byte(gb, GB_TIMA, timer_modulo);
                cpu_raise_interrupt(gb, IR_TIMER);

                // TODO: implement obscure timer behavior
            }
        }
    }
}