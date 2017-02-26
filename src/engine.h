#ifndef ENGINE_H_
#define ENGINE_H_

#include "camera.h"
#include "event.h"
#include "level.h"
#include "ui.h"

struct engine {
    engine();
    ~engine();

    static engine &get();

    double framerate;
    int screen_width;
    int screen_height;
    bool paused;

    class level level;
    class camera camera;
    class ui ui;
    class event::manager event_manager;

    SDL_Window *window;
    SDL_Renderer *renderer;

#ifndef NDEBUG
    static bool debug;
#endif

private:
    void tick(double dt);
    void start();

    SDL_Texture *draw_target;

    friend int main(int, char **);
};

#endif /* !ENGINE_H_ */
