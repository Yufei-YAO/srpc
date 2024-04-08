#include "common/log.h"
#include "common/fd_event.h"
#include <unistd.h>
#include <memory.h>
#include <fcntl.h>
namespace srpc{



FdEvent::FdEvent(int fd){
    m_fd = fd;
    memset((void*)&m_event,0,sizeof(m_event));
    
}
FdEvent::FdEvent(){
    memset((void*)&m_event,0,sizeof(m_event));
}
FdEvent::~FdEvent(){
    //DEBUGLOG("~FdEvent");
    //if(m_fd<0) return;
    //close(m_fd);
}
void FdEvent::close(){
    if(m_fd<0) return;
    ::close(m_fd);

}
void FdEvent::setNonblock(){
    int flags = fcntl(m_fd,F_GETFL,0);
    if(flags & O_NONBLOCK){
        return;
    }
    fcntl(m_fd,F_SETFL,flags|O_NONBLOCK);

}
void FdEvent::setHandler(Event e, std::function<void()> cb, std::function<void()> errcb){
    m_event.data.fd = m_fd;
    if(e == Event::EPOLL_IN){
        //INFOLOG("listen read on fd=%d",m_fd);
        m_inCallback = cb;
        m_event.events|=EPOLLIN;
    }else if(e == Event::EPOLL_OUT){
        m_outCallback = cb;
        m_event.events|=EPOLLOUT;
    }
    m_errCallback = errcb;
}
void FdEvent::cancelHandler(Event e){
    if(e == Event::EPOLL_IN){
        m_event.events&=(~EPOLLIN);
        m_inCallback = nullptr;
    } else if(e == Event::EPOLL_OUT){
        m_event.events&=(~EPOLLOUT);
        m_outCallback = nullptr;
    }

}
std::function<void()> FdEvent::getHandler(Event e) const{
    if(e == Event::EPOLL_IN){
        return m_inCallback;
    }else if(e == Event::EPOLL_OUT){
        return m_outCallback;
    }else if(e == Event::EPOLL_ERROR){
        return m_errCallback;
    }
    ERRORLOG("FdEvent::getHandler invalid event");
    return m_errCallback;
}

void FdEvent::cancel(Event e){
    if(e == Event::EPOLL_IN){
        m_event.events&=(~EPOLLIN);
        m_inCallback = nullptr;
    }else if(e == Event::EPOLL_OUT){
        m_event.events&=(~EPOLLOUT);
        m_outCallback = nullptr;
    }else if(e == Event::EPOLL_ERROR){
        m_errCallback = nullptr;
    }

}


}