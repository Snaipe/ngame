#ifndef ENGINE_H_
#define ENGINE_H_

#include "camera.h"
#include "level.h"

struct engine {
    engine();
    ~engine();

    static engine &get();

    double framerate;
    int screen_width;
    int screen_height;

    class level level;
    class camera camera;

    SDL_Window *window;
    SDL_Renderer *renderer;

private:
    void tick(double dt);
    void start();

    SDL_Texture *draw_target;

    friend int main(int, char **);
};

#endif /* !ENGINE_H_ */
