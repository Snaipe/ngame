#ifndef LEVEL_HH_
#define LEVEL_HH_

#include <complex>
#include <queue>
#include <vector>
#include <unordered_map>

#include <SDL2/SDL_render.h>

#include "drawable.h"
#include "tickable.h"
#include "metaball.h"

class entity
    : public virtual drawable
    , public virtual tickable
{
public:
    entity(std::complex<double> pos);

    void draw(SDL_Renderer *renderer) override;
    void tick(double dt) override;

    const std::complex<double> &pos;
    SDL_Color color;

    std::complex<double> velocity;

private:
    std::queue<std::complex<double>> lastpos;
    struct metaballs mb;
};

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
    void add_entity(entity e);
    void add_bgelement(bgelement e);
private:
    // TODO: octrees if too slow to process
    std::vector<bgelement> bgelements;
    std::vector<entity> entities;
};

#endif /* !LEVEL_HH_ */
