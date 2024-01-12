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

    inline quint8 readbyte_zram(quint16 address){ return zram[address & 0xFF]; }
    inline quint8 readbyte_voam(quint16 address){
        if(address < 0xFEA0){
            return voam[address & 0xFF];
        }
        return 0;
    }
    inline quint8 readbyte_wram(quint16 address){ return wram[address & 0x1FFF]; }
    inline quint8 readbyte_vram(quint16 address){ return vram[address & 0x1FFF]; }

    inline void writebyte_zram(quint16 address, quint8 value){ zram[address & 0xFF] = value; }
    inline void writebyte_voam(quint16 address, quint8 value){
        if(address < 0xFEA0){
            voam[address & 0xFF] = value;
        }
    }
    inline void writebyte_wram(quint16 address, quint8 value){ wram[address & 0x1FFF] = value; }
    inline void writebyte_vram(quint16 address, quint8 value){ vram[address & 0x1FFF] = value; }

private:
    z80mbc *mbc;
    std::vector<quint8> wram, vram, voam, zram;
    std::string savefilename;
};

#endif // MMU_H
