
#ifndef SPI_HPP
#define SPI_HPP

#include <avr/io.h>

#include "datatypes.h"
#include "ports/ports_defs.hpp"

class Spi
{
    byte send(byte data)
    {
      /* Start transmission */
      SPDR = data;

      /* Wait for transmission complete ( */
      while ((SPSR & (1<<SPIF))==0);

      return SPDR;
    }

  public:
    enum Mode
    {
      Master,
      Slave
    };

    enum Pins
    {
#if defined _AVR_IOMX8_H_ || defined __AVR_ATmega8__
      SPI_Port=Ports::PortB, 
      Ss=2,
      Mosi=3,
      Miso=4,
      Sck=5
#endif
    };

    Spi()
    {}

    void setMode(Mode mode, byte div=64)
    {
//       static_assert(div==4 || div==8 || div==16 || div==32 ||
//                     div==64 || div==128, "divider must be 4, 8, 16, 32, 64 or 128");
      TIOPort<SPI_Port> spiPort;
      if (mode==Master)
      {
        spiPort.dir=(1<<Mosi) | (1<<Sck) | (1<<Ss);
        spiPort|=(1<<Ss);   //ss w stan wyoski - nieaktywne
      }
      else
        spiPort.dir=(1<<Miso);

      byte set=(1<<SPE);
      if (mode==Master)
        set|=(1<<MSTR);

      switch (div)
      {
        case 2:
          SPSR|= (1<<SPI2X);
          break;
        case 4:
          break;
        case 8:
          SPSR|= (1<<SPI2X);
          set|=(1<<SPR0);
          break;
        case 16:
          set|=1<<SPR0;
          break;
        case 32:
          SPSR|= (1<<SPI2X);
          set|= (1<<SPR1);
          break;

        default:
        case 64:
          set|=(1<<SPR1);
          break;
        case 128:
          set|=(1<<SPR1) | (1<<SPR0);
          break;
      }

      SPCR=set;
    }

    byte write(byte data)
    {
      TIOPort<SPI_Port> spiPort;
      spiPort&=~(1<<Ss);

      byte ret=send(data);

      spiPort|=(1<<Ss);
      return ret;
    }

    word write16(word data)
    {
      word ret;
      TIOPort<SPI_Port> spiPort;
      spiPort&=~(1<<Ss);

      ret=send(data >> 0x8) << 0x08;
      ret|=send(data & 0xff);

      spiPort|=(1<<Ss);
      return ret;
    }
};

#endif   //SPI_HPP
