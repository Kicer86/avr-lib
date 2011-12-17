
#ifndef PORTS_DEFS_HPP
#define PORTS_DEFS_HPP

#include <avr/io.h>

//#include "ports.hpp"
#include "../common.hpp"

namespace Ports
{
    typedef word Port;

#ifdef PORTA
    constexpr Port PortA = reinterpret_cast<word>(&PORTA);
#endif    
    
#ifdef PORTB
    constexpr Port PortB = reinterpret_cast<word>(&PORTB);
#endif
    
#ifdef PORTC
    constexpr Port PortC = reinterpret_cast<word>(&PORTC);
#endif    
    
#ifdef PORTD
    constexpr Port PortD = reinterpret_cast<word>(&PORTD);
#endif
    
#ifdef PORTE
    constexpr Port PortE = reinterpret_cast<word>(&PORTE);
#endif     
}

#endif
