/*
 * HR_FONTX2.c
 *
 *  Created on: Apr 28, 2022
 *      Author: Haru
 */

#include "HR_FONTX2.h"

fontxmeta_t _HR_FONTX2_zmeta, _HR_FONTX2_hmeta;
void (*_HR_FONTX2_writedot)(uint16_t x, uint16_t y, uint16_t color);
void (*_HR_FONTX2_readHFont)(uint32_t offset,uint32_t size,uint8_t* out);
void (*_HR_FONTX2_readZFont)(uint32_t offset,uint32_t size,uint8_t* out);

bool HR_FONTX2_init(void (*readHFont)(uint32_t offset,uint32_t size,uint8_t* out), void (*readZFont)(uint32_t offset,uint32_t size,uint8_t* out),
		void (*_writedot)(uint16_t x, uint16_t y, uint16_t color)) {
	_HR_FONTX2_readHFont = readHFont;
	_HR_FONTX2_readZFont = readZFont;
	_HR_FONTX2_writedot = _writedot;

	fontxmeta_t *font;
	font = (fontxmeta_t*) malloc((size_t) sizeof(fontxmeta_t));
	if (font == NULL)
		return false;

	if(_HR_FONTX2_readZFont != NULL){
		_HR_FONTX2_readZFont(0,sizeof(fontxmeta_t),(uint8_t*)font);
		if (!strcmp(font->sig, "FONTX2") || font->ccode != 1)
			_HR_FONTX2_zmeta.width = 0;
		else
			_HR_FONTX2_zmeta = *font;
	}else
		_HR_FONTX2_zmeta.width = 0;

	if(_HR_FONTX2_readHFont != NULL){
		_HR_FONTX2_readHFont(0,sizeof(fontxmeta_t),(uint8_t*)font);
		if (!strcmp(font->sig, "FONTX2") || font->ccode != 0)
			_HR_FONTX2_hmeta.width = 0;
		else
			_HR_FONTX2_hmeta = *font;
	}else
		_HR_FONTX2_hmeta.width = 0;
	free(font);

	if(_HR_FONTX2_hmeta.width == 0 && _HR_FONTX2_zmeta.width == 0)
		return false;

	return true;
}

uint8_t HR_FONTX2_writeChar(uint16_t c, uint16_t xs, uint16_t ys,
		uint16_t color) {
	if (_HR_FONTX2_writedot == NULL)
		return 0;

	uint32_t offset = 0;
	uint8_t lsize = 0;
	void (*readFont)(uint32_t offset,uint32_t size,uint8_t* out) = NULL;
	fontxmeta_t *meta;

	uint32_t readoffset = 0;

	if (HR_FONTX2_chk_zenkaku(c)) {
		readFont = _HR_FONTX2_readZFont;
		meta = &_HR_FONTX2_zmeta;
		lsize = ((meta->width + 7) / 8) * meta->height;
		offset = HR_FONTX2_findchar(c);
		if (offset == 0)
			return 0;
	} else {
		readFont = _HR_FONTX2_readHFont;
		meta = &_HR_FONTX2_hmeta;
		lsize = ((meta->width + 7) / 8) * meta->height;
		offset = 17 + c * lsize;
	}

	if(readFont == NULL)
		return 0;

	readoffset = offset;

	uint8_t *buf = (uint8_t*) malloc((size_t) lsize);
	if (buf == NULL)
		return 0;

	readFont(readoffset,lsize,buf);

	uint8_t ptr = 0, x;
	for (uint8_t y = 0; y < meta->height; y++) {
		x = 0;
		for (uint8_t ls = 0; ls < ((meta->width + 7) / 8); ls++) {
			uint8_t line = buf[ptr++], lx = 8;
			while (lx--) {
				if (line & (1 << lx))
					_HR_FONTX2_writedot(xs + x, ys + y, color);
				x++;
			}
		}
	}
	free(buf);
	return meta->width;
}

