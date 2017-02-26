#include <iostream>
#include <exception>
#include <deque>

#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>

#include "engine.h"
#include "metaball.h"
#include "textures.h"

class quit {};

static engine *singleton;

engine &engine::get() {
    return *singleton;
}

engine::engine()
    : framerate(1./60.)
{
    window = SDL_CreateWindow("Game",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            1600, 900, SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
        throw std::invalid_argument(SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        throw std::invalid_argument(SDL_GetError());
    }
}

engine::~engine()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

#ifndef NDEBUG
bool engine::debug = true;
#endif

void engine::tick(double dt)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_MOUSEWHEEL:
                if (ev.wheel.y > 0)
                    camera.zoom *= 1.1;
                else if (ev.wheel.y < 0)
                    camera.zoom /= 1.1;
                break;
            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_SPACE)
                    paused = !paused;
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    throw quit();
                break;
            case SDL_WINDOWEVENT:
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    screen_width  = ev.window.data1;
                    screen_height = ev.window.data2;
                }
                break;
            case SDL_QUIT:
                throw quit();
            default: break;
        }
    }

    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    level.draw(renderer);

    ui.draw(renderer);

    ui.tick(dt);
    level.tick(dt);

    SDL_RenderPresent(renderer);
}

void engine::start()
{
    using namespace std::complex_literals;

    level.add_group({ 0, 252, 133, 255 });
    level.add_group({ 247, 11, 119, 255 });
    level.add_group({ 255, 201, 20, 255 });
    level.add_group({ 25, 76, 142, 255 });
    level.add_group({ 147, 69, 243, 255 });
    level.add_group({ 255, 69, 62, 255 });

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    camera.zoom = 0.1;
    textures::init();

    level.add_entity(std::make_shared<entity>((300 + 400i) / camera.zoom));
    level.add_entity(std::make_shared<entity>((400 + 500i) / camera.zoom));
    level.add_entity(std::make_shared<entity>((200 + 200i) / camera.zoom));
    level.add_entity(std::make_shared<entity>((100 + 100i) / camera.zoom));
    level.add_entity(std::make_shared<entity>((500 + 600i) / camera.zoom));
    level.add_entity(std::make_shared<entity>((600 + 500i) / camera.zoom));

    for (auto &e : level.pop.entities)
        level.pop.groups[0].add(e);

    SDL_RaiseWindow(window);

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
    IMG_Init(IMG_INIT_PNG);
    SDL_ShowCursor(SDL_DISABLE);

    try {
        engine e;
        singleton = &e;

        e.start();
    } catch (quit &q) {
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    SDL_ShowCursor(SDL_ENABLE);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
