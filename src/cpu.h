#pragma once

#include <stdint.h>
#include "gb.h"

void cpu_raise_interrupt(struct gb_s *gb, interrupts_t ir);
void cpu_run(struct gb_s *gb);