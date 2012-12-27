
#ifndef LIB_HPP
#define LIB_HPP

#include <lib.h>

#if defined __AVR_ATmega8__

#define FLASH_SIZE  8192
#define EEPROM_SIZE 512
#define SRAM_SIZE   1024

#elif __AVR_ATtin2313__

#define FLASH_SIZE  2048
#define EEPROM_SIZE 128
#define SRAM_SIZE   128

#endif

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
