
#ifndef __AVR_CPP_LIB__INTERRUPTS__
#define __AVR_CPP_LIB__INTERRUPTS__

#ifndef __TESTS
    #include <avr/interrupt.h>
#endif   

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

class Interrupts
{
        byte flags;
        
    public:
        Interrupts(): flags(SREG)
        {
            cli();
        };

        ~Interrupts()
        {
            SREG = flags;
        }
};

#define DISABLE_INTERRUPTS_IN_BLOCK Interrupts __interrupts_saver

#endif
