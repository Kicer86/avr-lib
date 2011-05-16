
#ifndef PORTS_HPP
#define PORTS_HPP

#include "../common.hpp"
#define SFR_OFFSET 32

typedef word port_ptr;       //wskaźnik na port - do używania w template

template<port_ptr addr>
class TPort
{
    inline TPort(const TPort&)
    {}

    inline TPort &operator=(const TPort&)
    {
      return *this;
    }

  protected:
    typedef volatile byte port_t;

    inline byte read() const
    {
      return *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET));
    }

    inline void write(byte v) const
    {
      *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET))=v;
    }

  public:

    TPort()
    {}

    inline const TPort &operator=(byte v) const
    {
      write(v);
      return *this;
    }

    inline operator byte() const
    {
      return read();
    }

    inline const TPort &operator|=(byte v) const
    {
      *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET))|=v;
      return *this;
    }

    inline const TPort &operator&=(byte v) const
    {
      *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET))&=v;
      return *this;
    }

    inline port_ptr address() const
    {
      return addr;
    }
};


class Port
{
  inline Port(const Port&):addr(0)
  {}

  inline Port &operator=(const Port&)
  {
    return *this;
  }

protected:
  typedef volatile byte port_t;

  port_ptr addr:4; //4 bajty starcza, porty io mają niskie wartości

  inline byte read() const
  {
    return *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET));
  }

  inline void write(byte v) const
  {
    *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET))=v;
  }

public:

  Port(port_ptr a):addr(a)
  {}

  inline const Port &operator=(byte v) const
  {
    write(v);
    return *this;
  }

  inline operator byte() const
  {
    return read();
  }

  inline const Port &operator|=(byte v) const
  {
    *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET))|=v;
    return *this;
  }

  inline const Port &operator&=(byte v) const
  {
    *(reinterpret_cast<port_t*>(addr+ SFR_OFFSET))&=v;
    return *this;
  }

  inline port_ptr address() const
  {
    return addr;
  }
};


template<port_ptr baseAddr>
class TIOPort:public TPort<baseAddr>
{
    const TPort<baseAddr-2> pin;

  public:
    const TPort<baseAddr-1> dir;

    inline TIOPort():TPort<baseAddr>(),pin(),dir()
    {}

    inline byte operator()() const
    {
      return pin;
    }

    inline const TIOPort &operator=(byte v) const
    {
      TPort<baseAddr>::write(v);
      return *this;
    }

    inline port_ptr pinAddr() const
    {
      return pin.address();
    }

    inline port_ptr ddrAddr() const
    {
      return dir.address();
    }

    inline port_ptr portAddr() const
    {
      return TPort<baseAddr>::address();
    }
};


class IOPort:public Port
{
  const Port pin;

public:
  const Port dir;

  inline IOPort(port_ptr a):Port(a),pin(a-2),dir(a-1)
  {}

  inline byte operator()() const
  {
    return pin;
  }

  inline const IOPort &operator=(byte v) const
  {
    write(v);
    return *this;
  }

  inline port_ptr pinAddr() const
  {
    return pin.address();
  }

  inline port_ptr ddrAddr() const
  {
    return dir.address();
  }

  inline port_ptr portAddr() const
  {
    return address();
  }
};

#endif //PORTS_HPP
