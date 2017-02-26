#include <SDL2/SDL_image.h>

#include "textures.h"
#include "engine.h"

texture *textures::crosshair;

texture *textures::trait_major_heart;
texture *textures::trait_major_heart_full;
texture *textures::trait_major_hourglass;
texture *textures::trait_major_hourglass_full;

texture::texture(const char *path)
{
    sdl_texture = IMG_LoadTexture(engine::get().renderer, path);
    if (sdl_texture == nullptr)
        throw std::invalid_argument(SDL_GetError());

    SDL_QueryTexture(sdl_texture, NULL, NULL, &w, &h);
}

void texture::draw(SDL_Renderer *renderer, SDL_Point &p, const SDL_Rect *src)
{
    SDL_Rect dst = { p.x, p.y, w, h };
    if (src != nullptr) {
        dst.w = src->w;
        dst.h = src->h;
    }
    SDL_RenderCopy(renderer, sdl_texture, src, &dst);
}

void textures::init()
{
    textures::crosshair = new texture("resources/crosshair.png");

    textures::trait_major_heart             = new texture("resources/traits/major/heart.png");
    textures::trait_major_heart_full        = new texture("resources/traits/major/heart_full.png");
    textures::trait_major_hourglass         = new texture("resources/traits/major/hourglass.png");
    textures::trait_major_hourglass_full    = new texture("resources/traits/major/hourglass_full.png");
}
