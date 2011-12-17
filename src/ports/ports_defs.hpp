
#ifndef PORTS_DEFS_HPP
#define PORTS_DEFS_HPP

#include <avr/io.h>

//#include "ports.hpp"
#include "../common.hpp"

namespace Ports
{
    typedef volatile byte * PortT;

#ifdef PORTA
    constexpr PortT PortA = &PORTA;
#endif    
    
#ifdef PORTB
    constexpr PortT PortB = &PORTB;
#endif
    
#ifdef PORTC
    constexpr PortT PortC = &PORTC;
#endif    
    
#ifdef PORTD
    constexpr PortT PortD = &PORTD;
#endif
    
#ifdef PORTE
    constexpr PortT PortE = &PORTE;
#endif     
}

#endif
