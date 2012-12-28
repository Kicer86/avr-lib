
#ifndef LIB_HPP
#define LIB_HPP

#include <lib.h>

//gcc 4.5.0 wymagany

//optimisations:
//http://www.tty1.net/blog/2008-04-29-avr-gcc-optimisations_en.html

extern "C" void atexit( void ) {}
extern "C" void __cxa_pure_virtual() { for(;;) {} }
extern "C" void __cxa_atexit() {}
void operator delete(void*) {}

extern "C" unsigned char _end;

void* operator new(unsigned int size)
{
    static unsigned char *last = &_end;    
    void *ret = last;
    
    last += size;
    
    return ret;
}

#endif //LIB_HPP
