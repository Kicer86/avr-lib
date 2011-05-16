/*
    Podstawa pod wyświetlacze graficzne
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


#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "../datatypes.h"
#include "font.h"

template<byte pages, byte width>
class Display
{
  protected:
    byte posX, posY;

    virtual void writeData(byte)=0;

  public:
    Display():posX(0),posY(0)
    {}

    virtual void setPixel(byte, byte, byte)=0;
    virtual void goTo(byte x, byte y)=0;

    void getPos(byte* x, byte* y) const
    {
      *x=posX;
      *y=posY;
    }

    byte getHeight() const
    {
      return pages*8;
    }

    byte getWidth() const
    {
      return width;
    }

    byte getX() const
    {
      return posX;
    }

    byte getY() const
    {
      return posY;
    }

    void clearScreen()
    {
      byte i, j;
      for (j = 0; j < pages; j++)
      {
        goTo(0,j);
        for (i = 0; i < width; i++)
          writeData(0x00);
      }
      goTo(0,0);
    }

    void clearPageToCol(byte x)
    {
      for (byte i=posX;i<x;i++)
        writeData(0x00);

      posX=x;
    }

    void writeChar(char c)
    {
      int i;
      c -= 32;
      for (i = 0; i < 5; i++)
        writeData(Com::readFlash8(&font5x8[(c<<2) + c + i]));
      writeData(0x00);
    }

    void writeString(const char *str)     //string z sramu
    {
      while (*str)
        writeChar(*str++);
    }

    void writePString(const char * str)    //string z pamięci flash
    {
      char cr;
      while ( (cr=Com::readFlash8(str)) )
      {
        writeChar(cr);
        str++;
      }
    }

    void drawRectangle(byte x, byte y, byte b, byte a)
    {
      byte j; // zmienna pomocnicza
      // rysowanie linii pionowych (boki)
      for (j = 0; j < a; j++)
      {
        setPixel(x, y + j, 1);
        setPixel(x + b - 1, y + j, 1);
      }
      // rysowanie linii poziomych (podstawy)
      for (j = 0; j < b; j++)
      {
        setPixel(x + j, y, 1);
        setPixel(x + j, y + a - 1, 1);
      }
    }

    void drawCircle(byte cx, byte cy ,byte radius)
    {
      signed char x, y;
      int xchange, ychange, radiusError;
      x = radius;
      y = 0;
      xchange = 1 - (radius<<1);
      ychange = 1;
      radiusError = 0;
      while (x >= y)
      {
        setPixel(cx+x, cy+y, 1);
        setPixel(cx-x, cy+y, 1);
        setPixel(cx-x, cy-y, 1);
        setPixel(cx+x, cy-y, 1);
        setPixel(cx+y, cy+x, 1);
        setPixel(cx-y, cy+x, 1);
        setPixel(cx-y, cy-x, 1);
        setPixel(cx+y, cy-x, 1);
        y++;
        radiusError += ychange;
        ychange += 2;
        if ( (radiusError<<1) + xchange > 0 )
        {
          x--;
          radiusError += xchange;
          xchange += 2;
        }
      }
    }

    void drawLine(byte x1, byte y1, byte x2, byte y2)
    {
      byte currentX, currentY;
      int xInc, yInc, dx, dy, twoDx, twoDy,
      twoDxAccumulatedError, twoDyAccumulatedError;

      dx = (x2-x1); // obliczenie składowej poziomej
      dy = (y2-y1); // obliczenie składowej pionowej

      twoDx = dx + dx; // podwojona składowa pozioma
      twoDy = dy + dy; // podwojona składowa pionowa

      currentX = x1; // zaczynamy od X1
      currentY = y1; // oraz Y1

      xInc = 1; // ustalamy krok zwiększania pozycji w poziomie
      yInc = 1; // ustalamy krok zwiększania pozycji w pionie

      if (dx < 0) // jesli składowa pozioma jest ujemna
      {
        xInc = -1; // to będziemy się "cofać" (krok ujemny)
        dx = -dx;  // zmieniamy znak składowej na dodatni
        twoDx = -twoDx; // jak również podwojonej składowej
      }

      if (dy < 0) // jeśli składowa pionowa jest ujemna
      {
        yInc = -1; // to będziemy się "cofać" (krok ujemny)
        dy = -dy; // zmieniamy znak składowej na dodatki
        twoDy = -twoDy; // jak równiez podwojonej składowej
      }

      setPixel(x1, y1, 1); // stawiamy pierwszy krok (zapalamy pierwszy piksel)

      if ((dx != 0) || (dy != 0)) // sprawdzamy czy linia składa się z więcej niż jednego punktu ;)
      {
        // sprawdzamy czy składowa pionowa jest mniejsza lub równa składowej poziomej
        if (dy <= dx) // jeśli tak, to idziemy "po iksach"
        {
          twoDxAccumulatedError = 0; // zerujemy zmienną
          do // ruszamy w drogę
          {
            currentX += xInc; // do aktualnej pozycji dodajemy krok
            twoDxAccumulatedError += twoDy; // a tu dodajemy podwojoną składową pionową
            if (twoDxAccumulatedError > dx) // jeśli TwoDxAccumulatedError jest większy od Dx
            {
              currentY += yInc; // zwiększamy aktualną pozycję w pionie
              twoDxAccumulatedError -= twoDx; // i odejmujemy TwoDx
            }
            setPixel(currentX, currentY, 1);// stawiamy następny krok (zapalamy piksel)
          }
          while (currentX != x2); // idziemy tak długo, aż osiągniemy punkt docelowy
        }
        else // w przeciwnym razie idziemy "po igrekach"
        {
          twoDyAccumulatedError = 0;
          do
          {
            currentY += yInc;
            twoDyAccumulatedError += twoDx;
            if (twoDyAccumulatedError>dy)
            {
              currentX += xInc;
              twoDyAccumulatedError -= twoDy;
            }
            setPixel(currentX, currentY, 1);
          }
          while (currentY != y2);
        }
      }
    }

    void drawTo(byte x, byte y)
    {
      drawLine(posX,posY,x,y);
    }
};

#endif // DISPLAY_HPP
