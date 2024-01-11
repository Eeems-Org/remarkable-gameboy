#include "gbgpu.h"

gbgpu::gbgpu(z80mmu *mmu) {
	this->mmu = mmu;
	reset();
}

void gbgpu::reset() {
	mode = 0;
	modeclock = 0;
	line = 0;
	scanlinetransfered = false;
	registers.resize(_GBGPU_VREGSIZE, 0);

	for (int i = 0; i < 4; ++i) {
		pallete_bg[i] = pallete_obj0[i] = pallete_obj1[i] = 255;
	}

	for (int y = 0; y < _GBGPU_H; ++y) {
		for (int x = 0; x < _GBGPU_W; ++x) {
			for (int c = 0; c < 4; ++c) {
				screen_buffer[y][x][c] = 255;
			}
		}
	}
}

#include <iostream>

void gbgpu::step(int z80m) {
	preprocessram();

	updated = false;
	if (!lcd_on()) return;
	modeclock += z80m;

	switch (mode) {
	case 2:
		if (modeclock >= 20) {
			modeclock -= 20;
			mode = 3;
			scanlinetransfered = false;
		}
		break;
	case 3:
		if (modeclock >= 12 && !scanlinetransfered) {
			renderscan();
			scanlinetransfered = true;
		}
		if (modeclock >= 43) {
			modeclock -= 43;
			mode = 0;
		}
		break;
	case 0:
		if (modeclock >= 51) {
			modeclock -= 51;
			++line;
			if (line == 144) {
				mode = 1;
				updated = true;
			} else {
				mode = 2;
			}
		}
		break;
	case 1:
		if (modeclock >= 114) {
			modeclock -= 114;

			// stay 10 iterations in this loop, but increase line the first 9 times
			if (line == 153) {
				mode = 2;
				line = 0;
			}

			if (mode == 1) {
				++line;
			}
		}
		break;
	}
	postprocessram();
}

quint8 *gbgpu::getLCD() {
	return &screen_buffer[0][0][0];
}

bool gbgpu::is_updated() {
	return updated;
}

bool gbgpu::lcd_on() {
	return registers[0] & 0x80;
}

bool gbgpu::bg_on() {
	return registers[0] & 0x01;
}

bool gbgpu::win_on() {
	return registers[0] & 0x20;
}

bool gbgpu::sprite_large() {
	return registers[0] & 0x04;
}

quint16 gbgpu::bg_mapbase() {
	if (registers[0] & 0x08) {
		return _GBGPU_VRAMBASE + 0x1C00;
	} else {
		return _GBGPU_VRAMBASE + 0x1800;
	}
}

quint16 gbgpu::win_mapbase() {
	if (registers[0] & 0x40) {
		return _GBGPU_VRAMBASE + 0x1C00;
	} else {
		return _GBGPU_VRAMBASE + 0x1800;
	}
}

bool gbgpu::tileset1() {
	return registers[0] & 0x10;
}

bool gbgpu::sprite_on() {
	return registers[0] & 0x02;
}

quint8 gbgpu::yscroll() {
	return registers[2];
}

quint8 gbgpu::xscroll() {
	return registers[3];
}

quint8 gbgpu::winypos() {
	return registers[10];
}

quint8 gbgpu::winxpos() {
	return registers[11];
}

quint8 gbgpu::linecmp() {
	return registers[5];
}

