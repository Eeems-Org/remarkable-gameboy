#include "z80alu.h"

void z80alu::setregisters(z80register *afregister, z80register *hlregister) {
	af = afregister;
	hl = hlregister;
}

void z80alu::add(quint8 b, bool withcarry) {
	quint8 a = af->gethi();
	quint8 carry = (withcarry && af->getflag('c')) ? 1 : 0;
	quint8 res = a + b + carry;

	af->setflag('z', res == 0);
	af->setflag('h', (a & 0x0F) + (b & 0x0F) + carry > 0x0F);
	af->setflag('n', false);
	af->setflag('c', a > 0xFF - b - carry);

	af->sethi(res);
}

void z80alu::sub(quint8 b, bool withcarry) {
	quint8 a = af->gethi();
	quint8 carry = (withcarry && af->getflag('c')) ? 1 : 0;
	quint8 res = a - (b + carry);

	af->setflag('z', res == 0);
	af->setflag('h', (a & 0x0F) < (b & 0x0F) + carry);
	af->setflag('n', true);
	af->setflag('c', a < b + carry);

	af->sethi(res);
}

void z80alu::land(quint8 b) {
	quint8 a = af->gethi();
	quint8 res = a & b;

	af->setflag('z', res == 0);
	af->setflag('h', true);
	af->setflag('n', false);
	af->setflag('c', false);

	af->sethi(res);
}

void z80alu::lor(quint8 b) {
	quint8 a = af->gethi();
	quint8 res = a | b;

	af->setflag('z', res == 0);
	af->setflag('h', false);
	af->setflag('n', false);
	af->setflag('c', false);

	af->sethi(res);
}

void z80alu::lxor(quint8 b) {
	quint8 a = af->gethi();
	quint8 res = a ^ b;

	af->setflag('z', res == 0);
	af->setflag('h', false);
	af->setflag('n', false);
	af->setflag('c', false);

	af->sethi(res);
}

void z80alu::cp(quint8 b) {
	quint8 a = af->gethi();
	sub(b, false);
	af->sethi(a);
}

quint8 z80alu::inc(quint8 a) {
	quint8 res = a + 1;

	af->setflag('z', res == 0);
	af->setflag('h', (a & 0x0F) + 1 > 0x0F);
	af->setflag('n', false);

	return res;
}

quint8 z80alu::dec(quint8 a) {
	quint8 res = a - 1;

	af->setflag('z', res == 0);
	af->setflag('h', (a & 0x0F) == 0);
	af->setflag('n', true);

	return res;
}

void z80alu::cpl() {
	af->sethi(~af->gethi());
	af->setflag('h', true);
	af->setflag('n', true);
}

void z80alu::ccf() {
	af->setflag('c', !af->getflag('c'));
	af->setflag('h', false);
	af->setflag('n', false);
}

void z80alu::scf() {
	af->setflag('c', true);
	af->setflag('h', false);
	af->setflag('n', false);
}

void z80alu::add16(quint16 b) {
	quint16 a = hl->getfull();
	quint16 res = a + b;

	af->setflag('h', (a & 0x07FF) + (b & 0x07FF) > 0x07FF);
	af->setflag('n', false);
	af->setflag('c', a > 0xFFFF - b);

	hl->setfull(res);
}

quint16 z80alu::add16mixed(quint16 a, qint8 b) {
	quint16 other = (quint16) (((qint16)(b << 8)) >> 8);

	af->setflag('z', false);
	af->setflag('n', false);
	af->setflag('h', (a & 0x000F) + (other & 0x000F) > 0x000F);
	af->setflag('c', (a & 0x00FF) + (other & 0x00FF) > 0x00FF);

	return a + other;
}

quint8 z80alu::rr(quint8 val) {
	quint8 carry = val & 1;
	quint8 result = (val >> 1) | (af->getflag('c') ? 0x80 : 0);

	setregistersaftershift(result, carry);
	return result;
}

quint8 z80alu::rrc(quint8 val) {
	quint8 carry = val & 1;
	quint8 result = (val >> 1) | (carry << 7);

	setregistersaftershift(result, carry);
	return result;
}

quint8 z80alu::rl(quint8 val) {
	quint8 carry = val & 0x80;
	quint8 result = (val << 1) | (af->getflag('c') ? 1 : 0);

	setregistersaftershift(result, carry);
	return result;
}

quint8 z80alu::rlc(quint8 val) {
	quint8 carry = val & 0x80;
	quint8 result = (val << 1) | (carry >> 7);

	setregistersaftershift(result, carry);
	return result;
}

quint8 z80alu::sla(quint8 val) {
	quint8 carry = val & 0x80;
	quint8 result = val << 1;

	setregistersaftershift(result, carry);
	return result;
}

// This is a signed shift right
quint8 z80alu::sra(quint8 val) {
	quint8 carry = val & 1;

#if (-2 >> 1) == -1
	quint8 result = ((qint8)val) >> 1;
#else
	quint8 result = (val >> 1) | (val & 0x80);
#endif

	setregistersaftershift(result, carry);
	return result;
}

quint8 z80alu::srl(quint8 val) {
	quint8 carry = val & 1;
	quint8 result = val >> 1;

	setregistersaftershift(result, carry);
	return result;
}

void z80alu::daa() {
	quint8 a = af->gethi();
	quint16 adjust = af->getflag('c') ? 0x60 : 0x00;
	if(af->getflag('h'))
		adjust |= 0x06;
	if(!af->getflag('n')) {
		if((a & 0x0F) > 0x09)
			adjust |= 0x06;
		if(a > 0x99)
			adjust |= 0x60;
		a += adjust;
	} else {
		a -= adjust;
	}

	af->setflag('c', adjust >= 0x60);
	af->setflag('h', false);
	af->setflag('z', a == 0);

	af->sethi(a);
}

void z80alu::setregistersaftershift(quint8 result, quint8 carry) {
	af->setflag('h', false);
	af->setflag('n', false);

	af->setflag('z', result == 0);
	af->setflag('c', carry != 0);
}
