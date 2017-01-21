#ifndef METABALL_H_
#define METABALL_H_

#include <complex>
#include <vector>

#include <SDL2/SDL_render.h>

struct metaball : public std::complex<double> {
    constexpr metaball(const std::complex<double> &pos, double sz)
        : std::complex<double>(pos)
        , size(sz)
    {}

    double size;
};

class metaballs {
public:
    metaballs(double visc, double thresh);

    void draw(SDL_Renderer *renderer);
    void add_ball(metaball &&mb);
    std::vector<metaball> &getballs();
private:
    std::complex<double> tangent(const std::complex<double> &pos);
    std::complex<double> normal(const std::complex<double> &pos);
    double correct_edge(std::complex<double> &pos);
    std::complex<double> find_border(SDL_Renderer *renderer, std::complex<double> pos);
    void recompute_size();

    std::vector<metaball> balls;
    double viscosity;
    double threshold;
    double minSz;
};

#endif /* !METABALL_H_ */
