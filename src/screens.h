#ifndef SCREENS_H_
#define SCREENS_H_

#include "camera.h"
#include "drawable.h"
#include "event.h"
#include "level.h"
#include "tickable.h"
#include "ui.h"

namespace screen {

class screen : public tickable, public drawable {
public:
    class event::manager event_manager;
    ui::ui ui;

    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;
};

class title : public screen {
public:
    title();
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;

private:
    bool display_click;
    double display_click_fade;
};

class intro : public screen {
public:
    intro();
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;

    enum intro_phase {
        INIT,
        SELECT,
        ZONE,
    };

    intro_phase phase;
    intro_phase next_phase;
private:
    double display_click_fade;
};

class level : public screen {
public:
    level();
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;
};

}

#endif /* !SCREENS_H_ */
