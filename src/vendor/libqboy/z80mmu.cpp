#include "z80mmu.h"

#include <cassert>
#include <fstream>

z80mmu::z80mmu() {
	mbc = 0;
	reset();
}

z80mmu::~z80mmu() {
	if (mbc != 0) mbc->save(savefilename);
}

void z80mmu::reset() {
	wram.resize(0x2000, 0);
	vram.resize(0x2000, 0);
	voam.resize(0xA0, 0);
	zram.resize(0x100, 0);
	if (mbc != 0) delete mbc;
	mbc = 0;

	zram[0x05] = 0;
	zram[0x06] = 0;
	zram[0x07] = 0;
	zram[0x40] = 0x91;
	zram[0x42] = 0;
	zram[0x43] = 0;
	zram[0x45] = 0;
	zram[0x47] = 0xFC;
	zram[0x48] = 0xFF;
	zram[0x49] = 0xFF;
	zram[0x4A] = 0;
	zram[0x4B] = 0;
	zram[0x0F] = 0;
	zram[0xFF] = 0;
}

void z80mmu::load(std::string filename) {
	char byte;
	std::vector<quint8> rom;
	std::ifstream fin(filename.c_str(), std::ios_base::in | std::ios_base::binary);

	if (!fin.is_open()) return;

	while (fin.read(&byte, 1)) {
		rom.push_back(byte);
	}
	fin.close();

	savefilename = filename;
	savefilename.append(".gbsave");

	quint8 mbctype = rom[0x0147];
	switch (mbctype) {
	case 0x0:
		mbc = new z80mbc0(rom);
		break;
	case 0x1: case 0x2: case 0x3:
		mbc = new z80mbc1(rom);
		break;
	case 0xF: case 0x10: case 0x11: case 0x12: case 0x13:
		mbc = new z80mbc3(rom);
		break;
	case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1E:
		mbc = new z80mbc5(rom);
		break;
	default:
		assert(false && "MBC not supported");
		break;
	}

	mbc->load(savefilename);
}

quint8 z80mmu::readbyte(quint16 address) {
	switch(address & 0xF000) {
	// ROM bank 0
	case 0x0000:
	case 0x1000:
	case 0x2000:
	case 0x3000:
	// ROM bank 1
	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
		return mbc->readROM(address);

	// VRAM
	case 0x8000: case 0x9000:
		return vram[address & 0x1FFF];

	// External RAM
	case 0xA000: case 0xB000:
		return mbc->readRAM(address);

	// Work RAM and echo
	case 0xC000: case 0xD000: case 0xE000:
		return wram[address & 0x1FFF];

	// Everything else
	case 0xF000:
		switch(address & 0x0F00) {
		// Echo RAM
		case 0x000: case 0x100: case 0x200: case 0x300:
		case 0x400: case 0x500: case 0x600: case 0x700:
		case 0x800: case 0x900: case 0xA00: case 0xB00:
		case 0xC00: case 0xD00:
			return wram[address & 0x1FFF];

		// OAM
		case 0xE00:
			return address < 0xFEA0 ? voam[address & 0xFF] : 0;

		// Zeropage RAM, I/O
		case 0xF00:
			return zram[address & 0xFF];

		}
	}

	return 0;
}

quint16 z80mmu::readword(quint16 address) {
	quint16 ret;
	ret = readbyte(address + 1);
	ret <<= 8;
	ret |= readbyte(address);
	return ret;
}

void z80mmu::writebyte(quint16 address, quint8 value) {
	switch(address & 0xF000) {
	// bios and ROM bank 0
	case 0x0000:
	case 0x1000:
	case 0x2000:
	case 0x3000:
	// ROM bank 1
	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
		mbc->writeROM(address, value);
		break;

	// VRAM
	case 0x8000: case 0x9000:
		vram[address & 0x1FFF] = value;
		break;

	// External RAM
	case 0xA000: case 0xB000:
		mbc->writeRAM(address, value);
		break;

	// Work RAM and echo
	case 0xC000: case 0xD000: case 0xE000:
		wram[address & 0x1FFF] = value;
		break;

	// Everything else
	case 0xF000:
		switch(address & 0x0F00) {
		// Echo RAM
		case 0x000: case 0x100: case 0x200: case 0x300:
		case 0x400: case 0x500: case 0x600: case 0x700:
		case 0x800: case 0x900: case 0xA00: case 0xB00:
		case 0xC00: case 0xD00:
			wram[address & 0x1FFF] = value;
			break;

		// OAM
		case 0xE00:
			if(address < 0xFEA0) voam[address & 0xFF] = value;
			break;

		// Zeropage RAM, I/O
		case 0xF00:
			zram[address & 0xFF] = value;
			break;
		}

		break;
	}
}

void z80mmu::writeword(quint16 address, quint16 value) {
	writebyte(address, value & 0xFF);
	writebyte(address + 1, value >> 8);
}
