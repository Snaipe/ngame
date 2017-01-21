#include <vector>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <limits>
#include "metaball.h"
#include "engine.h"

#define DRAW_THRESH 250
#define DRAW_STEPS 20
#define DRAW_LOWER_THRESH 3
#define CORRECT_LARGE 100000
#define STEP_DELTA 0.1

void metaballs::recompute_size()
{
    decltype(balls)::iterator minSz, maxSz;
    std::tie(minSz, maxSz) = std::minmax_element(begin(balls), end(balls),
            [](auto &lhs, auto &rhs) { return lhs.size < rhs.size; });

    if (minSz != end(balls))
        this->minSz = minSz->size;
}

metaballs::metaballs(double visc, double thresh)
    : balls()
    , viscosity(visc)
    , threshold(thresh)
{
    recompute_size();
}

void metaballs::add_ball(metaball mb)
{
    balls.push_back(mb);
    recompute_size();
}

std::complex<double> metaballs::find_border(std::complex<double> pos)
{
    double force = std::numeric_limits<double>::max();

    while (force > threshold) {
        force = correct_edge(pos);

#ifndef NDEBUG
        if (engine::debug) {
            SDL_SetRenderDrawColor(engine::get().renderer, 255, 255, 0, 255);
            SDL_Point p = engine::get().camera.coord_to_pixel(pos);
            SDL_RenderDrawPoint(engine::get().renderer, p.x, p.y);
        }
#endif
    }
    return pos;
}

double metaballs::correct_edge(std::complex<double> &pos)
{
    double force = 0;
    for (auto &ball : balls) {
        double ratio = std::pow(std::abs(ball - pos), viscosity);
        force += ratio != 0 ? ball.size / ratio : CORRECT_LARGE;
    }
    std::complex<double> normal = this->normal(pos);

    double step = std::pow(minSz / threshold, 1 / viscosity)
                - std::pow(minSz / force,     1 / viscosity)
                + STEP_DELTA;
    pos += normal * step;
    return force;
}

std::complex<double> metaballs::normal(const std::complex<double> &pos)
{
    std::complex<double> np = 0;
    for (auto &ball : balls) {
        double ratio = std::pow(std::abs(ball - pos), viscosity + 2);
        np += -viscosity * ball.size * (ball - pos) / ratio;
    }
    return np / std::abs(np);
}

std::complex<double> metaballs::tangent(const std::complex<double> &pos)
{
    std::complex<double> normal = this->normal(pos);
    return std::complex<double>(-normal.imag(), normal.real());
}

struct ballcontext {
    std::complex<double> initp;
    std::complex<double> edgep;
    metaball &ball;
    bool done;
};

void metaballs::draw(SDL_Renderer *renderer)
{
    using namespace std::complex_literals;

    std::complex<double> one = 1i;
    std::vector<ballcontext> ctx;
    for (auto &ball : balls) {
        std::complex<double> edge = find_border(ball + one);
        ctx.push_back({ edge, edge, ball, false });

#ifndef NDEBUG
        if (engine::debug) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Point p = engine::get().camera.coord_to_pixel(ball);
            SDL_RenderDrawPoint(renderer, p.x, p.y);
        }
#endif
    }

    double step = DRAW_STEPS;
    for (size_t until = 0; until < DRAW_THRESH; ++until) {
        for (auto &ballctx : ctx) {
            if (ballctx.done)
                continue;
            std::complex<double> prev = ballctx.edgep;

            // rk2: pos + h * f(pos + f(pos) * h / 2)
            ballctx.edgep += step * tangent(ballctx.edgep + tangent(ballctx.edgep) * step / 2.);
            correct_edge(ballctx.edgep);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_Point pprev = engine::get().camera.coord_to_pixel(prev);
            SDL_Point pnext = engine::get().camera.coord_to_pixel(ballctx.edgep);
            SDL_RenderDrawLine(renderer, pprev.x, pprev.y, pnext.x, pnext.y);

            // mark balls done when gone full circle
            for (auto &octx : ctx) {
                if ((octx.ball != ballctx.ball || until > DRAW_LOWER_THRESH)
                        && std::abs(octx.initp - ballctx.edgep) <= step) {
                    ballctx.done = true;
                }
            }
        }
        bool all_done = true;
        for (auto &ballctx : ctx) {
            if (!ballctx.done) {
                all_done = false;
                break;
            }
        }
        if (all_done)
            break;
    }
}

std::vector<metaball> &metaballs::getballs()
{
    return balls;
}
