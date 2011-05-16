
#ifndef _7SEG_HPP
#define _7SEG_HPP

#include "../../datatypes.h"
#include "../../ports/ports.hpp"


#define CREATE_TRANSLATION_TABLE(name,sA,sB,sC,sD,sE,sF,sG,sDP) \
  char name[11]={ \
                  (1<<sG) | (1<<sDP), \
                  (1<<sA) | (1<<sD) | (1<<sE) | (1<<sF) | (1<<sG) | (1<<sDP), \
                  (1<<sC) | (1<<sF) | (1<<sDP) ,\
                  (1<<sE) | (1<<sF) | (1<<sDP) ,\
                  (1<<sA) | (1<<sD) | (1<<sE) | (1<<sDP), \
                  (1<<sB) | (1<<sE) | (1<<sDP), \
                  (1<<sB) | (1<<sDP), \
                  (1<<sD) | (1<<sE) | (1<<sF) | (1<<sG) | (1<<sDP), \
                  (1<<sDP), \
                  (1<<sE) | (1<<sDP), \
                  (1<<sA) |(1<<sB) | (1<<sC) | (1<<sD) | (1<<sE) | (1<<sF) | (1<<sG) | (1<<sDP) \
                }


template <byte modules, port_ptr data_port, port_ptr seg_port, byte segShift, const char * translation>
class Disp7Seg
{
    byte pos;      //pozycja
    byte posShf;   //pozacja jako krążące 1
    volatile word onTime, offTime;  //miganie
    volatile byte values[modules];

  public:
    Disp7Seg():pos(0),posShf(1), onTime(1), offTime(1)
    {
      const TIOPort<data_port> dataPort;
      const TIOPort<seg_port> segPort;
      const byte modMask=(1<<modules)-1;

      dataPort.dir=0xff;              //wyjścia
      dataPort=0xff;                  //stan wysoki
      segPort.dir|=modMask << segShift;
      segPort|=modMask << segShift;   //stan wysoki

      clear();
    }

    void setValue(byte val[])
    {
      for (int i=0;i<modules;i++)
        values[i]=translation[val[i]];
    }

    void clear()
    {
      for (int i=0;i<modules;i++)
        values[i]=translation[0];
    }

    void setEmpty()
    {
      for (int i=0;i<modules;i++)
        values[i]=translation[10];   //pustka
    }

    //f - częstotliwość migania obrazka - dobrać experymentalnie
    //duty - wypełnienie - 128 = 100%
    void blink(word f, byte duty)
    {
      onTime=f*duty/128;
      offTime=f;
    }

    void interrupt()
    {
      const TIOPort<data_port> dataPort;
      const TIOPort<seg_port> segPort;
      const byte modMask=(1<<modules)-1;
      static word cycle;

      //wygaś kolumny (stan wysoki)
      segPort|=modMask << segShift;    //stan wysoki

      if (cycle++<onTime)              //świeć tylko jesli dany cykl zawiera się w czasie świecenia
      {
        //wystaw nowa wartość
        dataPort=values[pos];

        //załącz daną kolumnę
        segPort&=~(posShf<<segShift);
      }

      if (cycle>=offTime)              //przekroczne granice cyklu?
        cycle=0;                       //restart

      //przejście do następnej kolumny
      pos++;
      posShf<<=1;

      if ( (posShf & modMask)==0)
      {
        //od nowa
        posShf=1;
        pos=0;
      }
    }
};

#endif //_7SEG_HPP
