#ifndef AI_H_
#define AI_H_

#include <random>
#include "level.h"
#include "tickable.h"

struct idle;
struct wander;
struct go_to;

struct ai_state {
    virtual void enter(entity &target);
    virtual void exit(entity &target);
    virtual void tick(entity &target, double dt) = 0;

    static idle IDLE;
    static wander WANDER;
    static go_to GOTO;
};

struct idle : ai_state {
    void tick(entity &target, double dt) override;
};

struct wander : ai_state {
    wander();
    void tick(entity &target, double dt) override;
    void enter(entity &target) override;

    std::random_device rand;
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;
};

struct go_to : ai_state {
    void tick(entity &target, double dt) override;
};

#endif /* !AI_H_ */
