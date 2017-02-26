#ifndef TEXTURES_H_
#define TEXTURES_H_

#include <SDL2/SDL_render.h>

struct texture {
    SDL_Texture *sdl_texture;
    int w, h;

    texture(const char *path);
    void draw(SDL_Renderer *renderer, SDL_Point &p, const SDL_Rect *src = nullptr);
};

struct textures {
    static texture *crosshair;

    static texture *trait_major_heart;
    static texture *trait_major_heart_full;

    static texture *trait_major_hourglass;
    static texture *trait_major_hourglass_full;

    static void init();
};

#endif /* !TEXTURES_H_ */
