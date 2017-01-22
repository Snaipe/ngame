#ifndef LEVEL_HH_
#define LEVEL_HH_

#include <complex>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <SDL2/SDL_render.h>

#include "ai.h"
#include "drawable.h"
#include "metaball.h"
#include "tickable.h"

struct entity_attr {
    constexpr entity_attr()
        : speed(1000)
    {}

    double speed;
};

class entity;

struct group_type
{
    group_type(SDL_Color c);

    SDL_Color color;
    std::unordered_set<std::shared_ptr<entity>> members;

    void add(std::shared_ptr<entity> e);
    void remove(std::shared_ptr<entity> e);
};

class entity
    : public virtual drawable
    , public virtual tickable
{
public:
    entity(std::complex<double> pos);

    void draw(SDL_Renderer *renderer) override;
    void tick(double dt) override;

    std::complex<double> pos();
    SDL_Color color;

    group_type *group;

    std::complex<double> velocity;
    std::complex<double> goal;
    entity_attr attr;
    state_machine ai;

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
    void add_entity(const std::shared_ptr<entity> &e);
    void add_entity(std::shared_ptr<entity> &&e);
    void add_bgelement(bgelement e);
    void add_group(SDL_Color c);

    void select(const std::shared_ptr<entity> &e);
    void deselect_all();

    SDL_Rect select_area;
    std::unordered_set<std::shared_ptr<entity>> selected;
    std::vector<group_type> groups;
    std::vector<std::shared_ptr<entity>> entities;

private:
    // TODO: octrees if too slow to process
    std::vector<bgelement> bgelements;
};

#endif /* !LEVEL_HH_ */
