
#ifndef PORTS_DEFS_HPP
#define PORTS_DEFS_HPP

#include <avr/io.h>

//#include "ports.hpp"
#include "../common.hpp"

namespace Ports
{
    typedef word PortT;
    typedef volatile byte* PortAddr;
    
    constexpr word convert(PortAddr port)
    {
        return reinterpret_cast<word>(port);
    }
        
#ifdef PORTA
    constexpr PortT PortA = convert(&PORTA);
#endif

#ifdef PORTB
    constexpr PortT PortB = convert(&PORTB);
#endif

#ifdef PORTC
    constexpr PortT PortC = convert(&PORTC);
#endif

#ifdef PORTD
    constexpr PortT PortD = convert(&PORTD);
#endif

#ifdef PORTE
    constexpr PortT PortE = convert(&PORTE);
#endif
}

#endif
