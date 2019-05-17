/*
 * HR_FONT.hpp
 *
 *  Created on: 2019/05/11
 *      Author: haru
 */

#ifndef HRLIBS_INC_HR_FONTX2_HPP_
#define HRLIBS_INC_HR_FONTX2_HPP_

extern "C" {
#include "ff.h"
}
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#define FX2_LOWMEM 64

typedef struct{
	char sig[6];
	char name[8];
	uint8_t width;
	uint8_t height;
	uint8_t ccode;
	uint8_t codeblock; //only wide
}fontxmeta_t;

class HR_FONTX2{
public:
	HR_FONTX2(FIL* _zfont,FIL* _hfont,void (*_writedot)(uint16_t x,uint16_t y,uint16_t color));
	//HR_FONTX2(FIL* _zfont,FIL* _hfont);
	bool init();
	uint8_t writeChar(uint16_t c,uint16_t xs,uint16_t ys,uint16_t color);
	uint16_t writeString(char* s,uint16_t xs,uint16_t ys,uint16_t color);
	//uint16_t getCharBMP(uint16_t ccode,uint8_t* ret);
private:
	uint32_t findchar(uint16_t c);
	bool chk_zenkaku(uint16_t c);
	bool chk_zenkaku(uint8_t c);
};

#endif /* HRLIBS_INC_HR_FONTX2_HPP_ */
