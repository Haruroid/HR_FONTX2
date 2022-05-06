/*
 * HR_FONT.hpp
 *
 *  Created on: 2019/05/11
 *      Author: haru
 */

#ifndef HRLIBS_INC_HR_FONTX2_HPP_
#define HRLIBS_INC_HR_FONTX2_HPP_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#define FX2_LOWMEM 64

typedef struct {
	char sig[6];
	char name[8];
	uint8_t width;
	uint8_t height;
	uint8_t ccode;
	uint8_t codeblock; //only wide
} fontxmeta_t;

bool HR_FONTX2_init(void (*readHFont)(uint32_t offset,uint32_t size,uint8_t* out), void (*readZFont)(uint32_t offset,uint32_t size,uint8_t* out),
		void (*_writedot)(uint16_t x, uint16_t y, uint16_t color));
uint8_t HR_FONTX2_writeChar(uint16_t c, uint16_t xs, uint16_t ys,
		uint16_t color);
uint16_t HR_FONTX2_writeString(char *s, uint16_t xs, uint16_t ys,
		uint16_t color);
uint32_t HR_FONTX2_findchar(uint16_t c);
bool HR_FONTX2_chk_zenkaku(uint16_t c);
bool HR_FONTX2_chk_zenkaku8(uint8_t c);

#endif /* HRLIBS_INC_HR_FONTX2_HPP_ */
