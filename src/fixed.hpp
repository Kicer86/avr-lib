/*
Copyright (c) 2006 Henry Strickland & Ryan Seto

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

        (* http://www.opensource.org/licenses/mit-license.php *)
*/

#include "datatypes.h"

class Fixed
{
    long g; // the guts

    const static long BP= sizeof(g)/2*8;        // how many low bits are right of Binary Point
    const static long BP2= BP*2;     // how many low bits are right of Binary Point
    const static long BPhalf= BP/2;  // how many low bits are right of Binary Point

    // for private construction via guts
    enum FixedRaw { RAW };
    Fixed(FixedRaw, int guts) : g(guts) {}

  public:
    Fixed() : g(0) {}
    Fixed(const Fixed& a) : g( a.g ) {}
    Fixed(int a) : g( static_cast<long>(a) << BP ) {}
    Fixed(long a) : g( a << BP ) {}

    byte toString(char *buf, byte fr) const
    {
      const char * const base=buf;
      int v=g>>BP;
      int m=10000;

      //część całkowita
      for (int i=5;i>0;i--,m/=10)
      {
        if (v<m)
        {
          int t=v/m;
          *buf++=t+'0';
          v-=t*m;
        }
      }

      *buf++='.';

      //poskładaj sumę ułamków
      unsigned int a=50000,s=0;
      unsigned int lm=1<<15;
      int f=g & ((1L<<BP)-1);
      for (int i=0; i<16; i++, lm>>=1, a/=2)
        if (f & lm)
          s+=a;

      unsigned int fm=10000;
      //dopisz ułamek do bufora
      for (int i=0; i<fr; i++, fm/=10)
      {
        if (s<fm)
        {
          int t=s/fm;
          *buf++=t+'0';
          s-=t*fm;
        }
      }

      *buf++='\0';
      return buf-base-1;
    }

    int toInt() const
    {
      return g>>BP;
    }

    long toLong() const
    {
      return g>>BP;
    }

    Fixed& operator=(const Fixed& a)
    {
      g= a.g;
      return *this;
    }

    Fixed& operator=(int a)
    {
      g= Fixed(a).g;
      return *this;
    }

    Fixed& operator=(long a)
    {
      g= Fixed(a).g;
      return *this;
    }

//     operator int()
//     {
//       return g>>BP;
//     }

//     operator long()
//     {
//       return g>>BP;
//     }

    Fixed operator+() const
    {
      return Fixed(RAW,g);
    }

    Fixed operator-() const
    {
      return Fixed(RAW,-g);
    }

    Fixed operator+(const Fixed& a) const
    {
      return Fixed(RAW, g + a.g);
    }

    Fixed operator-(const Fixed& a) const
    {
      return Fixed(RAW, g - a.g);
    }
#if 1
    // more acurate, using long long
    Fixed operator*(const Fixed& a) const
    {
      return Fixed(RAW,  (int)( (static_cast<long long>(g) * static_cast<long long>(a.g) ) >> BP));
    }
#else
    // faster, but with only half as many bits right of binary point
    Fixed operator*(const Fixed& a) const
    {
      return Fixed(RAW, (g>>BPhalf) * (a.g>>BPhalf) );
    }
#endif
    Fixed operator/(const Fixed& a) const
    {
      return Fixed(RAW, int( ((static_cast<long long>(g) << BP2) / static_cast<long long>(a.g)) >> BP) );
    }

    Fixed operator/(long a) const
    {
      return Fixed(RAW, int( ((static_cast<long long>(g) << BP2) / static_cast<long long>(Fixed(a).g)) >> BP) );
    }
    
        Fixed operator/(int a) const
    {
      return Fixed(RAW, int( (( static_cast<long long>(g) << BP2) / (Fixed(a).g)) >> BP) );
    }

    Fixed& operator+=(Fixed a)
    {
      return *this = *this + a;
    }

    Fixed& operator-=(Fixed a)
    {
      return *this = *this - a;
    }

    Fixed& operator*=(Fixed a)
    {
      return *this = *this * a;
    }

    Fixed& operator/=(Fixed a)
    {
      return *this = *this/ a;
    }

    Fixed& operator+=(int a)
    {
      return *this = *this + Fixed(a);
    }

    Fixed& operator -=(int a)
    {
      return *this = *this - Fixed(a);
    }

    Fixed& operator *=(int a)
    {
      return *this = *this * Fixed(a);
    }

    Fixed& operator /=(int a)
    {
      return *this = *this / Fixed(a);
    }

    Fixed& operator +=(long a)
    {
      return *this = *this + Fixed(a);
    }

    Fixed& operator -=(long a)
    {
      return *this = *this - Fixed(a);
    }

    Fixed& operator *=(long a)
    {
      return *this = *this * Fixed(a);
    }

    Fixed& operator /=(long a)
    {
      return *this = *this / Fixed(a);
    }

    bool operator ==(const Fixed& a) const
    {
      return g == a.g;
    }

    bool operator !=(const Fixed& a) const
    {
      return g != a.g;
    }

    bool operator <=(const Fixed& a) const
    {
      return g <= a.g;
    }

    bool operator >=(const Fixed& a) const
    {
      return g >= a.g;
    }

    bool operator  <(const Fixed& a) const
    {
      return g  < a.g;
    }

    bool operator  >(const Fixed& a) const
    {
      return g  > a.g;
    }

    bool operator ==(int a) const
    {
      return g == Fixed(a).g;
    }

    bool operator !=(int a) const
    {
      return g != Fixed(a).g;
    }

    bool operator <=(int a) const
    {
      return g <= Fixed(a).g;
    }

    bool operator >=(int a) const
    {
      return g >= Fixed(a).g;
    }

    bool operator<(int a) const
    {
      return g  < Fixed(a).g;
    }

    bool operator>(int a) const
    {
      return g  > Fixed(a).g;
    }
};

int& operator +=(int& a, const Fixed& b);
int& operator -=(int& a, const Fixed& b);
int& operator *=(int& a, const Fixed& b);
int& operator /=(int& a, const Fixed& b);
long& operator +=(long& a, const Fixed& b);
long& operator -=(long& a, const Fixed& b);
long& operator *=(long& a, const Fixed& b);
long& operator /=(long& a, const Fixed& b);
