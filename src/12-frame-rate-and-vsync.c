#include <stdio.h>
#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

const int screenW = 640;
const int screenH = 480;
const int FPS = 60;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

typedef struct Timer {
    Uint64 StartTicks;
    Uint64 PauseTicks;
    bool IsStarted;
    bool IsPaused;
} Timer;

Timer Timer_New() {
    Timer t = {0};
    return t;
}

Uint64 Timer_GetTicksNS(Timer* t) {
    if (t->IsStarted) {
        return t->IsPaused ? t->PauseTicks : SDL_GetTicksNS() - t->StartTicks;
    }
    return 0;
}

void Timer_Start(Timer* t) {
    t->StartTicks = SDL_GetTicksNS();
    t->PauseTicks = 0;
    t->IsStarted = true;
    t->IsPaused = false;
}

void Timer_Stop(Timer* t) {
    t->IsPaused=false;
    t->IsStarted=false;
    t->StartTicks=0;
    t->PauseTicks=0;
}

void Timer_Pause(Timer* t) {
    if (t->IsStarted && !t->IsPaused) {
        t->PauseTicks = SDL_GetTicksNS() - t->StartTicks;
        t->StartTicks = 0;
        t->IsPaused = true;
    }
}

void Timer_Unpause(Timer* t) {
    if (t->IsStarted && t->IsPaused) {
        t->IsPaused = false;
        t->StartTicks = SDL_GetTicksNS() - t->PauseTicks;
        t->PauseTicks = 0;
    }
}


bool Init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Best Window", screenW, screenH, 0, &gWindow, &gRenderer);
    return TTF_Init();
}

bool LoadFont() {
    gFont = TTF_OpenFont("assets/lazy.ttf", 28);
    return gFont != NULL;
}

void Close() {

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    TTF_CloseFont(gFont);
    gFont = NULL;

    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    if (!Init()) {
        SDL_Log("faiiled to init! %s", SDL_GetError());
        return 1;
    }
    
    if (!LoadFont()) {
        SDL_Log("faiiled to load font! %s", SDL_GetError());
        return 1;
    }


    bool vsync_enabled = true;
    bool fps_capp_enabled = !vsync_enabled;
    Uint64 rendering_ns = 0;
    const Uint64 ns_per_frame = 1000000000 / FPS;
    Timer timer = Timer_New();
    char text_buf[256];

    bool is_running = true;
    SDL_Event e;
    SDL_zero(e);

    while(is_running) {

        Timer_Start(&timer);

        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                is_running = false;
            } else if (e.type == SDL_EVENT_KEY_DOWN) {
                switch(e.key.key) {
                    case SDLK_RETURN: {
                        vsync_enabled = !vsync_enabled;
                        SDL_SetRenderVSync(gRenderer, vsync_enabled ? 1 : SDL_RENDERER_VSYNC_DISABLED);
                    } break;
                    case SDLK_SPACE: {
                        fps_capp_enabled = !fps_capp_enabled;
                    } break;
                }
            }
        }

        SDL_Texture* tex = NULL;
        SDL_FRect dest_rect = {0};
        if (rendering_ns > 0) {
            double frames_per_sec = 1000000000.0 / (double)rendering_ns;
            snprintf(text_buf, sizeof(text_buf), 
                "VSYNC: %d FPSCAP:%d FPS: %.1f", 
                vsync_enabled, fps_capp_enabled, frames_per_sec);

            SDL_Color text_color = {0x88,0x88,0x99, 0xFF};
            SDL_Surface* surface = TTF_RenderText_Blended(gFont, text_buf, 0, text_color);
            tex = SDL_CreateTextureFromSurface(gRenderer, surface);
            dest_rect.h = surface->h;
            dest_rect.w = surface->w;
            dest_rect.x = (screenW - dest_rect.w) / 2;
            dest_rect.y = (screenH - dest_rect.h) / 2;

            SDL_DestroySurface(surface);
        }
        
        SDL_SetRenderDrawColor(gRenderer, 0x22, 0x12,0x12, 0xff);
        SDL_RenderClear(gRenderer);

        if (tex != NULL) {
            SDL_RenderTexture(gRenderer, tex, NULL, &dest_rect);
            SDL_DestroyTexture(tex);
        }
        
        SDL_RenderPresent(gRenderer);

        // get time need to render frame
        rendering_ns = Timer_GetTicksNS(&timer);

        if (fps_capp_enabled && rendering_ns < ns_per_frame) {
            Uint64 sleep_time = ns_per_frame - rendering_ns;
            SDL_DelayNS(sleep_time);
            rendering_ns = Timer_GetTicksNS(&timer);
        }
    }

    Close();

    printf("Done!\n");
    return 0;
}