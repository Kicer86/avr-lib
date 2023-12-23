
#ifndef USART_HPP
#define USART_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../datatypes.h"
#include "../checks/require_fcpu.h"
#include "usartbase.hpp"


class Usart: public UsartBase<Usart>
{
  public:
    template<bool tx, bool rx, dword baud, Parity p, StopBits sb, DataSize ds>
    void configure() const
    {
      byte b = 0;

      changeBaudRate<baud>();

      if (tx)
        b |= /*(1<<UDRIE) |*/ // włącz przerwanie od pustego bufora
          (1<<TXEN);   // włącz nadajnik

      if (rx)
        b |= /*(1<<RXCIE) |*/  //włącz przerwanie odbiornika
          (1<<RXEN);    //włącz odbiornik

      UCSRB=b;

      //TODO: reenable UCSRC=(1<<URSEL) | //w atmega8 trzeba ustawić ten bit gdy się pisze do UCSRC
      UCSRC = p          | //parzystość
              sb         | //bity stopu
              ds;          //bity danych
    }

    template<dword baud>
    void changeBaudRate() const
    {
      constexpr int mul = 16;
      constexpr double exact_ubrr = static_cast<double>(F_CPU) / (mul * baud) - 1;
      constexpr long rounded_ubrr = round<long>(exact_ubrr);
      static_assert(rounded_ubrr < 65536);

      constexpr long  real_baud = F_CPU / (mul * (rounded_ubrr + 1));
      constexpr long diff = abs(real_baud - baud);
      constexpr auto error = diff/static_cast<double>(baud) * 100;

      // based on https://stackoverflow.com/questions/65092264/how-to-set-atmega64a-au-usart-speed-to-115200
      static_assert(error <= 4.5, "Too big frequency error for USART. Adjust F_CPU and/or baud rate");

      // setup usart module
      UBRRH = (baud >> 8) & 0xff;
      UBRRL = baud & 0xff;
    }

    void enableRXInt(bool enable=true) const
    {
      if (enable)
        UCSRB |= (1<<RXCIE);
      else
        UCSRB &= ~(1<<RXCIE);
    }

    void enableTXInt(bool enable=true) const
    {
      if (enable)
        UCSRB |= (1<<UDRIE);
      else
        UCSRB &= ~(1<<UDRIE);
    }

    bool hasData() const
    {
      return UCSRA & (1<<RXC);
    }

    byte read() const
    {
      return UDR;
    }

    void flush() const
    {
      while ((UCSRA & (1<<TXC)) == 0);  //wait for TX to be ready
    }

    bool error(byte* err = nullptr) const
    {
      byte d = UCSRA;
      d >>= 2;
      d &= 7;
      if (err)
        *err = d;

      return d != 0;
    }

  private:
    void sendByte(byte dta) const
    {
      while ((UCSRA & (1<<UDRE)) == 0); //wait for TX buffer empty
      UDR=dta;
    }

    void clearFlushFlag()
    {
      UCSRA |= (1<<TXC);                 //clear TXC flag
    }
};

// [1] - meanual clearing TXC flag is necessary before each TX transmission to be sure,
// that flush command will not be fooled by one ofthre previous transmissions (TXC flag
// is cleared automnatically only in TX interrupt).

#endif
