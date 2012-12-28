
#ifndef __AVR_CPP_LIB__DS18B20__
#define __AVR_CPP_LIB__DS18B20__

/***************************************
 * DS18B20 communiaction functionality *
 * 25.12.2012 Michał Walenciak         *
 ***************************************/

#include "ports/ports.hpp"
#include "protocols/one_wire.hpp"

template<Ports::PortT port, byte pin, bool parasiteMode = true>
class DS18B20
{
        const byte readRomCmd = 0x33;
        const byte convertCmd = 0x44;
        const byte skipRomCmd = 0xCC;
        const byte readScratchpadCmd = 0xBE;
    
    public:
        DS18B20(): m_oneWire(), m_data()
        {
            static_assert(sizeof(Data) == 8, "Size of Data != 8");            
        }
        
        struct __attribute__((packed)) Data
        {
            byte m_family;
            byte m_serial[6];
            byte m_crc;
        };
        
        /*
        //detect sensors
        const Data& detectSensors()
        {
            byte *buffer = reinterpret_cast<byte *>(&m_data);
            
            m_oneWire.write(readRomCmd);
            m_oneWire.read(sizeof(Data), buffer);
            return m_data;
        }
        */
        
        //returned value which consists of 12bit. (LSB is 2^-4. up to 2^6. rest of bits are "sign")
        int readRawTemp()
        {
            init();
            
            m_oneWire.write(convertCmd);
                       
            if (parasiteMode)
                m_oneWire.powerSlaves(750);           //TODO: configureable
            else
                while(m_oneWire.read() == false);
            
            init();
            
            struct __attribute__((packed)) Scratchpad
            {
                byte t_lsb;
                char t_msb;
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
            m_oneWire.read(2, buffer);
            m_oneWire.emptyRead(7);          //don't care about next 7 bytes
            
            return scratchpad.t_lsb + 256 * scratchpad.t_msb;
        }
        
    private:       
        OneWire::InterruptBased<port, pin> m_oneWire;        
        Data m_data;
        
        //init transaction - current implementation supports only one slave        
        void init()
        {
            m_oneWire.reset();
            m_oneWire.write(skipRomCmd);
        }
};

#endif
