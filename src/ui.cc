#include <SDL2/SDL_mouse.h>

#include "textures.h"
#include "ui.h"

void ui::tick(double dt)
{
}

void ui::draw(SDL_Renderer *renderer)
{
    for (auto &d : elements)
        d->draw(renderer);

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x -= textures::crosshair->w / 2;
    mouse.y -= textures::crosshair->h / 2;
    textures::crosshair->draw(renderer, mouse);
}
