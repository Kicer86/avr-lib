
#ifndef __AVR_CPP_LIB__DS18B20__
#define __AVR_CPP_LIB__DS18B20__

/***************************************
 * DS18B20 communiaction functionality *
 * 25.12.2012 Michał Walenciak         *
 ***************************************/

#include "ports/ports.hpp"
#include "protocols/one_wire.hpp"

template<Ports::PortT port, byte pin>
class DS18B20
{
        const byte readRomCmd = 0x33;
        const byte convertCmd = 0x44;
        const byte readScratchpadCmd = 0xBE;
    
    public:
        DS18B20() 
        {
            static_assert(sizeof(Data) == 8, "Size of Data != 8");            
        }
        
        struct __attribute__((packed)) Data
        {
            byte m_family;
            byte m_serial[6];
            byte m_crc;
        };
        
        //detect sensor - current implementation supports only one slave        
        void init()
        {            
            byte *buffer = reinterpret_cast<byte *>(&m_data);
            
            m_oneWire.reset();
            m_oneWire.write(readRomCmd);
            m_oneWire.read(sizeof(Data), buffer); 
        }
        
        //get info about detected sensor
        const Data& getInfo() const
        {
            return m_data;
        }
        
        word readRawTemp() const
        {
            m_oneWire.write(convertCmd);
            
            //TODO: defferent reactions in parasite/normal powering modes
            
            //normal powering:            
            while(m_oneWire.read() == false);
            //
            
            struct __attribute__((packed)) Scratchpad
            {
                byte t_lsb;
                byte t_msb;
                byte th;
                byte tl;
                byte config;
                byte res1;
                byte res2;
                byte res3;
                byte crc;
            } scratchpad;
            
            static_assert(sizeof(Scratchpad) == 9, "invalid size of scratchpad structure");
            byte *buffer = reinterpret_cast<byte *>(&scratchpad);
            
            m_oneWire.write(readScratchpadCmd);   
            m_oneWire.read(sizeof(Scratchpad), buffer);
            
            return scratchpad.t_lsb + 256 * scratchpad.t_msb;
        }
        
    private:
        OneWire::InterruptBased<port, pin> m_oneWire;        
        Data m_data;
};

#endif
