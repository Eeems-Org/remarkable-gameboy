#include "z80.h"

#include <cassert>

z80::z80(z80mmu *mmu) {
	this->mmu = mmu;
	reset();
}

void z80::cycle() {
	last_m = 0;

	if (handle_interrupt()) return;

	if (halted) {
		op_nop();
	} else {
		call(getbytearg());
	}

	if (last_m == 0) {
		assert(false && "No clock cycles used this cycle.");
	}
}

void z80::reset() {
	last_m = 0;
	halted = false;
	interrupt_enable = true;

	af.reset();
	af.setaf();
	bc.reset();
	de.reset();
	hl.reset();
	pc.reset();
	sp.reset();

	alu.setregisters(&af, &hl);

	af.setfull(0x01B0);
	bc.setfull(0x0013);
	de.setfull(0x00D8);
	hl.setfull(0x014D);
	sp.setfull(0xFFFE);
	pc.setfull(0x0100);
}

int z80::get_m() {
	return last_m;
}

bool z80::is_halted() {
	return halted;
}

bool z80::handle_interrupt() {
	if (interrupt_enable == false && halted == false) return false;

	quint8 interrupt_occured = mmu->readbyte(0xFF0F);
	quint8 interrupt_enabled = mmu->readbyte(0xFFFF);

	for (unsigned int i = 0; i < 5; ++i) {
		if ((interrupt_occured & interrupt_enabled & (1u << i)) != 0) {
			halted = false;
			if (interrupt_enable) {
				mmu->writebyte(0xFF0F, interrupt_occured & ~(1u << i));
				op_rst_int(0x0040 | (i << 3));
				return true;
			}
			return false;
		}
	}

	return false;
}

quint8 z80::getbyteregisterval(int code) {
	assert(code != 6);
	switch (code) {
	case 7: return af.gethi();
	case 0: return bc.gethi();
	case 1: return bc.getlo();
	case 2: return de.gethi();
	case 3: return de.getlo();
	case 4: return hl.gethi();
	case 5: return hl.getlo();
	}
	return 0;
}

void z80::setbyteregisterval(int code, quint8 val) {
	assert(code != 6);
	switch (code) {
	case 7: af.sethi(val); break;
	case 0: bc.sethi(val); break;
	case 1: bc.setlo(val); break;
	case 2: de.sethi(val); break;
	case 3: de.setlo(val); break;
	case 4: hl.sethi(val); break;
	case 5: hl.setlo(val); break;
	}
}

quint16 z80::getwordregisterval(int code, bool lastsp) {
	switch (code) {
	case 0: return bc.getfull();
	case 1: return de.getfull();
	case 2: return hl.getfull();
	case 3: return (lastsp) ? sp.getfull() : af.getfull();
	}
	return 0;
}

void z80::setwordregisterval(int code, bool lastsp, quint16 val) {
	switch (code) {
	case 0: bc.setfull(val); break;
	case 1: de.setfull(val); break;
	case 2: hl.setfull(val); break;
	case 3: if (lastsp) sp.setfull(val); else af.setfull(val); break;
	}
}

quint8 z80::getbytearg() {
	quint8 retval = mmu->readbyte(pc.getfull());
	pc += 1;
	return retval;
}

quint16 z80::getwordarg() {
	quint16 retval = mmu->readword(pc.getfull());
	pc += 2;
	return retval;
}

void z80::pushstack(quint16 val) {
	sp -= 2;
	mmu->writeword(sp.getfull(), val);
}

quint16 z80::popstack() {
	quint16 ret = mmu->readword(sp.getfull());
	sp += 2;
	return ret;
}

void z80::addticks(int m) {
	assert(last_m == 0);
	last_m += m;
}

bool z80::jumpcond(int arg) {
	switch (arg) {
	case 0:
		return !af.getflag('z');
	case 1:
		return af.getflag('z');
	case 2:
		return !af.getflag('c');
	case 3:
		return af.getflag('c');
	}

	assert(false && "Errornous jump condition");

	return false;
}

