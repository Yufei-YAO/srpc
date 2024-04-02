#include <sys/timerfd.h>
#include <cstring>
#include <algorithm>
#include <queue>
#include <functional>
#include "common/timer_fd_event.h"
#include "common/log.h"
#include "common/util.h"

namespace srpc{






TimeEvent::TimeEvent(uint64_t ms, std::function<void()> cb, bool isRecuring){
    uint64_t cur_ms = getCurrentMS();    
    m_ms = ms;
    m_cb = cb;
    m_next = cur_ms + m_ms;
    m_isRecuring = isRecuring;
    m_isCancel = false;
}


TimerFdEvent::TimerFdEvent() {
    m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    DEBUGLOG("timer fd=%d", m_fd);
    // 把 fd 可读事件放到了 eventloop 上监听
    if(m_fd < 0){
        ERRORLOG("TimerFdEvent::TimerFdEvent eventfd fails errno=%d errstr=%s",errno,strerror(errno));
        exit(-1);
    }
    FdEvent::setHandler(Event::EPOLL_IN,std::bind(std::mem_fn(&TimerFdEvent::onTimer),this));
    this->setNonblock();
}


void TimerFdEvent::addTimeEvent(TimeEvent::ptr te){
    uint64_t now = getCurrentMS();
    te->m_next = now+te->m_ms;
    DEBUGLOG("add time event %u",te->m_next);
    std::unique_lock<std::mutex> l(m_mutex);
    m_events.insert(te);
    if(*m_events.begin() == te){
        l.unlock();
        resetArriveTime();
    }
}
void TimerFdEvent::addTimeEvent(const std::vector<TimeEvent::ptr>& te){
    bool reset = false;
    uint64_t now = getCurrentMS();
    for(auto &k : te){
        k->m_next = now+k->m_ms;
    }
    std::unique_lock<std::mutex> l(m_mutex);
    for(auto &k : te){
        m_events.insert(k);
        if(*m_events.begin() == k){
            reset = true;
        }
    }
    l.unlock();
    if(reset)
        resetArriveTime();
}
void TimerFdEvent::cancelTimeEvent(TimeEvent::ptr te){
    te->m_isCancel = true;
    std::unique_lock<std::mutex> l(m_mutex);


    auto begin = m_events.lower_bound(te);
    auto end = m_events.upper_bound(te);
    if(begin == m_events.end()){
        return;
    }
    for(;begin!=end;++begin){
        if(*begin == te){
            break;
        }
    }
    bool need_reset = false;
    if(begin == m_events.begin()){
        need_reset = true;
    }
    m_events.erase(begin);
    l.unlock();
    if(need_reset){
        resetArriveTime();
    }
}
void TimerFdEvent::onTimer(){
    DEBUGLOG("on timer");
    char buf[8];
    while(true){
        if((-1==read(m_fd,buf,8))&& (errno==EAGAIN)){
            break;
        }
    }
    uint64_t now = getCurrentMS();
    std::vector<TimeEvent::ptr> time_events;
    std::unique_lock<std::mutex> l(m_mutex);
    DEBUGLOG("process timer %d",m_events.size());
    
    for(auto it = m_events.begin();it != m_events.end(); ++it){
        DEBUGLOG("process timer %u",now);
        DEBUGLOG("process timer %u %u",now, (*it)->m_next);
        if((*it)->m_next<=now){
            time_events.push_back(*it);
        }else{
            break;
        }
    }
    for(auto &k : time_events){
        m_events.erase(k);
    }
    l.unlock();
    std::vector<TimeEvent::ptr> recur;
    std::vector<std::function<void()>> tasks;
    for(size_t i = 0 ; i < time_events.size(); ++i){
        if(time_events[i]->m_isCancel){
            continue;
        }
        if(time_events[i]->m_isRecuring){
            time_events[i]->m_next = time_events[i]->m_ms + now;
            recur.push_back(time_events[i]);
        }
        tasks.push_back(time_events[i]->m_cb);
    }
    addTimeEvent(recur);
    for(auto & t : tasks){
        t();
    }
    resetArriveTime();

}

void TimerFdEvent::resetArriveTime(){
    uint64_t new_time;
    std::unique_lock<std::mutex> l(m_mutex);
    if(m_events.empty()){
        return;
    }
    new_time = (*m_events.begin())->m_next;
    l.unlock();
    uint64_t cur_time = getCurrentMS();
    if(new_time < cur_time){
        new_time = 100;
    }else{
        new_time = new_time - cur_time;
    }

    struct itimerspec iti;
    memset((void*)&iti,0,sizeof(iti));
    iti.it_value.tv_sec = new_time/1000;
    iti.it_value.tv_nsec = (new_time%1000)*1000000;
    int rt = timerfd_settime(m_fd, 0,&iti,0);
    if(rt == -1){
        ERRORLOG("reset timerfd fail with errno=%d,errstr=%s",errno,strerror(errno));
    }
    DEBUGLOG("timerfd reset");
}

}