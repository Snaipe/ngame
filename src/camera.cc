#include "camera.h"

void camera::set(const std::complex<double> &pos)
{
    using std::min;
    using std::max;

    if (clip) {
        x_ = min(max(int(pos.real()), boundaries.x), boundaries.x + boundaries.w);
        y_ = min(max(int(pos.imag()), boundaries.y), boundaries.y + boundaries.h);
    } else {
        x_ = int(pos.real());
        y_ = int(pos.imag());
    }
}

void camera::tick(double dt)
{
}

double camera::x()
{
    return x_;
}

double camera::y()
{
    return y_;
}

SDL_Point camera::coord_to_pixel(const std::complex<double> &pos)
{
    return { int((pos.real() - x_) * zoom), int((pos.imag() - y_) * zoom) };
}

std::complex<double> camera::pixel_to_coord(const SDL_Point &pos)
{
    return std::complex<double>((pos.x + x_) / zoom, (pos.y + y_) / zoom);
}
