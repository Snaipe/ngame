#include <SDL2/SDL_mouse.h>
#include "event.h"

namespace event {

void manager::tick(double dt)
{
    mouse_event mouse { last_mouse };
    if (mouse.changed()) {
        call_handlers<mouse_event>(mouse);
        last_mouse = mouse;
    }

}

mouse_event::mouse_event() {
    state = SDL_GetMouseState(&position.x, &position.y);
    last_state = SDL_GetMouseState(&last_position.x, &last_position.y);
}

mouse_event::mouse_event(mouse_event &last) {
    state = SDL_GetMouseState(&position.x, &position.y);
    last_state = last.state;
    last_position = last.position;
}

bool mouse_event::changed() const {
    return state != last_state || moved();
}

bool mouse_event::moved() const {
    return position.x != last_position.x || position.y != last_position.y;
}

bool mouse_event::left_clicked() const {
    return state & SDL_BUTTON_LMASK && !(last_state & SDL_BUTTON_LMASK);
}

bool mouse_event::right_clicked() const {
    return state & SDL_BUTTON_RMASK && !(last_state & SDL_BUTTON_RMASK);
}

bool mouse_event::left_pressed() const {
    return state & SDL_BUTTON_LMASK;
}

bool mouse_event::right_pressed() const {
    return state & SDL_BUTTON_RMASK;
}

bool mouse_event::left_released() const {
    return !(state & SDL_BUTTON_LMASK) && last_state & SDL_BUTTON_LMASK;
}

bool mouse_event::right_released() const {
    return !(state & SDL_BUTTON_RMASK) && last_state & SDL_BUTTON_RMASK;
}

}
