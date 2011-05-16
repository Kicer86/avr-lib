
#ifndef LIB_HPP
#define LIB_HPP

int main() __attribute__((noreturn));
extern "C" void atexit( void ) {}
extern "C" void __cxa_pure_virtual() {}
void operator delete(void*) {}


#if defined __AVR_ATmega8__

#define FLASH_SIZE  8192
#define EEPROM_SIZE 512
#define SRAM_SIZE   1024

#endif

#endif //LIB_HPP