void z80::call(quint8 opcode) {
	int hi2 = opcode >> 6;
	int mid3 = (opcode >> 3) & 7;
	int low3 = opcode & 7;
	int mid2 = (opcode >> 4) & 3;

	switch (hi2) {
	case 0:
		switch (low3) {
		case 0:
			if (mid3 == 0) op_nop();
			else if (mid3 == 1) op_ld_mm_sp();
			else if (mid3 == 2) op_nop(); // Actually STOP opcode
			else if (mid3 >= 3) op_jump_rel(mid3);
			break;
		case 1:
			if ((mid3 & 1) == 0) op_ld_dd_nn(mid2);
			else op_add16_rr(mid2);
			break;
		case 2:
			if ((mid3 & 4) == 0) op_ld_a_ss(mid3);
			else op_ld_a_hl(mid3);
			break;
		case 3:
			if ((mid3 & 1) == 0) op_inc16_rr(mid2);
			else op_dec16_rr(mid2);
			break;
		case 4:
			op_inc_r(mid3); break;
		case 5:
			op_dec_r(mid3); break;
		case 6:
			op_ld_r_n(mid3); break;
		case 7:
			switch (mid3) {
			case 0:
				op_rdca(Direction_LEFT); break;
			case 1:
				op_rdca(Direction_RIGHT); break;
			case 2:
				op_rda(Direction_LEFT); break;
			case 3:
				op_rda(Direction_RIGHT); break;
			case 4:
				op_daa(); break;
			case 5:
				op_cpl(); break;
			case 6:
				op_scf(); break;
			case 7:
				op_ccf(); break;
			}
			break;
		}
		break;
	case 1:
		if (mid3 == 6 && low3 == 6) op_halt();
		else op_ld_r_r(mid3, low3);
		break;
	case 2:
		switch (mid3) {
		case 0:
			op_add_r(low3, false); break;
		case 1:
			op_add_r(low3, true); break;
		case 2:
			op_sub_r(low3, false); break;
		case 3:
			op_sub_r(low3, true); break;
		case 4:
			op_and_r(low3); break;
		case 5:
			op_xor_r(low3); break;
		case 6:
			op_or_r(low3); break;
		case 7:
			op_cp_r(low3); break;
		}
		break;
	case 3:
		switch (low3) {
		case 0:
			if ((mid3 & 4) == 0) op_ret_cond(mid3);
			else if ((mid3 & 1) == 0) op_ld_a_n(mid2);
			else if (mid3 == 5) op_ld_sp_sn();
			else op_ld_hl_sp_sn();
			break;
		case 1:
			switch (mid3) {
			case 0: case 2: case 4: case 6:
				op_pop_qq(mid2); break;
			case 1:
				op_ret(); break;
			case 3:
				op_reti(); break;
			case 5:
				op_jump_hl(); break;
			case 7:
				op_ld_sp_hl(); break;
			}
			break;
		case 2:
			if ((mid3 & 4) == 0) op_jump_cond(mid3);
			else if ((mid3 & 1) == 0) op_ld_a_c(mid2);
			else op_ld_a_nn(mid2);
			break;
		case 3:
			switch (mid3) {
			case 0:
				op_jump(); break;
			case 1:
				call_extended(); break;
			case 6:
				op_di(); break;
			case 7:
				op_ei(); break;
			}
			break;
		case 4:
			op_call_cond(mid3);
			break;
		case 5:
			if ((mid3 & 1) == 0) op_push_qq(mid2);
			else if (mid3 == 1) op_call();
			break;
		case 6:
			switch (mid3) {
			case 0:
				op_add_n(false); break;
			case 1:
				op_add_n(true); break;
			case 2:
				op_sub_n(false); break;
			case 3:
				op_sub_n(true); break;
			case 4:
				op_and_n(); break;
			case 5:
				op_xor_n(); break;
			case 6:
				op_or_n(); break;
			case 7:
				op_cp_n(); break;
			}
			break;
		case 7:
			op_rst_p(mid3);
			break;
		}
		break;
	}
}

void z80::call_extended() {
	int opcode = getbytearg();

	int arg = opcode & 7;
	int hi2 = opcode >> 6;
	int mid3 = (opcode >> 3) & 7;

	switch (hi2) {
	case 0:
		switch (mid3) {
		case 0:
			op_rdc_r(arg, Direction_LEFT); break;
		case 1:
			op_rdc_r(arg, Direction_RIGHT); break;
		case 2:
			op_rd_r(arg, Direction_LEFT); break;
		case 3:
			op_rd_r(arg, Direction_RIGHT); break;
		case 4:
			op_sda(arg, Direction_LEFT); break;
		case 5:
			op_sda(arg, Direction_RIGHT); break;
		case 6:
			op_swap(arg); break;
		case 7:
			op_srl(arg); break;
		}
		break;
	case 1:
		op_bit(mid3, arg); break;
	case 2:
		op_res(mid3, arg); break;
	case 3:
		op_set(mid3, arg); break;
	}
}

