#ifndef MMU_H
#define MMU_H

#include "libqboy_global.h"
#include "z80mbc.h"

#include <string>
#include <vector>

class z80mmu {
public:
	z80mmu();
	~z80mmu();
	void reset();
	void load(std::string filename);
	quint8 readbyte(quint16 address);
	quint16 readword(quint16 address);
	void writebyte(quint16 address, quint8 value);
	void writeword(quint16 address, quint16 value);

private:
	z80mbc *mbc;
	std::vector<quint8> wram, vram, voam, zram;
	std::string savefilename;
};

#endif // MMU_H
