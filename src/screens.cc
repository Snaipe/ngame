#include <memory>

#include "engine.h"
#include "screens.h"
#include "ui-title.h"
#include "ui.h"
#include "colors.h"
#include "textures.h"

namespace screen {

void screen::tick(double dt)
{
    event_manager.tick(dt);
    ui.tick(dt);
    engine::get().level.tick(dt);
}

void screen::draw(SDL_Renderer *renderer)
{
    engine::get().level.draw(renderer);
    ui.draw(renderer);
}

void level::tick(double dt)
{
    screen::tick(dt);
}

void level::draw(SDL_Renderer *renderer)
{
    screen::draw(renderer);
}

level::level()
{
    using namespace event;

    event_manager.register_handler<mouse_event>([](auto &ev) {
            return engine::get().level.handle_mouse(ev);
        }, priorities::NORMAL);

    event_manager.register_handler<mouse_event>(ui::group_picker::handle_mouse, priorities::HIGH);
}

void intro::tick(double dt)
{
    auto &e = engine::get();

    screen::tick(dt);
    if (phase != next_phase) {
        display_click_fade -= dt * 2;
        if (display_click_fade <= 0) {
            phase = next_phase;
        }
        display_click_fade = std::max(display_click_fade, 0.);
    } else {
        display_click_fade += dt;
        display_click_fade = std::min(display_click_fade, .5);

        if (phase == intro_phase::SELECT) {
            if (!e.level.selected.empty()) {
                next_phase = intro_phase::ZONE;

                std::complex<double> np = e.camera.pixel_to_coord({
                        e.screen_width,
                        e.screen_height
                    });

                std::random_device rand;
                std::mt19937 mt(rand());
                double pi = std::acos(-1);
                std::uniform_real_distribution<double> dist(-pi, pi);

                for (int i = 0; i < 2; ++i) {
                    double ang = dist(mt);

                    np = std::abs(np) * std::exp(std::complex<double>(0, 1) * ang);

                    auto ent = std::make_shared<entity>(np);
                    ent->ai.push(*ent, ai_state::FIND_FOOD);
                    e.level.pop.groups[0].add(ent);
                    e.level.add_entity(ent);
                }
                e.camera.lock = false;
                e.camera.zoom_to(0.1);
                e.camera.lock = true;
            }
        } else if (phase == intro_phase::ZONE) {
            if (e.level.create_area.w > 0 || e.level.create_area.h > 0) {
                next_phase = intro_phase::INIT;
            }
        }
    }
}

void intro::draw(SDL_Renderer *renderer)
{
    auto &e = engine::get();

    texture *tex = nullptr;

    switch (phase) {
        case intro_phase::SELECT:
            tex = textures::tuto_select; break;
        case intro_phase::ZONE:
            tex = textures::tuto_drag; break;
        default: break;
    }

    if (tex) {
        SDL_Color c { 255, 255, 255, Uint8(255 * display_click_fade) };

        float scale = e.camera.zoom() * 10;
        SDL_Point at = {
            int((e.screen_width  - tex->w * scale) / 2.f),
            int((e.screen_height - tex->h * scale) / 2.f) + 20
        };
        tex->draw(renderer, at, nullptr, scale, &c);
    }
    screen::draw(renderer);
}

intro::intro()
    : display_click_fade(0)
{
    using namespace event;

    auto &e = engine::get();
    e.camera.zoom_to(0.3);
    e.camera.follow(nullptr);
    e.camera.lock = true;

    for (auto &ent : e.level.pop.entities) {
        ent->age = 0;
        ent->ai.push(*ent, ai_state::FIND_FOOD);
    }

    event_manager.register_handler<mouse_event>([](auto &ev) {
            return engine::get().level.handle_mouse(ev);
        }, priorities::NORMAL);

    event_manager.register_handler<mouse_event>(ui::group_picker::handle_mouse, priorities::HIGH);

    event_manager.schedule([this]() {
            next_phase = SELECT;
        }, 2);

    ui.add(new ui::group_picker());
    ui.add(new ui::stat_panel());
}

void title::tick(double dt)
{
    screen::tick(dt);
    if (display_click) {
        display_click_fade += dt;
        display_click_fade = std::min(display_click_fade, 1.);
    }
}

void title::draw(SDL_Renderer *renderer)
{
    screen::draw(renderer);
    if (display_click) {
        auto &e = engine::get();
        SDL_Color c { 255, 255, 255, Uint8(255 * display_click_fade) };
        colors::darken(c, 20);

        float scale = 1;
        SDL_Point at = {
            int((e.screen_width  - textures::tuto_click->w * scale) / 2.f),
            int((e.screen_height - textures::tuto_click->h * scale) / 2.f) + 20
        };
        textures::tuto_click->draw(renderer, at, nullptr, scale, &c);
    }
}

title::title()
    : display_click(false)
    , display_click_fade(0)
{
    auto &e = engine::get();
    e.camera.zoom(1e-10);
    e.camera.zoom_to(1);

    auto ent = std::make_shared<entity>(0);
    e.level.add_entity(ent);

    //ent->ai.push(*ent, ai_state::IDLE);
    ent->age = -std::numeric_limits<decltype(ent->age)>::infinity();

    e.camera.follow(ent);

    for (auto &ent : e.level.pop.entities)
        e.level.pop.groups[0].add(ent);

    auto title_text = new ui::title();
    ui.add(title_text);

    using namespace event;

    event_manager.register_handler<mouse_event>([](mouse_event &ev) {
            if (ev.left_released()) {
                engine::get().screen = new intro();
            }
            return true;
        }, priorities::HIGHEST);

    event_manager.schedule([this]() {
            display_click = true;
        }, 5);
}

}
