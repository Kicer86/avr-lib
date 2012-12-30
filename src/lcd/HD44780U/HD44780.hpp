/*
    Wyświetlacz alfanumeryczny 2x16 znaków HD44780.
    Wersja 4bitowa
    Copyright (C) 2011  Michał Walenciak <Kicer86@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//usefull links:
//http://www.geocities.com/dinceraydin/lcd/commands.htm

#include "../../ports/ports.hpp"
#include "../../delay.hpp"

#define HD44780_CLEAR         0x01

#define HD44780_HOME          0x02

#define HD44780_ENTRY_MODE        0x04
#define HD44780_EM_SHIFT_CURSOR   0
#define HD44780_EM_SHIFT_DISPLAY  1
#define HD44780_EM_DECREMENT    0
#define HD44780_EM_INCREMENT    2

#define HD44780_DISPLAY_ONOFF     0x08
#define HD44780_DISPLAY_OFF     0
#define HD44780_DISPLAY_ON      4
#define HD44780_CURSOR_OFF      0
#define HD44780_CURSOR_ON     2
#define HD44780_CURSOR_NOBLINK    0
#define HD44780_CURSOR_BLINK    1

#define HD44780_DISPLAY_CURSOR_SHIFT  0x10
#define HD44780_SHIFT_CURSOR    0
#define HD44780_SHIFT_DISPLAY   8
#define HD44780_SHIFT_LEFT      0
#define HD44780_SHIFT_RIGHT     4

#define HD44780_FUNCTION_SET      0x20
#define HD44780_FONT5x7       0
#define HD44780_FONT5x10      4
#define HD44780_ONE_LINE      0
#define HD44780_TWO_LINE      8
#define HD44780_4_BIT       0
#define HD44780_8_BIT       16

#define HD44780_CGRAM_SET       0x40
#define HD44780_DDRAM_SET       0x80

//connect rw to gnd
template<Ports::PortT data_port, byte dataShift, Ports::PortT ctrl_port, byte rsPos, byte ePos>
class HD44780
{
    public:
        HD44780(): m_data()
        {
            IOPort dataPort(data_port);
            IOPort ctrlPort(ctrl_port);

            dataPort.dir |= 0xf << dataShift;
            ctrlPort.dir |= (1 << rsPos) /*| (1<<rwPos)*/ | (1 << ePos);
