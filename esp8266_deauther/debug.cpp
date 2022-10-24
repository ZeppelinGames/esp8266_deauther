#include "debug.h"

#include <SPI.h>
#include <TFT_eSPI.h>

#define ROTATION 0
TFT_eSPI tft = TFT_eSPI();

void debug_init() {
  DEBUG_PORT.begin(DEBUG_BAUD);
  DEBUG_PORT.setTimeout(LONG_MAX);
  DEBUG_PORT.println();

  tft.init();
  tft.setRotation(ROTATION);

  tft.fillScreen(0x0000);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.println("Debugger intialised");
}

template <typename T>
void Debugger::debugln(T t) {
  DEBUG_PORT.println(t);
  tft.println(t);
}
void Debugger::debugln() {
  DEBUG_PORT.println();
  tft.println();
}