void gbgpu::preprocessram() {
	std::vector<quint8> newregisters(_GBGPU_VREGSIZE, 0);

	for (int i = 0; i < _GBGPU_VREGSIZE; ++i) {
		newregisters[i] = mmu->readbyte(_GBGPU_VREGBASE + i);
	}

	if (newregisters[6] != 0) {
		quint16 baseaddr = newregisters[6];
		baseaddr <<= 8;
		for(quint8 i = 0; i < 0xA0; ++i) {
			mmu->writebyte(_GBGPU_VOAMBASE + i, mmu->readbyte(baseaddr + i));
		}
		mmu->writebyte(_GBGPU_VREGBASE + 6, 0);
	}

	if (newregisters[7] != registers[7]) {
		for(int i = 0; i < 4; ++i) {
			switch((newregisters[7] >> (2*i)) & 3) {
			case 0: pallete_bg[i] = 255; break;
			case 1: pallete_bg[i] = 192; break;
			case 2: pallete_bg[i] = 96; break;
			case 3: pallete_bg[i] = 0; break;
			}
		}
	}

	if (newregisters[8] != registers[8]) {
		for(int i = 0; i < 4; ++i) {
			switch((newregisters[8] >> (2*i)) & 3) {
			case 0: pallete_obj0[i] = 255; break;
			case 1: pallete_obj0[i] = 192; break;
			case 2: pallete_obj0[i] = 96; break;
			case 3: pallete_obj0[i] = 0; break;
			}
		}
	}

	if (newregisters[9] != registers[9]) {
		for(int i = 0; i < 4; ++i) {
			switch((newregisters[9] >> (2*i)) & 3) {
			case 0: pallete_obj1[i] = 255; break;
			case 1: pallete_obj1[i] = 192; break;
			case 2: pallete_obj1[i] = 96; break;
			case 3: pallete_obj1[i] = 0; break;
			}
		}
	}

	registers = newregisters;
	oldmode = mode;
	oldline = line;
}

void gbgpu::postprocessram() {
	mmu->writebyte(_GBGPU_VREGBASE + 4, line);

	quint8 vreg1 = registers[1];
	vreg1 &= 0xF8;
	vreg1 |= (line == linecmp() ? 4 : 0) | (mode & 0x3);

	mmu->writebyte(_GBGPU_VREGBASE + 1, vreg1);

	bool lcdstat = false;
	if (line != oldline) {
		if ((vreg1 & 0x40) && line == linecmp()) lcdstat = true;
	}
	if (mode != oldmode) {
		if ((vreg1 & 0x20) && mode == 2) lcdstat = true;
		if ((vreg1 & 0x10) && mode == 1) lcdstat = true;
		if ((vreg1 & 0x08) && mode == 0) lcdstat = true;
	}

	if (lcdstat || updated) {
		quint8 int_flags = mmu->readbyte(0xFF0F);
		int_flags |= (lcdstat ? 0x2 : 0) | (updated ? 0x1 : 0);
		mmu->writebyte(0xFF0F, int_flags);
	}
}

void gbgpu::renderscan() {
	drawbackground();
	drawwindow();
	drawsprites();
}

void gbgpu::drawbackground() {
	if (!bg_on()) return;

	int bgx = xscroll();
	int bgy = yscroll() + line;
	quint16 mapbase = bg_mapbase();

	int tiley = (bgy >> 3) & 31;
	int tilex;

	for (int x = 0;x < _GBGPU_W; ++x, ++bgx) {
		tilex = (bgx >> 3) & 31;

		quint16 tileaddress = 0;
		if (tileset1()) {
			quint8 tilenr = mmu->readbyte(mapbase + tiley * 32 + tilex);
			tileaddress = 0x8000 + tilenr * 16;
		} else {
			// Signed!
			qint8 tilenr = mmu->readbyte(mapbase + tiley * 32 + tilex);
			tileaddress = 0x9000 + tilenr * 16;
		}

		quint8 byte1 = mmu->readbyte(tileaddress + ((bgy & 0x07) * 2));
		quint8 byte2 = mmu->readbyte(tileaddress + ((bgy & 0x07) * 2) + 1);

		quint8 xbit = bgx & 0x07;
		quint8 colnr = 0;
		colnr |= (byte1 & (0x01 << (7 - xbit))) ? 0x1 : 0;
		colnr |= (byte2 & (0x01 << (7 - xbit))) ? 0x2 : 0;
		quint8 colour = pallete_bg[colnr];

		setcolour(x, colour);
	}
}