void z80::op_nop() {
	addticks(1);
}

void z80::op_ld_r_r(int arg1, int arg2) {
	if (arg1 == 6) {
		// (HL) <- r
		mmu->writebyte(hl.getfull(), getbyteregisterval(arg2));
		addticks(2);
	} else if (arg2 == 6) {
		// r <- (HL)
		setbyteregisterval(arg1, mmu->readbyte(hl.getfull()));
		addticks(2);
	} else {
		// r <- r'
		setbyteregisterval(arg1, getbyteregisterval(arg2));
		addticks(1);
	}
}

void z80::op_ld_r_n(int arg) {
	quint8 n = getbytearg();
	if (arg == 6) {
		// (HL) <- n;
		mmu->writebyte(hl.getfull(), n);
		addticks(3);
	} else {
		// r <- n
		setbyteregisterval(arg, n);
		addticks(2);
	}
}

void z80::op_ld_a_ss(int arg) {
	quint16 addr;
	if ((arg & 2) == 0) {
		addr = bc.getfull();
	} else {
		addr = de.getfull();
	}

	if ((arg & 1) == 0) {
		mmu->writebyte(addr, af.gethi());
	} else {
		af.sethi(mmu->readbyte(addr));
	}

	addticks(2);
}

void z80::op_ld_a_hl(int arg) {
	if ((arg & 1) == 0) {
		mmu->writebyte(hl.getfull(), af.gethi());
	} else {
		af.sethi(mmu->readbyte(hl.getfull()));
	}

	if ((arg & 2) == 0) {
		hl += 1;
	} else {
		hl -= 1;
	}

	addticks(2);
}

void z80::op_ld_dd_nn(int arg) {
	setwordregisterval(arg, true, getwordarg());
	addticks(3);
}

void z80::op_ld_sp_hl() {
	sp.setfull(hl.getfull());
	addticks(2);
}

void z80::op_push_qq(int arg) {
	pushstack(getwordregisterval(arg, false));
	addticks(4);
}

void z80::op_pop_qq(int arg) {
	setwordregisterval(arg, false, popstack());
	addticks(3);
}

void z80::op_add_r(int arg, bool withcarry) {
	if (arg == 6) {
		alu.add(mmu->readbyte(hl.getfull()), withcarry);
		addticks(2);
	} else {
		alu.add(getbyteregisterval(arg), withcarry);
		addticks(1);
	}
}

void z80::op_add_n(bool withcarry) {
	alu.add(getbytearg(), withcarry);
	addticks(2);
}

void z80::op_sub_r(int arg, bool withcarry) {
	if (arg == 6) {
		alu.sub(mmu->readbyte(hl.getfull()), withcarry);
		addticks(2);
	} else {
		alu.sub(getbyteregisterval(arg), withcarry);
		addticks(1);
	}
}

void z80::op_sub_n(bool withcarry) {
	alu.sub(getbytearg(), withcarry);
	addticks(2);
}

void z80::op_and_r(int arg) {
	if (arg == 6) {
		alu.land(mmu->readbyte(hl.getfull()));
		addticks(2);
	} else {
		alu.land(getbyteregisterval(arg));
		addticks(1);
	}
}

void z80::op_and_n() {
	alu.land(getbytearg());
	addticks(2);
}

void z80::op_or_r(int arg) {
	if (arg == 6) {
		alu.lor(mmu->readbyte(hl.getfull()));
		addticks(2);
	} else {
		alu.lor(getbyteregisterval(arg));
		addticks(1);
	}
}

void z80::op_or_n() {
	alu.lor(getbytearg());
	addticks(2);
}

void z80::op_xor_r(int arg) {
	if (arg == 6) {
		alu.lxor(mmu->readbyte(hl.getfull()));
		addticks(2);
	} else {
		alu.lxor(getbyteregisterval(arg));
		addticks(1);
	}
}

void z80::op_xor_n() {
	alu.lxor(getbytearg());
	addticks(2);
}

