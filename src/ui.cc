#include <cmath>
#include <complex>
#include <iostream>

#include <SDL2/SDL_mouse.h>

#include "colors.h"
#include "engine.h"
#include "textures.h"
#include "ui.h"

#define GROUP_TILE_WIDTH 25
#define GROUP_TILE_MARGIN 20
#define STAT_MARGIN 20
#define STAT_COL_WIDTH 80
#define STAT_PANEL (2 * STAT_COL_WIDTH + 3 * STAT_MARGIN)
#define STAT_PANEL_VELOCITY_THRESH 30

ui_element::ui_element(int x, int y, int w, int h)
    : SDL_Rect({x, y, w, h})
    , parent()
    , elements()
{}

void ui_element::tick(double dt)
{
    for (auto &d : elements)
        d->tick(dt);
}

void ui_element::draw(SDL_Renderer *renderer)
{
    for (auto &d : elements)
        d->draw(renderer);
}

void ui_element::add(std::shared_ptr<ui_element> &&elt)
{
    elements.push_back(elt);
}

void ui_element::add(std::shared_ptr<ui_element> &elt)
{
    elements.push_back(elt);
}

void ui_element::close()
{
    elements.clear();
}

ui::ui()
    : ui_element(0, 0, 0, 0)
{
}

void ui::tick(double dt)
{
    ui_element::tick(dt);

    level &level = engine::get().level;

    Uint32 state = SDL_GetMouseState(NULL, NULL);
    if (!(state & SDL_BUTTON_LMASK)) {
        double a = level.select_area_color.a - 1000. * dt;
        if (a < 0)
            a = 0;
        level.select_area_color.a = Uint8(a);
    }
}

void ui::draw(SDL_Renderer *renderer)
{
    level &level = engine::get().level;

    {
        SDL_Point a = { level.select_area.x, level.select_area.y };
        SDL_Point b = {
            level.select_area.x + level.select_area.w,
            level.select_area.y + level.select_area.h
        };
        SDL_Color &c = level.select_area_color;
        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        SDL_RenderDrawLine(renderer, a.x, a.y, a.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, a.y, b.x, a.y);
        SDL_RenderDrawLine(renderer, b.x, a.y, b.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, b.y, b.x, b.y);
    }

    {
        SDL_Point a = { level.create_area.x, level.create_area.y };
        SDL_Point b = {
            level.create_area.x + level.create_area.w,
            level.create_area.y + level.create_area.h
        };

        SDL_Color c = colors::white;
        auto s = level.selected.begin();
        if (s != level.selected.end()) {
            c = (*s)->group->color;
        }

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 25);
        SDL_Rect r = { std::min(a.x, b.x), std::min(a.y, b.y),
                       std::abs(a.x - b.x), std::abs(a.y - b.y) };
        SDL_RenderFillRect(renderer, &r);

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
        SDL_RenderDrawLine(renderer, a.x, a.y, a.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, a.y, b.x, a.y);
        SDL_RenderDrawLine(renderer, b.x, a.y, b.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, b.y, b.x, b.y);
    }

    ui_element::draw(renderer);

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x -= textures::crosshair->w / 2;
    mouse.y -= textures::crosshair->h / 2;
    textures::crosshair->draw(renderer, mouse);
}

void ui::close()
{
}

colorpick::colorpick(int x, int y, int w, int h)
    : ui_element(x, y, w, h)
{
    SDL_Renderer *renderer = engine::get().renderer;

    /* texture is a circle with a diameter of w */
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, w);
    if (texture == nullptr)
        throw std::invalid_argument(SDL_GetError());

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    SDL_Texture *old = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderSetLogicalSize(renderer, w, w);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderClear(renderer);

    double twopi = 2. * std::acos(-1.);
    SDL_Point center = { w / 2, h / 2 };
    for (int py = 0; py < h; ++py) {
        for (int px = 0; px < w; ++px) {
            double rsq = std::pow(px - center.x, 2) + std::pow(py - center.y, 2);
            if (rsq > (w*w) / 4. || rsq < (50*50))
                continue;

            std::complex<double> p(px, py);
            std::complex<double> c(center.x, center.y);
            double hue = std::fmod(std::floor(std::arg(p - c) / twopi * 360.) + 360., 360.);

            int r, g, b;
            colors::hsv_to_rgb(hue, 0.85, 0.8, r, g, b);

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }

    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, old);
}

void colorpick::tick(double dt)
{
}

void colorpick::draw(SDL_Renderer *renderer)
{
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}

group_picker::group_picker()
    : ui_element(0, 0, 0, 0)
{
    using namespace event;

    auto &em = engine::get().event_manager;
    em.register_handler<mouse_event>(handle_mouse, priorities::HIGH);
}

