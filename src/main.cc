#include <iostream>
#include <exception>
#include <deque>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "metaball.h"

struct engine {
    engine();
    ~engine();

    void tick(double dt);
    void start();

    SDL_Window *window;
    SDL_Renderer *renderer;

    metaballs mb;
};

class quit {};

engine::engine()
    : mb(2, 0.00004)
{
    window = SDL_CreateWindow("Game",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1600, 900, SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
        throw std::invalid_argument(SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
        throw std::invalid_argument(SDL_GetError());
}

engine::~engine()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

std::deque<std::complex<double>> lastpos;

void engine::tick(double dt)
{
    std::cout << dt << std::endl;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        std::cout << ev.type << std::endl;
        switch (ev.type) {
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    throw quit();
                break;
            case SDL_QUIT:
                throw quit();
            default: break;
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int x, y;
    SDL_GetMouseState(&x, &y);
    std::complex<double> pos = x + y * 1i;
    lastpos.push_back(pos);

    mb.getballs()[1].real(pos.real());
    mb.getballs()[1].imag(pos.imag());
    mb.getballs()[0].real(lastpos.front().real());
    mb.getballs()[0].imag(lastpos.front().imag());
    lastpos.pop_front();

    mb.draw(renderer);

    SDL_RenderPresent(renderer);
}

void engine::start()
{
    int framerate = 60;
    Uint64 last  = SDL_GetPerformanceCounter();

    mb.add_ball(metaball(0, 2));
    mb.add_ball(metaball(0, 1));

    SDL_PumpEvents();

    int x, y;
    SDL_GetMouseState(&x, &y);
    std::complex<double> pos = x + y * 1i;

    for (size_t i = 0; i < framerate / 10; ++i)
        lastpos.push_back(pos);

    for (;;) {
        Uint64 freq = SDL_GetPerformanceFrequency();
        Uint64 now  = SDL_GetPerformanceCounter();
        double dt = (now - last) / (double) freq;

        tick(dt);

        double wait = framerate - dt * 1000;
        if (wait > 0)
            SDL_Delay(wait);

        last = now;
    }
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    try {
        engine e;

        e.start();
    } catch (quit &q) {
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    SDL_Quit();
    return 0;
}