void z80::op_cp_r(int arg) {
	if (arg == 6) {
		alu.cp(mmu->readbyte(hl.getfull()));
		addticks(2);
	} else {
		alu.cp(getbyteregisterval(arg));
		addticks(1);
	}
}

void z80::op_cp_n() {
	alu.cp(getbytearg());
	addticks(2);
}

void z80::op_inc_r(int arg) {
	if (arg == 6) {
		quint16 addr = hl.getfull();
		mmu->writebyte(addr, alu.inc(mmu->readbyte(addr)));
		addticks(3);
	} else {
		setbyteregisterval(arg, alu.inc(getbyteregisterval(arg)));
		addticks(1);
	}
}

void z80::op_dec_r(int arg) {
	if (arg == 6) {
		quint16 addr = hl.getfull();
		mmu->writebyte(addr, alu.dec(mmu->readbyte(addr)));
		addticks(3);
	} else {
		setbyteregisterval(arg, alu.dec(getbyteregisterval(arg)));
		addticks(1);
	}
}

void z80::op_cpl() {
	alu.cpl();
	addticks(1);
}

void z80::op_ccf() {
	alu.ccf();
	addticks(1);
}

void z80::op_scf() {
	alu.scf();
	addticks(1);
}

void z80::op_halt() {
	halted = true;
	addticks(1);
}

void z80::op_di() {
	interrupt_enable = false;
	addticks(1);
}

void z80::op_ei() {
	interrupt_enable = true;
	addticks(1);
}

void z80::op_add16_rr(int arg) {
	alu.add16(getwordregisterval(arg, true));
	addticks(2);
}

void z80::op_inc16_rr(int arg) {
	setwordregisterval(arg, true, getwordregisterval(arg, true) + 1);
	addticks(2);
}

void z80::op_dec16_rr(int arg) {
	setwordregisterval(arg, true, getwordregisterval(arg, true) - 1);
	addticks(2);
}

void z80::op_rdca(Direction dir) {
	quint8 ans = af.gethi();
	if (dir == Direction_LEFT) {
		ans = alu.rlc(ans);
	} else {
		ans = alu.rrc(ans);
	}
	af.sethi(ans);
	af.setflag('z', false);

	addticks(1);
}

void z80::op_rda(Direction dir) {
	quint8 ans = af.gethi();
	if (dir == Direction_LEFT) {
		ans = alu.rl(ans);
	} else {
		ans = alu.rr(ans);
	}
	af.sethi(ans);
	af.setflag('z', false);

	addticks(1);
}

void z80::op_rdc_r(int arg, Direction dir) {
	quint8 ans;
	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		if (dir == Direction_LEFT) {
			ans = alu.rlc(ans);
		} else {
			ans = alu.rrc(ans);
		}
		mmu->writebyte(addr, ans);

		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		if (dir == Direction_LEFT) {
			ans = alu.rlc(ans);
		} else {
			ans = alu.rrc(ans);
		}
		setbyteregisterval(arg, ans);

		addticks(2);
	}
}

void z80::op_rd_r(int arg, Direction dir) {
	quint8 ans;
	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		if (dir == Direction_LEFT) {
			ans = alu.rl(ans);
		} else {
			ans = alu.rr(ans);
		}
		mmu->writebyte(addr, ans);

		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		if (dir == Direction_LEFT) {
			ans = alu.rl(ans);
		} else {
			ans = alu.rr(ans);
		}
		setbyteregisterval(arg, ans);

		addticks(2);
	}
}

void z80::op_sda(int arg, Direction dir) {
	quint8 ans;
	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		if (dir == Direction_LEFT) {
			ans = alu.sla(ans);
		} else {
			ans = alu.sra(ans);
		}
		mmu->writebyte(addr, ans);

		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		if (dir == Direction_LEFT) {
			ans = alu.sla(ans);
		} else {
			ans = alu.sra(ans);
		}
		setbyteregisterval(arg, ans);

		addticks(2);
	}
}

void z80::op_srl(int arg) {
	quint8 ans;
	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		ans = alu.srl(ans);
		mmu->writebyte(addr, ans);

		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		ans = alu.srl(ans);
		setbyteregisterval(arg, ans);

		addticks(2);
	}
}

void z80::op_bit(quint8 bit, int arg) {
	quint8 ans;
	quint8 test = 1u << bit;

	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		addticks(3);
	} else {
		ans = getbyteregisterval(arg);
		addticks(2);
	}

	af.setflag('z', (ans & test) == 0);
	af.setflag('n', false);
	af.setflag('h', true);
}

