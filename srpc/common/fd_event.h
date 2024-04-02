#pragma once
#include <functional>
#include <sys/epoll.h>
#include <memory>

namespace srpc{



class FdEvent{
public:
    friend class FdEventGroup;  
    typedef std::shared_ptr<FdEvent> ptr;
    enum class Event{
        EPOLL_IN,
        EPOLL_OUT,
        EPOLL_ERROR
    };
    FdEvent(int fd);
    FdEvent();
    virtual ~FdEvent();

    void setNonblock();

    void setHandler(Event e, std::function<void()> cb, std::function<void()> errcb = nullptr);
    std::function<void()> getHandler(Event e) const;
    void close();
    void cancel(Event e);
    int getFd() const {return m_fd;}
    epoll_event getEpollEvent() const {return m_event;}

protected:
    int m_fd{-1};

    epoll_event m_event;
    std::function<void()> m_inCallback;
    std::function<void()> m_outCallback;
    std::function<void()> m_errCallback;


};










}
