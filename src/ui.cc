#include <cmath>
#include <complex>
#include <iostream>

#include <SDL2/SDL_mouse.h>

#include "engine.h"
#include "textures.h"
#include "ui.h"

#define GROUP_TILE_WIDTH 25
#define GROUP_TILE_MARGIN 20

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

static void HSVtoRGB(double h, double s, double v, int &r, int &g, int &b) {
    double chroma = v * s;

    h = std::fmod(h / 60.0, 6);
    double fX = chroma * (1 - fabs(fmod(h, 2) - 1));
    double fM = v - chroma;

    double fR = 0, fG = 0, fB = 0;
    if( 0 <= h && h < 1) {
        fR = chroma;
        fG = fX;
    } else if (1 <= h && h < 2) {
        fR = fX;
        fG = chroma;
    } else if (2 <= h && h < 3) {
        fG = chroma;
        fB = fX;
    } else if (3 <= h && h < 4) {
        fG = fX;
        fB = chroma;
    } else if (4 <= h && h < 5) {
        fR = fX;
        fB = chroma;
    } else if (5 <= h && h < 6) {
        fR = chroma;
        fB = fX;
    }

    r = int((fR + fM) * 255);
    g = int((fG + fM) * 255);
    b = int((fB + fM) * 255);
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
            HSVtoRGB(hue, 0.85, 0.8, r, g, b);

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
