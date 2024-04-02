#pragma once
#include <set>
#include <mutex>
#include <unistd.h>
#include <functional>
#include <cstdint>
#include "common/fd_event.h"
namespace srpc{


class TimeEvent{
public:
    friend class TimerFdEvent;
    typedef std::shared_ptr<TimeEvent> ptr;
    TimeEvent(uint64_t ms, std::function<void()> cb, bool isRecuring = false);
    void setCancelState(bool s) { m_isCancel = s;}

private:

    uint64_t m_next;
    uint64_t m_ms;
    bool m_isRecuring;
    bool m_isCancel;
    std::function<void()> m_cb;
};


class TimerFdEvent : public FdEvent{

public:
    typedef std::shared_ptr<TimerFdEvent> ptr;
    TimerFdEvent();
    
    void addTimeEvent(TimeEvent::ptr te);
    void addTimeEvent(const std::vector<TimeEvent::ptr>& te);
    void cancelTimeEvent(TimeEvent::ptr te);
    void onTimer();

private: 
    void resetArriveTime();
    struct cmp{
        bool operator()(const TimeEvent::ptr& l, const TimeEvent::ptr & r) const{
           return l->m_next < r->m_next || (l->m_next == r->m_next && l.get()<r.get()) ; 
        }
    };

private:
    std::multiset<TimeEvent::ptr,cmp> m_events;
    std::mutex m_mutex;


};









}