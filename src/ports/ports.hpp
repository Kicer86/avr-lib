
#ifndef PORTS_HPP
#define PORTS_HPP

#include "ports_defs.hpp"
#include "utils/macros.hpp"

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
        inline operator bool() const inline_attribute;                    //read value
        inline const Pin& operator=(bool value) const inline_attribute;   //write value
};


class IOPin
{
        friend class IOPort;
        
        const Port &m_port;
        const Port &m_pin;
        const byte m_pos;
        
        constexpr IOPin(const Port &port, const Port &pin, byte pos): m_port(port), m_pin(pin), m_pos(pos) {}
        
    public:
        inline operator bool() const inline_attribute;                      //read value
        inline const IOPin& operator=(bool value) const inline_attribute;   //write value
};


class Port
{
    friend class IOPort;

    protected:       
        const Ports::PortAddr addr;

        inline byte read() const inline_attribute
        {
            return *addr;
        }

        inline void write(byte v) const inline_attribute
        {
            *addr = v;
        }

    public:
        constexpr Port(Ports::PortT a): addr(convert(a))
        {}
        
        explicit Port(const Port&) = delete;

        inline const Port &operator=(byte v) const inline_attribute
        {
            write(v);
            return *this;
        }
        
        constexpr inline const Pin operator[](byte pos) const inline_attribute
        {
            return Pin(*this, pos);
        }

        inline operator byte() const inline_attribute
        {
            return read();
        }

        inline const Port &operator|=(byte v) const inline_attribute
        {
            *addr |= v;
            return *this;
        }

        inline const Port &operator&=(byte v) const inline_attribute
        {
            *addr &= v;
            return *this;
        }

        inline Ports::PortAddr address() const inline_attribute
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

        inline operator byte() const inline_attribute
        {
            return pin;
        }

        inline const IOPort &operator=(byte v) const inline_attribute
        {
            port.write(v);
            return *this;
        }        

        constexpr inline const IOPin operator[](byte pos) const inline_attribute //write to IOPin writes to port, read reads from pin
        {
            return IOPin(port, pin, pos);
        }

        inline Ports::PortAddr pinAddr() const inline_attribute
        {
            return pin.address();
        }

        inline Ports::PortAddr ddrAddr() const inline_attribute
        {
            return dir.address();
        }

        inline Ports::PortAddr portAddr() const inline_attribute
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
