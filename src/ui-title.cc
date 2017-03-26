#include "engine.h"
#include "ui-title.h"

ui::title::title()
    : ui_element(0, 0, 0, 0)
    , title_text(std::make_unique<texture>("resources/title.png"))
{
}

void ui::title::draw(SDL_Renderer *renderer)
{
    float scale = 0.45;
    SDL_Point at = {
        int((engine::get().screen_width  - title_text->w * scale) / 2.f),
        int((engine::get().screen_height - title_text->h * scale) / 2.f) - 100
    };
    title_text->draw(renderer, at, nullptr, scale);
}
