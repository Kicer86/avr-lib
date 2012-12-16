
#include "common.hpp"

namespace
{
    const byte FLASH ( shiftTable8 ) shiftTable8[8] =
    {1, 2, 4, 8, 16, 32, 64, 128};


    const word FLASH ( shiftTable16 ) shiftTable16[16] =
    {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};

}

namespace common
{

    byte readFlash8 ( const byte* ptr )
    {
        byte ret;
        asm (
            "lpm %0,Z    \n\t"
            :"=r" ( ret )
            :"z" ( ptr )
        );
        return ret;
    }


    char readFlash8 ( const char* ptr )
    {
        char ret;
        asm (
            "lpm %0,Z    \n\t"
            :"=r" ( ret )
            :"z" ( ptr )
        );
        return ret;
    }


    word readFlash16 ( const word* ptr )
    {
        word ret;
        asm
        (
            "lpm %A0,Z+    \n\t"
            "lpm %B0,Z     \n\t"
            :"=r" ( ret )
            :"z" ( ptr )
        );
        return ret;
    }


    byte fastShift8 ( byte v )
    {
        return readFlash8 ( &shiftTable8[v] );
    }


    word fastShift16 ( byte v )
    {
        return readFlash16 ( &shiftTable16[v] );
    }


    byte byte2hex ( byte v )
    {
        v &= 0xf;
        return v < 10 ? '0' + v : 'A' + v - 10;
    }

}
