
/***************************************
 * Implementation of One Wire protocol *
 * 21.12.2012 Michał Walenciak         *
 ***************************************/

#ifndef __AVR_CPP_LIB__ONE_WIRE__
#define __AVR_CPP_LIB__ONE_WIRE__

#include <ports/ports_defs.hpp>


//interface for OneWire implementations
class IOneWire
{
    public:
        virtual ~IOneWire() {}
        virtual void init() = 0;
};


//factory for OneWire implementations
struct OneWireFactory
{
    enum class Type
    {
        InterruptBased
    };
    
    static IOneWire *getOneWire(OneWireFactory::Type type, const Ports::PortT &, byte pin);
};

#endif
