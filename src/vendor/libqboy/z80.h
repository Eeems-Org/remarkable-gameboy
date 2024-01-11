#ifndef Z80_H
#define Z80_H

#include "libqboy_global.h"
#include "z80mmu.h"
#include "z80alu.h"
#include "z80register.h"

enum Direction {
	Direction_LEFT,
	Direction_RIGHT
};

class z80 {
public:
	z80(z80mmu *mmu);
	void reset();
	void cycle();
	int get_m();
	bool is_halted();
private:
	unsigned int last_m;
	bool interrupt_enable;

	z80mmu *mmu;
	z80alu alu;
	z80register af, bc, de, hl, sp, pc;
	bool halted;

	bool handle_interrupt();
	quint8 getbytearg();
	quint16 getwordarg();
	quint8 getbyteregisterval(int code);
	void setbyteregisterval(int code, quint8 val);
	quint16 getwordregisterval(int code, bool lastsp);
	void setwordregisterval(int code, bool lastsp, quint16 val);
	void pushstack(quint16 val);
	quint16 popstack();
	void addticks(int m);

	bool jumpcond(int arg);

	void call(quint8 opcode);
	void call_extended();

	void op_nop();
	void op_ld_r_r(int arg1, int arg2);
	void op_ld_r_n(int arg);
	void op_ld_a_ss(int arg);
	void op_ld_a_hl(int arg);
	void op_ld_dd_nn(int arg);
	void op_ld_sp_hl();
	void op_push_qq(int arg);
	void op_pop_qq(int arg);
	void op_add_r(int arg, bool withcarry);
	void op_add_n(bool withcarry);
	void op_sub_r(int arg, bool withcarry);
	void op_sub_n(bool withcarry);
	void op_and_r(int arg);
	void op_and_n();
	void op_or_r(int arg);
	void op_or_n();
	void op_xor_r(int arg);
	void op_xor_n();
	void op_cp_r(int arg);
	void op_cp_n();
	void op_inc_r(int arg);
	void op_dec_r(int arg);
	void op_cpl();
	void op_ccf();
	void op_scf();
	void op_halt();
	void op_di();
	void op_ei();
	void op_add16_rr(int arg);
	void op_inc16_rr(int arg);
	void op_dec16_rr(int arg);
	void op_rdca(Direction dir);
	void op_rda(Direction dir);
	void op_rdc_r(int arg, Direction dir);
	void op_rd_r(int arg, Direction dir);
	void op_sda(int arg, Direction dir);
	void op_srl(int arg);
	void op_bit(quint8 bit, int arg);
	void op_set(quint8 bit, int arg);
	void op_res(quint8 bit, int arg);
	void op_jump();
	void op_jump_cond(int arg);
	void op_jump_rel(int arg);
	void op_jump_hl();
	void op_call();
	void op_call_cond(int arg);
	void op_ret();
	void op_ret_cond(int arg);
	void op_reti();
	void op_rst_p(int arg);
	void op_rst_int(int address);
	void op_ld_mm_sp();
	void op_ld_a_n(int arg);
	void op_ld_sp_sn();
	void op_ld_hl_sp_sn();
	void op_ld_a_c(int arg);
	void op_ld_a_nn(int arg);
	void op_swap(int arg);
	void op_daa();
};

#endif // Z80_H
