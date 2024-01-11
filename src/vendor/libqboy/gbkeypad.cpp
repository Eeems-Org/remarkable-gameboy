#include "gbkeypad.h"

gbkeypad::gbkeypad(z80mmu *mmu) {
	this->mmu = mmu;
	reset();
}

void gbkeypad::step() {
	quint8 mem = mmu->readbyte(_GBKEYPAD_MEMADDR);
	mem &= 0x30;
	if (mem & 0x10) mem |= row0;
	if (mem & 0x20) mem |= row1;
	mmu->writebyte(_GBKEYPAD_MEMADDR, mem);

	if (interrupt) {
		mmu->writebyte(0xFF0F, mmu->readbyte(0xFF0F) | 0x10);
	}
	interrupt = false;
}

void gbkeypad::keydown(GBKeypadKey key) {
	switch (key) {
	case GBKeypadKey_RIGHT: row1 &= 0xE; break;
	case GBKeypadKey_LEFT: row1 &= 0xD; break;
	case GBKeypadKey_UP: row1 &= 0xB; break;
	case GBKeypadKey_DOWN: row1 &= 0x7; break;
	case GBKeypadKey_A: row0 &= 0xE; break;
	case GBKeypadKey_B: row0 &= 0xD; break;
	case GBKeypadKey_SELECT: row0 &= 0xB; break;
	case GBKeypadKey_START: row0 &= 0x7; break;
	case GBKeypadKey_NONE:
	default:
		break;
	}
	interrupt = true;
}

void gbkeypad::keyup(GBKeypadKey key) {
	switch (key) {
	case GBKeypadKey_RIGHT: row1 |= 0x1; break;
	case GBKeypadKey_LEFT: row1 |= 0x2; break;
	case GBKeypadKey_UP: row1 |= 0x4; break;
	case GBKeypadKey_DOWN: row1 |= 0x8; break;
	case GBKeypadKey_A: row0 |= 0x1; break;
	case GBKeypadKey_B: row0 |= 0x2; break;
	case GBKeypadKey_SELECT: row0 |= 0x4; break;
	case GBKeypadKey_START: row0 |= 0x8; break;
	case GBKeypadKey_NONE:
	default:
		break;
	}
}

void gbkeypad::reset() {
	row0 = row1 = 0x0F;
	column = 0x30;
	interrupt = false;
}
