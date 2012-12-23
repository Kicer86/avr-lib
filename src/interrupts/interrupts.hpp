
#ifndef __AVR_CPP_LIB__INTERRUPTS__
#define __AVR_CPP_LIB__INTERRUPTS__

#include <ports/ports.hpp>

namespace Register
{
    namespace
    {
        constexpr Ports::PortT _gimsk = reinterpret_cast<Ports::PortT>(&GIMSK);
        constexpr Ports::PortT _pcmsk = reinterpret_cast<Ports::PortT>(&PCMSK);
    }
    
    constexpr Port gimsk(_gimsk);
    constexpr Port pcmsk(_pcmsk);
}

#endif
