
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

class Com
{
    static const byte FLASH(shiftTable8) shiftTable8[8];
    static const word FLASH(shiftTable16) shiftTable16[16];
    
  public:
    static byte readFlash8(const byte *ptr)    //czytanie flasha, nie wymaga includowania avr/pgmspace, w zamian wymagając procesorów >=avr25
    {
      byte ret;
      asm (
        "lpm %0,Z    \n\t"
        :"=r"(ret)
        :"z"(ptr)
          );
      return ret;
    }

    static char readFlash8(const char *ptr)
    {
      char ret;
      asm (
        "lpm %0,Z    \n\t"
      :"=r"(ret)
            :"z"(ptr)
          );
      return ret;
    }

    static word readFlash16(const word *ptr)
    {
      word ret;
      asm
      (
        "lpm %A0,Z+    \n\t"
        "lpm %B0,Z     \n\t"
      :"=r"(ret)
            :"z"(ptr)
          );
      return ret;
    }

    static byte fastShift8(byte v)             //funkcja zwracająca 1<<v o stałym czasie wykonania (v max ==7)
    {
      return readFlash8(&shiftTable8[v]);
    }

    static word fastShift16(byte v)            //funkcja zwracająca 1<<v o stałym czasie wykonania (v max ==15)
    {
      return readFlash16(&shiftTable16[v]);
    }
    
    static byte byte2hex(byte v)
    {
      v&=0xf;
      return v<10? '0'+v : 'A'+v-10;
    }
};


class Interrupts
{
  byte flags;
public:
  Interrupts():flags(0)
  {};
  
  void disable()
  {
    flags=SREG;
    cli();
  }
  
  void restore() const
  {
    SREG=flags;
  }
};

#endif //COMMON_HPP
