//klasa abstrakcyjna (baza) pod usart (sprzętowy i programowy)

#ifndef USARTBASE_HPP
#define USARTBASE_HPP


#include "../datatypes.h"
#include "../progmem.hpp"
#include "baudrate.hpp"

class UsartBase
{
  protected:
    virtual void sendByte(byte dta) const=0;
    virtual void clearFlushFlag()=0;

  public:
    enum DataSize
    {
      D5=0,
      D6=2,
      D7=4,
      D8=6
    };

    enum StopBits
    {
      S1=0,
      S2=1<<3
    };

    enum Parity
    {
      None=0,
      Even=0x20,
      Odd=0x30
    };

    virtual void configure(bool tx, bool rx, Baudrate::Baudrates br, Parity p, StopBits sb, DataSize ds) const=0 ;
    virtual void changeBaudRate(Baudrate::Baudrates) const=0;
    virtual bool hasData() const=0;
    virtual byte read() const=0;
    virtual void flush() const=0;             //make sure transmition is over
    virtual bool error(byte *err=0) const=0;  //check if error occured. err will be filled with status (frame error (4), data overrun(2) or parity error (1) )

    void write(byte data)
    {
      clearFlushFlag();
      sendByte(data);
    }

    void writeString(const char *str)
    {
      byte c;
      clearFlushFlag();
      while ( (c=*str++)!=0)
        sendByte(c);
    }

    template<typename T>
    void write_PString(T *str)
    {
      byte c;
      clearFlushFlag();
      while ( (c=PMem::readByte(str++))!=0)
        sendByte(c);
    }

    void writeData(const byte *buff, byte size)
    {
      for (byte i=0; i<size; ++i)
        sendByte(buff[i]);
    }

    template<typename T>
    void write_PData(T *buff, byte size)
    {
      for (byte i=0; i<size; ++i)
        sendByte(PMem::readByte(buff+i));
    }
};

#endif
