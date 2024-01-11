#include "z80timer.h"

z80timer::z80timer(z80mmu *mmu) {
	this->mmu = mmu;
	reset();
}

void z80timer::reset() {
	internal_counter = 0;
}

void z80timer::step(int z80m) {
	bool interrupt = false;
	quint8 divider, counter, modulo, control;

	divider = mmu->readbyte(0xFF04);
	counter = mmu->readbyte(0xFF05);
	modulo = mmu->readbyte(0xFF06);
	control = mmu->readbyte(0xFF07);

	int step = 0;
	switch (control & 0x3) {
	case 0:
		step = 256; break;
	case 1:
		step = 4; break;
	case 2:
		step = 16; break;
	case 3:
		step = 64; break;
	}

	internal_counter %= 256;

	for (int i = 0; i < z80m; ++i) {
		internal_counter++;
		if (internal_counter % _Z80TIMER_DIVIDER_STEP == 0) {
			divider++;
		}

		if ((control & 0x4) == 0) continue;

		if (internal_counter % step == 0) {
			counter++;
			if (counter == 0) {
				counter = modulo;
				interrupt = true;
			}
		}
	}

	mmu->writebyte(0xFF04, divider);
	mmu->writebyte(0xFF05, counter);
	mmu->writebyte(0xFF06, modulo);

	if (interrupt) {
		mmu->writebyte(0xFF0F, mmu->readbyte(0xFF0F) | 0x04);
	}
}