uint16_t HR_FONTX2_writeString(char *s, uint16_t xs, uint16_t ys,
		uint16_t color) {
	uint16_t ret = 0;
	uint8_t ptr = 0;
	for (;;) {
		if (s[ptr] == '\0' || s[ptr] == '\n')
			break;
		if (HR_FONTX2_chk_zenkaku8((uint8_t) s[ptr])) {
			uint16_t c = (s[ptr++]) << 8 | (s[ptr++]);
			ret += HR_FONTX2_writeChar(c, xs + ret, ys, color) + 1;
		} else {
			ret += HR_FONTX2_writeChar(s[ptr++], xs + ret, ys, color) + 1;
		}
	}
	return ret;
}

uint32_t HR_FONTX2_findchar(uint16_t c) {
	if(_HR_FONTX2_readZFont == NULL)
		return 0;

	uint16_t *buf, count = 0;
	uint32_t offset = 0;
	buf = (uint16_t*) malloc((size_t) (sizeof(uint16_t) * _HR_FONTX2_zmeta.codeblock * 2));
	if (buf == NULL) { //low-memory mode
		buf = (uint16_t*) malloc((size_t) (sizeof(uint16_t) * FX2_LOWMEM * 2));
		if (buf == NULL)
			return 0; //failed to get 256B buffer

		uint8_t cnt = 0;
		uint16_t byte = _HR_FONTX2_zmeta.codeblock * 4;
		uint8_t r = byte % FX2_LOWMEM;
		cnt = byte / FX2_LOWMEM;
		for (uint8_t i = 0; i < cnt; i++) {
			offset = 18 + FX2_LOWMEM * i;
			_HR_FONTX2_readZFont(offset,FX2_LOWMEM * 2,(uint8_t*)buf);
			for (uint8_t j = 0; j < FX2_LOWMEM / 2; j += 2) {
				if (c >= buf[j] && c <= buf[j + 1]) {
					count += c - buf[j];
					free(buf);
					return 18 + 4 * _HR_FONTX2_zmeta.codeblock
							+ count * ((_HR_FONTX2_zmeta.width + 7) / 8) * _HR_FONTX2_zmeta.height;
				}
				count += buf[j + 1] - buf[j] + 1;
			}
		}
		if (r > 0) {
			offset = 18 + FX2_LOWMEM * cnt;
			_HR_FONTX2_readZFont(offset,r,(uint8_t*)buf);
			for (uint8_t j = 0; j < (r / 2); j += 2) {
				if (c >= buf[j] && c <= buf[j + 1]) {
					count += c - buf[j];
					free(buf);
					return 18 + 4 * _HR_FONTX2_zmeta.codeblock
							+ count * ((_HR_FONTX2_zmeta.width + 7) / 8) * _HR_FONTX2_zmeta.height;
				}
				count += buf[j + 1] - buf[j] + 1;
			}
		}
		goto err;
	} else {
		offset = 18;
		_HR_FONTX2_readZFont(offset,sizeof(uint16_t) * _HR_FONTX2_zmeta.codeblock * 2,(uint8_t*)buf);
		for (uint8_t j = 0; j < _HR_FONTX2_zmeta.codeblock * 2; j += 2) {
			if (c >= buf[j] && c <= buf[j + 1]) {
				count += c - buf[j];
				free(buf);
				return 18 + 4 * _HR_FONTX2_zmeta.codeblock
						+ count * ((_HR_FONTX2_zmeta.width + 7) / 8) * _HR_FONTX2_zmeta.height;
			}
			count += buf[j + 1] - buf[j] + 1;
		}
		goto err;
	}

	err: free(buf);
	return 0;
}

bool HR_FONTX2_chk_zenkaku(uint16_t c) {
	uint8_t fc = c >> 8;
	return (fc >= 0x80 && fc <= 0x9F) || (fc >= 0xe0 && fc <= 0xfc);
}

bool HR_FONTX2_chk_zenkaku8(uint8_t c) {
	return (c >= 0x80 && c <= 0x9F) || (c >= 0xe0 && c <= 0xfc);
}
