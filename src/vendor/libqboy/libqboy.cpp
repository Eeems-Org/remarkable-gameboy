#include "libqboy.h"

libqboy::libqboy() : cpu(&mmu), timer(&mmu), gpu(&mmu), keypad(&mmu) {
	reset();
}

void libqboy::reset() {
	mmu.reset();
	gpu.reset();
	timer.reset();
	keypad.reset();
	cpu.reset();
}

void libqboy::loadgame(std::string filename) {
	mmu.load(filename);
}

quint8 *libqboy::getLCD() {
	return gpu.getLCD();
}

void libqboy::cycle() {
	cpu.cycle();
	gpu.step(cpu.get_m());
	keypad.step();
	timer.step(cpu.get_m());
}

int libqboy::get_elapsed_time() {
	return cpu.get_m();
}

void libqboy::keyup(GBKeypadKey key) {
	keypad.keyup(key);
}


void libqboy::keydown(GBKeypadKey key) {
	keypad.keydown(key);
}

bool libqboy::refresh_screen() {
	return gpu.is_updated();
}
