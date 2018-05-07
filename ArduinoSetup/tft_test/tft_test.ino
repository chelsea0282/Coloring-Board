#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>

Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9);
PImage logo = tft.loadImage("GemImage.bmp");

void setup() {
  Serial.begin(9600);

  
  tft.begin();

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX);
  tft.fillScreen(ILI9341_BLACK);
}

/*
 void loop(void) {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    testText();
    delay(1000);
  }
  testFillScreen();
  delay(1000);
}
*/
 void loop(void) {
  int x = 1;
  while(x) {
    tft.setRotation(1);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_RED);    
    tft.setTextSize(6);
    tft.println("Coloring Board");
    delay(500);
  }
  while (!x){
    tft.setCursor(0, 0);
//    bmpDraw(GemImage, x, y); 
  }
 }
 

void testFillScreen() {
  tft.fillScreen(ILI9341_BLACK);
  delay(500);
  tft.fillScreen(ILI9341_RED);
  delay(500);
  tft.fillScreen(ILI9341_GREEN);
  delay(500);
  tft.fillScreen(ILI9341_BLUE);
  delay(500);
  tft.fillScreen(ILI9341_BLACK);
  delay(500);
}

void testText() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Coloring board");
}

