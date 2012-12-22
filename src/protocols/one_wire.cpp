 
#include "one_wire.hpp"
#include "../ports/ports.hpp"
#include "../interrupts/pcint.hpp"

namespace
{
    struct InterruptBased: public IOneWire
    {
        
        InterruptBased() {}
        virtual ~InterruptBased() {}
        
        virtual void init()
        {
            
        }
        
        void setupInterrupt(const Ports::PortT &port, byte pin)
        {
            
        }
        
    };
}


IOneWire* OneWireFactory::getOneWire(OneWireFactory::Type type, const Ports::PortT &port, byte pin)
{
    IOneWire *result = 0;
    
    switch (type)
    {
        case Type::InterruptBased:
        {
            InterruptBased *intBased = new InterruptBased;
            intBased->setupInterrupt(port, pin);
            
            result = intBased;
            break;
        }
        
        default:
            break;
    }
    
    return result;
}
