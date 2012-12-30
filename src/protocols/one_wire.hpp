
/***************************************
 * Implementation of One Wire protocol *
 * 21.12.2012 Michał Walenciak         *
 ***************************************/

#ifndef __AVR_CPP_LIB__ONE_WIRE__
#define __AVR_CPP_LIB__ONE_WIRE__

#include <ports/ports_defs.hpp>
#include <ports/ports.hpp>
#include <delay.hpp>
#include <utils/macros.hpp>

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
                Delay::us<490>();
                
                //wait for response
                releaseBus();
                
                Delay::us<60>();           //maximum time for setting up        
                
                const bool state = readBusState();
                
                if (state == false)
                    m_state = Ready; 
                
                //wait up to 480µs
                Delay::us<480-60>();             
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
            
            void emptyRead() const               //read but don't care about value
            {
                for(byte i = 0; i < 8; i++)
                    read<true>();
            }
            
            void emptyRead(byte count) const    //read but don't care about value
            {
                for(byte i = 0; i < count; i++)
                    emptyRead();
            }
            
            template<int us>
            void powerSlaves() const 
            {
                strongPullUp();                
                Delay::us<us>();
                releaseStrongPullUp();                
            }
            
            void powerSlaves(word ms) const inline_attribute
            {
                strongPullUp();                
                Delay::ms(ms);
                releaseStrongPullUp();                
            }
            
        private:        
            enum State
            {
                Ready,
                Reset
            } m_state;
                        
            inline void pullBus() const inline_attribute     //send 0 to bus
            {
                IOPort bus(port);
                bus.dir[pin] = true;                         //switch to output
            }
            
            inline void strongPullUp() const inline_attribute    //set bus to 1 (power slaves)
            {
                IOPort bus(port);
                bus[pin] = true;                  //1 to output
                bus.dir[pin] = true;              //switch to output
            }
            
            inline void releaseBus() const inline_attribute      //release bus
            {
                IOPort bus(port);
                bus.dir[pin] = false;             //switch to input
            }
            
            inline void releaseStrongPullUp() const inline_attribute
            {
                releaseBus();
                
                IOPort bus(port);
                bus[pin] = true;                  //0 to output (no internal pullups)
            }
            
            template<bool addRecoveryTime>
            void write(bool value) const
            {
                pullBus();
                                    
                if (value)
                    Delay::us<5>();      // 1 < x < 15
                else
                    Delay::us<60>();
                
                releaseBus();            //release bus
                
                if (value)
                    Delay::us<55>();     //fill cycle (up to 60µs)
                
                if (addRecoveryTime)
                    Delay::us<1>();
            }
            
            template<bool addRecoveryTime>
            bool read() const
            {
                pullBus();                
                Delay::us<5>();         // 1 < x < 15
                releaseBus();
                Delay::us<10>();        // wait for slaves to react
                
                const bool state = readBusState();
                
                Delay::us<45>();        // fill cycle (up to 60us)
                
                if (addRecoveryTime)
                    Delay::us<1>();
                
                return state;
            }
            
            inline bool readBusState() const inline_attribute
            {
                IOPort bus(port);
                return bus[pin];
            }
        
    };
}
#endif
