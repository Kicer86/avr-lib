
#ifndef _7SEG_HPP
#define _7SEG_HPP

#include "../../datatypes.h"
#include "../../ports/ports.hpp"


//object of class Disp7Seg can work in two modes:
//1. with translation table (software 7seg decoding)
//2. without translation table (hardware 7seg decoding)
//When in first mode, translation cannot be equal to null ptr.
//On data port, catodes of LEDs have to be connected (LED is activated by low state on port's pin).
//On seg port, pnp transistor is expected (activated by low state).
//In second mode, on data port current serving led decoder is expected.
//Data port will output data as binary code.
//On seg port npn transistor is expected (connected to LED's cotode). (LED activated by high state on port's pin)


//makro do tworzenia kodów wyświetlaczy 7 segmentowych
//name - nazwa powstałej tabeli
//kolejne wartości -> numery pinów portu pod którymi znajdują się dane segmenty
#define CREATE_TRANSLATION_TABLE(name,sA,sB,sC,sD,sE,sF,sG,sDP) \
    char name[11]={ \
                    (1<<sG) | (1<<sDP), \
                    (1<<sA) | (1<<sD) | (1<<sE) | (1<<sF) | (1<<sG) | (1<<sDP), \
                    (1<<sC) | (1<<sF) | (1<<sDP) ,\
                    (1<<sE) | (1<<sF) | (1<<sDP) ,\
                    (1<<sA) | (1<<sD) | (1<<sE) | (1<<sDP), \
                    (1<<sB) | (1<<sE) | (1<<sDP), \
                    (1<<sB) | (1<<sDP), \
                    (1<<sD) | (1<<sE) | (1<<sF) | (1<<sG) | (1<<sDP), \
                    (1<<sDP), \
                    (1<<sE) | (1<<sDP), \
                    (1<<sA) |(1<<sB) | (1<<sC) | (1<<sD) | (1<<sE) | (1<<sF) | (1<<sG) | (1<<sDP) \
                  }


template <byte modules, IOPort &data_port, IOPort &seg_port, byte segShift, const char *translation, byte dataShift = 0>
class Disp7Seg
{
        byte pos;      //pozycja
        byte posShf;   //pozycja jako krążące 1
        volatile word onTime, offTime;  //miganie
        volatile byte values[modules];

    public:
        Disp7Seg(): pos(0), posShf(1), onTime(1), offTime(1)
        {
            static_assert(translation == nullptr || dataShift == 0, "If translation table is provided, data cannot be shifted");
            
            const byte modMask = (1 << modules) - 1;
            const byte dataMask = translation == nullptr? 0x0f : 0xff;

            data_port.dir = dataMask << dataShift; //wyjścia
            data_port = dataMask << dataShift;     //stan wysoki
            seg_port.dir |= modMask << segShift;   //wyjścia
            seg_port |= modMask << segShift;       //stan wysoki

            clear();
        }

        void setValue(byte val[])
        {
            for (int i = 0; i < modules;i++)
                if (translation != nullptr)
                    values[i] = translation[val[i]];
                else
                    values[i] = val[i];      //no translation
        }

        void clear()
        {
            for (int i = 0; i < modules;i++)
                values[i] = translation[0];
        }

        void setEmpty()
        {
            for (int i = 0; i < modules;i++)
                values[i] = translation[10]; //pustka
        }

        //f - częstotliwość migania obrazka - dobrać experymentalnie
        //duty - wypełnienie - 128 = 100%
        void blink(word f, byte duty)
        {
            onTime = f * duty / 128;
            offTime = f;
        }

        //this function has to be called frequently, to make display work
        void interrupt()
        {
            const byte modMask = (1 << modules) - 1;
            const byte dataMask = (translation == nullptr? 0x0f : 0xff) << dataShift;
            static word cycle = 0;

            //wygaś kolumny
            if (translation != nullptr)
                seg_port |= modMask << segShift;     //high
            else
                seg_port &= ~(modMask << segShift);  //low

            if (cycle++ < onTime)             //świeć tylko jesli dany cykl zawiera się w czasie świecenia
            {
                //wystaw nowa wartość
                if (translation != nullptr)   //translation? use whole port
                    data_port = values[pos];
                else
                {
                    byte port_val = data_port;   //read value send to port
                    port_val &= ~dataMask;       //clear value
                    port_val |= values[pos] << dataShift;  //set value
                    data_port = port_val;        //send it to port
                }

                //załącz daną kolumnę
                if (translation != nullptr)
                    seg_port &= ~(posShf << segShift);  //low
                else
                    seg_port |= posShf << segShift;     //high
            }

            if (cycle >= offTime)              //przekroczne granice cyklu?
                cycle = 0;                     //restart

            //przejście do następnej kolumny
            pos++;
            posShf <<= 1;

            if ( (posShf & modMask) == 0)
            {
                //od nowa
                posShf = 1;
                pos = 0;
            }
        }
};

#endif //_7SEG_HPP
