#ifndef METABALL_H_
#define METABALL_H_

#include <array>
#include <complex>
#include <vector>

#include <SDL2/SDL_render.h>

struct metaball : public std::complex<double> {
    constexpr metaball(const std::complex<double> &pos, double sz)
        : std::complex<double>(pos)
        , size(sz)
        , vel(0)
    {}

    double size;
    std::complex<double> vel;
};

class metaballs {
public:
    metaballs(double visc, double thresh);

    template <typename ...Args>
    metaballs(double visc, double thresh, Args ...args)
        : metaballs(visc, thresh)
    {
        std::array<metaball, sizeof...(Args)> balls = {args...};

        for (auto &ball : balls) {
            add_ball(ball);
        }
        recompute_size();
    }

    void tick(double dt);
    void draw(SDL_Renderer *renderer, SDL_Color &color);
    void add_ball(metaball mb);
    std::vector<metaball> &getballs();
private:
    std::complex<double> tangent(const std::complex<double> &pos);
    std::complex<double> normal(const std::complex<double> &pos);
    double correct_edge(std::complex<double> &pos);
    std::complex<double> find_border(std::complex<double> pos);
    void recompute_size();

    std::vector<metaball> balls;
    double viscosity;
    double threshold;
    double minSz;
    double fluct;
};

#endif /* !METABALL_H_ */