static void group_picker_boundaries(int offset, SDL_Point &a, SDL_Point &b)
{
    a = {
        engine::get().screen_width - offset * (GROUP_TILE_WIDTH + GROUP_TILE_MARGIN),
        GROUP_TILE_MARGIN
    };
    b = {
        engine::get().screen_width - (offset - 1) * (GROUP_TILE_WIDTH + GROUP_TILE_MARGIN) - GROUP_TILE_MARGIN,
        GROUP_TILE_MARGIN + GROUP_TILE_WIDTH
    };
}

bool group_picker::handle_mouse(event::mouse_event &ev)
{
    if (ev.left_released()) {
        level &level = engine::get().level;

        int offset = 1;
        for (auto &g : level.pop.groups) {
            SDL_Point a, b;
            group_picker_boundaries(offset, a, b);

            if (ev.position.x >= a.x && ev.position.x <= b.x
                    && ev.position.y >= a.y && ev.position.y <= b.y) {

                if (level.selected.empty()) {
                    for (auto &s : g.members) {
                        level.select(s);
                    }
                } else {
                    for (auto &e : level.selected) {
                        g.add(e);
                    }
                }
                return true;
            }
            ++offset;
        }
    }
    return false;
}

void group_picker::draw(SDL_Renderer *renderer)
{
    level &level = engine::get().level;

    int offset = 1;
    for (auto &g : level.pop.groups) {
        SDL_Point a, b;
        group_picker_boundaries(offset, a, b);

        SDL_SetRenderDrawColor(renderer, g.color.r, g.color.g, g.color.b, 255);
        SDL_Rect fill = { a.x, a.y, b.x - a.x, b.y - a.y };
        SDL_RenderFillRect(renderer, &fill);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, a.x, a.y, a.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, a.y, b.x, a.y);
        SDL_RenderDrawLine(renderer, b.x, a.y, b.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, b.y, b.x, b.y);
        ++offset;
    }
}

stat_panel::stat_panel()
    : ui_element(0, 0, STAT_PANEL, 0)
{}

void stat_panel::tick(double dt)
{
    const double acceleration = 10.;
    if (engine::get().level.selected.empty()) {
        if (x < STAT_PANEL_VELOCITY_THRESH && x > 0)
            transvelocity -= acceleration * dt;
        else if (x > 0)
            transvelocity = 1000.;
        else
            transvelocity = 0;
        x -= transvelocity * dt;
    } else if (x < w) {
        if (w - x < STAT_PANEL_VELOCITY_THRESH)
            transvelocity -= acceleration * dt;
        else if (w - x > 0)
            transvelocity = 1000.;
        else
            transvelocity = 0;
        x += transvelocity * dt;
    }
}

static int draw_ratioed_icon(SDL_Renderer *renderer, texture &full, texture &contour, SDL_Point p, double ratio)
{
    //p.y += (STAT_PANEL / rescale - full.h) / 2;

    int full_y = full.h * (ratio);
    SDL_Rect perc = { 0, full.h - full_y, full.w, full_y };
    SDL_Point point = { int(p.x), int(p.y + (full.h - full_y)) };
    full.draw(renderer, point, &perc);

    point = { int(p.x), int(p.y) };
    contour.draw(renderer, point);

    return p.y + full.h + STAT_MARGIN;
}

static int draw_heart(SDL_Renderer *renderer, int x, int y, double mean_health, double mean_max_health)
{
    SDL_Point p = { x, y };
    return draw_ratioed_icon(renderer,
            *textures::trait_major_heart_full,
            *textures::trait_major_heart,
            p, mean_health / mean_max_health);
}

static int draw_hourglass(SDL_Renderer *renderer, int x, int y, double mean_age, double mean_max_age)
{
    SDL_Point p = { x, y };
    return draw_ratioed_icon(renderer,
            *textures::trait_major_hourglass_full,
            *textures::trait_major_hourglass,
            p, 1 - mean_age / mean_max_age);
}

void stat_panel::draw(SDL_Renderer *renderer)
{
    auto &engine = engine::get();
    int panel_x = x;
    SDL_Rect rect = {0, 0, panel_x, engine.screen_height};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    double mean_age = 0;
    double mean_max_age = 0;

    for (auto &s : engine.level.selected) {
        mean_age        += s->age;
        mean_max_age    += s->attr.max_age;
    }

    mean_age     /= (double) engine.level.selected.size();
    mean_max_age /= (double) engine.level.selected.size();

    {
        int y = STAT_MARGIN, x = panel_x - w + STAT_MARGIN;
        y += draw_hourglass(renderer, x, y, mean_age, mean_max_age);
    }

    {
        int y = STAT_MARGIN, x = panel_x - w + STAT_COL_WIDTH + 2 * STAT_MARGIN;
        y += draw_heart(renderer, x, y, 100, 100);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, panel_x, 0, panel_x, engine.screen_height);
}
