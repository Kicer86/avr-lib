/*
    Klasa udostępniająca funkcje pomiaru czasu.
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


#ifndef TIMER_HPP
#define TIMER_HPP

#include "datatypes.h"

class Timer
{
    volatile dword time;
    const word time_base;  // częstotliwość (czas między) wywołań przerwania (funkcji interrupt)

  public:
    Timer(word base):time(0), time_base(base)
    {}

    void interrupt()                //hardware timer interrupt 
    {
      time+=time_base;
    }

    void start()                    //start timer
    {
      time=0;
    }

    bool elapsed(dword v) const     //check for timeout
    {
      return time>=v;
    }

    dword elapsed() const           //time already elapsed
    {
      return time;
    }
};

#endif // TIMER_HPP
