#include <string.h>
#include "common/fd_event_group.h"
#include "common/log.h"
#include "net/tcp/tcp_connection.h"
#include "net/rpc/rpc_dispatcher.h"
#include "protocol/string_coder.h"
#include "protocol/tiny_pb_coder.h"
#include "protocol/tiny_pb_protocol.h"

namespace srpc
{

TcpConnection::TcpConnection(EventLoop::ptr event_loop, int fd, int buffer_size, NetAddr::ptr peer_addr, NetAddr::ptr local_addr, Type type)
    : m_eventLoop(event_loop)
    , m_localAddr(local_addr)
    , m_peerAddr(peer_addr)
    , m_type(type){
    
    m_state = State::CONNECT;
    m_coder.reset(new TinyPBCoder());
    m_inBuffer.reset(new TcpBuffer(buffer_size));
    m_outBuffer.reset(new TcpBuffer(buffer_size));

    m_fd = fd;
    FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(fd)->setNonblock();
}
TcpConnection::TcpConnection(EventLoop::ptr event_loop, int fd, int buffer_size, NetAddr::ptr peer_addr, Type type)
    : m_eventLoop(event_loop)
    , m_peerAddr(peer_addr)
    , m_type(type){
    
    m_state = State::CLOSE;
    m_coder.reset(new TinyPBCoder);
    m_inBuffer.reset(new TcpBuffer(buffer_size));
    m_outBuffer.reset(new TcpBuffer(buffer_size));

    m_fd = fd;
    FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(fd)->setNonblock();
}
TcpConnection::~TcpConnection(){
    DEBUGLOG("~TcpConnection");
    shutdown();
    clear();
}

void TcpConnection::onRead(){
    if(m_state != State::CONNECT){
        ERRORLOG("TcpConnection onRead on a closed fd");
        return;
    }

    bool is_read_all = false;
    bool is_close = false;

    while(!is_read_all){
        if(m_inBuffer->writeAble()==0){
            m_inBuffer->resizeBuffer(m_inBuffer->m_size * 2);
        }
        int read_able = m_inBuffer->writeAble();
        int rt  = read(m_fd,&(m_inBuffer->m_cache[m_inBuffer->writeIndex()]),read_able);


        DEBUGLOG("TcpConnection read on fd=%d with rt=%d readAble=%d",m_fd, rt,read_able);
        if(rt  > 0){
            m_inBuffer->moveWriteIndex(rt);
            //INFOLOG("%d",m_inBuffer->m_cache[0]);
            if(rt == read_able){
                continue;
            }else{
                is_read_all = true;
                break;
            }
        }else if(rt == 0){
            is_read_all = true;
            is_close = true;
            break;
        }else if(rt < 0 ) {
            DEBUGLOG("TcpConnection read on fd=%d with errno=%d errstr=%s",m_fd, errno, strerror(errno));
            if(errno ==  EAGAIN){
                is_read_all = true;
                break;
            }
        }
    }

    if(is_close){
        INFOLOG("the socket has closed");
        shutdown();
        clear();
        return;
    }

    execute();

}
void TcpConnection::execute(){
    if(m_type == Type::SERVER){


        std::vector<AbstractProtocol::ptr> tmp;
        //INFOLOG("start decoding");
        m_coder->decode(tmp,m_inBuffer);
        //INFOLOG("start decoding");
        std::vector<AbstractProtocol::ptr> ret;

        for(auto k : tmp){

            auto sp = std::dynamic_pointer_cast<TinyPBProtocol>(k);
            if(sp == nullptr){
                ERRORLOG("cannot transform to TinyPBProtocol");
                continue;
            }
            INFOLOG("recevive pb data:%s",sp->m_pbData.c_str());
            TinyPBProtocol::ptr res = std::make_shared<TinyPBProtocol>();
            auto conn = shared_from_this();
            
            RpcDispatcher::GetRpcDispatcher()->dispatch(sp,res,conn);
            INFOLOG("out pb data:%s",res->debugPBToHex().c_str());
            ret.push_back(res);
            
        }        
        if(tmp.size()==0){
            INFOLOG("decode size = 0 inBufferReadAble=%d",m_inBuffer->readAble());
            return;
        }

        m_coder->encode(ret,m_outBuffer);
        //INFOLOG("TcpConnection execute with ret size=%d",msg.size());
        //m_outBuffer->writeToBuffer(msg.c_str(),msg.size());
        listenWrite();
    } else{
        std::vector<AbstractProtocol::ptr> msgs;
        m_coder->decode(msgs,m_inBuffer); 
        for(auto k : msgs){
            auto sp = std::dynamic_pointer_cast<TinyPBProtocol>(k);
            if(sp == nullptr){
                ERRORLOG("cannot transform to TinyPBProtocol");
                continue;
            }
            INFOLOG("recevive pb data: %s",sp->debugPBToHex().c_str());
            //sp->m_pbData="some thing important return";
        }        
        for(auto msg:msgs){
            if(m_readDones.find(msg->getReqID())!= m_readDones.end()){
                m_readDones[msg->getReqID()](msg);
                m_readDones.erase(msg->getReqID());
            }else{
                ERRORLOG("unknown request id");
            }
        }
        
    }

}
void TcpConnection::onWrite(){
    if(m_state != State::CONNECT){
        ERRORLOG("TcpConnection onWrite on a closed fd");
        return;
    }

    INFOLOG("TcpConnection on  write on fd=%d",m_fd);
    if(m_type == Type::CLIENT){
        std::vector<AbstractProtocol::ptr> messages;

        for (size_t i = 0; i< m_writeDones.size(); ++i) {
            messages.push_back(m_writeDones[i].first);
        } 
        m_coder->encode(messages, m_outBuffer);
    }

    bool is_write_all = false;
    bool is_close = false;

    while(!is_write_all){
        int write_able = m_outBuffer->readAble();
        if(write_able == 0){
            is_write_all =true;
            continue;
        }
        int rt  = write(m_fd,&(m_outBuffer->m_cache[m_outBuffer->readIndex()]),write_able);
        INFOLOG("TcpConnection write on fd=%d with rt=%d readAble=%d",m_fd, rt,write_able);
        if(rt> 0){
            m_outBuffer->moveReadIndex(rt);
            if(rt == write_able){
                continue;
            }else{
                is_write_all = true;
                break;
            }
        }else if(rt == 0){
            is_close = true;
            break;
        }else if(rt < 0 ) {
            if(errno ==  EAGAIN){
                break;
            }
        }
    }

    if(is_write_all){
        FdEvent::ptr fdp =  FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_fd);
        fdp->cancelHandler(FdEvent::Event::EPOLL_OUT);
        m_eventLoop->addEpollEvent(fdp);
    }
    if(is_close){
        shutdown();
        clear();
    }

