#include <cmath>
#include <iostream>

#include <SDL2/SDL_mouse.h>

#include "ai.h"
#include "engine.h"

idle ai_state::IDLE;
wander ai_state::WANDER;
go_to ai_state::GOTO;
find_food ai_state::FIND_FOOD;

void ai_state::enter(entity &target)
{
}

void ai_state::exit(entity &target)
{
}

void state_machine::push(entity &ent, ai_state &state)
{
    state.enter(ent);
    states.push(&state);
}

void state_machine::set(entity &ent, ai_state &state)
{
    if (!states.empty())
        pop(ent);
    push(ent, state);
}

void state_machine::pop(entity &ent)
{
    ai_state *top = states.top();
    states.pop();
    top->exit(ent);
}

void state_machine::tick(entity &ent, double dt)
{
    ai_state *top = states.empty() ? &ai_state::IDLE : states.top();
    top->tick(ent, dt);
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
    if (std::abs(target.goal - target.pos()) < GOTO_THRESH) {
        target.velocity /= 2;
        target.ai.pop(target);
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

void find_food::tick(entity &target, double dt)
{
    std::random_device rand;
    std::mt19937 mt(rand());

    auto &e = engine::get();

    std::complex<double> sd = e.camera.pixel_to_coord({e.screen_width, e.screen_height});

    std::complex<double> min = e.camera.pos() - sd / 2.;
    std::complex<double> max = e.camera.pos() + sd / 2.;

    std::uniform_real_distribution<double> distx(min.real(), max.real());
    std::uniform_real_distribution<double> disty(min.imag(), max.imag());

    target.goal = std::complex<double>(distx(mt), disty(mt));

    std::cout << "going to " << target.goal << std::endl;

    target.ai.push(target, ai_state::GOTO);
}
