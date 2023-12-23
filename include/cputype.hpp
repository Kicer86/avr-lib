
#ifndef __AVR_CPP_LIB__CPUTYPE__
#define __AVR_CPP_LIB__CPUTYPE__

namespace Cpu
{

enum Type
{
    Unknown,
    AtTiny2313,
    AtTiny4313,
    AtMega8
};

constexpr Type type()
{
    return
#if defined __AVR_ATtiny2313__
    AtTiny2313
#elif defined __AVR_ATtiny4313__
    AtTiny4313
#elif defined __AVR_ATmega8__
    AtMega8
#else
    Unknown
#endif
;

}

}

#endif
