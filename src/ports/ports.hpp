
#ifndef PORTS_HPP
#define PORTS_HPP

#include "../common.hpp"
#include "ports_defs.hpp"

#define SFR_OFFSET 0 //__SFR_OFFSET


//Ports, to be well optimized, require -fwhole-program (if one file is being compiled)
//or -flto (for compilation) + -fwhole-program -flto (for linking).

//If you don'w want to use those options, declare all Ports as local (stack) variables or 
//as global objects (but with static keyword).
//This will make them incompatible with all Port related templates from this library

class Port
{
        inline Port ( const Port& ) :addr ( 0 )
        {}

        inline Port &operator= ( const Port& )
        {
            return *this;
        }

    protected:
        const Ports::PortT addr;

        inline byte read() const
        {
            return *( addr + SFR_OFFSET );
        }

        inline void write(byte v) const
        {
            *(addr + SFR_OFFSET) = v;
        }

    public:
        constexpr Port(Ports::PortT a): addr(a)
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
            *( addr + SFR_OFFSET ) |= v;
            return *this;
        }

        inline const Port &operator&=(byte v) const
        {
            *( addr + SFR_OFFSET ) &= v;
            return *this;
        }

        inline Ports::PortT address() const
        {
            return addr;
        }
};


class IOPort:public Port
{
        const Port pin;

    public:
        const Port dir;

        constexpr IOPort(Ports::PortT a): 
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

        inline Ports::PortT pinAddr() const
        {
            return pin.address();
        }

        inline Ports::PortT ddrAddr() const
        {
            return dir.address();
        }

        inline Ports::PortT portAddr() const
        {
            return address();
        }
};


#endif //PORTS_HPP
