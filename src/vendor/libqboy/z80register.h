#ifndef Z80REGISTER_H
#define Z80REGISTER_H

#include "libqboy_global.h"

class z80register {
public:
	z80register();
	void setaf();
	void reset();
	void setfull(quint16 val);
	void setlo(quint8 val);
	void sethi(quint8 val);
	quint16 getfull();
	quint8 getlo();
	quint8 gethi();
	bool getflag(char type);
	void setflag(char type, bool val);

	void operator+=(qint16 val);
	void operator-=(qint16 val);
private:
	union {
		struct part_s {
			quint8 lo;
			quint8 hi;
		} part;
		quint16 full;
	} data;
	int lomask;
	int getflagmask(char type);
};

#endif // Z80REGISTER_H
