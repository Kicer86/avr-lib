
#ifndef __AVR_CPP_LIB__DS18B20__
#define __AVR_CPP_LIB__DS18B20__

/***************************************
 * DS18B20 communiaction functionality *
 * 25.12.2012 Michał Walenciak         *
 ***************************************/

#include <util/crc16.h>

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
        static constexpr int readError = 0x6E;
        
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
        //returns 'readError' when read was invalid
        int readRawTemp()
        {
            init();
            
            m_oneWire.write(convertCmd);
                       
            if (parasiteMode)
                m_oneWire.powerSlaves(750);           //TODO: configureable
            else
                while(m_oneWire.read() == false);
            
            init();
            
            struct __attribute__((packed)) Data
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
            };
            
            union Scratchpad
            {
                Data data;                
                byte rawData[9];
            } scratchpad;
            
            static_assert(sizeof(Data) == 9, "invalid size of scratchpad structure");
                        
            m_oneWire.write(readScratchpadCmd);   
            m_oneWire.read(9, scratchpad.rawData);
            
            //verify crc
            byte crc = 0;
            for (byte i = 0; i < 64/8; i++)
                crc = _crc_ibutton_update(crc, scratchpad.rawData[i]);
            
            const int temp = scratchpad.data.t_lsb + 256 * scratchpad.data.t_msb;
            const int ret = crc == scratchpad.data.crc? temp : readError;
                            
            return ret;
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
