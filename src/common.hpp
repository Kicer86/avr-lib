
#ifndef COMMON_HPP
#define COMMON_HPP

#include <avr/interrupt.h>

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
    static byte readFlash8(const byte *ptr);    //czytanie flasha, nie wymaga includowania avr/pgmspace, w zamian wymagając procesorów >=avr25
    static char readFlash8(const char *ptr);
    static word readFlash16(const word *ptr);
    static byte fastShift8(byte v);            //funkcja zwracająca 1<<v o stałym czasie wykonania (v max ==7)
    static word fastShift16(byte v);           //funkcja zwracająca 1<<v o stałym czasie wykonania (v max ==15)
    static byte byte2hex(byte v);
}


class Interrupts
{
        byte flags;
        
    public:
        Interrupts(): flags(0)
        {};

        void disable()
        {
            flags = SREG;
            cli();
        }

        void restore() const
        {
            SREG = flags;
        }
};

#endif //COMMON_HPP
