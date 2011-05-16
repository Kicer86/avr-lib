#include "ports.hpp"

#ifndef PORTS_DEFS_HPP
#define PORTS_DEFS_HPP

#include <avr/io.h>

namespace Ports
{
  enum PortsList
  {
    
#if defined _AVR_IOMXX4_H_ || defined _AVR_IOMX8_H_       //procesory serii atmegaxx4 i atmegaxx8
#if defined _AVR_IOMXX4_H_
    PortA=0x02,                                           //port A tylko dla xx4 (pochodne mega16)
#endif
    PortB=0x05,
    PortC=0x08,
    PortD=0x0B
#endif

#if defined __AVR_ATmega8__ || defined __AVR_ATmega16__    //atmega8 i atmega16
#if defined __AVR_ATmega16__
    PortA=0x1B,                                            //port A tylko dla atmega16
#endif
    PortB=0x18,
    PortC=0x15,
    PortD=0x12
#endif
  };
}

#endif
