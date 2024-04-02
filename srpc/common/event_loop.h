#pragma once
#include <sys/epoll.h>
#include <mutex>
#include <queue>
#include <set>
#include <memory>
#include "common/fd_event.h"
#include "common/timer_fd_event.h"
#include "common/wakeup_fd_event.h"

namespace srpc{




class EventLoop{
public:
    typedef std::shared_ptr<EventLoop> ptr;
    EventLoop();
    ~EventLoop();


    void loop();
    void wakeup();
    void stop();

    bool isStop() const {return m_isStop;}
    bool isLooping() const {return m_isLooping;}

    void addEpollEvent(FdEvent::ptr fd);
    void deleteEpollEvent(FdEvent::ptr fd);
    void addTimerEvent(TimeEvent::ptr fd);
    void deleteTimerEvent(TimeEvent::ptr fd);

    void addTask(std::function<void()> cb, bool is_wakeup = false);

    bool isInLoopThread() const;
public:
    static EventLoop::ptr GetCurrentEventLoop();

private:
    void initWakeupEvent();
    void initTimerEvent();



private:
    int32_t m_threadID;
    int m_epollFd;
    int m_wakeFd;
    int m_timeFd; 
    std::mutex m_mutex; 
    bool m_isStop{true};
    std::queue<std::function<void()>> m_pendingTasks; 
    std::set<int> m_listenfds;
    bool m_isLooping{false};


};










}