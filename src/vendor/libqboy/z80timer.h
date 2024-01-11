#ifndef Z80TIMER_H
#define Z80TIMER_H

#include "libqboy_global.h"
#include "z80mmu.h"

const int _Z80TIMER_DIVIDER_STEP = 64;

class z80timer {
public:
	z80timer(z80mmu *mmu);
	void reset();
	void step(int z80m);
private:
	z80mmu *mmu;
	int internal_counter;
};

#endif // Z80TIMER_H
