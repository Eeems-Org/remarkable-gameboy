#ifndef GBKEYPAD_H
#define GBKEYPAD_H

#include "libqboy_global.h"
#include "z80mmu.h"

enum GBKeypadKey {
	GBKeypadKey_RIGHT,
	GBKeypadKey_LEFT,
	GBKeypadKey_UP,
	GBKeypadKey_DOWN,
	GBKeypadKey_A,
	GBKeypadKey_B,
	GBKeypadKey_SELECT,
	GBKeypadKey_START,
	GBKeypadKey_NONE
};

const int _GBKEYPAD_MEMADDR = 0xFF00;

class gbkeypad {
public:
	gbkeypad(z80mmu *mmu);
	void reset();
	void keyup(GBKeypadKey key);
	void keydown(GBKeypadKey key);
	void step();
private:
	z80mmu *mmu;
	quint8 row0, row1;
	quint8 column;
	bool interrupt;
};

#endif // GBKEYPAD_H
