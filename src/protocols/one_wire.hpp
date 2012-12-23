
/***************************************
 * Implementation of One Wire protocol *
 * 21.12.2012 Michał Walenciak         *
 ***************************************/

#ifndef __AVR_CPP_LIB__ONE_WIRE__
#define __AVR_CPP_LIB__ONE_WIRE__

#include <ports/ports_defs.hpp>
#include <ports/ports.hpp>
#include <delay.hpp>

namespace OneWire
{
    template<Ports::PortT port, byte pin>
    class InterruptBased
    {        
        public:
            InterruptBased():m_state(Reset)
            {
                releaseBus();
            }
                    
            void reset()
            {                
                IOPort bus(port);
                
                //set output to zero
                bus[pin] = false;          //init pin output as 0
                pullBus();
                
                //wait 480-960µs            
                Delay::us<500>();
                
                //wait for response
                releaseBus();
                
                Delay::us<60>();           //maximum time for setting up        
                
                const bool state = readBusState();
                
                if (state == false)
                    m_state = Ready; 
                
                //wait for bus to be released
                while (readBusState() == false);
            }
            
            void write(byte value) const
            {
                for(byte i = 0; i < 8; i++)
                {
                    write<true>( (value & 1 ) > 0 );
                    value >>= 1;
                }
            }
            
            byte read() const
            {
                byte result = 0;
                for(byte i = 0; i < 8; i++)
                {
                    result >>= 1;
                    result |= read<true>() * 128;
                }
                   
                return result;
            }
            
            void read(byte count, byte *buffer) const
            {
                for(byte i = 0; i < count; i++)
                    buffer[i] = read();
            }            
            
        private:        
            enum State
            {
                Ready,
                Reset
            } m_state;
                        
            inline void pullBus() const __attribute__((always_inline))
            {
                IOPort bus(port);
                bus.dir[pin] = true;       //switch to output
            }
            
            inline void releaseBus() const __attribute__((always_inline))
            {
                IOPort bus(port);
                bus.dir[pin] = false;      //switch to input
            }
            
            template<bool addRecoveryTime>
            inline void write(bool value) const
            {
                pullBus();
                                    
                if (value)
                    Delay::us<5>();      // 1 < x < 15
                else
                    Delay::us<60>();
                
                releaseBus();            //release bus
                
                if (value)
                    Delay::us<55>();     //fill cycle (to 60µs)
                
                if (addRecoveryTime)
                    Delay::us<1>();
            }
            
            template<bool addRecoveryTime>
            inline bool read() const
            {
                pullBus();                
                Delay::us<5>();         // 1 < x < 15
                releaseBus();
                Delay::us<10>();        // wait for slaves to react
                
                const bool state = readBusState();
                
                Delay::us<45>();        // fill cycle (to 60us)
                
                if (addRecoveryTime)
                    Delay::us<1>();
                
                return state;
            }
            
            inline bool readBusState() const __attribute__((always_inline))
            {
                IOPort bus(port);
                return bus[pin];
            }
        
    };
}
#endif
