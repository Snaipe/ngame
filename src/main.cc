#include <iostream>
#include <exception>
#include <deque>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

struct engine {
    engine();
    ~engine();

    void tick(double dt);
    void start();

    SDL_Window *window;
    SDL_Renderer *renderer;
};

class quit {};

engine::engine()
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

    SDL_RenderPresent(renderer);
}

void engine::start()
{
    int framerate = 60;
    Uint64 last  = SDL_GetPerformanceCounter();

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
