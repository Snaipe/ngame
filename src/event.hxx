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
    auto it = handlers[typeid(mouse_event)].begin();
    while (it != handlers[typeid(mouse_event)].end()) {
        try {
            if (it->second(&ev))
                break;
            ++it;
        } catch (struct deregister_handler &d) {
            it = handlers[typeid(mouse_event)].erase(it);
        }
    }
}

}

#endif /* !EVENT_HXX_ */
