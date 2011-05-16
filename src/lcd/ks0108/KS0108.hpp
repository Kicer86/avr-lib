//-------------------------------------------------------------------------------------------------
// Universal KS0108 driver library
// Atmel AVR MCU low-level driver.
// (c) Radosław Kwiecień, radek@dxp.pl
//-------------------------------------------------------------------------------------------------


#include "../../delay.hpp"
#include "../display.hpp"
#include "../../datatypes.h"
#include "../../ports/ports.hpp"

namespace Ks0108Consts
{
  const byte ScreenHeight=64;

  const byte SetY=0x40;
  const byte SetX=0xB8;
  const byte StartLine=0xC0;
  const byte OnCmd=0x3E;
  const byte On=0x01;
  const byte Off=0x00;
  const byte StatusBusy=0x80;
}

template <byte width, port_ptr dtaPort, port_ptr ctrlPort, byte rwPos, byte enPos, byte rsPos, byte cs1Pos, byte cs2Pos=0, byte cs3Pos=0>
class Ks0108: public Display<Ks0108Consts::ScreenHeight/8, width>
{
    typedef Display<Ks0108Consts::ScreenHeight/8, width> DisplayBase;

    void delay(void) const
    {
      Delay::us<1>();
    }

    void enableController(byte controller) const
    {
      TIOPort<ctrlPort> controlPort;

      switch (controller)
      {
        case 0:
          controlPort&= ~(1 << cs1Pos);
          break;
        case 1:
          if (width>64)
            controlPort&= ~(1 << cs2Pos);
          break;
        case 2:
          if (width>128)
            controlPort&= ~(1 << cs3Pos);
          break;
      }
    }

    void disableController(byte controller) const
    {
      TIOPort<ctrlPort> controlPort;

      switch (controller)
      {
        case 0 :
          controlPort|= 1 << cs1Pos;
          break;
        case 1 :
          if (width>64)
            controlPort|= 1 << cs2Pos;
          break;
        case 2 :
          if (width>128)
            controlPort|= 1 << cs3Pos;
          break;
      }
    }

    byte readStatus(byte controller) const
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      dataPort.dir = 0x00;
      controlPort |= 1 << rwPos;
      controlPort &= ~ (1 << rsPos);
      enableController(controller);
      controlPort |= 1 << enPos;
      delay();

      byte status = dataPort();
      controlPort &= ~(1 << enPos);
      disableController(controller);
      return status;
    }

    void initializePorts(void) const
    {
      TIOPort<ctrlPort> controlPort;
      controlPort.dir |=( (1<<cs1Pos) | (1<<cs2Pos) | (width>128? 1<<cs3Pos: 0) | (1<<rsPos) | (1<<rwPos) | (1<<enPos));
      controlPort |= ( (1<<cs1Pos) | (1<<cs2Pos) | (width>128? 1<<cs3Pos: 0));
    }

    void initialize(void) const
    {
      initializePorts();
      for (byte i = 0; i < width/64; i++)
        writeCommand(Ks0108Consts::OnCmd | Ks0108Consts::On, i);
    }

    byte readData(void)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      while (readStatus(DisplayBase::posX / 64) & Ks0108Consts::StatusBusy);
      dataPort.dir = 0x00;
      controlPort |= (1 << rwPos) | (1 << rsPos);
      enableController(DisplayBase::posX / 64);
//GLCD_delay();
      controlPort |= 1 << enPos;
      delay();
      byte data = dataPort();
      controlPort &= ~(1 << enPos);
      disableController(DisplayBase::posX / 64);
      DisplayBase::posX++;
      return data;
    }

    void writeCommand(byte cmd, byte controller) const
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      while (readStatus(controller) & Ks0108Consts::StatusBusy);

      controlPort &= ~( (1 << rwPos) | (1 << rsPos));
//       Delay::us<10>();
      //zmiana w stosunku do oryginału: ustawienie wyjść po przełączeniu się w tryb write
      //oryginalnie uC ustawial kierunek na wyjścia przed zmianą kierunku portu wyświetlacza.
      //Powodowalo to że na magistrali były wyjścia z obydwu stron, co raczej nie jest zdrowe :)
      dataPort.dir = 0xFF;
      enableController(controller);
      dataPort  = cmd;
      controlPort|= 1 << enPos;
      delay();
      controlPort&= ~(1 << enPos);
      disableController(controller);
    }

  public:
    Ks0108():Display<Ks0108Consts::ScreenHeight/8, width>()
    {
      initialize();
      DisplayBase::clearScreen();
    }

    virtual void writeData(byte dataToWrite)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      while (readStatus(DisplayBase::posX / 64) & Ks0108Consts::StatusBusy);
      controlPort&=~(1<<rwPos);
//       Delay::us<10>();                   //zamiana jak w przypadku writeCommand
      dataPort.dir = 0xFF;
      controlPort|= 1 << rsPos;
      dataPort = dataToWrite;
      enableController(DisplayBase::posX / 64);
      controlPort|= 1 << enPos;
      delay();
      controlPort&= ~(1 << enPos);
      disableController(DisplayBase::posX / 64);
      DisplayBase::posX++;
    }

    virtual void goTo(byte x, byte y)
    {
      DisplayBase::posX = x;
      DisplayBase::posY = y;

      for (byte i = 0; i < width/64; i++)
      {
        writeCommand(Ks0108Consts::SetY | 0,i);
        writeCommand(Ks0108Consts::SetX | y,i);
        writeCommand(Ks0108Consts::StartLine | 0,i);
      }
      writeCommand(Ks0108Consts::SetY | (x % 64), (x / 64));
      writeCommand(Ks0108Consts::SetX | y, (x / 64));
    }

    virtual void setPixel(byte x, byte y, byte color)
    {
      byte tmp;
      goTo(x, (y / 8));
      tmp = readData();
      goTo(x, (y / 8));
      tmp = readData();
      goTo(x, (y / 8));
      byte row=(1 << (y % 8));
      if (color)
        tmp|= row;
      else
        tmp&= ~row;
      writeData(tmp);
    }

    void bitmap(const byte *bmp, byte x, byte y, byte dx, byte dy)
    {
      for (byte j=0; j<(dy/8); j++)
      {
        goTo(x,y + j);
        for (byte i=0; i < dx; i++)
          writeData(Com::readFlash8(bmp++));
      }
    }
};
