
#ifndef PORTS_HPP
#define PORTS_HPP

#include "ports_defs.hpp"

#define SFR_OFFSET 0 //__SFR_OFFSET


//Ports, to be well optimized, require -fwhole-program (if one file is being compiled)
//or -flto (for compilation) + -fwhole-program -flto (for linking).

//If you don'w want to use those options, declare all Ports as local (stack) variables or 
//as global objects (but with static keyword).

namespace
{
    constexpr Ports::PortAddr convert(Ports::PortT t)
    {
        return reinterpret_cast<Ports::PortAddr>(t);
    }
}


class Port;
class IOPort;

class Pin
{
        friend class Port;
        
        const Port &m_port;
        const byte m_pos;
        
        constexpr Pin(const Port &port, byte pos): m_port(port), m_pos(pos) {}
        
    public:
        inline operator bool() const __attribute__((always_inline));                    //read value
        inline const Pin& operator=(bool value) const __attribute__((always_inline));   //write value
};


class IOPin
{
        friend class IOPort;
        
        const Port &m_port;
        const Port &m_pin;
        const byte m_pos;
        
        constexpr IOPin(const Port &port, const Port &pin, byte pos): m_port(port), m_pin(pin), m_pos(pos) {}
        
    public:
        inline operator bool() const __attribute__((always_inline));                    //read value
        inline const IOPin& operator=(bool value) const __attribute__((always_inline));   //write value
};


class Port
{
    friend class IOPort;

    protected:       
        const Ports::PortAddr addr;

        inline byte read() const
        {
            return *( addr + SFR_OFFSET );
        }

        inline void write(byte v) const
        {
            *(addr + SFR_OFFSET) = v;
        }

    public:
        constexpr Port(Ports::PortT a): addr(convert(a))
        {}
        
        explicit Port(const Port&) = delete;

        inline const Port &operator=(byte v) const
        {
            write(v);
            return *this;
        }
        
        const Pin operator[](byte pos) const
        {
            return Pin(*this, pos);
        }

        inline operator byte() const
        {
            return read();
        }

        inline const Port &operator|=(byte v) const
        {
            *( addr + SFR_OFFSET ) |= v;
            return *this;
        }

        inline const Port &operator&=(byte v) const
        {
            *( addr + SFR_OFFSET ) &= v;
            return *this;
        }

        inline Ports::PortAddr address() const
        {
            return addr;
        }
        
        Port& operator= ( const Port& ) const = delete;
};


Pin::operator bool() const
{
    return (m_port & (1 << m_pos)) > 0;
}


const Pin& Pin::operator=(bool value) const
{
    if (value)
        m_port |= 1 << m_pos;
    else
        m_port &= ~(1 << m_pos);
    
    return *this;
}


class IOPort
{
    public:
        const Port port;
        const Port dir;
        const Port pin;

        constexpr IOPort(Ports::PortT a): 
            port(a), 
            dir(a - 1),
            pin(a - 2)
        {}

        inline operator byte() const
        {
            return pin;
        }

        inline const IOPort &operator=(byte v) const
        {
            port.write(v);
            return *this;
        }        

        inline const IOPin operator[](byte pos)    //write to IOPin writes to port, read reads from pin
        {
            IOPin result(port, pin, pos);
            return result;
        }

        inline Ports::PortAddr pinAddr() const
        {
            return pin.address();
        }

        inline Ports::PortAddr ddrAddr() const
        {
            return dir.address();
        }

        inline Ports::PortAddr portAddr() const
        {
            return port.address();
        }
        
        IOPort& operator= ( const IOPort& ) const = delete;
};


IOPin::operator bool() const
{
    return (m_pin & (1 << m_pos)) > 0;
}


const IOPin& IOPin::operator=(bool value) const
{
    if (value)
        m_port |= 1 << m_pos;
    else
        m_port &= ~(1 << m_pos);
    
    return *this;
}

#endif //PORTS_HPP
