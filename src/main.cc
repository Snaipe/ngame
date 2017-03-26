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
#include "ui-title.h"

#define SCREEN_TRANSITION_SPEED 1e-10

static engine *singleton;

engine &engine::get() {
    return *singleton;
}

engine::engine()
    : framerate(1./60.)
    , screen_width(800)
    , screen_height(600)
    , fullscreen(0)
    , paused(false)
    , screen(nullptr)
    , next_screen(nullptr)
    , screen_transition(0)
{
    window = SDL_CreateWindow("Game",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            screen_width, screen_height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
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
    SDL_SetWindowFullscreen(window, 0);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

#ifndef NDEBUG
bool engine::debug = false;
#endif

void engine::tick(double dt)
{
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (screen) {
        screen->draw(renderer);
        screen->tick(dt);
    }

    camera.tick(dt);

    SDL_RenderPresent(renderer);
}

void engine::start()
{
    using namespace std::complex_literals;

    textures::init();

    level.add_group({ 0, 252, 133, 255 });
    level.add_group({ 247, 11, 119, 255 });
    level.add_group({ 255, 201, 20, 255 });
    level.add_group({ 25, 76, 142, 255 });
    level.add_group({ 147, 69, 243, 255 });
    level.add_group({ 255, 69, 62, 255 });

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RaiseWindow(window);

    screen = new screen::title();

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
