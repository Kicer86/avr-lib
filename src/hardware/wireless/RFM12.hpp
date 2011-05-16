
#ifndef RFM12_HPP
#define RFM12_HPP

#include "../../ports/ports.hpp"
#include "../../devices/spi.hpp"
#include "../../devices/baudrate.hpp"

//urządzenie nie wymaga obsługi przerwaniami, co aczkolwiek jest możliwe

// template<port_ptr intPort, byte intPin>
class Rfm12
{
    Spi *spi;
    const byte c1,c2;

    void wait4ready()
    {
//       TIOPort<intPort> readyPort;
//       while ( (readyPort() & (1<<intPin))!=0 );  //dopóki stan wysoki, czekaj
      while (ready()==false) ;
    }

    void send(byte data)
    {
      wait4ready();
      spi->write16(0xB800 | data);
    }

    byte get()
    {
      wait4ready();

      spi->write16(0x0000);
      return spi->write16(0xB000);
    }

    void setFrequencyOffset(word freq) const
    {
      spi->write16(0xA000 | (freq & 0xfff) );
    }

    void setBaudRate(byte baud) const
    {
      spi->write16(0xc600 | baud);
    }

    byte strlen(const char * str) const
    {
      byte len=0;
      while (*str++!='\0')
        len++;

      return len;
    }

  public:
    enum Mode
    {
      TX,
      RX
    };

    enum Band
    {
      B433=1,
      B868=2,
      B915=3
    };

    Rfm12(Spi *s, const Band &band):
        spi(s),
        c1(band==B433? 1: band==B868? 2: 3),
        c2(band==B915? 30: 43)
    {
//       TIOPort<intPort> readyPort;
//       readyPort.dir&=~(1<<intPin);
//       readyPort=(1<<intPin);
      spi->write16(0x0000); //band |int REG+FIFO| capacitor(bylo 7)
      spi->write16(0x8000 | (band<<4) | 0xC0 | 0x08);
    };

    void getC1C2(byte *_c1, byte *_c2) const
    {
      *_c1=c1;
      *_c2=c2;
    }

    void setMode(Mode mode, byte baud, word freq) const
    {
      if (mode==TX)
      {
        spi->write16(0x8209);          //takie różne pierdółki

        setFrequencyOffset(freq);
        setBaudRate(baud);

        //parametry transmisji
        //                   dewiacja (90kHz)
        spi->write16(0x9800 | 0x50    | 0x00);
      }
      else //RX
      {
        spi->write16(0x82D9);          //włącz odbiornik

        setFrequencyOffset(freq);
        setBaudRate(baud);

        //parametry odbiornika                     (134kHz)
        //                   VDI, Medium response     BW    LNA    RSSI
        spi->write16(0x9000 |0x0500                | 0xA0 | 0x00 | 0x00 );

        //konfiguracje filtrów
        //                      CR    DQD
        spi->write16(0xC200 | 0x60 | 0x0C);

        //AFC (?)
        //
        spi->write16(0xC400 | 0xB0  | 0x07);

        //kontrola fifo oraz resetu
        //                   FIFO len   Reset i synchro
        spi->write16(0xCA00 | 0x80    | 0x01);

        //synchro pattern
        //                    byte0
        spi->write16(0xCE00 | 0xD4);
      }
    }

    //"Wygodna" wersja funkcji setMode.
    //Przy braku odpowiednich optymalizacji i rozbudowanym kodzie
    //drastycznie zwiększa rozmiar binarki
    void setFMode(Mode mode, Baudrate::Baudrates baud, float freq) __attribute__((hot))
    {
      word f=4000.0*(freq/(10*c1)-c2);
      const byte cs=baud<2694?1:0;
      word r=(10000000.0/29/(1+cs*7)/baud-1)+.5; //.5 do zaokrąglenia
      setMode(mode, r | (cs*128), f);
    }

    virtual void openRX()
    {
      spi->write16(0xCA83);  //włącz FIFO
    }

    virtual void openTX()
    {
      spi->write16(0);       //odczyt statusu
      spi->write16(0x8239);  //włącz transmiter

      //nagłówek
      send(0xaa);
      send(0xaa);
      send(0xaa);
      send(0xaa);
      send(0x2d);
      send(0xd4);
    }

    virtual void closeTX()
    {
      //dummy bytes
      send(0xaa);
      send(0xaa);
      spi->write16(0x8201);   //wyłącz nadajnik
    }

    virtual void closeRX()
    {
      //spi->write16(0x8201);   //wyłącz odbiornik
      spi->write16(0xCA81);   //wyłącz FIFO
    }

    bool ready()  //w trybie odbiornika - dane przyszyły, w trybie nadajnika - gotowy do wysyłki
    {
      word status=spi->write16(0x0000);   //odczytaj status
      return status & (1 << 15);
    }

    void write(const char *data)
    {
      write(reinterpret_cast<const byte*>(data), strlen(data));
    }

    void write(const byte *data, byte size)
    {
      for (int i=0; i<size; i++)
        write(data[i]);
    }

    void write(byte data)
    {
      send(data);
    }

    byte read()
    {
      return get();
    }
};

#endif
