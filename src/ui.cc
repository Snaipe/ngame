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
    add(std::make_shared<group_picker>());
    add(std::make_shared<stat_panel>());
}

void ui::draw(SDL_Renderer *renderer)
{
    ui_element::draw(renderer);

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x -= textures::crosshair->w / 2;
    mouse.y -= textures::crosshair->h / 2;
    textures::crosshair->draw(renderer, mouse);

    level &level = engine::get().level;
    if (level.select_area.x != -1 || level.select_area.y != -1) {
        SDL_Point a = { level.select_area.x, level.select_area.y };
        SDL_Point b = {
            level.select_area.x + level.select_area.w,
            level.select_area.y + level.select_area.h
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, a.x, a.y, a.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, a.y, b.x, a.y);
        SDL_RenderDrawLine(renderer, b.x, a.y, b.x, b.y);
        SDL_RenderDrawLine(renderer, a.x, b.y, b.x, b.y);
    }
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
{}

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

void group_picker::tick(double dt)
{
    static Uint32 laststate;

    SDL_Point mouse;
    Uint32 state = SDL_GetMouseState(&mouse.x, &mouse.y);

    if ((laststate & SDL_BUTTON_LMASK) & !(state & SDL_BUTTON_LMASK)) {
        std::cout << "clicked @ " << mouse.x << ", " << mouse.y << std::endl;
        level &level = engine::get().level;

        int offset = 1;
        for (auto &g : level.pop.groups) {
            SDL_Point a, b;
            group_picker_boundaries(offset, a, b);

            if (mouse.x >= a.x && mouse.x <= b.x
                    && mouse.y >= a.y && mouse.y <= b.y) {

                std::cout << "clicked in picker" << std::endl;
                for (auto &e : level.selected) {
                    g.add(e);
                }
            }
            ++offset;
        }
    }

    laststate = state;
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
