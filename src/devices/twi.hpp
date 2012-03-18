
#ifndef TWI_HPP
#define TWI_HPP

#include <avr/io.h>

#include "datatypes.h"
#include "ports/ports_defs.hpp"

class Twi
{
    public:
        Twi() 
        {
            //enable TWI
            
            TWCR = 0x80;       //clear interrupt flag, and disable TWI
        }
        
        ~Twi() {}
};

#endif
