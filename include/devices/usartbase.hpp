//klasa abstrakcyjna (baza) pod usart (sprzętowy i programowy)

#ifndef USARTBASE_HPP
#define USARTBASE_HPP


#include "../datatypes.h"
#include "../progmem.hpp"


template<typename Impl>
class UsartBase
{
  public:
    enum DataSize
    {
      D5 = 0,
      D6 = 2,
      D7 = 4,
      D8 = 6
    };

    enum StopBits
    {
      S1 = 0,
      S2 = 1<<3
    };

    enum Parity
    {
      None = 0,
      Even = 0x20,
      Odd  = 0x30
    };

    template<bool tx, bool rx, dword baud, Parity p, StopBits sb, DataSize ds>
    void configure() const
    {
      static_cast<Impl*>(this)->template configure<tx, rx, baud, p, sb, ds>();
    }

    template<dword br>
    void changeBaudRate() const
    {
      static_cast<Impl*>(this)->template changeBaudRate<br>();
    }

    bool hasData() const
    {
      return static_cast<Impl*>(this)->hasData();
    }

    byte read() const
    {
      return static_cast<Impl*>(this)->read();
    }

    void flush() const                //make sure transmition is over
    {
      static_cast<Impl*>(this)->flush();
    }

    bool error(byte* err = nullptr) const     //check if error occured. err will be filled with status (frame error (4), data overrun(2) or parity error (1) )
    {
      return static_cast<Impl*>(this)->error(err);
    }

    void write(byte data)
    {
      clearFlushFlag();
      sendByte(data);
    }

    void writeString(const char *str)
    {
      char c;
      clearFlushFlag();
      while ((c = *str++) != 0)
        sendByte(c);
    }

    template<typename T>
    void write_PString(T *str)
    {
      byte c;
      clearFlushFlag();
      while ((c = PMem::readByte(str++)) != 0)
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

  private:
    void sendByte(byte dta) const
    {
      static_cast<Impl*>(this)->sendByte(dta);
    }

    void clearFlushFlag()
    {
      static_cast<Impl*>(this)->clearFlushFlag();
    }
};

#endif
