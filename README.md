# HR_FONTX2 : Universal FONTX2 Library written in C
複数の環境やLCDで動作させることを想定し，関数ポインタを用いてフォント読み込みや描画を行うことでポータビリティを向上させています．

## 要求環境
- C
- ドット描画を行う事ができる関数
- 全角・半角のフォントを読み出す事ができる関数

ドット描画に関数ポインタを使用しているので様々な液晶で動かせると思います。
ドット描画を行えない表示器の場合は，バッファを操作する関数を作成する事で代用できます．

フォントは少なくともどちらかがあれば動く為，メモリの少ないマイコンでは英字のみを使用する等が可能です．

## 使用例
![oled](https://user-images.githubusercontent.com/13781980/167238118-de0f574b-ad0f-448c-a0b9-42d05f32bdf1.jpg)
STM32F401CCU6とSSD1306での動作イメージ

![lcd](https://user-images.githubusercontent.com/13781980/167238163-648adac6-cf12-4e46-b360-a61b0e45b07d.jpg)
STM32F3-DiscoveryとILI9488での動作イメージ

※左半分が割れているので表示が変になっています．

ソースコード:
https://github.com/Haruroid/STM32-ILI9488-GPIO

![gbdk](https://user-images.githubusercontent.com/13781980/167248508-308141bc-cbfe-4b6a-beb1-afc71f297d8e.png)
GBDKを使用したゲームボーイでの動作イメージ

ソースコード:
https://github.com/Haruroid/GB-FONTX2-Sample


```c
void lcdwrite(uint16_t x, uint16_t y, uint16_t col) {
	ILI9488_drawRect(x, y, 1, 1, col);
}

void readZ(uint32_t off,uint32_t size,uint8_t* buf){
	memcpy(buf,ZenFont+off,size);
}

void readH(uint32_t off,uint32_t size,uint8_t* buf){
	memcpy(buf,HanFont+off,size);
}

void app(){
	if(!HR_FONTX2_init(readH,readZ,lcdwrite))
		while(1);
	HR_FONTX2_writeString("日本語テスト卍English", 200, 0, 0xffff);
}
```
半角・全角のフォント読み込み関数，描画関数を指定してwriteStringで描画を行います．
SJISなので注意して下さい．

バグがあるかもしれません．報告歓迎です
