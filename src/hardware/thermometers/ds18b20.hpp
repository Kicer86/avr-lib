
#ifndef __AVR_CPP_LIB__DS18B20__
#define __AVR_CPP_LIB__DS18B20__

#include "ports/ports.hpp"
#include "protocols/one_wire.hpp"

template<Ports::PortT port, byte pin>
class DS18B20
{
    public:
        DS18B20() 
        {
            static_assert(sizeof(Data), "Size of Data != 8");            
        }
        
        struct __attribute__((packed)) Data
        {
            byte m_family;
            byte m_serial[6];
            byte m_crc;
        };
        
        void init()
        {            
            byte *buffer = reinterpret_cast<byte *>(&m_data);
            
            m_oneWire.reset();
            m_oneWire.write(0x33);
            m_oneWire.read(sizeof(Data), buffer); 
        }
        
        const Data& getInfo() const
        {
            return m_data;
        }
        
    private:
        OneWire::InterruptBased<port, pin> m_oneWire;        
        Data m_data;
};

#endif
