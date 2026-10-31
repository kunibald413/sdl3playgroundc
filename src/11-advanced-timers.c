#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>


const int screenW = 640;
const int screenH = 480;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

typedef struct Timer {
    Uint64 StartTicks;
    Uint64 PausedTicks;
    bool IsPaused;
    bool IsStarted;
} Timer;

void Timer_Stop(Timer* t) {
    t->IsStarted=false;
    t->IsPaused=false;
    t->PausedTicks = 0;
    t->StartTicks = 0;
}
void Timer_Start(Timer* t) {
    t->IsStarted = true;
    t->IsPaused = false;
    t->StartTicks = SDL_GetTicksNS();
    t->PausedTicks = 0;
}
void Timer_Pause(Timer* t) {
    if (t->IsStarted && !t->IsPaused) {
        t->IsPaused=true;
        t->PausedTicks = SDL_GetTicksNS() - t->StartTicks;
        t->StartTicks = 0;
    }
}
void Timer_Unpause(Timer* t) {
    if (t->IsStarted && t->IsPaused) {
        t->IsPaused=false;
        t->StartTicks = SDL_GetTicksNS() - t->PausedTicks;
        t->PausedTicks = 0;
    }
}
Uint64 Timer_GetTicks(Timer* t) {
    if (t->IsStarted) {
        return t->IsPaused ? t->PausedTicks : SDL_GetTicksNS() - t->StartTicks;
    }
    return 0;
}

bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("best", screenW, screenH, 0, &gWindow, &gRenderer);
    return TTF_Init();
}

void Close() {
    TTF_CloseFont(gFont);
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;

    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!Init()) {
        SDL_Log("Failed to init! err: %s", SDL_GetError());
        return 1;
    }

    gFont = TTF_OpenFont("assets/lazy.ttf", 35);

    char text[256];
    SDL_Color text_color = {0xAA,0x88, 0x88, 0xFF};
    snprintf(text, sizeof(text), "We started this shit!");
    Timer timer = {0};
    Timer_Start(&timer);

    SDL_Event e;
    SDL_zero(e);
    bool is_runninig = true;
    while(is_runninig) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_runninig = false;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_SPACE) {
                    if (timer.IsStarted) {
                        if (timer.IsPaused) Timer_Unpause(&timer);
                        else Timer_Pause(&timer);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 0x33, 0x33, 0x33, 0xff);
        SDL_RenderClear(gRenderer);

        snprintf(text, sizeof(text), "Running for %llu ms", Timer_GetTicks(&timer) / 1000000);
        SDL_Surface* surface = TTF_RenderText_Blended(gFont, text, 0, text_color);
        if (surface == NULL) {
            SDL_Log("Failed to load surface! err: %s", SDL_GetError());
            return 1;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(gRenderer, surface);
        SDL_FRect dst_rect = {
            .x = (screenW - surface->w) / 2, .y = (screenH  - surface->h) / 2, 
            .w = surface->w, .h = surface->h,
        };
        SDL_RenderTexture(gRenderer, tex, NULL, &dst_rect);
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(tex);

        SDL_RenderPresent(gRenderer);
    }
    printf("done!\n");

    Close();
    return 0;
}