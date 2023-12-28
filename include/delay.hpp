
#ifndef DELAY_HPP
#define DELAY_HPP

#include "checks/require_fcpu.h"
#include "common.hpp"

constexpr float tics_per_mhz = F_CPU / 1e6;

class Delay
{
    static void tick(byte ticks) __attribute__((hot, always_inline))
    {
      for (byte i = 0; i < ticks / 2; i++)
        asm volatile
        (
          "rjmp 0f\n"
          "0:\n"
        );

      if ((ticks % 2) == 1)
        asm volatile ("nop");
    }

  public:
    template<word delay>
    inline static void __attribute__((always_inline, hot)) ns()
    {
      constexpr dword ticks=static_cast<dword>(delay) * tics_per_mhz/1000;
      tick (ticks);
    }

    template<word delay>
    inline static void __attribute__((always_inline, hot)) us ()
    {
      constexpr dword ticks = static_cast<dword>(delay) * tics_per_mhz;

      if constexpr (ticks <= 2)                           // up to 2 cycles
        tick(ticks & 0xff);
      else if constexpr (ticks>2 && ticks<=770)           // more than 2 but less than 770 (256*3+2) cycles
      {
        byte tmp;
        constexpr word loops = (ticks/3) & 0xffff;        // number of loops = required time / single loop cost
        asm volatile
        (
          "ldi %0,%1\n"                                   // minimal cost: 3 cycles
          "0:\n"
          "dec %0\n"
          "brne 0b\n"
          :"=d"(tmp)
          :"X"(loops<256? loops : 0)                      // use 0 to simulate 256
        );

        // add final cost
        constexpr byte r = ticks - loops * 3;
        tick(r);
      }
      else
      {
        static_assert(ticks == 0 || (ticks - 1) / 4 <= 65536, "Too big loop for delay_us");
        byte t1, t2;
        constexpr word loops = ((ticks - 1) / 4) & 0xffff;     // number of loops = required time / single loop cost (there is one additional op which needs to be subtracted)
        constexpr byte l = loops & 0xff;
        constexpr byte h = loops >> 8;

        asm volatile
        (
          "ldi %0,%2\n"                                    //minimal cost: 4 cycles (first run: 5)
          "ldi %1,%3\n"
          "0:\n"
          "subi %0,1\n"
          "sbci %1,0\n"
          "brne 0b\n"
          :"=d"(t1),"=d"(t2)
          :"X"(l),"X"(h)
        );

        // add final cost
        constexpr byte r = ((ticks - 1) - loops * 4) & 0xff;
        tick(r);
      }
    }

    inline static void ms(word delay) __attribute__((cold, noinline))  //without "cold" delay's body may not become a simple loop, which is not what we want
    {
      for (word i = 0; i < delay; i++)
        us<1000>();
    }

};

#endif
