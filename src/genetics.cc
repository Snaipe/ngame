#include <algorithm>
#include <iostream>

#include "engine.h"
#include "genetics.h"

#define GENES_PER_ENTITY 10

gene::gene(const char *key, double val, double w)
    : weight(w)
{
    attr[key] = val;
}

group_type::group_type(SDL_Color c)
    : color(c)
{}

void group_type::add(std::shared_ptr<entity> e)
{
    members.insert(e);
    e->color = color;
    if (e->group)
        e->group->remove(e);
    e->group = this;
}

void group_type::remove(std::shared_ptr<entity> e)
{
    members.erase(e);
    e->group = nullptr;
}

static gene random_gene()
{
    static const char *stats[] = {
        "speed",
        "maturation_speed",
        "max_age",
    };

    static std::random_device rand;
    static std::mt19937 mt(rand());
    static std::uniform_int_distribution<int> dist(0, sizeof (stats) / sizeof (stats[0]) - 1);
    static std::uniform_real_distribution<double> dist_val(0.9, 1.1);
    static std::uniform_real_distribution<double> dist_weight(0, 1);

    return gene(stats[dist(mt)], dist_val(mt), dist_weight(mt));
}

static genome random_genome(size_t nb_genes)
{
    genome g;
    for (size_t i = 0; i < nb_genes; ++i) {
        g.emplace_back(std::array<gene, 2>{ random_gene(), random_gene() });
    }
    return g;
}

entity::entity(std::complex<double> pos)
    : entity(pos, random_genome(GENES_PER_ENTITY))
{}

entity::entity(std::complex<double> pos, genome g)
    : group(nullptr)
    , genes(g)
    , attr(g)
    , ai()
    , mb(2, 0.00004, metaball(pos, 2), metaball(pos, 1.5))
{
    //ai.push(*this, ai_state::FIND_FOOD);
}

void entity::tick(double dt)
{
    ai.tick(*this, dt);
    mb.tick(dt);

    //maturity += attr.maturation_speed * dt;
    age += dt;

    std::complex<double> p = pos();

    // dp / dt = v -> p(n+1) - p(n) = v * dt
    p += velocity * dt;

    mb.getballs()[0].real(p.real());
    mb.getballs()[0].imag(p.imag());

    for (auto &m : mb.getballs()) {
        if (m == p)
            continue;
        double dist = std::abs(m - p);
        double DRAG_THRESHOLD = 0;
        if (dist > DRAG_THRESHOLD) {
            // TODO: blobby force stuff
            //std::complex<double> force = (p - m) * 100.;
            //std::cout << force << std::endl;
            //m.vel += force * dt;
            m.vel = (p - m) * 5.;
            std::complex<double> mp = m + m.vel * dt;
            m.real(mp.real());
            m.imag(mp.imag());
        }
    }
}

std::complex<double> entity::pos()
{
    return mb.getballs()[0];
}

void entity::draw(SDL_Renderer *renderer)
{
    if (age > 0) {
        color.a = std::round(255 * (attr.max_age - age) / attr.max_age);
    } else {
        color.a = 255;
    }
    mb.draw(renderer, color);
}

void population::tick(double dt)
{
    std::vector<std::shared_ptr<entity>> ready_to_fuck;

    for (auto i = entities.begin(); i != entities.end(); ) {
        auto &ent = *i;
        ent->tick(dt);

        if (ent->maturity > 1)
            ready_to_fuck.push_back(ent);

        if (ent->age > ent->attr.max_age)
            i = entities.erase(i);
        else
            ++i;
    }

    auto it = ready_to_fuck.begin();
    while (it != ready_to_fuck.end()) {
        auto father = *it++;
        if (it == ready_to_fuck.end())
            break;
        auto mother = *it++;
        breed(*father, *mother);
        father->maturity = 0;
        mother->maturity = 0;
    }
}

void population::draw(SDL_Renderer *renderer)
{
    for (auto &e : entities)
        e->draw(renderer);
}

void population::breed(entity &father, entity &mother)
{
    static std::random_device rand;
    static std::mt19937 mt(rand());
    static std::uniform_int_distribution<int> dist(0, 1);

    int father_pivot = dist(mt);
    int mother_pivot = dist(mt);

    size_t nb_genes = std::min(father.genes.size(), mother.genes.size());

    genome child_genome;

    for (size_t i = 0; i < nb_genes; ++i) {
        auto &fg = father.genes[i][father_pivot];
        auto &mg = father.genes[i][mother_pivot];

        std::array<gene, 2> gene { fg, mg };
        child_genome.emplace_back(gene);
    }

    auto child = std::make_shared<entity>((father.pos() + mother.pos()) / 2.,
            child_genome);

    entities.insert(child);
    father.group->add(child);
}

static double get_mult(const char *name, genome &g)
{
    double mult = 1;
    for (auto &allele : g) {
        auto &gene = allele[0].weight > allele[1].weight
                ? allele[0]
                : allele[1];
        auto it = gene.attr.find(name);
        if (it != gene.attr.end())
            mult *= it->second;
    }
    return mult;
}

entity_attr::entity_attr(genome &g)
    : entity_attr()
{
    speed               *= get_mult("speed", g);
    maturation_speed    *= get_mult("maturation_speed", g);
    max_age             *= get_mult("max_age", g);
}
