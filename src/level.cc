#include <iostream>

#include "ai.h"
#include "engine.h"
#include "level.h"

entity::entity(std::complex<double> pos)
    : mb(2, 0.00004, metaball(pos, 2), metaball(pos, 1.5))
{
    for (size_t i = 0; i < 2 / engine::get().framerate; ++i)
        lastpos.push(pos);

    curstate = &ai_state::GOTO;
    curstate->enter(*this);
}

void entity::tick(double dt)
{
    curstate->tick(*this, dt);

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
    mb.draw(renderer);
}

bgelement::bgelement(SDL_Texture *tex_, std::complex<double> pos_)
    : tex(tex_)
    , pos(pos_)
{}

void bgelement::draw(SDL_Renderer *renderer)
{
    SDL_Rect dst = {int(pos.real()), int(pos.imag()), 0, 0};
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(renderer, tex, NULL, &dst);
}

level::level()
{
}

void level::draw(SDL_Renderer *renderer)
{
    for (auto &e : bgelements) {
        e.draw(renderer);
    }
    for (auto &e : entities) {
        e.draw(renderer);
    }
}

void level::tick(double dt)
{
    for (auto &e : entities) {
        e.tick(dt);
    }
}

void level::add_entity(entity e)
{
    entities.push_back(e);
}

void level::add_bgelement(bgelement e)
{
    bgelements.push_back(e);
}
