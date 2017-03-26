#include "camera.h"
#include "engine.h"

#define ZOOM_SPEED_FACTOR 5.
#define FOLLOW_SPEED_FACTOR 10.

void camera::set(const std::complex<double> &pos)
{
    x(pos.real());
    y(pos.imag());
}

void camera::tick(double dt)
{
    if (target) {
        std::complex<double> pos { actual.x, actual.y };
        pos += (target->pos() - pos) * FOLLOW_SPEED_FACTOR * dt;
        actual.x = pos.real();
        actual.y = pos.imag();
    }

    actual.zoom += (desired.zoom - actual.zoom) * ZOOM_SPEED_FACTOR * dt;

#ifndef NDEBUG
    if (engine::debug) {
        SDL_SetRenderDrawColor(engine::get().renderer, 0, 255, 255, 255);
        SDL_Point p = engine::get().camera.coord_to_pixel({x(), y()});
        SDL_RenderDrawPoint(engine::get().renderer, p.x, p.y);
    }
#endif
}

double camera::x() const
{
    return actual.x;
}

double camera::y() const
{
    return actual.y;
}

std::complex<double> camera::pos() const
{
    return std::complex<double>(actual.x, actual.y);
}

void camera::x(double val)
{
    using std::min;
    using std::max;

    auto &e = engine::get();

    double lo = pixel_to_x(boundaries.x + e.screen_width / 2);
    double hi = pixel_to_x(boundaries.x + boundaries.w - e.screen_width / 2);
    actual.x = clip ? min(max(val, lo), hi) : val;
    desired.x = val;
    desired.y = actual.y;
}

void camera::y(double val)
{
    using std::min;
    using std::max;

    auto &e = engine::get();

    double lo = pixel_to_x(boundaries.y + e.screen_height / 2);
    double hi = pixel_to_x(boundaries.y + boundaries.h - e.screen_height / 2);
    actual.y = clip ? min(max(val, lo), hi) : val;
    desired.y = val;
    desired.x = actual.x;
}

double camera::zoom() const
{
    return actual.zoom;
}

void camera::zoom(double val)
{
    actual.zoom = val;
    desired.zoom = val;
}

int camera::x_to_pixel(double x) const
{
    auto &e = engine::get();
    return int((x - actual.x) * actual.zoom) + e.screen_width / 2;
}

double camera::pixel_to_x(int x) const
{
    auto &e = engine::get();
    return (x - e.screen_width / 2) / actual.zoom + actual.x;
}

int camera::y_to_pixel(double y) const
{
    auto &e = engine::get();
    return int((y - actual.y) * actual.zoom) + e.screen_height / 2;
}

double camera::pixel_to_y(int y) const
{
    auto &e = engine::get();
    return (y - e.screen_height / 2) / actual.zoom + actual.y;
}

SDL_Point camera::coord_to_pixel(const std::complex<double> &pos) const
{
    return {
        x_to_pixel(pos.real()),
        y_to_pixel(pos.imag())
    };
}

std::complex<double> camera::pixel_to_coord(const SDL_Point &pos) const
{
    return std::complex<double>(pixel_to_x(pos.x), pixel_to_y(pos.y));
}

void camera::zoom_to(double zoom)
{
    if (lock) return;
    desired.zoom = zoom;
}

void camera::zoom_by(double fact)
{
    if (lock) return;
    desired.zoom += desired.zoom * fact;
}

void camera::move_to(std::complex<double> &pos)
{
    if (lock) return;
    desired.x = pos.real();
    desired.y = pos.imag();
}

void camera::follow(std::shared_ptr<entity> &tgt)
{
    target = tgt;
}

void camera::follow(std::shared_ptr<entity> &&tgt)
{
    target = tgt;
}
