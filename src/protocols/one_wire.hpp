
/***************************************
 * Implementation of One Wire protocol *
 * 21.12.2012 Michał Walenciak         *
 ***************************************/

#ifndef __AVR_CPP_LIB__ONE_WIRE__
#define __AVR_CPP_LIB__ONE_WIRE__

#include <ports/ports_defs.hpp>
#include <ports/ports.hpp>
#include <interrupts/pcint.hpp>
#include <delay.hpp>

namespace OneWire
{
    template<Ports::PortT port, byte pin>
    class InterruptBased
    {        
        public:
            InterruptBased():m_state(Reset)
            {
                switchToInput();
                
                PCInt::setup(port, pin);
            }
                    
            void reset()
            {
                PCInt::disable();
                
                IOPort bus(port);
                
                //set output to zero
                bus[pin] = false;          //set output to zero
                switchToOutput();
                
                //wait 480-960µs            
                Delay::us<500>();
                
                //wait for response
                switchToInput();
                
                PCInt::enable();
            }
            
            void interrupt()
            {
                switch(m_state)
                {
                    case Reset:
                        m_state = Ready;
                        break;
                }
            }
                    
        private:        
            enum State
            {
                Ready,
                Reset
            } m_state;
            
            inline void switchToOutput()
            {
                IOPort bus(port);
                bus.dir[pin] = true;       //switch to output
            }
            
            inline void switchToInput()
            {
                IOPort bus(port);
                bus.dir[pin] = false;      //switch to input
            }
            
            
            inline void write(bool value)
            {
                IOPort bus(port);
                bus.dir[pin] = false;      //switch to input
            }
        
    };
}
#endif
