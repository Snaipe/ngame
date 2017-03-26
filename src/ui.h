#ifndef UI_H_
#define UI_H_

#include <memory>
#include <vector>

#include "drawable.h"
#include "event.h"
#include "tickable.h"

namespace ui {

class ui_element
    : public drawable
    , public tickable
    , public SDL_Rect
{
public:
    ui_element(int x, int y, int w, int h);
    inline virtual ~ui_element() {}
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;
    void add(ui_element *elt);
    virtual void close();
    ui_element* parent;

private:
    std::vector<ui_element *> elements;
};

class ui : public ui_element {
public:
    ui();
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;
    void close() override;
};

class group_picker : public ui_element {
public:
    group_picker();
    void draw(SDL_Renderer *renderer) override;

    static bool handle_mouse(event::mouse_event &ev);
};

class colorpick : public ui_element {
public:
    colorpick(int x, int y, int w = 150, int h = 150);
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;
private:
    SDL_Texture *texture;
};

class stat_panel : public ui_element {
public:
    stat_panel();
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;
private:
    double transvelocity;
};

}

#endif /* !UI_H_ */
