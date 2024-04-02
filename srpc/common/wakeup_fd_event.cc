#include "common/wakeup_fd_event.h"
#include "common/log.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <cstring>

namespace srpc
{
    
WakeupFdEvent::WakeupFdEvent() {
    m_fd = eventfd(0,EFD_NONBLOCK);
    if(m_fd < 0){
        ERRORLOG("WakeupFdEvent::WakeupFdEvnet eventfd fails errno=%d errstr=%s",errno,strerror(errno));
        exit(-1);
    }
    
    FdEvent::setHandler(Event::EPOLL_IN,std::bind(std::mem_fn(&WakeupFdEvent::onWakeup),this));
    this->setNonblock();
}

void WakeupFdEvent::wakeup(){
    char buf[8] = {'a'};
    int rt = write(m_fd,(const char*)buf,sizeof(buf));
    if(rt!=8){
        ERRORLOG("wakeup write fail with rt=%d fd=%d",rt,m_fd);
    }
    DEBUGLOG("wake up");
}

void WakeupFdEvent::onWakeup(){
    char buf[8];
    while(1) {
        if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN) {
            break;
        }
    }
}

} // namespace srpc
