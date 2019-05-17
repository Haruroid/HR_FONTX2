# HR_FONTX2 : Universal FONTX2 Library written in C++
このライブラリは複数の環境やLCDで動かすことを想定して作られています。しかし今の所STM32でしか動かしたことがないので現状サポート環境はSTM32のみです(笑)

## 要求環境
- C++
- **Chan氏のFatFs**
- void型で(uint16_t x,uint16_t y,uint16_t color)を引数に持つドット描画関数

ドット描画に関数ポインタを使用しているので様々な液晶で動かせると思います。

またC++の練習で作ったので荒いコーディングは無視して下さい....

Chan氏のFatFsに結構依存してるのでArduinoとかで使う場合は少し工夫が必要です。
多分Arduinoにはもっといいライブラリがあるでしょうが...

## 使用例:
![イメージ](https://haruroid.0t0.jp/blog/wp-content/uploads/2019/05/20190517_225250.jpg)

```
	st = new ST7789(s, d, r);
	st->init();

	FIL zfont, hfont;
	void (*writedot)(uint16_t x, uint16_t y, uint16_t color) = writed;

	uint32_t rn;

	st->fillRect(0, 240, 0, 240, RGB565(0x00, 0x00, 0x00));

	if (f_open(&zfont, "/ILGZ24XB.FNT", FA_OPEN_EXISTING | FA_READ)
			!= FR_OK) {
		st->fillRect(0, 240, 0, 240, RGB565(0xFF, 0x00, 0xFF));
		while (1)
			;
	}
	if (f_open(&hfont, "/ILGH24XB.FNT", FA_OPEN_EXISTING | FA_READ) != FR_OK) {
		st->fillRect(0, 240, 0, 240, RGB565(0xFF, 0x00, 0xFF));
		while (1)
			;
	}

	HR_FONTX2 fnx = HR_FONTX2(&zfont,&hfont, writedot);
	if (!fnx.init()) {
		st->fillRect(0, 240, 0, 240, RGB565(0xFF, 0x00, 0xFF));
		while (1)
			;
	}

	fnx.writeString("abcあいう漢卍",0,0,0xFFFF);
```
※writedは液晶のドット描画する関数(のラッパ)です。クラスへの関数ポインタがいまいちわからないのでこういう事になってます...

バグがあるかもしれません。報告歓迎です