void gbgpu::drawwindow() {
	if (!win_on()) return;
	int winy = line - winypos();
	if (winy < 0) return;

	int winx = - (winxpos() - 7);
	quint16 mapbase = win_mapbase();

	int tiley = (winy >> 3) & 31;
	int tilex;

	for (int x = 0; x < _GBGPU_W; ++x, ++winx) {
		if (winx < 0) continue;
		tilex = (winx >> 3) & 31;

		quint16 tileaddress = 0;
		if (tileset1()) {
			quint8 tilenr = mmu->readbyte(mapbase + tiley * 32 + tilex);
			tileaddress = 0x8000 + tilenr * 16;
		} else {
			// Signed!
			qint8 tilenr = static_cast<qint8>(mmu->readbyte(mapbase + tiley * 32 + tilex));
			tileaddress = 0x9000 + tilenr * 16;
		}

		quint8 byte1 = mmu->readbyte(tileaddress + ((winy & 0x07) * 2));
		quint8 byte2 = mmu->readbyte(tileaddress + ((winy & 0x07) * 2) + 1);

		quint8 xbit = winx % 8;
		quint8 colnr = 0;
		colnr |= (byte1 & (0x80 >> xbit)) ? 0x1 : 0;
		colnr |= (byte2 & (0x80 >> xbit)) ? 0x2 : 0;
		quint8 colour = pallete_bg[colnr];

		setcolour(x, colour);
	}
}

void gbgpu::drawsprites() {
	if (!sprite_on()) return;

	int spriteheight = sprite_large() ? 16 : 8;

	int numsprites = 0;
	std::vector<int> spritey(40);
	std::vector<int> spritex(40);
	std::vector<int> spritetile(40);
	std::vector<gbgpu_oamflags> spriteflags(40);

	for (int i = 0; i < 40; ++i) {
		quint16 spriteaddr = _GBGPU_VOAMBASE + i*4;

		spritey[i] = mmu->readbyte(spriteaddr + 0) - 16;
		spritex[i] = mmu->readbyte(spriteaddr + 1) - 8;
		spritetile[i] = mmu->readbyte(spriteaddr + 2) & (spriteheight == 16 ? 0xFE : 0xFF);
		spriteflags[i].byte = mmu->readbyte(spriteaddr + 3);

		if (line >= spritey[i] && line < spritey[i] + spriteheight)
			numsprites++;
	}



	for (int i = 39; i >= 0; --i) {
		if (line < spritey[i] || line >= spritey[i] + spriteheight)
			continue;

		if (numsprites-- > 10) continue;

		if (spritex[i] < -7 || spritex[i] >= _GBGPU_W)
			continue;

		int tiley = line - spritey[i];
		if (spriteflags[i].flags.yflip)
			tiley = (spriteheight - 1) - tiley;

		quint16 tileaddress = 0x8000 + spritetile[i] * 16 + tiley * 2;
		quint8 byte1 = mmu->readbyte(tileaddress);
		quint8 byte2 = mmu->readbyte(tileaddress + 1);

		for (int x = 0; x < 8; ++x) {
			if (spritex[i] + x < 0 || spritex[i] + x >= _GBGPU_W)
				continue;

			int colnr = 0;
			colnr |= (byte1 & (0x01 << (spriteflags[i].flags.xflip ? x : 7 - x))) ? 1 : 0;
			colnr |= (byte2 & (0x01 << (spriteflags[i].flags.xflip ? x : 7 - x))) ? 2 : 0;

			// colnr 0 is always transparant
			if (colnr == 0) continue;
			// if belowbg, then only draw on bg[0]
			if (spriteflags[i].flags.belowbg && getcolour(spritex[i] + x) != pallete_bg[0]) continue;

			int colour = spriteflags[i].flags.pal1 ? pallete_obj1[colnr] : pallete_obj0[colnr];

			setcolour(spritex[i] + x, colour);
		}
	}
}

void gbgpu::setcolour(int x, quint8 value) {
	screen_buffer[line][x][0] = screen_buffer[line][x][1] = screen_buffer[line][x][2] = value;
}

quint8 gbgpu::getcolour(int x) {
	return screen_buffer[line][x][0];
}
