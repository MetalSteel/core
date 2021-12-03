#include "event_loop.h"
#include <iostream>

using namespace core;

EventLoop::EventLoop() {
    // create epoll fd
    fd_ = epoll_create1(EPOLL_CLOEXEC);

    // resize vector size
    act_evs_.resize(1024);
}

void EventLoop::AddEvent(Event event) {
    // if event not exist in event map add it
    if(ref_evs_.find(event.fd) == ref_evs_.end()) {
        // create event
        epoll_event ev;
        ev.events = event.mask;
        ev.data.fd = event.fd;

        // add epoll event
        epoll_ctl(fd_, EPOLL_CTL_ADD, event.fd, &ev);

        // add Event to map
        ref_evs_[event.fd] = event;
        return;
    }

    // error message
    std::cout << "add event to epoll failed, because this event already exist." << std::endl;
}

void EventLoop::ModEvent(Event event) {
    // check event already in event map
    if(ref_evs_.find(event.fd) != ref_evs_.end()) {
        // create event
        epoll_event ev;
        ev.events = event.mask;
        ev.data.fd = event.fd;

        // mod epoll event
        epoll_ctl(fd_, EPOLL_CTL_MOD, event.fd, &ev);
        ref_evs_[event.fd] = event;
        return;
    }

    // error message
    std::cout << "mod event failed, because this event not exist." << std::endl;
}

void EventLoop::DelEvent(Event event) {
    auto itor = ref_evs_.find(event.fd);

    // check event already in event map
    if(itor != ref_evs_.end()) {
        // delete event from epoll
        epoll_ctl(fd_, EPOLL_CTL_DEL, event.fd, nullptr);

        // delete event from event map
        ref_evs_.erase(itor->first);
        return;
    }

    // error message
    std::cout << "mod event failed, because this event not exist." << std::endl;
}

void EventLoop::Loop() {
    while(true) {
        // wait activate event
        int n = epoll_wait(fd_, act_evs_.data(), act_evs_.size(), -1);

        // check error
        if(n == -1) {
            // epoll wait maybe interrupt
            if(errno == EINTR) {
                perror("epoll wait interrupt");
                continue;
            }
            else {
                perror("epoll wait failed");
                break;
            }
        }

        // foreach loop call event function
        for(int i = 0; i < n; ++i) {
            // ready event and fd
            auto ev = act_evs_[i].events;
            auto fd = act_evs_[i].data.fd;

            // find event callback function
            auto itor = ref_evs_.find(fd);

            // check fd exist in map
            if(itor != ref_evs_.end()) {
                // fd correspondence event
                Event event = itor->second;

                if(ev & EPOLLIN) {
                    // read callback
                    event.read_callback();
                }
                else if(ev & EPOLLOUT) {
                    // write callback
                    event.write_callback();
                }
                else if(ev & EPOLLERR) {
                    // error callback
                    event.error_callback();
                }
                else {
                    perror("unknown epoll event");
                }
            }
            else {
                std::cout << "ready fd not exist in map" << std::endl;
            }
        }
    }
}