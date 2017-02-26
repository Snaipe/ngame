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

void level::tick(double dt)
{
    SDL_Point mouse;
    Uint32 state = SDL_GetMouseState(&mouse.x, &mouse.y);

    bool do_select = false, do_pick = false;
    if (state & SDL_BUTTON_LMASK) {
        if (!(laststate & SDL_BUTTON_LMASK)) {
            select_area_color = colors::white;
            select_area.x = mouse.x;
            select_area.y = mouse.y;
        }
        select_area.w = mouse.x - select_area.x;
        select_area.h = mouse.y - select_area.y;
    } else if (laststate & SDL_BUTTON_LMASK) {
        deselect_all();
        select_area.w = mouse.x - select_area.x;
        select_area.h = mouse.y - select_area.y;
        if (std::abs(select_area.w * select_area.h) > SELECT_THRESHOLD)
            do_select = true;
        else
            do_pick = true;
    } else if (state & SDL_BUTTON_RMASK) {
        if (!(laststate & SDL_BUTTON_RMASK)) {
            create_area.x = mouse.x;
            create_area.y = mouse.y;
        }
        create_area.w = mouse.x - create_area.x;
        create_area.h = mouse.y - create_area.y;
    } else if (laststate & SDL_BUTTON_RMASK) {
        create_area.w = mouse.x - create_area.x;
        create_area.h = mouse.y - create_area.y;
    }

    if (!engine::get().paused)
        pop.tick(dt);

    for (auto &e : pop.entities) {
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

    laststate = state;
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
