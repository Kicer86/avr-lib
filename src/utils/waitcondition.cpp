
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <utils/waitcondition.hpp>

WaitCondition::WaitCondition(): m_wakeup(false)
{

}


WaitCondition::~WaitCondition()
{

}


void WaitCondition::wait()
{
    m_wakeup = false;
        
    cli();
    while (m_wakeup == false)
    {
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
        cli();
    }
    sei();
}


void WaitCondition::wakeup()
{
    m_wakeup = true;
}
