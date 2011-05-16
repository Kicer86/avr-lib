
#include "../display.hpp"
#include "../../datatypes.h"
#include "../../ports/ports.hpp"
#include "delay.hpp"


namespace Sed1520Consts
{
  const byte Display_On=0xAF;
  const byte Display_Off=0xAE;
  const byte DisplayStartLine=0xC0;
  const byte PageAddressSet=0xB8;
  const byte ColumnAddressSet=0x00;
  const byte AdcClockwise=0xA0;
  const byte AdcCounterClockwise=0xA1;
  const byte StaticDriveOn=0xA5;
  const byte StaticDriveOff=0xA4;
  const byte DutyRatio16=0xA8;
  const byte DutyRatio32=0xA9;
  const byte ReadModifyWrite=0xE0;
  const byte EndReadModify=0xEE;
  const byte Reset=0xE2;

  const byte ScreenWidth=122;
}

template<port_ptr dtaPort, port_ptr ctrlPort, byte e1, byte e2, byte rw, byte a0, byte res>
class Sed1520:public Display<32/8, Sed1520Consts::ScreenWidth>
{
    void waitForStatus(byte status, byte controller)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      char tmp;
      controlPort &= ~ (1<<a0);
      controlPort |= 1<<rw;
      dataPort.dir = 0x00;
      do
      {
        if (controller == 0)
        {
          controlPort |= 1 << e1;
          asm("nop");
          asm("nop");
          tmp = dataPort();
          controlPort &= ~ (1<<e1);
        }
        else
        {
          controlPort |= 1<<e2;
          asm("nop");
          asm("nop");
          tmp = dataPort();
          controlPort &= ~(1<<e2);
        }
      }
      while (tmp & status);
      dataPort.dir = 0xFF;
    }

    void writeCommand(byte commandToWrite, byte ctrl)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      waitForStatus(0x80, ctrl);

      controlPort &= ~(1 << a0);
      controlPort &= ~(1 << rw);

      dataPort = commandToWrite;

      if (ctrl)
      {
        controlPort |= 1 << e2;
        asm("nop");
        asm("nop");
        controlPort &= ~(1 << e2);
      }
      else
      {
        controlPort |= (1<< e1);
        asm("nop");
        asm("nop");
        controlPort &= ~(1<<e1);
      }
    }

    byte readData(void)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;
      byte tmp;

      waitForStatus(0x80, 0);
      waitForStatus(0x80, 1);
      controlPort |= 1 << a0;
      controlPort |= 1 << rw;
      dataPort.dir = 0x00;
      dataPort = 0xFF;
      if (posX < 61)
      {
        controlPort |= 1<<e1;
        asm("nop");
        asm("nop");
        controlPort &= ~(1<<e1);
        asm("nop");
        asm("nop");
        controlPort |= 1<<e1;
        asm("nop");
        asm("nop");
        tmp = dataPort();
        controlPort &= ~(1 << e1);
      }
      else
      {
        controlPort |= 1<<e2;
        asm("nop");
        asm("nop");
        controlPort &= ~(1<<e2);
        asm("nop");
        asm("nop");
        controlPort |= 1<<e2;
        asm("nop");
        asm("nop");
        tmp = dataPort();
        controlPort &= ~(1<<e2);
      }
      dataPort.dir = 0xFF;
      posX++;
      if (posX > 121)
        posX = 0;
      return tmp;
    }

    void initPorts(void)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      dataPort.dir=0xff;
      controlPort.dir|=(1<<e2) | (1<<e1) | (1<<rw) | (1<<a0) | (1<<res);
      Delay::ms(10);
      controlPort|=1<<res;
    }

    void initialize(void)
    {
      initPorts();

      // TODO: poniższe zawiesza model proteusa

//   WriteCommand(Sed1520Consts::Reset, 0);
//   WriteCommand(Sed1520Consts::Reset, 1);
//   WaitForStatus(0x10, 0);
//   WaitForStatus(0x10, 1);
      writeCommand(Sed1520Consts::Display_On, 0);
      writeCommand(Sed1520Consts::Display_On, 1);
      writeCommand(Sed1520Consts::DisplayStartLine | 0, 0);
      writeCommand(Sed1520Consts::DisplayStartLine | 0, 1);
    }

  public:
    Sed1520(): Display()
    {
      initialize();
    }

    void writeData(byte dataToWrite)
    {
      TIOPort<ctrlPort> controlPort;
      TIOPort<dtaPort> dataPort;

      waitForStatus(0x80, 0);
      waitForStatus(0x80, 1);
      controlPort |= 1<<a0;
      controlPort &= ~(1<<rw);
      dataPort = dataToWrite;
      if (posX < 61)
      {
        controlPort |= 1<<e1;
        asm("nop");
        asm("nop");
        controlPort &= ~(1<<e1);
      }
      else
      {
        controlPort |= 1<<e2;
        asm("nop");
        asm("nop");
        controlPort &= ~(1<<e2);
      }
      posX++;
      if (posX >= Sed1520Consts::ScreenWidth)
        posX = 0;
    }

    void goTo(byte x, byte y)
    {
      posX = x;
      posY = y;

      if (x < (Sed1520Consts::ScreenWidth/2))
      {
        writeCommand(Sed1520Consts::ColumnAddressSet | posX, 0);
        writeCommand(Sed1520Consts::PageAddressSet | posY, 0);
        writeCommand(Sed1520Consts::ColumnAddressSet | 0, 1);
        writeCommand(Sed1520Consts::PageAddressSet | posY, 1);
      }
      else
      {
        writeCommand(Sed1520Consts::ColumnAddressSet | (posX - (Sed1520Consts::ScreenWidth/2)), 1);
        writeCommand(Sed1520Consts::PageAddressSet | posY, 1);
      }
    }

    void setPixel(byte x, byte y, byte color)
    {
      byte temp;
      goTo(x, y/8);
      temp = readData();
      goTo(x, y/8);
      if (color)
        writeData(temp | (1 << (y % 8)));
      else
        writeData(temp & ~(1 << (y % 8)));
    }

    void bitmap(const byte* bmp, byte x, byte y, byte dx, byte dy);
};
