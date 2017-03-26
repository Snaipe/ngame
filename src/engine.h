#ifndef ENGINE_H_
#define ENGINE_H_

#include "camera.h"
#include "event.h"
#include "level.h"
#include "screens.h"
#include "ui.h"

class quit {};

struct engine {
    engine();
    ~engine();

    static engine &get();

    double framerate;
    int screen_width;
    int screen_height;
    int fullscreen;
    bool paused;

    class camera camera;
    class level level;
    screen::screen *screen;
    screen::screen *next_screen;
    double screen_transition;

    SDL_Window *window;
    SDL_Renderer *renderer;

#ifndef NDEBUG
    static bool debug;
#endif

private:
    void tick(double dt);
    void start();

    friend int main(int, char **);
};

#endif /* !ENGINE_H_ */
