
#include <interrupts/pcint.hpp>

#include <interrupts/interrupts.hpp>
#include <cputype.hpp>
#include <ports/ports_defs.hpp>
#include <ports/ports.hpp>

void PCInt::setup(const Ports::PortT& port, byte pin, bool set)
{
    switch(CpuType::cpuType)
    {
        case CpuType::AtTiny2313:
        {           
            if (port == Ports::PortB)
                Register::pcmsk[pin] = set;
                
            break;
        }

        case CpuType::AtMega8:
            break;
    }
}


void PCInt::disable()
{
    Register::gimsk[5] = false;
}

void PCInt::enable()
{
    Register::gimsk[5] = true;
}

