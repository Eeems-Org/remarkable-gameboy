#ifndef Z80ALU_H
#define Z80ALU_H

#include "libqboy_global.h"
#include "z80register.h"

class z80alu {
public:
	void setregisters(z80register *afregister, z80register *hlregister);
	void add(quint8 val, bool withcarry);
	void sub(quint8 val, bool withcarry);
	void land(quint8 val);
	void lor(quint8 val);
	void lxor(quint8 val);
	void cp(quint8 val);
	quint8 inc(quint8 val);
	quint8 dec(quint8 val);
	void cpl();
	void ccf();
	void scf();
	void add16(quint16 val);
	quint16 add16mixed(quint16 a, qint8 other);
	quint8 rr(quint8 val);
	quint8 rrc(quint8 val);
	quint8 rl(quint8 val);
	quint8 rlc(quint8 val);
	quint8 sla(quint8 val);
	quint8 sra(quint8 val);
	quint8 srl(quint8 val);
	void daa();
private:
	z80register *af, *hl;
	void setregistersaftershift(quint8 result, quint8 carry);
};

#endif // Z80ALU_H
