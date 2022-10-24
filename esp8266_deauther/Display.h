#pragma once

#include <Arduino.h> // Serial

class Display {
  public:
    static void display_init();
    //
    //    template <typename T>
    //    static void debugln(T);
    static void debugln(String s);
    static void debugln(char* s);
    static void debugln(long s);
    static void debugln();

    static void debug(String s);
    static void debug(char* s);
    static void debug(long s);
};
