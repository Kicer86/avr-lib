
#ifndef USART_HPP
#define USART_HPP

#include <utility>
#include <cmath>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../cputype.hpp"
#include "../datatypes.h"
#include "../checks/require_fcpu.h"
#include "usartbase.hpp"


namespace
{
  template<bool doubleSpeed, dword baud>
  constexpr std::pair<word, double> calculateUBRRwithError()
  {
    constexpr int mul = doubleSpeed? 8 : 16;
    constexpr double exact_ubrr = static_cast<double>(F_CPU) / (mul * baud) - 1;
    constexpr long rounded_ubrr = std::lround(exact_ubrr);
    static_assert(rounded_ubrr < 65536);

    constexpr long real_baud = F_CPU / (mul * (rounded_ubrr + 1));
    constexpr long diff = labs(real_baud - baud);
    constexpr auto error = diff/static_cast<double>(baud) * 100;

    return {rounded_ubrr, error};
  }

  template<dword baud>
  constexpr std::pair<word, bool> calculateUBRR()
  {
    constexpr double maxError = 4.5;

    // try to calculate UBRR value for single speed
    constexpr auto ubrr16 = calculateUBRRwithError<false, baud>();
    if constexpr (ubrr16.second < maxError)
      return {ubrr16.first, false};
    else
    {
      // try to calculate UBRR value for double speed
      constexpr auto ubrr8 = calculateUBRRwithError<true, baud>();
      static_assert(ubrr8.second < maxError, "Too big frequency error for USART. Adjust F_CPU and/or baud rate");

      return {ubrr8.first, true};
    }
  }
}


class Usart: public UsartBase<Usart>
{
  public:
    friend UsartBase<Usart>;

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

      byte c = static_cast<byte>(sb) | static_cast<byte>(ds);

#ifdef URSEL
      if constexpr (Cpu::type() == Cpu::Type::AtMega8)
        c |= 1 << URSEL;            //writing to UCSRC on AtMega8 requires this (see reference manual for details)
#endif

      UCSRC = c;
    }

    template<dword baud>
    void changeBaudRate() const
    {
      constexpr auto ubrr = calculateUBRR<baud>();

      // setup usart module
      UBRRH = (ubrr.first >> 8) & 0xff;
      UBRRL = ubrr.first & 0xff;

      // setup double speed
      if constexpr (ubrr.second)
        UCSRA |= U2X;
      else
        UCSRA &= ~U2X;
    }

    void enableRXInt(bool enable = true) const
    {
      if (enable)
        UCSRB |= (1<<RXCIE);
      else
        UCSRB &= ~(1<<RXCIE);
    }

    void enableTXInt(bool enable = true) const
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