//       ctrlPort&=~(1<<rwPos);    //force write only

            init();
        }
        
        void clear()
        {
            writeCommand(HD44780_CLEAR);
        }
        
        void turnOff()
        {
            m_data.m_screenOn = 0;
            setDisplayMode();
        }        
        
        void turnOn()
        {
            m_data.m_screenOn = 1;
            setDisplayMode();
        }

        void write_PString(const char *str)
        {
            byte data;
            while ( (data = common::readFlash8(str++)) != 0)
                writeChar(data);
        }

        void writeString(const char *str)
        {
            byte data;
            while ( (data = *str++) != 0)
                writeChar(data);
        }
        
        void setCursorVisible(bool visible = true)
        {
            m_data.m_cursorOn = visible? 1 : 0;
            setDisplayMode();
        }
        
        void setCursorBlink(bool blink = true)
        {
            m_data.m_blinkOn = blink? 1 : 0;
            setDisplayMode();
        }
        
        void moveCursorLeft()
        {
            writeCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_LEFT);
        }
        
        void moveCursorRight()
        {
            writeCommand(HD44780_DISPLAY_CURSOR_SHIFT | HD44780_SHIFT_CURSOR | HD44780_SHIFT_RIGHT);
        }
        
        void setCursorPos(int x, int y)
        {
            setAddressDD( (y % 2) * 64 + x );
        }

        void writeChar(char c)
        {
            writeData(c);
        }

        void writeHex(byte c)
        {
            writeData(common::byte2hex(c >> 4));
            writeData(common::byte2hex(c));
        }

    private:
        struct Data
        {
            byte m_screenOn:1;
            byte m_cursorOn:1;
            byte m_blinkOn:1;
        } m_data;
        
        void setAddressDD(byte addr)
        {
            writeCommand( addr | HD44780_DDRAM_SET);
        }
        
        void writeNibble(byte nibble) const
        {
            IOPort dataPort(data_port);
            IOPort ctrlPort(ctrl_port);

            ctrlPort[ePos] = true;
            dataPort.port &= ~( (0xf << dataShift) & 0xff );
            dataPort.port |= (nibble & 0xf) << dataShift;
            ctrlPort[ePos] = false;
        }

        void writeData(byte data) const
        {
            IOPort ctrlPort(ctrl_port);

            ctrlPort[rsPos] = true;
            writeNibble(data >> 4);
            writeNibble(data);

            Delay::ms(2);
            /*
             clr   XH
            ldi   XL,250
            rcall Wait4xCycles
            */
        }

        void writeCommand(byte cmd)
        {
            IOPort ctrlPort(ctrl_port);

            ctrlPort[rsPos] = false;
            writeNibble(cmd >> 4);
            writeNibble(cmd);

            Delay::ms(2);
        }
        
        void setDisplayMode()
        {
            byte options = 0;
            options |= m_data.m_blinkOn?  HD44780_CURSOR_BLINK : HD44780_CURSOR_NOBLINK;
            options |= m_data.m_cursorOn? HD44780_CURSOR_ON    : HD44780_CURSOR_OFF;
            options |= m_data.m_screenOn? HD44780_DISPLAY_ON   : HD44780_DISPLAY_OFF;
            
            writeCommand(HD44780_DISPLAY_ONOFF | options);
        }

        void init()
        {
            m_data.m_blinkOn = 0;
            m_data.m_cursorOn = 0;
            m_data.m_screenOn = 1;
            
            IOPort dataPort(data_port);

            Delay::ms(100);   //najgorszy przypadek - zasilanie 2,7V

            for (byte i = 0; i < 3; i++)
            {
                writeNibble(0x03);
                Delay::ms(5);
            }

            writeNibble(0x02);
            Delay::ms(1);

            writeCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT);
            writeCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF);
            clear();
            writeCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);
            setDisplayMode();                     
        }
};

/*
;------------------------------------------------------------------------------
; HD44780 LCD Assembly driver
; http://avr-mcu.dxp.pl
; (c) Radoslaw Kwiecien
;------------------------------------------------------------------------------
.include "tn2313def.inc"
.include "hd44780.inc"

.equ  LCD_PORT  = PORTB
.equ  LCD_DDR   = DDRB
.equ    LCD_PIN   = PINB

.equ  LCD_D4    = 0
.equ  LCD_D5    = 1
.equ  LCD_D6    = 2
.equ  LCD_D7    = 3

.equ  LCD_RS    = 4
.equ  LCD_EN    = 6



;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
LCD_WriteHexDigit:
  cpi   r16,10
  brlo  Num
  ldi   r17,'7'
  add   r16,r17
  rcall LCD_WriteData
  ret
Num:
  ldi   r17,'0'
  add   r16,r17
  rcall LCD_WriteData
  ret
;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
LCD_WriteHex8:
  push  r16

  swap  r16
  andi  r16,0x0F
  rcall LCD_WriteHexDigit

  pop   r16
  andi  r16,0x0F
  rcall LCD_WriteHexDigit
  ret
;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
LCD_WriteDecimal:
  clr   r14
LCD_WriteDecimalLoop:
  ldi   r17,10
  rcall div8u
  inc   r14
  push  r15
  cpi   r16,0
  brne  LCD_WriteDecimalLoop

LCD_WriteDecimalLoop2:
  ldi   r17,'0'
  pop   r16
  add   r16,r17
  rcall LCD_WriteData
  dec   r14
  brne  LCD_WriteDecimalLoop2

  ret

;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------
LCD_SetAddressCG:
  ori   r16, HD44780_CGRAM_SET
  rcall LCD_WriteCommand
  ret
*/
