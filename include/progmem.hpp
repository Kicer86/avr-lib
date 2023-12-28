
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

  template<typename T>
  class Ptr
  {
    public:
      Ptr(T* addr): m_addr(addr) {}
      T operator[](int idx) const
      {
        return static_cast<T>(readByte(m_addr + idx));
      }

    private:
      T* m_addr;
  };
}

#define P_STR(str)                        \
  [](){                                   \
    static const char s[] PROGMEM = str;  \
    return PMem::Ptr<const char>(s);      \
  }()

#endif //PROGMEM_HPP
