
#ifndef __AVR_CPP_LIB__PCINT__
#define __AVR_CPP_LIB__PCINT__

#include <ports/ports.hpp>

class PCInt
{
    public:
        PCInt() = delete;
        
        static void setup(const Ports::PortT &, byte pin, bool set = true);
};

#endif

