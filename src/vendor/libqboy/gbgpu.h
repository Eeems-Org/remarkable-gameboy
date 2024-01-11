#ifndef GBGPU_H
#define GBGPU_H

#include "libqboy_global.h"
#include "z80mmu.h"
#include <vector>

const int _GBGPU_W = 160;
const int _GBGPU_H = 144;
const int _GBGPU_VRAMBASE = 0x8000;
const int _GBGPU_VREGBASE = 0xFF40;
const int _GBGPU_VREGSIZE = 0xC;
const int _GBGPU_VOAMBASE = 0xFE00;

union gbgpu_oamflags {
	struct {
		quint8 reserved : 4;
		bool pal1 : 1;
		bool xflip : 1;
		bool yflip : 1;
		bool belowbg : 1;
	} flags;
	quint8 byte;
};

class gbgpu {
public:
	gbgpu(z80mmu *mmu);
	void reset();
	void step(int z80m);
	bool is_updated();
	quint8 *getLCD();
private:
	z80mmu *mmu;
	quint8 screen_buffer[_GBGPU_H][_GBGPU_W][4];

	int mode;
	int modeclock;
	int line;
	bool scanlinetransfered;
	int oldmode;
	int oldline;

	bool updated;

	quint8 pallete_bg[4], pallete_obj0[4], pallete_obj1[4];
	std::vector<quint8> registers;

	bool lcd_on();
	bool bg_on();
	bool win_on();
	quint16 bg_mapbase();
	quint16 win_mapbase();
	bool tileset1();
	bool sprite_on();
	bool sprite_large();
	quint8 yscroll();
	quint8 xscroll();
	quint8 linecmp();
	quint8 winxpos();
	quint8 winypos();


	void renderscan();
	void drawbackground();
	void drawwindow();
	void drawsprites();
	void updatebuffer();
	void preprocessram();
	void postprocessram();
	void setcolour(int x, quint8 value);
	quint8 getcolour(int x);
};

#endif // GBGPU_H
