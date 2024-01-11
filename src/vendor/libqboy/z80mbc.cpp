#include "z80mbc.h"

#include <cstring>
#include <fstream>

z80mbc0::z80mbc0(const std::vector<quint8> &rom) {
	this->rom = rom;
	ram.resize(0x2000);
}

quint8 z80mbc0::readROM(quint16 address) {
	return rom[address];
}

quint8 z80mbc0::readRAM(quint16 address) {
	return ram[address];
}

void z80mbc0::writeROM(quint16, quint8) {}

void z80mbc0::writeRAM(quint16 address, quint8 value) {
	ram[address] = value;
}

/********************************/

z80mbc1::z80mbc1(const std::vector<quint8> &rom) {
	this->rom = rom;
	ram.resize(0x10000);
	rombank = 1;
	rambank = 0;
	extram_on = false;
	ram_mode = false;
}

quint8 z80mbc1::readROM(quint16 address) {
	if (address < 0x4000) return rom[address];
	address &= 0x3FFF;
	int the_rombank = (ram_mode) ? rombank & 0x1F : rombank;
	return rom[the_rombank * 0x4000 | address];
}

quint8 z80mbc1::readRAM(quint16 address) {
	if (!extram_on || !ram_mode) return 0;
	address &= 0x1FFF;
	return ram[rambank * 0x2000 | address];
}

void z80mbc1::writeROM(quint16 address, quint8 value) {
	switch (address & 0xF000) {
	case 0x0000:
	case 0x1000:
		extram_on = (value == 0x0A);
		break;
	case 0x2000:
	case 0x3000:
		value &= 0x1F;
		if (value == 0) value = 1;
		rombank = (rombank & 0x60) | value;
		break;
	case 0x4000:
	case 0x5000:
		value &= 0x03;
		if (ram_mode) {
			rambank = value;
		} else {
			rombank = (value << 5) | (rombank & 0x1F);
		}
		break;
	case 0x6000:
	case 0x7000:
		ram_mode = value & 1;
		break;
	}
}

void z80mbc1::writeRAM(quint16 address, quint8 value) {
	if (!extram_on || !ram_mode) return;
	if (!ram_mode) rambank = 0;
	address &= 0x1FFF;
	ram[rambank * 0x2000 | address] = value;
}

/********************************/

z80mbc3::z80mbc3(const std::vector<quint8> &rom) {
	this->rom = rom;

	int ramsize = 0;
	switch (rom[0x0149]) {
	case 0x00:
		ramsize = 0; break;
	case 0x01:
		ramsize = 0x800; break; // 2 kB
	case 0x02:
		ramsize = 0x2000; break; // 8 kB
	case 0x03:
		ramsize = 0x8000; break; // 32 kB
	}

	ram.resize(ramsize);
	rtc.resize(5);
	rombank = 1;
	rambank = 0;
	extram_on = false;
	rtc_lock = false;
}

quint8 z80mbc3::readROM(quint16 address) {
	if (address < 0x4000) return rom[address];
	address &= 0x3FFF;
	return rom[rombank * 0x4000 + (int)address];
}

quint8 z80mbc3::readRAM(quint16 address) {
	if (extram_on == false) return 0;
	if (rambank <= 3) {
		address &= 0x1FFF;
		return ram[rambank * 0x2000 + (int)address];
	} else {
		return rtc[rambank - 0x08];
	}
}

void z80mbc3::writeROM(quint16 address, quint8 value) {
	switch (address & 0xF000) {
	case 0x0000:
	case 0x1000:
		extram_on = (value == 0x0A);
		break;
	case 0x2000:
	case 0x3000:
		value &= 0x7F;
		rombank = (value == 0x00) ? 0x01 : value;
		break;
	case 0x4000:
	case 0x5000:
		rambank = value;
		break;
	case 0x6000:
	case 0x7000:
		switch (value) {
		case 0x00:
			rtc_lock = false;
			break;
		case 0x01:
			if (rtc_lock == false) calc_rtcregs();
			rtc_lock = true;
			break;
		}
		break;
	}
}

void z80mbc3::writeRAM(quint16 address, quint8 value) {
	if (extram_on == false) return;

	if (rambank <= 3) {
		address &= 0x1FFF;
		ram[rambank * 0x2000 + (int)address] = value;
	} else {
		rtc[rambank - 0x8] = value;
		calc_rtczero();
	}
}

