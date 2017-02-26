#ifndef LEVEL_HH_
#define LEVEL_HH_

#include <complex>
#include <memory>
#include <unordered_set>
#include <vector>

#include <SDL2/SDL_render.h>

#include "drawable.h"
#include "genetics.h"
#include "tickable.h"

struct bgelement
    : public virtual drawable
{
    bgelement(SDL_Texture *tex, std::complex<double> pos);
    void draw(SDL_Renderer *renderer) override;

    std::complex<double> pos;
    SDL_Texture *tex;
};

class level
    : public virtual drawable
    , public virtual tickable
{
public:
    level();

    void draw(SDL_Renderer *renderer) override;
    void tick(double dt) override;
    void add_entity(const std::shared_ptr<entity> &e);
    void add_entity(std::shared_ptr<entity> &&e);
    void add_bgelement(bgelement e);
    void add_group(SDL_Color c);

    void select(const std::shared_ptr<entity> &e);
    void deselect_all();

    Uint32 laststate;

    SDL_Color select_area_color;
    SDL_Rect select_area;
    std::unordered_set<std::shared_ptr<entity>> selected;

    SDL_Rect create_area;

    population pop;

private:
    // TODO: octrees if too slow to process
    std::vector<bgelement> bgelements;
};

#endif /* !LEVEL_HH_ */