    if (m_type == Type::CLIENT) {
        for (size_t i = 0; i < m_writeDones.size(); ++i) {
            m_writeDones[i].second(m_writeDones[i].first);
        }
        m_writeDones.clear();
    }
    

}


void TcpConnection::clear(){
    if(m_state == State::CLOSE){
        return ;
    }
    FdEvent::ptr fdp = FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_fd);
    fdp->cancelHandler(FdEvent::Event::EPOLL_IN);
    fdp->cancelHandler(FdEvent::Event::EPOLL_OUT);
    m_eventLoop->deleteEpollEvent(fdp);
    fdp->close();
    FdEventGroup::GetGlobalFdEventGroup()->resetFdEvent(fdp->getFd());
    m_state = State::CLOSE;

}
void TcpConnection::shutdown(){
    if(m_state != State::CONNECT){
        return ;
    }
    ::shutdown(m_fd,SHUT_RDWR);
    m_state = State::HALFCLOSE;
}


void TcpConnection::listenRead(){
    FdEvent::ptr fdp = FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_fd);
    fdp->setHandler(FdEvent::Event::EPOLL_IN, std::bind(&TcpConnection::onRead,this));
    m_eventLoop->addEpollEvent(fdp);
}
void TcpConnection::listenWrite(){
    FdEvent::ptr fdp = FdEventGroup::GetGlobalFdEventGroup()->getFdEvent(m_fd);
    fdp->setHandler(FdEvent::Event::EPOLL_OUT, std::bind(&TcpConnection::onWrite,this));
    m_eventLoop->addEpollEvent(fdp);

}

void TcpConnection::pushSendMessage(AbstractProtocol::ptr msg, std::function<void(AbstractProtocol::ptr)> done){
    m_writeDones.push_back({msg,done});

}
void TcpConnection::pushReadMessage(const std::string & req_id, std::function<void(AbstractProtocol::ptr)> done){
    m_readDones.insert({req_id,done});
}

} // namespace srpc
