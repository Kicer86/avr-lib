
#include "fixed.hpp"


int& operator +=(int& a, const Fixed& b)
{
  a = (Fixed(a) + b).toInt();
  return a;
}

int& operator -=(int& a, const Fixed& b)
{
  a = (Fixed(a) - b).toInt();
  return a;
}

int& operator *=(int& a, const Fixed& b)
{
  a = (Fixed(a) * b).toInt();
  return a;
}

int& operator /=(int& a, const Fixed& b)
{
  a = (Fixed(a) / b).toInt();
  return a;
}

long& operator +=(long& a, const Fixed& b)
{
  a = (Fixed(a) + b).toLong();
  return a;
}

long& operator -=(long& a, const Fixed& b)
{
  a = (Fixed(a) - b).toLong();
  return a;
}

long& operator *=(long& a, const Fixed& b)
{
  a = (Fixed(a) * b).toLong();
  return a;
}

long& operator /=(long& a, const Fixed& b)
{
  a = (Fixed(a) / b).toLong();
  return a;
}
