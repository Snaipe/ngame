#include <algorithm>
#include <cmath>

#include "colors.h"

namespace colors {

void hsv_to_rgb(double h, double s, double v, int &r, int &g, int &b) {
    double chroma = v * s;

    h = std::fmod(h / 60.0, 6);
    double fX = chroma * (1 - std::abs(std::fmod(h, 2) - 1));
    double fM = v - chroma;

    double fR = 0, fG = 0, fB = 0;
    if( 0 <= h && h < 1) {
        fR = chroma;
        fG = fX;
    } else if (1 <= h && h < 2) {
        fR = fX;
        fG = chroma;
    } else if (2 <= h && h < 3) {
        fG = chroma;
        fB = fX;
    } else if (3 <= h && h < 4) {
        fG = fX;
        fB = chroma;
    } else if (4 <= h && h < 5) {
        fR = fX;
        fB = chroma;
    } else if (5 <= h && h < 6) {
        fR = chroma;
        fB = fX;
    }

    r = int((fR + fM) * 255);
    g = int((fG + fM) * 255);
    b = int((fB + fM) * 255);
}

void rgb_to_hsv(int r, int g, int b, double& h, double& s, double& v) {
    double fR = r / 255.;
    double fG = g / 255.;
    double fB = b / 255.;
    double cmax = std::max(std::max(fR, fG), fB);
    double cmin = std::min(std::min(fR, fG), fB);
    double delta = cmax - cmin;

    h = 0;
    s = 0;
    if (delta > 0) {
        if(cmax == fR) {
            h = 60 * (std::fmod(((fG - fB) / delta), 6));
        } else if(cmax == fG) {
            h = 60 * (((fB - fR) / delta) + 2);
        } else if(cmax == fB) {
            h = 60 * (((fR - fG) / delta) + 4);
        }

        s = cmax > 0 ? delta / cmax : 0;
    }
    v = cmax;
    h = std::fmod(h + 360, 360);
}

SDL_Color darken(const SDL_Color &color, double factor)
{
    int r, g, b;
    double h, s, v;
    rgb_to_hsv(color.r, color.g, color.b, h, s, v);
    v /= (1 + factor);
    hsv_to_rgb(h, s, v, r, g, b);
    return { Uint8(r), Uint8(g), Uint8(b), Uint8(color.a) };
}

SDL_Color lighten(const SDL_Color &color, double factor)
{
    int r, g, b;
    double h, s, v;
    rgb_to_hsv(color.r, color.g, color.b, h, s, v);
    v *= (1 + factor);
    hsv_to_rgb(h, s, v, r, g, b);
    return { Uint8(r), Uint8(g), Uint8(b), Uint8(color.a) };
}

}
