
#ifndef __AVR_CPP_LIB__CPUTYPE__
#define __AVR_CPP_LIB__CPUTYPE__

namespace CpuType
{
    
constexpr enum CpuType
{
    AtTiny2313,
    AtMega8
} cpuType = 

#if defined __AVR_ATtiny2313__
    AtTiny2313
#elif defined __AVR_ATmega8__
    AtMega8
#endif
;

}

#endif
