
#ifndef USART_HPP
#define USART_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../datatypes.h"
#include "baudrate.hpp"
#include "usartbase.hpp"

class Usart: public UsartBase<Usart>
{
  protected:
    void sendByte(byte dta) const
    {
      while ( (UCSRA & (1<<UDRE))==0); //wait for TX buffer empty
      UDR=dta;
    }

    void clearFlushFlag()
    {
      UCSRA|=(1<<TXC);                 //clear TXC flag
    }

  public:
    void configure(bool tx, bool rx, Baudrate::Baudrates br, Parity p, StopBits sb, DataSize ds) const
    {
      byte b=0;

      changeBaudRate(br);

      if (tx)
        b|=/*(1<<UDRIE) |*/ // włącz przerwanie od pustego bufora
          (1<<TXEN);   // włącz nadajnik

      if (rx)
        b|=/*(1<<RXCIE) |*/  //włącz przerwanie odbiornika
          (1<<RXEN);    //włącz odbiornik

      UCSRB=b;

      //TODO: reenable UCSRC=(1<<URSEL) | //w atmega8 trzeba ustawić ten bit gdy się pisze do UCSRC
      UCSRC=p          | //parzystość
            sb         | //bity stopu
            ds;          //bity danych
    }

    void changeBaudRate(Baudrate::Baudrates br) const
    {
      word ubrr=F_CPU/(16L*br)-1;
      UBRRH=ubrr>>8;
      UBRRL=ubrr;
    }

    void enableRXInt(bool enable=true) const
    {
      if (enable)
        UCSRB|=(1<<RXCIE);
      else
        UCSRB&=~(1<<RXCIE);
    }

    void enableTXInt(bool enable=true) const
    {
      if (enable)
        UCSRB|=(1<<UDRIE);
      else
        UCSRB&=~(1<<UDRIE);
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
      while ( (UCSRA & (1<<TXC))==0);  //wait for TX to be ready
    }

    bool error(byte* err = 0) const
    {
      byte d=UCSRA;
      d>>=2;
      d&=7;
      if (err)
        *err=d;

      return d!=0;
    }
};

// [1] - meanual clearing TXC flag is necessary before each TX transmission to be sure,
// that flush command will not be fooled by one ofthre previous transmissions (TXC flag
// is cleared automnatically only in TX interrupt).

#endif
