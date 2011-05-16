#ifndef KEYPAD12_HPP
#define KEYPAD12_HPP

#include "../delay.hpp"
#include "../ports/ports.hpp"

template<port_ptr rows_port, byte rows_shf, port_ptr cols_port, byte cols_shf, void (*pressed)(byte)>
class Keypad12
{
   byte state[2];

   byte decode(byte d)
   {
     byte ret=0xff;
     switch(d)
     {
       case 1: ret=1; break;
       case 2: ret=2; break;
       case 4: ret=3; break;
       case 5: ret=4; break;
       case 6: ret=5; break;
       case 8: ret=6; break;
       case 9: ret=7; break;
       case 10: ret=8; break;
       case 12: ret=9; break;
       case 13: ret=32; break; //*
       case 14: ret=0; break;
       case 16: ret=64; break; //#
     }
     return ret;
   }

  public:
    Keypad12()
    {
      const TIOPort<rows_port> rowsPort;
      const TIOPort<cols_port> colsPort;
      const byte rowsMask=0xf << rows_shf, colsMask=0x7<<cols_shf;

      rowsPort.dir|=rowsMask;
      rowsPort|=rowsMask;

      colsPort.dir&=~colsMask;
      colsPort|=colsMask;       //włącz podciąganie
    }

    void interrupt()
    {
      const TIOPort<rows_port> rowsPort;
      const TIOPort<cols_port> colsPort;
      const byte rowsMask=0xf << rows_shf;
      static byte current;
      static byte row, rowShf=1;

      //wystaw 0 na bieżący wiersz
      rowsPort&=~(rowShf<<rows_shf);

      Delay::us<5>();                                //odczekaj chwilę
      byte cols=(~(colsPort() >> cols_shf)) & 0x7;   //odczytaj wartość kolumn (zamien zera w jedynki)

      //zapamietaj wynik (na jeden wiersz przypadają 3 wartości:
      //0, 1, 2 lub 4, żeby wynik był rozróznialny, trzeba wiersz wymnożyć przez 4)
      if (cols>0)
        current=row*4+cols;

      row++;
      rowShf<<=1;
      if ( (rowShf & rowsMask)==0)
      {
        rowShf=1;
        row=0;
        if (state[0]==state[1] && state[1]!=0 && current==0)  //reaguj po puszczeniu przycisku
          pressed(decode(state[0]));

        state[0]=state[1];
        state[1]=current;
        current=0;
      }
      rowsPort|=rowsMask;                         //wygaś wiersz
    }

    enum Keys
    {
      K0=0,
      K1=1,
      K2=2,
      K3=3,
      K4=4,
      K5=5,
      K6=6,
      K7=7,
      K8=8,
      K9=9,
      K_ast=32,
      K_hash=64
    };
};


#endif //KEYPAD12_HPP
