
#ifndef COMMON_HPP
#define COMMON_HPP

#include "datatypes.h"

// byte isPowerOf2(byte arg)
// {
//   char ret=0xff;
//   if (arg==0x01)
//     ret=0;
//   else if(arg==0x02)
//     ret=1;
//   else if(arg==0x04)
//     ret=2;
//   else if(arg==0x08)
//     ret=3;
//   else if(arg==0x10)
//     ret=4;
//   else if(arg==0x20)
//     ret=5;
//   else if(arg==0x40)
//     ret=6;
//   else if(arg==0x80)
//     ret=7;
//
//   return ret;
// }

#define FLASH(name) \
    __attribute__((section(".progmem."#name)))

namespace common
{
    byte readFlash8(const byte *ptr);    //czytanie flasha, nie wymaga includowania avr/pgmspace, w zamian wymagając procesorów >=avr25
    char readFlash8(const char *ptr);
    word readFlash16(const word *ptr);
    byte fastShift8(byte v);            //funkcja zwracająca 1<<v o stałym czasie wykonania (v max ==7)
    word fastShift16(byte v);           //funkcja zwracająca 1<<v o stałym czasie wykonania (v max ==15)
    byte byte2hex(byte v);
}

#endif //COMMON_HPP
