#ifndef EVENT_HXX_
#define EVENT_HXX_

#include <iostream>

namespace event {

template<typename Ev>
void manager::register_handler(handler_fn<Ev> const &f, priority prio) {
    handlers[typeid(Ev)].emplace(std::make_pair(prio, f));
}

template<typename Ev>
void manager::call_handlers(Ev &ev) {
    std::cout << "event triggered" << std::endl;
    for (auto &pair : handlers[typeid(mouse_event)]) {
        std::cout << pair.first << std::endl;
        if (pair.second(&ev))
            break;
    }
    std::cout << "event handled" << std::endl;
}

}

#endif /* !EVENT_HXX_ */
