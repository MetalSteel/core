/**
 * Author: Gang
 * Date: 2021-12-03
 * Version: 1.0
 * Description: epoll event loop reactor
 */

#ifndef CORE_EVENT_LOOP_H
#define CORE_EVENT_LOOP_H

#include <sys/epoll.h>

#include <vector>
#include <map>

#include "event.h"

namespace core {
    class EventLoop {
    public:
        EventLoop();

        void AddEvent(Event event);
        void ModEvent(Event event);
        void DelEvent(Event event);

        void Loop();
    private:
        int fd_; // epoll fd
        std::vector<epoll_event> act_evs_; // epoll activate event array default size 1024
        std::map<int, Event> ref_evs_; // fd reference to event
    };
}

#endif //CORE_EVENT_LOOP_H
