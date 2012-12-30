
#ifndef __AVR_CPP_LIB__WAIT_CONDITION__
#define __AVR_CPP_LIB__WAIT_CONDITION__

class WaitCondition
{
        volatile bool m_wakeup;
        
    public:
        WaitCondition();        
        ~WaitCondition();
        
        void wait();        
        void wakeup();
};

#endif
