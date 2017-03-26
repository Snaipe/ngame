#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>

#include "engine.h"
#include "event.h"

namespace event {

void manager::schedule(task fn, double seconds)
{
    scheduled.push_back(std::make_pair(fn, seconds));
}

void manager::tick(double dt)
{
    engine &e = engine::get();
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_MOUSEWHEEL:
                if (ev.wheel.y > 0)
                    e.camera.zoom_by(.1);
                else if (ev.wheel.y < 0)
                    e.camera.zoom_by(-.1);
                break;
            case SDL_KEYDOWN:
#ifndef NDEBUG
                if (ev.key.keysym.sym == SDLK_F1)
                    engine::debug = !engine::debug;
#endif
                if (ev.key.keysym.sym == SDLK_F11) {
                    e.fullscreen ^= SDL_WINDOW_FULLSCREEN;
                    SDL_SetWindowFullscreen(e.window, e.fullscreen);
                }
                if (ev.key.keysym.sym == SDLK_SPACE)
                    e.paused = !e.paused;
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    throw quit();
                break;
            case SDL_WINDOWEVENT:
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    e.screen_width  = ev.window.data1;
                    e.screen_height = ev.window.data2;
                }
                break;
            case SDL_QUIT:
                throw quit();
            default: break;
        }
    }

    mouse_event mouse { last_mouse };
    if (mouse.changed()) {
        call_handlers<mouse_event>(mouse);
        last_mouse = mouse;
    }

    for (auto it = scheduled.begin(); it != scheduled.end(); ) {
        it->second -= dt;
        if (it->second <= 0) {
            it->first();
            it = scheduled.erase(it);
        } else {
            ++it;
        }
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
