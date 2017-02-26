#include <iostream>

#include <SDL2/SDL_mouse.h>

#include "ai.h"
#include "colors.h"
#include "engine.h"
#include "level.h"

#define SELECT_THRESHOLD 10
#define PICK_THRESHOLD 30

bgelement::bgelement(SDL_Texture *tex_, std::complex<double> pos_)
    : pos(pos_)
    , tex(tex_)
{}

void bgelement::draw(SDL_Renderer *renderer)
{
    SDL_Rect dst = {int(pos.real()), int(pos.imag()), 0, 0};
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(renderer, tex, NULL, &dst);
}

level::level()
    : select_area({ -1, -1, -1, -1 })
    , create_area({ -1, -1, -1, -1 })
{
}

void level::select(const std::shared_ptr<entity> &e)
{
    e->color = { 255, 255, 255, 255 };
    selected.insert(e);
}

void level::deselect_all()
{
    for (auto &e : selected) {
        e->color = e->group->color;
    }
    selected.clear();
}

void level::draw(SDL_Renderer *renderer)
{
    for (auto &e : bgelements) {
        e.draw(renderer);
    }
    for (auto &e : pop.entities) {
        e->draw(renderer);
    }
}

void level::init() {
    using namespace event;

    auto &em = engine::get().event_manager;
    em.register_handler<mouse_event>([this](auto &ev) { return this->handle_mouse(ev); }, priorities::NORMAL);
}

bool level::handle_mouse(event::mouse_event &ev)
{
    if (ev.left_pressed()) {
        if (ev.left_clicked()) {
            select_area_color = colors::white;
            select_area.x = ev.position.x;
            select_area.y = ev.position.y;
        }
        select_area.w = ev.position.x - select_area.x;
        select_area.h = ev.position.y - select_area.y;
        return true;
    }

    if (ev.left_released()) {
        deselect_all();
        select_area.w = ev.position.x - select_area.x;
        select_area.h = ev.position.y - select_area.y;
        if (std::abs(select_area.w * select_area.h) > SELECT_THRESHOLD) {
            for (auto &e : pop.entities) {
                SDL_Point p = engine::get().camera.coord_to_pixel(e->pos());
                if (((p.x >= select_area.x && p.x <= select_area.x + select_area.w)
                  || (p.x <= select_area.x && p.x >= select_area.x + select_area.w))
                 && ((p.y >= select_area.y && p.y <= select_area.y + select_area.h)
                  || (p.y <= select_area.y && p.y >= select_area.y + select_area.h)))
                {
                    select(e);
                }
            }
        } else {
            for (auto &e : pop.entities) {
                SDL_Point p = engine::get().camera.coord_to_pixel(e->pos());
                if (std::pow(p.x - ev.position.x, 2) + std::pow(p.y - ev.position.y, 2)
                        < PICK_THRESHOLD * PICK_THRESHOLD) {
                    select(e);
                }
            }
        }
        return true;
    }

    if (ev.right_pressed()) {
        if (ev.right_clicked()) {
            create_area.x = ev.position.x;
            create_area.y = ev.position.y;
        }
        create_area.w = ev.position.x - create_area.x;
        create_area.h = ev.position.y - create_area.y;
        return true;
    }

    if (ev.right_released()) {
        create_area.w = ev.position.x - create_area.x;
        create_area.h = ev.position.y - create_area.y;
        return true;
    }

    return false;
}

void level::tick(double dt)
{
    if (!engine::get().paused)
        pop.tick(dt);
}

void level::add_entity(const std::shared_ptr<entity> &e)
{
    pop.entities.insert(e);
}

void level::add_entity(std::shared_ptr<entity> &&e)
{
    pop.entities.insert(e);
}

void level::add_bgelement(bgelement e)
{
    bgelements.push_back(e);
}

void level::add_group(SDL_Color c)
{
    pop.groups.emplace_back(c);
}
