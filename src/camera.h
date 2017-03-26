#ifndef CAMERA_H_
#define CAMERA_H_

#include <complex>
#include <memory>
#include <SDL2/SDL_rect.h>

#include "genetics.h"

struct camerapos {
    constexpr camerapos() : x(0), y(0), zoom(1) {}

    double x;
    double y;
    double zoom;
};

class camera {
public:
    constexpr camera()
        : clip(false)
        , lock(false)
        , boundaries({0, 0, 0, 0})
        , actual()
        , desired()
        , target(nullptr)
    {}

    bool clip;
    bool lock;
    SDL_Rect boundaries;

    double x() const;
    void x(double val);
    double y() const;
    void y(double val);
    std::complex<double> pos() const;
    double zoom() const;
    void zoom(double val);

    int x_to_pixel(double x) const;
    double pixel_to_x(int x) const;
    int y_to_pixel(double y) const;
    double pixel_to_y(int y) const;
    SDL_Point coord_to_pixel(const std::complex<double> &pos) const;
    std::complex<double> pixel_to_coord(const SDL_Point &pos) const;

    void zoom_to(double zoom);
    void zoom_by(double fact);
    void move_to(std::complex<double> &pos);

    void set(const std::complex<double> &pos);
    void tick(double dt);
    void follow(std::shared_ptr<entity> &tgt);
    void follow(std::shared_ptr<entity> &&tgt);

private:
    camerapos actual;
    camerapos desired;
    std::shared_ptr<entity> target;
};

#endif /* !CAMERA_H_ */
