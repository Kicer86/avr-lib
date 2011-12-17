
#ifndef PORTS_HPP
#define PORTS_HPP

#include "../common.hpp"
#include "ports_defs.hpp"

#define SFR_OFFSET __SFR_OFFSET

class Port
{
        inline Port ( const Port& ) :addr ( 0 )
        {}

        inline Port &operator= ( const Port& )
        {
            return *this;
        }

    protected:
        typedef volatile byte port_t;
        typedef port_t* port_ptr;

        const Ports::Port addr;

        inline byte read() const
        {
            return *( reinterpret_cast<port_t*> ( addr + SFR_OFFSET ) );
        }

        inline void write(byte v) const
        {
            *( reinterpret_cast<port_t*> ( addr + SFR_OFFSET ) ) = v;
        }

    public:
        constexpr Port(Ports::Port a): addr(a)
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

        inline const Port &operator|= ( byte v ) const
        {
            * ( reinterpret_cast<port_t*> ( addr+ SFR_OFFSET ) ) |=v;
            return *this;
        }

        inline const Port &operator&= ( byte v ) const
        {
            * ( reinterpret_cast<port_t*> ( addr+ SFR_OFFSET ) ) &=v;
            return *this;
        }

        inline Ports::Port address() const
        {
            return addr;
        }
};


class IOPort:public Port
{
        const Port pin;

    public:
        const Port dir;

        constexpr IOPort(Ports::Port a): 
            Port(a), 
            pin(a - 2),
            dir(a - 1)
        {}

        inline byte operator() () const
        {
            return pin;
        }

        inline const IOPort &operator=(byte v) const
        {
            write(v);
            return *this;
        }

        inline Ports::Port pinAddr() const
        {
            return pin.address();
        }

        inline Ports::Port ddrAddr() const
        {
            return dir.address();
        }

        inline Ports::Port portAddr() const
        {
            return address();
        }
};


#endif //PORTS_HPP
