
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
        constexpr Ports::PortT _gimsk = Ports::convert(&GIMSK);
        constexpr Ports::PortT _pcmsk = Ports::convert(&PCMSK);
    }
    
    constexpr Port gimsk(_gimsk);
    constexpr Port pcmsk(_pcmsk);
}

class Interrupts
{
        const byte flags;
        
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


class InterruptsBlock
{
        byte flags;
        
    public:
        constexpr InterruptsBlock(): flags(0)
        {
        }
        
        void block()
        {
            flags = SREG;
            cli();
        };

        void free()
        {
            SREG = flags;
        }
};

#define DISABLE_INTERRUPTS_IN_BLOCK Interrupts __interrupts_saver

#endif
