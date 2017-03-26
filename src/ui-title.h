#ifndef UI_TITLE_H_
#define UI_TITLE_H_

#include <memory>
#include "ui.h"
#include "textures.h"

namespace ui {

class title : public ui_element
{
public:
    title();
    void draw(SDL_Renderer *renderer) override;
private:
    std::unique_ptr<texture> title_text;
};

}

#endif /* !UI_TITLE_H_ */
