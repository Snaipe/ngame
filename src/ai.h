#ifndef AI_H_
#define AI_H_

#include <stack>
#include <random>
#include "tickable.h"

struct entity;

struct idle;
struct wander;
struct go_to;
struct find_food;

struct ai_state {
    virtual void enter(entity &target);
    virtual void exit(entity &target);
    virtual void tick(entity &target, double dt) = 0;

    static idle IDLE;
    static wander WANDER;
    static go_to GOTO;
    static find_food FIND_FOOD;
};

struct state_machine {
    state_machine()
        : states()
    {}

    void push(entity &ent, ai_state &state);
    void set(entity &ent, ai_state &state);
    void pop(entity &ent);
    void tick(entity &ent, double dt);
private:
    std::stack<ai_state*> states;
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

struct find_food : ai_state {
    void tick(entity &target, double dt) override;
};

#endif /* !AI_H_ */
