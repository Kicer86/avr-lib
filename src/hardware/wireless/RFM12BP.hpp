
#ifndef RFM12BP_HPP
#define RFM12BP_HPP

#include "RFM12.hpp"

template<port_ptr rxPort, byte rxPin, port_ptr txPort, byte txPin>
class Rfm12BP: public Rfm12
{
  public:
    Rfm12BP(Spi *s, const Band &band):Rfm12(s, band)
    {
      //configure ports
      TIOPort<rxPort> rx_port;
      TIOPort<txPort> tx_port;

      rx_port.dir|=1<<rxPin;
      tx_port.dir|=1<<txPin;
      
      rx_port&=(1<<rxPin);
      tx_port&=(1<<txPin);
    }

    virtual void openRX()
    {
      TIOPort<rxPort> port;
      port|=(1<<rxPin);     //turn on RX
      Rfm12::openRX();
    }

    virtual void openTX()
    {
      TIOPort<txPort> port;
      port|=(1<<txPin);     //turn on TX
      Rfm12::openTX();
    }

    virtual void closeRX()
    {
      Rfm12::closeRX();

      //turn off RX
      TIOPort<rxPort> port;
      port&=~(1<<rxPin);
    }

    virtual void closeTX()
    {
      Rfm12::closeTX();

      //turn off TX
      TIOPort<txPort> port;
      port&=~(1<<txPin);
    }

};

#endif //RFM12BP_HPP
