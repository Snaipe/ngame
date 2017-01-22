#include <iostream>

#include <SDL2/SDL_mouse.h>

#include "ai.h"
#include "engine.h"
#include "level.h"

#define SELECT_THRESHOLD 10
#define PICK_THRESHOLD 30

group_type::group_type(SDL_Color c)
    : color(c)
{}

void group_type::add(std::shared_ptr<entity> e)
{
    members.insert(e);
    e->color = color;
    if (e->group)
        e->group->remove(e);
    e->group = this;
}

void group_type::remove(std::shared_ptr<entity> e)
{
    members.erase(e);
    e->group = nullptr;
}

entity::entity(std::complex<double> pos)
    : group(nullptr)
    , ai()
    , mb(2, 0.00004, metaball(pos, 2), metaball(pos, 1.5))
{
    for (size_t i = 0; i < 2 / engine::get().framerate; ++i)
        lastpos.push(pos);

    ai.push(*this, ai_state::FIND_FOOD);
}

void entity::tick(double dt)
{
    ai.tick(*this, dt);

    std::complex<double> p = pos();

    // dp / dt = v -> p(n+1) - p(n) = v * dt
    p += velocity * dt;

    mb.getballs()[0].real(p.real());
    mb.getballs()[0].imag(p.imag());

    lastpos.push(p);

    p = lastpos.front();
    mb.getballs()[1].real(p.real());
    mb.getballs()[1].imag(p.imag());
    lastpos.pop();
}

std::complex<double> entity::pos()
{
    return mb.getballs()[0];
}

void entity::draw(SDL_Renderer *renderer)
{
    mb.draw(renderer, color);
}

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
    for (auto &e : entities) {
        e->draw(renderer);
    }
}

void level::tick(double dt)
{
    SDL_Point mouse;
    Uint32 state = SDL_GetMouseState(&mouse.x, &mouse.y);

    bool do_select = false, do_pick = false;
    if (state & SDL_BUTTON_LMASK) {
        if (select_area.x == -1 || select_area.y == -1) {
            select_area.x = mouse.x;
            select_area.y = mouse.y;
        } else {
            select_area.w = mouse.x - select_area.x;
            select_area.h = mouse.y - select_area.y;
        }
    } else if (select_area.x != -1 || select_area.y != -1) {
        deselect_all();
        select_area.w = mouse.x - select_area.x;
        select_area.h = mouse.y - select_area.y;
        if (std::abs(select_area.w * select_area.h) > SELECT_THRESHOLD)
            do_select = true;
        else
            do_pick = true;
    }

    for (auto &e : entities) {
        e->tick(dt);

        if (do_select) {
            SDL_Point p = engine::get().camera.coord_to_pixel(e->pos());
            if (((p.x >= select_area.x && p.x <= select_area.x + select_area.w)
              || (p.x <= select_area.x && p.x >= select_area.x + select_area.w))
             && ((p.y >= select_area.y && p.y <= select_area.y + select_area.h)
              || (p.y <= select_area.y && p.y >= select_area.y + select_area.h)))
            {
                select(e);
            }
        } else if (do_pick) {
            SDL_Point p = engine::get().camera.coord_to_pixel(e->pos());
            if (std::pow(p.x - mouse.x, 2) + std::pow(p.y - mouse.y, 2)
                    < PICK_THRESHOLD * PICK_THRESHOLD) {
                select(e);
            }
        }
    }

    if (!(state & SDL_BUTTON_LMASK)) {
        select_area.x = -1;
        select_area.y = -1;
        select_area.w = -1;
        select_area.h = -1;
    }
}

void level::add_entity(const std::shared_ptr<entity> &e)
{
    entities.push_back(e);
}

void level::add_entity(std::shared_ptr<entity> &&e)
{
    entities.push_back(e);
}

void level::add_bgelement(bgelement e)
{
    bgelements.push_back(e);
}

void level::add_group(SDL_Color c)
{
    groups.emplace_back(c);
}
