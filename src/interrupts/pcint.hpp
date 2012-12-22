
#ifndef __AVR_CPP_LIB__PCINT__
#define __AVR_CPP_LIB__PCINT__

#include "../ports/ports.hpp"
#include "../cputype.hpp"

class PCInt
{
    public:
        PCInt() = delete;
        
        void setup(const Ports::PortT &port, byte pin)
        {            
            switch(CpuType::cpuType)
            {
                case CpuType::AtTiny2313:
                    break;
                    
                    
                case CpuType::AtMega8:
                    break;
            }
        }
};

#endif

