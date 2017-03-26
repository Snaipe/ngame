#ifndef TEXTURES_H_
#define TEXTURES_H_

#include <SDL2/SDL_render.h>

#include "colors.h"

struct texture {
    SDL_Texture *sdl_texture;
    int w, h;

    texture(const char *path);
    void draw(SDL_Renderer *renderer, SDL_Point &p, const SDL_Rect *src = nullptr, float scale = 1, const SDL_Color *color = nullptr);
};

struct textures {
    static texture *crosshair;

    static texture *trait_major_heart;
    static texture *trait_major_heart_full;

    static texture *trait_major_hourglass;
    static texture *trait_major_hourglass_full;

    static texture *tuto_click;
    static texture *tuto_select;
    static texture *tuto_drag;

    static void init();
};

#endif /* !TEXTURES_H_ */
