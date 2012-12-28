
#ifndef PORTS_DEFS_HPP
#define PORTS_DEFS_HPP

#ifndef __TESTS
    #include <avr/io.h>
#endif

#include <datatypes.h>

namespace Ports
{
    typedef ptr_t PortT;
    typedef volatile byte* PortAddr;
    
    constexpr ptr_t convert(PortAddr port)
    {
        return reinterpret_cast<ptr_t>(port);
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