void z80mbc3::save(std::string filename) {
	std::ofstream fout(filename.c_str(), std::ios_base::out | std::ios_base::binary);

	char head[] = "GB_MBC";
	fout.write(head, sizeof head);
	fout.write(reinterpret_cast<char *>(&rom[0x0147]), 1);

	for (int i = 0; i < 5; ++i) {
		fout.write(reinterpret_cast<char *>(&rtc[i]), 1);
	}
	quint64 rtc = rtczero;
	fout.write(reinterpret_cast<char *>(&rtc), 8);

	for (unsigned i = 0; i < ram.size(); ++i) {
		fout.write(reinterpret_cast<char *>(&ram[i]), 1);
	}
	fout.close();
}

void z80mbc3::load(std::string filename) {
	std::ifstream fin(filename.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!fin.is_open()) return;

	char head[7];
	char ver;

	fin.read(head, 7);
	fin.read(&ver, 1);
	if (strcmp(head, "GB_MBC") != 0) {
		fin.close();
		return;
	}


	char byte;
	rtc.clear();
	for (int i = 0; i < 5; ++i) {
		fin.read(&byte, 1);
		rtc.push_back(byte);
	}
	quint64 rtc;
	fin.read(reinterpret_cast<char *>(&rtc), 8);
	rtczero = rtc;

	ram.clear();
	while (fin.read(&byte, 1)) {
		ram.push_back(byte);
	}
	fin.close();


	calc_rtcregs();
}

void z80mbc3::calc_rtczero() {
	time_t difftime = time(NULL);
	long long days;
	difftime -= rtc[0];
	difftime -= rtc[1] * 60;
	difftime -= rtc[2] * 3600;
	days = rtc[4] & 0x1;
	days = days << 8 | rtc[3];
	difftime -= days * 3600 * 24;
	rtczero = difftime;
}

void z80mbc3::calc_rtcregs() {
	if (rtc[4] & 0x40) {
		return;
	}

	time_t difftime = time(NULL) - rtczero;
	rtc[0] = difftime % 60;
	rtc[1] = (difftime / 60) % 60;
	rtc[2] = (difftime / 3600) % 24;
	long long days = (difftime / (3600*24));
	rtc[3] = days & 0xFF;
	rtc[4] = (rtc[4] & 0xFE) | ((days >> 8) & 0x1);

	if (days >= 512) {
		rtc[4] |= 0x80;
		calc_rtczero();
	}
}

/********************************/

z80mbc5::z80mbc5(const std::vector<quint8> &rom) {
	this->rom = rom;
	ram.resize(0x20000);
	rombank = 1;
	rambank = 0;
	extram_on = false;
}

quint8 z80mbc5::readROM(quint16 address) {
	if (address < 0x4000) return rom[address];
	address &= 0x3FFF;

	return rom[rombank * 0x4000 | address];
}

quint8 z80mbc5::readRAM(quint16 address) {
	if (!extram_on) return 0;

	address &= 0x1FFF;
	return ram[rambank * 0x2000 | address];
}

void z80mbc5::writeROM(quint16 address, quint8 value) {
	switch (address & 0xF000) {
	case 0x0000:
	case 0x1000:
		extram_on = (value == 0x0A);
		break;
	case 0x2000:
		rombank = (rombank & 0x100) | (value);
		break;
	case 0x3000:
		value &= 0x1;
		rombank = (rombank & 0xFF) | (((int)value) << 8);
		break;
	case 0x4000:
	case 0x5000:
		rambank = value & 0x0F;
		break;
	}
}

void z80mbc5::writeRAM(quint16 address, quint8 value) {
	if (!extram_on) return;

	address &= 0x1FFF;
	ram[rambank * 0x2000 | address] = value;
}

void z80mbc5::save(std::string filename) {
	std::ofstream fout(filename.c_str(), std::ios_base::out | std::ios_base::binary);
	for (unsigned i = 0; i < ram.size(); ++i) {
		fout.write((char*)&ram[i], 1);
	}
	fout.close();
}

void z80mbc5::load(std::string filename) {
	std::ifstream fin(filename.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!fin.is_open()) return;

	char byte;
	ram.clear();
	while (fin.read(&byte, 1)) {
		ram.push_back(byte);
	}
	fin.close();
}
