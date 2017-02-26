#ifndef COLORS_H_
#define COLORS_H_

#include <SDL2/SDL_pixels.h>

namespace colors {

    const SDL_Color black = { 0, 0, 0, 255 };
    const SDL_Color white = { 255, 255, 255, 255 };

    void hsv_to_rgb(double h, double s, double v, int &r, int &g, int &b);
    void rgb_to_hsv(double h, double s, double v, int &r, int &g, int &b);

    SDL_Color darken(const SDL_Color &color, double factor);
    SDL_Color lighten(const SDL_Color &color, double factor);

};

#endif /* !COLORS_H_ */
