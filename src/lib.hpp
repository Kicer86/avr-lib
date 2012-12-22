
#ifndef LIB_HPP
#define LIB_HPP

//gcc 4.5.0 wymagany

int main() __attribute__((noreturn));
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


#if defined __AVR_ATmega8__

#define FLASH_SIZE  8192
#define EEPROM_SIZE 512
#define SRAM_SIZE   1024

#endif

#endif //LIB_HPP
