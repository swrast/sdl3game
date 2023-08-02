#include "SDL_error.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>

#include <entt/entt.hpp>
#include <iostream>

struct MyException : public std::exception {
    std::string s;
    MyException(std::string ss) : s(ss) {}
    ~MyException() throw() {}
    const char *what() const throw() { return s.c_str(); }
};

int main() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("HI");

        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("sdl3game", 640, 480, 0);
    if (!window) {
        return 1;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        return 1;
    }

    // SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // if (renderer == NULL) {
    //     std::cout << SDL_GetError() << std::endl;
    //     std::cout << "trying nonaccelerated backend" << std::endl;
    //
    //     renderer = SDL_CreateRenderer(window, NULL,
    //     SDL_RENDERER_PRESENTVSYNC);
    // }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX,
                                     SDL_SCALEMODE_LINEAR);

    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);
    std::cout << "current renderer: " << rendererInfo.name << std::endl;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    entt::registry registry;
    auto putin = registry.create();
    registry.emplace<SDL_Surface *>(putin, IMG_Load("assets/putin.png"));
    if (!registry.get<SDL_Surface *>(putin))
        throw MyException(SDL_GetError());
    registry.emplace<SDL_Texture *>(
        putin, SDL_CreateTextureFromSurface(
                   renderer, registry.get<SDL_Surface *>(putin)));
    if (!registry.get<SDL_Texture *>(putin))
        throw MyException(SDL_GetError());

    bool running = true;

    SDL_FRect r;
    SDL_FRect t;
    t.x = 0;
    t.y = 0;

    r.x = 50;
    r.y = 50;
    r.w = registry.get<SDL_Surface *>(putin)->w;
    r.h = registry.get<SDL_Surface *>(putin)->h;

    Uint64 oldTime = SDL_GetTicks();
    SDL_bool isFullscreen = SDL_FALSE;

    while (running) {
        Uint64 newTime = SDL_GetTicks();
        Uint64 delta = newTime - oldTime;

        SDL_Event event;
        if (SDL_PollEvent(&event) > 0) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.keysym.sym) {
                case SDLK_F5:
                    isFullscreen =
                        (isFullscreen == SDL_FALSE) ? SDL_TRUE : SDL_FALSE;
                    SDL_SetWindowFullscreen(window, isFullscreen);
                    break;
                }
                break;
            }
        }

        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        if (keyboardState[SDL_SCANCODE_UP])
            t.y = -0.25 * delta;
        if (keyboardState[SDL_SCANCODE_DOWN])
            t.y = 0.25 * delta;
        if (keyboardState[SDL_SCANCODE_RIGHT])
            t.x = 0.25 * delta;
        if (keyboardState[SDL_SCANCODE_LEFT])
            t.x = -0.25 * delta;

        r.x += t.x;
        r.y += t.y;

        SDL_SetRenderDrawColor(renderer, 0x30, 0x45, 0x36, 0xFF);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        auto txt = registry.get<SDL_Texture *>(putin);
        if (txt == NULL)
            throw MyException(SDL_GetError());

        // std::cout << r.x << " " << r.y << " " << r.w << " " << r.h <<
        // std::endl;

        SDL_RenderTexture(renderer, txt, NULL, &r);

        SDL_RenderPresent(renderer);

        oldTime = newTime;
        t.x = 0;
        t.y = 0;
    }

    SDL_DestroyRenderer(renderer);
    // SDL_DestroySurface(screenSurface);
    SDL_DestroySurface(registry.get<SDL_Surface *>(putin));
    SDL_DestroyTexture(registry.get<SDL_Texture *>(putin));
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