void z80::op_set(quint8 bit, int arg) {
	quint8 ans;
	quint8 test = 1 << bit;

	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		mmu->writebyte(addr, ans | test);
		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		setbyteregisterval(arg, ans | test);
		addticks(2);
	}
}

void z80::op_res(quint8 bit, int arg) {
	quint8 ans;
	quint8 test = 1 << bit;

	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		mmu->writebyte(addr, ans & (~test));
		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		setbyteregisterval(arg, ans & (~test));
		addticks(2);
	}
}

void z80::op_jump() {
	pc.setfull(getwordarg());
	addticks(4);
}

void z80::op_jump_cond(int arg) {
	quint16 jumpaddr = getwordarg();

	if (jumpcond(arg)) {
		pc.setfull(jumpaddr);
		addticks(4);
	} else {
		addticks(3);
	}
}

void z80::op_jump_rel(int arg) {
	qint8 addr = getbytearg();
	bool jump = false;

	if (arg == 3) jump = true;
	else jump = jumpcond(arg & 3);

	if (jump) {
		pc += addr;
		addticks(3);
	} else {
		addticks(2);
	}
}

void z80::op_jump_hl() {
	pc.setfull(hl.getfull());
	addticks(1);
}

void z80::op_call() {
	quint16 addr = getwordarg();

	pushstack(pc.getfull());

	pc.setfull(addr);
	addticks(6);
}

void z80::op_call_cond(int arg) {
	if (jumpcond(arg)) {
		op_call();
	} else {
		pc += 2; // ''getwordarg()''
		addticks(3);
	}
}

void z80::op_ret() {
	pc.setfull(popstack());
	addticks(4);
}

void z80::op_ret_cond(int arg) {
	if (jumpcond(arg)) {
		addticks(5);
		pc.setfull(popstack());
	} else {
		addticks(2);
	}
}

void z80::op_reti() {
	interrupt_enable = true;
	op_ret();
}

void z80::op_rst_p(int arg) {
	quint8 addr = arg << 3;

	pushstack(pc.getfull());
	pc.setfull(addr);

	addticks(4);
}

void z80::op_rst_int(int address) {
	interrupt_enable = false;

	pushstack(pc.getfull());
	pc.setfull(address);

	addticks(4);
}

void z80::op_ld_mm_sp() {
	mmu->writeword(getwordarg(), sp.getfull());
	addticks(5);
}

void z80::op_ld_a_n(int arg) {
	quint16 addr = 0xFF00 | getbytearg();
	if ((arg & 1) == 0) {
		mmu->writebyte(addr, af.gethi());
	} else {
		af.sethi(mmu->readbyte(addr));
	}
	addticks(3);
}

void z80::op_ld_sp_sn() {
	sp.setfull(alu.add16mixed(sp.getfull(), getbytearg()));
	addticks(4);
}

void z80::op_ld_hl_sp_sn() {
	hl.setfull(alu.add16mixed(sp.getfull(), getbytearg()));
	addticks(3);
}

void z80::op_ld_a_c(int arg) {
	quint16 addr = 0xFF00 | bc.getlo();
	if ((arg & 1) == 0) {
		mmu->writebyte(addr, af.gethi());
	} else {
		af.sethi(mmu->readbyte(addr));
	}
	addticks(2);
}

void z80::op_ld_a_nn(int arg) {
	quint16 addr = getwordarg();
	if ((arg & 1) == 0) {
		mmu->writebyte(addr, af.gethi());
	} else {
		af.sethi(mmu->readbyte(addr));
	}
	addticks(4);
}

void z80::op_swap(int arg) {
	quint8 ans;

	if (arg == 6) {
		quint16 addr = hl.getfull();
		ans = mmu->readbyte(addr);
		ans = (ans >> 4) | (ans << 4);
		mmu->writebyte(addr, ans);
		addticks(4);
	} else {
		ans = getbyteregisterval(arg);
		ans = (ans >> 4) | (ans << 4);
		setbyteregisterval(arg, ans);
		addticks(2);
	}

	af.setflag('z', ans == 0);
	af.setflag('n', false);
	af.setflag('h', false);
	af.setflag('c', false);
}

void z80::op_daa() {
	alu.daa();
	addticks(1);
}
