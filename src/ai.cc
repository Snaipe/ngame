#include <cmath>
#include <iostream>

#include <SDL2/SDL_mouse.h>

#include "ai.h"
#include "engine.h"

idle ai_state::IDLE;
wander ai_state::WANDER;
go_to ai_state::GOTO;

void ai_state::enter(entity &target)
{
}

void ai_state::exit(entity &target)
{
}

void idle::tick(entity &target, double dt)
{
}

wander::wander()
    : rand()
{}

void wander::enter(entity &target)
{
    double pi = std::acos(-1.);
    mt = std::mt19937(rand());
    dist = std::uniform_real_distribution<double>(-pi / 3, pi / 3);
}

void wander::tick(entity &target, double dt)
{
    std::complex<double> one = 1i;
    double pi = std::acos(-1.);

    double theta = dist(mt);
    if (theta < -pi / 6)
        theta = -pi / 24;
    else if (theta > pi / 6)
        theta = pi / 24;
    else
        return;
    double origth = std::arg(target.velocity);

    target.velocity = target.attr.speed * std::exp(one * (origth + theta));
}

#define GOTO_THRESH 100
#define GOTO_MAX_TH (std::acos(-1.) / 12)
#define GOTO_MIN_TH -GOTO_MAX_TH

void go_to::tick(entity &target, double dt)
{
#ifndef NDEBUG
    if (engine::debug) {
        SDL_Point p;
        SDL_GetMouseState(&p.x, &p.y);
        target.goal = engine::get().camera.pixel_to_coord(p);
        SDL_SetRenderDrawColor(engine::get().renderer, 0, 255, 255, 255);
        SDL_RenderDrawPoint(engine::get().renderer, p.x, p.y);
    }
#endif

    if (std::abs(target.goal - target.pos()) < GOTO_THRESH) {
        target.velocity /= 2;
        return;
    }

    double pi = std::acos(-1.);
    double goalth = std::arg(target.goal - target.pos());

    std::complex<double> velocity = std::abs(target.velocity) * std::exp((std::arg(target.velocity) - pi / 2) * std::complex<double>(1.i));

    double origth = std::arg(velocity);
    double diffth = std::fmod((velocity * std::conj(target.goal - target.pos())).real(), 2. * pi); 

#ifndef NDEBUG
    if (engine::debug) {
        SDL_Point p = engine::get().camera.coord_to_pixel(target.pos());
        SDL_Point p2 = engine::get().camera.coord_to_pixel(target.pos() + 100. * std::exp(std::complex<double>(1i) * origth));
        SDL_RenderDrawLine(engine::get().renderer, p.x, p.y, p2.x, p2.y);

        p2 = engine::get().camera.coord_to_pixel(target.pos() + 100. * std::exp(std::complex<double>(1i) * goalth));
        SDL_RenderDrawLine(engine::get().renderer, p.x, p.y, p2.x, p2.y);
    }
#endif

    double theta = std::min(std::max(diffth, GOTO_MIN_TH), GOTO_MAX_TH);

    target.velocity = target.attr.speed * std::exp(std::complex<double>(1i) * (origth - theta + pi / 2.));
}
