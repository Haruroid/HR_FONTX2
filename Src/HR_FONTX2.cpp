/*
 * HR_FONT.cpp
 *
 *  Created on: 2019/05/11
 *      Author: haru
 */
#include <HR_FONTX2.hpp>

FIL *zfont,*hfont;
fontxmeta_t zmeta,hmeta;
void (*writedot)(uint16_t x, uint16_t y, uint16_t color);

HR_FONTX2::HR_FONTX2(FIL* _zfont,FIL* _hfont,
		void (*_writedot)(uint16_t x, uint16_t y, uint16_t color)) {
	zfont = _zfont;
	hfont = _hfont;
	writedot = _writedot;
}

//HR_FONTX2::HR_FONTX2(FIL* _zfont,FIL* _hfont) {
//	HR_FONTX2(_zfont,_hfont, NULL);
//}

bool HR_FONTX2::init() {
	fontxmeta_t* font;
	font = (fontxmeta_t*) malloc((size_t) sizeof(fontxmeta_t));
	if (font == NULL)
		return false;
	UINT br;
	//Zenkaku-font
	f_lseek(zfont, 0);
	if (f_read(zfont, font, sizeof(fontxmeta_t), &br) != FR_OK)
		goto err;
	if (!strcmp(font->sig, "FONTX2"))
		goto err;
	if(font->ccode != 1)
		goto err;
	zmeta = *font;

	//Hankaku-font
	f_lseek(hfont, 0);
	if (f_read(hfont, font, sizeof(fontxmeta_t), &br) != FR_OK)
		goto err;
	if (!strcmp(font->sig, "FONTX2"))
		goto err;
	if(font->ccode != 0)
		goto err;
	hmeta = *font;

	free(font);
	return true;

	err:
	free(font);
	return false;
}

uint8_t HR_FONTX2::writeChar(uint16_t c, uint16_t xs, uint16_t ys,
		uint16_t color) {
	if (writedot == NULL)
		return 0;

	uint32_t offset = 0;
	uint8_t lsize = 0;
	FIL* ffont;
	fontxmeta_t* meta;

	if(chk_zenkaku(c)){
		ffont = zfont;
		meta = &zmeta;
		lsize = ((meta->width + 7)/8) * meta->height;
		offset = findchar(c);
		if(offset == 0)
			return 0;
	}else{
		ffont = hfont;
		meta = &hmeta;
		lsize = ((meta->width + 7)/8) * meta->height;
		offset = 17 + c * lsize;
	}

	f_lseek(ffont, offset);

	uint8_t* buf = (uint8_t*) malloc((size_t) lsize);
	if (buf == NULL)
		return 0;

	UINT br;
	f_read(ffont, buf, lsize, &br);

	uint8_t ptr = 0, x;
	for (uint8_t y = 0; y < meta->height; y++) {
		x = 0;
		for (uint8_t ls = 0; ls < ((meta->width + 7) / 8); ls++) {
			uint8_t line = buf[ptr++],lx = 7;
			while(lx--){
				if (line & (1 << lx))
					writedot(xs + x, ys + y, color);
				x++;
			}
		}
	}
	return meta->width;
}

uint16_t HR_FONTX2::writeString(char* s,uint16_t xs,uint16_t ys,uint16_t color){
	uint16_t ret = 0;
	uint8_t ptr = 0;
	for(;;){
		if(s[ptr] == '\0' || s[ptr] == '\n')
			break;
		if(chk_zenkaku((uint8_t)s[ptr])){
			uint16_t c = (s[ptr++]) <<8 | (s[ptr++]);
			ret += writeChar(c,xs + ret,ys,color) + 1;
		}else{
			ret += writeChar(s[ptr++],xs + ret,ys,color) + 1;
		}
	}
	return ret;
}

//uint16_t HR_FONTX2::getCharBMP(uint16_t ccode, uint8_t* ret) {
//	return 1;
//}

uint32_t HR_FONTX2::findchar(uint16_t c) {
	uint16_t *buf, count = 0;
	UINT br;
	buf = (uint16_t*) malloc((size_t) (sizeof(uint16_t) * zmeta.codeblock * 2));
	if (buf == NULL) { //low-memory mode
		buf = (uint16_t*) malloc((size_t) (sizeof(uint16_t) * FX2_LOWMEM * 2));
		if (buf == NULL)
			return 0; //failed to get 256B buffer

		uint8_t cnt = 0;
		uint16_t byte = zmeta.codeblock * 4;
		uint8_t r = byte % FX2_LOWMEM;
		cnt = byte / FX2_LOWMEM;
		for (uint8_t i = 0; i < cnt; i++) {
			f_lseek(zfont, 18 + FX2_LOWMEM * i);
			if (f_read(zfont, buf, FX2_LOWMEM * 2, &br) != FR_OK)
				goto err;
			for (uint8_t j = 0; j < FX2_LOWMEM / 2; j += 2) {
				if (c >= buf[j] && c <= buf[j + 1]) {
					count += c - buf[j];
					free(buf);
					return 18 + 4 * zmeta.codeblock
							+ count * ((zmeta.width + 7) / 8) * zmeta.height;
				}
				count += buf[j + 1] - buf[j] + 1;
			}
		}
		if (r > 0) {
			f_lseek(zfont, 18 + FX2_LOWMEM * cnt);
			if (f_read(zfont, buf, r, &br) != FR_OK)
				goto err;
			for (uint8_t j = 0; j < (r / 2); j += 2) {
				if (c >= buf[j] && c <= buf[j + 1]) {
					count += c - buf[j];
					free(buf);
					return 18 + 4 * zmeta.codeblock
							+ count * ((zmeta.width + 7) / 8) * zmeta.height;
				}
				count += buf[j + 1] - buf[j] + 1;
			}
		}
		goto err;
	} else {
		f_lseek(zfont, 18);
		f_read(zfont, buf, sizeof(uint16_t) * zmeta.codeblock * 2, &br);
		for (uint8_t j = 0; j < zmeta.codeblock * 2; j += 2) {
			if (c >= buf[j] && c <= buf[j + 1]) {
				count += c - buf[j];
				free(buf);
				return 18 + 4 * zmeta.codeblock
						+ count * ((zmeta.width + 7) / 8) * zmeta.height;
			}
			count += buf[j + 1] - buf[j] + 1;
		}
		goto err;
	}

	err:
	free(buf);
	return 0;
}

bool HR_FONTX2::chk_zenkaku(uint16_t c){
	uint8_t fc = c >> 8;
	return (fc >= 0x80 && fc <= 0x9F) || (fc >= 0xe0 && fc <= 0xfc);
}

bool HR_FONTX2::chk_zenkaku(uint8_t c){
	return (c >= 0x80 && c <= 0x9F) || (c >= 0xe0 && c <= 0xfc);
}
