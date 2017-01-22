#include <SDL2/SDL_image.h>

#include "textures.h"
#include "engine.h"

texture *textures::crosshair;

texture::texture(const char *path)
{
    sdl_texture = IMG_LoadTexture(engine::get().renderer, path);
    if (sdl_texture == nullptr)
        throw std::invalid_argument(SDL_GetError());

    SDL_QueryTexture(sdl_texture, NULL, NULL, &w, &h);
}

void texture::draw(SDL_Renderer *renderer, SDL_Point &p)
{
    SDL_Rect dst = { p.x, p.y, w, h };
    SDL_RenderCopy(renderer, sdl_texture, NULL, &dst);
}

void textures::init()
{
    textures::crosshair = new texture("resources/crosshair.png");
}
