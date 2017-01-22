#ifndef UI_H_
#define UI_H_

#include <vector>

#include "drawable.h"
#include "tickable.h"

class ui
    : public drawable
    , public tickable
{
public:
    void tick(double dt) override;
    void draw(SDL_Renderer *renderer) override;

private:
    std::vector<drawable *> elements;
};

#endif /* !UI_H_ */
