#ifndef CAMERA_H_
#define CAMERA_H_

#include <complex>
#include <SDL2/SDL_rect.h>

class camera {
public:
    constexpr camera()
        : clip(false)
        , zoom(1)
        , boundaries({0, 0, 0, 0})
        , x_(0)
        , y_(0)
    {}

    bool clip;
    double zoom;
    SDL_Rect boundaries;

    double x();
    double y();

    SDL_Point coord_to_pixel(const std::complex<double> &pos);
    std::complex<double> pixel_to_coord(const SDL_Point &pos);

    void set(const std::complex<double> &pos);
    void tick(double dt);

private:
    double x_;
    double y_;
};

#endif /* !CAMERA_H_ */
