#ifndef DELAY_HPP
#define DELAY_HPP


#ifndef F_CPU
#error F_CPU is not set
#endif

#include "common.hpp"

constexpr float tics_per_mhz=F_CPU/1e6;

class Delay
{
    static void tick(byte ticks) __attribute__((hot, always_inline))
    {
      for (byte i=0; i<ticks/2; i++)
        asm volatile
        ( 
          "rjmp 0f\n"
          "0:\n"
        );

      if ((ticks%2)==1)
        asm volatile ("nop");
    }

  public:
    template <word delay>
    static void ns()
    {
      const dword ticks=static_cast<dword>(delay) * tics_per_mhz/1000;
      tick (ticks);
    }
    
    template <word delay>
    static void us() 
    {
      const dword ticks=static_cast<dword>(delay) * tics_per_mhz;

      if ( ticks<=2)         //do 2 cykli
        tick(ticks & 0xff);
      else if (ticks>2 && ticks<=770)      //powyżej 2 cykli ale do 770 (256*3+2)
      {
        byte tmp;
        word loops=(ticks/3) & 0xffff;     //liczba pętli = rządany czas / koszt całej pętli
        asm volatile
        (
          "ldi %0,%1\n"  //minimalny koszt całości to 3 cykle
          "0:\n"
          "dec %0\n"
          "brne 0b\n"
          :"=d"(tmp)
          :"X"(loops<256?loops:0)   //256 zastępujemy 0
        );

        //dołóż to co jeszcze zostało (reszta)
        const byte r=ticks-(loops)*3;
        tick(r);
      }
      else if (ticks>770)
      {
        //static_assert( ticks == 0 || (ticks-1)/4<=65536, "Too big loop for delay_us");
        byte t1, t2;
        const word loops=((ticks-1)/4) & 0xffff;     //liczba pętli = rządany czas / koszt całej pętli (jeden rozkaz jest nadmiarowy, trzeba go odjąc)
        const byte l=loops & 0xff;
        const byte h=loops >> 8;
        asm volatile
        (
          "ldi %0,%2\n"  //minimalny koszt całości to 4 cykli (pierwszy przebieg to 5 (uwzględniając brakujący cykl z ostatniego przebiegu))
          "ldi %1,%3\n"
          "0:\n"
          "subi %0,1\n"
          "sbci %1,0\n"
          "brne 0b\n"
          :"=d"(t1),"=d"(t2)
          :"X"(l),"X"(h)
        );

        //dołóż to co jeszcze zostało (reszta)
        const byte r=((ticks-1)-(loops)*4) & 0xff;
        tick(r);
      }
    }

    
    static void ms(word delay) __attribute__((cold)); //without "cold" delay's body may not become a simple loop, which is not what we want
};


void Delay::ms(word delay)
{
    for (word i=0; i<delay; i++)
        us<1000>();
}

#endif
