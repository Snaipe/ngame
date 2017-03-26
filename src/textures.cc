#include <SDL2/SDL_image.h>

#include "textures.h"
#include "engine.h"

texture *textures::crosshair;
texture *textures::tuto_click;
texture *textures::tuto_select;
texture *textures::tuto_drag;

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

void texture::draw(SDL_Renderer *renderer, SDL_Point &p,
        const SDL_Rect *src, float scale, const SDL_Color *color)
{
    SDL_Rect dst = {
        p.x,
        p.y,
        int(w * scale),
        int(h * scale)
    };
    if (src != nullptr) {
        dst.w = int(src->w * scale);
        dst.h = int(src->h * scale);
    }
    SDL_Color c;
    if (color) {
        SDL_GetTextureColorMod(sdl_texture, &c.r, &c.g, &c.b);
        SDL_GetTextureAlphaMod(sdl_texture, &c.a);
        SDL_SetTextureColorMod(sdl_texture, color->r, color->g, color->b);
        SDL_SetTextureAlphaMod(sdl_texture, color->a);
    }
    SDL_RenderCopy(renderer, sdl_texture, src, &dst);
    if (color) {
        SDL_SetTextureColorMod(sdl_texture, c.r, c.g, c.b);
        SDL_SetTextureAlphaMod(sdl_texture, c.a);
    }
}

void textures::init()
{
    textures::crosshair = new texture("resources/crosshair.png");
    textures::tuto_click = new texture("resources/tutorial/click.png");
    textures::tuto_select = new texture("resources/tutorial/select.png");
    textures::tuto_drag = new texture("resources/tutorial/drag.png");

    textures::trait_major_heart             = new texture("resources/traits/major/heart.png");
    textures::trait_major_heart_full        = new texture("resources/traits/major/heart_full.png");
    textures::trait_major_hourglass         = new texture("resources/traits/major/hourglass.png");
    textures::trait_major_hourglass_full    = new texture("resources/traits/major/hourglass_full.png");
}
