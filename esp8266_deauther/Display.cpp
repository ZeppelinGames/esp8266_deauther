#include "Display.h"

#include <SPI.h>
#include <TFT_eSPI.h>

#define ROTATION 0
TFT_eSPI tft = TFT_eSPI();

void Display::display_init() {
  tft.init();
  tft.setRotation(ROTATION);

  tft.fillScreen(0x0000);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.println("Display intialised");
}

//dies with templates for some reason

//template <typename T>
//void Display::debugln(T t) {
//  tft.println(t);
//}
//
void Display::debugln(String s) {
  tft.println(s);
}
void Display::debugln(char* s) {
  tft.println(s);
}
void Display::debugln(long s) {
  tft.println(s);
}
void Display::debugln() {
  tft.println();
}

void Display::debug(String s) {
  tft.println(s);
}
void Display::debug(char* s) {
  tft.println(s);
}
void Display::debug(long s) {
  tft.println(s);
}
