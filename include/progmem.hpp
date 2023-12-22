
#ifndef PROGMEM_HPP
#define PROGMEM_HPP

#include <avr/pgmspace.h>
#include <avr/io.h>

#include "datatypes.h"
#include "common.hpp"

//typedef would be better but unfortunatelly attribute __progmem__ doesn't work for c++
// and section attribute cannot be used with typedef 

#define p_byte const byte FLASH(const_progmem_data)
#define p_char const char FLASH(const_progmem_data)
// typedef const byte p_byte; //todo: FLASH()
// typedef const char p_char; //todo: FLASH()

// typedef uint_farptr_t /*FLASH(const_data)*/ p_pointer_far;
// typedef p_char /*FLASH(const_data)*/* p_pointer;

namespace PMem
{
  template<typename T>
  byte readByte(T *addr)
  {
    return pgm_read_byte(reinterpret_cast<const byte*>(addr));
  }

#ifdef RAMPZ
  byte readByte(p_pointer_far addr)
  {
    return pgm_read_byte_far(addr);
  }
#endif
}

#endif //PROGMEM_HPP
