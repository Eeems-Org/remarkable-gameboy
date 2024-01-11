#include "z80register.h"

z80register::z80register() {
	reset();
}

void z80register::setaf() {
	lomask = 0xF0;
}

void z80register::reset() {
	data.full = 0;
	lomask = 0xFF;
}

void z80register::setfull(quint16 val) {
	data.full = val;
	data.part.lo &= lomask;
}

void z80register::setlo(quint8 val) {
	data.part.lo = val;
	data.part.lo &= lomask;
}

void z80register::sethi(quint8 val) {
	data.part.hi = val;
}

quint16 z80register::getfull() {
	return data.full;
}

quint8 z80register::getlo() {
	return data.part.lo;
}

quint8 z80register::gethi() {
	return data.part.hi;
}

bool z80register::getflag(char type) {
	int bit = getflagmask(type);
	return data.part.lo & bit;
}

void z80register::setflag(char type, bool val) {
	int bit = getflagmask(type);
	if (val) data.part.lo |= bit;
	else data.part.lo &= ~bit;
}

void z80register::operator+=(qint16 val) {
	data.full += val;
}

void z80register::operator-=(qint16 val) {
	data.full -= val;
}

int z80register::getflagmask(char type) {
	switch (type) {
	case 'c': return (1 << 4);
	case 'h': return (1 << 5);
	case 'n': return (1 << 6);
	case 'z': return (1 << 7);
	default: return 0;
	}
}

