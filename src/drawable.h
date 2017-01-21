#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <SDL2/SDL_render.h>

class drawable {
public:
    virtual void draw(SDL_Renderer *renderer) = 0;
};

#endif /* !DRAWABLE_H_ */
