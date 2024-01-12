#ifndef Z80REGISTER_H
#define Z80REGISTER_H

#include "libqboy_global.h"

#define FLAG_BIT_c (1 << 4)
#define FLAG_BIT_h (1 << 5)
#define FLAG_BIT_n (1 << 6)
#define FLAG_BIT_z (1 << 7)

#define setflag(flag, val) \
_setflag(FLAG_BIT_##flag, val)

#define getflag(flag) \
_getflag(FLAG_BIT_##flag)

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
    inline void _setflag(int flag, bool val){ setlo(val ? getlo() | flag : getlo() & ~flag); }
    inline bool _getflag(int flag){ return getlo() & flag; }

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
};

#endif // Z80REGISTER_H
