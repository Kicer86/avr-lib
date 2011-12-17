
#ifndef _7SEG_HPP
#define _7SEG_HPP

#include "../../datatypes.h"
#include "../../ports/ports.hpp"


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


template <byte modules, IOPort &data_port, IOPort &seg_port, byte segShift, const char * translation>
class Disp7Seg
{
        byte pos;      //pozycja
        byte posShf;   //pozacja jako krążące 1
        volatile word onTime, offTime;  //miganie
        volatile byte values[modules];

    public:
        Disp7Seg(): pos(0), posShf(1), onTime(1), offTime(1)
        {
            const byte modMask = (1 << modules) - 1;

            data_port.dir = 0xff;            //wyjścia
            data_port = 0xff;                //stan wysoki
            seg_port.dir |= modMask << segShift;
            seg_port |= modMask << segShift; //stan wysoki

            clear();
        }

        void setValue(byte val[])
        {
            for (int i = 0;i < modules;i++)
                values[i] = translation[val[i]];
        }

        void clear()
        {
            for (int i = 0;i < modules;i++)
                values[i] = translation[0];
        }

        void setEmpty()
        {
            for (int i = 0;i < modules;i++)
                values[i] = translation[10]; //pustka
        }

        //f - częstotliwość migania obrazka - dobrać experymentalnie
        //duty - wypełnienie - 128 = 100%
        void blink(word f, byte duty)
        {
            onTime = f * duty / 128;
            offTime = f;
        }

        void interrupt()
        {
            const byte modMask = (1 << modules) - 1;
            static word cycle;

            //wygaś kolumny (stan wysoki)
            seg_port |= modMask << segShift;  //stan wysoki

            if (cycle++ < onTime)            //świeć tylko jesli dany cykl zawiera się w czasie świecenia
            {
                //wystaw nowa wartość
                data_port = values[pos];

                //załącz daną kolumnę
                seg_port &= ~(posShf << segShift);
            }

            if (cycle >= offTime)            //przekroczne granice cyklu?
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
