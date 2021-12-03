/**
 * Author: Gang
 * Date: 2021-12-03
 * Version: 1.0
 * Description: epoll event
 */

#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <functional>

namespace core {
    struct Event {
        using EventCallback = std::function<void()>;

        int fd = 0;
        uint32_t mask = 0; // EPOLLIN | EPOLLOUT | EPOLLERR
        EventCallback read_callback;
        EventCallback write_callback;
        EventCallback error_callback;
    };
}

#endif //CORE_EVENT_H
