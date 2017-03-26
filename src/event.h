#ifndef EVENT_H_
#define EVENT_H_

#include <cassert>
#include <functional>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include "tickable.h"

namespace event {

enum priorities {
    HIGHEST     = -300,
    VERY_HIGH   = -200,
    HIGH        = -100,
    NORMAL      = 0,
    LOW         = 100,
    VERY_LOW    = 200,
    LOWEST      = 200,
};

struct mouse_event {
    mouse_event();
    mouse_event(mouse_event &last);

    SDL_Point position;
    SDL_Point last_position;
    Uint32 state;
    Uint32 last_state;

    bool changed() const;
    bool moved() const;
    bool left_clicked() const;
    bool right_clicked() const;
    bool left_pressed() const;
    bool right_pressed() const;
    bool left_released() const;
    bool right_released() const;
};

template <typename Ev>
using handler_fn = std::function<bool(Ev &)>;

struct deregister_handler {};

struct handler {
    handler() {}
    ~handler() { delete ptr; }
    handler(handler&& o) noexcept : ptr(std::move(o.ptr)) {
        o.ptr = nullptr;
    }

    bool operator()(void *ev) { assert(ptr); return (*ptr)(ev); }

    struct hndl
    {
        virtual ~hndl() {}
        virtual bool operator()(void *ev) const = 0;
    };

    template <typename Ev>
    struct impl : hndl {
        impl(handler_fn<Ev> const& f) : fn(f) {}
        bool operator()(void *ev) const { return fn(*static_cast<Ev *>(ev)); }
        handler_fn<Ev> fn;
    };

    template <typename Ev>
    handler(handler_fn<Ev> const& t) : ptr(new impl<Ev>(t)) {}

    hndl *ptr;
};

using task = std::function<void(void)>;
using timed_task = std::pair<task, double>;

class manager : public tickable {
public:
    inline manager() : handlers(), last_mouse() {}

    using priority = int;

    template<typename Ev>
    void register_handler(handler_fn<Ev> const &f, priority prio = 0);

    template<typename Ev>
    void call_handlers(Ev &ev);

    void schedule(task fn, double seconds);

    void tick(double dt) override;

private:
    std::unordered_map<std::type_index, std::map<priority, handler>> handlers;
    mouse_event last_mouse;
    std::deque<timed_task> scheduled;
};

}

#include "event.hxx"

#endif /* !EVENT_H_ */
