
#ifndef LIB_H
#define LIB_H

#if defined __AVR_ATmega8__

#define FLASH_SIZE  8192
#define EEPROM_SIZE 512
#define SRAM_SIZE   1024

#elif __AVR_ATtiny2313__

#define FLASH_SIZE  2048
#define EEPROM_SIZE 128
#define SRAM_SIZE   128

#elif __AVR_ATtiny4313__

#define FLASH_SIZE  4096
#define EEPROM_SIZE 256
#define SRAM_SIZE   256

#else

#error Unknown avr

#endif

void main() __attribute__((noreturn, OS_main));

#endif //LIB_H
