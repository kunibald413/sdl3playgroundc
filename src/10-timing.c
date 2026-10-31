
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>

const int screenW = 640;
const int screenH = 480;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;


// Texture

bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Best Window", screenW, screenH, 0, &gWindow, &gRenderer);

    return TTF_Init();
}

int main(int argc, char* argv[]) {
    if (!Init()) {
        SDL_Log("Failed to init! SDL error: %s", SDL_GetError());
        return 1;
    }

    gFont = TTF_OpenFont("assets/lazy.ttf", 25);
    if (gFont == NULL) {
        SDL_Log("Failed to load font! err: %s", SDL_GetError());
        return 2;
    }

    SDL_Event e;
    SDL_zero(e);

    char time_text[256];
    Uint32 start_time = 0;

    bool is_running = true;
    while (is_running) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x88, 0x99, 0x44, 0xFF);
        SDL_RenderClear(gRenderer);

        snprintf(time_text, sizeof(time_text), 
            "Hello, millis since start: %u", 
            SDL_GetTicks() - start_time);

        SDL_Color tcolor = {0x00, 0x00, 0x00, 0xFF};
        // surface and texture allocs every frame
        SDL_Surface* surface = TTF_RenderText_Blended(gFont, time_text, 0, tcolor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
        if (texture != NULL) {
            SDL_FRect dst_rect = {.x = 50, .y = 50, .w = surface->w, .h = surface->h};
            SDL_RenderTexture(gRenderer, texture, NULL, &dst_rect);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);

        SDL_RenderPresent(gRenderer);

        SDL_Delay(1000 / 60);
    }

    return 0;
}