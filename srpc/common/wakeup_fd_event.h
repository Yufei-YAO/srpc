#pragma once

#include "common/fd_event.h"



namespace srpc{

class WakeupFdEvent : public FdEvent{
public:
    typedef std::shared_ptr<WakeupFdEvent> ptr;
    WakeupFdEvent() ;


    void wakeup();

public:
    void onWakeup();

private:


};








}