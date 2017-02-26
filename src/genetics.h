#ifndef GENETICS_H_
#define GENETICS_H_

#include <complex>
#include <memory>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL_render.h>

#include "ai.h"
#include "drawable.h"
#include "metaball.h"
#include "tickable.h"

struct gene {
    gene(const char *key, double val, double weight);

    std::unordered_map<std::string, double> attr;
    double weight;
};

using allele = std::array<gene, 2>;
using genome = std::vector<allele>;

struct entity_attr {
    constexpr entity_attr()
        : speed(1000)
        , maturation_speed(0.2)
        , max_age(60)
    {}

    entity_attr(genome &g);

    double speed;
    double maturation_speed;
    double max_age;
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
    entity(std::complex<double> pos, genome g);

    void draw(SDL_Renderer *renderer) override;
    void tick(double dt) override;

    std::complex<double> pos();
    SDL_Color color;

    group_type *group;

    std::complex<double> velocity;
    std::complex<double> goal;
    double maturity;
    double age;

    genome genes;
    entity_attr attr;
    state_machine ai;

private:
    std::queue<std::complex<double>> lastpos;
    struct metaballs mb;
};

class population
    : public tickable
    , public drawable
{
public:
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;

    std::vector<group_type> groups;
    std::unordered_set<std::shared_ptr<entity>> entities;

private:
    void breed(entity &father, entity &mother);
};

#endif /* !GENETICS_H_ */
